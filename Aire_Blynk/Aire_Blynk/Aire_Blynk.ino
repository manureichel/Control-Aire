#define BLYNK_TEMPLATE_ID           "TMPLIdEhJObA"
#define BLYNK_DEVICE_NAME           "New Template"
#define BLYNK_AUTH_TOKEN            "kiotsPbiMooc5pftT19Sua0kAoRYU3U8"

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "CTBot.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Electra.h>

char auth[] = BLYNK_AUTH_TOKEN;

const char* ssid = "Manuel 2.4GHz";
const char* password = "manuelreichel92";

#define DHTPIN D3
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

const uint16_t kIrLed = D2;
IRElectraAc ac(kIrLed);

int tempSet = 21;

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V7, tempSet);
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  if(pinValue == 1) turnOn();
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  if(pinValue == 1) turnOff();
}

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  if(pinValue == 1) increaseTemp();
}

BLYNK_WRITE(V4)
{
  int pinValue = param.asInt();
  if(pinValue == 1) reduceTemp();
}

void setup()
{
  Serial.begin(115200);
  ac.begin(); 
  Blynk.begin(auth, ssid, password);
  dht.begin();
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}

void increaseTemp() {
 if(tempSet != 32)
  tempSet++;
 ac.setTemp(tempSet);
 ac.send();
 Blynk.virtualWrite(V7, tempSet);
}

void reduceTemp() {
  if(tempSet != 16)
    tempSet--;
  ac.setTemp(tempSet);
  ac.send();
  Blynk.virtualWrite(V7, tempSet);
}

void turnOff() {
    ac.off();
    ac.send();
    Serial.println("AC OFF");
}
 
void turnOn() {      
      // Set Mode
      ac.setMode(kElectraAcCool);
      ac.setMode(1);
      ac.setFan(1);
      ac.setSwingH(false);
      ac.setSwingV(false);
      ac.setLightToggle(false);
      ac.setClean(false);
      ac.setTurbo(false);
      ac.setIFeel(false);
      ac.setTemp(tempSet);
      ac.on();
      ac.send();
      Serial.println("AC ON");
}
