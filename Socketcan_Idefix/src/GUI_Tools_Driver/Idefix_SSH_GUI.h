/*
 * Idefix_SSH_GUI.h
 *
 *  Created on: 01.05.2010
 *      Author: locher
 */

#ifndef IDEFIX_SSH_GUI_H_
#define IDEFIX_SSH_GUI_H_

#include <gtkmm-2.4/gtkmm/window.h>
#include <gtkmm-2.4/gtkmm/box.h>
#include <gtkmm-2.4/gtkmm/togglebutton.h>
#include <gtkmm-2.4/gtkmm/button.h>

#include <iostream>
#include <sstream>


/**
 * \brief If you have ssh access to Idefix public-key authorization
 * this Class Provides a GUI a lot of usefull things
 *
 * You can
 * <ul>
 * <li>
 * upload new binaries and config files to idefix
 * <li>Start und Stop Player and Payerv
 * <li>Start and Stop Clients
 * <li>Start und Stop the Socket-CAN-Interfaces
 * <li>Reboot und Poweroff (you need public-key ssh-access to the root account
 * </ul>
 */
class Idefix_SSH_GUI : public Gtk::Window{

private:


	Gtk::Button upload_button;
	Gtk::ToggleButton start_can_button;
	Gtk::Button start_player_button;
	Gtk::Button stop_player_button;
	Gtk::Button shutdown_idefix_button;
	Gtk::Button reboot_idefix_button;
	Gtk::Button start_playerv_button;
	Gtk::ToggleButton start_moves_test_button;

	Gtk::HBox myHbox;
	Gtk::VBox myLeftVbox;

	Gtk::HBox player_hbox;
	Gtk::HBox idefix_hbox;


	void on_upload_button();

	void on_start_can_button();
	void on_start_player_button();
	void on_stop_player_button();
	void on_shutdown_idefix_button();
	void on_reboot_idefix_button();
	void on_start_playerv_button();
	void on_start_moves_test_button();


	pid_t player_ssh_pid;
	pid_t move_test_client_pid;

public:
	Idefix_SSH_GUI();
	virtual ~Idefix_SSH_GUI();
};

#endif /* IDEFIX_SSH_GUI_H_ */
