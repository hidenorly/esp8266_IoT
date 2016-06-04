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

#include "base.h"
#include "DHT11.h"

// static members declaration with initial value
uint16_t DHT11::mTemp = 0;
uint16_t DHT11::mHumidity = 0;
unsigned long DHT11::mLastMeasured = 0;

// singleton
DHT11* DHT11::mpThis = NULL;
bool DHT11::mInitialied = false;
bool DHT11::mTransacting = false;
int DHT11::mRefCount = 0;

DHT11::_DHT11_wait* DHT11::mWaitForStartConfition = NULL;


#define DHT11_DATA  4

DHT11::DHT11()
{
}

DHT11::~DHT11()
{
  cleanUp();
}

void DHT11::cleanUp(void)
{
  if(mpThis){
    mRefCount = 0;
    DHT11* pThis = mpThis;
    if(mWaitForStartConfition){
      delete mWaitForStartConfition;
      mWaitForStartConfition = NULL;
    }    
    mpThis = NULL;
    delete pThis;
  }
}

void DHT11::initialize(void)
{
  if(!mInitialied){
    mInitialied = true;
  }
}

DHT11* DHT11::getInstance(void)
{
  if( NULL == mpThis){
    mpThis = new DHT11();
  }
  mRefCount++;
  return mpThis;
}

void DHT11::releaseInstance(void)
{
  mRefCount--;
  if(mRefCount < 0){
    cleanUp();
  }
}


void DHT11::doRead(void)
{
  // avoid duplicate request in too short time
  unsigned long n = millis();
  if( (n - mLastMeasured) < 1000 ) return; // within 1000msec barrier
  mLastMeasured = n;

  if( !mTransacting ) {
    mTransacting = true;
    _startCodition();
  }
}

void DHT11::_startCodition(void)
{
  // step 2 start
  setOutputAndValue(DHT11_DATA, LOW);
  if( NULL == mWaitForStartConfition){
    mWaitForStartConfition = new DHT11::_DHT11_wait(DHT11_START_CONDITION_PERIOD, this, reinterpret_cast<TimerContextTicker::CALLBACK_FUNC>(_afterWaitStartCondition));
  }
  mWaitForStartConfition->registerToTimer();
}


DHT11::_DHT11_wait::_DHT11_wait(int dutyMSec, DHT11* pParent, TimerContextTicker::CALLBACK_FUNC pFunc):TimerContextTicker(pFunc, reinterpret_cast<void*>(pParent), dutyMSec)
{
}

#define DHT11_RESPONSE_TIMEOUT  160 //80
void DHT11::_afterWaitStartCondition(DHT11* pThis)
{
  // step 2 continued part.
  // This read process is timing critical (us order and then we should disable all of interrupts)
  AutoDisableInterrupt lock;
  pThis->mWaitForStartConfition->unregisterFromTimer();
  pThis->mTransacting = false;

  pinMode(DHT11_DATA, INPUT); // DATA pin will be HIGH by external pull up
  unsigned long time = micros();
  while( digitalRead(DHT11_DATA) != HIGH && (micros()<(time+DHT11_RESPONSE_TIMEOUT)) ); // wait to be HIGH by DHT11
  if( digitalRead(DHT11_DATA) != HIGH ){
    // failure to response
//    DEBUG_PRINTLN("step2 failure: waiting HIGH(pull up)");
    return;
  }
  // step 2 done

  // step 3 start
  time = micros();
  while( digitalRead(DHT11_DATA) != LOW && (micros()<(time+DHT11_RESPONSE_TIMEOUT))); // wait to be LOW by DHT11
  if( digitalRead(DHT11_DATA) != LOW ){
    // failure to response
//    DEBUG_PRINTLN("step3 failure: waiting LOW");
    return;
  }
  time = micros();
  while( digitalRead(DHT11_DATA) != HIGH && (micros()<(time+DHT11_RESPONSE_TIMEOUT)) ); // wait to be HIGH by DHT11
  if( digitalRead(DHT11_DATA) != HIGH ){
    // failure to response
//    DEBUG_PRINTLN("step3 failure: waiting HIGH");
    return;
  }
  // step 3 done

  // step 4 start
  // read humidity 16bit
  int humidity = _readWord();
  if( -1 == humidity ){
//    DEBUG_PRINTLN("step4 failure: reading humidity");
    return;
  }
  // read temperature 16bit
  int temp = _readWord();
  if( -1 == temp ){
//    DEBUG_PRINTLN("step4 failure: reading temp");
    return;
  }
  // read parity 8bit
  int parity = _readByte();
  if( -1 == parity ){
//    DEBUG_PRINTLN("step4 failure: reading parity");
    return;
  }

  // parity check
  if( (parity & 0xFF) != ( ((humidity & 0xFF) + ((humidity & 0xFF00) >> 8)) + ((temp & 0xFF) + ((temp & 0xFF00) >> 8)) ) ){
    DEBUG_PRINTLN("step4 failure: parity error");
    char buf[50];
    sprintf(buf, "%x %x %x", parity, humidity, temp);
    DEBUG_PRINTLN(buf);
    return;
  }

  mHumidity = humidity;
  mTemp = temp;
  // step 4 end
}

#define DHT11_DATA_TIMEOUT  140
#define DHT11_DATA_TIMEOUT2  140
#define DHT11_DATA_TIMEOUT3  50
// failure:-1 / success:0 or 1
int DHT11::_readBit(void)
{
  unsigned long time = micros();
  while( digitalRead(DHT11_DATA) != LOW && (micros()<(time+DHT11_DATA_TIMEOUT))); // wait to be LOW by DHT11
  if( digitalRead(DHT11_DATA) != LOW ){
    // failure to response
    DEBUG_PRINTLN("DHT11:_readBit:error waiting LOW");
    return -1;
  }
  
  time = micros();
  while( digitalRead(DHT11_DATA) != HIGH && (micros()<(time+DHT11_DATA_TIMEOUT2)) ); // wait to be HIGH by DHT11
  if( digitalRead(DHT11_DATA) != HIGH ){
    // failure to response
    DEBUG_PRINTLN("DHT11:_readBit:error waiting HIGH");
    return -1;
  }

  time = micros();
  while( digitalRead(DHT11_DATA) == HIGH && (micros()<(time+DHT11_DATA_TIMEOUT3)) ); // wait to be HIGH by DHT11
  if( digitalRead(DHT11_DATA) == HIGH ){
    return 1;
  } else {
    return 0;
  }
}

int DHT11::_readByte(void)
{
  int ret = 0;
  for(int i=7; i>=0; i--){
    int val = _readBit();
    if( -1 == val ){
      DEBUG_PRINT("DHT11::_readByte:error to read at ");
      DEBUG_PRINTLN(i);
      return -1;
    }
    ret = ret | ((val & 1) << i);
  }
  return ret;
}

int DHT11::_readWord(void)
{
  int MSB = _readByte();
  if( -1 == MSB ){
    return -1;
  }
  int LSB = _readByte();
  if( -1 == LSB ){
    return -1;
  }
  return (MSB << 8) | LSB;
}

float DHT11::getHumidity(void)
{
  doRead();
  return (float)((int)mHumidity) / 256.0f;
}

float DHT11::getTemperature(void)
{
  doRead();
  return (float)((int)mTemp) / 256.0f;
}

