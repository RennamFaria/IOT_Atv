// esp32

#include <Wire.h>

// C++ code

//dataloger remoter
//googlesheets esp32 -> podemos usar para ser BD

#include <PubSubClient.h>
#include <WiFi.h>
const char *WIFI_SSID = "moto g84 5G_5388";
const char *WIFI_PASSW = "fcnb64axjefe8a9";
const char *MQTT_SERVER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *MQTT_MONITORADOR_SONO_GIRO = "/publishESP32/IoT5/giroscópio";
const char *MQTT_MONITORADOR_SONO_TEMP = "/publishESP32/IoT5/temp";
const char *MQTT_MONITORADOR_SONO_LUZ = "/publishESP32/IoT5/luz";
//const char *MQTT_MONITORADOR_SONO_UMIDADE = "/publishESP32/IoT5/umidade";

unsigned long previousMillisMenu = 0;
unsigned long previuosMillisResponse = 0;
bool grupo2 = false;
char opcao;
const long interval1 = 100;
float valorRecebido;
int opcaotransmitida = 0;

void recebeFloat(){
  Wire.requestFrom(8, 4);
  byte dados[4];
  int i = 0;
  while (Wire.available() && i < 4) {
    dados[i++] = Wire.read();
  }
  memcpy(&valorRecebido, dados, 4);
}

void transmiteOpcao(int opcaotransmitida){
  Wire.beginTransmission(8);
  Wire.write(opcaotransmitida);
  Wire.endTransmission();
  
  previuosMillisResponse = millis();
}

void sendData(int opcao, char *MQTT_TOPIC){
  transmiteOpcao(opcao);
  delay(10);
  recebeFloat();
  
  char msg[20];
  dtostrf(valorRecebido, 1, 3, msg);
  mqttClient.publish(MQTT_TOPIC, msg);
}

void sendAllData(){
  sendData(a, MQTT_MONITORADOR_SONO_GIRO);
  sendData(b, MQTT_MONITORADOR_SONO_TEMP);
  sendData(c, MQTT_MONITORADOR_SONO_LUZ);
  //sendData(d, MQTT_MONITORADOR_SONO_UMIDADE);
}

void ConnectToWiFi(){
  Serial.print("Connecting to WiFi ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSW);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to ");
  Serial.println(WIFI_SSID);
}

void ConnectToMqtt(){
 Serial.println("Connecting to MQTT Broker...");
 while (!mqttClient.connected()){
  char clientId[100] = "\0";
  sprintf(clientId, "ESP32Client-dI8n9P1-", random(0xffff));   //variavel = clientID
  Serial.println(clientId);
  if (mqttClient.connect(clientId)){
    Serial.println("Connected to MQTT broker."); 
		mqttClient.subscribe(MQTT_MONITORADOR_SONO_GIRO);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_TEMP);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_LUZ);
    //mqttClient.subscribe(MQTT_PUBLISH_RESPOSTA_GRUPO1);
  }
 }
}

/*
void CallbackMqtt(char* topic, byte* payload, unsigned int length){
    
    String topicoStr = String(topic);


    //  1 Se for comando ('d','e','f') 
    if (topicoStr == MQTT_PUBLISH_PEDIDO_GRUPO1) {
        
        char comando = (char)payload[0];
        Serial.print("Comando recebido via MQTT: ");
        Serial.println(comando);

        if (comando == 'd') opcaotransmitida = 1;
        else if (comando == 'e') opcaotransmitida = 2;
        else if (comando == 'f') opcaotransmitida = 3;
        else return;

        transmiteOpcao(opcaotransmitida);
        grupo2 = true;
        return;
    }

    //  2 Se for RESPOSTA do outro grupo float 
    if (topicoStr == MQTT_PUBLISH_RESPOSTA_GRUPO1) {

        char buf[20];
        memcpy(buf, payload, length);
        buf[length] = '\0';

        float valor = atof(buf);
        
        if(opcaotransmitida == 4){
          Serial.print("Distância: ");
          Serial.println(valor, 3);
        } else if(opcaotransmitida == 5){
          Serial.print("Tempeatura: ");
          Serial.println(valor, 3);
        }

        return;
    }
}
*/


void SetupMqtt(){
 mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
 //mqttClient.setCallback(CallbackMqtt);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  ConnectToWiFi();
  SetupMqtt();
}

void loop()
{
  if (!mqttClient.connected()){
    ConnectToMqtt();
  }
  mqttClient.loop();
  
  if(millis() - previuosMillisResponse >= 1000)
  {
    sendAllData();

    previuosMillisResponse = millis();
  }
}