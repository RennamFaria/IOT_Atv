// esp32

#include <Wire.h>

#include <WiFiUdp.h>
#include <coap-simple.h>

#include <PubSubClient.h>
#include <WiFi.h>
//const char *WIFI_SSID = "AndroidAP54EA";  
//const char *WIFI_PASSW = "12345678";    
const char *WIFI_SSID = "moto g84 5G_5388";
const char *WIFI_PASSW = "fcnb64axjefe8a9";
WiFiClient wifiClient;

unsigned long previousMillisMenu = 0;
unsigned long previuosMillisResponse = 0;
bool grupo2 = false;
char opcao;
const long interval1 = 100;
float valorRecebido;
int opcaotransmitida = 0;

// --- Coap things ---

WiFiUDP udp;
Coap coap(udp);

// CoAP server endpoint URL
void callback_light(CoapPacket &packet, IPAddress ip, int port) {
  char clientPayload[packet.payloadlen + 1];
  memcpy(clientPayload, packet.payload, packet.payloadlen);
  clientPayload[packet.payloadlen] = NULL;

  String message(clientPayload);
  Serial.println("Received:light");

  switch(packet.code){
      case COAP_GET:  
      //VERIFICAR -> Se da para mandar em float ou se necessita transformar para String
        if(message == 'a'){  //temp
         transmiteOpcao(1);
         recebeFloat();
         coap.sendResponse(ip, port, packet.messageid, valorRecebido);
        } else if(message == 'b'){  //temp
         transmiteOpcao(2);
         recebeFloat();
         coap.sendResponse(ip, port, packet.messageid, valorRecebido);
        } else if(message == 'c'){  //temp
         transmiteOpcao(3);
         recebeFloat();
         coap.sendResponse(ip, port, packet.messageid, valorRecebido);
        }
      break;
      /*
      case COAP_PUT:// acho que não precisa de PUT
          coap.sendResponse(ip, port, packet.messageid, "1");        
      break;
      */
      default:         
          coap.sendResponse(ip, port, packet.messageid, "Not Supported");
      break;
  }
  
}



// --- Others ---

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

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Menu: ");
  Serial.println("a) Distância Grupo 1");
  Serial.println("b) Temperatura Grupo 1");
  Serial.println("c) LED Grupo 1");
  ConnectToWiFi();

  //coap.server = endpoints do server
  coap.server(callback_light, "light");
  coap.start();
}

void loop()
{
  if (Serial.available()) {
    opcao = Serial.read();
    if(opcao == 'a'){
      Serial.println("opcao 1");
      opcaotransmitida = 1;
      transmiteOpcao(opcaotransmitida);
    }else if(opcao == 'b'){
      Serial.println("opcao 2");
      opcaotransmitida = 2;
      transmiteOpcao(opcaotransmitida);
    }else if(opcao == 'c'){
      Serial.println("opcao 3");
      opcaotransmitida = 3;
      transmiteOpcao(opcaotransmitida);
    }
  }
 }
  
  // Adaptar para o Coap
  if((opcaotransmitida == 1 || opcaotransmitida == 2) && millis() - previuosMillisResponse >= 100)
  {
    recebeFloat();
    
      if(opcaotransmitida == 1)
      {
      	Serial.print("Ditância: ");
      	Serial.println(valorRecebido, 3);
        if(grupo2 == true)
        {
          char msg[20];
          dtostrf(valorRecebido, 1, 3, msg);
          mqttClient.publish(MQTT_SUBSCRIBE_RESPOSTA_GRUPO2, msg);
          grupo2 = false;
        }
      }else if(opcaotransmitida == 2)
      {
      	Serial.print("Temperatura: ");
      	Serial.println(valorRecebido, 3);
         if(grupo2 == true)
    	 {
      		char msg[20];
      		dtostrf(valorRecebido, 1, 3, msg);
      		mqttClient.publish(MQTT_SUBSCRIBE_RESPOSTA_GRUPO2, msg);
      		grupo2 = false;
    	 }
       }
    opcaotransmitida = -1;
  }
}