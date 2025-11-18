// esp32
// slave

// envia pro master os valores dos sensores
// (o master faz a media e guarda os valores)
// Quando a gente pedir, eles tem que devolver

// a gente envia infos para o master a cada 100ms

//ordem
//Temperatura - a
//humidade - b
//distancia - c


// temperatura -> está recebendo distancia
// Humidade -> está recebendo temperatura
// Distancia -> está recebendo humidade

#include <Wire.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

unsigned long previousMillisMenu = 0;
unsigned long previousMillisResponse = 0;
bool response = false;
char opcao;
const long interval1 = 100;
float valorRecebido;
int opcaotransmitida = -1;

void recebeFloat() {
    //Serial.println("Entrando recebeFloat()");
    //const int floatSize = sizeof(float);
    Wire.requestFrom(8, 4);
    byte dados[4];
    int i = 0;
    while (Wire.available() && i < 4) {
        dados[i++] = Wire.read();
    }
    memcpy(&valorRecebido, dados, 4);
    //Serial.println(valorRecebido);
    previousMillisResponse = millis();
}

void transmiteOpcao(int opcaotransmitida) {
    Wire.beginTransmission(8);
    Wire.write(opcaotransmitida);
    Wire.endTransmission();
}

void setup()
{
    Serial.begin(115200);
    opcaotransmitida = -1;
    Wire.begin();
    SerialBT.begin("ESP32_Time2");
    Serial.println("Menu: ");
    Serial.println("a) Temperatura");
    Serial.println("b) Umidade");
    Serial.println("c) Distância");
}

void loop()
{
    if (SerialBT.available()) {
        opcao = SerialBT.read();
        //Serial.print(opcao);
        if (opcao == 'a') {
            Serial.println("opcao 1");
            opcaotransmitida = 1;
            transmiteOpcao(opcaotransmitida);
        } else if (opcao == 'b') {
            Serial.println("opcao 2");
            opcaotransmitida = 2;
            transmiteOpcao(opcaotransmitida);
        } else if (opcao == 'c') {
            Serial.println("opcao 3");
            opcaotransmitida = 3;
            transmiteOpcao(opcaotransmitida);
        }
    }

    //if ((opcaotransmitida > 0) && (millis() - previousMillisResponse) >= 50) {
    if ((opcaotransmitida > 0)) {
        recebeFloat();

        if (opcaotransmitida == 1) {// 3 -> Envia Distancia
            SerialBT.write((const uint8_t*)&valorRecebido, sizeof(valorRecebido));
            //SerialBT.write("Enviando temperatura");
            //SerialBT.write(1);
            //SerialBT.write(valorRecebido);

            Serial.print("Temperatura: ");
            Serial.println(valorRecebido, 3);
        } else if (opcaotransmitida == 2) {// 1 -> Envia Temperatura
            SerialBT.write((const uint8_t*)&valorRecebido, sizeof(valorRecebido));
            Serial.print("Umidade: ");
            Serial.println(valorRecebido, 3);
        } else if (opcaotransmitida == 3) {// 2 -> Envia Umidade
            SerialBT.write((const uint8_t*)&valorRecebido, sizeof(valorRecebido));

            Serial.print("Ditância: ");
            Serial.println(valorRecebido, 3);
        }

        opcaotransmitida = -1;
    }
}
