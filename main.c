//
//	main.c
//
//	Michael Jean <michael.jean@shaw.ca>
//

#include <avr/interrupt.h>
#include <avr/io.h>

#include "can.h"
#include "can_config.h"

#include "adc.h"
#include "pressure.h"
#include "state.h"

//
//	System state handling functions are defined here. This variable is
//	referenced externally by the state management code.
//

void (*state_handlers[])(void) =
{
	idle_state_handler, 	/* state_idle */
	idle_state_handler, 	/* state_error_recoverable */
	idle_state_handler,		/* state_error_fatal */
	idle_state_handler,		/* state_pcal_request_min */
	idle_state_handler,		/* state_pcal_wait_min */
	idle_state_handler,		/* state_pcal_sample_min */
	idle_state_handler,		/* state_pcal_request_max */
	idle_state_handler,		/* state_pcal_wait_max */
	idle_state_handler,		/* state_pcal_sample_max */
	idle_state_handler,		/* state_pcal_update */
	idle_state_handler 		/* state_pcal_abort */
};

//
//	General-purpose interrupt handler that fires every millisecond. Calls
//	periodic interrupt handlers for each subsystem.
//

ISR (TIMER0_COMP_vect)
{
	pressure_periodic_interrupt_handler ();
}

//
//	The idle state handler runs when the system has nothing to do.
//

void
idle_state_handler (void)
{
	/* zzz... */
}

//
//	Initialize the general-purpose io ports. Sets up the output led
//	on PG3 as an output port and ties it high so it's not lit.
//

void
io_init (void)
{
	DDRG |= _BV (PG3);
	PORTG |= _BV (PG3);
}

//
//	Initialize the message objects.
//

void
mob_init (void)
{
	mob_config_t mob_config;

	mob_config.id_type = standard;
	mob_config.mask = 0x7FF;
	mob_config.tx_callback_ptr = 0;

	mob_config.id = (MODULE_ID << 8) | msg_id_pressure_calibration;
	mob_config.rx_callback_ptr = pressure_calibration_rx_callback;
	can_config_mob (mob_in_pressure_calibration, &mob_config);
	can_ready_to_receive (mob_in_pressure_calibration);

	mob_config.id = (MODULE_ID << 8) | msg_id_pressure_calibration;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_pressure_calibration, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_pressure_front;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_pressure_front, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_pressure_rear;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_pressure_rear, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_bias_calibration;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_in_bias_calibration, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_bias_calibration;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_bias_calibration, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_bias_adjust;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_in_bias_adjust, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_bias_adjust;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_bias_adjust, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_bias_position;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_rpl_bias_position, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_overtravel;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_overtravel, &mob_config);

	mob_config.id = (MODULE_ID << 8) | msg_id_error;
	mob_config.rx_callback_ptr = 0;
	can_config_mob (mob_out_error, &mob_config);
}

//
//	Initialize the general-purpose timer to interrupt every millisecond.
//

void
timer_init (void)
{
	TCCR0A = _BV (WGM01) | _BV (CS01) | _BV (CS00);
	OCR0A = 249;
	TIMSK0 = _BV (OCIE0A);
}

//
//	Program entry point here.
//

int
main (void)
{
	adc_init ();
	can_init ();

	io_init ();
	mob_init ();
	timer_init ();

	pressure_init ();

	sei ();

	for (;;)
		state_execute_current_state ();

	return 0;
}
