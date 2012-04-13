/*
 * CAN_Node.cpp
 *
 *  Created on: 29.03.2010
 *      Author: locher
 */

#include "CAN_PDO_Thread.h"

namespace sfzcan {

using namespace sfzdebug;


	CAN_PDO_Thread::CAN_PDO_Thread(canid_t node_id)throw():
			CAN_PDO_Socket(node_id)
	{

		setSleepTime(1000);
		start_the_thread();


	}

	CAN_PDO_Thread::~CAN_PDO_Thread()throw()  {


		stop_the_thread();
		if( close(my_socket)==-1){
			perror("Closing Socket");
		}

		CAN_PDO_Socket::dout <<sfzdebug::linestart<< "CAN_Node: Destruktion";
		CAN_PDO_Socket::dout << endl;
	}



	void CAN_PDO_Thread::process(){

		if(socket_is_bound){

			static int count=0;
			++count;

			can_frame f;
			ssize_t nbytes=0;

			nbytes= read(my_socket,&f,sizeof(f));


			if(nbytes==-1){ // Read fehlgeschlagen

				// Timeout bei errno==EAGAIN,
				// Timeout ignorieren sonst fheler ausgeben
				if(errno!=EAGAIN){

					perror("Socket Read: ");

				}

			}else{ //read war erfolgreich

				CAN_PDO_Socket::dout << "Read can_frame " << f << sfzdebug::endl;

				// receive time ist über ioctrl erhältlich mit timeval in mus
				timeval tv;
				ioctl(my_socket,SIOCGSTAMP, &tv);

				// alternativ mit timespec in nanos
	//			timespec ns;
	//			ioctl(my_socket,SIOCGSTAMPNS, &ns);


				//jetzt das can_frame an Kind klassen weiter reichen
				// dienext_can_frame_to_process überschreiben


				next_can_frame_to_process(f,tv);



				if(debug_message){
					std::cout<< std::dec << count  <<"   Anz: " << nbytes<< " ";
					std::cout<< f;
					std::cout << tv.tv_sec<<"s "<< tv.tv_usec << "mus" << std::endl;
				}

			} //end erfolgreiches read

		} // end if socket_bound

	}

	void CAN_PDO_Thread::next_can_frame_to_process(const can_frame& f,timeval& tv){


	}



}  // namespace sfzcan
