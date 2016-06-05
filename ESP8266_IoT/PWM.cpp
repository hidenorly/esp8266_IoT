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
#include "PWM.h"
#include "limits.h"

PWM::PWM(int nGPO, int nCycleMSec, float dutyPercent):mGPO(nGPO),mCycleMSec(nCycleMSec)
{
  setOutputAndValue(nGPO, LOW);
  updateDutyMicroSec(dutyPercent);
  PWMManager* pManager = PWMManager::getInstance();
  pManager->addPWM(this);
}

PWM::~PWM()
{
  PWMManager* pManager = PWMManager::getInstance();
  pManager->removePWM(this);
}

void PWM::setDuty(float dutyPercent)
{
  updateDutyMicroSec(dutyPercent);
#if 0
  DEBUG_PRINT("PWM::setDuty(");
  DEBUG_PRINT(dutyPercent);
  DEBUG_PRINT(") on ");
  DEBUG_PRINT(mGPO);
  DEBUG_PRINT(mDutyMicroSec/1000.0f);
  DEBUG_PRINT(" / ");
  DEBUG_PRINTLN(mCycleMSec);
#endif
}

void PWM::setDuty(int dutyPecent)
{
  setDuty((float)dutyPecent);
}

int PWM::getCycleMSec(void)
{
  return mCycleMSec;
}

float PWM::getDuty(void)
{
  return mDutyMicroSec/10.0f/mCycleMSec;
}

int PWM::getPort(void)
{
  return mGPO;
}

int PWM::getDutyMicroSec(void)
{
  return mDutyMicroSec; 
}

void PWM::updateDutyMicroSec(float dutyPercent)
{
  mDutyMicroSec = (mCycleMSec * dutyPercent * 10);
}


// --- PWM Data Manager
PWMManager* PWMManager::mpThis = NULL;

template class TemplateArray<PWM>;
TemplateArray<PWM> PWMManager::mpPWMs(8);

PWMManager::PWMManager():mpPoller(NULL)
{
}

PWMManager::~PWMManager()
{
  cleanUp();
}


PWMManager* PWMManager::getInstance(void)
{
  if( NULL==mpThis ){
    mpThis = new PWMManager();
  }
  return mpThis;
}

void PWMManager::cleanUp(void)
{
  PWMManager* pThis = mpThis;
  mpThis = NULL;
  delete mpThis;
}

void PWMManager::terminate(void)
{
  cleanUp();
}

void PWMManager::addPWM(PWM* pPWM)
{
  mpPWMs.add(pPWM);
  setPWMCycle();
}

void PWMManager::removePWM(PWM* pPWM)
{
  mpPWMs.remove(pPWM);
}

int PWMManager::getOptimalCycle(void)
{
  int ret = INT_MAX;

  // TODO: improvement is required, such as GCD method etc.
  for(int i=0, c=mpPWMs.size(); i<c; i++){
    PWM* pPWM = mpPWMs.getPtr(i);
    if( NULL != pPWM ){
      int nCycle = pPWM->getCycleMSec();
      if( nCycle < ret ){
        ret = nCycle;
      }
    }
  }
  
  return ret;
}

void PWMManager::setPWMCycle(void)
{
  int cycle = getOptimalCycle();
  bool bCreate = false;
  if( NULL == mpPoller ){
    mpPoller = new Poller(cycle, this);
    bCreate = true;
  } else if( mpPoller->getDutyMSec() != cycle ){
    mpPoller->unregisterFromTimer();
    delete mpPoller;
    mpPoller = new Poller(cycle, this);
    bCreate = true;
  }
  if(bCreate){
    mpPoller->registerToTimer();
  }
}

PWMManager::Poller::Poller(int dutyMSec, PWMManager* pThis):TimerContextTicker(NULL, reinterpret_cast<void*>(pThis), dutyMSec)
{
}

void PWMManager::Poller::doCallback(void)
{
  if( mpArg != NULL){
    PWMManager* pManager = reinterpret_cast<PWMManager*>(mpArg);
    pManager->tick();
  }
}

void PWMManager::tick(void)
{
  if( mpPoller == NULL ) return;
  
  unsigned long start_time = micros();
  int cycle = mpPoller->getDutyMSec();

  // set HIGH first
  int nextDutyMicroSec = INT_MAX;
  for(int i=0, c=mpPWMs.size(); i<c; i++){
    PWM* pPWM = mpPWMs.getPtr(i);
    if( NULL != pPWM ){
      digitalWrite(pPWM->getPort(), HIGH);
      int candidate = pPWM->getDutyMicroSec();
      if( candidate < nextDutyMicroSec ){
        nextDutyMicroSec = candidate;
      }
    }
  }
  
  // TODO: wait with ticker if the next is more than 1 msec (nextDutyMicroSec > 1000), etc.
  // Otherwise, if we set the duty=100%, always MCU will be occupied by this loop.
  {
    AutoDisableInterrupt();
    int nPWM = mpPWMs.size();
    bool bFound=false;
    do {
      bFound=false;
      unsigned long nowDelta = micros() - start_time;
      for(int i=0; i<nPWM; i++){
        PWM* pPWM = mpPWMs.getPtr(i);
        if( NULL != pPWM ){
          if( pPWM->getDutyMicroSec() < nowDelta ){
            digitalWrite(pPWM->getPort(), LOW);
/*          
            DEBUG_PRINT("PWM:");
            DEBUG_PRINT(pPWM->getDutyMicroSec());
            DEBUG_PRINT("/");
            DEBUG_PRINT(nowDelta);
            DEBUG_PRINT(":");
            DEBUG_PRINT(pPWM->getPort());
            DEBUG_PRINTLN(":LOW");
*/
          } else {
            bFound = true;
          }
        }
      }
    } while(bFound);
  }
}


