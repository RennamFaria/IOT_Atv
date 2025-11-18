#include <PubSubClient.h>
#include <WiFi.h>

//trocar topico quando for publicar

const char *WIFI_SSID = "AndroidAP54EA";
//broker publica 
//const char *MQTT_SERVER = "test.mosquitto.org";    // endereço do broker
//broker da flespi
const char *MQTT_SERVER = "mqtt.flespi.io";    // endereço do broker

const uint16_t MQTT_PORT = 1883;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void ConnectToWiFi(){
  Serial.print("Connecting to WiFi ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, "12345678");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to ");
  Serial.println(WIFI_SSID);
}

// Connect to broker
void ConnectToMqtt(){
 Serial.println("Connecting to MQTT Broker...");
 while (!mqttClient.connected()){
  char clientId[100] = "\0";    //nome do meu ID
  sprintf(clientId, "ESP32Client-%04X", random(0xffff));
  Serial.println(clientId);
  if (mqttClient.connect(clientId, "TJfcwD2jYbcUn7sl5UhVg3TcO0wumfSCrt4GG38SI4D3KLo5S76i4TAxRSt9rP3A", "")){
    Serial.println("Connected to MQTT broker."); 
		mqttClient.subscribe("/subscribeESP32/ICT-UNIFESP/IoT");
  }
 }
}

void CallbackMqtt(char* topic, byte* payload, unsigned int length){
   Serial.println("ReceivedMessage!"); 
     if (String(topic) == "/subscribeESP32/ICT-UNIFESP/IoT") {
 Serial.println(payload[0]);
 }
}

void SetupMqtt(){
 mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
 mqttClient.setCallback(CallbackMqtt);
}

void setup(){
  Serial.begin(9600);
  Serial.println("Setup begin");
  ConnectToWiFi();
  SetupMqtt();
  Serial.println("Setup end");
}

void loop(){
   if (!mqttClient.connected()){
      ConnectToMqtt();
   }
   mqttClient.loop();
  
	 delay(500);
   mqttClient.publish("/buh/guh/juh/otag", "1");
}





