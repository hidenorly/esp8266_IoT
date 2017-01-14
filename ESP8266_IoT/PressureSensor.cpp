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
#include "PressureSensor.h"

PressureSensor::PressureSensor():m_pSensorDriver(NULL)
{
}

PressureSensor::~PressureSensor()
{
}

void PressureSensor::initialize(void)
{
  if(NULL == m_pSensorDriver){
    m_pSensorDriver = BMP180::getInstance();
    m_pSensorDriver->initialize();
  }
}

void PressureSensor::uninitialize(void)
{
  if(m_pSensorDriver) m_pSensorDriver->releaseInstance();
  m_pSensorDriver = NULL;
}

int PressureSensor::getIntValue(void)
{
  return (int)m_pSensorDriver->getPressure();
}

float PressureSensor::getFloatValue(void)
{
  return m_pSensorDriver->getPressure();
}

const char* PressureSensor::getUnit(void)
{
  static const char* unit = "hPa";
  return unit;
}

const char* PressureSensor::getName(void)
{
  static const char* sensorName = "Pressure";
  return sensorName;
}

int PressureSensor::getSensorType(void)
{
  return SENSOR_PRESSURE;
}

