#include <cv.h>
#include "highgui.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

int main (argc, char ** argv) {
    
    IplImage *src,
             *resize,
             *blue,
             *white,
             *yellow;
    
    IplImage *firstPixel,
             *secPixel,
             *thirdPixel;
    
    CvScalar firstPixScal,
             secPixScal,
             thirdPixScal;
             
    CvScalar *colors;
    
    int i;
    
    int *cColors;
             
    
             

    
    src = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
    resize = cv_CreateImage(cvSize(400, 200), IPL_DEPTH_8U, 1);
    cvResize( srv, resize, CV_INTER_LINEAR) ; 
    
    
    //currently these args are just 3 pictures of colors i took screenshots of
    blue = cvLoadImage(argv[2], CV_LOAD_IMAGE_COLOR);
    white = cvLoadImage(argv[3], CV_LOAD_IMAGE_COLOR);
    yellow = cvLoadImage(argv[4], CV_LOAD_IMAGE_COLOR);
    
    
    firstPixel = cvCreateImage(cvSize(2,2), resize->depth, resize->nChannels);
    secPixel = cvCreateImage(cvSize(2,2), resize->depth, resize->nChannels);
    thirdPixel = cvCreateImage(cvSize(2,2), resize->depth, resize->nChannels);
    
    // start breaking up into pixels
    
    cvSetImageROI(resize, cvRect(380,5,400,200));
    
    cvCopy(resize, firstPixel, NULL);
    
    cvResetImageROI(resize);
    
    cvSetImageROI(resize, cvRect(380, 100, 400, 200));
    
    cvCopy(resize, secPixel, NULL);
    
    cvResetImageROI(resize);
    
    cvSetImageROI(resize, cvRect(380, 180, 400, 200));
    
    cvResetImageROI;
    
    
    // get avg for the colors
    
    colors = (CvScalar*) malloc (3 * sizeof(CvScalar));
    
    colors[0] = cvAvg(blue, NULL);
    colors[1] = cvAvg(white, NULL);
    colors[2] = cvAvg(yellow, NULL);
    
    // get avg for pix
    
    firtPixScal = cvAvg(firstPixel, NULL);
    secPixScal = cvAvg(secPixel, NULL);
    thirdPixScal = cvAvg(thirdPixScal);
    
    cColors = (int*) malloc(3 * sizeof(int));
    
    cColors[0] = findClosest(firstPixScal, colors, 3);
    cColors[1] = findClosest(secPixScal, colors, 3);
    cColors[2] = findClosest(secPixScal, colors, 3);
    
    
    printf( "color 1: %d\n", cColors[0]);
    
    printf("color 2: %d\n", cColors[1]);
    
    printf("color 3: %d\n", cColors[2]);
    
    return(0);

}

    
         



int findClosest(CvScalar t, CvScalar* scolors, int numColors) {
	int rv = 0, // return value
		 i; // used to iterate
	double d, // stores the result of distance
	       m = colorDistance(t, scolors[0]); // the current minmum distance
	// TODO: iterate over scolors
    for (i=0; i < numColors; i++)
    {
		// TODO: compute the distance between t and scolors[i]
        d = colorDistance(t,scolors[i]);
        
		// TODO: check if the distance is less then current minimum
        if(d < m)
        {
			// TODO: if it is, store i as the current result and cache the minimum distance
            rv = i;
            m = d;
        }
    }
    
	// TODO: return result.
    return rv;
}           
