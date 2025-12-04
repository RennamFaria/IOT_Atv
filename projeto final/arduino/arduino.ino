// Arduino Code

//necessita modificar o código

#include "DHT.h"
#define DHTPIN 5
#include <Wire.h>
#define DHTTYPE DHT11 // DHT11
#define SOUND_SPEED 0.034
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillisSensors = 0;
volatile byte requisicao;
int valorTensaoldr = 0;
float valorADC = 0;

long duration;
double tempC;

void dhtCheck(){ 
  tempC = dht.readTemperature();
  //Serial.print("Temperature: "); 
  //Serial.print(tempC);Serial.println(" *C ");
}

void receberRequisicao(int numBytes) {
  //Serial.println("entrando receberRequisicao");
  if (Wire.available()) {
    //Serial.println("Entrando no if da func");
    requisicao = Wire.read(); 
    if(requisicao == 3) invertLed();
  }
}

void enviarFloat() {
  float valorEnviar;

  //hcsr04();
  //dhtCheck();

  //Serial.println("Entrando no EnviaFloat(0)");
  if (requisicao == 1) valorEnviar = tempC;
  else return; 

  Serial.println(valorEnviar);
  byte *ptr = (byte*)&valorEnviar;
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(ptr[i]);
  }
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  Wire.onReceive(receberRequisicao); 
  Wire.onRequest(enviarFloat);
}

void loop() {
  if(millis() - previousMillisSensors >= 100)
  {
    previousMillisSensors = millis();
    dhtCheck();
  }
}