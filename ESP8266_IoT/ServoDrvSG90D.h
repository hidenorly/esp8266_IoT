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

#ifndef __SERVODRV_SG90D_H__
#define __SERVODRV_SG90D_H__

#include "base.h"

#ifdef ENABLE_SERVO

#include "ServoManager.h"
#include "PWM.h"

class ServoDriverSG90D:public IServo
{
  public:
    ServoDriverSG90D(int nGPO=13, float initialAngle=0.0f);
    virtual ~ServoDriverSG90D();
    virtual void setAngle(float angle);
    virtual void enable(bool bEnable, bool bRepeat=true, int nCycles=-1);

  protected:
    PWM* mPWM;
};

#endif // ENABLE_SERVO

#endif // __SERVODRV_SG90D_H__
