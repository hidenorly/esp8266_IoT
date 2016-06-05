/* 
 Copyright (C) 2016 hidenorly

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "base.h"
#include "config.h"
#include <ESP8266WiFi.h>
#include "WiFiUtil.h"
#include "WebConfig.h"
#include "NtpUtil.h"
#include "LooperThreadTicker.h"
#include "PressureSensor.h"
#include "TemperatureSensor.h"
#include "HumiditySensor.h"

#include <FS.h>
#include <Time.h>
#include <TimeLib.h>
#include "ServoManager.h"
#include "ServoDrvSG90D.h"


// --- mode changer
void initializeProperMode(){
  if( (digitalRead(MODE_PIN) == 0) || (!SPIFFS.exists(WIFI_CONFIG)) ){
    // setup because WiFi AP mode is specified or WIFI_CONFIG is not found.
    setupWiFiAP();
    setup_httpd();
  } else {
    setupWiFiClient();
    setup_httpd();  // comment this out if you don't need to have httpd on WiFi client mode
  }
}

// --- handler for WiFi client enabled
void onWiFiClientConnected(){
  DEBUG_PRINTLN("WiFi connected.");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
  start_NTP(); // socket related is need to be executed in main loop.
}

#ifdef ENABLE_SENSOR
#define NUM_OF_SENSORS  (ENABLE_SENSOR_PRESSURE+ENABLE_SENSOR_TEMPERATURE+ENABLE_SENSOR_HUMIDITY)
ISensor* g_pSensors[NUM_OF_SENSORS];
int g_NUM_SENSORS=0;
#endif // ENABLE_SENSOR

class Poller:public LooperThreadTicker
{
  protected:
    DHT11* mpDHT11;
  public:
    Poller(int dutyMSec=0):LooperThreadTicker(NULL, NULL, dutyMSec)
    {
      mpDHT11 = DHT11::getInstance();
    }
    virtual void doCallback(void)
    {
      char s[30];
      time_t n = now();
      sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d", year(n), month(n), day(n), hour(n), minute(n), second(n));
    
      DEBUG_PRINT("UTC : ");
      DEBUG_PRINTLN(s);
    
    #ifdef ENABLE_SENSOR
      if( g_pSensors ){
        for(int i=0; i<g_NUM_SENSORS; i++){
          if( g_pSensors[i]!=NULL){
            DEBUG_PRINT(g_pSensors[i]->getName());
            DEBUG_PRINT(" :");
            DEBUG_PRINT(g_pSensors[i]->getFloatValue());
            DEBUG_PRINT(" [");
            DEBUG_PRINT(g_pSensors[i]->getUnit());
            DEBUG_PRINTLN("]");
          }
        }
      }
    #endif // ENABLE_SENSOR

    #ifdef ENABLE_SERVO
    ServoManager* pServoManager = ServoManager::getInstance();
    static int d=0;
    d++;
    if( d & 0x01 ){
      pServoManager->setAngle(0, 30.0f);
      pServoManager->setAngle(1, 0.0f);
    } else {
      pServoManager->setAngle(0, 00.0f);
      pServoManager->setAngle(1, -30.0f);
    }
    #endif // ENABLE_SERVO
    }
};


// --- General setup() function
void setup() {
  // Initialize GPIO
  initializeGPIO();
  
  // Initialize SerialPort
  Serial.begin(115200);

  // Initialize SPI File System
  SPIFFS.begin();

  // Check mode
  delay(1000);
  initializeProperMode();

#ifdef ENABLE_SENSOR
  // Initialize sensor
  int n=0;
#ifdef ENABLE_SENSOR_PRESSURE
  g_pSensors[n++] = new PressureSensor();
#endif // ENABLE_SENSOR_PRESSURE
#ifdef ENABLE_SENSOR_TEMPERATURE
  g_pSensors[n++] = new TemperatureSensor();
#endif // ENABLE_SENSOR_TEMPERATURE
#ifdef ENABLE_SENSOR_HUMIDITY
  g_pSensors[n++] = new HumiditySensor();
#endif // ENABLE_SENSOR_HUMIDITY
  g_NUM_SENSORS = n;
  for(int i=0; i<n; i++){
    g_pSensors[i]->initialize();
  }
#endif // ENABLE_SENSOR

  static Poller* sPoll=new Poller(1000);
  g_LooperThreadManager.add(sPoll);
}


void loop() {
  // put your main code here, to run repeatedly:
  handleWiFiClientStatus();
  handleWebServer();
  g_LooperThreadManager.handleLooperThread();
}
