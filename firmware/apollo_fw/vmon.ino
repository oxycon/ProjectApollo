// read the voltage, if it exceeds any thresholds, register the fault_save
// if the threshold is crossed, do not clear the fault until it recovers passing a hysteresis
void vmon_task()
{
	uint16_t mv = voltage_read();

	if (mv < VMON_THRESH_LOW)
	{
		if ((current_faults & FAULTCODE_UNDERVOLTAGE) == 0)
		{
			current_faults |= FAULTCODE_UNDERVOLTAGE;
			fault_save(current_faults);
		}
	}
	else if (mv >= (VMON_THRESH_LOW + VMON_THRESH_HYSTER))
	{
		current_faults &= ~FAULTCODE_UNDERVOLTAGE;
	}

	if (mv > VMON_THRESH_HIGH)
	{
		if ((current_faults & FAULTCODE_OVERVOLTAGE) == 0)
		{
			current_faults |= FAULTCODE_OVERVOLTAGE;
			fault_save(current_faults);
		}
	}
	else if (mv <= (VMON_THRESH_HIGH - VMON_THRESH_HYSTER))
	{
		current_faults &= ~FAULTCODE_OVERVOLTAGE;
	}
}

uint16_t voltage_read()
{
	uint16_t x;

	// set the analog reference here
	// just in case other ADC pins require another reference
	analogReference(INTERNAL);
	pinMode(PIN_12V_MONITOR, INPUT);

	x = analogRead(PIN_12V_MONITOR);
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
	x *= 10; // with the original equation, there's a 32-bit overflow problem, so we do the earlier math with smaller mutltiplication
	return x;
}
