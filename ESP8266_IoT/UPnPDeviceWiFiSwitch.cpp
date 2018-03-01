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

#include "UPnPDeviceWiFiSwitch.h"

UPnPDeviceWiFiSwitch::UPnPDeviceWiFiSwitch(String deviceName, int port, String urn, String uuid, String serviceUrl):UPnPDevice(deviceName, port, urn, uuid,serviceUrl),mSwitchStatus(false)
{

}

UPnPDeviceWiFiSwitch::~UPnPDeviceWiFiSwitch()
{
}

void UPnPDeviceWiFiSwitch::_handleService(void)
{
	if(mpHttpd){
		String response = 
			"<?xml version=\"1.0\"?>"
			"<root>"
				"<device>"
					"<deviceType>urn:Belkin:device:controllee:1</deviceType>"
					"<friendlyName>"+ mDeviceName +"</friendlyName>"
					"<manufacturer>Belkin International Inc.</manufacturer>"
					"<modelName>Socket</modelName>"
					"<modelNumber>1.0</modelNumber>"
					"<modelDescription>Belkin Plugin Socket 1.0</modelDescription>\r\n"
					"<UDN>uuid:"+ mUuid +"</UDN>"
					"<serialNumber>"+mUuid+"</serialNumber>"
					"<binaryState>0</binaryState>"
					"<serviceList>"
						"<service>"
							"<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
							"<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
							"<controlURL>/upnp/control/basicevent1</controlURL>"
							"<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
							"<SCPDURL>/eventservice.xml</SCPDURL>"
						"</service>"
					"</serviceList>" 
				"</device>"
			"</root>\r\n"
			"\r\n";

		mpHttpd->send(200, "text/plain", response.c_str());
		DEBUG_PRINTLN("--- On _handleService ---");
		DEBUG_PRINTLN(response);
	}
}

void UPnPDeviceWiFiSwitch::_handleUpnpControl(void)
{
	if(mpHttpd){
		String response;
		String request = mpHttpd->arg(0);

		if(request.indexOf("SetBinaryState")!= -1) {
			bool requestStatue = true;
			if(request.indexOf("<BinaryState>1</BinaryState>")!=-1) {
				requestStatue = true;
			} else if(request.indexOf("<BinaryState>0</BinaryState>")!=-1) {
				requestStatue = false;
			}
			onSetSwitchState(requestStatue);
			response = _makeStateResponse(requestStatue);
		} else if(request.indexOf("GetBinaryState")!=-1 ) {
			response = _makeStateResponse(onGetState());
		}
		mpHttpd->send(200, "text/plain", response.c_str());

		DEBUG_PRINTLN("--- On _handleUpnpControl ---");
		DEBUG_PRINTLN(response);
	}
}

String UPnPDeviceWiFiSwitch::_makeStateResponse(bool bState)
{
	String response = 
		"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n"
			"<s:Body>\r\n"
				"<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
					"<BinaryState>" + String(bState ? "1" : "0" ) + "</BinaryState>\r\n"
				"</u:GetBinaryStateResponse>\r\n"
			"</s:Body>\r\n"
		"</s:Envelope>\r\n"
		"\r\n";

	return response;
}


void UPnPDeviceWiFiSwitch::_handleEventservice(void)
{
	if(mpHttpd){
		const static char response[] PROGMEM = 
			"<scpd xmlns=\"urn:Belkin:service-1-0\">"
				"<actionList>"
					"<action>"
						"<name>SetBinaryState</name>"
						"<argumentList>"
							"<argument>"
								"<retval/>"
								"<name>BinaryState</name>"
								"<relatedStateVariable>BinaryState</relatedStateVariable>"
								"<direction>in</direction>"
							"</argument>"
						"</argumentList>"
					"</action>"
					"<action>"
						"<name>GetBinaryState</name>"
						"<argumentList>"
							"<argument>"
								"<retval/>"
								"<name>BinaryState</name>"
								"<relatedStateVariable>BinaryState</relatedStateVariable>"
								"<direction>out</direction>"
							"</argument>"
						"</argumentList>"
					"</action>"
				"</actionList>"
				"<serviceStateTable>"
					"<stateVariable sendEvents=\"yes\">"
						"<name>BinaryState</name>"
						"<dataType>Boolean</dataType>"
						"<defaultValue>0</defaultValue>"
					"</stateVariable>"
					"<stateVariable sendEvents=\"yes\">"
						"<name>level</name>"
						"<dataType>string</dataType>"
						"<defaultValue>0</defaultValue>"
					"</stateVariable>"
				"</serviceStateTable>"
			"</scpd>\r\n"
			"\r\n";

		mpHttpd->send(200, "text/plain", response);

		DEBUG_PRINTLN("--- On _handleEventservice ---");
		DEBUG_PRINTLN(response);
	}
}

void UPnPDeviceWiFiSwitch::onSetSwitchState(bool bState)
{
	DEBUG_PRINT("Request state is ");
	DEBUG_PRINT(String(bState));
	DEBUG_PRINTLN("");
	mSwitchStatus = bState ? true : false;
}

bool UPnPDeviceWiFiSwitch::onGetState(void)
{
	return mSwitchStatus;
};

