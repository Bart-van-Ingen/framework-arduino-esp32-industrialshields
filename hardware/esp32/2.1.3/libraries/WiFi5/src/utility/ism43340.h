#ifndef __ISM43340_H__
#define __ISM43340_H__

#include <Arduino.h>

#include "format.h"

#define ISM43340_WAIT_CMD_DATA_READY_TIMEOUT	500UL
#define ISM43340_RX_BUFFER_LEN					64
#define ISM43340_NUM_SOCKETS					4

class ISM43340Class {
	public:
		bool init();
		inline bool updateFirmware() {
			return execute("ZU");
		}
		bool getInfo(char* info, uint16_t* infoLen);
		bool startSTA(const char* ssid, uint8_t securityMode, const char* key, uint8_t joinRetries);
		bool stopSTA();
		bool refreshStatus(char* ssid, IPAddress& ip, IPAddress& netmask, IPAddress& gateway, IPAddress& dns, bool* connected);
		inline bool disableAsyncMessages() {
			return setValue("MS", 2);
		}

		int findAvailableSocket();

		bool connect(int socket, IPAddress ip, uint16_t port);
		bool disconnect(int socket);
		bool isConnected(int socket);
		size_t write(int socket, const uint8_t* buff, uint16_t len);
		int read(int socket, uint8_t* buff, uint16_t len);

		bool startServer(int socket, uint16_t port);
		bool stopServer(int socket);

		bool hostByName(const char* hostname, IPAddress& ip);
		bool macAddress(uint8_t* mac);
		int32_t RSSI();

	private:
		bool sendRequest(
		 const char* cmd,
		 const char* payload = nullptr, uint16_t payloadLen = 0,
		 const uint8_t* data = nullptr, uint16_t dataLen = 0
		 );
		bool receiveResponse(uint8_t* buff = nullptr, uint16_t* len = nullptr, uint32_t timeout = ISM43340_WAIT_CMD_DATA_READY_TIMEOUT);
		inline bool receiveResponse(uint32_t timeout) {
			return receiveResponse(nullptr, nullptr, timeout);
		}

		inline void initCS() {
			pinMode(csPin, OUTPUT);
		}
		inline void enableCS() {
			digitalWrite(csPin, LOW);
		}
		inline void disableCS() {
			digitalWrite(csPin, HIGH);
		}
		inline void initReset() {
			pinMode(resetPin, OUTPUT);
		}
		inline void initDataReady() {
			pinMode(cmdDataReadyPin, INPUT);
		}

		void reset();
		void sendBuffer(const uint8_t* buff, uint16_t len, uint8_t delimiter);
		inline bool waitCmdDataReady(uint32_t timeout = ISM43340_WAIT_CMD_DATA_READY_TIMEOUT) {
			return waitCmdData(HIGH, timeout);
		}
		inline bool waitCmdDataFinished(uint32_t timeout = ISM43340_WAIT_CMD_DATA_READY_TIMEOUT) {
			return waitCmdData(LOW, timeout);
		}
		bool waitCmdData(int status, uint32_t timeout);
		bool waitPrompt();

		bool execute(const char* cmd, uint32_t timeout = ISM43340_WAIT_CMD_DATA_READY_TIMEOUT);

		bool setValue(const char* cmd, const char* value);
		bool setValue(const char* cmd, int32_t value, uint8_t base = 10);
		bool setValue(const char* cmd, uint32_t value, uint8_t base = 10);
		bool setValue(const char* cmd, const IPAddress value);

		inline bool setValue(const char* cmd, const String& value) {
			return setValue(cmd, value.c_str());
		}
		inline bool setValue(const char* cmd, int8_t value, uint8_t base = 10) {
			return setValue(cmd, int32_t(value), base);
		}
		inline bool setValue(const char* cmd, uint8_t value, uint8_t base = 10) {
			return setValue(cmd, uint32_t(value), base);
		}
		inline bool setValue(const char* cmd, int16_t value, uint8_t base = 10) {
			return setValue(cmd, int32_t(value), base);
		}
		inline bool setValue(const char* cmd, uint16_t value, uint8_t base = 10) {
			return setValue(cmd, uint32_t(value), base);
		}

		bool getValue(const char* cmd, const char* payload, uint16_t payloadLen, uint8_t* value, uint16_t* valueLen);
		bool getValue(const char* cmd, const char* payload, uint16_t payloadLen, char* value, uint16_t* valueLen);
		bool getValue(const char* cmd, const char* payload, uint16_t payloadLen, int32_t* value);
		bool getValue(const char* cmd, const char* payload, uint16_t payloadLen, uint32_t* value);
		bool getValue(const char* cmd, const char* payload, uint16_t payloadLen, IPAddress* value);

		inline bool getValue(const char* cmd, uint8_t* value, uint16_t* valueLen) {
			return getValue(cmd, nullptr, 0, value, valueLen);
		}
		inline bool getValue(const char* cmd, char* value, uint16_t* valueLen) {
			return getValue(cmd, nullptr, 0, value, valueLen);
		}
		inline bool getValue(const char* cmd, int32_t* value) {
			return getValue(cmd, nullptr, 0, value);
		}
		inline bool getValue(const char* cmd, uint32_t* value) {
			return getValue(cmd, nullptr, 0, value);
		}
		inline bool getValue(const char* cmd, IPAddress* value) {
			return getValue(cmd, nullptr, 0, value);
		}

		inline bool selectSocket(int socket) {
			return setValue("P0", socket);
		}

	private:
		int csPin;
		int cmdDataReadyPin;
		int resetPin;

		uint8_t paddingByte;
		uint8_t pendingWriteDelimiter;
		uint8_t rxBytes[ISM43340_RX_BUFFER_LEN];

		bool busySockets[ISM43340_NUM_SOCKETS];
		int serverSocket;
};

extern ISM43340Class ISM43340;

#endif
