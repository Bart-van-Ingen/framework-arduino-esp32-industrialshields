#ifndef __WIFI5_FORMAT_H__
#define __WIFI5_FORMAT_H__

#include <Arduino.h>

uint8_t hex2byte(const char* hex);
uint8_t hex2nib(char hex);

char nib2hex(uint8_t nibble);
void num2str(uint32_t num, char* str, uint8_t base);

char* getTokenField(char* fields, char token);

#endif
