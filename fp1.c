
#include <cv.h>
#include "highgui.h"
//#include "/usr/include/opencv2/opencv.hpp"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

int main( int argc, char ** argv )
{
	if( argc < 3 )
		exit (0);
	else
	{
		//Mkay, let's deal with the template
		//Load template image
		IplImage * template_img = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE);

		//Create placeholder for black and with image
		IplImage * templateBW_img =  cvCreateImage(cvGetSize(template_img), IPL_DEPTH_8U, 1);

		//Skipping the convert to graysalce step...
		//Convert image to binary (black and white)
		cvThreshold( template_img, templateBW_img, 128, 255, CV_THRESH_BINARY |
				CV_THRESH_OTSU);

		//Invert image
		cvNot( templateBW_img, templateBW_img );

		//Save results
//		cvSaveImage( "templateBW_img.jpg", templateBW_img, NULL );

		
		//Now let's see if we can find (and count) the contours
		//Create necessary structures
		CvSeq * cvSeq = 0;
		CvMemStorage * storage = cvCreateMemStorage(0);

		//Find contours
		int numContours;
		numContours = cvFindContours( templateBW_img, storage, &cvSeq, sizeof(CvContour),
				CV_RETR_EXTERNAL,	CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	
		printf("\nContours found: %d\n", numContours);	//should be 36

		//Now actually put the lines in	
		cvDrawContours( templateBW_img, cvSeq, cvScalarAll(255),
				cvScalarAll(255), 1, -1, 8, cvPoint(0,0) );

		//Declare a structure for holding everything we need
		typedef struct charInfo charInfo;
		struct charInfo
		{
			IplImage * charImage;
			double contourArea;
			char * charText;
		}; 

		//Fill an array with the array positing of each character for easier
		//processing later (if not now...ugh)
		char * position_text[] = {"9", "8", "7", "5", "3", "2", "1", "0", "Z",
			"Y", "X", "W", "V", "U", "T", "6", "4", "S", "R", "Q", "P", "O",
			"N", "M", "L", "K", "J", "I", "H", "G", "F", "E", "D", "C", "B",
			"A"};

		//Allocate an array ofcharInfo structures
		charInfo * char_info = malloc( sizeof(charInfo) * numContours );

		//Put everything in its place
		int i;
		CvRect char_rect;

		for( i = 0; i < numContours; i++)
		{
			//Get bounding rect
			char_rect = cvBoundingRect( cvSeq, 0 );
		
			//Create and set individual images
			char_info[i].charImage  = cvCreateImage( cvSize( char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

			//Set ROI for copying and copy	
			cvSetImageROI( templateBW_img, char_rect );
			cvCopy( templateBW_img, char_info[i].charImage, NULL );

			//Set remaining charInfo data
			char_info[i].contourArea = cvContourArea( cvSeq, CV_WHOLE_SEQ, 0 );
			char_info[i].charText = position_text[i];

			cvSeq = cvSeq->h_next;
		}
		
		//Test the above -  works!
//		printf("\nCharacter 23 Info---\nText: %s\nContour Area: %f\n",
//				char_info[23].charText, char_info[23].contourArea);


		/*******Below all works. Commenetd out to work on building the above**********
		//Now try to get these individually...
		IplImage ** template_chars = malloc( sizeof(IplImage*) * numContours);
		//Create an image to extract contours - this worked before, may use again later...
		//IplImage * test = cvCreateImage( cvSize( char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

		int i;
		CvRect char_rect;
	  
		for( i = 0; i < numContours; i++)
		{
			//Get bounding rect
			char_rect = cvBoundingRect( cvSeq, 0 );
		
			//Create individual images
			template_chars[i] = cvCreateImage( cvSize( char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

			//Set ROI for copying and copy	
			cvSetImageROI( templateBW_img, char_rect );
			cvCopy( templateBW_img, template_chars[i], NULL );
		
			cvSeq = cvSeq->h_next;
		}
	
*/	
/*		//Save individually (yikes!)
		char * buffer[] = {"0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg","5.jpg", "6.jpg",
			"7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg", "12.jpg", "13.jpg",
			"14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "19.jpg",
			"20.jpg", "21.jpg", "22.jpg", "23.jpg", "24.jpg", "25.jpg",
			"26.jpg", "27.jpg", "28.jpg", "29.jpg", "30.jpg", "31.jpg",
			"32.jpg", "33.jpg", "34.jpg", "35.jpg"};
		
		for( i = 0; i < numContours; i++)
		{
			cvSaveImage( buffer[i], char_info[i].charImage, NULL );
			//template_chars[i]
		}
*/
		//Free stuff (put here temporarily)
/*		cvReleaseImage(&template_img);
		cvReleaseImage(&templateBW_img);
		cvReleaseMemStorage(&storage);
		for( i = 0; i < numContours; i++)
		{		cvReleaseImage(&char_info[i].charImage);		}
		free( char_info );
//		for( i = 0; i < numContours; i++)
//		{		cvReleaseImage(&template_chars[i]);		}
//		free(template_chars);
//		cvReleaseImage(&test);

		**********************End below all works section***************************/

		//Load original license image
		IplImage * orig_img = cvLoadImage( argv[2], CV_LOAD_IMAGE_COLOR );

		//Create placeholder for gray image
		IplImage * gray_img = cvCreateImage(cvGetSize(orig_img), IPL_DEPTH_8U, 1);
		
		//Convert to image grayscale
		cvCvtColor( orig_img, gray_img, CV_RGB2GRAY );
		
		//Create placeholder for black and white image
		IplImage * bw_img = cvCreateImage(cvGetSize(gray_img), IPL_DEPTH_8U,	1);

		//Convert gray image to binary (black and white)
		cvThreshold( gray_img, bw_img, 128, 255, CV_THRESH_BINARY |
				CV_THRESH_OTSU);

		//Invert image
		IplImage * rev_img = cvCreateImage(cvGetSize(bw_img), IPL_DEPTH_8U, 1);
		cvNot( bw_img, rev_img );

		//Save results
//		cvSaveImage( "bw_img.jpg", bw_img, NULL );
//		cvSaveImage( "rev_img.jpg", rev_img, NULL );


		/*************************Newly Added*******************************/
		//Resize the reversed image: 400x200 (or 435x218)
		IplImage * resize_img = cvCreateImage(cvSize(400, 200), IPL_DEPTH_8U, 1);
		cvResize( rev_img, resize_img, CV_INTER_LINEAR) ;

		//Save results
//		cvSaveImage( "resize_img.jpg", resize_img, NULL );
		/*********************End Newly Added******************************/
	
		//Okay, now find the reversed (and resized) image's size
		CvSize resize_size = cvGetSize( resize_img );
		int w = resize_size.width;
		int h = resize_size.height;

		printf("Width: %d\nHieght: %d\n", w, h);

		//Allrighty, now try to crop
		//First, create new image in the right size
		//Old data for below: cvSize( w, h-108 ) (w-30, h-108)
		IplImage * resize_crop = cvCreateImage(cvSize(w-60, h-108), IPL_DEPTH_8U, 1);

		//Old data for below: cvRect( 0, 54, w, h-108)  (15, 54 w-30, h-108)
		cvSetImageROI( resize_img, cvRect(30, 54, w-60, h-108) );//15
		cvCopy( resize_img, resize_crop, NULL );

		//Save this result
//		cvSaveImage( "resize_crop.jpg", resize_crop, NULL );

		CvSize resizeCrop_size = cvGetSize( resize_crop );
		int w2 = resizeCrop_size.width;
		int h2 = resizeCrop_size.height;

		printf("\nWidth: %d\nHieght: %d\n", w2, h2);

		//Now get the characters (using stuff from commented section below
		//Allocate image for individual character
//		IplImage * char_img = cvCreateImage( cvSize(40, h2), IPL_DEPTH_8U, 1);

		//Array to hold each character from plate - 8 chars (includes keystone)
//		IplImage ** plate_chars = malloc( sizeof(char_img) * 8 );
		IplImage ** plate_chars = malloc( sizeof(IplImage*) * 8 );

		
		/********Trying this **************************/
			
		cvClearMemStorage(storage);
		cvSeq = 0;

		//Poor man's debugger...
/*		char * plate_buffer[] = {"plate0.jpg", "plate1.jpg", "plate2.jpg",
			"plate3.jpg", "plate4.jpg","plate5.jpg", "plate6.jpg",
			"plate7.jpg"};
*/
		int numContours2 = cvFindContours( resize_crop, storage, &cvSeq, sizeof(CvContour),
				CV_RETR_EXTERNAL,	CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
		
		cvDrawContours( resize_crop, cvSeq, cvScalarAll(255),
				cvScalarAll(255), 1, -1, 8, cvPoint(0,0) );
	  
		for( i = 0; i < numContours2; i++)
		{
			//Get bounding rect
			char_rect = cvBoundingRect( cvSeq, 0 );
		
			//Create and set individual images
			plate_chars[i]  = cvCreateImage(cvSize(char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

			//Set ROI for copying and copy	
			cvSetImageROI( resize_crop, char_rect );
			cvCopy( resize_crop, plate_chars[i], NULL );
	
//			cvSaveImage( plate_buffer[i], plate_chars[i], NULL );

			cvSeq = cvSeq->h_next;
		}
	
		IplImage * char_img = cvCreateImage( cvSize(char_rect.width,
					char_rect.height), IPL_DEPTH_8U, 1);
	/************End trying this************/


		/****This section works, but badly. Comment out to try something new*****
		//Get increment variables ready
		i = 0;
		int j;
	
		//Get an array of images from plate
		for(j = 0; j < w2-40; j+=40) //j=15
		{
			cvSetImageROI( resize_crop, cvRect(j, 0, 40, h2)); // j,0,50,h2
			
			//cvSetImageROI(allChars_img, cvRect(0, i, 40, h2));
			plate_chars[i] = cvCreateImage(cvGetSize(char_img), IPL_DEPTH_8U, 1);

			//cvCopy( resize_crop, allChars_img, NULL);
			cvCopy( resize_crop, plate_chars[i], NULL);

			//cvResetImageROI(allChars_img);
			cvResetImageROI(resize_crop);
	
			//Shify x,y as needed for each picture	
			j+=2;
//			i+=h2;
			i++;

			if( i == h2*3 ) //to shift for the keystone symbol
				j+=4; //(w2/8); 
		}
		*************************end badly working section*********************/
		
		//Now that that's done, we can compare...
		//Delcare a buffer to hold the comparison results
		char * plate_number[7];
	
		//Compare method 1: cvMatchShapes (um...)
		double match = 1; //To hold the double returned by cvMatchShape
		double temp = 0;
		int pos = 0;
		i = 1;
		int j = 0;
		
		CvSeq * cvSeq2 = 0;
		cvClearMemStorage(storage);

		//Poor man's debugger...
/*		char * plate_buffer[] = {"plate0.jpg", "plate1.jpg", "plate2.jpg",
			"plate3.jpg", "plate4.jpg","plate5.jpg", "plate6.jpg",
			"plate7.jpg"};

	
		for( i = 0; i < 7; i++ )
		{
			cvFindContours( plate_chars[i], storage, &cvSeq2, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
			cvDrawContours( plate_chars[i], cvSeq2, cvScalarAll(255),
					cvScalarAll(255), 1, -1, 8, cvPoint(0,0) );
//			cvSaveImage( plate_buffer[i], plate_chars[i], NULL );
		}
*/

		while( i < 8 ) //7
		{
			while( j < numContours ) //for( j = 0; j < numContours; j++ )
			{
				cvResize(char_info[j].charImage, char_img, CV_INTER_LINEAR); 

				//Note this newly added...
				cvSmooth( char_img, char_img, CV_GAUSSIAN, 3, 0, 0, 0);

//				char_info[j].contourArea = cvContourArea( cvSeq, CV_WHOLE_SEQ, 0 );
	
//				cvFindContours( plate_chars[i], storage2, &cvSeq2, sizeof(CvContour), CV_RETR_EXTERNAL,
//						CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
//				cvDrawContours( plate_chars[i], cvSeq2, cvScalarAll(255),
//						cvScalarAll(255), 1, CV_FILLED, 8, cvPoint(0,0) );

				temp = cvMatchShapes( plate_chars[i], char_img,
						CV_CONTOURS_MATCH_I3, 0 );
//				temp = cvMatchShapes( plate_chars[i], cvSeq2, CV_CONTOURS_MATCH_I3, 0 );
//				temp = cvMatchShapes( plate_chars[i], char_info[j].charImage,
//						CV_CONTOURS_MATCH_I1, 0 );

				printf("j: %d  temp: %f   letter: %s\n", j, temp, char_info[j].charText);
		
				if( temp <= match )
				{
					match = temp;
			//		printf("\nmatch: %f\n", match);
					pos = j;
					printf("char_info pos: %d   match: %f   letter: %s\n", pos,
							match, char_info[pos].charText);
				}

				j++;
			}

			plate_number[i] = char_info[pos].charText;
			printf("\nmatch: %f\n", match);
//			printf("\nchar_info pos: %d\n", pos);
			i++;
			match = 1;
			j= 0;
			pos = 0;
	}

		//Mkay, this should do it..
		//Print that bad boy!
		printf("\nPlate number: ");
		for(i = 1; i < 8; i++)
		{
			printf("%s", plate_number[i]);
		}
		printf("\n");

/***************Below works. Commented out to work on above*****************

		//Now, try to get one individual character... Below section all works!!
//		IplImage * char_img = cvCreateImage( cvSize(40, h2), IPL_DEPTH_8U, 1);
//		cvSetImageROI(	resize_crop, cvRect(0, 0, 40, h2)); //13
//		cvCopy( resize_crop, char_img, NULL);

		//Save this result
//		cvSaveImage( "char_img.jpg", char_img, NULL );


		//See if we can get all the characters from a plate
		//New image is 1 character wide and 8 chars tall (for 2014 std plates)
		//7 chars + keystone symbol = 8

		IplImage * allChars_img = cvCreateImage(cvSize(40, (h2*8)), IPL_DEPTH_8U, 1);
		int i = 0;	//For allcars_img height, which is h2*7, minus keystone
		int j;	//For cropped image width, starting at 15, incrementing by 50

		for(j = 0; j < w2-40; j+=40) //j=15
		{
			cvSetImageROI( resize_crop, cvRect(j, 0, 40, h2)); // j,0,50,h2
//			cvCopy( resize_crop, char_img, NULL ); //Copy individual character images 
			
			cvSetImageROI(allChars_img, cvRect(0, i, 40, h2));
//	 		cvCopy( char_img, allChars_img, NULL);
			cvCopy( resize_crop, allChars_img, NULL);

			cvResetImageROI(allChars_img);
			cvResetImageROI(resize_crop);
	
			//Shify x,y as needed for each picture	
			j+=2;
			i+=h2;

			if( i == h2*3 ) //to shift for the keystone symbol
				j+=4;//(w2/8);
		}

		//Save this result
//		cvSaveImage( "allChars_img.jpg", allChars_img, NULL );
//		cvSaveImage( "char_img.jpg", char_img, NULL );

****************End working section noted above*********************************/

		//Release and free	
		cvReleaseImage(&template_img);
		cvReleaseImage(&templateBW_img);
		cvReleaseMemStorage(&storage);
		for( i = 0; i < numContours; i++)
		{		cvReleaseImage(&char_info[i].charImage);	}
		free( char_info );

		cvReleaseImage(&orig_img);
		cvReleaseImage(&gray_img);
		cvReleaseImage(&bw_img);
		cvReleaseImage(&rev_img);
		cvReleaseImage(&resize_img);
		cvReleaseImage(&resize_crop);
		cvReleaseImage(&char_img);
		for( i = 0; i < 8; i++)
		{		cvReleaseImage(&plate_chars[i]);		}
		free( plate_chars );
		
//		cvReleaseImage(&allChars_img);

		return EXIT_SUCCESS;

	}
}
