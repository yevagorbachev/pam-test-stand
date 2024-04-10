#include "pam-test-stand.hpp"

float apply_cal(struct linear_cal* cal, float value) {
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

// ADS1220
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

float posn;
float pres;
float load;

void read_chans(void) {
	ads.setCompareChannels(CHAN_LOAD);
	load = ads.getVoltage_mV();
	load = apply_cal(NULL, load);

	ads.setCompareChannels(CHAN_PRES);
	pres = ads.getVoltage_mV();
	pres = apply_cal(&mv_to_100psi, pres);

	ads.setCompareChannels(CHAN_POT);
	posn = ads.getVoltage_mV();
	posn = apply_cal(&mv_to_mm, posn);
}

void setup() {
	init_serial();
	init_ads();

	uprintf(L_RAW, "T_0 [s]\tdt [s]\tF [mV]\tx [mm]\tP [psi]\t\r\n");
}

void loop() {
	float T_0 = millis() / 1000.0;
	read_chans();
	float T_1 = millis() / 1000.0;
	uprintf(L_RAW, "%.3f\t%.3f\t%.2f\t%.2f\t%.2f\r\n", 
		T_0, T_1 - T_0, load, posn, pres);
	delay(100);
}
