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

#ifndef __ISENSOR_H__
#define __ISENSOR_H__

#include "base.h"

enum
{
  SENSOR_TEMPERATURE,
  SENSOR_PRESSURE,
  SENSOR_HUMIDITY,
  SENSOR_NOT_ASIGNED
};

class ISensor
{
  public:
    virtual ~ISensor(){};
    virtual void initialize(void){};
    virtual void uninitialize(void){};
    virtual int getIntValue(void)=0;
    virtual float getFloatValue(void)=0;
    virtual const char* getUnit(void)=0;
    virtual const char* getName(void)=0;
    virtual int getSensorType(void){return SENSOR_NOT_ASIGNED;};
};

#endif // __ISENSOR_H__

