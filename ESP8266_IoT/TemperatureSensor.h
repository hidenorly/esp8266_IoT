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

#ifndef __TEMPERATURE_SENSOR_H__
#define __TEMPERATURE_SENSOR_H__

#include "config.h"

#ifdef ENABLE_SENSOR
#if ENABLE_SENSOR_TEMPERATURE

#include "SensorBase.h"
#include "BMP180.h"
#include "DHT11.h"

class TemperatureSensor:public ISensor
{
  public:
    TemperatureSensor();
    ~TemperatureSensor();
    virtual void initialize(void);
    virtual void uninitialize(void);
    virtual int getIntValue(void);
    virtual float getFloatValue(void);
    virtual const char* getUnit(void);
    virtual const char* getName(void);
    virtual int getSensorType(void);

  protected:
    TEMPERATURE_SENSOR_DRIVER* m_pSensorDriver;
};

#endif // ENABLE_SENSOR_TEMPERATURE
#endif // ENABLE_SENSOR

#endif // __TEMPERATURE_SENSOR_H__
