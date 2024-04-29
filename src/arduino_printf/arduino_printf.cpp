#include "arduino_printf.hpp"

int uprintf(print_mode_t mode, const char* fmt, ...) {
	char line[LINE_LENGTH];
	int ret = 0;

	va_list ap;
	va_start(ap, fmt);
	ret = vsnprintf(line, sizeof(line), fmt, ap);
	va_end(ap);

	if (ret >= LINE_LENGTH) {
		Serial.print("# printf ovverran line buffer");
		Serial.println(ret);
		return ret;
	}
	if (ret < 0) {
		Serial.print("Error status returned by vsnprintf:");
		Serial.println(ret);
		return ret;
	}

	switch (mode) {
		case L_RAW:
			break;
		case L_COMMENT:
			Serial.print("# ");
			break;
		case L_EVENT:
			Serial.print("# EVENT: ");
			break;
	}
	Serial.print(line);
	
	return ret;
}


