/*
 * Configuraton.cpp
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#include "Configuration.h"


namespace idefix {





	Configuration::Configuration() {
		// TODO Auto-generated constructor stub

	}

	Configuration::~Configuration() {
		// TODO Auto-generated destructor stub
	}




	void Configuration::wait_for_reaching_Preoperational_Mode(CAN_SDO_Socket& s, CAN_PDO_Socket& t)
		throw(E_CAN_IO_Exception,E_SDO_Exception){

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;
		eds_ptr=CAN_Config_EDS::get_EDS(
				Epos_24_5_StatusWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_READ);

		msgdata_t status= (msgdata_t(0x1) << 9) ; // Bit 9 set


		boost::posix_time::ptime start=boost::posix_time::microsec_clock::universal_time();

		while( ( status & (msgdata_t(0x1)<<9 ) )  ==  (msgdata_t(0x1) <<9 ) ){
			s.sendSDO(*eds_ptr);

			status 	=eds_ptr->getReturnedData();

			//std::cout << "waiting" << std::endl;

			boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();

			if( (now-start).total_microseconds() >500000 ) {

				throw(E_CAN_IO_Exception("Timeout "));

			}

		}



	}


	// ****************************************************************************************

	bool Configuration::isStatusFault(CAN_SDO_Socket& s, CAN_PDO_Socket& t)
		throw(E_CAN_IO_Exception,E_SDO_Exception){

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;
		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_StatusWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_READ);
		s.sendSDO(*eds_ptr);



		if(  (eds_ptr->getReturnedData()& I_C::STATUSWORD_FAULT_BIT)==I_C::STATUSWORD_FAULT_BIT){
			return true;
		}else{
			return false;
		}
	}

	// ****************************************************************************************

	void Configuration::resetFault(CAN_SDO_Socket& s, CAN_PDO_Socket& t)
		throw(E_CAN_IO_Exception,E_SDO_Exception){

		std::auto_ptr<CAN_EDS_SDO> eds_ptr;
		eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,I_C::CONTROLWORD_FAULT_RESET_BIT);
		s.sendSDO(*eds_ptr);

	}

	// ****************************************************************************************


	void Configuration::enable_Device(CAN_SDO_Socket& s, CAN_PDO_Socket& t)
		throw(E_CAN_IO_Exception,E_SDO_Exception){



			if(isStatusFault(s,t)){
				resetFault(s,t);

			}

			std::auto_ptr<CAN_EDS_SDO> eds_ptr;

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,0x6);
			s.sendSDO(*eds_ptr);

			eds_ptr=CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,t.get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,0xF);
			s.sendSDO(*eds_ptr);



	}


	void Configuration::disable_Device(CAN_PDO_Socket& t)
		throw(E_CAN_IO_Exception,E_SDO_Exception){

		CAN_PDO pdo(t.get_node_id(),CAN_PDO::PDO_1);
			pdo.setNewData(0x0,2);
			pdo.appendMsgData(0,4);
			t.sendPDO(pdo);



	}





}  // namespace idefix
