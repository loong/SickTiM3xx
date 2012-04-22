/*
 * MeasurementDatasetParser.h
 *
 *  Created on: 22.04.2012
 *      Author: locher
 */

#ifndef MEASUREMENTDATASETPARSER_H_
#define MEASUREMENTDATASETPARSER_H_

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <string>
#include <sstream>

class Measurement_Dataset_Parser {

private:

	uint8_t* actual_data_ptr;
	int	data_size;

public:
	Measurement_Dataset_Parser();
	virtual ~Measurement_Dataset_Parser();


	char kommandoart[4];
	char kommando[12];
	uint16_t versionsnummer;
	uint16_t geraetenummer;
	uint32_t seriennummer;
	uint8_t geraetestatus_high;
	uint8_t geraetestatus_low;
	uint16_t telegrammzaehler;
	uint16_t scanzaehler;
	uint32_t einschaltdauer_mus;
	uint32_t uebertragungsdauer_mus;
	uint8_t eingangsstatus_high;
	uint8_t eingangsstatus_low;
	uint8_t ausgangsstatus_high;
	uint8_t ausgangsstatus_low;
	uint16_t reserved;
	uint32_t scanfrequenz;
	uint32_t messfrequenz;
	uint16_t drehgeberanzahl;
	uint16_t kanalzahl;
	char dist_kennung[6]; // DIST1
	int32_t skalierungsfaktor;
	uint32_t skalierungsoffset;
	int32_t startwinkel; // zahntausendstel grad
	uint16_t winkelschrittweite; // zahntausendstel grad
	uint16_t datensatz_anzahl;
	uint16_t dist_daten[271];




	void set_pointer_to_data_buf(uint8_t* data_buf_ptr,int the_data_size);


	void parse_data();


	void print_data();

};

#endif /* MEASUREMENTDATASETPARSER_H_ */
