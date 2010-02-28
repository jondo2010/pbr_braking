//
//	adc.h
//
//	Michael Jean <michael.jean@shaw.ca>
//

#ifndef _ADC_H
#define _ADC_H

#include <inttypes.h>

//
//	Analogue-to-Digital Channel Allocations
//

typedef enum adc_chan_t
{
	adc_chan_front_pressure	= 0,
	adc_chan_rear_pressure	= 1
}
adc_chan_t;

//
//	Initialize the ADC subsystem.
//

void
adc_init (void);

//
//	Return a 10-bit sample from the ADC channel indexed by `channel'.
//
//	N.B. 	This is a blocking operation but does not require interrupts
//			to be disabled.
//

uint16_t
adc_get_sample
(
	uint8_t channel
);

#endif
