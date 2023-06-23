#include "format.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t hex2byte(const char* hex) {
	uint8_t ret = hex2nib(*hex++) << 4;
	return ret + hex2nib(*hex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t hex2nib(char hex) {
	if (hex >= 'a') {
		return hex - 'a' + 0xa;
	} else if (hex >= 'A') {
		return hex - 'A' + 0xa;
	}
	return hex - '0';
}

////////////////////////////////////////////////////////////////////////////////////////////////////
char nib2hex(uint8_t nibble) {
	if (nibble >= 0xa) {
		return 'A' + nibble - 0xa;
	}
	return '0' + nibble;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static char* nextField = nullptr;

char* getTokenField(char* fields, char token) {
	char *field = fields;
	if (fields) {
		nextField = fields;
	} else {
		field = nextField;
	}

	while (*nextField != '\0') {
		if (*nextField == token) {
			*nextField = '\0';
			++nextField;
			break;
		}
		++nextField;
	}
	return field;
}
