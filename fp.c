Enter file contents here

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


	/*************************Newly Added*******************************/
	//Resize the reversed image: 400x200 (or 435x218)
	IplImage * resize_img = cvCreateImage(cvSize(400, 200), IPL_DEPTH_8U, 1);
	cvResize( rev_img, resize_img, CV_INTER_LINEAR) ;

	//Save results
//	cvSaveImage( "resize_img.jpg", resize_img, NULL );
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
//	cvSaveImage( "resize_crop.jpg", resize_crop, NULL );

	CvSize resizeCrop_size = cvGetSize( resize_crop );
	int w2 = resizeCrop_size.width;
	int h2 = resizeCrop_size.height;

	printf("\nWidth: %d\nHieght: %d\n", w2, h2);

	//Now get the characters (using stuff from commented section below
	//Allocate image for individual character
//	IplImage * char_img = cvCreateImage( cvSize(40, h2), IPL_DEPTH_8U, 1);

	//Array to hold each character from plate - 8 chars (includes keystone)
//	IplImage ** plate_chars = malloc( sizeof(char_img) * 8 );
//	IplImage ** plate_chars = malloc( sizeof(IplImage*) * 8 );
	
	struct plateInfo * plate_info = malloc( sizeof(plateInfo) * 8 );
	
/*************************Trying this*****************************/
	//cvClearMemStorage(storage);
	//cvSeq = 0;

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
		//plate_chars[i]  = cvCreateImage(cvSize(char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

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
				&cvSeq, sizeof(CvContour), CV_RETR_TREE,
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
	
/*******************End trying this*******************************/


/*********This section works, but badly. Comment out to try something new*****
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
//		i+=h2;
		i++;

		if( i == h2*3 ) //to shift for the keystone symbol
			j+=4; //(w2/8); 
	}
*************************end badly working section*********************/

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

	cvReleaseImage(&orig_img);
	cvReleaseImage(&gray_img);
	cvReleaseImage(&bw_img);
	cvReleaseImage(&rev_img);
	cvReleaseImage(&resize_img);
	cvReleaseImage(&resize_crop);
	cvReleaseMemStorage(&storage);
//	cvReleaseImage(&allChars_img);

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
	*numContours = cvFindContours( templateBW_img, storage2, &cvSeq2, sizeof(CvContour), CV_RETR_EXTERNAL,	CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	
	//Verify contour count (should be 36)
	printf("\nContours found: %d\n", *numContours);

	//Now actually put the lines in	
	cvDrawContours( templateBW_img, cvSeq2, cvScalarAll(255),
			cvScalarAll(255), 1, -1, 8, cvPoint(0,0) );

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
	int i;
	CvRect char_rect;

	for( i = 0; i < (*numContours); i++)
	{
		//Get bounding rect
		char_rect = cvBoundingRect( cvSeq2, 0 );

//		double test = cvContourArea( cvSeq2, CV_WHOLE_SEQ, 0 );

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
	}

	//Fix area and perimeter prob***********************
	for( i = 0; i < (*numContours); i++)
	{
		cvClearMemStorage(storage2);
	
  		int singlecon = cvFindContours( char_info[i].charImage, storage2,
				&cvSeq2, sizeof(CvContour), CV_RETR_TREE,
				CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

		//templateBW_img
		cvDrawContours( char_info[i].charImage, cvSeq2, cvScalarAll(255),
			cvScalarAll(255), 1, -1, 8, cvPoint(0,0) );

//		printf("\nnumContours single char %d\n", singlecon);
		char_info[i].contourArea = cvContourArea( cvSeq2, CV_WHOLE_SEQ, 0 );
		char_info[i].arcLength = cvArcLength( cvSeq2, CV_WHOLE_SEQ, -1 );
	//	printf("templateChar contour area: %f\n", char_info[i].contourArea);
	//	printf("templateChar perimeter: %f\n", char_info[i].arcLength);
	}

	//Test the above -  works!
//	printf("\nCharacter 23 Info---\nText: %s\nContour Area: %f\n", char_info[23].charText, char_info[23].contourArea);


/*********Below all works. Commenetd out to work on building the above**********
	//Now try to get these individually...
	IplImage ** template_chars = malloc( sizeof(IplImage*) * *numContours);
	//Create an image to extract contours - this worked before, may use again later...
	//IplImage * test = cvCreateImage( cvSize( char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

	//int i;
	CvRect char_rect;
  
	for( i = 0; i < numContours; i++)
	{
		//Get bounding rect
		char_rect = cvBoundingRect( cvSeq2, 0 );
	
		//Create individual images
		template_chars[i] = cvCreateImage( cvSize( char_rect.width, char_rect.height), IPL_DEPTH_8U, 1);

		//Set ROI for copying and copy	
		cvSetImageROI( templateBW_img, char_rect );
		cvCopy( templateBW_img, template_chars[i], NULL );
		
		cvSeq2 = cvSeq2->h_next;
	}
	

	//Save individually (yikes!)
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
/*	cvReleaseImage(&template_img);
	cvReleaseImage(&templateBW_img);
	cvReleaseMemStorage(&storage);
	for( i = 0; i < numContours; i++)
	{		cvReleaseImage(&char_info[i].charImage);		}
	free( char_info );*/
//	for( i = 0; i < numContours; i++)
//	{		cvReleaseImage(&template_chars[i]);		}
//	free(template_chars);
//	cvReleaseImage(&test);

//************************End below all works section***************************/

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
	
	//Did we actually get the plates?
/*	char * plate_buffer[] = {"plate0.jpg", "plate1.jpg", "plate2.jpg",
		"plate3.jpg", "plate4.jpg","plate5.jpg", "plate6.jpg",
		"plate7.jpg"};

	for( i = 0; i < 7; i++ )
	{
		cvSaveImage( plate_buffer[i], plate_chars[i], NULL );
	}
*/


/*	//Compare method 1: cvMatchShapes (um...why is this result different than when
 *	the whole thing was in main???)
	double match = 1; //To hold the double returned by cvMatchShape
	double temp = 0;
	int pos = 0;
	int i = 1;	//Don't compare the keystone
	int j = 0;
*/		

/*	//Compare method 2: compare area and perimeter (maybe better?)
	double match = 1; //To hold the double returned by cvMatchShape
	double temp = 0;
	double plateArea = 0;
	double charArea = 0;
	double platePer = 0;
	double charPer = 0;
	double areaDif = 0;
	double perDif = 0;
	int pos = 0;
	int i = 1;	//Don't compare the keystone
	int j = 0;
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

//	CvSeq * cvSeq = 0;
//	CvSeq * cvSeq2 = 0;
//	CvMemStorage * storage = cvCreateMemStorage(0);
//	CvMemStorage * storage2 = cvCreateMemStorage(0);


	//Allocate histograms
	CvHistogram * plate_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY,
			ranges, 1);
	CvHistogram * char_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY,
			ranges, 1);


	//CompareMethod 4: Adding a countnonzero on each column to "method 3" area
	CvMat plateMat;
	CvMat charMat;
	CvSize plateSize;
	CvSize charSize;
	int plateH;
	int plateW;
	int charH;
	int charW;
	int plateNZCount;
	int charNZCount;
	int countDif;
	int totalSim = 0;
	int totalDif = 0;
	int k; //row counter for individual images

	for(i=1; i<8; i++)	//while( i < 8 ) 
	{
		cvCalcHist(&plate_info[i].charImage, plate_hist, 0, NULL);
		char_img = cvCreateImage( cvGetSize( plate_info[i].charImage ), IPL_DEPTH_8U, 1);

		for( j=0; j<(*numContours); j++)		//	while( j < *numContours )
		{
/***************Below is the one used in method 1******************
	//		cvResize(char_info[j].charImage, char_img, CV_INTER_LINEAR); 
			
			//Note this newly added...
			//cvSmooth( char_img, char_img, CV_GAUSSIAN, 3, 0, 0, 0);

			temp = cvMatchShapes( plate_info[i].charImage, char_img, CV_CONTOURS_MATCH_I3, 0 );
//			temp = cvMatchShapes( plate_info[i].charImage, char_info[j].charImage,
//					CV_CONTOURS_MATCH_I3, 0 );

//			printf("j: %d  temp: %f   letter: %s\n", j, temp, char_info[j].charText);
	
			if( temp < match )
			{
				match = temp;
		//		printf("\nmatch: %f\n", match);
				pos = j;
				printf("char_info pos: %d   match: %f   letter: %s\n", pos,
						match, char_info[pos].charText);
			}
***********************End method 1*******************************/

/***************Below is the one used in method 2******************

			//Method 2  
			//	plateArea = 0;  charArea = 0;  platePer = 0;  charPer = 0;
			plateArea = plate_info[i].contourArea;
			charArea = char_info[j].contourArea;
			platePer = plate_info[i].arcLength;
			charPer = char_info[j].arcLength;
		
//			double frac = 0;

			//printf("plateArea: %f\n", plateArea);
			//printf("charArea: %f\n", charArea);
			//printf("areaDif: %f\n",	areaDif);

	if( (plateArea / charArea <= 5.6) && ( plateArea / charArea >= 5.0) )
//			if( (modf((plateArea/charArea), &frac) >= 0) && (
//				modf((plateArea/charArea), &frac) <= .05))
		{
			temp = cvMatchShapes( plate_info[i].charImage,
					char_img, CV_CONTOURS_MATCH_I3, 0 );
			if( temp <= match )
			{
				match = temp;
		//		printf("\nmatch: %f\n", match);
				pos = j;
//				printf("char_info pos: %d   areaDif: %f   letter: %s\n", pos,
//						areaDif, char_info[pos].charText);
			}
		}
***********************End method 2*******************************/
			
	//		char_img= cvCreateImage( cvGetSize( plate_info[i].charImage ), IPL_DEPTH_8U, 1);

			cvResize(char_info[j].charImage, char_img, CV_INTER_LINEAR); 
		
//			cvSaveImage( "char_img.jpg", char_img, NULL );

			cvCalcHist(&char_img, char_hist, 0, NULL);
//			cvCalcHist(&char_info[j].charImage, char_hist, 0, NULL); //char_img

			result = cvCompareHist( plate_hist, char_hist, CV_COMP_CORREL );
//			temp = cvMatchShapes( char_info[j].charImage, plate_info[i].charImage,
//					 CV_CONTOURS_MATCH_I1, 0 ); //char_img
			
			temp = cvMatchShapes( char_img, plate_info[i].charImage,
					 CV_CONTOURS_MATCH_I1, 0 ); //char_img

//			printf("char_info pos: %d   letter: %s   ", pos, char_info[pos].charText);
//			printf("temp: %f\n", temp );

//			if( result >= .9 ) //.853 
			if( temp <= .017 ) //.129 //.055
			{	
//				printf("char_info j: %d   letter: %s   ", j, char_info[j].charText);
//				printf("temp: %f  j: %d \n", temp, j );
//				temp = cvMatchShapes( plate_info[i].charImage, char_img, CV_CONTOURS_MATCH_I1, 0 );
//				printf("char_info pos: %d   letter: %s   ", pos, char_info[pos].charText);
//				printf("temp: %f  ", temp );

//				if( temp >= .18 && temp <= .4 ) 
//				if( result >= .9 ) //.853 
//				{	
		//			int countP = cvCountNonZero( plate_info[i].charImage );
		//			int countC = cvCountNonZero( char_img );
			//		printf("NonZero Plate Char: %d   ", countP );
			//		printf("NonZero Char Char: %d\n", countC );
			//		plateArea = plate_info[i].contourArea;
			//		charArea = char_info[j].contourArea;
					platePer = plate_info[i].arcLength;
					charPer = char_info[j].arcLength;
		//
			//		double frac = modf((plateArea/charArea), &frac); 
			//		if( (frac <= .9) && (frac >= 0) )
			//		if( (countP/countC) > 9.98);
			//		{
					
						double perRatio = (platePer/charPer);
			//			double areaRatio = (plateArea/charArea);

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
								cvGetRow( plate_info[i].charImage, &plateMat, k );
								cvGetRow( char_img, &charMat, k );

								plateNZCount = cvCountNonZero( &plateMat );
								charNZCount = cvCountNonZero( &charMat );
								countDif =  charNZCount - plateNZCount; 	
								if( countDif == 0 )	//The magic happens HERE...
									totalSim++;
								else
									totalDif++;							
							}
			
							printf("char_info j: %d   letter: %s   ", j, char_info[j].charText);
							printf("temp: %f  ", temp);
							printf("totalSim: %d   totalDif: %d  countDif: %d\n",
									totalSim, totalDif, countDif);

							cvSetZero(&plateMat);
							cvSetZero(&charMat);							

							if( totalSim > totalDif )	
							{
								match = temp;
								pos = j;
							}
		
							totalSim = 0;
							totalDif = 0;

/*							//Maybe use this later... Goes just under "if(temp<=match)"
							if( (perRatio <= 1.98 && perRatio >= 1.905) ||
								(perRatio <= 1.894 && perRatio >= 1.85))				
							{	match = temp;
								pos = j;
							}			*/
					}
	//					printf("char_info pos: %d   letter: %s   ", pos,
	//							char_info[pos].charText); 
					}
//				}
	//		}
		//	cvReleaseImage(&char_img);
			cvClearHist(char_hist);
//			j++;
		}

		plate_number[i] = char_info[pos].charText;
	
		cvClearHist(plate_hist);
		cvReleaseImage(&char_img);
//		IplImage * char_img = NULL;
//		printf("\nmatch: %f\n", match);
//		printf("\nchar_info pos: %d\n", pos);
//		i++;
//		j=0;
//		pos=0;
		match=1;

	}

/*	//Mkay, this should do it..	
	//Print that bad boy!
	printf("\nPlate number: ");
	for(i = 1; i < 8; i++)
	{
		printf("%s", plate_number[i]);
	}
	printf("\n");
*/
//	cvReleaseImage(&char_img);

	//For histogram method 3
	cvReleaseHist(&plate_hist);
	cvReleaseHist(&char_hist);
	//cvReleaseMat(charMat);
	//cvReleaseMat(plateMat);

//	cvReleaseMemStorage(&storage);

	return plate_number;
}


