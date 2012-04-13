/*
 * Drive_Configuration.cpp
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */



#include "Drive_Configuration.h"


namespace idefix {



	Drive_Configuration::Drive_Configuration() {
		// TODO Auto-generated constructor stub

	}

	Drive_Configuration::~Drive_Configuration() {
		// TODO Auto-generated destructor stub
	}




	void Drive_Configuration::configure_Drive(CAN_SDO_Socket& s, CAN_PDO_Socket& t)throw(E_SDO_Exception){


		/*
		 * PDO Mapping Konfiguration so, dass
		 * RxPDO1 die Motorgeschwindigkeit festlegt und
		 * RxPDO2 die Neue Position festlegt
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

		//std::cout << "Debug Test" << std::endl;
		if(isStatusFault(s,t)){
			resetFault(s,t);
		}
		//std::cout << "Debug Test 1" << std::endl;

//		n.sendResetCommunicationMSG();
//		wait_for_reaching_Preoperational_Mode(n);

		t.sendEnterPreOperationalMode();

		wait_for_reaching_Preoperational_Mode(s,t);




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


		// Setting CAN_Object Id and Valid RxPDO2
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_2_Parameter_COB_ID_used_by_RxPDO_2,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x00000300 +t.get_node_id()
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
				0xFF
				);
		s.sendSDO(*eds_ptr);


		// Setting Trasmission Type asynchron (0xFF)
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_2_Parameter_Transmission_Type,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x1
				);
		s.sendSDO(*eds_ptr);

		// Mapping the RxPDO1


			//	  One has to disable the PDO before mapping the Objects
			//	  by setting the Number of Object to 0


	// Mapping the RxPDO1


		//	  One has to disable the PDO before mapping the Objects
		//	  by setting the Number of Object to 0
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0
				);
		s.sendSDO(*eds_ptr);


	// Mapping Controlword

		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_1st_Mapped_Object_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x60400010  //Controlword
				);
		s.sendSDO(*eds_ptr);


	// Mapping Target Velocity

		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_Receive_PDO_1_Mapping_2nd_Mapped_Object_in_RxPDO_1,
				t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				0x60FF0020 // TargetVelocity
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
		// Controlword

			//	  One has to disable the PDO before mapping the Objects
			//	  by setting the Number of Object to 0
			eds_ptr=CAN_Config_EDS::get_EDS(
					Epos_24_5_Receive_PDO_2_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_2,
					t.get_node_id(),
					CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
					0
					);
			s.sendSDO(*eds_ptr);


		// Mapping Controlword

			eds_ptr=CAN_Config_EDS::get_EDS(
					Epos_24_5_Receive_PDO_2_Mapping_1st_Mapped_Object_in_RxPDO_2,
					t.get_node_id(),
					CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
					0x60400010  //Controlword
					);
			s.sendSDO(*eds_ptr);


			eds_ptr=CAN_Config_EDS::get_EDS(
					Epos_24_5_Receive_PDO_2_Mapping_2nd_Mapped_Object_in_RxPDO_2,
					t.get_node_id(),
					CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
					0x607A0020  //Target Position
					);
			s.sendSDO(*eds_ptr);



			// Number of Mapped Objects in RxPDO2
			eds_ptr=CAN_Config_EDS::get_EDS(
					Epos_24_5_Receive_PDO_2_Mapping_Number_of_Mapped_Application_Objects_in_RxPDO_2,
					t.get_node_id(),
					CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
					2
					);
			s.sendSDO(*eds_ptr);




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



//			NMT::sendStartRemoteNodeMSG(nodeId);





	}




	void Drive_Configuration::setProfileVelocityMode(CAN_SDO_Socket& s, CAN_PDO_Socket& t)throw(E_CAN_IO_Exception)
	{

		std::cout << "Profile Velocity Mode" << std::endl;

//		disable_Device(t);


		std::auto_ptr<CAN_EDS_SDO> eds_ptr;

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::PROFILE_VELOCITY_MODE);
		s.sendSDO(*eds_ptr);


		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Acceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_acceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_deceleration);
		s.sendSDO(*eds_ptr);


		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_QuickStop_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_quickStopdeceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_maxVelocity);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Motion_ProfileType,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE, I_C::D_motionProfileType);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),
				CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				(I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT) );
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				(	I_C::CONTROLWORD_ENABLE_OPERATION_BIT |
					I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT |
					I_C::CONTROLWORD_QUICK_STOP_BIT |
					I_C::CONTROLWORD_SWITCH_ON_BIT) );
		s.sendSDO(*eds_ptr);


		//setVelocity(t,0);

//		enable_Device(s,t);


	}  // end setProfileVelocityMode


	void Drive_Configuration::setVelocity_rpm( CAN_PDO_Socket& t,msgdata_t value_rpm)throw(E_CAN_IO_Exception){

		CAN_PDO pdo(t.get_node_id(),CAN_PDO::PDO_1);
		pdo.setNewData(I_C::CONTROLWORD_START_MOVE,2);
		pdo.appendMsgData(-value_rpm,4);

		t.sendPDO(pdo);
	}



	void Drive_Configuration::setProfilePositionMode(CAN_SDO_Socket& s, CAN_PDO_Socket& t)throw(E_CAN_IO_Exception)
	{

		std::cout << "Drive Configuration: Set  Profile Position Mode" << std::endl;


//		disable_Device(t);

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::PROFILE_POSITION_MODE);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Modes_of_Operation_Display,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_READ);
				s.sendSDO(*eds_ptr);
		std::cout << "Mode of Operation: " << eds_ptr->getReturnedData()<< std::endl;


		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Following_Error,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_maxFollowingErrorQC);
		s.sendSDO(*eds_ptr);

		std::cout<< "Min Pos Limit " << I_C::D_minPositionLimit << " Max " << I_C::D_maxPositionLimit << std::endl;

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Min_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_minPositionLimit);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Software_Position_Limit_Max_Position_Limit,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_maxPositionLimit);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Acceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_acceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_deceleration);
		s.sendSDO(*eds_ptr);


		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_QuickStop_Deceleration,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_quickStopdeceleration);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Max_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_maxVelocity);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Profile_Velocity,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::D_desiredVelocity);
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_Motion_ProfileType,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE, I_C::S_motionProfileType);
			s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE, (I_C::CONTROLWORD_QUICK_STOP_BIT | I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT) );
		s.sendSDO(*eds_ptr);

		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
				(	I_C::CONTROLWORD_ENABLE_OPERATION_BIT |	I_C::CONTROLWORD_ENABLE_VOLTAGE_BIT | I_C::CONTROLWORD_QUICK_STOP_BIT |	I_C::CONTROLWORD_SWITCH_ON_BIT) );
		s.sendSDO(*eds_ptr);

		//moveDistance(t,0);

//		enable_Device(s,t);

	}  // end setProfilePositionMode



	void Drive_Configuration::moveDistance(CAN_PDO_Socket& t,msgdata_t quadcounts)throw(E_CAN_IO_Exception){

		//		msgdata_t pos=(msgdata_t)(value_m/I_C::QC_TO_METER_FACTOR_DRIVES);
		std::cout << "Moving Distance" << std::endl;
		//msgdata_t pos=(msgdata_t)(100000);

		CAN_PDO pdo(t.get_node_id(),CAN_PDO::PDO_2);
		pdo.setNewData(I_C::POS_MODE_CONTROLWORD_RELATIVE_IMMEDEATE,2); // Controlword has two bytes
		pdo.appendMsgData(-quadcounts,4); //pos has 4 bytes

		t.sendPDO(pdo);



	}




}  // namespace idefix
