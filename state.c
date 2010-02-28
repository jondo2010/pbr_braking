//
//	state.c
//
//	Michael Jean <michael.jean@shaw.ca>
//

#include <util/atomic.h>

#include "state.h"

static volatile state_t current_state = state_idle;

static volatile int 	transition_requested;
static volatile state_t transition_state;

static volatile int		isr_transition_requested;
static volatile state_t isr_transition_state;

extern static void (*state_handlers[])(void);

void
state_execute_current_state (void)
{
	state_handlers[current_state] ();

	ATOMIC_BLOCK (ATOMIC_RESTORESTATE)
	{
		if 		(isr_transition_requested)	current_state = isr_transition_state;
		else if (transition_requested)		current_state = transition_state;

		transition_requested = 0;
		isr_transition_requested = 0;
	}
}

state_t
state_get_current_state (void)
{
	return current_state;
}

void
state_transition (state_t new_state)
{
	transition_requested = 1;
	transition_state = new_state;
}

void
state_isr_transition (state_t new_state)
{
	isr_transition_requested = 1;
	isr_transition_state = new_state;
}
