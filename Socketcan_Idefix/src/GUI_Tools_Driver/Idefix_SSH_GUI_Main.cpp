/*
 * Idefix_SSH_GUI_Main.cpp
 *
 *  Created on: 01.05.2010
 *      Author: locher
 */

#include <gtkmm-2.4/gtkmm/main.h>

#include <iostream>
#include <string>
#include <unistd.h>

#include "Idefix_SSH_GUI.h"

int main(int argc, char **argv) {


	// changing working directory to the workspace dir directory

	std::string path(argv[0]);
	size_t last_delim_pos=path.find_last_of("/");

	std::string filename=path.substr(last_delim_pos+1);

	size_t delim_before_bin =path.find_last_of("/",last_delim_pos-1);

	std::string working_dir=path.substr(0,delim_before_bin);

	chdir(working_dir.c_str());


//	std::cout <<path<< std::endl;
//	std::cout<< last_delim_pos<< " " << path.length() << " "  << filename << std::endl;
//	std::cout << working_dir << std::endl;



	Gtk::Main m(argc,argv );



	Idefix_SSH_GUI w;

	m.run(w);

}
