//
//	pressure.c
//
//	Michael Jean <michael.jean@shaw.ca>
//

#include <avr/io.h>

#include "can.h"
#include "can_config.h"

#include "eeprom.h"

#include "adc.h"
#include "error.h"
#include "pressure.h"
#include "state.h"

static volatile uint16_t front_pressure;
static volatile uint16_t rear_pressure;

static uint16_t front_min_pressure, front_max_pressure;
static uint16_t rear_min_pressure, rear_max_pressure;

static volatile uint16_t tmp_front_min_pressure, tmp_front_max_pressure;
static volatile uint16_t tmp_rear_min_pressure, tmp_rear_max_pressure;

void
pressure_init (void)
{
	pressure_load_front_calibration
		((uint16_t *)&front_min_pressure, (uint16_t *)&front_max_pressure);

	pressure_load_rear_calibration
		((uint16_t *)&rear_min_pressure, (uint16_t *)&rear_max_pressure);
}

uint16_t
pressure_sample_front_sensor (void)
{
	uint16_t sample, psi;

	sample = adc_get_sample (adc_chan_front_pressure);
	psi = pressure_convert_sample_to_psi (sample);

	return psi;
}

uint16_t
pressure_sample_rear_sensor (void)
{
	uint16_t sample, psi;

	sample = adc_get_sample (adc_chan_rear_pressure);
	psi = pressure_convert_sample_to_psi (sample);

	return psi;
}

uint16_t
pressure_convert_sample_to_psi (uint16_t sample)
{
	uint32_t converted;

	converted = (uint32_t)sample * PSI_PER_VOLT * 5 / 1024; /* 1 */
	return (uint16_t)converted;
}

//
//	1.	The full-scale voltage is 5V, and the range of the ADC is 2^10.
//

void
pressure_load_front_calibration (uint16_t *min, uint16_t *max)
{
	uint8_t	min_bytes[2], max_bytes[2];

	eeprom_read_many (front_min_pressure_addr, min_bytes, 2);
	eeprom_read_many (front_max_pressure_addr, max_bytes, 2);

	front_min_pressure = (uint16_t)(min_bytes[0] << 8) | min_bytes[1];
	front_max_pressure = (uint16_t)(max_bytes[0] << 8) | max_bytes[1];
}

void
pressure_load_rear_calibration (uint16_t *min, uint16_t *max)
{
	uint8_t	min_bytes[2], max_bytes[2];

	eeprom_read_many (rear_min_pressure_addr, min_bytes, 2);
	eeprom_read_many (rear_max_pressure_addr, max_bytes, 2);

	rear_min_pressure = (uint16_t)(min_bytes[0] << 8) | min_bytes[1];
	rear_max_pressure = (uint16_t)(max_bytes[0] << 8) | max_bytes[1];
}

void
pressure_store_front_calibration (uint16_t min, uint16_t max)
{
	uint8_t	min_bytes[2], max_bytes[2];

	min_bytes[0] = (uint8_t)(front_min_pressure >> 8);
	min_bytes[1] = (uint8_t)(front_min_pressure);

	max_bytes[0] = (uint8_t)(front_max_pressure >> 8);
	max_bytes[1] = (uint8_t)(front_max_pressure);

	eeprom_write_many (front_min_pressure_addr, min_bytes, 2);
	eeprom_write_many (front_max_pressure_addr, max_bytes, 2);
}

void
pressure_store_rear_calibration (uint16_t min, uint16_t max)
{
	uint8_t	min_bytes[2], max_bytes[2];

	min_bytes[0] = (uint8_t)(rear_min_pressure >> 8);
	min_bytes[1] = (uint8_t)(rear_min_pressure);

	max_bytes[0] = (uint8_t)(rear_max_pressure >> 8);
	max_bytes[1] = (uint8_t)(rear_max_pressure);

	eeprom_write_many (rear_min_pressure_addr, min_bytes, 2);
	eeprom_write_many (rear_max_pressure_addr, max_bytes, 2);
}

void
pressure_broadcast_pressure_readings (void)
{
	uint8_t front_data[6], rear_data[6];

	front_data[0] = (uint8_t)(front_pressure >> 8);
	front_data[1] = (uint8_t)(front_pressure);
	front_data[2] = (uint8_t)(front_min_pressure >> 8);
	front_data[3] = (uint8_t)(front_min_pressure);
	front_data[4] = (uint8_t)(front_max_pressure >> 8);
	front_data[5] = (uint8_t)(front_max_pressure);

	rear_data[0] = (uint8_t)(rear_pressure >> 8);
	rear_data[1] = (uint8_t)(rear_pressure);
	rear_data[2] = (uint8_t)(rear_min_pressure >> 8);
	rear_data[3] = (uint8_t)(rear_min_pressure);
	rear_data[4] = (uint8_t)(rear_max_pressure >> 8);
	rear_data[5] = (uint8_t)(rear_max_pressure);

	can_load_data (mob_out_pressure_front, front_data, 6);
	can_ready_to_send (mob_out_pressure_front);

	can_load_data (mob_out_pressure_rear, rear_data, 6);
	can_ready_to_send (mob_out_pressure_rear);
}

void
pressure_periodic_interrupt_handler (void)
{
	static uint16_t update_ticks = 0;
	static uint16_t broadcast_ticks = 0;

	if (PRESSURE_UPDATE_PERIOD && (++update_ticks == PRESSURE_UPDATE_PERIOD))
	{
		front_pressure = pressure_sample_front_sensor ();
		rear_pressure = pressure_sample_rear_sensor ();
		update_ticks = 0;
	}

	if (PRESSURE_BROADCAST_PERIOD && (++broadcast_ticks == PRESSURE_BROADCAST_PERIOD))
	{
		pressure_broadcast_pressure_readings ();
		broadcast_ticks = 0;
	}
}

void
pressure_calibration_rx_callback (uint8_t mob_index, uint32_t id, packet_type_t type)
{
	pcal_msg_t 	message;
	state_t		current_state;

	can_read_data (mob_index, (uint8_t *)&message, 1);
	current_state = state_get_current_state ();

	switch (command)
	{
		case pcal_msg_begin_calibration:

			if (current_state != state_idle)
			{
				error_set_error_code (err_cmd_unexpected);
				state_isr_transition (state_error_recoverable);
			}
			else
			{
				state_isr_transition (state_pcal_request_min);
			}

			break;

		case pcal_msg_abort_calibration:

			if
			(
				current_state != state_pcal_request_min 	&&
				current_state != state_pcal_wait_min 		&&
				current_state != state_pcal_sample_min 		&&
				current_state != state_pcal_request_max 	&&
				current_state != state_pcal_wait_max
			)
			{
				error_set_error_code (err_cmd_unexpected);
				state_isr_request_new_state (state_error_recoverable);
			}
			else
			{
				state_isr_request_new_state (state_pcal_abort);
			}

			break;

		case pcal_msg_min_pressure_applied:

			if (current_state != state_pcal_wait_min)
			{
				error_set_error_code (err_cmd_unexpected);
				state_isr_request_new_state (state_error_recoverable);
			}
			else
			{
				state_isr_request_new_state (state_pcal_sample_min);
			}

			break;

		case pcal_msg_max_pressure_applied:

			if (current_state != state_pcal_wait_max)
			{
				error_set_error_code (err_cmd_unexpected);
				state_isr_request_new_state (state_error_recoverable);
			}
			else
			{
				state_isr_request_new_state (state_pcal_sample_max);
			}

			break;

		default:

			error_set_error_code (err_cmd_unknown);
			state_isr_request_new_state (state_error_recoverable);

			break;
	}

	can_ready_to_receive (mob_in_pressure_calibration);
}

void
pressure_calibration_request_min (void)
{
	pcal_msg_t message = pcal_msg_apply_min_pressure;

	can_load_data (mob_out_pressure_calibration, &message, 1);
	can_ready_to_send (mob_out_pressure_);

	state_transition (state_pcal_wait_min);
}

void
pressure_calibration_wait_min (void)
{
	/* zzz... */
}

void
pressure_calibration_sample_min (void)
{

}


