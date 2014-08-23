#ifndef lpstate_c
#define lpstate_c

#include "cv.h"
#include "highgui.h"

double colorDistance(CvScalar c1, CvScalar c2);
int findClosest(CvScalar t, CvScalar* scolors, int numColors);
char* findState(char** filenames);


#endif

