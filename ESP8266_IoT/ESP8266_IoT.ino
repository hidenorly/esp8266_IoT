/* 
 Copyright (C) 2016, 2017, 2018 hidenorly

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

extern "C" {
#include "user_interface.h"
}

#include "base.h"
#include "config.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include "WiFiUtil.h"
#include "WebConfig.h"
#include "NtpUtil.h"
#include "LooperThreadTicker.h"
#include "PressureSensor.h"
#include "TemperatureSensor.h"
#include "HumiditySensor.h"
#include "OtaManager.h"

#include <FS.h>
#include <Time.h>
#include <TimeLib.h>
#include "ServoSwitch.h"
#include "MQTTManager.h"

#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)

// --- global variable definition
#ifdef ENABLE_SENSOR
#define NUM_OF_SENSORS  (ENABLE_SENSOR_PRESSURE+ENABLE_SENSOR_TEMPERATURE+ENABLE_SENSOR_HUMIDITY)
ISensor* g_pSensors[NUM_OF_SENSORS];
int g_NUM_SENSORS=0;
#endif // ENABLE_SENSOR

#ifdef ENABLE_MQTT
MQTTManager* gpMQTTManager = MQTTManager::getInstance();
#endif // ENABLE_MQTT

// --- UPnP Device
#ifdef ENABLE_UPNP
#include "Ssdp.h"
#include "UPnPDeviceWiFiSwitch.h"

static Ssdp* g_pSsdp=NULL;
static UPnPDevice* g_pSwitch1=NULL;
#endif // ENABLE_UPNP

// --- per feature setup
#ifdef ENABLE_MQTT
void setupMQTT(void)
{
  MQTTManager::initialize(MQTT_SERVER, MQTT_SERVER_PORT, MQTT_USERNAME, MQTT_PASSWORD);

  // create MQTT publisher
  #if ENABLE_SENSOR_PRESSURE
    gpMQTTManager->addPublisher(SENSOR_PRESSURE, MQTT_CLIENTID"sensor/pressure");
  #endif // ENABLE_SENSOR_PRESSURE
  #if ENABLE_SENSOR_TEMPERATURE
    gpMQTTManager->addPublisher(SENSOR_TEMPERATURE, MQTT_CLIENTID"sensor/temperature");
  #endif // ENABLE_SENSOR_TEMPERATURE
  #if ENABLE_SENSOR_HUMIDITY
    gpMQTTManager->addPublisher(SENSOR_HUMIDITY, MQTT_CLIENTID"sensor/humidity");
  #endif // ENABLE_SENSOR_HUMIDITY

  // create MQTT subscriber
  #if ENABLE_SWITCH_FAN
    gpMQTTManager->addSubscriber(0, MQTT_CLIENTID"switch/fan");
  #endif // ENABLE_SWITCH_FAN
  }
#endif // ENABLE_MQTT

#ifdef ENABLE_SENSOR
void setupSensors(void)
{
    // Initialize sensor
    int n=0;
  #if ENABLE_SENSOR_PRESSURE
    g_pSensors[n++] = new PressureSensor();
  #endif // ENABLE_SENSOR_PRESSURE
  #if ENABLE_SENSOR_TEMPERATURE
    g_pSensors[n++] = new TemperatureSensor();
  #endif // ENABLE_SENSOR_TEMPERATURE
  #if ENABLE_SENSOR_HUMIDITY
    g_pSensors[n++] = new HumiditySensor();
  #endif // ENABLE_SENSOR_HUMIDITY
    g_NUM_SENSORS = n;
    for(int i=0; i<n; i++){
      DEBUG_PRINT("Init:");
      DEBUG_PRINTLN(g_pSensors[i]->getName());
      g_pSensors[i]->initialize();
    }
    DEBUG_PRINT(n);
    DEBUG_PRINTLN(" sensors are initialized.");
}
#endif // ENABLE_SENSOR

// --- handler for WiFi client enabled
void onWiFiClientConnected(){
  DEBUG_PRINTLN("WiFi connected.");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
  start_NTP(); // socket related is need to be executed in main loop.
  #ifdef ENABLE_MQTT
  #if ENABLE_SWITCH_FAN
  gpMQTTManager->enableSubscriber(0, true);
  #endif // ENABLE_SWITCH_FAN
  gpMQTTManager->connect();
  #endif // ENABLE_MQTT

  #ifdef ENABLE_UPNP
  if(g_pSsdp){
    if(!g_pSwitch1){
      g_pSwitch1 = new UPnPDeviceWiFiSwitch("test", 31415, "Belkin:device:**", "38323636-4558-4dda-9188-cda0e6aabbcc", "/setup.xml");
    }
    if(g_pSwitch1){
      g_pSwitch1->enable(true);
      g_pSsdp->addUPnPDevice(g_pSwitch1);
    }
    g_pSsdp->enable(true);
  }
  #endif // ENABLE_UPNP
}


// --- mode changer
bool initializeProperMode(bool bSPIFFS){
  if( !bSPIFFS || (digitalRead(MODE_PIN) == 0) || (!SPIFFS.exists(WIFI_CONFIG))){
    // setup because WiFi AP mode is specified or WIFI_CONFIG is not found.
    setupWiFiAP();
    setup_httpd();
    return false;
  } else {
    setupWiFiClient();
    setup_httpd();  // comment this out if you don't need to have httpd on WiFi client mode
  }
  return true;
}

class Poller:public LooperThreadTicker
{
  public:
    Poller(int dutyMSec=0):LooperThreadTicker(NULL, NULL, dutyMSec)
    {
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

            #ifdef ENABLE_MQTT
            if( gpMQTTManager ) {
              MQTT_PUBLISHER* pPublisher = gpMQTTManager->getPublisher(g_pSensors[i]->getSensorType());
              if( pPublisher ){
                pPublisher->publish(g_pSensors[i]->getFloatValue());
              }
            }
            #endif // ENABLE_MQTT
          }
        }
      }
    #endif // ENABLE_SENSOR
    }
};

// --- General setup() function
void setup() {
  // Initialize GPIO
  initializeGPIO();
  
  // Initialize SerialPort
  Serial.begin(115200);

  // Initialize SPI File System
  bool bSPIFFS = SPIFFS.begin();
  if(!bSPIFFS){
    SPIFFS.format();
  }

  // Check mode
  delay(1000);
  if(initializeProperMode(bSPIFFS)){
  #ifdef ENABLE_UPNP
    g_pSsdp = new Ssdp();
  #endif // ENABLE_UPNP

  #ifdef ENABLE_SENSOR
  setupSensors();
  #endif // ENABLE_SENSOR

  #ifdef ENABLE_MQTT
    setupMQTT();
  #endif // ENABLE_MQTT
    static Poller* sPoll=new Poller(1000);
    g_LooperThreadManager.add(sPoll);
  }
}

void disableWdt(void)
{
  wdt_disable();
  ESP.wdtDisable();
}

void loop() {
//  Serial.printf("loop heap size: %u\r\n", ESP.getFreeHeap());
  handleWiFiClientStatus();
  handleWebServer();
  g_LooperThreadManager.handleLooperThread();

  #ifdef ENABLE_MQTT
  if( gpMQTTManager ) {
    int subscriberKey=0;
    if( gpMQTTManager->handleSubscriber(subscriberKey) ){
      if( subscriberKey==0 ){
        const char* result = gpMQTTManager->getLastSubscriberValue(subscriberKey);
        DEBUG_PRINT("Switch request: "); DEBUG_PRINTLN(result);
        #ifdef ENABLE_SERVO
        #if ENABLE_SWITCH_FAN
        static DualServoSeesawSwitch seesawSwitch(GPO_SERVO_SWITCH1,GPO_SERVO_SWITCH2);
        seesawSwitch.turnOn( (result[0]=='1') ? true : false );
        #endif // ENABLE_SWITCH_FAN
        #endif // ENABLE_SERVO
      }
    }
  }
  #endif // ENABLE_MQTT

  #ifdef ENABLE_OTA
  // OTA
  static OtaManager ota(OTA_PIN, OTA_PIN_PERIOD);
  ota.handle();
  #endif // ENABLE_OTA

  #ifdef ENABLE_UPNP
  // UPnP
  if(g_pSsdp && g_pSsdp->getEnabled()){
    g_pSsdp->handle();
  }
  #endif // ENABLE_UPNP
}
