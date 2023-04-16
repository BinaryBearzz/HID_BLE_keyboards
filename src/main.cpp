#include <Arduino.h>
#include <BleKeyboard.h>
#include <ESP32Encoder.h>
#include <EventChange.h>
#include "BLEClient.h"
const int Apin = 2;
const int Bpin = 4;
#define LED 5

std::string nameDevice = "BLE_VOLUME";
BleKeyboard BLE_KB(nameDevice, "Espressif", 100);

ESP32Encoder encoder;
EventChange ev_change;
unsigned long prv = 0;
int interval = 500;

void blinkStatus()
{

    if (!BLE_KB.isConnected())
    {
        if (millis() - prv > interval)
        {
            digitalWrite(LED, !digitalRead(LED));
            prv = millis();
        }
    }
    else
    {
        digitalWrite(LED, LOW);
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    Serial.println("Starting BLE work!");
    BLE_KB.begin();
    encoder.attachFullQuad(Apin, Bpin);
    encoder.setCount(0);
    ev_change.onChange([&](bool isUP)
                       {
        if (BLE_KB.isConnected())
        {
            if(isUP)
            {
                BLE_KB.write(KEY_MEDIA_VOLUME_UP);
                Serial.println(F("UP"));
            }
            else{
                BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
                Serial.println(F("DOWN"));
            } 
        } });
}

void loop()
{
    long newPosition = encoder.getCount() / 2;
    ev_change.handler(newPosition);
    delay(50);
    blinkStatus();
}