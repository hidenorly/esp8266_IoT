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

#ifndef __SERVO_MANAGER_H__
#define __SERVO_MANAGER_H__

#include "TemplateArray.h"

class IServo
{
  public:
    IServo(int nGPO, float initialAngle=0.0f){};
    virtual ~IServo(){};
    virtual void setAngle(float angle)=0;
    virtual void enable(bool bEnable)=0;
};

class ServoManager
{
  public:
    static ServoManager* getInstance(void);
    int addServo(IServo* pServo);
    void removeServo(IServo* pServo);
    void removeServo(int nServo);
    void terminate(void);
    void setAngle(int nServo, float angle);
    void enableServo(int nServo, bool bEnable);

  protected:
    ServoManager();
    ~ServoManager();
    static void cleanUp(void);

    static TemplateArray<IServo> mpServos;
    static ServoManager* mpThis;
};

#endif // __SERVO_MANAGER_H__
