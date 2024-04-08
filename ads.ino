#ifdef APP_ADS

const enum ADS1220_MUX CHAN_LOAD = ADS1220_MUX_2_3;

float pos;
float pres;
float load;

void read_chans(void) {
	ads.setCompareChannels(CHAN_LOAD);
	load = ads.getVoltage_mV();
	ads.setCompareChannels(ADS1220_MUX_0_AVSS);
	pos = ads.getVoltage_mV();
	ads.setCompareChannels(ADS1220_MUX_1_AVSS);
	pres = ads.getVoltage_mV();
}

void setup() {
	init_serial();
	init_ads();

	uprintf(L_RAW, "T_0 [s]\tt [s]\t10 [mV]\t2G [mV]\t3G [mV]\t\r\n");
}

void loop() {
	float T_0 = millis() / 1000.0;
	read_chans();
	float T_1 = millis() / 1000.0;
	uprintf(L_RAW, "%.3f\t%.3f\t%.2f\t%.2f\t%.2f\r\n", 
		T_0, T_1 - T_0, load, pos, pres);
	delay(100);
}
#endif
