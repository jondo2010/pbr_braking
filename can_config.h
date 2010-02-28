//
//	can_config.h
//
//	Michael Jean <michael.jean@shaw.ca>
//

#ifndef _CAN_CONFIG_H
#define _CAN_CONFIG_H

#define	MODULE_ID	0x02

typedef enum can_mob_t
{
	mob_in_pressure_calibration,
	mob_out_pressure_calibration,
	mob_out_pressure_front,
	mob_out_pressure_rear,
	mob_in_bias_calibration,
	mob_out_bias_calibration,
	mob_in_bias_adjust,
	mob_out_bias_adjust,
	mob_rpl_bias_position,
	mob_out_overtravel,
	mob_out_error
}
mob_id_t;

typedef enum can_message_id_t
{
	msg_id_pressure_calibration		= 0x00,
	msg_id_pressure_front			= 0x01,
	msg_id_pressure_rear			= 0x02,
	msg_id_bias_calibration			= 0x10,
	msg_id_bias_position			= 0x11,
	msg_id_bias_adjust				= 0x12,
	msg_id_overtravel				= 0x20,
	msg_id_error					= 0x30
}
can_message_id_t;

#endif
