/*
 * EDS_Entry.cpp
 *
 *  Created on: 23.02.2009
 *      Author: locher
 */

#include "EDS_File_Entry.h"

namespace sfzcan {





	EDS_File_Entry::EDS_File_Entry()
	:	parameterName(""),
		objectType(""),
		dataType(""),
		accessType(""),
		defaultValue(""),
		pdoMapping(""),
		subNumber(""),
		lowLimit(""),
		highLimit(""),
		objFlags(""),
		compactSubObj("")

	{
		cout << "EDS Entry Constructed" << endl;
	}

	EDS_File_Entry::~EDS_File_Entry() {
	}


	void EDS_File_Entry::saveSettings(
			ostream& header_out_file,
			string profilname){

		// Leerzeichen und - im Parameter Name  durch Unterstriche ersetzen

		string::size_type pos;

		// $NODEID+ loeschen
		pos=defaultValue.find("$NODEID+");

	//	cout << "defaultValue:" << defaultValue << (int)pos << endl;

		if((int)pos>=0){
			defaultValue.erase(pos,8);
		}


		pos=defaultValue.find("+NodeID");

	//	cout << "defaultValue:" << defaultValue << (int)pos << endl;

		if((int)pos>=0){
			defaultValue.erase(pos,7);
		}



		// Wenn es sich um einen ARRAY RECORD handelt
		// kommt bei den momentan  verwendeten eds-Dateien
		// aber gar nicht auf
		if(subNumber.length()>0){

			cout << "Don't save Array Record " <<endl;
	/*
			header_out_file<< "static EDS_Object_Description eds_" <<
				parameterName << "();"<< std::endl;

			implementation_out_file << "EDS_Object_Description "<<classname<<"::eds_" <<
					parameterName << "(){"<< std::endl << std::endl;
			implementation_out_file <<"\t EDS_Object_Description result(" << std::endl;

			implementation_out_file <<"\t\t0x"<< std::hex << (unsigned int)index << "," << std::endl;
	//		implementation_out_file <<"\t\t"<< std::hex << subindex << "," << std::endl;
			implementation_out_file <<"\t\t\""<<  parameterName<< "\"," << std::endl;
			implementation_out_file <<"\t\t0x"<<  objectType << "," << std::endl;
			implementation_out_file <<"\t\t0x"<< subNumber<< std::endl;
			implementation_out_file <<"\t\t"<< ");" << std::endl<< std::endl;

			if((int)objFlags.length()>0){
				implementation_out_file <<"\t"<< "result.setObjFlags(" << objFlags <<");" << endl;
			}

			implementation_out_file <<"\n\treturn result;" << std::endl;
			implementation_out_file <<"}" << std::endl;



	*/
		}


		// Wenn es sich um einen VAR TYPE handelt

		if(compactSubObj.length()==0 && subNumber.length()==0){


			header_out_file <<"#define "
							<<profilname<<"_"<<parameterName <<" CAN_EDS_SDO(" <<std::hex
							<<"0x"<< (unsigned int)index<< ",0x" << (unsigned int)subindex<< ",\""
							<< parameterName <<"\",";



//			header_out_file<< "static EDS_Object_Description eds_" <<
//				parameterName << "();"<< std::endl;

//			implementation_out_file << "EDS_Object_Description "<<classname<<"::eds_" <<
//					parameterName << "(){"<< std::endl << std::endl;
//
//			implementation_out_file <<"\t EDS_Object_Description result(" << std::endl;
//
//			implementation_out_file <<"\t\t0x"<< std::hex << (unsigned int)index << "," << std::endl;
//			implementation_out_file <<"\t\t"<< std::hex <<(unsigned int) subindex << "," << std::endl;
//			implementation_out_file <<"\t\t\""<<  parameterName<< "\"," << std::endl;

			string theDataType;

			std::stringstream ss(dataType);
			unsigned int nDataType;
			if(ss>> hex >>nDataType){


				switch(nDataType){

				case CAN_EDS_SDO::DATA_TYPE_INTEGER_8:
					theDataType="CAN_EDS_SDO::DATA_TYPE_INTEGER_8";
				break;

				case CAN_EDS_SDO::DATA_TYPE_INTEGER_16:
					theDataType="CAN_EDS_SDO::DATA_TYPE_INTEGER_16";
				break;


				case CAN_EDS_SDO::DATA_TYPE_INTEGER_32:
					theDataType="CAN_EDS_SDO::DATA_TYPE_INTEGER_32";
				break;


				case CAN_EDS_SDO::DATA_TYPE_UNSIGNED_8:
					theDataType="CAN_EDS_SDO::DATA_TYPE_UNSIGNED_8";
				break;


				case CAN_EDS_SDO::DATA_TYPE_UNSIGNED_16:
					theDataType="CAN_EDS_SDO::DATA_TYPE_UNSIGNED_16";
				break;


				case CAN_EDS_SDO::DATA_TYPE_UNSIGNED_32:
					theDataType="CAN_EDS_SDO::DATA_TYPE_UNSIGNED_32";
				break;

				case CAN_EDS_SDO::DATA_TYPE_VISIBLE_STRING :
					theDataType="CAN_EDS_SDO::DATA_TYPE_VISIBLE_STRING";
				break;

				case CAN_EDS_SDO::DATA_TYPE_UNKNOWN :
					theDataType="CAN_EDS_SDO::DATA_TYPE_UNKNOWN";
				break;

				case CAN_EDS_SDO::DATA_TYPE_UNSIGNED_64:
					theDataType="CAN_EDS_SDO::DATA_TYPE_UNSIGNED_64";
				break;



				default:
					theDataType="unsupported DataType";

				}

			}else{
				cout << "Conversion Error" << endl;

			}

			header_out_file<< theDataType << ",";

//			implementation_out_file <<"\t\t"<<  theDataType << "," << std::endl;

			string at;

			if(accessType=="ro"||accessType=="RO")at="CAN_EDS_SDO::ACCESS_TYPE_RO";
			if(accessType=="rw"||accessType=="RW")at="CAN_EDS_SDO::ACCESS_TYPE_RW";
			if(accessType=="wo"||accessType=="WO")at="CAN_EDS_SDO::ACCESS_TYPE_WO";
			if(accessType=="rwr"||accessType=="RWR")at="CAN_EDS_SDO::ACCESS_TYPE_RWR";
			if(accessType=="rww"||accessType=="RWW")at="CAN_EDS_SDO::ACCESS_TYPE_RWW";
			if(accessType=="const"||accessType=="Const")at="CAN_EDS_SDO::ACCESS_TYPE_CONST";


			header_out_file<< at << ",";


//			implementation_out_file <<"\t\t"<< at<< std::endl;
//			implementation_out_file <<"\t\t"<< ");" << std::endl<< std::endl;


			if((int)defaultValue.length()>0){

				header_out_file<<"true,";

				if(nDataType==CAN_EDS_SDO::DATA_TYPE_VISIBLE_STRING){

					header_out_file<< CAN_Tools::fourCharStringTo_msgdata( defaultValue)<<",";
					//implementation_out_file <<"\t"<< "result.setDefaultValue(" <<dec <<
					//CAN_Tools::fourCharStringTo_msgdata( defaultValue) <<");" << endl;
				}else{
					header_out_file<<""<<defaultValue<<",";
					//implementation_out_file <<"\t"<< "result.setDefaultValue(" << defaultValue<<");" << endl;
				}
			}else{
				header_out_file<<"false,0,";
			}


			if((int)lowLimit.length()>0){
				header_out_file<<"true,"<<""<<lowLimit<<",";
				//implementation_out_file <<"\t"<< "result.setLowLimit(" << lowLimit<<");" << endl;
			}else{
				header_out_file<<"false,0,";
			}


			if((int)highLimit.length()>0){
				header_out_file<<"true,"<<""<<highLimit<<",";
				//implementation_out_file <<"\t"<< "result.setHighLimit(" << highLimit<<");" << endl;
			}else{
				header_out_file<<"false,0,";
			}


			if((int)pdoMapping.length()>0){
				header_out_file<<"true,"<<""<<pdoMapping<<",";
				//implementation_out_file <<"\t"<< "result.setPdoMapping(" << pdoMapping<<");" << endl;
			}else{
				header_out_file<<"false,0,";
			}



			if((int)objFlags.length()>0){
				header_out_file<<"true,"<<""<<objFlags<<")"<<endl;
				//implementation_out_file <<"\t"<< "result.setObjFlags(" << objFlags <<");" << endl;
			}else{
				header_out_file<<"false,0)"<< endl;
			}


			//implementation_out_file <<"\n\treturn result;" << std::endl;
			//implementation_out_file <<"}" << std::endl;



		}
	/*
		implementation_out_file << std::hex << (unsigned int) index << std::endl;
		implementation_out_file << std::hex << (unsigned int) subindex << std::endl;
		implementation_out_file << parameterName <<std::endl;
		implementation_out_file << objectType << std::endl;
		implementation_out_file << dataType<< std::endl;
		implementation_out_file << accessType<< std::endl;
		implementation_out_file << defaultValue << std::endl;
		implementation_out_file << pdoMapping << std::endl;
		implementation_out_file << subNumber<< std::endl;
		implementation_out_file << lowLimit << std::endl;
		implementation_out_file << highLimit << std::endl;
		implementation_out_file << objFlags<< std::endl;
		implementation_out_file << compactSubObj << std::endl;
	 */
	}

	void EDS_File_Entry::clearAllEntries(){
		parameterName="";
		objectType="";
		dataType="";
		accessType="";
		defaultValue="";
		pdoMapping="";
		subNumber="";
		lowLimit="";
		highLimit="";
		objFlags="";
		compactSubObj="";
	}


}  // namespace sfzcan
