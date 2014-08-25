
#ifndef fp_H_
#define fp_H_

#include <cv.h>
#include "highgui.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

//Stucture to hold character template images and data
typedef struct charInfo charInfo;
struct charInfo
{
	IplImage * charImage;
	double contourArea;
	double arcLength;
	char* charText;
}; 


//Stucture to hold character template images and data
typedef struct plateInfo plateInfo;
struct plateInfo
{
	IplImage * charImage;
	double contourArea;
	double arcLength;
}; 

plateInfo* processPlateChars( IplImage * orig_img, int * numContoursP );
charInfo* processTemplateChars( IplImage * template_img, int * numContours );
char** compareChar( charInfo * char_info, plateInfo * plate_info, int *
		numContours, int * numContoursP );

#endif /* fp_H_ */

