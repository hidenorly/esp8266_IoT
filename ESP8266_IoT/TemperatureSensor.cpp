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
#include "TemperatureSensor.h"

#if ENABLE_SENSOR_TEMPERATURE

TemperatureSensor::TemperatureSensor():m_pSensorDriver(NULL)
{
}

TemperatureSensor::~TemperatureSensor()
{
}

void TemperatureSensor::initialize(void)
{
  if(NULL == m_pSensorDriver){
    m_pSensorDriver = TEMPERATURE_SENSOR_DRIVER::getInstance();
    m_pSensorDriver->initialize();
  }
}

void TemperatureSensor::uninitialize(void)
{
  if(m_pSensorDriver) m_pSensorDriver->releaseInstance();
  m_pSensorDriver = NULL;
}

int TemperatureSensor::getIntValue(void)
{
  return (int)m_pSensorDriver->getTemperature();
}

float TemperatureSensor::getFloatValue(void)
{
  return m_pSensorDriver->getTemperature();
}

const char* TemperatureSensor::getUnit(void)
{
  static const char* unit = "Celsius";
  return unit;
}

const char* TemperatureSensor::getName(void)
{
  static const char* sensorName = "Temperature";
  return sensorName;
}

int TemperatureSensor::getSensorType(void)
{
  return SENSOR_TEMPERATURE;
}


#endif // ENABLE_SENSOR_TEMPERATURE

