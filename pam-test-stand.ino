#include "src/ArduinoPrintf/ArduinoPrintf.hpp"

typedef struct {
	float min;
	float max;
	float range;
} linear_cal_t;

typedef struct {
	uint8_t pin;
	linear_cal_t cal;
	int ct;
	float mv;
	float value;
} analog_t;

#define LINE_LENGTH 256
#define BAUD 115200

linear_cal_t identity = {
	.min = 0,
	.max = 1,
	.range = 1,
};

linear_cal_t ct_to_mv = {
	.min = 0,
	.max = 1023,
	.range = 5e3,
};


float apply_cal(linear_cal_t* cal, int value) {
	if (NULL == cal) {
		return value;
	} else {
		return (value - cal->min) / (cal->max - cal->min) * cal->range;
	}
}
