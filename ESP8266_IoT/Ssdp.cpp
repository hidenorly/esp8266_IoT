/* 
 Copyright (C) 2018 hidenorly

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
#include "Ssdp.h"
#include <ESP8266WiFi.h>
#include "StringUtil.h"

bool Ssdp::mEnable = false;
UPnPDeviceArray* Ssdp::mpUPnPDevices = NULL;
WiFiUDP* Ssdp::mpUDP = NULL;

#define SSDP_MULTICAST_ADDRESS IPAddress(239, 255, 255, 250)
#define SSDP_PORT 1900
#define SSDP_PACKET_BUF 512

#define ENABLE_SSDP_DEBUG 0

Ssdp::Ssdp()
{
	clearAll();
}

Ssdp::~Ssdp()
{
	clearAll(false);
	if( mpUDP ){
		delete mpUDP;
		mpUDP = NULL;
	}
}

bool Ssdp::isCheckSsdpPacket(char* buf, int nSize)
{
	bool result = false;

	result = (StringUtil::inInclude(buf, nSize, "M-SEARCH")) ||
		(StringUtil::inInclude(buf, nSize, "MAN: \"ssdp:discover\"")) ||
		(StringUtil::inInclude(buf, nSize, "ssdp:all")) ||
		(StringUtil::inInclude(buf, nSize, "upnp:rootdevice"));

	return result;
}

void Ssdp::handle(void)
{
	if(mEnable && mpUDP){
		int nSize = mpUDP->parsePacket();
		if( nSize>0 ){
			char buf[SSDP_PACKET_BUF];
			int nRead = mpUDP->read(buf, SSDP_PACKET_BUF-1);
			if(nRead<SSDP_PACKET_BUF){
				buf[nRead]='\0';
			}
			// TODO: do remaing data & loop
			if(isCheckSsdpPacket(buf, nRead)){
				#if ENABLE_SSDP_DEBUG
				DEBUG_PRINTLN("---ssdp received message---");
				DEBUG_PRINTLN(buf);
				#endif /* ENABLE_SSDP_DEBUG */
				IPAddress senderIP = mpUDP->remoteIP();
				unsigned int senderPort = mpUDP->remotePort();

				if(mpUPnPDevices){
					for(int i=0; i<mpUPnPDevices->size(); i++){
						UPnPDevice* pDevice = mpUPnPDevices->getPtr(i);
						if(pDevice){
							String response = pDevice->handleMSearch(buf);
							#if ENABLE_SSDP_DEBUG
							DEBUG_PRINTLN("---ssdp response message---");
							DEBUG_PRINTLN(response);
							#endif /* ENABLE_SSDP_DEBUG */
							mpUDP->beginPacket(senderIP, senderPort);
							mpUDP->write(response.c_str());
							mpUDP->endPacket();
						}
					}
				}
			}
		}
	}
}

void Ssdp::enable(bool bEnable)
{
	if( bEnable!=mEnable){
		if( bEnable ){
			// enable
			if(!mpUDP){
				mpUDP = new WiFiUDP();
			}
			if(mpUDP){
				mEnable = (0 != 
					mpUDP->beginMulticast(
						WiFi.localIP(), 
						SSDP_MULTICAST_ADDRESS,
						SSDP_PORT
					)
				);
				if(!mEnable){
					DEBUG_PRINTLN("Failed to open UDP");
				}
			}
		} else {
			// disable
			if(mpUDP){
				mpUDP->stop();
				delete mpUDP;
				mpUDP = NULL;
			}
		}
	}
}

bool Ssdp::getEnabled(void)
{
	return mEnable;
}

void Ssdp::clearAll(bool bReCreate, int nMaxDevice)
{
	if(mpUPnPDevices){
		delete mpUPnPDevices;
		mpUPnPDevices = NULL;
	}
	if(bReCreate){
		mpUPnPDevices = new UPnPDeviceArray(nMaxDevice);
	}
}

void Ssdp::addUPnPDevice(UPnPDevice* pDevice)
{
	if(mpUPnPDevices){
		mpUPnPDevices->add(pDevice);
	}
}

void Ssdp::removeUPnPDevice(UPnPDevice* pDevice)
{
	if(mpUPnPDevices){
		mpUPnPDevices->remove(pDevice);
	}
}