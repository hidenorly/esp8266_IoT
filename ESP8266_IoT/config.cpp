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

#include "base.h"
#include "config.h"

// --- config
const int MODE_PIN = 0; // GPIO0 {Low: WiFi AP / High: WiFi Client}

// --- config: WIFI
const char* WIFI_CONFIG = "/wifi_config";
const char* WIFIAP_PASSWORD = "1234567890"; // you can see WiFi Mac Address's SSID and this is password for setup SSID/Password from web.

// --- config: NTP
const char* NTP_SERVER = "time.google.com";

// --- config: httpd
int HTTP_SERVER_PORT = 80;
const char* HTML_HEAD = "<html><head><title>hidenorly's ESP8266</title></head><body>";

// --- config: MQTT
#ifdef ENABLE_MQTT
const char* MQTT_SERVER = "192.168.10.31";
const int MQTT_SERVER_PORT = 1883;
//const char* MQTT_CLIENTID = "_hidenoly_client_id";
const char* MQTT_USERNAME = "username";
const char* MQTT_PASSWORD = "password";
#endif // ENABLE_MQTT

// --- config: OTA
#ifdef ENABLE_OTA
const int OTA_PIN = MODE_PIN; // GPIO0
#endif // ENABLE_OTA

// --- GPIO config
void initializeGPIO(void) {
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
  pinMode(MODE_PIN, INPUT_PULLUP); // GPIO0 is for switching mode Low: WiFi AP Mode (Config) / High: WiFi Client (Normal)

  // If pin is NC, we should set {output / High} or {input / pull up enabled} on the pin.
#if ENABLE_SENSOR_HUMIDITY
  pinMode(4, INPUT);   // for DHT11
#else // ENABLE_DHT11
  setOutputAndValue(4, HIGH);
#endif // ENABLE_DHT11
  setOutputAndValue(5, HIGH);
#ifndef ENABLE_SERVO_SWITCH
  setOutputAndValue(12, HIGH); // GPO_SERVO_SWITCH1 if ENABLE_SERVO_SWITCH
  setOutputAndValue(13, HIGH); // GPO_SERVO_SWITCH2 if ENABLE_SERVO_SWITCH
#endif // ENABLE_SERVO_SWITCH
#ifndef ENABLE_I2C_BUS
  setOutputAndValue(14, HIGH);
#endif // ENABLE_I2C_BUS
  setOutputAndValue(16, HIGH);
}

