//
//	error.h
//
//	Michael Jean <michael.jean@shaw.ca>
//

#ifndef _ERROR_H
#define _ERROR_H

//
//	The error packet is made up of two payload bytes. The first byte is the
//	severity. The second byte is the actual error code. Both are defined below.
//

typedef enum err_severity_t
{
	err_sev_recoverable				= 0x00,
	err_sev_fatal					= 0x01
}
err_severity_t;

typedef enum err_code_t
{
	err_cmd_unexpected				= 0x01,
	err_cmd_unknown					= 0x02,
	err_pcal_minf_gt_maxf			= 0x03,
	err_pcal_minr_gt_maxr			= 0x04,
	err_pcal_deltaf_lt_threshf		= 0x05,
	err_pcal_deltar_lt_threshr		= 0x06
}
err_code_t;

//
//	Return the currently set error code.
//

err_code_t
error_get_error_code (void);

//
//	Set the current error code to `error_code'.
//

void
error_set_error_code
(
	err_code_t error_code
);

//
//	Clear the current error code.
//

void
error_clear_error_code (void);

//
//	Broadcast an error code `error_code' over the CAN channel. Set the error
//	severity to `error_severity'.
//

void
error_broadcast_error_code
(
	err_severity_t 	error_severity,
	err_code_t 		error_code
);

//
//	Handle a fatal error state. Announce the error over the CAN channel.
//	Enter a failure mode and blink the status LED rapidly. Disable all
//	incoming and outgoing CAN communications.
//

void
error_fatal_error (void);

//
//	Handle a non-fatal error state. Announce the error over the CAN channel.
//	Return to the idle state.
//

void
error_recoverable_error (void);

#endif
