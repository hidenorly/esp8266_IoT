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

#include "MQTTManager.h"
#include <ESP8266WiFi.h>

extern template class TemplateArray<MQTTPubContainer>; // here is unable to instanciate the template class. I'm not sure why. See TemplateArray.cpp
TemplateArray<MQTTPubContainer> MQTTManager::mpMQTTPub(8);
TemplateArray<MQTTSubContainer> MQTTManager::mpMQTTSub(8);
MQTTManager* MQTTManager::mpThis = NULL;
MQTT_CLIENT* MQTTManager::mpClient = NULL;
Client* MQTTManager::mpWiFiClient = NULL;


MQTTManager::MQTTManager()
{
}

MQTTManager::~MQTTManager()
{
  terminate();
}

void MQTTManager::initialize(const char* server, uint16_t port, const char* username, const char* password, bool bSecure)
{
  if( mpWiFiClient ){
    delete mpWiFiClient;
  }
  if( bSecure ){
    mpWiFiClient = new WiFiClientSecure();
  } else {
    mpWiFiClient = new WiFiClient();
  }

  if( mpWiFiClient ){
    if( mpClient ){
      delete mpClient;
    }
#if USE_ADAFRUIT_MQTT
    mpClient = new Adafruit_MQTT_Client(mpWiFiClient, server, port, username, password);
#endif // USE_ADAFRUIT_MQTT
  }
}

MQTT_CLIENT* MQTTManager::referClient(void)
{
  return mpClient;
}

MQTTManager* MQTTManager::getInstance(void)
{
  if( NULL==mpThis ){
    mpThis = new MQTTManager();
  }
  return mpThis;
}

void MQTTManager::terminate(void)
{
  // clean up publisher instance
  for(int i=0; i<mpMQTTPub.size(); i++){
    MQTTPubContainer* pData = mpMQTTPub.getPtr(i);
    if( pData && pData->pPublish ){
      delete pData->pPublish;
      pData->pPublish=NULL;
      // pData itself is removed by TemplateArray's destructor
    }
  }
  // clean up subscriber instance
  for(int i=0; i<mpMQTTSub.size(); i++){
    MQTTSubContainer* pData = mpMQTTSub.getPtr(i);
    if( pData && pData->pSubscriber ){
      delete pData->pSubscriber;
      pData->pSubscriber=NULL;
      // pData itself is removed by TemplateArray's destructor
    }
  }

  delete mpClient; mpClient = NULL;
  delete mpWiFiClient; mpWiFiClient = NULL;

  MQTTManager* pThis = mpThis;
  mpThis = NULL;
  delete mpThis;
}

void MQTTManager::addPublisher(int key, const char *feed)
{
#if USE_ADAFRUIT_MQTT
  if( mpClient) {
    mpMQTTPub.add(new MQTTPubContainer(key, new MQTT_PUBLISHER(mpClient, feed)));
  }
#endif // USE_ADAFRUIT_MQTT 
}


MQTT_PUBLISHER* MQTTManager::getPublisher(int key)
{
  for(int i=0; i<mpMQTTPub.size(); i++){
  	MQTTPubContainer* pData = mpMQTTPub.getPtr(i);
  	if( pData && pData->key == key){
  		return pData->pPublish;
  	}
  }
  return NULL;
}

void MQTTManager::addSubscriber(int key, const char *feed)
{
#if USE_ADAFRUIT_MQTT
  if( mpClient) {
    mpMQTTSub.add(new MQTTSubContainer(key, new MQTT_SUBSCRIBER(mpClient, feed)));
  }
#endif // USE_ADAFRUIT_MQTT 
}


MQTT_SUBSCRIBER* MQTTManager::getSubscriber(int key)
{
  for(int i=0; i<mpMQTTSub.size(); i++){
    MQTTSubContainer* pData = mpMQTTSub.getPtr(i);
    if( pData && pData->key == key){
      return pData->pSubscriber;
    }
  }
  return NULL;
}

void MQTTManager::enableSubscriber(int key, bool bEnable)
{
  if( mpClient ) {
    MQTT_SUBSCRIBER* pSubscribe = getSubscriber(key);
    if( pSubscribe ){
#if USE_ADAFRUIT_MQTT
      if( bEnable ){
        mpClient->subscribe(pSubscribe);
      } else {
        mpClient->unsubscribe(pSubscribe);
      }
#endif // USE_ADAFRUIT_MQTT
    }
  }
}

bool MQTTManager::handleSubscriber(int& receivedSubscriberKey, int nTimeOut)
{
  if( mpClient ) {
#if USE_ADAFRUIT_MQTT
    MQTT_SUBSCRIBER* pSubNow;
    while( pSubNow = mpClient->readSubscription(nTimeOut) ) {
      for(int i=0; i<mpMQTTSub.size(); i++){
        MQTTSubContainer* pData = mpMQTTSub.getPtr(i);
        if( pData && pData->pSubscriber == pSubNow){
          receivedSubscriberKey = i;
          return true;
        }
      }
    }
#endif
  }
  return false;
}

const char* MQTTManager::getLastSubscriberValue(int key)
{
  if( mpClient ) {
#if USE_ADAFRUIT_MQTT
    for(int i=0; i<mpMQTTSub.size(); i++){
      MQTTSubContainer* pData = mpMQTTSub.getPtr(i);
      if( pData && pData->key == key){
        return (const char*)pData->pSubscriber->lastread;
      }
    }
#endif  // USE_ADAFRUIT_MQTT
  }
  return NULL;
}



void MQTTManager::connect(void)
{
  if( mpClient ) {
#if USE_ADAFRUIT_MQTT
    mpClient->connect();
#endif // USE_ADAFRUIT_MQTT
  }
}

void MQTTManager::disconnect(void)
{
  if( mpClient ) {
#if USE_ADAFRUIT_MQTT
    mpClient->disconnect();
#endif // USE_ADAFRUIT_MQTT
  }
}

