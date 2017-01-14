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
#include "HumiditySensor.h"

HumiditySensor::HumiditySensor():m_pSensorDriver(NULL)
{
}

HumiditySensor::~HumiditySensor()
{
}

void HumiditySensor::initialize(void)
{
  if(NULL == m_pSensorDriver){
    m_pSensorDriver = DHT11::getInstance();
    m_pSensorDriver->initialize();
  }
}

void HumiditySensor::uninitialize(void)
{
  if(m_pSensorDriver) m_pSensorDriver->releaseInstance();
  m_pSensorDriver = NULL;
}

int HumiditySensor::getIntValue(void)
{
  return (int)m_pSensorDriver->getHumidity();
}

float HumiditySensor::getFloatValue(void)
{
  return m_pSensorDriver->getHumidity();
}

const char* HumiditySensor::getUnit(void)
{
  static const char* unit = "%";
  return unit;
}

const char* HumiditySensor::getName(void)
{
  static const char* sensorName = "Humidity";
  return sensorName;
}

int HumiditySensor::getSensorType(void)
{
  return SENSOR_HUMIDITY;
}



