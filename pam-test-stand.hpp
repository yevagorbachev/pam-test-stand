#include <ADS1220_WE.h>
#include <stdint.h>
#include "src/ArduinoPrintf/ArduinoPrintf.hpp"

struct linear_cal {
	float min;
	float max;
	float range;
};

#define LINE_LENGTH 256
#define BAUD 115200

const uint8_t PIN_ADS_CS = 53;
const uint8_t PIN_ADS_DRDY = 47;
const enum ADS1220_MUX CHAN_LOAD = ADS1220_MUX_3_2;
const enum ADS1220_MUX CHAN_POT = ADS1220_MUX_0_AVSS;
const enum ADS1220_MUX CHAN_PRES = ADS1220_MUX_1_AVSS;

const uint8_t PIN_OUTLET = 4;
const uint8_t PIN_INLET = 5;


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
