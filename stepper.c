//
//	stepper.c
//
//	Michael Jean <michael.jean@shaw.ca>
//

#include <avr/io.h>
#include <util/delay.h>

#include "stepper.h"

void
stepper_init (void)
{
	DDRB |= _BV (STEPPER_ENABLE) | _BV (STEPPER_SLEEP) | _BV (STEPPER_MS2) |
			_BV (STEPPER_MS1) | _BV (STEPPER_MS1) | _BV (STEPPER_DIR) |
			_BV (STEPPER_RESET)	| _BV (STEPPER_STEP);

	PORTB = _BV (STEPPER_RESET) | _BV (STEPPER_SLEEP);
	_delay_ms (STEPPER_SLEEP_DELAY);
}

void
stepper_step (uint16_t steps, stepper_dir_t direction, float step_delay)
{
	uint16_t i;

	if (step_delay < STEPPER_STEP_DELAY)
		step_delay = STEPPER_STEP_DELAY;

	PORTB |= (direction == forward) ? 0x0 : _BV (STEPPER_DIR);
	_delay_us (STEPPER_DIR_DELAY);

	for (i = steps; i > 0; i--)
	{
		PORTB |= _BV (STEPPER_STEP);
		_delay_us (STEPPER_STEP_DELAY);

		PORTB &= ~_BV (STEPPER_STEP);
		_delay_ms (step_delay);
	}
}
