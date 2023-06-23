#include "RS232.h"

#if defined(HAVE_RS232)

#if defined(HAVE_RS232_HARD)

#include <driver/uart.h>

RS232Class::RS232Class(uint32_t spi_cs, uint8_t channel) : SC16IS752HardwareSerial(spi_cs, channel) {}

#endif // HAVE_RS232_HARD

RS232Class RS232(SERIAL_SC_0_CS, 0);

void RS232Event() __attribute__((weak));

void serial1EventRun() {
	if (RS232Event && RS232.available()) {
		RS232Event();
	}
}

#endif // HAVE_RS232
