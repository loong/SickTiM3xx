/*
 * MeasurementDatasetParser.cpp
 *
 *  Created on: 22.04.2012
 *      Author: locher
 */

#include "TiM3xxDataParser.h"

TiM3xx_Data_Parser::TiM3xx_Data_Parser() {
	// TODO Auto-generated constructor stub

}

TiM3xx_Data_Parser::~TiM3xx_Data_Parser() {
	// TODO Auto-generated destructor stub
}

void TiM3xx_Data_Parser::set_pointer_to_data_buf(uint8_t* data_buf_ptr, int the_data_size){

	actual_data_ptr=data_buf_ptr;
	data_size=the_data_size;


}


void TiM3xx_Data_Parser::parse_data(){

	// check termination of Buf
	if(actual_data_ptr[data_size]!=0){
		std::cerr << "data_buf has no cstring termination" << std::endl;
	}else{

		// look for STX (Start of Text byte: STX =2)
		if(actual_data_ptr[0]!=2){

			std::cerr << "Pars error" << std::endl;

		}else{

			union caster_t{
				uint32_t ui32;
				int32_t i32;
				float fl;
			} m_caster;
			;
			actual_data_ptr++;
			std::stringstream ss((const char*)actual_data_ptr);

			ss >> std::hex>> kommandoart;
			ss >> kommando;
			ss >> versionsnummer;
			ss >> geraetenummer;
			ss >> seriennummer;
			ss >> geraetestatus_high;
			ss >> geraetestatus_low;
			ss >> telegrammzaehler;
			ss >> scanzaehler;
			ss >> einschaltdauer_mus;
			ss >> uebertragungsdauer_mus;
			ss >> eingangsstatus_high;
			ss >> eingangsstatus_low;
			ss >> ausgangsstatus_high;
			ss >> ausgangsstatus_low;
			ss >> reserved;
			ss >> scanfrequenz;
			ss >> messfrequenz;
			ss >> drehgeberanzahl;
			ss >> kanalzahl;
			ss >> dist_kennung; // DIST1
			ss >> m_caster.ui32;
				skalierungsfaktor=m_caster.fl;
			ss >> skalierungsoffset;
			ss >> m_caster.ui32;
				startwinkel=m_caster.i32; // zahntausendstel grad
			ss >> winkelschrittweite; // zahntausendstel grad
			ss >> datensatz_anzahl;
			for(int i=0; i<271; i++){
				ss >>  dist_daten[i];
			}



		}

	}


}


void TiM3xx_Data_Parser::print_data(){


	std::cout.precision(5);

	std::cout << kommandoart << ":"
			<< kommando << ":"
			<< versionsnummer << ":"
			<< geraetenummer << ":"
			<< std::hex <<  seriennummer << ":"
			<< geraetestatus_high<< ":"
			<< geraetestatus_low<< ":"
			<< std::dec <<telegrammzaehler<< ":"
			<< scanzaehler<< ":"
			<< kanalzahl <<":"
			<< dist_kennung <<":"
			<< std::hex <<  skalierungsfaktor<< ":"
			<< skalierungsoffset<< ":"
			<< std::dec << startwinkel<< ":"
			<< winkelschrittweite<< ":"
			<< std::dec << datensatz_anzahl <<":";

//			std::cout << std::hex;

			for(int i=0; i<271; i++){
				std::cout << dist_daten[i] <<"|";
			}


			std::cout <<std::endl;

}
