/*
 * ComputerVisionExample.h
 *
 *  Created on: 31.05.2011
 *      Author: locher
 */

#ifndef COMPUTERVISIONEXAMPLE_H_
#define COMPUTERVISIONEXAMPLE_H_

#include "ComputerVisionBaseClass.h"
#include "OpenCV_GTK_Image.h"
#include "Example_Cam_Proxy.h"

class ComputerVisionExample : public ComputerVisionBaseClass, public  OpenCV_GTK_Image{

private:
	virtual void process_image(IplImage*);

public:
	ComputerVisionExample();
	virtual ~ComputerVisionExample();
};

#endif /* COMPUTERVISIONEXAMPLE_H_ */
