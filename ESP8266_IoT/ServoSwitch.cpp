/* 
 Copyright (C) 2017,2018 hidenorly

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

#ifdef ENABLE_SERVO
#ifdef ENABLE_SERVO_SWITCH

#include "ServoSwitch.h"
#include "ServoManager.h"

#include "ServoDrvSG90D.h"
#define SERVO_DRIVER ServoDriverSG90D

#define SEESAW_ANGLE0 30.0f
#define SEESAW_ANGLE1 15.0f
#define SEESAW_ANGLE_ADJUST_ON 2.0f

#define SERVO_NOT_FOUND -1

DualServoSeesawSwitch::DualServoSeesawSwitch(int nGPO1, int nGPO2)
{
  setOutputAndValue(nGPO1, LOW);
  setOutputAndValue(nGPO2, LOW);
  ServoManager* pServoManager = ServoManager::getInstance();
  if( pServoManager ){
    mServo[0] = pServoManager->addServo( new SERVO_DRIVER(nGPO1) );
    mServo[1] = pServoManager->addServo( new SERVO_DRIVER(nGPO2) );
  } else {
    mServo[0] = SERVO_NOT_FOUND;
    mServo[1] = SERVO_NOT_FOUND;
  }
}

DualServoSeesawSwitch::~DualServoSeesawSwitch()
{
  ServoManager* pServoManager = ServoManager::getInstance();
  if( pServoManager && mServo[0]!=SERVO_NOT_FOUND && mServo[1]!=SERVO_NOT_FOUND ){
    pServoManager->removeServo(mServo[0]);
    pServoManager->removeServo(mServo[1]);
  }
}

void DualServoSeesawSwitch::turnOn(bool bOn)
{
  ServoManager* pServoManager = ServoManager::getInstance();
  if( pServoManager && mServo[0]!=SERVO_NOT_FOUND && mServo[1]!=SERVO_NOT_FOUND ){
    if( bOn ){
      pServoManager->setAngle(mServo[0], SEESAW_ANGLE1);
      pServoManager->setAngle(mServo[1], -SEESAW_ANGLE0-SEESAW_ANGLE_ADJUST_ON);
    } else {
      pServoManager->setAngle(mServo[0], SEESAW_ANGLE0);
      pServoManager->setAngle(mServo[1], -SEESAW_ANGLE1);
    }
    pServoManager->enableServo(mServo[0], true, true, 3);
    pServoManager->enableServo(mServo[1], true, true, 3);
  }
  mLastOn = bOn;
}

#endif // ENABLE_SERVO_SWITCH
#endif // ENABLE_SERVO