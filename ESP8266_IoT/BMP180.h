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

#ifndef __BMP180_H__
#define __BMP180_H__

#include "base.h"

#ifdef ENABLE_SENSOR
#if ENABLE_SENSOR_PRESSURE || ENABLE_SENSOR_TEMPERATURE

#include "i2c.h"

class BMP180
{
  public:
    void initialize();

    float getPressure(void);
    float getTemperature(void);
    void doRead(void);
    static BMP180* getInstance(void);
    static void releaseInstance(void);
  
  protected:
    BMP180();
    ~BMP180();
    static void cleanUp(void);

    static const int SLA_BMP180 = 0x77;
    I2C mI2C = I2C(SLA_BMP180);

    static const int8_t oss = 0;
    static const int CONVERSION_TIME = 5; // in ultra low power mode (OSS=0)

    static const BYTE REG_CONTROL = 0xF4;
    static const BYTE REG_WORD_RESULT = 0xF6;

    static const BYTE VAL_TEMPERATURE = 0x2E;
    static const BYTE VAL_PRESSURE = 0x34;
    //const BYTE VAL_TEMPERATURE[4]= {0x34, 0x74, 0xB4, 0xF4}; // oss=0,1,2,3

    // compensation values
    static int16_t ac1, ac2, ac3;
    static uint16_t ac4, ac5, ac6;
    static int16_t b1, b2;
    static int16_t mb, mc, md; 

    // last measured value
    static int32_t mTemp;
    static int32_t mPressure;
    static unsigned long mLastMeasured;

    // singleton
    static BMP180* mpThis;
    static bool mInitialied;
    static int mRefCount;
};

#endif // ENABLE_SENSOR_PRESSURE || ENABLE_SENSOR_TEMPERATURE
#endif // ENABLE_SENSOR

#endif // __BMP180_H__
