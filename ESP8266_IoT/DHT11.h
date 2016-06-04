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

#ifndef __DHT11_H__
#define __DHT11_H__

#include "LooperThreadTicker.h"

#define DHT11_START_CONDITION_PERIOD  20  // Specification says more than 18msec. 

class DHT11
{
  public:
    void initialize();

    float getHumidity(void);
    float getTemperature(void);
    void doRead(void);
    static DHT11* getInstance(void);
    static void releaseInstance(void);
  
  protected:
    DHT11();
    ~DHT11();
    static void cleanUp(void);
    void _startCodition(void);
    class _DHT11_wait:public TimerContextTicker
    {
      public:
        _DHT11_wait(int dutyMSec=DHT11_START_CONDITION_PERIOD, DHT11* mParent=NULL, TimerContextTicker::CALLBACK_FUNC pFunc=NULL);
    };

    static _DHT11_wait* mWaitForStartConfition;
    static void _afterWaitStartCondition(DHT11* pThis);
    static int _readBit(void);
    static int _readByte(void);
    static int _readWord(void);
    
    // last measured value
    static uint16_t mTemp;
    static uint16_t mHumidity;
    static unsigned long mLastMeasured;

    // singleton
    static DHT11* mpThis;
    static bool mInitialied;
    static bool mTransacting;
    static int mRefCount;
};

#endif // __DHT11_H__
