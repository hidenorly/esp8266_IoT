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
#include "NtpUtil.h"
#include <Ticker.h>
#include <NTP.h>

extern const char* NTP_SERVER;

Ticker g_NtpStatusTracker;
void checkNtpStatus(CTrackerParam*);
void setupNtpStatusTracker(){
  static int bInitialized = false;
  if( !bInitialized ) {
    g_NtpStatusTracker.attach_ms<CTrackerParam*>(500, checkNtpStatus, NULL);
  }
}

void checkNtpStatus(CTrackerParam* p){
  time_t n = now();
  if( year(n) == 1970 ) {
    // NTP is not succeeded
    static int bInitialized1 = false;
    if( !bInitialized1 ){
      setSyncInterval(10);
      bInitialized1 = true;
    }
  } else {
    // after successful to set NTPed time on system
    static int bInitialized2 = false;
    if( !bInitialized2 ){
      setSyncInterval(300);
      bInitialized2 = true;
    }
  }
}

void start_NTP(){
  static int bInitialized=false;
  if(!bInitialized){
    setTimeServer(NTP_SERVER);
    ntp_begin(2390);
    bInitialized = true;
  }
  setupNtpStatusTracker();
}