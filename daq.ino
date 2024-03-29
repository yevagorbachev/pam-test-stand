#ifdef APP_DAQ
void setup(void) {
	Serial.begin(BAUD);
	for (int i = 0; i < sizeof(meas_pins); i++) {
		pinMode(meas_pins[i], INPUT);
	}
	uprintf(L_COMMENT, "Timestamped voltage measurements\n");
	uprintf(L_RAW, "Time [ms],A0 [mV],A1 [mV],A2 [mV]\n");
}

void loop(void) {
	read_analog_pins();
	uprintf(L_RAW, "%7lu,%4.0f,%4.0f,%4.0f\r", millis(),
		pin_mvolts[0], pin_mvolts[1], pin_mvolts[2]);

	delay(50);
}
#endif
