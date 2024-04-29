#include <ADS1220_WE.h>
#include "src/arduino_printf/arduino_printf.hpp"
#include "src/scheduler/scheduler.hpp"

struct linear_cal {
	float min;
	float max;
	float range;
};

#define LINE_LENGTH 256
#define BAUD 460800

const struct linear_cal mv_to_100psi = {
	.min = 0.5e3,
	.max = 4.5e3,
	.range = 100,
};

const struct linear_cal mv_to_mm = {
	.min = 0e3,
	.max = 5e3,
	.range = 110,
};

const char START_TEST_CMD[] = "start";
