/*
 * EDS_File_Parser.cpp
 *
 *  Created on: 23.02.2009
 *      Author: locher
 */

#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <libpcan.h>
#include <sstream>
#include <vector>
#include <map>

#include "EDS_File_Entry.h"



using namespace std;

// @TODO organize EDS Files for Baumer inclination Sensor

namespace sfzcan {



	//Global Variables

	// Save all Parameternames to check if a Parametername is unique
	vector<string> parameternames;


	// Which EDS Entries are Array record Types
	multimap<__u16,string> arrayRecordNames;


	pair<__u16,string> recordEntry;
	string value;

//involved filenames

/**
 * Add Filepath and headerfilename for new EDS Files to
 * generate the makros for EDS_SDO creation
 */


//	const string the_eds_filename="eds_files/Maxon_Epos_24_5.eds";
//	const string the_header_filename="src/CAN_EDS_Profiles/Maxon_Epos_24_5_Profile.h";
//	const string profilname="Epos_24_5";


//	const string the_eds_filename="eds_files/EstAIM04.eds";
//	const string the_header_filename="src/CAN_EDS_Profiles/EstAIM04_Profile.h";
//	const string profilname="AIM04";


	const string the_eds_filename="eds_files/Maxon_MCD_EPOS_60W.eds";
	const string the_header_filename="src/CAN_EDS_Profiles/Maxon_Mcd_Epos_60W_Profile.h";
	const string profilname="Epos_60W";


//		const string the_eds_filename="eds_files/BMSxInt.eds";
//		const string the_header_filename="src/CAN_EDS_Profiles/BMSxInt_Profile.h";
//		const string profilname="BMS";

	//outputstreams
	ofstream* pHeader_outputstream;
//	ofstream* pImplementation_outputstream;



	//EDS Parameter Names as Strings
	string parameterName("ParameterName");
	string objectType("ObjectType");
	string dataType("DataType");
	string accessType("AccessType");
	string defaultValue("DefaultValue");
	string pdoMapping("PDOMapping");
	string subNumber("SubNumber");
	string lowLimit("LowLimit");
	string highLimit("HighLimit");
	string objFlags("ObjFlags");
	string compactSubObj("CompactSubObj");






	// Removing chars which are not C++ function-name conform
	// Checking if function Name is unique
	void  makeUniqueAndLeagalParameterName(string& parameterName){

		//cout << parameterName << endl;

		string::size_type pos=parameterName.find(' ');

		while((int )pos>0){
			parameterName.replace(pos,1,"_");
			pos=parameterName.find(' ',pos);
		}

		pos=parameterName.find('-');

		while((int )pos>0){
			parameterName.replace(pos,1,"_");
			pos=parameterName.find('-',pos);
		}


		// Klammern im parameterName loeschen
		pos=parameterName.find('(');

		while((int )pos>0){
			parameterName.erase(pos,1);
			pos=parameterName.find('(',pos);
		}
		//cout << parameterName << endl;

		pos=parameterName.find(')');

		while((int )pos>0){
			parameterName.erase(pos,1);
			pos=parameterName.find(')',pos);
		}
		//cout << parameterName << endl;

		// Das > Zeichen loeschen
		pos=parameterName.find('>');

		while((int )pos>0){
			parameterName.erase(pos,1);
			pos=parameterName.find('>',pos);
		}

		// Das . Zeichen loeschen
		pos=parameterName.find('.');

		while((int )pos>0){
			parameterName.erase(pos,1);
			pos=parameterName.find('.',pos);
		}

			//cout<< "> "  << parameterName << endl;



			vector<string>::iterator iter;
			bool isParameterNameInParameterNames=true;

			while(isParameterNameInParameterNames){

				bool found=false;

				for(iter=parameternames.begin(); iter!=parameternames.end(); ++iter){

					if(*iter==parameterName){
						found=true;
						break;
					}
				}
				if(found){
					parameterName.append("I");
					//cout << "Loop: " << parameterName << endl;
				}else{

					isParameterNameInParameterNames=false;
				}
			}
			iter=parameternames.end();
			parameternames.insert(iter,parameterName);
	}


	void findRecordArrayNames(ifstream& eds_file_in){


		string line;

		__u16 index=0;
		__u8 subindex=0;


		std::string messageDescription;

		bool firstTime=true;
		bool numericalIndex=false;

		bool subNumberFound=false;


		while( !getline(eds_file_in,line).eof() ){


			// Index Zeile finden
			unsigned int n=line.find(']')-1;
			if(	 (line.at(0)=='[')&& (n>0) ) {

					string entry(line.substr(1,n));

					// Index herausloesen-->entry
					if(
							isxdigit(entry.at(0) ) &&
							isxdigit(entry.at(1) ) &&
							isxdigit(entry.at(2) ) &&
							isxdigit(entry.at(3) )

					){
						numericalIndex=true;

						if(!firstTime){
							if(subNumberFound){
								value.append("_");
								recordEntry=pair<__u16,string>(index,value);
								arrayRecordNames.insert(recordEntry);
							}
							subNumberFound=false;
						}else{
							firstTime=false;
						}


						//cout <<entry << endl;

						unsigned int index_int;
						std::stringstream ss(entry);

						if(!(ss>> std::hex>>index_int)){
							cout <<"Conversion Error" << endl;
						}
						index=(__u16) index_int;
						//cout<< "Index: " <<std::hex<<  index << endl;




						// Nach Subindex suchen
						subindex=0;
						int n=entry.find("sub",4);
						if(n>=4){
							unsigned int subindex_int;
							std::stringstream ss(entry.substr(7,entry.length()-1));

							if(!(ss>> std::hex>>subindex_int)){
								cout <<"Conversion Error" << endl;
							}
							subindex=(__u8) subindex_int;

						}
						//cout<< "Subindex: "<<std::hex<< (unsigned int) subindex << endl;




					}else{
						numericalIndex=false;
					}






			}else{
				int n=line.find(parameterName,0);
				if(n>=0){
					value=line.substr(parameterName.length()+1,line.length()-(parameterName.length()+2));
					makeUniqueAndLeagalParameterName(value);
				}

				n=line.find(subNumber,0);
				if(n>=0){
					subNumberFound=true;
				}



			}
		} // End While loop

		//cout<< "saving Last Objekt:" << index << " "<< subindex << endl;
		if(subNumberFound){

			recordEntry=pair<__u16,string>(index,value);
			arrayRecordNames.insert(recordEntry);

		}

		eds_file_in.close();

	}


	void writeHeaderHead(){

	*pHeader_outputstream << "/********************************************" << endl;
	*pHeader_outputstream << "* Autogenereted Macros for CAN_EDS Konstruktion" << endl;
	*pHeader_outputstream << "* for "<< profilname<< endl;
	*pHeader_outputstream << "**********************************************/" <<endl<<endl<< endl;

	*pHeader_outputstream <<"#ifndef __"<< profilname<<"_H_"<<endl;
	*pHeader_outputstream <<"#define __"<< profilname<<"_H_"<<endl<<endl;




	}
	void writeHeaderTail(){
		*pHeader_outputstream<< endl <<"#endif" <<"//__"<< profilname<<"_H_" << endl;
	}

//	void writeImplementationHead(){
//
//	*pImplementation_outputstream << "#include \""<< classname <<".h\""<<endl<<endl;
//
//	}

} // end sfzcan Namespace




// Parst eine EDS Datei und erstellt
// Quelltexte für statische Methoden, die
// EDS_Objekt_Description Objekte zurückliefern
// Der Quelltext wird an momentan an cout ausgegeben
// und kann von dort in eine geeignete Datei eingeben werden




using namespace sfzcan;


/**
 * \brief Mit dem EDS-File Parser werden aus einer <B>E</B>lectronik-<B>D</B>ata <B>S</B>heet-Datei eines CANopen-Profiles
 * Makros zur Erzeugung eines EDS_SDOs generiert und in einer Header Datei eingebunden
 *
 */


int main(int argc, char **argv) {



	ifstream eds_file_in0(the_eds_filename.c_str());

	if(!eds_file_in0){
		cout << "Start EDS File Parser from Project path" << endl;
		cout << the_eds_filename << " konnte nicht geöffnet werden" << endl;
		return 1;
	}


	findRecordArrayNames(eds_file_in0);

	eds_file_in0.close();


	ifstream eds_file_in(the_eds_filename.c_str());



	parameternames.clear();


	pHeader_outputstream= new ofstream(the_header_filename.c_str());
	if(!pHeader_outputstream){
		cout << the_header_filename<< " konnte nicht geöffnet werden" << endl;
		return 1;
	}

//	pImplementation_outputstream=new ofstream(the_implementation_filename.c_str());
//	if(!pImplementation_outputstream){
//		cout << the_implementation_filename<< " konnte nicht geöffnet werden" << endl;
//		return 1;
//	}

	writeHeaderHead();
//	writeImplementationHead();

	string line;

	__u16 index=0;
	__u8 subindex=0;
	std::string messageDescription;

	bool firstTime=true;
	bool numericalIndex=false;

	EDS_File_Entry edsEntry;



	while( !getline(eds_file_in,line).eof() ){


		// Index Zeile finden
		unsigned int n=line.find(']')-1;
		if(	 (line.at(0)=='[')&& (n>0) ) {

				string entry(line.substr(1,n));

				// Index herausloesen-->entry
				if(
						isxdigit(entry.at(0) ) &&
						isxdigit(entry.at(1) ) &&
						isxdigit(entry.at(2) ) &&
						isxdigit(entry.at(3) )

				){
					numericalIndex=true;

					if(!firstTime){

						multimap<__u16,string>::iterator it;

						it=arrayRecordNames.find(index);

						// Im Falle eines arrayRecords Arry Record Name voranstellen
						if(edsEntry.subNumber==""  && it!=arrayRecordNames.end()){
							recordEntry=*it;
							value=recordEntry.second;
							value.append(edsEntry.parameterName);
							edsEntry.parameterName=value;

						}
						makeUniqueAndLeagalParameterName(edsEntry.parameterName);
						edsEntry.saveSettings(*pHeader_outputstream,profilname);
						edsEntry.clearAllEntries();

					}else{
						firstTime=false;
						edsEntry.clearAllEntries();
					}


					//cout <<entry << endl;

					unsigned int index_int;
					std::stringstream ss(entry);

					if(!(ss>> std::hex>>index_int)){
						cout <<"Conversion Error" << endl;
					}
					index=(__u16) index_int;
					cout<< "Index: " <<std::hex<<  index << endl;




					// Nach Subindex suchen
					subindex=0;
					int n=entry.find("sub",4);
					if(n>=4){
						unsigned int subindex_int;
						std::stringstream ss(entry.substr(7,entry.length()-1));

						if(!(ss>> std::hex>>subindex_int)){
							cout <<"Conversion Error" << endl;
						}
						subindex=(__u8) subindex_int;

					}
					cout<< "Subindex: "<<std::hex<< (unsigned int) subindex << endl;

					edsEntry.index=index;
					edsEntry.subindex=subindex;



				}else{
					numericalIndex=false;
				}






		}else{
			int n=line.find(parameterName,0);
			if(n>=0){
				edsEntry.parameterName=line.substr(parameterName.length()+1,line.length()-(parameterName.length()+2));
				cout << parameterName<<": " << edsEntry.parameterName <<endl;
				//makeUniqueAndLeagalParameterName(edsEntry.parameterName);
			}

			n=line.find(objectType,0);
			if(n>=0){
				edsEntry.objectType=line.substr(objectType.length()+1,line.length()-(objectType.length()+2));
				cout << objectType<<": " << edsEntry.objectType <<endl;
			}


			n=line.find(dataType,0);
			if(n>=0){
				edsEntry.dataType=line.substr(dataType.length()+1,line.length()-(dataType.length()+2));
				cout << dataType<<": " << edsEntry.dataType<<endl;
			}

			n=line.find(accessType,0);
			if(n>=0){
				edsEntry.accessType=line.substr(accessType.length()+1,line.length()-(accessType.length()+2));
				cout << accessType<<": " << edsEntry.accessType <<endl;
			}

			n=line.find(defaultValue,0);
			if(n>=0){
				edsEntry.defaultValue=line.substr(defaultValue.length()+1,line.length()-(defaultValue.length()+2));
				cout << defaultValue<<": " << edsEntry.defaultValue<<endl;
			}

			n=line.find(pdoMapping,0);
			if(n>=0){
				edsEntry.pdoMapping=line.substr(pdoMapping.length()+1,line.length()-(pdoMapping.length()+2));
				cout << pdoMapping<<": " << edsEntry.pdoMapping<<endl;
			}

			n=line.find(subNumber,0);
			if(n>=0){
				edsEntry.subNumber=line.substr(subNumber.length()+1,line.length()-(subNumber.length()+2));
				cout << subNumber<<": " << edsEntry.subNumber<<endl;
			}

			n=line.find(lowLimit,0);
			if(n>=0){
				edsEntry.lowLimit=line.substr(lowLimit.length()+1,line.length()-(lowLimit.length()+2));
				cout << lowLimit<<": " << edsEntry.lowLimit <<endl;
			}

			n=line.find(highLimit,0);
			if(n>=0){
				edsEntry.highLimit=line.substr(highLimit.length()+1,line.length()-(highLimit.length()+2));
				cout << highLimit<<": " << edsEntry.highLimit<<endl;
			}

			n=line.find(objFlags,0);
			if(n>=0){
				edsEntry.objFlags=line.substr(objFlags.length()+1,line.length()-(objFlags.length()+2));
				cout << objFlags<<": " << edsEntry.objFlags<<endl;
			}

			n=line.find(compactSubObj,0);
			if(n>=0){
				edsEntry.compactSubObj=line.substr(compactSubObj.length()+1,line.length()-(compactSubObj.length()+2));
				cout << compactSubObj<<": " << edsEntry.compactSubObj<<endl;
			}





		}
	} // End While loop

	//cout<< "saving Last Objekt:" << index << " "<< subindex << endl;

	multimap<__u16,string>::iterator it;

	it=arrayRecordNames.find(index);

	// Im Falle eines arrayRecords Arry Record Name voranstellen
	if(edsEntry.subNumber==""  && it!=arrayRecordNames.end()){
		recordEntry=*it;
		value=recordEntry.second;
		value.append(edsEntry.parameterName);
		edsEntry.parameterName=value;

	}
	makeUniqueAndLeagalParameterName(edsEntry.parameterName);
	edsEntry.saveSettings(*pHeader_outputstream,profilname);





	eds_file_in.close();

	writeHeaderTail();

	pHeader_outputstream->close();

}
