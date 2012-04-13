#ifndef driver_logging_H
#define driver_logging_H

/*
* Logging support for player drivers, driver class needs
* to have a member variable named driverName containing the name of the driver.
*
*/


#include <iostream>
#include <iomanip>

using namespace std;

#define LOG(m) \
cout << "LOG   " << "[ " << setw(15) << left << this->driverName << " ]  : " << m << endl

#define WARNING(m) \
cout << "WARN  " << "[ " << setw(15) << left << this->driverName << " ]  : " << m << endl

#define ERROR(m) \
cout << "ERROR " << "[ " << setw(15) << left << this->driverName << " ]  : " << m << endl


#endif //driver_logging_H
