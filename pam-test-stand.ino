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

linear_cal_t mv_to_30psi = {
	.min = 0.5e3,
	.max = 4.5e3,
	.range = 30,
};

linear_cal_t mv_to_100psi = {
	.min = 0.5e3,
	.max = 4.5e3,
	.range = 100,
};

linear_cal_t mv_to_mm = {
	.min = 0e3,
	.max = 5e3,
	.range = 110,
};


float apply_cal(linear_cal_t* cal, int value) {
	if (NULL == cal) {
		return value;
	} else {
		return (value - cal->min) / (cal->max - cal->min) * cal->range;
	}
}

/// PERIPHERAL INITIALIZATION
// Serial
void init_serial(void) {
	Serial.begin(BAUD);
	uprintf(L_COMMENT, "Initialized serial\r\n");
}

#include <HX711.h>
// HX711
HX711 hx;

const int PIN_HX_DOUT = 2;
const int PIN_HX_SCK = 3;
const float HX_SCALE = -2.32e5 / 2403;
const int HX_OVERSAMPLE = 5;

void init_hx(void) {
	hx.begin(PIN_HX_DOUT, PIN_HX_SCK);
	hx.tare();
	hx.set_scale();
}

// ADS1220
#include <ADS1220_WE.h>

// const int PIN_ADS_COPI = 51;
// const int PIN_ADS_CIPO = 50; 
// const int PIN_ADS_SCLK = 52;
const int PIN_ADS_CS = 53;
const int PIN_ADS_DRDY = 47;

ADS1220_WE ads(PIN_ADS_CS, PIN_ADS_DRDY);

void init_ads(void) {
	pinMode(PIN_ADS_DRDY, INPUT);
	pinMode(PIN_ADS_CS, OUTPUT);
	if (!ads.init()) {
		uprintf(L_COMMENT, "Failed to initialize ADS1220\r\n");
		return;
	} else {
		uprintf(L_COMMENT, "Initialized ADS1220\r\n");
	}
	ads.bypassPGA(true);
	ads.setGain(ADS1220_GAIN_1);
	ads.setAvddAvssAsVrefAndCalibrate();
	ads.setDataRate(ADS1220_DR_LVL_6);
	ads.setOperatingMode(ADS1220_TURBO_MODE);
	ads.setConversionMode(ADS1220_CONTINUOUS);
}
