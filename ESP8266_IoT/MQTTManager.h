/* 
 Copyright (C) 2017 hidenorly

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

#ifndef __MQTTMANAGER_H__
#define __MQTTMANAGER_H__

#include "base.h"
#include "TemplateArray.h"

#define USE_ADAFRUIT_MQTT 1

#ifdef USE_ADAFRUIT_MQTT
  #include <Adafruit_MQTT.h>
  #include <Adafruit_MQTT_Client.h>
  #define MQTT_PUBLISHER Adafruit_MQTT_Publish
#endif

class MQTTContainer
{
  public:
    MQTTContainer(int key, MQTT_PUBLISHER* pPublish):key(key),pPublish(pPublish){};
    ~MQTTContainer(){};
    int key;
    MQTT_PUBLISHER* pPublish;
};

class MQTTManager
{
  public:
    static MQTTManager* getInstance(void);
    static void releaseInstance(void);
    void addPublisher(int key, MQTT_PUBLISHER* pPublish);
    MQTT_PUBLISHER* getPublisher(int key);

  protected:
    MQTTManager();
    ~MQTTManager();
    static void cleanUp(void);

    static TemplateArray<MQTTContainer> mpMQTT;

    static MQTTManager* mpThis;
    static int mRefCount;
};


#endif // __MQTTMANAGER_H__
