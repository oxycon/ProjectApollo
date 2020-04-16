uint16_t voltage_read()
{
	uint16_t x = analogRead(PIN_12V_MONITOR);
	return adc_to_mv(x);
}

uint32_t adc_to_mv(uint16_t adc)
{
	volatile uint32_t x = adc;
	/*
	raw / 1024 = mv_divided / 1.1
	mv_divided = ((raw * 1100) / 1024)
	mv_divided = mv_input * (r2 / (r1 + r2))
	((raw * 1100) / 1024) = mv_input * (r2 / (r1 + r2))
	mv_input = ((raw * 1100) / 1024) /
	                                  (r2 / (r1 + r2))
	mv_input = ((raw * 1100 * (r1 + r2)) / 1024) / r2
	mv_input = (raw * 1100 * (r1 + r2)) / (1024 * r2)
	*/
	uint32_t r_sum = VMON_R1 + VMON_R2;
	uint32_t denom = 1024 * VMON_R2;
	x *= 110;
	x *= r_sum;
	x /= denom;
	x *= 10;
	return x;
}
