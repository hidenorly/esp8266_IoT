/* 
 Copyright (C) 2016,2018 hidenorly

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

// --- config: UPnP
#define ENABLE_UPNP

// --- config: OTA
#define ENABLE_OTA

#ifdef ENABLE_OTA
  extern const int OTA_PIN;
  #define OTA_PIN_PERIOD  5000
#endif // ENABLE_OTA

// --- board configs
#define BB  1
#define BOX1  2
#define WEMOSD1 4

#define TARGET_PRODUCT BOX1
//#define TARGET_PRODUCT BB
//#define TARGET_PRODUCT WEMOSD1


#if TARGET_PRODUCT == BOX1
  // --- config: MQTT
  #define ENABLE_MQTT
  
  // --- config: sensor support
  #define ENABLE_SENSOR_PRESSURE 0     // 1 Require BMP180
  #define ENABLE_SENSOR_HUMIDITY 1     // 1 Require DHT11

  #define ENABLE_SENSOR_TEMPERATURE 1 // 1 Require BMP180 or DHT11
  #if ENABLE_SENSOR_TEMPERATURE
//  #define TEMPERATURE_SENSOR_DRIVER BMP180 // Require ENABLE_I2C_BUS
    #define TEMPERATURE_SENSOR_DRIVER DHT11
  #endif // ENABLE_SENSOR_TEMPERATURE

  // --- config: servo support
  #define ENABLE_SERVO // require ENABLE_PWM 

  // --- config: fan sw
  #define ENABLE_SWITCH_FAN 1
  #define ENABLE_SERVO_SWITCH

  #ifdef ENABLE_SERVO_SWITCH
    #define GPO_SERVO_SWITCH1 12
    #define GPO_SERVO_SWITCH2 13
  #endif // ENABLE_SERVO_SWITCH

#else // TARGET_PRODUCT

  #define ENABLE_MQTT
  
  // --- config: sensor support
  #define ENABLE_SENSOR_PRESSURE 0     // 1 Require BMP180
  #define ENABLE_SENSOR_HUMIDITY 0     // 1 Require DHT11

  #define ENABLE_SENSOR_TEMPERATURE 0 // 1 Require TEMPERATURE_SENSOR_DRIVER
  #if ENABLE_SENSOR_TEMPERATURE
//  #define TEMPERATURE_SENSOR_DRIVER BMP180 // Require ENABLE_I2C_BUS
//  #define TEMPERATURE_SENSOR_DRIVER DHT11
  #endif // ENABLE_SENSOR_TEMPERATURE

  #if ENABLE_SENSOR_PRESSURE || ENABLE_SENSOR_TEMPERATURE || ENABLE_SENSOR_HUMIDITY
    #define ENABLE_SENSOR  // Require at least one ENABLE_SENSOR_XXXX = 1
  #endif // ENABLE_SENSOR_PRESSURE || ENABLE_SENSOR_TEMPERATURE || ENABLE_SENSOR_HUMIDITY
  
  // --- config: servo support
//  #define ENABLE_SERVO // require ENABLE_PWM
  
  // --- config: fan sw
//  #define ENABLE_SWITCH_FAN 1 // Require ENABLE_SERVO
#endif // TARGET_PRODUCT

// --- GPIO initial setup
void initializeGPIO(void);


// --- config: MQTT
#ifdef ENABLE_MQTT
  extern const char* MQTT_SERVER;
  extern const int MQTT_SERVER_PORT;
  //extern const char* MQTT_CLIENTID;
  #define MQTT_CLIENTID "/hidenorly/"
  extern const char* MQTT_USERNAME;
  extern const char* MQTT_PASSWORD;
#endif // ENABLE_MQTT

// --- sensors dependency
#if ENABLE_SENSOR_PRESSURE || ENABLE_SENSOR_TEMPERATURE || ENABLE_SENSOR_HUMIDITY
  #ifndef ENABLE_SENSOR
    #define ENABLE_SENSOR  // Require at least one ENABLE_SENSOR_XXXX = 1
  #endif // ENABLE_SENSOR
#endif // ENABLE_SENSOR_PRESSURE || ENABLE_SENSOR_TEMPERATURE || ENABLE_SENSOR_HUMIDITY

// ENABLE_SERVO_SWITCH dependency
#ifdef ENABLE_SERVO_SWITCH
  #define ENABLE_SERVO
#endif // ENABLE_SERVO_SWITCH

// --- SERVO dependency
#ifdef ENABLE_SERVO
  #ifndef ENABLE_PWM
    #define ENABLE_PWM
  #endif // ENABLE_PWM
#endif // ENABLE_SERVO

// BMP180 dependency
#if TEMPERATURE_SENSOR_DRIVER==BMP180
  #ifndef ENABLE_I2C_BUS
    #define ENABLE_I2C_BUS
  #endif // ENABLE_I2C_BUS
#endif // TEMPERATURE_SENSOR_DRIVER

#endif // __CONFIG_H__
