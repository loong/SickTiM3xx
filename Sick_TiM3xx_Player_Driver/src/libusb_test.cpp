/*
 * libusb_test.cpp
 *
 *  Created on: 21.04.2012
 *      Author: locher
 */



#include <libusb-1.0/libusb.h>

// I try to use asyncronous device I/O
// http://libusb.sourceforge.net/api-1.0/group__asyncio.html


// Udev Rule:
// SUBSYSTEM=="usb", ATTR{idVendor}=="19a2", ATTR{idProduct}=="5001", MODE="0666", GROUP="video"

#include <iostream>
#include <cstdlib>


#include <MeasurementDatasetParser.h>


struct dataset{

	char kommandoart[3];
	char kommando[11];
	uint16_t verionsnummer;
	uint16_t geraetenummer;





};


void printdev(libusb_device *dev) {

	libusb_device_descriptor desc;

	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		std::cout<<"failed to get device descriptor"<<std::endl;
		return;
	}
	std::cout<<"Number of possible configurations: "<<(int)desc.bNumConfigurations<<"  ";
	std::cout<<"Device Class: "<<(int)desc.bDeviceClass<<"  ";
	std::cout<<"VendorID: "<<desc.idVendor<<"  ";
	std::cout<<"ProductID: "<<desc.idProduct<<std::endl;

	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);

	std::cout<<"Interfaces: "<<(int)config->bNumInterfaces<<" ||| ";

	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;

	for(int i=0; i<(int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		std::cout<<"Number of alternate settings: "<<inter->num_altsetting<<" | ";
		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			std::cout<<"Interface Number: "<<(int)interdesc->bInterfaceNumber<<" | ";
			std::cout<<"Number of endpoints: "<<(int)interdesc->bNumEndpoints<<" | ";

			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				std::cout<<"Descriptor Type: "<<(int)epdesc->bDescriptorType<<" | ";
				std::cout<<"EP Address: "<<(int)epdesc->bEndpointAddress<<" | ";
			}
		}
	}
	std::cout<<std::endl<<std::endl<<std::endl;
	libusb_free_config_descriptor(config);
}



int main(int argc, char **argv) {


	libusb_context* m_context;

	int init_result= libusb_init(&m_context);
	if(init_result){
		std::cerr << "USB init failed: "<< libusb_error_name(init_result) << std::endl;

		exit(EXIT_FAILURE);
	}


	libusb_set_debug(m_context,3);


//	libusb_device** m_device_list;
//	libusb_device** actual_device_ptr;
//
//	ssize_t num_devices=libusb_get_device_list(m_context, &m_device_list);
//
//	actual_device_ptr=m_device_list;
//
//	// iterate device list
//	while(*actual_device_ptr!=0){
//		libusb_device_descriptor desc;
//
//		if(int error=libusb_get_device_descriptor(*m_device_list,&desc)){
//
//			std::cerr << "Get device error: " << libusb_error_name(error) <<std::endl;
//		}
//
//		std::cout << std::hex
//				<< "VendorID: " <<  desc.idVendor << std::endl
//				<< "ProductID: " << desc.idProduct << std::endl
//				<< "USB Release: "<< desc.bcdUSB << std::endl
//				<< "Serial number: " <<(int) desc.iSerialNumber << std::endl
//
//				<< std::dec;
//
//
//
//	   std::cout << "******************************************"<< std::endl;
//
//		actual_device_ptr++;
//	}
//
//	std::cout << "Devices found: "<< num_devices << std::endl;
//
//	libusb_free_device_list(m_device_list,0);
//



	#define VENDORID 0x19a2
	#define PRODUCTID 0x5001


	libusb_device_handle* m_device_handle = libusb_open_device_with_vid_pid(m_context, VENDORID, PRODUCTID);
//	libusb_device_handle* m_device_handle = libusb_open_device_with_vid_pid(m_context, 0x05e3,0x1205);

	if( !m_device_handle){

			std::cerr << "Error open device:" <<  std::endl;
	}else{



		libusb_device* m_device =libusb_get_device(m_device_handle);

		printdev(m_device);

		std::cout << " Device Class 0: All Interfaces operate independently" << std::endl;
		std::cout << " Interface Number: 0" << std::endl;
		std::cout << " Two descriptors of type Endpoint" << std::endl;
		std::cout << " Endpoint Adresses 129 and 2, does thes mean we have Adress 2 with different directions? " << std::endl;

		// Our only available interface number
		int interface_number=0;

		if(libusb_kernel_driver_active(m_device_handle,interface_number)){

			std::cout << "Error: Kernel driver found" << std::endl;

			if (int error=libusb_detach_kernel_driver(m_device_handle,interface_number)){
				std::cout << "Detaching Kernel Driver failed " << libusb_error_name(error) << std::endl;
				exit(EXIT_FAILURE);
			}

		}else{
			std::cout << " No Kernel driver for attached Device found, "
					"that's good, libusb handels the device" << std::endl;
		}


		// Now we claim the device
		int error = libusb_claim_interface(m_device_handle,interface_number);
		if(error){

			std::cout << "Claiming interface failed " << libusb_error_name(error) << std::endl;
			exit(EXIT_FAILURE);

		}


		static const uint8_t	STX=2; //Start of text
		static const uint8_t	ETX=3; // End of Text

		uint8_t start_one_scan[]={STX,'s','R','N',' ','L','M','D','s','c','a','n','d','a','t','a',ETX};

		uint8_t start_continuous_scan[]={STX,'s','E','N',' ','L','M','D','s','c','a','n','d','a','t','a',' ','1',ETX};
		uint8_t stop_continuous_scan[]={STX,'s','E','N',' ','L','M','D','s','c','a','n','d','a','t','a',' ','0',ETX};



		uint8_t	read_endpoint=129;
		uint8_t	write_endpoint=2;
		unsigned int timeout_millis=10000;

//		std::cout << sizeof(start_one_scan) << std::endl;


		int transferred_data_size=0;

		uint8_t receive_buf[2049];


			error =libusb_bulk_transfer(
					m_device_handle,
					write_endpoint,
//					start_one_scan,
//					sizeof(start_one_scan),
					start_continuous_scan,
					sizeof(start_continuous_scan),
					&transferred_data_size,
					timeout_millis);

			if(error){

				std::cout << "Write Bulk Transfer failed " << libusb_error_name(error) << std::endl;
				exit(EXIT_FAILURE);

			}

			std::cout<< "Transfered Data written: " << transferred_data_size<< std::endl;

			// Receive acknoledge

			error =libusb_bulk_transfer(
					m_device_handle,
					read_endpoint,
					receive_buf ,
					sizeof(receive_buf)-1, // leave one for termination
					&transferred_data_size,
					timeout_millis);

			if(error){

				std::cout << "Read Bulk Transfer failed " << libusb_error_name(error) << std::endl;
				exit(EXIT_FAILURE);

			}


			// String terminierung
			receive_buf[transferred_data_size]=0;
			std::cout<< "Acknoledge: " << receive_buf<< std::endl;



		Measurement_Dataset_Parser mdp;


		for (int i = 0; i < 10; ++i) {


			error =libusb_bulk_transfer(
					m_device_handle,
					read_endpoint,
					receive_buf ,
					sizeof(receive_buf)-1, // leave one for termination
					&transferred_data_size,
					timeout_millis);

			if(error){

				std::cout << "Read Bulk Transfer failed " << libusb_error_name(error) << std::endl;
				exit(EXIT_FAILURE);

			}

			//std::cout<< "Transfered Data read: " << transferred_data_size<< std::endl;

			// String terminierung
			receive_buf[transferred_data_size]=0;

			if(receive_buf[0]!=STX){ // read from device buff until it is empty, and next data set starts with STX-Code
				std::cout << ".";
			}else{
//				std::cout << receive_buf << std::endl;
				mdp.set_pointer_to_data_buf(receive_buf,transferred_data_size);
				mdp.parse_data();
				mdp.print_data();

			}

		} // end of for loop


		// Stop Measurment

		error =libusb_bulk_transfer(
				m_device_handle,
				write_endpoint,
				stop_continuous_scan,
				sizeof(stop_continuous_scan),
				&transferred_data_size,
				timeout_millis);

		if(error){

			std::cout << "Write Bulk Transfer failed " << libusb_error_name(error) << std::endl;
			exit(EXIT_FAILURE);

		}

		std::cout<< "Transfered Data written: " << transferred_data_size<< std::endl;



		error =libusb_bulk_transfer(
				m_device_handle,
				read_endpoint,
				receive_buf ,
				sizeof(receive_buf)-1, // leave one for termination
				&transferred_data_size,
				timeout_millis);

		if(error){

			std::cout << "Read Bulk Transfer failed " << libusb_error_name(error) << std::endl;
			exit(EXIT_FAILURE);

		}



		std::cout<< "Transfered Data read: " << transferred_data_size<< std::endl;
		// String terminierung
		receive_buf[transferred_data_size]=0;
		std::cout << receive_buf << std::endl;



		error= libusb_release_interface(m_device_handle,interface_number);

		if(error){

			std::cout << "Releasing interface failed " << libusb_error_name(error) << std::endl;
			exit(EXIT_FAILURE);

		}





//		libusb_device_descriptor desc;
//
//		libusb_get_device_descriptor(m_device,&desc);

//		uint8_t buff[2048];
//
//			for(int desc_index=1; desc_index<256;desc_index++){
//
//				int num_bytes_read=libusb_get_string_descriptor_ascii(m_device_handle,desc_index,buff,2048);
//
//
//				if(num_bytes_read>0){
//					std::cout<< desc_index << ": " << num_bytes_read << ": " << buff << std::endl;
//
//					if(libusb_kernel_driver_active(m_device_handle,0)){
//
//						std::cout << "Kernel driver found" << std::endl;
//					}
//
//
//				}else{
//					std::cerr<< desc_index << ": " << libusb_error_name(num_bytes_read) << std::endl;
//				}
//			}
//			std::cout  << "**********************************************" << std::endl;






	}





	if( m_device_handle){

		libusb_close(m_device_handle);

	}


	libusb_exit(m_context);

}

