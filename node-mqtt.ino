/*
 * Basic ESP8266 MQTT
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

// WiFi Parameters & MQTT Server IP Address

SimpleDHT11 dht11;
char ssid[] = "xxx";                                 // WiFi SSID
char password[] = "yyyxxxzzz";                       // WiFi password
char mqtt_server[] = "192.168.1.254";                // mqtt server IP address
long lastMsg = 0;
char msg[50];
int value1 = 0;
int value2 = 0;
int OUT_LED = 5;
int DHTPIN = 16;
byte temp = 0;
byte humid = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
// We start by connecting to a WiFi network
Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
Serial.print("*");
delay(500);
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}

Serial.println();
// Switch on the LED if an 1 was received as first character
if ((char)payload[0] == '1') {
digitalWrite(OUT_LED, HIGH);
// Turn the LED on - Note that LOW is the voltage level
// But actually the LED is on.
// It is acive low on the ESP32.
} else {

digitalWrite(OUT_LED, LOW);
         
}
}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection ...");
// Attempt to connect
if (client.connect("ESP8266Client")) {
Serial.println("connected");
// Once connected, publish an announcement
client.publish("outTopic", "Hello !!!");
// And resubscribe
client.subscribe("inTopic");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying
delay(5000);
}
}
}

void setup() {
pinMode(OUT_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
Serial.begin(115200);
setup_wifi();
client.setServer(mqtt_server, 1883);
client.setCallback(callback);

}

void loop() {
if (WiFi.status() != WL_CONNECTED) {
WiFi.disconnect();
delay(500); 
setup_wifi(); 
}
if (!client.connected()) {
reconnect();
}
client.loop();
long now = millis();
if (now - lastMsg > 2000) {
lastMsg = now;
dht11.read(DHTPIN, &temp, &humid, NULL);
value1 = (int)temp;
value2 = (int)humid;
snprintf (msg, 75, "Temperature is %d and Humidity is %d", value1, value2);
Serial.print("Publish message: ");
Serial.println(msg);
client.publish("outTopic", msg);
}
}
