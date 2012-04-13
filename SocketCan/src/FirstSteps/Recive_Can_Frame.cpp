/*
 * Main.cpp
 *
 *  Created on: 14.03.2010
 *      Author: locher
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <iostream>

extern "C"

int main(int argc, char **argv) {

	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];

	int s=socket(PF_CAN,SOCK_RAW,CAN_RAW );
	if(s<0){
		perror("socket") ;
		std::cout << "Failed to create Socket:" <<  std::endl;
		return 1;
	}


	sockaddr_can addr;
	ifreq ifr;

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, "vcan0");
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
		perror("SIOCGIFINDEX");
	addr.can_ifindex = ifr.ifr_ifindex;

	if (!addr.can_ifindex) {
		perror("invalid bridge interface");
		return 1;
	}


	can_filter my_filter;
	my_filter.can_id=0x701;
	my_filter.can_mask=CAN_SFF_MASK;

	setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,&my_filter,sizeof(my_filter));

	if(bind(s, (sockaddr *)(&addr),sizeof(addr) ) < 0 ){
		perror("Bind");
		return 1;
	}

	iovec iov;
	can_frame frame;


	iov.iov_base = &frame;
	msghdr msg;

	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;


	can_frame f;

	int count=0;
	while(true){

		if(count++ > 15){
			my_filter.can_mask=0x7F0U;
			setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,&my_filter,sizeof(my_filter));
		}

		iov.iov_len = sizeof(frame);
		msg.msg_namelen = sizeof(addr);
		msg.msg_controllen = sizeof(ctrlmsg);
		msg.msg_flags = 0;


		ssize_t nbytes;

		nbytes= read(s,&f,30);

		timeval tv;
		// Eintreffzeit der Nachricht auslesen
		ioctl(s, SIOCGSTAMP, &tv);
		if(nbytes<0){perror("RecieveMessage");}
		std::cout<<count  <<"   Anz: " << std::dec << nbytes<< " ";


		std::cout<< std::hex <<f.can_id<< " " <<(int)f.can_dlc <<" "<< std::dec << tv.tv_sec << " "<< tv.tv_usec;


		std::cout << std::endl;
	}





}
