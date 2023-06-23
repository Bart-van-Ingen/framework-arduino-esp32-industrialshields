#ifndef __WiFi5_H__
#define __WiFi5_H__

#include <Arduino.h>

#include "WiFi5Client.h"
#include "WiFi5Server.h"
#include "utility/constants.h"

class WiFi5Class {
	public:
		bool getInfo(char* info, uint16_t* infoLen);
		bool updateFirmware();
		uint8_t begin();

		uint8_t begin(const char* ssid);
		uint8_t begin(const char* ssid, const char* key, uint8_t encType = WL_SEC_TYPE_WPA2);

		uint8_t beginAP(const char* ssid);
		uint8_t beginAP(const char* ssid, uint8_t channel);
		uint8_t beginAP(const char* ssid, const char* key);
		uint8_t beginAP(const char* ssid, const char* key, uint8_t channel);

		void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
		inline void config(IPAddress local_ip) {
			return config(local_ip, (local_ip & 0xffffff00) | 0x01);
		}
		inline void config(IPAddress local_ip, IPAddress dns_server) {
			return config(local_ip, dns_server, (local_ip & 0xffffff00) | 0x01);
		}
		inline void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
			return config(local_ip, dns_server, gateway, 0xffffff00);
		}

		void disconnect();
		void end();

		inline uint8_t* macAddress(uint8_t* mac) {
			return ISM43340.macAddress(mac) ? mac : nullptr;
		}

		uint8_t status();
		inline const char* SSID() const { return _ssid; }
		inline IPAddress localIP() const { return _ip; }
		inline IPAddress subnetMask() const { return _netmask; }
		inline IPAddress gatewayIP() const { return _gateway; }

		int32_t RSSI();

		inline bool hostByName(const char* hostname, IPAddress& result) {
			return ISM43340.hostByName(hostname, result);
		}
		inline bool hostByName(const String &hostname, IPAddress& result) {
			return hostByName(hostname.c_str(), result);
		}

		// TODO Ping methods
		// TODO Set contry code

	private:
		inline bool refreshStatus();
		uint8_t startSTA(const char* ssid, uint8_t securityMode, const char* key);
		uint8_t startAP(const char* ssid, const char* key, uint8_t channel);

	private:
		uint8_t _status;
		char _ssid[WL_SSID_SIZE];
		bool _dhcp;
		IPAddress _ip;
		IPAddress _netmask;
		IPAddress _gateway;
		IPAddress _dns1;
};

extern WiFi5Class WiFi5;

#endif // __WiFi5_H__
