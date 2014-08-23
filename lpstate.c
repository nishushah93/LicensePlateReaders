#ifndef lpstate_c
#define lpstate_c


#include "cv.h"
#include "highgui.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

// first two functions from HW4 can be reused to help
// these two will help determine what colors the three sections picked out are


double colorDistance(CvScalar c1, CvScalar c2) {
	double d = 0; // the result
	int i; // an iterator

	// TODO: iterate over the dimensions and compute the sum
	for(i = 0; i < 3; i++)
	{
		d += pow(c2.val[i] - c1.val[i], 2);
	}


	// TODO: return the square root of the result.
	// If d is zero, just return 0.
	if (d == 0)
	{
		return(0);
	}
	else
	{
		// math.h is included so sqrt can be used
		return sqrt(d);
	}
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




char* findState(char** filenames) {
    
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
    
    char* state;
             
    state = (char*) malloc(2 * sizeof(char));
             

    // resize the image
    src = cvLoadImage(filenames[1], CV_LOAD_IMAGE_COLOR);
    resize = cvCreateImage(cvSize(400, 200), IPL_DEPTH_8U, src->nChannels);
    cvResize( src, resize, CV_INTER_LINEAR) ; 
    
    
    //currently these args are just 3 pictures of colors i took screenshots of
    blue = cvLoadImage(filesnames[2], CV_LOAD_IMAGE_COLOR);
    white = cvLoadImage(filenames[3], CV_LOAD_IMAGE_COLOR);
    yellow = cvLoadImage(filenames[4], CV_LOAD_IMAGE_COLOR);
    
    // create the pixel images
    firstPixel = cvCreateImage(cvSize(5,5), resize->depth, resize->nChannels);
    secPixel = cvCreateImage(cvSize(5,5), resize->depth, resize->nChannels);
    thirdPixel = cvCreateImage(cvSize(5,5), resize->depth, resize->nChannels);
    
    // start breaking up into pixels
    
    cvSetImageROI(resize, cvRect(380,20,5,5));
    
    cvCopy(resize, firstPixel, NULL);
    
    cvResetImageROI(resize);
    
    cvSetImageROI(resize, cvRect(380, 70, 5, 5));
    
    cvCopy(resize, secPixel, NULL);
    
    cvResetImageROI(resize);
    
    cvSetImageROI(resize, cvRect(380, 180, 5, 5));
    
    cvCopy(resize, thirdPixel, NULL);
    
    cvResetImageROI;
    
    
    // get avg for the colors
    
    colors = (CvScalar*) malloc (3 * sizeof(CvScalar));
    
    colors[0] = cvAvg(blue, NULL);
    colors[1] = cvAvg(white, NULL);
    colors[2] = cvAvg(yellow, NULL);
    
    // get avg for pix
    
    firstPixScal = cvAvg(firstPixel, NULL);
    secPixScal = cvAvg(secPixel, NULL);
    thirdPixScal = cvAvg(thirdPixel, NULL);
    
    cColors = (int*) malloc(3 * sizeof(int));
    
    
    // find what color the pixels are closest to
    cColors[0] = findClosest(firstPixScal, colors, 3);
    cColors[1] = findClosest(secPixScal, colors, 3);
    cColors[2] = findClosest(thirdPixScal, colors, 3);
    
    
    // determine the state based of the colors    
    if( (cColors[0] == 2) && (cColors[1] == 2) && (cColors[2] == 2))
    {
        state = "NJ";
    }
    else if ((cColors[0] == 0) && (cColors[1] == 1) && (cColors[2] == 2))
    {
         state = "PA";
    }
    else
    {
        state = "NA";
    }
    


    
    return(state);
}

    
         



#endif
