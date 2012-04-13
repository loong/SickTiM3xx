/*
 *  $Id: SendCanFrame.cpp,v 1.4 2010/04/04 15:11:56 sfz-locher Exp $
 */

/*
 * cansend.c - simple command line tool to send CAN-frames via CAN_RAW sockets
 *
 * Copyright (c) 2002-2007 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Volkswagen nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * Alternatively, provided that this notice is retained in full, this
 * software may be distributed under the terms of the GNU General
 * Public License ("GPL") version 2, in which case the provisions of the
 * GPL apply INSTEAD OF those given above.
 *
 * The provided data structures and external interfaces from this code
 * are not restricted to be used by modules with a GPL compatible license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * Send feedback to <socketcan-users@lists.berlios.de>
 *
 */
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

extern "C"

int main(int argc, char **argv)
{
	int s; /* can raw socket */ 
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame1;
	struct can_frame frame2;
	struct ifreq ifr;


	memset(&frame1, 0, sizeof(frame1)); /* init CAN frame, e.g. DLC = 0 */

	frame1.can_id=0x701;
	frame1.can_dlc=2;
	frame1.data[0]=0;
	frame1.data[1]=1;

	memset(&frame2, 0, sizeof(frame2)); /* init CAN frame, e.g. DLC = 0 */

	frame2.can_id=0x601;
	frame2.can_dlc=4;
	frame2.data[0]=0x2B;
	frame2.data[1]=0x40;
	frame2.data[2]=0x60;
	frame2.data[3]=0;



	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		return 1;
	}

	addr.can_family = AF_CAN;

	strcpy(ifr.ifr_name, "can0");
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		return 1;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	/* send frame */

	for(int i=0;  i< 5 ; i++ ){

//		if ((nbytes = write(s, &frame1, sizeof(frame1))) != sizeof(frame1)) {
//			perror("write frame 1");
//			return 1;
//		}


		if ((nbytes = write(s, &frame2, sizeof(frame2))) != sizeof(frame2)) {
			perror("write frame 2");
			return 1;
		}
		std::cout << "sent two frames" << std::endl;
		usleep(100000);
	}
	//fprint_long_canframe(stdout, &frame, "\n", 0);

	sleep(10);

	close(s);

	return 0;
}
