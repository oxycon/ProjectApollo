#include <gasboard7500E.h>
#include <SoftwareSerial.h>

uint32_t oxy_timestamp;
uint32_t oxy_timeout = OXY_HW_TIMEOUT_LONG;
bool oxy_low = false;
uint32_t oxy_low_time;
bool flow_low = false;
uint32_t flow_low_time;

SoftwareSerial* softSer;

void oxy_init()
{
	pinMode(PIN_O2SENS_RX, INPUT);
	pinMode(PIN_O2SENS_TX, OUTPUT);
	softSer = new SoftwareSerial(PIN_O2SENS_RX, PIN_O2SENS_TX);
	softSer->begin(O2SENSE_BAUD_RATE);
}

void oxy_task()
{
	if (softSer->available() > 0) // has at least one byte from UART
	{
		o2sens_feedUartByte(softSer->read()); // give byte to parser
		if (o2sens_hasNewData()) // has complete packet
		{
			o2sens_clearNewData();
			oxy_timestamp = now;
			current_faults &= ~FAULTCODE_OXY_HW_FAIL;
			oxy_timeout = OXY_HW_TIMEOUT_SHORT; // at least one packet received, so the next timeout can be shorter

			if (o2sens_getConcentration16() < THRESH_OXY_CONCENTRATION)
			{
				if (oxy_low == false)
				{
					// recently dipped below thresh, remember the time
					oxy_low_time = now;
				}
				oxy_low = true;
				if (TIME_HAS_ELAPSED(now, oxy_low_time, OXY_LOW_TIMEOUT))
				{
					// have been low for too long
					if ((current_faults & FAULTCODE_OXY_LOW) == 0)
					{
						// register the fault
						current_faults |= FAULTCODE_OXY_LOW;
						fault_save(current_faults);
					}
				}
			}
			else
			{
				// enough oxygen, clear the faults
				oxy_low = false;
				current_faults &= ~FAULTCODE_OXY_LOW;
			}

			if (o2sens_getFlowRate16() < THRESH_OXY_FLOWRATE)
			{
				if (flow_low == false)
				{
					// recently dipped below thresh, remember the time
					flow_low_time = now;
				}
				flow_low = true;
				if (TIME_HAS_ELAPSED(now, flow_low_time, FLOW_LOW_TIMEOUT))
				{
					// have been low for too long
					if ((current_faults & FAULTCODE_FLOW_LOW) == 0)
					{
						// register the fault
						current_faults |= FAULTCODE_FLOW_LOW;
						fault_save(current_faults);
					}
				}
			}
			else
			{
				// enough flow, clear the faults
				flow_low = false;
				current_faults &= ~FAULTCODE_FLOW_LOW;
			}
		}
	}

	if (TIME_HAS_ELAPSED(now, oxy_timestamp, oxy_timeout))
	{
		oxy_timeout = OXY_HW_TIMEOUT_LONG; // assume hardware disconnected and need a long reboot
		if ((current_faults & FAULTCODE_OXY_HW_FAIL) == 0)
		{
			// register the fault
			current_faults |= FAULTCODE_OXY_HW_FAIL;
			fault_save(current_faults);
		}
	}
}