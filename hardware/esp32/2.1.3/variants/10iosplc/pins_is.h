#ifndef __pins_is_H__
#define __pins_is_H__

// Mapping: Digital inputs
#define PIN_I0_0                    (4)
#define PIN_I0_1                    (2)
#define PIN_I0_2                    (15)
#define PIN_I0_3                    (32)
#define PIN_I0_4                    (25)
#define PIN_I0_5                    (26)
#define PIN_I0_6                    (27)
#define PIN_I0_7                    (14)
#define PIN_I0_8                    (12)
#define PIN_I0_9                    (13)

static const uint8_t I0_0 = PIN_I0_0;
static const uint8_t I0_1 = PIN_I0_1;
static const uint8_t I0_2 = PIN_I0_2;
static const uint8_t I0_3 = PIN_I0_3;
static const uint8_t I0_4 = PIN_I0_4;
static const uint8_t I0_5 = PIN_I0_5;
static const uint8_t I0_6 = PIN_I0_6;
static const uint8_t I0_7 = PIN_I0_7;
static const uint8_t I0_8 = PIN_I0_8;
static const uint8_t I0_9 = PIN_I0_9;

// Mapping: Digital outputs
#if defined(PLC10IOS_DIGITAL)
#define PIN_Q0_0                    (4)
#define PIN_Q0_1                    (2)
#define PIN_Q0_2                    (15)
#define PIN_Q0_3                    (32)
#define PIN_Q0_4                    (25)
#define PIN_Q0_5                    (26)
#define PIN_Q0_6                    (27)
#define PIN_Q0_7                    (14)
#define PIN_Q0_8                    (12)
#define PIN_Q0_9                    (13)

static const uint8_t Q0_0 = PIN_Q0_0;
static const uint8_t Q0_1 = PIN_Q0_1;
static const uint8_t Q0_2 = PIN_Q0_2;
static const uint8_t Q0_3 = PIN_Q0_3;
static const uint8_t Q0_4 = PIN_Q0_4;
static const uint8_t Q0_5 = PIN_Q0_5;
static const uint8_t Q0_6 = PIN_Q0_6;
static const uint8_t Q0_7 = PIN_Q0_7;
static const uint8_t Q0_8 = PIN_Q0_8;
static const uint8_t Q0_9 = PIN_Q0_9;
#endif

// Mapping: Relay outputs
#if defined(PLC10IOS_RELAY)
#define PIN_R0_0                    (4)
#define PIN_R0_1                    (2)
#define PIN_R0_2                    (15)
#define PIN_R0_3                    (32)
#define PIN_R0_4                    (25)
#define PIN_R0_5                    (26)
#define PIN_R0_6                    (27)
#define PIN_R0_7                    (14)
#define PIN_R0_8                    (12)
#define PIN_R0_9                   (13)

static const uint8_t R0_0 = PIN_R0_0;
static const uint8_t R0_1 = PIN_R0_1;
static const uint8_t R0_2 = PIN_R0_2;
static const uint8_t R0_3 = PIN_R0_3;
static const uint8_t R0_4 = PIN_R0_4;
static const uint8_t R0_5 = PIN_R0_5;
static const uint8_t R0_6 = PIN_R0_6;
static const uint8_t R0_7 = PIN_R0_7;
static const uint8_t R0_8 = PIN_R0_8;
static const uint8_t R0_9 = PIN_R0_9;
#endif

// Ethernet
#define PIN_SPI_SS_ETHERNET_LIB		(33)

// RS-485
#define HAVE_RS485
#define HAVE_RS485_HARD
#define RS485_HWSERIAL              (2)
#define RS485_DE_RE                 (5)

#endif // __pins_is_H__
