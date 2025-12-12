// Arduino Code

//necessita modificar o código

#include "DHT.h"
#define DHTPIN 5
#include <Wire.h>
#define DHTTYPE DHT11 // DHT11
#define SOUND_SPEED 0.034
DHT dht(DHTPIN, DHTTYPE);

#define ADCPIN A3
#define LDRPIN A2// Pino de leitura do sensor


unsigned long previousMillisSensors = 0;
volatile byte requisicao;
int valorTensaoldr = 0;
float valorADC = 0;
float tempADC = 0;

long duration;
double tempC;

const int MPU = 0x68;


float ldrValor = 0;
float lm35Temp = 0;
float dhtHumi = 0;

int acelX, acelY, acelZ;
int giroX, giroY, giroZ;
float mpuTemp = 0;

void inicializaMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void gisroscopico() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);

  acelX = Wire.read()<<8 | Wire.read();
  acelY = Wire.read()<<8 | Wire.read();
  acelZ = Wire.read()<<8 | Wire.read();
  int tempRaw = Wire.read()<<8 | Wire.read();
  giroX = Wire.read()<<8 | Wire.read();
  giroY = Wire.read()<<8 | Wire.read();
  giroZ = Wire.read()<<8 | Wire.read();

  mpuTemp = tempRaw / 340.0 + 36.53;
  Serial.println("Giro X: ");
  Serial.println(giroX);
  Serial.println("Giro Y: ");
  Serial.println(giroY);
  Serial.println("Giro Z: ");
  Serial.println(giroZ);
}

void ldr(){
  analogReference(DEFAULT);
  delay(5);
  valorTensaoldr = analogRead(LDRPIN);
  ldrValor = valorTensaoldr *(5.0/1023);
  Serial.println(valorTensaoldr);
  Serial.println(ldrValor);
  if(ldrValor < 2.6){
    Serial.println("Baixa luminosidade");
    Serial.println(ldrValor);
  }else{
    Serial.println("Alta luminosidade");
  }
}

int analogAvg(int sensorPin) {
  int amostras = 20;
  long soma = 0;
  for (int i = 0; i < amostras; ++i) {
    soma += analogRead(sensorPin);
    delay(3); // pequeno delay para estabilizar entre amostras (opcional)
  }
  return (int)(soma / amostras);
}

void templm35() {
  analogReference(INTERNAL1V1);
  delay(5);
  int leitura = analogAvg(ADCPIN);
  Serial.println(leitura);
   
  lm35Temp = leitura * 1.1 / 1023.0 * 100.0;
  // float temperatura = (leitura * 1.1 / 1023 - 0.5 )* 100 ; tmp 63

  Serial.print("LM35: ");
  Serial.print(lm35Temp);
  Serial.println(" C");
}

void dhtCheck(){
   dhtHumi = dht.readHumidity();
   delay(3);
   Serial.print("Umidade: ");
   Serial.println(dhtHumi);
}

void receberRequisicao(int numBytes) {
  //Serial.println("entrando receberRequisicao");
  if (Wire.available()) {
    //Serial.println("Entrando no if da func");
    requisicao = Wire.read();
  }
}

void enviarFloat() {
  float valorEnviar;

  //Serial.println("Entrando no EnviaFloat(0)");
  switch (requisicao) {
    case 1: valorEnviar = ldrValor; break;
    case 2: valorEnviar = lm35Temp; break;
    case 3: valorEnviar = dhtHumi; break;
    case 4: valorEnviar = (float)giroX; break;
    case 5: valorEnviar = (float)giroY; break;
    case 6: valorEnviar = (float)giroZ; break;
    default: valorEnviar = 202; // erro
  }

  Serial.println(valorEnviar);
  byte *ptr = (byte*)&valorEnviar;
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(ptr[i]);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(4);
  Wire.onReceive(receberRequisicao);
  Wire.onRequest(enviarFloat);
  analogReference(INTERNAL1V1);
  inicializaMPU();

}

void loop() {
  if(millis() - previousMillisSensors >= 500)
  {
    previousMillisSensors = millis();
    ldr();
    templm35();
    dhtCheck();
    gisroscopico();
  }
}

