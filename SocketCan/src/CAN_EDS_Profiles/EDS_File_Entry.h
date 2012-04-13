/*
 * EDS_Entry.h
 *
 *  Created on: 23.02.2009
 *      Author: locher
 */

#ifndef EDS_FILE_ENTRY_H_
#define EDS_FILE_ENTRY_H_

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "CAN_types_and_tools.h"

#include "CAN_EDS_SDO.h"

using namespace std;

namespace sfzcan {



	class EDS_File_Entry {
	private:


	public:

		__u16 index;
		__u8 subindex;

		string parameterName;
		string objectType;
		string dataType;
		string accessType;
		string defaultValue;
		string pdoMapping;
		string subNumber;
		string lowLimit;
		string highLimit;
		string objFlags;
		string compactSubObj;

		void saveSettings(ostream& header_out_file, string classname);
		void clearAllEntries();

		EDS_File_Entry();
		virtual ~EDS_File_Entry();
	};

}  // namespace sfzcan

#endif /* EDS_FILE_ENTRY_H_ */
