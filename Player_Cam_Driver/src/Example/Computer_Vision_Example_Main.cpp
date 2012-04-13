/*
 * Computer_Vision_Main.cpp
 *
 *  Created on: 29.05.2011
 *      Author: locher
 */


#include <gtkmm-2.4/gtkmm/main.h>
#include <gtkmm-2.4/gtkmm/window.h>

#include "ComputerVisionExample.h"


int main(int argc, char **argv) {


		Gtk::Main m(argc,argv);

		ComputerVisionExample cvc;
		Gtk::Window w;
		w.add(cvc);
		cvc.setSleepTime(30000);
		cvc.start_the_thread();
		w.show_all_children();
		m.run(w);


}
