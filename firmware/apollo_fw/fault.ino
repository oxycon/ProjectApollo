#include <EEPROM.h>
#include <avr/eeprom.h>

#define FAULT_EEPROM_END (E2END - sizeof(fault_t) - 1)

void fault_save(uint32_t x)
{
	static uint8_t seq = 0;
	uint16_t addr;
	fault_t tmp;
	if (x == 0) {
		return;
	}
	seq += 1;
	addr = fault_findNextSpot(&seq);
	if (seq >= 0xFF || seq == 0x00) {
		seq = 1;
	}
	tmp.seq = seq;
	tmp.odometer_hours = nvm.odometer_hours;
	tmp.millis = millis();
	tmp.fault_code = fault_code;
	eeprom_update_block((void*)&tmp, (const void *)i, sizeof(fault_t));
}

uint16_t fault_findNextSpot(uint8_t* seq)
{
	uint16_t i;
	uint8_t  maxseq = 0;
	uint16_t maxadr = FAULTS_START_ADDR;
	for (i = FAULTS_START_ADDR; i < FAULT_EEPROM_END; i += sizeof(fault_t))
	{
		fault_t tmp;
		eeprom_read_block((void*)&tmp, (const void *)i, sizeof(fault_t));
		if (tmp.seq == 0x00 || tmp.seq == 0xFF) // blank spot found
		{
			if (maxseq != 0) {
				*seq = maxseq + 1;
			}
			return i;
		}
		if (tmp.seq > maxseq) {
			maxseq = tmp.seq;
			maxadr = i;
		}
	}
	if (maxseq != 0) {
		*seq = maxseq + 1;
	}
	maxadr += sizeof(fault_t);
	if (maxadr < FAULT_EEPROM_END)
	{
		return maxadr;
	}
	return FAULTS_START_ADDR; // nothing is found, or overflow occured, write to start location as default
}

uint16_t fault_findEarliest()
{
	uint16_t i;
	uint8_t  minseq = 0xFF;
	uint16_t minadr = FAULTS_START_ADDR; // if nothing is found, return starting address
	for (i = FAULTS_START_ADDR; i < FAULT_EEPROM_END; i += sizeof(fault_t))
	{
		fault_t tmp;
		eeprom_read_block((void*)&tmp, (const void *)i, sizeof(fault_t));
		if (tmp.seq == 0x00 || tmp.seq == 0xFF) // blank spot found
		{
			break;
		}
		if (tmp.seq < minseq) {
			minseq = tmp.seq;
			minadr = i;
		}
	}
	return minadr;
}

void fault_printAll(void)
{
	uint16_t i, addr;
	fault_t tmp;
	bool looped = false;
	bool has = false;
	for (i = addr = fault_findEarliest(); ; ) // loop will exit when either a blank spot is found or when a loop is detected
	{
		fault_t tmp;
		eeprom_read_block((void*)&tmp, (const void *)i, sizeof(fault_t));
		if (tmp.seq == 0x00 || tmp.seq == 0xFF) // no more to print
		{
			break;
		}
		else if (i == addr)
		{
			if (looped != false)
			{
				break;
			}
			looped = true;
		}

		if (has == false)
		{
			Serial.println(F("faults: "));
		}
		has = true; // at this point, we definitely do have a fault registered in memory

		Serial.print(F("\t ")); Serial.print(tmp.seq, DEC); Serial.print(F(": "));
		Serial.print(tmp.odometer_hours, DEC); Serial.print(F(", "));
		Serial.print(tmp.millis, DEC); Serial.print(F(", 0x"));
		Serial.println(tmp.fault_code, HEX);

		i += sizeof(fault_t);
		if (i >= FAULT_EEPROM_END) { // overflow, roll-over the address
			i = 0;
		}
	}
	if (has == false)
	{
		Serial.println(F("no faults"));
	}
	else
	{
		Serial.println(F("end of faults"));
	}
}

void fault_clearAll()
{
	EEPROM.update(FAULTS_START_ADDR, 0); // clearing the very first sequence number will fool above functions
}
