//
//	pressure.h
//
//	Michael Jean <michael.jean@shaw.ca>
//

#ifndef _PRESSURE_H
#define _PRESSURE_H

#include <inttypes.h>

//
//	The general-purpose timer runs at 1 ms and calls the periodic interrupt
//	handler. This handler updates the current pressure readings and broadcasts
//	them. The rate at which each occurs is tuneable below.
//
//	N.B. You can set any of these to zero to stop it from occuring.
//

#define	PRESSURE_UPDATE_PERIOD 		10 		/* ms */
#define	PRESSURE_BROADCAST_PERIOD 	1000 	/* ms */

//
//	Pressure calibration must pass several validation rules. There must
//	be a minimum difference between the minimum and maximum applied
//	pressures.
//

#define PRESSURE_CALIBRATION_MIN_DIFF	100		/* psi */

//
//	The pressure samples come in from a +5V range sensor through the onboard
//	10-bit DAC. Pressure is reported and tracked throughout the program in psi.
//

#define	PSI_PER_VOLT 	300

//
//	Pressure calibration values are stored in the eeprom at fixed addresses.
//	Since each value is 16-bits long, two bytes are required per parameter.
//

typedef enum pressure_eeprom_addr_t
{
	front_min_pressure_addr	= 0x00,
	front_max_pressure_addr	= 0x02,
	rear_min_pressure_addr	= 0x04,
	rear_max_pressure_addr	= 0x06
}
pressure_eeprom_addr_t;

//
//	Below are the pressure calibration messages. All pressure calibration
//	commands, incoming and outgoing, use a single byte message to coordinate
//	the calibration process.
//

typedef enum pcal_msg_t
{
	pcal_msg_begin_calibration		= 0x00,		/* driver -> brake module */
	pcal_msg_abort_calibration		= 0x01,		/* driver -> brake module */
	pcal_msg_apply_min_pressure		= 0x02,		/* driver <- brake module */
	pcal_msg_min_pressure_applied	= 0x03,		/* driver -> brake module */
	pcal_msg_apply_max_pressure		= 0x04,		/* driver <- brake module */
	pcal_msg_max_pressure_applied	= 0x05,		/* driver -> brake module */
	pcal_msg_calibration_ok			= 0x06,		/* driver <- brake module */
	pcal_msg_calibration_failed		= 0x07		/* driver <- brake module */
}
pcal_msg_t;

//
//	Initialize pressure subsystem of the controller. Read the pressure
//	calibration values from the eeprom.
//

void
pressure_init (void);

//
//	Take a reading from the front pressure sensor. Return the reading.
//	Reading is output in psi.
//

uint16_t
pressure_sample_front_sensor (void);

//
//	Take a reading from the rear pressure sensor. Return the reading.
//	Reading is output in psi.
//

uint16_t
pressure_sample_rear_sensor (void);

//
//	Convert a sampled sensor voltage reading `sample' into psi. Use the
//	scaling constant `PSI_PER_VOLT', defined above.
//

uint16_t
pressure_convert_sample_to_psi
(
	uint16_t sample
);

//
//	Load front pressure calibration values from eeprom into the variables
//	pointed to by `min' and `max'. Calibration values are in psi.
//

void
pressure_load_front_calibration
(
	uint16_t	*min,
	uint16_t	*max
);

//
//	Load rear pressure calibration values from eeprom into the variables
//	pointed to by `min' and `max'. Calibration values are in psi.
//

void
pressure_load_rear_calibration
(
	uint16_t	*min,
	uint16_t	*max
);

//
//	Store front pressure calibration values into eeprom from the variables
//	pointed to by `min' and `max'. Calibration values are in psi.
//


void
pressure_store_front_calibration
(
	uint16_t	min,
	uint16_t	max
);

//
//	Store rear pressure calibration values into eeprom from the variables
//	pointed to by `min' and `max'. Calibration values are in psi.
//

void
pressure_store_rear_calibration
(
	uint16_t	min,
	uint16_t	max
);

//
//	Broadcast pressure readings over the CAN bus to the other modules.
//
//	Two packets are broadcast, one for the front pressure, and one for the
//	right pressure. Separate IDs are used for each packet, and they are
//	defined in `can_config.h'.
//
//	Each packet contains six bytes:
//
//	0+1: The MSB and LSB of the 16-bit pressure reading (in psi)
//	2+3: The MSB and LSB of the 16-bit calibrated minimum pressure (in psi)
//	4+5: The MSB and LSB of the 16-bit calibrated maximum pressure (in psi)
//

void
pressure_broadcast_pressure_readings (void);

//
//	This function is fired every millisecond by the general-purpose timer.
//	It is used to periodically broadcast the pressure over the CAN bus.
//

void
pressure_periodic_interrupt_handler (void);

//
//	Pressure calibration message received callback function.
//

void
pressure_calibration_rx_callback
(
	uint8_t 		mob_index,
	uint32_t 		id,
	packet_type_t 	type
);

//
//	Broadcast a request to apply minimum braking pressure over the CAN
//	channel. Transition into waiting for minimum pressure.
//

void
pressure_calibration_request_min (void);

//
//	Wait for minimum braking pressure to be applied. Just a `nop' style
//	function like the idle state.
//

void
pressure_calibration_wait_min (void);

//
//	Sample the minimum braking pressure and record the values in temporary
//	variables for later use. Transition into requesting maximum pressure.
//

void
pressure_calibration_sample_min (void);

//
//	Broadcast a request to apply minimum braking pressure over the CAN
//	channel. Transition into waiting for minimum pressure.
//

void
pressure_calibration_request_min (void);

//
//	Wait for minimum braking pressure to be applied. Just a `nop' style
//	function like the idle state.
//

void
pressure_calibration_wait_min (void);

//
//	Sample the minimum braking pressure and record the values in temporary
//	variables for later use. Transition into requesting maximum pressure.
//

void
pressure_calibration_sample_min (void);

#endif
