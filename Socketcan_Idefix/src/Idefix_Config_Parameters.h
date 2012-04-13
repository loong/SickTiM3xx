/*
 * Idefix_Configuration.h
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#ifndef IDEFIX_CONFIGURATION_H_
#define IDEFIX_CONFIGURATION_H_




#define CAN_INTERFACE_NAME "can0"

#include <cmath>
#include <stdint.h>
#include "CAN_types_and_tools.h"

namespace idefix {




	using namespace sfzcan;

	/**
	 * \brief Use this class to provide any Idefic-Configuratio Parameters
	 */

	class I_C{



	public:


// All Drives Parameter

			static const uint_t ALL_DRIVES_THREAD_SLLEP_TIME=50000;

// Player Parameter

			static const uint32_t	SWITCH_TO_GAMEPAD_MODE_BUTTON=1; // Button 1
			static const uint32_t	SWITCH_TO_AUTO_MODE_BUTTON=2; // Button 2
			static const uint32_t	START_HOMING_MODE_BUTTON_FRONT=4; //Button 3
			static const uint32_t	START_HOMING_MODE_BUTTON_REAR=8; //Button 4
			static const uint32_t 	SWITCH_TO_DISABLE_DRIVES=0x0010; 	// Button 5
			static const uint32_t 	SWITCH_TO_REENABLE_DRIVES=0x0020; 	// Button 6



			// Node ID Zuordnung
			static const canid_t NODE_LENKUNG_VORNE = 1; //motor 1 klein
			static const canid_t NODE_ANTRIEB_VORNE = 4; //motor 1 groß
			static const canid_t NODE_LENKUNG_HINTEN = 2; //motor 2 klein
			static const canid_t NODE_ANTRIEB_HINTEN = 3; //motor 2 groß

			static const canid_t NODE_AIM04=0x65;	// Eckelmann AIM04




			// Motion Profile Types
			static const msgdata_t MOTION_PROFILE_TYPE_LINEAR_RAMP=0;
			static const msgdata_t MOTION_PROFILE_TYPE_SINUS_RAMP=1;




		// Steering Drive Parameters

			static const msgdata_t    S_desiredVelocity_front=4500;
//			static const msgdata_t    S_maxFollowingErrorQC=20000;
			static const msgdata_t    S_maxFollowingErrorQC=256*256*256*256L-1; // Switch off of following control

			static const msgdata_t    S_maxVelocity=5000;
			static const msgdata_t    S_acceleration=7000;
			static const msgdata_t    S_deceleration=7000;
			static const msgdata_t    S_quickStopdeceleration=10000;
			static const msgdata_t    S_motionProfileType=MOTION_PROFILE_TYPE_LINEAR_RAMP;


			// Homing Parameters

			static const msgdata_t H_homeOffset_front=49000;
			static const msgdata_t H_homeOffset_rear=49000;

			static const msgdata_t H_maxProfileVelocity=2000;
			static const msgdata_t H_quickStopDeceleration=1000;
			static const msgdata_t H_speedForSwitchSearch=1000;
			static const msgdata_t H_speedForZeroSearch=140;
			static const msgdata_t H_homingAcceleration=1500;
			static const msgdata_t H_currentThreshHoldmilliAmp=1800;
			static const msgdata_t H_homePosition=0;
			static const msgdata_t H_SICHERHEITSABSTAND_QC = 300;

			// Steering Drive Parameters
			static const msgdata_t	  S_pos_Limit_Delta=0;
			static const msgdata_t    S_minPositionLimit_front=-H_homeOffset_front-S_pos_Limit_Delta;
			static const msgdata_t    S_maxPositionLimit_front=H_homeOffset_front+S_pos_Limit_Delta;

			static const msgdata_t    S_minPositionLimit_rear=-H_homeOffset_rear-S_pos_Limit_Delta;
			static const msgdata_t    S_maxPositionLimit_rear=H_homeOffset_rear+S_pos_Limit_Delta;



			// @TODO Adjust Values for Profile Homing
//			static const msgdata_t MAX_FOLLOWING_ERROR_QC = 1500;
//			static const msgdata_t HOME_OFFSET=9500;
//			static const msgdata_t SPEED_FOR_SWITCH_SEARCH = 1000;
//			static const msgdata_t SPEED_FOR_ZERO_SEARCH = 400;
//			static const msgdata_t HOMING_ACCELERATION = 500;
//			static const msgdata_t CURRENT_THRESHHOLD_MILLI_AMP = 1300;
//			static const msgdata_t HOME_POSITION = 0;
			// Homing Methods
			static const msgdata_t HOMING_METHOD_ACTUAL_POSITION=35;
			static const msgdata_t HOMING_METHOD_CURRENT_THRESHHOLD_POSITIVE_SPEED=-0x3;
			static const msgdata_t HOMING_METHOD_CURRENT_THRESHHOLD_NEGATIVE_SPEED=-0x4;




			// Drive Parameter
			//static const msgdata_t    D_maxFollowingErrorQC=50000;
			static const msgdata_t    D_maxFollowingErrorQC=256*256*256*256L-1; // Switch off of following control

			static const msgdata_t    D_desiredVelocity=3500;

			static const msgdata_t    D_maxVelocity=7000;
			static const msgdata_t    D_acceleration=3200;
			static const msgdata_t    D_deceleration=3200;
			static const msgdata_t    D_minPositionLimit=(msgdata_t)-128*256*256*256;
			static const msgdata_t    D_maxPositionLimit=(msgdata_t)128*256*256*256-1;
			static const msgdata_t    D_quickStopdeceleration=5000;
			static const msgdata_t    D_motionProfileType=MOTION_PROFILE_TYPE_LINEAR_RAMP;





		//EPOS Configuration Constants

			// Statusword Bits
			static const msgdata_t STATUSWORD_FAULT_BIT=0x8;
			static const msgdata_t STATUSWORD_OFFSET_CURRENT_BIT=0x100;
			static const msgdata_t STATUSWORD_OPERATION_ENABLE_BIT=0x4;
			static const msgdata_t STATUSWORD_POSITION_REACHED_BIT=0x0400;
			static const msgdata_t STATUSWORD_HOMING_ATTAINED_BIT=0x1000;


			//Controlword Bits

			static const msgdata_t CONTROLWORD_ENABLE_OPERATION_BIT=0x8;
			static const msgdata_t CONTROLWORD_QUICK_STOP_BIT=0x04;
			static const msgdata_t CONTROLWORD_ENABLE_VOLTAGE_BIT=0x02;  //
			static const msgdata_t CONTROLWORD_SWITCH_ON_BIT=0x01;  //SHUTDOWN

			static const msgdata_t CONTROLWORD_START_HOMING_MODE=0x1F;

			static const msgdata_t CONTROLWORD_FAULT_RESET_BIT=0x80;
			static const msgdata_t CONTROLWORD_HALT_PROFILE_VELOCITY_MODE=0x10F;
			static const msgdata_t CONTROLWORD_START_MOVE=0x0F;


			// Modes of Operation
			static const msgdata_t PROFILE_POSITION_MODE=0x1;
			static const msgdata_t PROFILE_VELOCITY_MODE=0x3;
			static const msgdata_t PROFILE_HOMING_MODE=0x6;


			static const msgdata_t POS_MODE_CONTROLWORD_ABSOLUTE=0x1F;
			static const msgdata_t POS_MODE_CONTROLWORD_ABSOLUTE_IMMEDEATE=0x3F;
			static const msgdata_t POS_MODE_CONTROLWORD_RELATIVE=0x5F;
			static const msgdata_t POS_MODE_CONTROLWORD_RELATIVE_IMMEDEATE=0x7F;


			// Gear reduction (Getriebe Untersetzung)

			static const double DIFFERENTIAL_GEAR_REDUCTION=1.0/3.2;
			static const double MAIN_DRIVE_GEAR_REDUCTION=1.0/12.0;
			static const double MAIN_DRIVE_QC_PER_TURN=256.0;
			static const double WHEEL_DIAMETER_M=0.170;
			static const double QC_TO_METER_FACTOR_DRIVES=1.0/(3.2*12*1024)*M_PI*0.170;
			//DIFFERENTIAL_GEAR_REDUCTION;
					//1.0/MAIN_DRIVE_QC_PER_TURN*MAIN_DRIVE_GEAR_REDUCTION*DIFFERENTIAL_GEAR_REDUCTION;
			static const double VELOCITY_MS_TO_RPM_FACTOR=(60*3.2*12)/(M_PI*0.17);


			// Drive: Profile Position Mode: maximal Steering angle deviation
			// before steering angle is reached
			// position is reached
			static const double DELTA_FOR_STEERING_ANGLE_REACHED=0.02;


			//
			//  Old ROBO_II Parameter
			//
			//
			//
			//
			//		// TODO WERTE überprüfen
			//		static const msgdata LENKUNG_MAX_DEFAULT_VELOCITY=5000;
			//		static const msgdata ANTRIEB_MAX_DEFAULT_VELOCITY=7000;
			//		static const msgdata MCD60W_MAX_DEFAULT_VELOCITY=12000;
			//
			//		static const msgdata LENKUNG_MAX_DEFAULT_ACCELERATION=7000;
			//		static const msgdata ANTRIEB_MAX_DEFAULT_ACCELERATION=1000;
			//		static const msgdata MCD60W_MAX_DEFAULT_ACCELERATION=10000;
			//
			//		static const msgdata LENKUNG_MAX_DEFAULT_DECELERATION=10000;
			//		static const msgdata ANTRIEB_MAX_DEFAULT_DECELERATION=10000;
			//		static const msgdata MCD60W_MAX_DEFAULT_DECELERATION=10000;
			//
			//		static const msgdata LENKUNG_MAX_DEFAULT_QUICKSTOP_DECELERATION=10000;
			//		static const msgdata ANTRIEB_MAX_DEFAULT_QUICKSTOP_DECELERATION=10000;
			//		static const msgdata MCD60W_MAX_DEFAULT_QUICKSTOP_DECELERATION=10000;
			//
			//
			//		static const msgdata LENKUNG_MAX_DEFAULT_POSITION_LIMIT=(msgdata)128*256*256*256-1;
			//		static const msgdata ANTRIEB_MAX_DEFAULT_POSITION_LIMIT=(msgdata)128*256*256*256-1;
			//		static const msgdata MCD60W_MAX_DEFAULT_POSITION_LIMIT=(msgdata)128*256*256*256-1;
			//
			//		static const msgdata LENKUNG_MIN_DEFAULT_POSITION_LIMIT=(msgdata)-128*256*256*256;
			//		static const msgdata ANTRIEB_MIN_DEFAULT_POSITION_LIMIT=(msgdata)-128*256*256*256;
			//		static const msgdata MCD60W_MIN_DEFAULT_POSITION_LIMIT=(msgdata)-128*256*256*256;
			//





	};


}  // namespace idefix

#endif /* IDEFIX_CONFIGURATION_H_ */


