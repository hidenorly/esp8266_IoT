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
#include "Client.h"

#define USE_ADAFRUIT_MQTT 1

#ifdef USE_ADAFRUIT_MQTT
  #include <Adafruit_MQTT.h>
  #include <Adafruit_MQTT_Client.h>
  #define MQTT_CLIENT Adafruit_MQTT_Client
  #define MQTT_PUBLISHER Adafruit_MQTT_Publish
  #define MQTT_SUBSCRIBER Adafruit_MQTT_Subscribe
#endif

class MQTTPubContainer
{
  public:
    MQTTPubContainer(int key, MQTT_PUBLISHER* pPublish):key(key),pPublish(pPublish){};
    ~MQTTPubContainer(){};
    int key;
    MQTT_PUBLISHER* pPublish;
};
class MQTTSubContainer
{
  public:
    MQTTSubContainer(int key, MQTT_SUBSCRIBER* pSubscriber):key(key),pSubscriber(pSubscriber){};
    ~MQTTSubContainer(){};
    int key;
    MQTT_SUBSCRIBER* pSubscriber;
};

class MQTTManager
{
  public:
    static void initialize(const char* server, uint16_t port, const char* username, const char* password, bool bSecure=false);
    static void terminate(void);
    static MQTT_CLIENT* referClient(void);
    static MQTTManager* getInstance(void);
    void addPublisher(int key, const char *feed);
    MQTT_PUBLISHER* getPublisher(int key);
    void addSubscriber(int key, const char *feed);
    MQTT_SUBSCRIBER* getSubscriber(int key);
    void enableSubscriber(int key, bool bEnable=true);
    bool handleSubscriber(int& receivedSubscriberKey, int nTimeOut=5);
    const char* getLastSubscriberValue(int key);
    void connect(void);
    void disconnect(void);

  protected:
    MQTTManager();
    ~MQTTManager();

    static TemplateArray<MQTTPubContainer> mpMQTTPub;
    static TemplateArray<MQTTSubContainer> mpMQTTSub;

    static MQTTManager* mpThis;
    static MQTT_CLIENT* mpClient;
    static Client* mpWiFiClient;
};


#endif // __MQTTMANAGER_H__
