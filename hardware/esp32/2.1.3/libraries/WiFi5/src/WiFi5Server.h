#ifndef __WIFI5SERVER_H__
#define __WIFI5SERVER_H__

#include <Arduino.h>

#include "WiFi5Client.h"
#include "utility/ism43340.h"

class WiFi5Server : public Server {
	public:
		explicit WiFi5Server(uint16_t port) : port(port), socket(-1) {}

	public:
		virtual void begin();
		virtual inline void begin(uint16_t port);
		virtual WiFi5Client available();

		virtual size_t write(uint8_t) { return 0; }

	private:
		uint16_t port;
		int socket;
};

#endif
