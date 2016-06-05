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
#include "ServoManager.h"

// --- Servo Data Manager
ServoManager* ServoManager::mpThis = NULL;

template class TemplateArray<IServo>;
TemplateArray<IServo> ServoManager::mpServos(8);

ServoManager::ServoManager()
{
}

ServoManager::~ServoManager()
{
  cleanUp();
}


ServoManager* ServoManager::getInstance(void)
{
  if( NULL==mpThis ){
    mpThis = new ServoManager();
  }
  return mpThis;
}

void ServoManager::cleanUp(void)
{
  ServoManager* pThis = mpThis;
  mpThis = NULL;
  delete mpThis;
}

void ServoManager::terminate(void)
{
  cleanUp();
}

int ServoManager::addServo(IServo* pServo)
{
  mpServos.add(pServo);

  for(int i=0, c=mpServos.size(); i<c; i++){
    IServo* aServo = mpServos.getPtr(i);
    if( aServo == pServo ){
      return i;
    }
  }
  return -1;
}

void ServoManager::removeServo(IServo* pServo)
{
  mpServos.remove(pServo);
}

void ServoManager::removeServo(int nServo)
{
  IServo* pServo = mpServos.getPtr(nServo);
  mpServos.remove(pServo);
}

void ServoManager::setAngle(int nServo, float angle)
{
#if 0
  DEBUG_PRINT("ServoManager::setAngle:");
  DEBUG_PRINT(nServo);
  DEBUG_PRINT(":");
  DEBUG_PRINTLN(angle);
#endif
  IServo* pServo = mpServos.getPtr(nServo);
  if( pServo ){
    pServo->setAngle(angle);
  }
}

