/*
 * start_player.cpp
 *
 *  Created on: 07.04.2010
 *      Author: locher
 */

#include <iostream>
#include <unistd.h>
#include <cstdio>

int main(int argc, char **argv) {

	pid_t pid=fork();


	if(pid==0){

		std::cout << "Starting player start Skript" << std::endl;
		execl("scripts/start_player.sh","cfg/start_player.sh",(char*)(0));
		perror("what?:");
		std::cout << "Should never be reached" << std::endl;


	}else{

		sleep(6); // should be long enough ! Means first finisch homing Mode


					std::cout << "Starting playerv " << std::endl;
					execl("/usr/local/bin/playerv","/usr/local/bin/playerv","--position2d:0","--sonar:0", (char*)(0));
					perror("what?:");
					std::cout << "Should never be reached" << std::endl;


//		pid_t pid2=fork();
//
//
//		if(pid2==0){
//
//			std::cout << "Starting playerv " << std::endl;
//			execl("/usr/local/bin/playerv","/usr/local/bin/playerv","--position2d:0","--sonar:0", (char*)(0));
//			perror("what?:");
//			std::cout << "Should never be reached" << std::endl;
//
//		}else{
//
//			std::cout << "Starting playerjoy " << std::endl;
//			execl("/usr/local/bin/playerjoy","/usr/local/bin/playerjoy","-p","-dev", "/dev/input/js0", (char*)(0));
//			perror("what?:");
//			std::cout << "Should never be reached" << std::endl;
//
//		}

	}


}
