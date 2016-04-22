#ifndef _COMMS_H
#define _COMMS_H
#include "Config.h"
namespace Comms {

	/// <summary>
	/// Definition of all function codes used for communication with the buggy
	/// </summary>
	enum FunctionCodes : byte
	{
		/// <summary>
		/// Initial communication with the buggy
		/// </summary>
		ConnectionRequest = 0x01,

		/// <summary>
		/// Expected receved frame after sending a handshake request
		/// </summary>
		ConnectionAccept = 0x02,

		/// <summary>
		/// Request current status update from the buggy
		/// </summary>
		CurrentStatus = 0x03,

		/// <summary>
		/// Requests a variable value from the buggy for a particular parameter
		/// </summary>
		ParameterRequest = 0x04,

		/// <summary>
		/// Forcibily sets a parameter's value
		/// </summary>
		ParameterSet = 0x05,

		/// <summary>
		/// Used for sending the buggy the planned route
		/// </summary>
		RouteCmd = 0x06,

		/// <summary>
		/// Used for resetting the current route assembled by the buggy
		/// </summary>
		RouteRst = 0x07,

		/// <summary>
		/// Used for starting the buggies motion
		/// </summary>
		Go = 0x08,

		/// <summary>
		/// Achknowlege command function code
		/// </summary>
		Acknowledge = 0x10,

		/// <summary>
		/// Change the current phase
		/// </summary>
		ChangePhase = 0x11,

		/// <summary>
		/// Indicates that the last frame was incorrect
		/// </summary>
		Error = 0x12,

		/// <summary>
		/// This will run Buggy.Go with a single character
		/// </summary>
		ManualCtrl = 0x12,

		StringDMP = 0x13,
		MovementComplete = 0x14,

		BoxValue1 = 0x15,
		BoxValue2 = 0x16,
		BoxValue3 = 0x17,

		/// <summary>
		/// Upon the reciept of this function code. The buggy will immediately stop the wheels, and return to an "idle" state
		/// </summary>
		EmergencyStop = 0xFF
	};

	/// <summary>
	/// Definition of all parameter data types. Used for defining how many bytes need to be allocated in the frame for the data
	/// </summary>
	enum ParameterType : byte
	{
		/// <summary>
		/// 8 bit unsigned integer
		/// </summary>
		uint8_t = 0x01,

		/// <summary>
		/// 16 bit unsigned integer
		/// </summary>
		uint16_t = 0x02,

		/// <summary>
		/// 32 bit unsigned integer
		/// </summary>
		uint32_t = 0x03,

		/// <summary>
		/// Signed integer 32 bit
		/// </summary>
		SignedInteger = 0x04,

		/// <summary>
		/// Character 8-bit
		/// </summary>
		Char = 0x05
	};

	enum Phase {

		/// <summary>
		/// Idle State
		/// </summary>
		Idle = 0x01,

		/// <summary>
		/// Parameter Setup State
		/// </summary>
		ParameterSetup = 0x02,

		/// <summary>
		/// Route Download State
		/// </summary>
		RouteDownload = 0x03,

		/// <summary>
		/// Operational State
		/// </summary>
		Operational = 0x04

	};


	/// <summary>
	/// Various errorcodes that the buggy will use to tell the PC what went wrong
	/// </summary>
	enum ErrorCodes {

		/// <summary>
		/// Sent whenever a command is received that is not one of the predefined commands as described in funcioncodes
		/// </summary>
		UnknownCmd = 0x01,

		/// <summary>
		/// Sent whenever a command is issued when not in the correct phase
		/// </summary>
		IncorrectPhase = 0x02



	};

	enum Directions
	{
		Forward = 0x01,
		Backward = 0x02,
		TurnLeft = 0x03,
		TurnRight = 0x04


	};





}

#endif;