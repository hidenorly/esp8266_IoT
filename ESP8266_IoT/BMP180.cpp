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
#include "BMP180.h"

// static members declaration with initial value
int16_t BMP180::ac1=0, BMP180::ac2=0, BMP180::ac3=0;
uint16_t BMP180::ac4=0, BMP180::ac5=0, BMP180::ac6=0;
int16_t BMP180::b1=0, BMP180::b2=0;
int16_t BMP180::mb=0, BMP180::mc=0, BMP180::md=0; 

int32_t BMP180::mTemp = 0;
int32_t BMP180::mPressure = 0;
unsigned long BMP180::mLastMeasured = 0;

// singleton
BMP180* BMP180::mpThis = NULL;
bool BMP180::mInitialied = false;
int BMP180::mRefCount = 0;

BMP180::BMP180() // : mI2C(SLA_BMP180)
{
}

BMP180::~BMP180()
{
  cleanUp();
}

void BMP180::cleanUp(void)
{
  if(mpThis){
    mRefCount = 0;
    BMP180* pThis = mpThis;
    mpThis = NULL;
    delete pThis;
  }
}

void BMP180::initialize(void)
{
  if(!mInitialied){
    mInitialied = true;
    mI2C.init();
    ac1 = (int16_t)mI2C.readWord(0xAA); //DEBUG_PRINT("ac1="); DEBUG_PRINTLN(ac1);
    ac2 = (int16_t)mI2C.readWord(0xAC); //DEBUG_PRINT("ac2="); DEBUG_PRINTLN(ac2);
    ac3 = (int16_t)mI2C.readWord(0xAE); //DEBUG_PRINT("ac3="); DEBUG_PRINTLN(ac3);
    ac4 = mI2C.readWord(0xB0);          //DEBUG_PRINT("ac4="); DEBUG_PRINTLN(ac4);
    ac5 = mI2C.readWord(0xB2);          //DEBUG_PRINT("ac5="); DEBUG_PRINTLN(ac5);
    ac6 = mI2C.readWord(0xB4);          //DEBUG_PRINT("ac6="); DEBUG_PRINTLN(ac6);
    b1 =  (int16_t)mI2C.readWord(0xB6); //DEBUG_PRINT("b1="); DEBUG_PRINTLN(b1);
    b2 =  (int16_t)mI2C.readWord(0xB8); //DEBUG_PRINT("b2="); DEBUG_PRINTLN(b2);
    mb =  (int16_t)mI2C.readWord(0xBA); //DEBUG_PRINT("mb="); DEBUG_PRINTLN(mb);
    mc =  (int16_t)mI2C.readWord(0xBC); //DEBUG_PRINT("mc="); DEBUG_PRINTLN(mc);
    md =  (int16_t)mI2C.readWord(0xBE); //DEBUG_PRINT("md="); DEBUG_PRINTLN(md);
  }
}

BMP180* BMP180::getInstance(void)
{
  if( NULL == mpThis){
    mpThis = new BMP180();
  }
  mRefCount++;
  return mpThis;
}

void BMP180::releaseInstance(void)
{
  mRefCount--;
  if(mRefCount < 0){
    cleanUp();
  }
}


void BMP180::doRead(void)
{
  // avoid duplicate request in too short time
  unsigned long n = millis();
  if( (n - mLastMeasured) < (CONVERSION_TIME*(2+oss*2))) return; // within 5msec barrier
  mLastMeasured = n;
  
  // read uncompensated temperature value
  mI2C.writeByte(REG_CONTROL, VAL_TEMPERATURE);
  delay(CONVERSION_TIME);
  int32_t UT = (int32_t)mI2C.readWord(REG_WORD_RESULT);

  // read uncompensated pressure value
  mI2C.writeByte(REG_CONTROL, VAL_PRESSURE + (oss << 6));
  delay(CONVERSION_TIME*(oss*2+1));
  int32_t UP = 0;
  mI2C.read(REG_WORD_RESULT, (BYTE*)&UP, 3);
  UP = UP >> (8-oss);

  // Caluculate true temperature
  int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
  int32_t X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md); 
  int32_t B5 = X1 + X2;

  mTemp  = (B5+8) >> 4;

  // Caluculate true pressure
  int32_t B6 = B5 - 4000;
  X1 = ((int32_t)b2 * ((B6 * B6) >> 12)) >> 11;
  X2 = ((int32_t)ac2 * B6) >> 11;
  int32_t X3 = X1 + X2;
  int32_t B3 = (((int32_t)ac1 * 4 + X3) + 2) / 4;
  X1 = ((int32_t)ac3 * B6) >> 13;
  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  uint32_t B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  uint32_t B7 = ((uint32_t)UP - B3) * (uint32_t)(50000UL);
  
 if (B7 < 0x80000000) {
    mPressure = (B7 * 2) / B4;
  } else {
    mPressure = (B7 / B4) * 2;
  }

  X1 = (mPressure >> 8) * (mPressure >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * mPressure) >> 16;
  mPressure = mPressure + ((X1 + X2 + (int32_t)3791) >> 4);
}

float BMP180::getPressure(void)
{
  doRead();
  return (float) mPressure / 100.0f;
}

float BMP180::getTemperature(void)
{
  doRead();
  return (float) mTemp / 10.0f;
}

