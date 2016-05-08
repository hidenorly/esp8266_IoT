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
#include "LooperThreadTicker.h"

extern "C" {
#include "ets_sys.h"
#include "osapi.h"
}

// TODO: I'm not sure why but I cannot instanciate template class here. But TemplateArray.cpp is Ok...
//template class TemplateArray<LooperThreadTicker>;

// Global LooperThreadManager;
LooperThreadManager g_LooperThreadManager; // g_LooperThreadManager(20); // Specify max thread count if you need (default:10)


LooperThreadManager::LooperThreadManager(int nMaxTicker)
{
}

LooperThreadManager::~LooperThreadManager()
{
  for(int i=0, c=size(); i<c; i++){
    LooperThreadTicker* pTicker = getPtr(i);
    if( NULL != pTicker ){
      pTicker->unregisterFromTimer();
      delete pTicker;
    }
  }
}

void LooperThreadManager::handleLooperThread()
{
  for(int i=0, c=size(); i<c; i++){
    LooperThreadTicker* pTicker = getPtr(i);
    if( pTicker && pTicker->getActive() ){
      pTicker->doCallback();
    }
  }
}

void LooperThreadManager::add(LooperThreadTicker* pTicker)
{
  if( pTicker ){
    TickerArray::add(pTicker);
    pTicker->registerToTimer();
  }
}


LooperThreadTicker::LooperThreadTicker(CALLBACK_FUNC pFunc, void* pArg, int dutyMSec)
{
  mpFunc = pFunc;
  mpArg = pArg;
  mDutySec = dutyMSec;
  mFlagActivated = false;

  mpETSTimer = NULL;
}

LooperThreadTicker::~LooperThreadTicker()
{
  unregisterFromTimer();

  mpFunc = NULL;
  mpArg = NULL;
  mDutySec = 0;
  mFlagActivated = false;
}


void LooperThreadTicker::setActive(int bActive)
{
  mFlagActivated = bActive;  
}

int LooperThreadTicker::getActive()
{
  return mFlagActivated;
}

void LooperThreadTicker::doCallback(void)
{
  mFlagActivated = false;
  mpFunc(mpArg);
}

void LooperThreadTicker::registerToTimer(void)
{
  if( NULL == mpETSTimer ) {
    mpETSTimer = new ETSTimer;
  } else {
    os_timer_disarm(mpETSTimer);
  }
  os_timer_setfn(mpETSTimer, reinterpret_cast<ETSTimerFunc*>(_timerCallback), reinterpret_cast<void*>(this));
  os_timer_arm(mpETSTimer, mDutySec, 1);  // 1 means repeat
}

void LooperThreadTicker::unregisterFromTimer(void)
{
  if( NULL != mpETSTimer ) {
    os_timer_disarm(mpETSTimer);
    delete mpETSTimer; mpETSTimer=NULL;
  }
}

void LooperThreadTicker::_timerCallback(void* pLooperThreadTicker)
{
  // set just flag to callback from looper()
  (reinterpret_cast<LooperThreadTicker*>(pLooperThreadTicker))->setActive(true);
}
