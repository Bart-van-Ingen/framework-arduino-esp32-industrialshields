#include "RS485.h"

#if defined(HAVE_RS485)

#if defined(HAVE_RS485_HARD)

#include <driver/uart.h>

RS485Class::RS485Class(int numSerial): Uart(numSerial) {
}

void RS485Class::begin(unsigned long baud, uint8_t duplex, uint32_t config) {
	Uart::begin(baud, config);
	uart_set_pin(RS485_HWSERIAL, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, RS485_DE_RE, UART_PIN_NO_CHANGE);
	uart_set_mode(RS485_HWSERIAL, UART_MODE_RS485_HALF_DUPLEX);
}

RS485Class RS485(RS485_HWSERIAL);

#endif // HAVE_RS485_HARD

void RS485Event() __attribute__((weak));

void serial2EventRun() {
	if (RS485Event && RS485.available()) {
		RS485Event();
	}
}

#endif
