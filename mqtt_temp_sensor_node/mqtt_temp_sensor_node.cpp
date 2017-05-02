
// Simple MQTT Client for esp8266 +  DHT11 temperature/humidity sensor

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Setup temperature sensor library
#define DHTTYPE DHT11
#define DHTPIN  2
DHT dht(DHTPIN, DHTTYPE, 11);

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
long crrMsgNo = 0;

// INSERT WIFI CREDENTIALS AND MQTT-SERVER ADDRESS
const char* ssid     	= ""; 	// wifi ssid;
const char* password 	= ""; 		// wifi password;
const char* mqtt_server = "127.0.0.1";	// MQTT broker address
const char* topic_temp 	= "Temperature";

void initWiFi() {
	WiFi.begin(ssid, password);
	Serial.print("\n\r \n\Connecting");

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
}

void publishSensorData() {

	char temp[10];
	dtostrf(dht.readTemperature(false), 4, 0, temp);
    Serial.print("Publish temperature: ");
    Serial.println(temp);
    client.publish(topic_temp, temp);
}

void reconnect() {
  while (!client.connected()) {
	Serial.print("Attempting MQTT connection...");
	String clientId = "ESP8266Client-";
	clientId += String(random(0xffff), HEX);
	if (client.connect(clientId.c_str())) {
	  Serial.println("connected");
	  publishSensorData();
	} else {
	  Serial.print("failed, rc=");
	  Serial.print(client.state());
	  Serial.println(" try again in 5 seconds");
	  delay(5000);
	}
  }
}

void setup() {
	Serial.begin(115200);
	client.setServer(mqtt_server, 1883);
}

void loop() {

	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	if (millis() - lastMsg > 60000) {
		lastMsg = millis();
		++crrMsgNo;
	    publishSensorData();
	}
}
