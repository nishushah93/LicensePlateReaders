
#include "fp.h"


plateInfo* processPlateChars( IplImage * orig_img )
{
	//Create placeholder for gray image
	IplImage * gray_img = cvCreateImage(cvGetSize(orig_img), IPL_DEPTH_8U, 1);
		
	//Convert to image grayscale
	cvCvtColor( orig_img, gray_img, CV_RGB2GRAY );
		
	//Create placeholder for black and white image
	IplImage * bw_img = cvCreateImage(cvGetSize(gray_img), IPL_DEPTH_8U,	1);

	//Convert gray image to binary (black and white)
	cvThreshold( gray_img, bw_img, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	//Invert image
	IplImage * rev_img = cvCreateImage(cvGetSize(bw_img), IPL_DEPTH_8U, 1);
	cvNot( bw_img, rev_img );

	//Save results
//	cvSaveImage( "bw_img.jpg", bw_img, NULL );
//	cvSaveImage( "rev_img.jpg", rev_img, NULL );


	//Resize the reversed image: 400x200 (435x218??)
	IplImage * resize_img = cvCreateImage(cvSize(400, 200), IPL_DEPTH_8U, 1);
	cvResize( rev_img, resize_img, CV_INTER_LINEAR) ;

	//Save results
//	cvSaveImage( "resize_img.jpg", resize_img, NULL );
	
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
//	cvSaveImage( "resize_crop.jpg", resize_crop, NULL );

	CvSize resizeCrop_size = cvGetSize( resize_crop );
	int w2 = resizeCrop_size.width;
	int h2 = resizeCrop_size.height;

	printf("\nWidth: %d\nHieght: %d\n", w2, h2);

	//Now get the characters (using stuff from commented section below
	struct plateInfo * plate_info = malloc( sizeof(plateInfo) * 8 );
	
	CvSeq * cvSeq = 0;
	CvMemStorage * storage = cvCreateMemStorage(0);
	int numContours;
	int i;

	//Poor man's debugger...
	char * plate_buffer[] = {"plate0.jpg", "plate1.jpg", "plate2.jpg",
		"plate3.jpg", "plate4.jpg","plate5.jpg", "plate6.jpg",
		"plate7.jpg"};


	//This should be 8
	numContours = cvFindContours( resize_crop, storage, &cvSeq,
			sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	
	printf("\nnumContours plate: %d\n", numContours);
	
	cvDrawContours( resize_crop, cvSeq, cvScalarAll(255), cvScalarAll(0),
			1, -1, 8, cvPoint(0,0) );
	  
	for( i = 0; i < 8; i++)
	{
		//Get bounding rect
		CvRect char_rect = cvBoundingRect( cvSeq, 0 );
		
		//Create and set individual images
		plate_info[i].charImage = NULL;
		plate_info[i].charImage = cvCreateImage(cvSize(char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

		//****Moved to 'fix' below - contour area and perimeter*****
		//plate_info[i].contourArea = cvContourArea( cvSeq, CV_WHOLE_SEQ, 0 );
		//plate_info[i].arcLength = cvArcLength( cvSeq, CV_WHOLE_SEQ, -1 );

		//Set ROI for copying and copy	
		cvSetImageROI( resize_crop, char_rect );
		cvCopy( resize_crop, plate_info[i].charImage, NULL );
	
		//For the poor man's debugger
		//cvSaveImage( plate_buffer[i], plate_chars[i], NULL );
		//cvSaveImage( plate_buffer[i], plate_info[i].charImage, NULL );

		cvSeq = cvSeq->h_next;
	}


	//Fix area and perimeter prob***********************
	for( i = 0; i < 8; i++)
	{
		cvClearMemStorage(storage);
	
  		int singlecon = cvFindContours( plate_info[i].charImage, storage,
				&cvSeq, sizeof(CvContour), CV_RETR_EXTERNAL,
				CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

		//resize_crop
		cvDrawContours( plate_info[i].charImage, cvSeq, cvScalarAll(255),
				cvScalarAll(0), 1, -1, 8, cvPoint(0,0) );

//		printf("\nnumContours single %d\n", singlecon);
		plate_info[i].contourArea = cvContourArea( cvSeq, CV_WHOLE_SEQ, 0 );
		plate_info[i].arcLength = cvArcLength( cvSeq, CV_WHOLE_SEQ, 0 );
	//	printf("plateChar contour area: %f\n", plate_info[i].contourArea);
	//	printf("plateChar perimeter: %f\n", plate_info[i].arcLength);
	//	cvSaveImage( plate_buffer[i], plate_info[i].charImage, NULL );
	}
	

	cvReleaseImage(&orig_img);
	cvReleaseImage(&gray_img);
	cvReleaseImage(&bw_img);
	cvReleaseImage(&rev_img);
	cvReleaseImage(&resize_img);
	cvReleaseImage(&resize_crop);
	cvReleaseMemStorage(&storage);

	return plate_info;
}



//@char_info: pointer to an array declared in main
charInfo * processTemplateChars( IplImage * template_img, int * numContours )
{
	//Create placeholder for black and with image
	IplImage * templateBW_img =  cvCreateImage(cvGetSize(template_img), IPL_DEPTH_8U, 1);

	//Skipping the convert to graysalce step...
	//Convert image to binary (black and white)
	cvThreshold( template_img, templateBW_img, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	//Invert image
	cvNot( templateBW_img, templateBW_img );
	
	//Save results
//	cvSaveImage( "templateBW_img.jpg", templateBW_img, NULL );

	//Now let's see if we can find (and count) the contours
	//Create necessary structures
	CvSeq * cvSeq2 = 0;
	CvMemStorage * storage2 = cvCreateMemStorage(0);

	//Find contours
//	int numContours;
	*numContours = cvFindContours( templateBW_img, storage2, &cvSeq2,
			sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	
	//Verify contour count (should be 36)
	printf("\nContours found: %d\n", *numContours);

	//Now actually put the lines in	
	cvDrawContours( templateBW_img, cvSeq2, cvScalarAll(255),
			cvScalarAll(0), 1, -1, 8, cvPoint(0,0) );

	//Fill an array with the array positing of each character for easier
	//processing later (if not now...ugh)
	//For PA_Keystone_Template.jpg:
	char * position_text[] = {"7", "5", "4", "1", "V", "9", "8", "6", "3",
			"2", "0", "Z", "Y", "X", "W", "U", "T", "S", "R", "Q", "P", "O",
			"N", "M", "L", "K", "J", "I", "H", "G", "F", "E", "D", "C", "B",
			"A"}; 

	//for PA_Template.jpg:
/*	char * position_text[] = {"9", "8", "7", "5", "3", "2", "1", "0", "Z",
			"Y", "X", "W", "V", "U", "T", "6", "4", "S", "R", "Q", "P", "O",
			"N", "M", "L", "K", "J", "I", "H", "G", "F", "E", "D", "C", "B",
			"A"};	*/

	//Allocate an array ofcharInfo structures
	struct charInfo * char_info = malloc( sizeof(charInfo) * (*numContours) );

	//Put everything in its place
	int i = 0;
	CvRect char_rect;

	while( cvSeq2 )//for( i = 0; i < (*numContours); i++)
	{
		//Get bounding rect
		char_rect = cvBoundingRect( cvSeq2, 0 );

		//Create and set individual images
		char_info[i].charImage = NULL;
		char_info[i].charImage = cvCreateImage( cvSize( char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);
		
		//Set ROI for copying and copy	
		cvSetImageROI( templateBW_img, char_rect );
		cvCopy( templateBW_img, char_info[i].charImage, NULL );

		//Set remaining charInfo data - area and perimemter moved to 'fix'
		//below
//		char_info[i].contourArea = cvContourArea( cvSeq2, CV_WHOLE_SEQ, 0 );
//		char_info[i].arcLength = cvArcLength( cvSeq2, CV_WHOLE_SEQ, -1 );
		char_info[i].charText = position_text[i]; //***might need strcpy***

		cvSeq2 = cvSeq2->h_next; 
		i++;
	}

	*numContours = i;
	printf("\nDrawn template letters: %d\n", *numContours);

	//Fix area and perimeter prob***********************
	for( i = 0; i < (*numContours); i++)
	{
		cvClearMemStorage(storage2);
	
  		int singlecon = cvFindContours( char_info[i].charImage, storage2,
				&cvSeq2, sizeof(CvContour), CV_RETR_EXTERNAL,
				CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

		//templateBW_img
		cvDrawContours( char_info[i].charImage, cvSeq2, cvScalarAll(255),
			cvScalarAll(0), 1, -1, 8, cvPoint(0,0) );

//		printf("\nnumContours single char %d\n", singlecon);
		char_info[i].contourArea = cvContourArea( cvSeq2, CV_WHOLE_SEQ, 0 );
		char_info[i].arcLength = cvArcLength( cvSeq2, CV_WHOLE_SEQ, -1 );
	//	printf("templateChar contour area: %f\n", char_info[i].contourArea);
	//	printf("templateChar perimeter: %f\n", char_info[i].arcLength);
	}

	//Test the above -  works!
//	printf("\nCharacter 23 Info---\nText: %s\nContour Area: %f\n", char_info[23].charText, char_info[23].contourArea);

/*	//Save individually (yikes!)
	char * buffer[] = {"0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg","5.jpg", "6.jpg",
		"7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg", "12.jpg", "13.jpg",
		"14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "19.jpg",
		"20.jpg", "21.jpg", "22.jpg", "23.jpg", "24.jpg", "25.jpg",
		"26.jpg", "27.jpg", "28.jpg", "29.jpg", "30.jpg", "31.jpg",
		"32.jpg", "33.jpg", "34.jpg", "35.jpg"};
		
	for( i = 0; i < *numContours; i++)
	{
		cvSaveImage( buffer[i], char_info[i].charImage, NULL );
		//template_chars[i]
	}
*/
	//Free stuff (put here temporarily)
	cvReleaseImage(&template_img);
	cvReleaseImage(&templateBW_img);
//	for( i = 0; i < numContours; i++)
//	{		cvReleaseImage(&template_chars[i]);		}
//	free(template_chars);

	cvReleaseImage(&template_img);
	cvReleaseImage(&templateBW_img);
	cvReleaseMemStorage(&storage2);

	return char_info;
}



char** compareChar( charInfo * char_info, plateInfo * plate_info, int * numContours )
{
	//Now that that's done, we can compare...
	//Delcare a buffer to hold the comparison results
	//char * plate_number[7];
	char ** plate_number = malloc(sizeof(char*) * 7);

	//Get size of plate images to resize template for comparison
//	IplImage * char_img = cvCreateImage( cvSize(char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);
	IplImage * char_img = NULL; // = cvCreateImage( cvGetSize( plate_info[1].charImage ), IPL_DEPTH_8U, 1);
	
/*	//Did we actually get the plates?
	char * plate_buffer[] = {"plate0.jpg", "plate1.jpg", "plate2.jpg",
		"plate3.jpg", "plate4.jpg","plate5.jpg", "plate6.jpg",
		"plate7.jpg"};

	for( i = 0; i < 7; i++ )
	{
		cvSaveImage( plate_buffer[i], plate_chars[i], NULL );
	}
*/

	//Compare method 3: histograms!
	int i = 1;	//Don't compare the keystone
	int j = 0;
	int pos = 0;
	float range[] = {0, 255};
	float * ranges[] = { range };
	int hist_size = 255;
	double result = 0;


	//Try adding more stuff...
	double match = 1; //To hold the double returned by cvMatchShape
	double temp = 0;

	double plateArea = 0;
	double charArea = 0;
	double platePer = 0;
	double charPer = 0;

	//Allocate histograms
	CvHistogram * plate_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY,
			ranges, 1);
	CvHistogram * char_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY,
			ranges, 1);


	//CompareMethod 4: Adding a countnonzero on each column to "method 3" area
	IplImage * plateRow = NULL;
	IplImage * charRow = NULL;
	CvSize plateSize;
	CvSize charSize;
	int plateH;
	int plateW;
	int charH;
	int charW;
	int plateNZCount = 0;
	int charNZCount = 0;
	int countDif = 0;
	int totalSim = 0;
	int totalDif = 0;
	int maxSim = 0;
	int k; //row counter for individual images

	for(i=1; i<8; i++)	//while( i < 8 ) 
	{
		cvCalcHist(&plate_info[i].charImage, plate_hist, 0, NULL);

		char_img = cvCreateImage( cvGetSize( plate_info[i].charImage ), IPL_DEPTH_8U, 1);

		for( j=0; j<(*numContours); j++)		//	while( j < *numContours )
		{
			cvResize(char_info[j].charImage, char_img, CV_INTER_LANCZOS4); 	

//			cvSaveImage( "char_img.jpg", char_img, NULL );

			cvCalcHist(&char_img, char_hist, 0, NULL); 
			cvCalcHist(&plate_info[i].charImage, plate_hist, 0, NULL); //char_img

			result = cvCompareHist( plate_hist, char_hist, CV_COMP_CORREL );
//			temp = cvMatchShapes( char_info[j].charImage, plate_info[i].charImage,
//					 CV_CONTOURS_MATCH_I1, 0 ); //char_img
			
			temp = cvMatchShapes( char_img, plate_info[i].charImage,
					 CV_CONTOURS_MATCH_I1, 0 ); //char_img

//			printf("char_info pos: %d   letter: %s   ", pos, char_info[pos].charText);
//			printf("temp: %f\n", temp );

//			if( result >= .9 ) //.853 
			if( temp <= .017 ) //.129 //.055 //.017
			{	
//				if( temp >= .18 && temp <= .4 ) 
//				if( result >= .9 ) //.853 
			//		plateArea = plate_info[i].contourArea;
			//		charArea = char_info[j].contourArea;
					platePer = plate_info[i].arcLength;
					charPer = char_info[j].arcLength;
					double perRatio = (platePer/charPer);
	
//					printf("temp: %f  match: %f  result: %f\n", temp, match,
//							result);
//						printf("  perRatio: %f  areaRatio: %f\n", perRatio,
//								areaRatio);

					if(temp <= match)
					{		
						plateSize = cvGetSize( plate_info[i].charImage );
						plateH = plateSize.height;
						plateW = plateSize.width;
						
						charSize = cvGetSize ( char_img );
						charH = charSize.height;
						charW = charSize.width;
					
						if( plateH != charH )
						{
							printf("\nHeights not equal!!\n");
							printf("plateH: %d   charH: %d\n", plateH, charH);
							exit(0);
						}
		
						for( k = 0; k < charH; k++ )
						{
							plateRow = cvCreateImage(cvSize(plateW, 1), IPL_DEPTH_8U, 1);
							cvSetImageROI(plate_info[i].charImage, cvRect(0, k, plateW, 1));
							cvCopy(plate_info[i].charImage, plateRow, NULL);
							cvResetImageROI(plate_info[i].charImage);

							charRow = cvCreateImage(cvSize(charW, 1), IPL_DEPTH_8U, 1);
							cvSetImageROI(char_img, cvRect(0, k, charW, 1));
							cvCopy(char_img, charRow, NULL);
							cvResetImageROI(char_img);

							plateNZCount = cvCountNonZero( plateRow );
							charNZCount = cvCountNonZero( charRow );
							countDif = charNZCount - plateNZCount;
							//countDif = plateNZCount - charNZCount; 

//							printf("plateNZCount: %d  charNZCount: %d countDif: %d\n", plateNZCount, charNZCount, countDif);
							if( countDif == 0 )	//The magic happens HERE...
								totalSim++;
							else
								totalDif++;						
								
							cvReleaseImage(&plateRow);
							cvReleaseImage(&charRow);
						}
			
						printf("char_info j: %d  letter: %s  ", j, char_info[j].charText);
						printf("temp: %f  result: %f  ", temp, result);
						printf("totalSim: %d  totalDif: %d  sim-dif: %d\n", totalSim, totalDif, (totalSim-totalDif));

						if( totalSim > maxSim )	
						{
							match = temp;
							maxSim = totalSim;
							pos = j;
						}
		
						totalSim = 0;
						totalDif = 0;

/*						//Maybe use this later... Goes just under "if(temp<=match)"
						if( (perRatio <= 1.98 && perRatio >= 1.905) ||
							(perRatio <= 1.894 && perRatio >= 1.85))				
						{	match = temp;
							pos = j;
							}*/		
					}
//					printf("char_info pos: %d   letter: %s   ", pos,
//							char_info[pos].charText); 
				}
	//		}
			cvClearHist(char_hist);
		}

		plate_number[i] = char_info[pos].charText;
	
		cvClearHist(plate_hist);
		cvReleaseImage(&char_img);
		match=1;
		maxSim = 0;		

	}

	//For histogram method 3
	cvReleaseHist(&plate_hist);
	cvReleaseHist(&char_hist);

	return plate_number;
}


