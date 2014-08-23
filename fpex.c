E

#include "fp.h"

int main( int argc, char ** argv )
{
	//Declarations
	IplImage * template_img;
	IplImage * plate_img;
	int numContours;
	plateInfo * plate_info;
	charInfo * char_info;
	char** plate_number;
	int i;

	if( argc < 3 )
		exit (0);
	else
	{
		//Load template image
		template_img = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE);
		
		//Load original license image
		plate_img = cvLoadImage( argv[2], CV_LOAD_IMAGE_COLOR );

		plate_info = processPlateChars( plate_img );
		char_info = processTemplateChars( template_img, &numContours );
		plate_number = compareChar( char_info, plate_info, &numContours );


		//Mkay, this should do it..	
		//Print that bad boy!
		printf("\nPlate number: ");
		for(i = 1; i < 8; i++)
		{
			printf("%s", plate_number[i]);
		}
		printf("\n");


		//Release and free	
//		for( i = 0; i < numContours; i++ )
//		{	if(char_info[i].charImage != NULL)
//					cvReleaseImage(&char_info[i].charImage);	}
//		free( char_info );
		
//		for( i = 0; i < 8; i++)
//		{		cvReleaseImage(&plate_info[i].charImage);		}
//		free( plate_info );

//		for( i = 0; i < 8; i++ )
//			free( plate_number[i] );
//		free( plate_number );

		
		return EXIT_SUCCESS;
	}
}
nter file contents here
