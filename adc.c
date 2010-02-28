//
//	adc.h
//
//	Michael Jean <michael.jean@shaw.ca>
//

#include <avr/io.h>
#include "adc.h"

void
adc_init (void)
{
	ADCSRA |= _BV (ADEN) | _BV (ADPS2) | _BV (ADPS1) | _BV (ADPS0);
}

uint16_t
adc_get_sample (uint8_t channel)
{
	uint16_t sample = 0;

	ADMUX = channel;
	ADCSRA |= _BV (ADSC);
	loop_until_bit_is_set (ADCSRA, ADIF);

	sample = ADC;
	return sample;
}
