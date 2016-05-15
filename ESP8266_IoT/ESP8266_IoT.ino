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
#include <ESP8266WiFi.h>
#include "WiFiUtil.h"
#include "WebConfig.h"
#include "NtpUtil.h"
#include "LooperThreadTicker.h"
#include "PressureSensor.h"
#include "TemperatureSensor.h"

#include <FS.h>
#include <Time.h>
#include <TimeLib.h>

// --- config
const int MODE_PIN = 0; // GPIO0 {Low: WiFi AP / High: WiFi Client}

// --- config: WIFI
const char* WIFI_CONFIG = "/wifi_config";
const char* WIFIAP_PASSWORD = "1234567890"; // you can see WiFi Mac Address's SSID and this is password for setup SSID/Password from web.

// --- config: NTP
const char* NTP_SERVER = "jp.pool.ntp.org";

// --- config: httpd
int HTTP_SERVER_PORT = 80;
const char* HTML_HEAD = "<html><head><title>hidenorly's ESP8266</title></head><body>";

// --- config: sensor support
#define ENABLE_I2C_BUS
#define ENABLE_SENSOR
#define ENABLE_SENSOR_PRESSURE 1
#define ENABLE_TEMPERATURE_PRESSURE 1

// --- GPIO config
void setOutputAndValue(int port, int initialVal) {
  digitalWrite(port, initialVal); // Output data should be set first (before direction setting)
  pinMode(port, OUTPUT);
}

void initializeGPIO() {
  // General config : BOOT MODE (GPIO0,2,15) related
  pinMode(0, INPUT);
#ifdef ENABLE_I2C_BUS
  pinMode(2, INPUT_PULLUP);   // for I2C
  pinMode(14, INPUT_PULLUP);  // for I2C
#else // ENABLE_I2C_BUS
  pinMode(2, INPUT);
#endif // ENABLE_I2C_BUS
  pinMode(15, INPUT);

  // Project related config
  pinMode(MODE_PIN, INPUT); // GPIO0 is for switching mode Low: WiFi AP Mode (Config) / High: WiFi Client (Normal)

  // If pin is NC, we should set {output / High} or {input / pull up enabled} on the pin.
  setOutputAndValue(4, HIGH);
  setOutputAndValue(5, HIGH);
  setOutputAndValue(12, HIGH);
  setOutputAndValue(13, HIGH);
#ifndef ENABLE_I2C_BUS
  setOutputAndValue(14, HIGH);
#endif // ENABLE_I2C_BUS
  setOutputAndValue(16, HIGH);
}

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
#define NUM_OF_SENSORS  (ENABLE_SENSOR_PRESSURE+ENABLE_TEMPERATURE_PRESSURE)
ISensor* g_pSensors[NUM_OF_SENSORS];
int g_NUM_SENSORS=0;
#endif // ENABLE_SENSOR

// --- debug
void periodicTask(void* p){
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
}

class Poller:public TimerContextTicker
{
  public:
    Poller(int dutyMSec=0):TimerContextTicker(NULL, NULL, dutyMSec)
    {
    }
    virtual void doCallback(void)
    {
      DEBUG_PRINTLN("Poller callback");
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
#ifdef ENABLE_TEMPERATURE_PRESSURE
  g_pSensors[n++] = new TemperatureSensor();
#endif // ENABLE_TEMPERATURE_PRESSURE
  g_NUM_SENSORS = n;
  for(int i=0; i<n; i++){
    g_pSensors[i]->initialize();
  }
#endif // ENABLE_SENSOR

  // register periodic tasks which need to be called from loop() when the timer are activated.
  g_LooperThreadManager.add( new LooperThreadTicker(reinterpret_cast<LooperThreadTicker::CALLBACK_FUNC>(periodicTask), NULL, 1000) );

  static Poller* sPoll=new Poller(1000);
  sPoll->registerToTimer();
}


void loop() {
  // put your main code here, to run repeatedly:
  handleWiFiClientStatus();
  handleWebServer();
  g_LooperThreadManager.handleLooperThread();
}
