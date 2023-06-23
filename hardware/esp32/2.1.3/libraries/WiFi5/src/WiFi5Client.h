#ifndef __WIFI5CLIENT_H__
#define __WIFI5CLIENT_H__

#include <Arduino.h>

#include "utility/ism43340.h"

class WiFi5Client : public Client {
	public:
		explicit WiFi5Client();
		explicit WiFi5Client(int socket);

	public:
		virtual int connect(IPAddress ip, uint16_t port);
		virtual int connect(const char *host, uint16_t port);
		virtual size_t write(uint8_t);
		virtual size_t write(const uint8_t *buf, size_t size);
		virtual int available();
		virtual int read();
		virtual int read(uint8_t *buf, size_t size);
		virtual int peek();
		virtual void flush();
		virtual void stop();
		virtual uint8_t connected();
		virtual operator bool();

	private:
		int socket;
		uint8_t connectionStatus;
};

#endif
