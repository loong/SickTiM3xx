/*
 * Configuraton.cpp
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#include "AIM04_Data_Thread.h"


namespace idefix {

	AIM04_Data_Thread::AIM04_Data_Thread()throw():
			CAN_PDO_Socket(I_C::NODE_AIM04),
			Thread("AIM04_Data_Thread"),
			aim04_is_configured(false),
			can_interface_name(CAN_INTERFACE_NAME)
	{
		setSleepTime(200);
		start_the_thread();
	}

	AIM04_Data_Thread::~AIM04_Data_Thread()throw() {

		stop_the_thread();
	}



	void AIM04_Data_Thread::config(uint_t analog_delta_interrupt_time ){

		//  configuration should run only once
		if(!aim04_is_configured){

			sdo_socket.connect_socket_to_can_interface(can_interface_name);


			bind_socket_to_can_interface(can_interface_name);
			try{
				sendStartRemoteNodeMSG();

			}catch(std::exception e){
				std::cerr<< "AIM04_Data_Thread::config: "<< e.what()<< std::endl;
			}

			std::auto_ptr<CAN_EDS_SDO> eds_ptr;
//			eds_ptr=CAN_Config_EDS::get_EDS(
//					AIM04_analogue_input_interrupt_delta__1_analogue_input_interrupt_delta ,
//					I_C::NODE_AIM04,
//					CAN_EDS_SDO::READ_WRITE_STATE_WRITE,
//					analog_delta_interrupt_time
//					);
//			sdo_socket.sendSDO(*eds_ptr);

			aim04_is_configured=true;
		}
	}


	void AIM04_Data_Thread::process(){

		struct can_frame frame;

		if(read(my_socket,&frame, sizeof(frame))!=-1){
//		    std::cout<<std::hex<<"id: "<<frame.can_id<<" dlc: "<<(int)frame.can_dlc<<" data: ";
//		    for(int i=0; i<(int)frame.can_dlc; i++){
//		 	   std::cout<<(int) frame.data[i]<<" ";
//		    }
//		    std::cout << std::endl;

			{
				// Enter new scope which is mutex locked
				boost::mutex::scoped_lock sl(my_mutex);

				ranges[1]=tometers((unsigned int)(frame.data[0] | frame.data[1] <<8));
				ranges[2]=tometers((unsigned int)(frame.data[2] | frame.data[3] <<8));
				ranges[0]=tometers((unsigned int)(frame.data[4] | frame.data[5] <<8));
				ranges[3]=tometers((unsigned int)(frame.data[6] | frame.data[7] <<8));

				// leave scoped lock
			}
//	 		if(true){
//	 		std::cout<<"sensorwerte: ";
//	 		for(int i=0; i<4; i++){
//	 			std::cout<<std::dec<<datas[i]<<" ";
//	 		}
//	 		std::cout<<std::endl;
//	 		}
		}else if(errno==EAGAIN){
			//read timeout
		}else{
			perror("AIM04_Data_Thread: Error: ");
		}


	}


	//Werteumrechnungsfaktor
	float AIM04_Data_Thread::tometers(unsigned int val){
		float result= (val+3746)/36970.0;
		if(val <3000){
			result=1.0f;
		}
		return result;
	}

	/**
	 * \brief Übergabe eines Zeigers auf ein Driver float array.
	 * Über den Zeiger werden die Ultraschalldaten aktualisiert werden
	 *
	 */
	void AIM04_Data_Thread::get_ranges(float* my_ranges){

		boost::mutex::scoped_lock sl(my_mutex);

		for (int i=0; i<4; i++){
			my_ranges[i]=ranges[i];
		}
	}

	void AIM04_Data_Thread::setCanInterfaceName(std::string _can_interface_name){
		can_interface_name = _can_interface_name;
	}

//	//2te get methode
//	ultrasonic_data_t AIM04_Data_Thread::get_data(){
//		data.vl=datas[0];
//		data.vr=datas[1];
//		data.hl=datas[2];
//		data.hr=datas[3];
//		return data;
//	}

}  // namespace idefix
