#ifdef APP_HX
#include <HX711.h>

HX711 hx;

const int PIN_HX_DOUT = 2;
const int PIN_HX_SCK = 3;
const float HX_SCALE = -2.32e5 / 2403;
const int HX_OVERSAMPLE = 5;
const uint8_t PIN_POT = A8;
const uint8_t PIN_PRES = A15;

linear_cal_t mv_to_psi = {
	.min = 0.5e3,
	.max = 4.5e3,
	.range = 30,
};

linear_cal_t mv_to_mm = {
	.min = 0e3,
	.max = 5e3,
	.range = 110,
};

analog_t pins[] = {
	{.pin = PIN_POT, .cal = mv_to_mm, .ct = 0, .mv = 0, .value = 0},
	{.pin = PIN_PRES, .cal = mv_to_psi, .ct = 0, .mv = 0, .value = 0}
};

void read_pins(void) {
	for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
		pins[i].ct = analogRead(pins[i].pin);
		pins[i].mv = apply_cal(&ct_to_mv, pins[i].ct);
		pins[i].value = apply_cal(&(pins[i].cal), pins[i].mv);
	}
}

void setup() {
	Serial.begin(BAUD);
	hx.begin(PIN_HX_DOUT, PIN_HX_SCK);
	hx.tare();
	hx.set_scale();
	uprintf(L_RAW, "Time\tLoad [ct]\tLoad [N]\tmm\tpsig\r\n");
}


void loop() {
	read_pins();
	float pos = pins[0].value;
	float pres = pins[1].value;
	
	float val = hx.get_value(HX_OVERSAMPLE);
	float cal = val / HX_SCALE;

	uprintf(L_RAW, "%.3f\t%.2e\t%.2e\t%.1f\t%.2f\r", 
		millis() / 1000.0, val, cal, pos, pres);
}
#endif
