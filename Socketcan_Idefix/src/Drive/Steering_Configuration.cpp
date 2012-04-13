/*
 * Drive_Configuration.cpp
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */



#include "Steering_Configuration.h"




namespace idefix {



	Steering_Configuration::Steering_Configuration() {
		// TODO Auto-generated constructor stub

	}

	Steering_Configuration::~Steering_Configuration() {
		// TODO Auto-generated destructor stub
	}








	void Steering_Configuration::configure_Drive(CAN_SDO_Socket& s, CAN_PDO_Socket& t)throw(E_SDO_Exception){


		/*
		 * PDO Mapping Konfiguration so, dass
		 * RxPDO1 die Motorgeschwindigkeit festlegt und
		 * RxPDO2 eine  neue Position festlegt
		 *
		 * TxPDO1  EPOS sendet die aktuelle Geschwindigkeit
		 * und die aktuelle Position
		 * auf SYNC Anforderung
		 *
		 * TxPDO2 das Statusword
		 *
		 * Information about Object mapping can be found in
		 * EPOS Application Note CANopen Basic Information -E.pdf
		 * page 12 and EPOS Firmware Specification
		 */

		/*
		 * **************************
		 * RxPDO Configuration
		 * (The Rx Means EPOS Receives the Message)
		 * **************************
		 */
			/*
			 * Which RxPDOs are valid? (only RxPDO1 and RxPDO2)
			 *
			 * Configure 32-Bit COB-ID used by RxPDO1 up to  RxPD4
			 *
			 * Bit 31 set: not valid so COB-ID starts with 0x80....
			 * Bit 31 not set: valid so COB-ID starts with 0x00....
			 */

		if(isStatusFault(s,t)){
			resetFault(s,t);
		}

		t.sendResetCommunicationMSG();
		wait_for_reaching_Preoperational_Mode(s,t);
		t.sendEnterPreOperationalMode();
		wait_for_reaching_Preoperational_Mode(s,t);



// Disabeling PDO1 und PDO2

// Setting Receive PDOs Parameter

		// Setting CAN_Object Id and Valid RxPDO1
		std::auto_ptr<CAN_EDS_SDO> eds_ptr;
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Parameter_COB_ID_used_by_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x00000200+ t.get_node_id()
				);
		s.sendSDO(*eds_ptr);


		// Setting CAN_Object Id and In Valid RxPDO2
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_2_Parameter_COB_ID_used_by_RxPDO_2,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x80000000 +t.get_node_id()
				);
		s.sendSDO(*eds_ptr);



		// Setting CAN_Object Id and In Valid RxPDO3
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_3_Parameter_COB_ID_used_by_RxPDO_3,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x80000000+ t.get_node_id()
				);
		s.sendSDO(*eds_ptr);

		// Setting CAN_Object Id and In Valid RxPDO4
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_4_Parameter_COB_ID_used_by_RxPDO_4,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x80000000+ t.get_node_id()
				);
		s.sendSDO(*eds_ptr);



		// Setting Trasmission Type asynchron (0xFF)
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Parameter_Transmission_Type,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x1
				);
		s.sendSDO(*eds_ptr);


		// Setting Trasmission Type asynchron (0xFF)
//		eds_ptr=CAN_Config_EDS::get_EDS(
//				Epos_24_5_Receive_PDO_2_Parameter_Transmission_Type,
//				t.get_node_id(),
//				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//				0xFF
//				);
//		n.sendSDO(*eds_ptr);

		// Mapping the RxPDO1


			//	  One has to disable the PDO before mapping the Objects
			//	  by setting the Number of Object to 0

		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x0
				);
		s.sendSDO(*eds_ptr);

		// Mapping Controlword
//
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_1st_Mapped_Object_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x60400010 // Controlword
				);
		s.sendSDO(*eds_ptr);

		//Mapping Target Position
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_2nd_Mapped_Object_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x607A0020 //Targetposition
				);
		s.sendSDO(*eds_ptr);

		// Number of Mapped Objects in RxPDO1
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				2
				);
		s.sendSDO(*eds_ptr);


	// Mapping the RxPDO2


		//	  One has to disable the PDO before mapping the Objects
		//	  by setting the Number of Object to 0
//		eds_ptr=CAN_Config_EDS::get_EDS(
//				Epos_24_5_Receive_PDO_2_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_2,
//				t.get_node_id(),
//				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//				0
//				);
//		s.sendSDO(*eds_ptr);
//
//
//	// Mapping Controlword
//
//		eds_ptr=CAN_Config_EDS::get_EDS(
//				Epos_24_5_Receive_PDO_2_Mapping_1st_Mapped_Object_in_RxPDO_2,
//				t.get_node_id(),
//				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//				0x60400010  //Controlword
//				);
//		s.sendSDO(*eds_ptr);
//
//
//	// Mapping Target Velocity
//
//		eds_ptr=CAN_Config_EDS::get_EDS(
//				Epos_24_5_Receive_PDO_2_Mapping_2nd_Mapped_Object_in_RxPDO_2,
//				t.get_node_id(),
//				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//				0x60FF0020 // TargetVelocity
//				);
//		s.sendSDO(*eds_ptr);
//
//		// Number of Mapped Objects in RxPDO1
//		eds_ptr=CAN_Config_EDS::get_EDS(
//				Epos_24_5_Receive_PDO_2_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_2,
//				t.get_node_id(),
//				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//				2
//				);
//		s.sendSDO(*eds_ptr);


		/*
		 * **************************
		 * TxPDO Configuration:
		 * Two Object are mapped:
		 * Actual Position and actualVelocity
		 * in one PDO
		 * **************************
		 */

			/*
			 * Which TxPDOs are valid? (only TxPDO1)
			 * and which should Transmit data after a remote
			 * Transmission request?
			 *
			 * COB-ID used by TxPDO1 up to  RxPD4
			 * Bit 31 set: not valid so COB-ID starts with 0x80....
			 * Bit 31 not set: valid so COB-ID starts with 0x00....
			 *
			 * Bit 30 set: no RTR allowed
			 * Bit 30 not set: RTR allowed
			 *
			 * For valid TxPDO1 with no RTR allowed COB ID is 0x40000180+nodeID
			 * Invalid TxPDO1 with no RTR allowed COB ID is 0xC0000180+nodeID
			 */


		// PDO 1 and 2 setting Valid and  COB
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_1_Parameter_COB_ID_used_by_TxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x40000180 +t.get_node_id()
				);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_2_Parameter_COB_ID_used_by_TxPDO_2,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x40000280 +t.get_node_id()
				);
		s.sendSDO(*eds_ptr);



		// PDO 3 and 4  setting InValid
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_3_Parameter_COB_ID_used_by_TxPDO_3,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x80000000
				);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_4_Parameter_COB_ID_used_by_TxPDO_4,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x80000000
				);
		s.sendSDO(*eds_ptr);

		// Syncron sending of PDO 1 and PDO 2
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_1_Parameter_Transmission_Type,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x1
				);
		s.sendSDO(*eds_ptr);

		// Syncron sending of PDO 1 and PDO 2
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_2_Parameter_Transmission_Type,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x1
				);
		s.sendSDO(*eds_ptr);


		// Mapping of velocity and position to PDO1
		// Disabeling PDO 1
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_1_Mapping_Number_of_Mapped_Application_Objects_in_TxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0
				);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_1_Mapping_1st_Mapped_Object_in_TxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x606C0020 // Velocity actual Value
				);
		s.sendSDO(*eds_ptr);


		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_1_Mapping_2nd_Mapped_Object_in_TxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x60640020  // Position actual Value
				);
		s.sendSDO(*eds_ptr);


		// Enabeling PDO 1
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_1_Mapping_Number_of_Mapped_Application_Objects_in_TxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				2 // Two Mapped Objects
				);
		s.sendSDO(*eds_ptr);

		// Mapping of Statusword to PDO2
		// Disabeling PDO 1
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_2_Mapping_Number_of_Mapped_Application_Objects_in_TxPDO_2,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0  // Mapping Disabled
				);
		s.sendSDO(*eds_ptr);


		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_2_Mapping_1st_Mapped_Object_in_TxPDO_2,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x60410010 // statusword
				);
		s.sendSDO(*eds_ptr);

		// Enabeling PDO 1
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Transmit_PDO_2_Mapping_Number_of_Mapped_Application_Objects_in_TxPDO_2,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				1);  // One Mapped Objects
		s.sendSDO(*eds_ptr);



			t.sendStartRemoteNodeMSG();





	}




	void Steering_Configuration::setProfilePositionMode(CAN_SDO_Socket& s,CAN_PDO_Socket& t, robo_mount_pos_t mount_pos)throw(E_CAN_IO_Exception)
	{

		if(isStatusFault(s,t)){
			resetFault(s,t);
		}

//		t.sendResetCommunicationMSG();
//		wait_for_reaching_Preoperational_Mode(s,t);
		t.sendEnterPreOperationalMode();
		wait_for_reaching_Preoperational_Mode(s,t);


		//std::cout << "Profile Position Mode" << std::endl;

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::PROFILE_POSITION_MODE);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Following_Error,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_maxFollowingErrorQC);
		s.sendSDO(*eds_ptr);

		if(mount_pos==MOUNT_POS_FRONT){
			std::cout<< "Min Pos Limit " << I_C::S_minPositionLimit_front << " Max " << I_C::S_maxPositionLimit_front << std::endl;

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Min_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_minPositionLimit_front);
			s.sendSDO(*eds_ptr);

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Max_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_maxPositionLimit_front);
			s.sendSDO(*eds_ptr);
		}

		if(mount_pos==MOUNT_POS_REAR){
			std::cout<< "Min Pos Limit " << I_C::S_minPositionLimit_rear << " Max " << I_C::S_maxPositionLimit_rear << std::endl;

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Min_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_minPositionLimit_rear);
			s.sendSDO(*eds_ptr);

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Max_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_maxPositionLimit_rear);
			s.sendSDO(*eds_ptr);
		}

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Acceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_acceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_deceleration);
		s.sendSDO(*eds_ptr);


		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_QuickStop_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_quickStopdeceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_maxVelocity);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_desiredVelocity_front);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Motion_ProfileType,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE, I_C::S_motionProfileType);
			s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE, (I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT) );
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				(	I_C::CONTROLWORD_ENABLE_OPERATION_BIT |	I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT |	I_C::CONTROLWORD_SWITCH_ON_BIT) );
		s.sendSDO(*eds_ptr);


		t.sendStartRemoteNodeMSG();


	}  // end setProfilePositionMode



	// **********************************************************************************

	void Steering_Configuration::currentThreshHoldPositiveSpeedHoming(CAN_SDO_Socket& s,CAN_PDO_Socket& t, robo_mount_pos_t mount_pos
			)throw(E_CAN_IO_Exception){



//		t.sendResetCommunicationMSG();
//		wait_for_reaching_Preoperational_Mode(s,t);
		t.sendEnterPreOperationalMode();
		wait_for_reaching_Preoperational_Mode(s,t);

		if(isStatusFault(s,t)){
			resetFault(s,t);
		}


		if(I_C::H_speedForSwitchSearch>I_C::S_maxVelocity){
			throw(E_CAN_IO_Exception("Speed For Switch Search is to high"));
		}
		if(I_C::H_speedForZeroSearch>I_C::S_maxVelocity){
			throw(E_CAN_IO_Exception("Speed For Zero Search is to high"));
		}
		if(I_C::H_maxProfileVelocity> I_C::S_maxVelocity){
			throw(E_CAN_IO_Exception("Max Profile Velocity Value is to high"));
		}
		if(I_C::H_homingAcceleration> I_C::S_deceleration){
			throw(E_CAN_IO_Exception("Homing acceleration value is to high"));
		}

		if(I_C::H_currentThreshHoldmilliAmp> 2000){
			throw(E_CAN_IO_Exception("Homing acceleration value is to high"));
		}

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;

//		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::PROFILE_POSITION_MODE);
//		s.sendSDO(*eds_ptr);
//
//		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Min_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,msgdata_t(-128*256*256*256l));
//		s.sendSDO(*eds_ptr);
//
//		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Max_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,msgdata_t(128*256*256*256l-1));
//		s.sendSDO(*eds_ptr);
//
//		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE, (I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT) );
//		s.sendSDO(*eds_ptr);
//
//		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//				(	I_C::CONTROLWORD_ENABLE_OPERATION_BIT |	I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT |	I_C::CONTROLWORD_SWITCH_ON_BIT) );
//		s.sendSDO(*eds_ptr);



		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::PROFILE_HOMING_MODE);
		s.sendSDO(*eds_ptr);



		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Following_Error,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_maxFollowingErrorQC);
		s.sendSDO(*eds_ptr);

		if(mount_pos==MOUNT_POS_FRONT){
			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Home_Offset,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homeOffset_front);
			s.sendSDO(*eds_ptr);
		}

		if(mount_pos==MOUNT_POS_REAR){
			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Home_Offset,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homeOffset_rear);
			s.sendSDO(*eds_ptr);
		}

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_maxProfileVelocity);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_QuickStop_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_quickStopDeceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Speeds_Speed_for_Switch_Search,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_speedForSwitchSearch);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Speeds_Speed_for_Zero_Search,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_speedForZeroSearch);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Acceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homingAcceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Current_Threshold_for_Homing_Mode,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_currentThreshHoldmilliAmp);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Home_Position,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homePosition);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Method,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::HOMING_METHOD_CURRENT_THRESHHOLD_POSITIVE_SPEED);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				I_C::CONTROLWORD_SWITCH_ON_BIT| I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_OPERATION_BIT);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				I_C::CONTROLWORD_SWITCH_ON_BIT| I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_OPERATION_BIT);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::CONTROLWORD_START_HOMING_MODE);
		s.sendSDO(*eds_ptr);


		t.sendStartRemoteNodeMSG();

	}

	// **********************************************************************************

	void Steering_Configuration::currentThreshHoldNegativeSpeedHoming(CAN_SDO_Socket& s,CAN_PDO_Socket& t, robo_mount_pos_t mount_pos
			)throw(E_CAN_IO_Exception){



//		t.sendResetCommunicationMSG();
//		wait_for_reaching_Preoperational_Mode(s,t);
		t.sendEnterPreOperationalMode();
		wait_for_reaching_Preoperational_Mode(s,t);

		if(isStatusFault(s,t)){
			resetFault(s,t);
		}


		if(I_C::H_speedForSwitchSearch>I_C::S_maxVelocity){
			throw(E_CAN_IO_Exception("Speed For Switch Search is to high"));
		}
		if(I_C::H_speedForZeroSearch>I_C::S_maxVelocity){
			throw(E_CAN_IO_Exception("Speed For Zero Search is to high"));
		}
		if(I_C::H_maxProfileVelocity> I_C::S_maxVelocity){
			throw(E_CAN_IO_Exception("Max Profile Velocity Value is to high"));
		}
		if(I_C::H_homingAcceleration> I_C::S_deceleration){
			throw(E_CAN_IO_Exception("Homing acceleration value is to high"));
		}

		if(I_C::H_currentThreshHoldmilliAmp> 2000){
			throw(E_CAN_IO_Exception("Homing acceleration value is to high"));
		}

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;



		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::PROFILE_HOMING_MODE);
		s.sendSDO(*eds_ptr);



		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Following_Error,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::S_maxFollowingErrorQC);
		s.sendSDO(*eds_ptr);

		if(mount_pos==MOUNT_POS_FRONT){
			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Home_Offset,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homeOffset_front);
			s.sendSDO(*eds_ptr);
		}

		if(mount_pos==MOUNT_POS_REAR){
			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Home_Offset,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homeOffset_rear);
			s.sendSDO(*eds_ptr);
		}

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_maxProfileVelocity);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_QuickStop_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_quickStopDeceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Speeds_Speed_for_Switch_Search,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_speedForSwitchSearch);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Speeds_Speed_for_Zero_Search,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_speedForZeroSearch);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Acceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homingAcceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Current_Threshold_for_Homing_Mode,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_currentThreshHoldmilliAmp);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Home_Position,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::H_homePosition);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Homing_Method,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::HOMING_METHOD_CURRENT_THRESHHOLD_NEGATIVE_SPEED);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				I_C::CONTROLWORD_SWITCH_ON_BIT| I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_OPERATION_BIT);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				I_C::CONTROLWORD_SWITCH_ON_BIT| I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_OPERATION_BIT);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::CONTROLWORD_START_HOMING_MODE);
		s.sendSDO(*eds_ptr);


		t.sendStartRemoteNodeMSG();

	}


	// ****************************************************************************************


	void Steering_Configuration::waitForReachingTargetPosition(CAN_SDO_Socket& s,CAN_PDO_Socket& t)throw(E_SDO_Exception){


		msgdata_t status;

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;
		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_StatusWord ,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_READ);

		s.sendSDO(*eds_ptr);

		status=eds_ptr->getReturnedData();

		std::cout << "Steering_Configuration::wait for reaching Target Position: " << std::hex << status << std::dec << std::endl;


		do{



			usleep(500000);

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_StatusWord ,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_READ);
			s.sendSDO(*eds_ptr);

			status=eds_ptr->getReturnedData();

			std::cout << "Steering_Configuration::wait for reaching Target Psoition: " << std::hex << status << std::dec << std::endl;

		}

		// solange bis eines der beiden Bits gesetzt ist
		while( !(
						( (status & I_C::STATUSWORD_POSITION_REACHED_BIT ) >0 )
					||  (  ( status & I_C::STATUSWORD_HOMING_ATTAINED_BIT ) >0 )
				)
		);

	}
	// ****************************************************************************************




	void Steering_Configuration::set_position(CAN_PDO_Socket& t,double value, robo_mount_pos_t mount_pos)throw(E_SDO_Exception){

		// Value between -1 and 1
		if(value>1)value=1;
		if(value<-1)value=-1;

		msgdata_t pos=0;
		if(mount_pos==MOUNT_POS_FRONT){
			pos=(msgdata_t)(value*0.5*(I_C::S_maxPositionLimit_front - I_C::S_minPositionLimit_front-I_C::H_SICHERHEITSABSTAND_QC) );
		}

		if(mount_pos==MOUNT_POS_REAR){
			pos=(msgdata_t)(value*0.5*(I_C::S_maxPositionLimit_rear - I_C::S_minPositionLimit_rear-I_C::H_SICHERHEITSABSTAND_QC) );
		}


		CAN_PDO pdo(t.get_node_id(),CAN_PDO::PDO_1);
		pdo.setNewData(I_C::POS_MODE_CONTROLWORD_ABSOLUTE_IMMEDEATE,2); // Controlword has two bytes
		pdo.appendMsgData(pos,4); //pos has 4 bytes

		t.sendPDO(pdo);

	}



}  // namespace idefix
