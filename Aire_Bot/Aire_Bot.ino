#include "CTBot.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Electra.h>
#include <DHT.h>

CTBot miBot;
CTBotInlineKeyboard miTeclado;

#define DHTPIN D3 
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "WLAN_SSID";
const char* password = "WLAN_PASS";
const String token = "TOKEN_TELERGAM"; 

const uint16_t kIrLed = D2;
IRElectraAc ac(kIrLed);

int tempSet = 21;

void setup() {
  Serial.begin(115200);

  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  ESP.wdtFeed();

  ac.begin(); 

  Serial.println("Iniciando Bot Telegram...");

  miBot.wifiConnect(ssid, password);

  miBot.setTelegramToken(token);

  if (miBot.testConnection()) {
    Serial.println("Bot Conectado");
  }
  else {
    Serial.println("Error");
  }

  miTeclado.addButton("Encender ☃️", "encender", CTBotKeyboardButtonQuery);
  miTeclado.addButton("Apagar 💔", "apagar", CTBotKeyboardButtonQuery);
  miTeclado.addRow();
  miTeclado.addButton("+1 °C ➕", "plus", CTBotKeyboardButtonQuery);
  miTeclado.addButton("-1 °C ➖", "minus", CTBotKeyboardButtonQuery);
  miTeclado.addRow();
  miTeclado.addButton("Temperatura Actual 🌡️","temp", CTBotKeyboardButtonQuery);
  miTeclado.addRow();
  miTeclado.addButton("Humedad Actual ♒","hum", CTBotKeyboardButtonQuery);
}

void loop() {
  TBMessage msg;

  if (miBot.getNewMessage(msg)) {
    if (msg.messageType == CTBotMessageText) {
      if (msg.text.equalsIgnoreCase("opciones")) {
        miBot.sendMessage(msg.sender.id, "Aire Acondicionado", miTeclado);
      }
    } else if (msg.messageType == CTBotMessageQuery) {
      if (msg.callbackQueryData.equals("encender")) {
        turnOn();
        miBot.endQuery(msg.callbackQueryID, "Aire Encendido a " + String(tempSet) + "°C", true);
      } else if (msg.callbackQueryData.equals("apagar")) {
        turnOff();
        miBot.endQuery(msg.callbackQueryID, "Aire Apagado");
      }
       else if (msg.callbackQueryData.equals("temp")) {
        float t = dht.readTemperature(); 
        //String msg = "Temperatura actual: " + String(t) + " °C";
        miBot.endQuery(msg.callbackQueryID, "Temperatura actual: " + String(t) + "°C", true);
      }
       else if (msg.callbackQueryData.equals("plus")) {
        increaseTemp();
        miBot.endQuery(msg.callbackQueryID, "Temperatura Seteada: "  + String(tempSet) + "°C");
      }
       else if (msg.callbackQueryData.equals("minus")) {
         reduceTemp();
        miBot.endQuery(msg.callbackQueryID, "Temperatura Seteada: "  + String(tempSet) + "°C");
      }
       else if (msg.callbackQueryData.equals("hum")) {
        float h = dht.readHumidity();
        miBot.endQuery(msg.callbackQueryID, "Humedad Actual:  "  + String(h) + "%", true);
      }
    }
  }
}

void increaseTemp() {
 if(tempSet != 32)
  tempSet++;
 ac.setTemp(tempSet);
 ac.send();
}

void reduceTemp() {
  if(tempSet != 16)
    tempSet--;
  ac.setTemp(tempSet);
  ac.send();
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
