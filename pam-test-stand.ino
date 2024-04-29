#include "pam-test-stand.hpp"

const uint8_t PIN_ADS_CS = 53;
const uint8_t PIN_ADS_DRDY = 47;
const enum ADS1220_MUX CHAN_LOAD = ADS1220_MUX_3_2;
const enum ADS1220_MUX CHAN_POT = ADS1220_MUX_0_AVSS;
const enum ADS1220_MUX CHAN_PRES = ADS1220_MUX_1_AVSS;

const uint8_t PIN_OUTLET = 5;
const uint8_t PIN_INLET = 4;

// measurements
static float M_POSN_MV;
static float M_PRES_MV;
static float M_LOAD_MV;

static float M_POSN;
static float M_PRES;
static float M_LOAD;

// state variables
static float E_POSN;
static float E_VELO;
static float E_ACCL;

// input variables
static float U_VALVE;


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
	ads.setCompareChannels(CHAN_PRES);
}

int read_compare(const char* ref, size_t len) {
	int ret;
	char buf[LINE_LENGTH];
	Serial.flush();
	Serial.setTimeout(100 * 1000);
	uprintf(L_COMMENT, "Awaiting '%s'\r\n", ref);
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

ret_t measurement_entry(task_t* tdata) {
	ads.setCompareChannels(CHAN_LOAD);
	M_LOAD_MV = ads.getVoltage_mV();

	ads.setCompareChannels(CHAN_PRES);
	M_PRES_MV = ads.getVoltage_mV();
	M_PRES = apply_cal(&mv_to_100psi, M_PRES_MV);

	ads.setCompareChannels(CHAN_POT);
	M_POSN_MV = ads.getVoltage_mV();
	M_POSN = apply_cal(&mv_to_mm, M_POSN_MV);
}

task_t measurement_task = {
	.entry = measurement_entry,
	.timer = millis,
	.next = 0,
	.period = 2,
	.status = S_RUN,
};

ret_t data_output_entry(task_t* tdata) {
	uprintf(L_RAW, "%.4f\t%.2f\t%.2f\t%.2f\r\n", 
		micros() / 1e6, M_POSN, M_PRES, M_LOAD);
	return 0;
}

task_t data_output_task = {
	.entry = data_output_entry, 
	.timer = millis,
	.next = 0,
	.period = 10,
	.status = S_RUN,
};

void setup() {
	init_serial();
	init_ads();
	init_sched();

	tid_t measurement_id = add_task(measurement_task);
	uprintf(L_COMMENT, "Initialized task %d\n", measurement_id);
	tid_t data_output_id = add_task(data_output_task);
	uprintf(L_COMMENT, "Initialized task %d\n", data_output_id);
	uprintf(L_RAW, "Time\tP\r\n");
	uprintf(L_COMMENT, "[s]\t[psi]\r\n");
}

void loop() {
	run_tasks();
}
