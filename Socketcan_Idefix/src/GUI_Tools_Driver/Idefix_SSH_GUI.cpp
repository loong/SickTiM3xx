/*
 * Idefix_SSH_GUI.cpp
 *
 *  Created on: 01.05.2010
 *      Author: locher
 */

#include "Idefix_SSH_GUI.h"

Idefix_SSH_GUI::Idefix_SSH_GUI() :
	upload_button("Upload \nbin + cfg + libs + scripts  "),
	start_can_button("Start CAN Interfaces"),
	start_player_button("Start Player"),
	stop_player_button("Stop Player"),
	shutdown_idefix_button("Poweroff idefix"),
	reboot_idefix_button("Reboot Idefix"),
	start_playerv_button("Start Player View"),
	start_moves_test_button("Start_move_test_Client"),

	player_ssh_pid(0)


{
	set_title("Idefix Player Remote Control");

	set_default_size(300,200);

	add(myHbox);
	myHbox.add(myLeftVbox);

	myLeftVbox.add(upload_button);
	myLeftVbox.add(start_can_button);
	myLeftVbox.add(player_hbox);
	player_hbox.add(start_player_button);
	player_hbox.add(stop_player_button);
	myLeftVbox.add(start_playerv_button);
	myLeftVbox.add(idefix_hbox);
		idefix_hbox.add(shutdown_idefix_button);
		idefix_hbox.add(reboot_idefix_button);
	myLeftVbox.add(start_moves_test_button);

	upload_button.signal_clicked().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_upload_button));

	start_can_button.signal_toggled().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_start_can_button));

	start_player_button.signal_clicked().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_start_player_button));
	stop_player_button.signal_clicked().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_stop_player_button));

	shutdown_idefix_button.signal_clicked().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_shutdown_idefix_button));

	reboot_idefix_button.signal_clicked().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_reboot_idefix_button));

	start_playerv_button.signal_clicked().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_start_playerv_button));

	start_moves_test_button.signal_toggled().connect(sigc::mem_fun(this,
			&Idefix_SSH_GUI::on_start_moves_test_button));




	show_all_children();

}

Idefix_SSH_GUI::~Idefix_SSH_GUI() {

}

void Idefix_SSH_GUI::on_upload_button() {

	pid_t pid = fork();

	if (pid == 0) {

		std::cout << "Uploading " << std::endl;
		execl("scripts/rsync_upload.sh", "rsync_upload.sh", (char*) (0));
		perror("what?:");
		std::cout << "Should never be reached" << std::endl;

	}

}

void Idefix_SSH_GUI::on_start_can_button() {

	if (start_can_button.get_active()) {
		std::cout << "Starting up CAN Interfaces" << std::endl;

		pid_t pid = fork();

		if (pid == 0) {

			execl("/usr/bin/ssh", "ssh", "idefix@idefix",
					"/usr/local/bin/start_can_if start", (char*) (0));
			perror("what?:");
			std::cout << "Should never be reached" << std::endl;

		}

	}

	if (!start_can_button.get_active()) {
		std::cout << "Stopping  CAN Interfaces" << std::endl;
		pid_t pid = fork();

		if (pid == 0) {

			execl("/usr/bin/ssh", "ssh", "idefix@idefix",
					"/usr/local/bin/start_can_if stop", (char*) (0));
			perror("what?:");
			std::cout << "Should never be reached" << std::endl;

		}

	}

}

void Idefix_SSH_GUI::on_start_player_button() {

	if (player_ssh_pid == 0) {

		player_ssh_pid = fork();

		if (player_ssh_pid == 0) {

			std::cout << "Starting Player on Idefix" << std::endl;
			execl("/usr/bin/ssh", "ssh", "idefix@idefix",
					"/home/idefix/idefix_bin/scripts/start_player.sh",
					(char*) (0));
			perror("what?:");
			std::cout << "Should never be reached" << std::endl;

		}
	}

}
void Idefix_SSH_GUI::on_stop_player_button() {

	// ssh process killen
	if (player_ssh_pid != 0) {

		std::stringstream ss;

		ss << "kill " << player_ssh_pid;

		system(ss.str().c_str());

		//zur sicherheit alle idefix player Instanzen killen


		pid_t pid = fork();
		if (pid == 0) {
			std::cout << "killing player on Idefix" << std::endl;
			execl("/usr/bin/ssh", "ssh", "idefix@idefix",
					"/usr/bin/killall player", (char*) (0));
			perror("what?:");
			std::cout << "Should never be reached" << std::endl;
		}

		player_ssh_pid = 0;

	}

}

void Idefix_SSH_GUI::on_shutdown_idefix_button() {

	pid_t pid = fork();
	if (pid == 0) {
		std::cout << "Shutting down Idefix" << std::endl;
		execl("/usr/bin/ssh", "ssh", "root@idefix", "/sbin/poweroff",
				(char*) (0));
		perror("what?:");
		std::cout << "Should never be reached" << std::endl;
	}

}

void Idefix_SSH_GUI::on_reboot_idefix_button() {

	pid_t pid = fork();
	if (pid == 0) {
		std::cout << "Shuting down Idefix" << std::endl;
		execl("/usr/bin/ssh", "ssh", "root@idefix", "/sbin/reboot", (char*) (0));
		perror("what?:");
		std::cout << "Should never be reached" << std::endl;
	}

}




void Idefix_SSH_GUI::on_start_playerv_button() {

	pid_t pid = fork();
	if (pid == 0) {
		std::cout << "Starting Player View" << std::endl;
		execl("/usr/local/bin/playerv","/usr/local/bin/playerv","-h","idefix","-p","6665","--position2d:0","--sonar:0", (char*)(0));
		perror("what?:");
		std::cout << "Should never be reached" << std::endl;
	}

}




void Idefix_SSH_GUI::on_start_moves_test_button(){


	if(start_moves_test_button.get_active()){


		move_test_client_pid= fork();
		if (move_test_client_pid== 0) {
			std::cout << "Starting  Moves Test Client" << std::endl;
			execl("/usr/bin/ssh", "ssh", "idefix@idefix",
					"/home/idefix/idefix_bin/bin/Moves_Test_Client", (char*) (0));
			perror("what?:");
			std::cout << "Should never be reached" << std::endl;
		}
	}else{

		std::stringstream ss;

		ss << "kill " << move_test_client_pid;

		system(ss.str().c_str());

		//zur sicherheit die idefix client Instanz killen


		pid_t pid = fork();
		if (pid == 0) {
			std::cout << "killing Moves Test CLient on Idefix" << std::endl;
			execl("/usr/bin/ssh", "ssh", "idefix@idefix",
					"/usr/bin/killall Moves_Test_Client", (char*) (0));
			perror("what?:");
			std::cout << "Should never be reached" << std::endl;
		}
		move_test_client_pid=0;

	}


}
