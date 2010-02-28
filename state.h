//
//	state.h
//
//	Michael Jean <michael.jean@shaw.ca>
//

#ifndef _STATE_H
#define _STATE_H

typedef enum state_t
{
	state_idle,					/* idle state */
	state_error_recoverable,	/* recoverable error occured */
	state_error_fatal,			/* fatal error occured, halt */
	state_pcal_request_min,		/* request minimum pressure */
	state_pcal_wait_min,		/* wait for minimum pressure reply */
	state_pcal_sample_min,		/* sample minimum pressure */
	state_pcal_request_max,		/* request maximum pressure */
	state_pcal_wait_max,		/* wait for maximum pressure reply */
	state_pcal_sample_max,		/* sample maximum pressure */
	state_pcal_update,			/* store new pressure calibration values in eeprom */
	state_pcal_abort			/* abort calibration routine */
}
state_t;

void
state_execute_current_state (void);

state_t
state_get_current_state (void);

void
state_transition
(
	state_t new_state
);

void
state_isr_transition
(
	state_t new_state
);

#endif
