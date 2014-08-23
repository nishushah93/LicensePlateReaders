#ifndef lpstate_c
#define lpstate_c


double colorDistance(CvScalar c1, CvScalar c2);
int findClosest(CvScalar t, CvScalar* scolors, int numColors);
char* findState(char** filenames);


#endif

