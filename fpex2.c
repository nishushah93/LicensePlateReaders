#include "fp.h"
#include "database.h"
#include "lpstate.h"

int main( int argc, char ** argv )
{
	//Declarations
	IplImage * template_img;
	IplImage * plate_img;
	int numContours;
	int numContoursP;
	plateInfo * plate_info;
	charInfo * char_info;
	char** plate_number;
	char pNumber[7];
	int i;
        

	if( argc < 5 )
		exit (0);
	else
	{

	        char *state = findState(argv);

		//Load template image
		template_img = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE);
		
		//Load original license image
		plate_img = cvLoadImage( argv[2], CV_LOAD_IMAGE_COLOR );

		plate_info = processPlateChars( plate_img, &numContoursP );
		char_info = processTemplateChars( template_img, &numContours );
		plate_number = compareChar( char_info, plate_info, &numContours, &numContoursP );


		//Mkay, this should do it..	
		//Print that bad boy!
		printf("\nPlate number: ");
		for(i = 1; i < numContoursP; i++)
		{
			printf("%s", plate_number[i]);
		//	strcpy((pNumber + (i-1)), plate_number[i]);
		}
		printf("\n");

		int plateBuffer[] = {3, 2, 1, numContoursP-1, numContoursP-2, 5, 4};
		for(i = 1; i < numContoursP; i++)
		{
			strcpy((pNumber + (i-1)), plate_number[plateBuffer[i-1]]);
		}
	
		//printf("%s", pNumber);
		
		Owner owner;
		owner = newOwner(state, pNumber);
		WriteToXML(owner);

		
		return EXIT_SUCCESS;
	}   

}
