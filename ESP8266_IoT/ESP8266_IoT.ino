/* 
 Copyright (C) 2016,2017 hidenorly

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
#include "MQTTManager.h"

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)

#ifdef ENABLE_SENSOR
#define NUM_OF_SENSORS  (ENABLE_SENSOR_PRESSURE+ENABLE_SENSOR_TEMPERATURE+ENABLE_SENSOR_HUMIDITY)
ISensor* g_pSensors[NUM_OF_SENSORS];
int g_NUM_SENSORS=0;
#endif // ENABLE_SENSOR

#if ENABLE_MQTT
  #if ENABLE_SSL_MQTT
    WiFiClientSecure g_MQTTClient;
  #else // ENABLE_SSL_MQTT
    WiFiClient g_MQTTClient;
  #endif // ENABLE_SSL_MQTT
  Adafruit_MQTT_Client g_mqtt(&g_MQTTClient, MQTT_SERVER, MQTT_SERVER_PORT, MQTT_USERNAME, MQTT_PASSWORD);

  #if ENABLE_SWITCH_FAN
    Adafruit_MQTT_Subscribe g_fanSwitch = Adafruit_MQTT_Subscribe(&g_mqtt, MQTT_CLIENTID"switch/fan");
  #endif // ENABLE_SWITCH_FAN

  MQTTManager* gpMQTTManager = MQTTManager::getInstance();
  void setupMQTT(void)
  {
    #if ENABLE_SENSOR_PRESSURE
      gpMQTTManager->addPublisher(SENSOR_PRESSURE, new Adafruit_MQTT_Publish(&g_mqtt, MQTT_CLIENTID"sensor/pressure"));
    #endif // ENABLE_SENSOR_PRESSURE
    #if ENABLE_SENSOR_TEMPERATURE
      gpMQTTManager->addPublisher(SENSOR_TEMPERATURE, new Adafruit_MQTT_Publish(&g_mqtt, MQTT_CLIENTID"sensor/temperature"));
    #endif // ENABLE_SENSOR_TEMPERATURE
    #if ENABLE_SENSOR_HUMIDITY
      gpMQTTManager->addPublisher(SENSOR_HUMIDITY, new Adafruit_MQTT_Publish(&g_mqtt, MQTT_CLIENTID"sensor/humidity"));
    #endif // ENABLE_SENSOR_HUMIDITY
  }
#endif // ENABLE_MQTT


#define SERVO_TEST 0


// --- mode changer
bool initializeProperMode(){
  if( (digitalRead(MODE_PIN) == 0) || (!SPIFFS.exists(WIFI_CONFIG)) ){
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

// --- handler for WiFi client enabled
void onWiFiClientConnected(){
  DEBUG_PRINTLN("WiFi connected.");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
  start_NTP(); // socket related is need to be executed in main loop.
  #if ENABLE_MQTT
  #if ENABLE_SWITCH_FAN
  g_mqtt.subscribe(&g_fanSwitch);
  #endif // ENABLE_SWITCH_FAN
  g_mqtt.connect();
  #endif // ENABLE_MQTT
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

            #if ENABLE_MQTT
            if( gpMQTTManager ) {
              Adafruit_MQTT_Publish* pPublisher = gpMQTTManager->getPublisher(g_pSensors[i]->getSensorType());
              if( pPublisher ){
                pPublisher->publish(g_pSensors[i]->getFloatValue());
              }
            }
            #endif // ENABLE_MQTT
          }
        }
      }
    #endif // ENABLE_SENSOR

    #ifdef ENABLE_SERVO
#if SERVO_TEST
    ServoManager* pServoManager = ServoManager::getInstance();
    static int d=0;
    d++;
    if( d & 0x01 ){
      pServoManager->setAngle(0, 30.0f);
      pServoManager->setAngle(1, -15.0f);
    } else {
      pServoManager->setAngle(0, 15.0f);
      pServoManager->setAngle(1, -30.0f);
    }
    pServoManager->enableServo(0, true);
    pServoManager->enableServo(1, true);
#endif
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
  if(initializeProperMode()){
  #ifdef ENABLE_SENSOR
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
  #endif // ENABLE_SENSOR

  #if ENABLE_MQTT
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
  Serial.printf("loop heap size: %u\r\n", ESP.getFreeHeap());
  handleWiFiClientStatus();
  handleWebServer();
  g_LooperThreadManager.handleLooperThread();

  #if ENABLE_MQTT
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = g_mqtt.readSubscription(5))) {
  #if ENABLE_SWITCH_FAN
    if (subscription == &g_fanSwitch) {
      Serial.print("Got: ");
      Serial.println((char *)g_fanSwitch.lastread);
    }
  #endif // ENABLE_SWITCH_FAN
  }
  #endif // ENABLE_MQTT
}
