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

extern template class TemplateArray<MQTTContainer>; // here is unable to instanciate the template class. I'm not sure why. See TemplateArray.cpp
TemplateArray<MQTTContainer> MQTTManager::mpMQTT(16);
MQTTManager* MQTTManager::mpThis=NULL;
int MQTTManager::mRefCount=0;


MQTTManager::MQTTManager()
{
}

MQTTManager::~MQTTManager()
{
  cleanUp();
}


MQTTManager* MQTTManager::getInstance(void)
{
  if( NULL==mpThis ){
    mpThis = new MQTTManager();
  }
  mRefCount++;
  return mpThis;
}

void MQTTManager::cleanUp(void)
{
  // clean up publisher instance
  for(int i=0; i<mpMQTT.size(); i++){
    MQTTContainer* pData = mpMQTT.getPtr(i);
    if( pData && pData->pPublish ){
      delete pData->pPublish;
      pData->pPublish=NULL;
      // pData itself is removed by TemplateArray's destructor
    }
  }

  MQTTManager* pThis = mpThis;
  mpThis = NULL;
  mRefCount = 0;
  delete mpThis;
}

void MQTTManager::releaseInstance(void)
{
  mRefCount--;
  if( mRefCount<= 0 ){
    cleanUp();
  }
}

void MQTTManager::addPublisher(int key, MQTT_PUBLISHER* pPublish)
{
  mpMQTT.add(new MQTTContainer(key, pPublish));
}

MQTT_PUBLISHER* MQTTManager::getPublisher(int key)
{
  for(int i=0; i<mpMQTT.size(); i++){
  	MQTTContainer* pData = mpMQTT.getPtr(i);
  	if( pData && pData->key == key){
  		return pData->pPublish;
  	}
  }
  return NULL;
}

