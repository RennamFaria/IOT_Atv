
#include <WiFi.h>
#include "ThingSpeak.h" 

char ssid[] = "AndroidAP54EA"; 
char pass[] = "12345678";
int keyIndex = 0;         
WiFiClient  client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "3R6YUFWFOS93I2NW";

int number = 0;

void setup() {
  Serial.begin(115200);  //Initialize serial
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); 
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  int x = ThingSpeak.writeField(myChannelNumber, 1, number, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  number++;
  if(number > 99){
    number = 0;
  }
  
  delay(15000); // Wait 15 seconds to update the channel again (free tier)
}
