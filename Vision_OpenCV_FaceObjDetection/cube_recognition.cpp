/*
 * Created: 2011-06-17 15:29:49+05:30
 * Author: dhruv
 */
// OpenCV Application: cube recognition

// Include header files
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
// Create memory for calculations
static CvMemStorage* storage = 0;

// Create a new Haar classifier
static CvHaarClassifierCascade* cascade = 0;

// Function prototype for detecting and drawing an object from an image
void detect_and_draw( IplImage* image );

// Create a string that contains the cascade name
const char* cascade_name =
    "haarcascade_cubedetection.xml"; //haarcascade_cuberecognition.xml is generated by haartraining facility of opencv and using positive images of cube and other negative images.

// Main function, defines the entry point for the program.
int main( int argc, char** argv )
{

    // Structure for getting video from camera or avi
    CvCapture* capture = 0;

    // Images to capture the frame from video or camera or from file
    IplImage *frame, *frame_copy = 0;

    // Load the HaarClassifierCascade

    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
   
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return -1;
    }
  
    // Allocate the memory storage
    storage = cvCreateMemStorage(0);

    // Capture camera to get frames from. For mobile, give 0 for rear camera and 1 for front camera.
    capture = cvCaptureFromCAM( 0 ); 

    // Create a new named window with title: result
    cvNamedWindow( "result", 1 );

    // Find if the capture is loaded successfully or not.

    // If loaded succesfully, then:
    if( capture )
    {
        // Capture from the camera.

        for(;;)
        {
           // Capture the frame and load it in IplImage
            if( !cvGrabFrame( capture ))
                break;
            frame = cvQueryFrame( capture );

            // If the frame does not exist, quit the loop
            if( !frame )
                break;
            
            // Allocate framecopy as the same size of the frame
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );

            // Check the origin of image. If top left, copy the image frame to frame_copy. 
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            // Else flip and copy the image
            else
                cvFlip( frame, frame_copy, 0 );
            
            // Call the function to detect and draw the face
            detect_and_draw( frame_copy );
//		cvShowImage("result",frame_copy);

            // Wait for a while before proceeding to the next frame
            if( cvWaitKey( 10 ) >= 0 )
                break;
        }

        // Release the images, and capture memory
        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }

    // If the capture is not loaded succesfully, then:
    else
    {
    	printf("Camera couldn't be found.\n");
    }
    
    // Destroy the window previously created with filename: "result"
    cvDestroyWindow("result");

    // return 0 to indicate successfull execution of the program
    return 0;
}

// Function to detect and draw any faces that is present in an image
void detect_and_draw( IplImage* img )
{
    //int scale = 3;

    // Create a new image based on the input image with lower resolution to increase fps.
    IplImage* temp = cvCreateImage( cvSize(150,150), 8, 3 );

    //Copy current frame in new lower resolution small image.
    cvResize(img,temp,CV_INTER_LINEAR);

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    int i;

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence

       CvSeq* faces = cvHaarDetectObjects( temp, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40,40));

        // Loop the number of faces found.
     for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x;
            pt2.x = (r->x+r->width);
            pt1.y = r->y;
            pt2.y = (r->y+r->height);

            // Draw the rectangle in the small image
            cvRectangle( temp, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
        }
    }

    // Resize small image in correct resolution again to show it.
    cvResize(temp,img,CV_INTER_LINEAR);

    // Show the image in the window named "result"
    cvShowImage( "result", img );

    // Release the temp image created.
    cvReleaseImage( &temp );
}
