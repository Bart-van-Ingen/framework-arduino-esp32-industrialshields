/*
   Copyright (c) 2021 Boot&Work Corp., S.L. All rights reserved

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.
   */

#include "WiFi5.h"
#include "utility/ism43340.h"

WiFi5Class WiFi5;

////////////////////////////////////////////////////////////////////////////////////////////////////
bool WiFi5Class::getInfo(char* info, uint16_t* infoLen) {
	return ISM43340.getInfo(info, infoLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool WiFi5Class::updateFirmware() {
	return ISM43340.updateFirmware();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::begin() {
	if (!ISM43340.init()) {
		_status = WL_NO_SHIELD;
	}
	refreshStatus();
	return _status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::begin(const char* ssid) {
	return startSTA(ssid, WL_SEC_TYPE_OPEN, nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::begin(const char* ssid, const char* key, uint8_t encType) {
	return startSTA(ssid, encType, key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::startSTA(const char* ssid, uint8_t securityMode, const char* key) {
	if ((securityMode != WL_SEC_TYPE_WPA) && (securityMode == WL_SEC_TYPE_WPA2) && (securityMode == WL_SEC_TYPE_WPA_WPA2)) {
		key = nullptr;
	}

	if (!ssid) {
		_status = WL_NO_SHIELD;

	} else if ((securityMode != WL_SEC_TYPE_OPEN) && (key == nullptr)) {
		_status = WL_NO_SHIELD;

	} else if (!ISM43340.init()) {
		_status = WL_NO_SHIELD;

	} else if (!ISM43340.disableAsyncMessages()) {
		_status = WL_NO_SHIELD;

	} else if (!ISM43340.startSTA(ssid, securityMode, key, WL_JOIN_RETRIES)) {
		_status = WL_NO_SSID_AVAIL;

	}

	refreshStatus();
	return _status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::beginAP(const char* ssid, uint8_t channel) {
	return startAP(ssid, nullptr, channel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::beginAP(const char* ssid, const char* key, uint8_t channel) {
	return startAP(ssid, key, channel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::startAP(const char* ssid, const char* key, uint8_t channel) {
	// TODO Close open connections?
	// TODO Set max clients to the maximum (4): AT=4
	// TODO if key is null
	// TODO     security_mode = OPEN
	// TODO else
	// TODO     security_mode = WPA/WPA2
	// TODO Set security mode: A1=<security_mode>
	// TODO if security_mode is WPA or WPA2 or WPA/WPA2
	// TODO     Set security key: A2=<key>
	// TODO Set channel: AC=<channel>
	// TODO Set SSID: AS=<ssid>

	// TODO Disable async messages
	// TODO Activate AP: A0

	// TODO return value
	return WL_IDLE_STATUS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void WiFi5Class::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
	// TODO Disabe DHCP: C4=0
	// TODO Set IP address: C6=<local_ip>
	// TODO Set DNS1 server: C9=<dns_server>
	// TODO Set gateway IP address: C8=<gateway>
	// TODO Set netmask: C7=<subnet>
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void WiFi5Class::disconnect() {
	ISM43340.stopSTA();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void WiFi5Class::end() {
	disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t WiFi5Class::status() {
	refreshStatus();
	return _status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool WiFi5Class::refreshStatus() {
	bool connected;
	if (!ISM43340.refreshStatus(_ssid, _ip, _netmask, _gateway, _dns1, &connected)) {
		_status = WL_NO_SHIELD;
		return false;
	}

	_status = connected ? WL_CONNECTED : WL_DISCONNECTED;
	return true;
}
