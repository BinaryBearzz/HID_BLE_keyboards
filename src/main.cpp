#include <Arduino.h>
#include <BleKeyboard.h>
#include "EventVolume.h"
const int potPin = 34;
const int swPin = 2;

std::string nameDevice = "BLE_volume";
BleKeyboard BLE_KB(nameDevice, "Espressif", 100);

EventVolume volumes(potPin, 0, 4095, Analog);
EventVolume sws(swPin, Digital);
// int potValue = 0;
bool toggles = false;
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  BLE_KB.begin();
  
  volumes.onEventVolumeUp([](){
    Serial.println("VolumeUp+");
    if(BLE_KB.isConnected())
    {  
      BLE_KB.write(KEY_MEDIA_VOLUME_UP);
      BLE_KB.write(KEY_MEDIA_VOLUME_UP);
      BLE_KB.write(KEY_MEDIA_VOLUME_UP);
      BLE_KB.write(KEY_MEDIA_VOLUME_UP);
      BLE_KB.write(KEY_MEDIA_VOLUME_UP);
      BLE_KB.write(KEY_MEDIA_VOLUME_UP);
    }
  });
  volumes.onEventVolumeDown([](){
    Serial.println("VolumeDown-");
    if(BLE_KB.isConnected())
    {
      BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
      BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
      BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
      BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
      BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
      BLE_KB.write(KEY_MEDIA_VOLUME_DOWN);
    }
  });

  sws.onEventSW([](bool stateSW){
    if(stateSW)
    {
      if(BLE_KB.isConnected())
      {
        toggles = !toggles;
        if(toggles)
        {
          BLE_KB.write(KEY_MEDIA_MUTE);
        }
        else{
          BLE_KB.write(KEY_MEDIA_VOLUME_UP);
        }
      }
    }
  });
}

void loop() {

  volumes.handler();
  sws.handler();
  // potValue = analogRead(potPin);
  // Serial.println(potValue);
  // BLE_KB.press(KEY_MEDIA_VOLUME_UP);
  delay(50);
}