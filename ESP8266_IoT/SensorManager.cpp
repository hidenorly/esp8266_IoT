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

#include "SensorManager.h"
#include <Ticker.h>

// --- Sensor Data
SensorData::SensorData(float rateHz, int windowSize):mRateHz(rateHz), mWindowSize(windowSize), mCurrentPos(0), mCurrentTime( millis() )
{
  mpPoller = new Poller(1000/rateHz);
}

SensorData::~SensorData()
{
  delete mpPoller; mpPoller = NULL;
}

int SensorData::getNextDataPos(void)
{
   mCurrentPos++;
   mCurrentTime = millis();
   return mCurrentPos % mWindowSize;
}

template <typename T>
TSensorData<T>::TSensorData(T rateHz, int windowSize):SensorData(rateHz, windowSize)
{
  mpData = new T[windowSize];
}

template <typename T>
TSensorData<T>::~TSensorData()
{
  delete mpData;
  mpData = NULL;
}

template <typename T>
void TSensorData<T>::addData(T data)
{
  mpData[getNextDataPos()] = data;
}

template <typename T>
T TSensorData<T>::operator[](int nPos)
{
  if( (nPos>mCurrentPos) || (nPos<(mCurrentPos-mWindowSize)) ) return 0.0f;

  return mpData[nPos % mWindowSize];
}

template class TSensorData<int>;
template class TSensorData<float>;


// --- Sensor Data Manager
SensorManager* SensorManager::mpThis = NULL;
int SensorManager::mRefCount = 0;

template class TemplateArray<ISensor>; // here is unable to instanciate the template class. I'm not sure why. See TemplateArray.cpp
TemplateArray<ISensor> SensorManager::mpSensors(16);

SensorManager::SensorManager()
{
}

SensorManager::~SensorManager()
{
  cleanUp();
}


SensorManager* SensorManager::getInstance(void)
{
  if( NULL==mpThis ){
    mpThis = new SensorManager();
  }
  mRefCount++;
  return mpThis;
}

void SensorManager::cleanUp(void)
{
  SensorManager* pThis = mpThis;
  mpThis = NULL;
  mRefCount = 0;
  delete mpThis;
}

void SensorManager::releaseInstance(void)
{
  mRefCount--;
  if( mRefCount<= 0 ){
    cleanUp();
  }
}

void SensorManager::addSensor(ISensor* pSensor, SensorData* pData)
{
  mpSensors.add(pSensor);
//  pData->registerToTimer();
}


SensorData::Poller::Poller(int dutyMSec):TimerContextTicker(NULL, NULL, dutyMSec)
{
}

void SensorData::Poller::doCallback(void)
{
}

