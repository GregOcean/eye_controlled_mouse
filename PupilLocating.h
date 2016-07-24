#include "stdafx.h"
#include "cv.h"
#include "highgui.h"

//Í«¿×¶¨Î»Àà
#ifndef PUPIL_H
#define PUPIL_H

class PupilLocating{
private:
	void count(IplImage*& ,/*IplImage * ,*/CvPoint ,CvPoint ,CvPoint& );
	//void count(IplImage* ,IplImage * ,CvPoint ,CvPoint ,CvPoint& );
	void Coordinate(IplImage* ,int& ,int& ,int& ,int &);
public:
	void detectEyes(IplImage * ,CvPoint p[]);
};

#endif //PUPIL_H///:~