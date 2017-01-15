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

#include "ServoDrvSG90D.h"

ServoDriverSG90D::ServoDriverSG90D(int nGPO, float initialAngle):IServo(nGPO, initialAngle)
{
  mPWM = new PWM(nGPO, 20, 0.0f);
  setAngle(initialAngle);
}

ServoDriverSG90D::~ServoDriverSG90D()
{
  delete mPWM;
  mPWM=NULL;
}

void ServoDriverSG90D::setAngle(float angle)
{
  if( angle < -90.0f ) angle = -90.0f;
  if( angle > 90.0f ) angle = 90.0f;

#if 0
  char buf[50];
  sprintf(buf, "Servo:%x:", this);
  DEBUG_PRINT(buf);
  DEBUG_PRINTLN(angle);
#endif

  mPWM->setDuty( 100.0f * ((1.45f + (2.4f-1.45f)*angle/90.0f)/mPWM->getCycleMSec()) );
//  mPWM->setDuty( 100.0f * ((1.5f + (2.0f-1.5f)*angle/90.0f)/mPWM->getCycleMSec()) );
}

void ServoDriverSG90D::enable(bool bEnable, bool bRepeat, int nCycles)
{
  mPWM->setEnableOutput(bEnable, bRepeat, true, nCycles);
}

