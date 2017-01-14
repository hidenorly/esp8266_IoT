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

#ifndef __CONFIG_H__
#define __CONFIG_H__

// --- config
extern const int MODE_PIN;

// --- config: WIFI
extern const char* WIFI_CONFIG;
extern const char* WIFIAP_PASSWORD;

// --- config: NTP
extern const char* NTP_SERVER;

// --- config: httpd
extern int HTTP_SERVER_PORT;
extern const char* HTML_HEAD;

#define BB  1
#define BOX1  2
#define BB2 3
#define TARGET_PRODUCT BOX1
//#define TARGET_PRODUCT BB
//#define TARGET_PRODUCT BB2

#if TARGET_PRODUCT == BOX1
  // --- config: MQTT
  #define ENABLE_MQTT 1
  #if ENABLE_MQTT
  extern const char* MQTT_SERVER;
  extern const int MQTT_SERVER_PORT;
  //extern const char* MQTT_CLIENTID;
  #define MQTT_CLIENTID "/hidenorly/"
  extern const char* MQTT_USERNAME;
  extern const char* MQTT_PASSWORD;
  #endif // ENABLE_MQTT
  
  // --- config: sensor support
  //#define ENABLE_I2C_BUS
  #define ENABLE_SENSOR
  #define ENABLE_SENSOR_PRESSURE 0
  #define ENABLE_SENSOR_TEMPERATURE 1
  #define ENABLE_SENSOR_HUMIDITY 1
  #define TEMPERATURE_SENSOR_DRIVER DHT11
  
  // --- config: servo support
  #define ENABLE_SERVO
  
  // --- config: fan sw
  #define ENABLE_SWITCH_FAN 1

#elif TARGET_PRODUCT == BB2 // TARGET_PRODUCT

  // --- config: MQTT
  #define ENABLE_MQTT 0
  #if ENABLE_MQTT
  extern const char* MQTT_SERVER;
  extern const int MQTT_SERVER_PORT;
  //extern const char* MQTT_CLIENTID;
  #define MQTT_CLIENTID "/hidenorly/"
  extern const char* MQTT_USERNAME;
  extern const char* MQTT_PASSWORD;
  #endif // ENABLE_MQTT
  
  // --- config: sensor support
  //#define ENABLE_I2C_BUS
  //#define ENABLE_SENSOR
  //#define ENABLE_SENSOR_PRESSURE 0
  //#define ENABLE_SENSOR_TEMPERATURE 1
  //#define ENABLE_SENSOR_HUMIDITY 1
  //#define TEMPERATURE_SENSOR_DRIVER DHT11
  
  // --- config: servo support
  //#define ENABLE_SERVO
  
  // --- config: fan sw
  //#define ENABLE_SWITCH_FAN 1

#else // TARGET_PRODUCT

  // --- config: MQTT
  #define ENABLE_MQTT 1
  #if ENABLE_MQTT
  extern const char* MQTT_SERVER;
  extern const int MQTT_SERVER_PORT;
  //extern const char* MQTT_CLIENTID;
  #define MQTT_CLIENTID "/hidenorly/"
  extern const char* MQTT_USERNAME;
  extern const char* MQTT_PASSWORD;
  #endif // ENABLE_MQTT
  
  // --- config: sensor support
  #define ENABLE_I2C_BUS
  #define ENABLE_SENSOR
  #define ENABLE_SENSOR_PRESSURE 1
  #define ENABLE_SENSOR_TEMPERATURE 1
  #define ENABLE_SENSOR_HUMIDITY 1
  #define TEMPERATURE_SENSOR_DRIVER BMP180
  //#define TEMPERATURE_SENSOR_DRIVER DHT11
  
  // --- config: servo support
  #define ENABLE_SERVO
  
  // --- config: fan sw
  #define ENABLE_SWITCH_FAN 1
#endif // TARGET_PRODUCT

// --- GPIO initial setup
void initializeGPIO(void);

#endif // __CONFIG_H__

