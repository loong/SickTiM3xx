/*
 * EPOS_Tests_Main.cpp
 *
 *  Created on: 31.03.2010
 *      Author: locher
 */


#include "CAN_PDO_Socket.h"
#include "CAN_SDO_Socket.h"
#include "CAN_Config_EDS.h"

#include "Maxon_Mcd_Epos_60W_Profile.h"



using namespace sfzcan;


int main(int argc, char **argv) {


	std::string if_name="can0";

	CAN_SDO_Socket s;
	s.connect_socket_to_can_interface(if_name);

	CAN_PDO_Socket n(1);
	n.bind_socket_to_can_interface(if_name);


	std::auto_ptr<CAN_EDS_SDO> eds_ptr;

	eds_ptr=CAN_Config_EDS::get_EDS(Epos_60W_StatusWord,1,CAN_EDS_SDO::READ_WRITE_STATE_READ);

	std::cout << eds_ptr->get_can_frame_to_send() << std::endl;;
	s.sendSDO(*eds_ptr);
	std::cout <<std::hex << eds_ptr->getReturnedData() << std::dec << std::endl;;




	// READ Statusword 0x6041-00
	can_frame f;
	f.can_id=0x601;
	f.can_dlc=4;
	f.data[0]=0x40;
	f.data[1]=0x41;
	f.data[2]=0x60;
	f.data[3]=0;
	f.data[4]=0;
	f.data[5]=0;
	f.data[6]=0;
	f.data[7]=0;

	CAN_SDO sdo(f);

	// Select Mode of operation
	can_frame f2;
	f2.can_id=0x602;
	f2.can_dlc=5;
	f2.data[0]=0x2F;
	f2.data[1]=0x60;
	f2.data[2]=0x60;
	f2.data[3]=0;
	f2.data[4]=3; //
	f2.data[5]=0;
	f2.data[6]=0;
	f2.data[7]=0;

	CAN_SDO sdo2(f2);

	// Send Statusword
	can_frame f3;
	f3.can_id=0x601;
	f3.can_dlc=6;
	f3.data[0]=0x2B;
	f3.data[1]=0x40;
	f3.data[2]=0x60;
	f3.data[3]=0;
	f3.data[4]=0x6;
	f3.data[5]=0x0;
	f3.data[6]=0;
	f3.data[7]=0;

	CAN_SDO sdo3(f3);

	// set position data
	can_frame f4;
	f4.can_id=0x601;
	f4.can_dlc=8;
	f4.data[0]=0x23;
	f4.data[1]=0xFF;
	f4.data[2]=0x60;
	f4.data[3]=0;
	f4.data[4]=0x00;
	f4.data[5]=0x03;
	f4.data[6]=0x11;
	f4.data[7]=0x00;

	CAN_SDO sdo5(f4);


//	n.sendResetRemoteNodeMSG();
//	sleep(1);
//	n.sendStartRemoteNodeMSG();
//	sleep(1);

	try{
//		s.sendSDO(sdo);
		s.sendSDO(sdo2);
//		s.sendSDO(sdo3);
//		f3.data[4]=0x0F;
//
//		CAN_SDO sdo4(f3);
//
//		s.sendSDO(sdo4);
//		s.sendSDO(sdo5);
//
//
//		f3.data[4]=0x1F;
//		CAN_SDO sdo6(f3);
//
//		s.sendSDO(sdo6);

//		sleep(2);
//		n.sendResetRemoteNodeMSG();
	}catch (E_SDO_Exception e) {
		std::cout << e.what() << std::endl;


		std::cout <<sdo2.get_CANopen_Profile_301_Error_String() << std::endl;
	}

}
