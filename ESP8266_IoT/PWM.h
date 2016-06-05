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

#ifndef __PWM_H__
#define __PWM_H__

#include "TemplateArray.h"
#include "LooperThreadTicker.h"

class PWM
{
  public:
    PWM(int nGPO, int nCycleMSec, float dutyPercent=0.0f);
    ~PWM();
    void setDuty(float dutyPercent);
    void setDuty(int dutyPecent);
    int getCycleMSec(void);
    float getDuty(void);
    int getPort(void);
    int getDutyMicroSec(void);
    
  protected:
    void updateDutyMicroSec(float dutyPercent);
    int mGPO;
    int mCycleMSec;
    int mDutyMicroSec;
};

class PWMManager
{
  public:
    static PWMManager* getInstance(void);
    void addPWM(PWM* pPWM);
    void removePWM(PWM* pPWM);
    void terminate(void);

  protected:
    PWMManager();
    ~PWMManager();
    static void cleanUp(void);

    static TemplateArray<PWM> mpPWMs;
    static PWMManager* mpThis;

    class Poller:public TimerContextTicker
    {
      public:
        Poller(int dutyMSec=20, PWMManager* pThis=NULL);
      protected:
        virtual void doCallback(void);
    };
    Poller* mpPoller;

  int getOptimalCycle(void);
  void setPWMCycle(void);
public:
  void tick(void);
};
#endif // __PWM_H__
