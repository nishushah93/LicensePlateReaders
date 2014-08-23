LicensePlateReaders
===================

The purpose of this project is to develop a program that will identify, extract, and store information from an image of an American style license plate. The program will extract the plate number using elements of the OpenCV library and image correlation techniques. The program will also identify the plate’s issuing state (limited to Pennsylvania or New Jersey). Finally, the program will store the extracted information in an XML style database that will contain each plate number and issuing state.  Due to time constraints the images used in this stage of the program will be limited to a selection of pre-acquired images of similar scale, depth, and quality. The purpose here is for our team to develop software that can perform a successful and accurate extraction of data. Future versions of this software could implement support for additional states, as well as the ability to process images acquired in real time.


To compile the code run the following command: 

 gcc -I/usr/include/opencv -I/usr/include/opencv2 -L /usr/local/lib -o fp lpstate.h lpstate.c fpex.c fp.c fp.h database.h database.c -lopencv_core -lopencv_imgproc -lopencv_highgui -lm
