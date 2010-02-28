//
//	error.c
//
//	Michael Jean <michael.jean@shaw.ca>
//

#include <avr/io.h>
#include <util/delay.h>

#include "can.h"
#include "can_config.h"

#include "error.h"
#include "state.h"

static volatile err_code_t error_code = 0;

err_code_t
error_get_error_code (void)
{
	return error_code;
}

void
error_set_error_code (err_code_t new_error_code)
{
	error_code = new_error_code;
}

void
error_clear_error_code (void)
{
	error_code = 0;
}

void
error_broadcast_error_code (err_severity_t error_severity, err_code_t error_code)
{
	uint8_t data[2] = { error_severity, error_code };

	can_load_data (mob_out_error, data, 2);
	can_ready_to_send (mob_out_error);
}

void
error_fatal_error (void)
{
	error_broadcast_error_code (err_sev_fatal, error_code);

	can_init (); 	/* 1 */

	while (1)
	{
		PORTG ^= _BV (PG3);
		_delay_ms (250.0);
	}
}

void
error_recoverable_error (void)
{
	error_broadcast_error_code (err_sev_recoverable, error_code);
	state_transition (state_idle);
}
