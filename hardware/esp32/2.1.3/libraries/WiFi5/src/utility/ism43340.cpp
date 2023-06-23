#include "ism43340.h"

#include <SPI.h>

#define DEBUG 0
#define ISM43340_FIRST_BYTE_BUG 1

#ifndef ISM43340_CS_PIN
#define ISM43340_CS_PIN 2
#endif

#ifndef ISM43340_RESET_PIN
#define ISM43340_RESET_PIN 33
#endif

#ifndef ISM43340_CMD_DATA_READY_PIN
#define ISM43340_CMD_DATA_READY_PIN 32
#endif

ISM43340Class ISM43340;

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::init() {
	csPin = ISM43340_CS_PIN;
	cmdDataReadyPin = ISM43340_CMD_DATA_READY_PIN;
	resetPin = ISM43340_RESET_PIN;
	paddingByte = '\n';
	pendingWriteDelimiter = '\n';
	serverSocket = -1;

	for (int i = 0; i < ISM43340_NUM_SOCKETS; ++i) {
		busySockets[i] = false;
	}

	initDataReady();
	initReset();
	initCS();
	disableCS();

	SPI.begin();

	reset();

	delay(1000UL);

	return waitPrompt();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::getInfo(char* info, uint16_t* infoLen) {
	return getValue("I?", info, infoLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::waitPrompt() {
	uint8_t rxByte;

	while (digitalRead(cmdDataReadyPin) == LOW);

	enableCS();
	while (digitalRead(cmdDataReadyPin) != LOW) {
		SPI.transferBytes(&paddingByte, &rxByte, 1);
	}
	disableCS();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ISM43340Class::reset() {
	digitalWrite(resetPin, LOW);
	delay(100UL);
	digitalWrite(resetPin, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::sendRequest(const char* cmd, const char* payload, uint16_t payloadLen, const uint8_t* data, uint16_t dataLen) {
	if (!cmd) {
		return false;
	}

	uint16_t cmdLen = 2;

	if (!payload) {
		payloadLen = 0;
		data = nullptr;
		dataLen = 0;

	} else if (!data) {
		dataLen = 0;
	}

	if (!waitCmdDataReady()) {
		return false;
	}

	enableCS();

#ifdef ISM43340_FIRST_BYTE_BUG
#if DEBUG
	Serial.write(*cmd);
#endif

	SPI.transfer(*cmd++);
	--cmdLen;
	SPI.transfer(paddingByte);
#endif

	// Send command
	sendBuffer((const uint8_t*) cmd, cmdLen, payload ? '=' : '\r');

	// Send payload (optional)
	if (payloadLen > 0) {
		sendBuffer((const uint8_t*) payload, payloadLen, '\r');
	}

	// Send extra data (optional: S0 and S3 commands)
	if (dataLen > 0) {
		sendBuffer(data, dataLen, paddingByte);
	}

	// Send pending delimiter if any
	sendBuffer(nullptr, 0, paddingByte);

	disableCS();

	if (!waitCmdDataFinished()) {
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ISM43340Class::sendBuffer(const uint8_t* buff, uint16_t len, uint8_t delimiter) {
	// Send pending delimiter
	if (pendingWriteDelimiter != paddingByte) {
#if DEBUG
		Serial.write(pendingWriteDelimiter);
#endif
		if (len > 0) {
#if DEBUG
			Serial.write(*buff);
#endif
			SPI.transfer(*buff++);
			--len;

		} else {
#if DEBUG
			Serial.write(paddingByte);
#endif
			SPI.transfer(paddingByte);
		}

		SPI.transfer(pendingWriteDelimiter);

		// Delimiter is sent, so there is no pending delimiter to be sent
		pendingWriteDelimiter = paddingByte;
	}

	// Send buffer
	while (len >= 2) {
#if DEBUG
		Serial.write(*buff);
		Serial.write(*(buff + 1));
#endif
		SPI.transfer(*(buff + 1));
		SPI.transfer(*buff);
		buff += 2;
		len -= 2;
	}

	if (len == 1) {
		// Send last byte and delimiter and fit the frame
#if DEBUG
		Serial.write(*buff);
		Serial.write(delimiter);
#endif
		SPI.transfer(delimiter);
		SPI.transfer(*buff);
		--len;

		// Delimiter is sent, so there is no pending delimiter to be sent
		pendingWriteDelimiter = paddingByte;

	} else {
		// Save delimiter to be sent before the next buffer
		pendingWriteDelimiter = delimiter;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::receiveResponse(uint8_t* buff, uint16_t* len, uint32_t timeout) {
	uint8_t paddingBytes[2];
	uint16_t buffLen = 0;

	memset(rxBytes, 0, sizeof(rxBytes));
	memset(paddingBytes, paddingByte, sizeof(paddingBytes));

	if (!buff) {
		buffLen = 0;
	} else if (!len) {
		buffLen = 0;
	} else {
		buffLen = *len;
	}

	if (!waitCmdDataReady(timeout)) {
		return false;
	}

	enableCS();

	SPI.transferBytes(paddingBytes, rxBytes + ISM43340_RX_BUFFER_LEN - 2, 2); // Response start: \r\n
#if DEBUG
	Serial.write(rxBytes[ISM43340_RX_BUFFER_LEN - 1]);
	Serial.write(rxBytes[ISM43340_RX_BUFFER_LEN - 2]);
#endif

	uint16_t dataLen = 0;
	while (digitalRead(cmdDataReadyPin) != LOW) {
		// Shift last rx bytes to left 2 positions
		for (int i = 0; i < ISM43340_RX_BUFFER_LEN - 2; ++i) {
			rxBytes[i] = rxBytes[i + 2];
		}

		// Read 2 bytes and add them to the end of rxBytes
		SPI.transferBytes(paddingBytes, rxBytes + ISM43340_RX_BUFFER_LEN - 2, 2);
#if DEBUG
		Serial.write(rxBytes[ISM43340_RX_BUFFER_LEN - 1]);
		Serial.write(rxBytes[ISM43340_RX_BUFFER_LEN - 2]);
#endif
		dataLen += 2;

		if (buffLen > 0) {
			*buff++ = rxBytes[ISM43340_RX_BUFFER_LEN - 1];
			--buffLen;

			if (buffLen > 0) {
				*buff++ = rxBytes[ISM43340_RX_BUFFER_LEN - 2];
				--buffLen;
			}
		}
	}

	disableCS();

	// Count final 0x15 bytes
	int count15 = 0;
	for (int i = ISM43340_RX_BUFFER_LEN - 1; i > 0; --i) {
		if (rxBytes[i] != 0x15) {
			break;
		}
		++count15;
	}

	// Check if command is OK
	if ((rxBytes[ISM43340_RX_BUFFER_LEN - 5 - count15] == 'O') && (rxBytes[ISM43340_RX_BUFFER_LEN - 6 - count15] == 'K')) {
		// Adjust dataLen according to last 8 received bytes: "\r\nOK\r\n> "
		if (dataLen > 8 + count15) {
			dataLen -= 8 + count15;
		} else {
			// No data in the response
			dataLen = 0;
		}

	} else if ((rxBytes[ISM43340_RX_BUFFER_LEN - 8 - count15] == 'O') && (rxBytes[ISM43340_RX_BUFFER_LEN - 5 - count15] == 'K')) {
		// Adjust dataLen according to last 9 received bytes: "\r\nOK\r\n> \x15"
		if (dataLen > 9 + count15) {
			dataLen -= 9 + count15;
		} else {
			// No data in the response
			dataLen = 0;
		}

	} else {
		// Command failed
		return false;
	}

	if (len) {
		*len = dataLen;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::execute(const char* cmd, uint32_t timeout) {
	if (!sendRequest(cmd)) {
		return false;
	}
	return receiveResponse(timeout);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::setValue(const char* cmd, const char* value) {
	if (!sendRequest(cmd, value, strlen(value))) {
		return false;
	}
	return receiveResponse();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::setValue(const char* cmd, int32_t value, uint8_t base) {
	String valueStr(value, base);
	return setValue(cmd, valueStr.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::setValue(const char* cmd, uint32_t value, uint8_t base) {
	String valueStr(value, base);
	return setValue(cmd, valueStr.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::setValue(const char* cmd, IPAddress value) {
	String valueStr(value[0]);
	for (int i = 1; i < 4; ++i) {
		valueStr += '.';
		valueStr.concat(value[i]);
	}
	return setValue(cmd, valueStr.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::getValue(const char* cmd, const char* payload, uint16_t payloadLen, uint8_t* value, uint16_t* valueLen) {
	if (!sendRequest(cmd, payload, payloadLen)) {
		return false;
	}
	return receiveResponse(value, valueLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::getValue(const char* cmd, const char* payload, uint16_t payloadLen, char* value, uint16_t* valueLen) {
	uint16_t initialLen = 0;
	if (valueLen) {
		initialLen = *valueLen;
	}
	bool ret = getValue(cmd, payload, payloadLen, (uint8_t*) value, valueLen);
	if (ret) {
		if (valueLen) {
			if (initialLen == *valueLen) {
				--(*valueLen);
			}
			value[*valueLen] = '\0';
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::getValue(const char* cmd, const char* payload, uint16_t payloadLen, int32_t* value) {
	char valueStr[15];
	uint16_t len = sizeof(valueStr) / sizeof(char);
	if (!getValue(cmd, payload, payloadLen, valueStr, &len)) {
		return false;
	}
	if (value) {
		*value = atoi(valueStr);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::getValue(const char* cmd, const char* payload, uint16_t payloadLen, uint32_t* value) {
	char valueStr[15];
	uint16_t len = sizeof(valueStr) / sizeof(char);
	if (!getValue(cmd, payload, payloadLen, valueStr, &len)) {
		return false;
	}
	if (value) {
		*value = atoi(valueStr);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::getValue(const char* cmd, const char* payload, uint16_t payloadLen, IPAddress* value) {
	char valueStr[20];
	uint16_t len = sizeof(valueStr) / sizeof(char);
	if (!getValue(cmd, payload, payloadLen, valueStr, &len)) {
		return false;
	}
	if (value) {
		return value->fromString(valueStr);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::waitCmdData(int status, uint32_t timeout) {
	uint32_t startTime = millis();
	while (digitalRead(cmdDataReadyPin) != status) {
		if (millis() - startTime > timeout) {
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::startSTA(const char* ssid, uint8_t securityMode, const char* key, uint8_t joinRetries) {
	if (!setValue("C1", ssid)) { // Set SSID: C1=<ssid>
		return false;
	}

	if (!setValue("C3", securityMode)) { // Set security mode: C3=<securityMode>
		return false;
	}

	if (key && (!setValue("C2", key))) { // Set security key: C2=<key>
		return false;
	}

	if (!setValue("C4", 1)) { // Enable DHCP mode: C4=1
		return false;
	}

	if (!setValue("C5", 0)) { // Set IP version IPv4: C5=0
		return false;
	}

	if (!setValue("CB", joinRetries)) { // Set Join retries: CB=<numRetries>
		return false;
	}

	if (!execute("C0", 60000)) { // Join the network: C0
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::stopSTA() {
	// Disconnect from network: CD
	return execute("CD");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int ISM43340Class::findAvailableSocket() {
	for (int i = 0; i < ISM43340_NUM_SOCKETS; ++i) {
		if (!busySockets[i]) {
			return i;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::connect(int socket, IPAddress ip, uint16_t port) {
	if (!selectSocket(socket)) {
		return false;
	}

	// Set TCP protocol: P1=0
	if (!setValue("P1", 0)) {
		return false;
	}

	// Configure remote IP address: P3=<ip>
	if (!setValue("P3", ip)) {
		return false;
	}

	// Configure remote TCP port: P4=<port>
	if (!setValue("P4", port)) {
		return false;
	}

	// Start connection: P6=1
	if (!setValue("P6", 1)) {
		return false;
	}

	busySockets[socket] = true;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::disconnect(int socket) {
	if (!selectSocket(socket)) {
		return false;
	}

	if (socket != serverSocket) {
		// Stop connection: P6=0
		if (!setValue("P6", 0)) {
			return false;
		}

		busySockets[socket] = false;

	} else {
		// Accept next server connection: P5=10
		if (!setValue("P5", 10)) {
			return false;
		}

	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::isConnected(int socket) {
	if (!selectSocket(socket)) {
		return false;
	}

	// Get connection/socket status: P?
	char socketStatus[100];
	uint16_t socketStatusLen = sizeof(socketStatus) / sizeof(char);
	if (!getValue("P?", socketStatus, &socketStatusLen)) {
		return false;
	}

	// Socket protocol, IP address, port, ... from response 
	// PROTOCOL,CLIENT_IP,LOCAL_PORT,LOCAL_IP,CLIENT_PORT,TCP_SERVER,UDP_SERVER,TCP_BACKLOGS,ACCEPT_LOOP,READ_MODE
	char* field = getTokenField(socketStatus, ','); // PROTOCOL
	field = getTokenField(nullptr, ','); // CLIENT IP
	if (strcmp(field, "0.0.0.0") == 0) {
		// Not connected
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
size_t ISM43340Class::write(int socket, const uint8_t* buff, uint16_t len) {
	if (!selectSocket(socket)) {
		return 0;
	}

	// Write data: S3
	String lenStr(len);
	if (!sendRequest("S3", lenStr.c_str(), lenStr.length(), buff, len)) {
		return 0;
	}

	if (!receiveResponse()) {
		return 0;
	}

	return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int ISM43340Class::read(int socket, uint8_t* buff, uint16_t len) {
	if (!selectSocket(socket)) {
		return -1;
	}

	// Set read size: R1=<size>
	if (!setValue("R1", len)) {
		return -1;
	}

	// Set read timeout to 1ms: R2=1
	if (!setValue("R2", 1)) {
		return -1;
	}

	// Set read without delineation: R3=1
	if (!setValue("R3", 0)) {
		return -1;
	}

	// Read data: R0
	if (!getValue("R0", buff, &len)) {
		return 0;
	}

	return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::startServer(int socket, uint16_t port) {
	if (!selectSocket(socket)) {
		return false;
	}

	// Set TCP protocol: P1=0
	if (!setValue("P1", 0)) {
		return false;
	}

	// Configure local TCP port: P4=<port>
	if (!setValue("P2", port)) {
		return false;
	}

	// Start server: P5=11
	if (!setValue("P5", 11)) {
		return false;
	}

	busySockets[socket] = true;
	serverSocket = socket;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::stopServer(int socket) {
	// Stop server: 
	if (!setValue("P5", 0)) {
		return false;
	}

	busySockets[socket] = false;
	serverSocket = -1;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::hostByName(const char* hostname, IPAddress& ip) {
	if (!getValue("D0", hostname, strlen(hostname), &ip)) {
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::macAddress(uint8_t* mac) {
	if (!mac) {
		return false;
	}

	char macStr[18]; // MAC addr + '\0'
	uint16_t len = sizeof(macStr) / sizeof(char);
	if (!getValue("Z5", macStr, &len)) {
		return false;
	}
	if (len != 17) {
		return false;
	}

	const char* macStrPtr = macStr;
	for (int i = 0; i < 6; ++i) {
		*mac++ = hex2byte(macStrPtr);
		macStrPtr += 3;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t ISM43340Class::RSSI() {
	int32_t ret;
	if (!getValue("CR", &ret)) {
		return -100;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ISM43340Class::refreshStatus(char* ssid, IPAddress& ip, IPAddress& netmask, IPAddress& gateway, IPAddress& dns, bool* connected) {
	// Get network status: C?
	char networkStatus[300];
	uint16_t networkStatusLen = sizeof(networkStatus) / sizeof(char);
	if (!getValue("C?", networkStatus, &networkStatusLen)) {
		return false;
	}

	// Process IP address, netmask, ... from the response
	// SSID,KEY,SEC_TYPE,DHCP,IP_VERSION,IP,NETMASK,GATEWAY,DNS1,DNS2,RETRIES,AUTO_CONNECT,AUTHENTICATION,COUNTRY,STATUS
	char* field = getTokenField(networkStatus, ','); // SSID
	strcpy(ssid, field);
	field = getTokenField(nullptr, ','); // KEY
	field = getTokenField(nullptr, ','); // SEC_TYPE
	field = getTokenField(nullptr, ','); // DHCP
	field = getTokenField(nullptr, ','); // IP_VERSION
	field = getTokenField(nullptr, ','); // IP
	ip.fromString(field);
	field = getTokenField(nullptr, ','); // NETMASK
	netmask.fromString(field);
	field = getTokenField(nullptr, ','); // GATEWAY
	gateway.fromString(field);
	field = getTokenField(nullptr, ','); // DNS1
	dns.fromString(field);
	field = getTokenField(nullptr, ','); // DNS2
	field = getTokenField(nullptr, ','); // RETRIES
	field = getTokenField(nullptr, ','); // AUTO_CONNECT
	field = getTokenField(nullptr, ','); // AUTHENTICATION
	field = getTokenField(nullptr, ','); // COUNTRY
	field = getTokenField(nullptr, ','); // STATUS
	*connected = *field == '1';

	return true;
}
