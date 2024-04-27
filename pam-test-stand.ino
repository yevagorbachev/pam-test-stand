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
	uprintf(L_COMMENT, "Initialized serial\r\r\n");
}

// ADS1220
ADS1220_WE ads(PIN_ADS_CS, PIN_ADS_DRDY);

void init_ads(void) {
	pinMode(PIN_ADS_DRDY, INPUT);
	pinMode(PIN_ADS_CS, OUTPUT);
	if (!ads.init()) {
		uprintf(L_COMMENT, "Failed to initialize ADS1220\r\r\n");
		return;
	} else {
		uprintf(L_COMMENT, "Initialized ADS1220\r\r\n");
	}

	ads.bypassPGA(true);
	ads.setGain(ADS1220_GAIN_1);
	ads.setAvddAvssAsVrefAndCalibrate();
	ads.setDataRate(ADS1220_DR_LVL_6);
	ads.setOperatingMode(ADS1220_TURBO_MODE);
	ads.setConversionMode(ADS1220_CONTINUOUS);
}

/// APPLICATION CODE

// Reading ADS1220 channels
float posn;
float pres;
float load;
static unsigned long t_start = 0;

void read_chans(void) {
	float T = (millis() - t_start) / 1000.0;
	uprintf(L_RAW, "%.3f", T);

	/* ads.setCompareChannels(CHAN_LOAD);
	load = 0;
	const int n_samples = 100;
	for (int i = 0; i < n_samples; i++) {
		load += ads.getVoltage_mV();
	}
	load /= n_samples;
	uprintf(L_RAW, "\t%+04.2f", load); */

	ads.setCompareChannels(CHAN_PRES);
	pres = ads.getVoltage_mV();
	pres = apply_cal(&mv_to_100psi, pres);
	uprintf(L_RAW, "\t%.3f", pres);

	/* ads.setCompareChannels(CHAN_POT);
	posn = ads.getVoltage_mV();
	posn = apply_cal(&mv_to_mm, posn);
	uprintf(L_RAW, "\t%.2f", posn); */

	uprintf(L_RAW, "\r\n");
}

// Fill and drain tests
const float fill_time_s = 4;
const float drain_time_s = 10;
const float flush_time_s = 3;

void pwm_test(uint8_t duty) {
	long now;

	uprintf(L_EVENT, "Drain testing: %d duty\r\n", duty);
	uprintf(L_EVENT, "Fill start\r\n");

	now = millis();
	analogWrite(PIN_INLET, 255);
	while (millis() - now < 1000 * fill_time_s) {
		read_chans();
	}
	analogWrite(PIN_INLET, 0);

	uprintf(L_EVENT, "Fill end\r\n");
	uprintf(L_EVENT, "Drain start\r\n");

	now = millis();
	analogWrite(PIN_OUTLET, duty);
	while (millis() - now < 1000 * drain_time_s) {
		read_chans();
	}
	uprintf(L_EVENT, "Drain end\r\n");
	// analogWrite(PIN_OUTLET, 255);
	// delay(1000 * flush_time_s);
	analogWrite(PIN_OUTLET, 0);
}

int read_compare(const char* ref, size_t len) {
	int ret;
	char buf[LINE_LENGTH];
	Serial.flush();
	uprintf(L_COMMENT, "Awaiting command '%s'\r\n", ref);
	uprintf(L_COMMENT, "");
	ret = Serial.readBytesUntil('\r', buf, sizeof(buf) - 1);
	buf[ret] = 0;
	uprintf(L_COMMENT, "Acknowledged '%s'\r\n", buf);

	if (0 == ret) {
		return -1;
	}
	if (ret > len) {
		return len;
	}

	ret = strncmp(buf, ref, len);
	return ret;
}

enum TIM_PRESCALER {
	TIM_31K = 1,
	TIM_4K = 2,
	TIM_490 = 3,
	TIM_120 = 4,
	TIM_30 = 5,
	TIM_20 = 6,
};

const uint8_t pwm_sweep_start = 10;
const uint8_t pwm_sweep_step = 10;
const uint8_t pwm_sweep_end = 250;

void setup() {
	init_serial();
	init_ads();

	uprintf(L_RAW, "Time\tP\r\n");
	uprintf(L_COMMENT, "[s]\t[psi]\r\n");

	uint8_t eraser = 0b0000111;
	enum TIM_PRESCALER speed = TIM_30;
	TCCR3B &= ~eraser;
	TCCR3B |= speed;

	int ret = read_compare(START_TEST_CMD, sizeof(START_TEST_CMD));
	if (0 == ret) {
		uprintf(L_COMMENT, "Command correct\r\n");
		delay(3 * 1000);
		t_start = millis();
		for (uint8_t duty = pwm_sweep_start; 
		   duty <= pwm_sweep_end; 
		   duty += pwm_sweep_step) { 
		   pwm_test(duty);		   
		}
	} else {
		uprintf(L_COMMENT, "Command incorrect: %d\r\n", ret);
	}

	
}

void loop() {
}
