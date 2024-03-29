#include "src/ArduinoPrintf/ArduinoPrintf.hpp"

typedef struct {
	float min;
	float max;
	float range;
} linear_cal_t;

#define LINE_LENGTH 256
#define BAUD 115200

linear_cal_t ct_to_mv = {
	.min = 0,
	.max = 1023,
	.range = 5e3,
};

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

uint8_t meas_pins[] = {A0, A1, A2};
int pin_ct[sizeof(meas_pins)];
float pin_mvolts[sizeof(meas_pins)];

float apply_cal(linear_cal_t* cal, int value) {
	if (NULL == cal) {
		return value;
	} else {
		return (value - cal->min) / (cal->max - cal->min) * cal->range;
	}
}

void read_analog_pins(void) {
	for (int i = 0; i < sizeof(meas_pins); i++) {
		pin_ct[i] = analogRead(meas_pins[i]);
		pin_mvolts[i] = apply_cal(&ct_to_mv, pin_ct[i]);
	}
}
