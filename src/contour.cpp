#include <legacy/compat.hpp>
#include <highgui/highgui.hpp>
#include <contrib/contrib.hpp>
#include <imgproc/imgproc.hpp>
#include <imgproc/imgproc_c.h>

#include <stdio.h>

//Some defines we left out of the book
#define CVX_RED		CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN	CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE	CV_RGB(0x00,0x00,0xff)

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2 
double
angle(CvPoint * pt1, CvPoint * pt2, CvPoint * pt0)
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 +
            dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}


//  Example 8-3. Finding and drawing contours on an input image
int main(int argc, char* argv[]) {

  cvNamedWindow( argv[0], 0 );
  IplImage* img_8uc1 = NULL;
  
  //Changed this a little for safer image loading and help if not
  if( argc != 2 || !(img_8uc1 = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE )) ){
      printf("\nExample 8_3 Drawing Contours\nCall is:\n./ch8_ex8_3 image\n\n");
  return -1;}
  
  
  IplImage* img_edge = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );
  IplImage* img_8uc3 = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
  cvThreshold( img_8uc1, img_edge, 128, 255, CV_THRESH_BINARY );
  CvMemStorage* storage = cvCreateMemStorage();
  CvSeq* first_contour = NULL;
  int Nc = cvFindContours(
     img_edge,
     storage,
     &first_contour,
     sizeof(CvContour),
     CV_RETR_CCOMP// Try all four values and see what happens
  );
  int n=0,k;
  printf( "Total Contours Detected: %d\n", Nc );
  cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
  CvSeq* contours=first_contour;
  while (contours) {
//  for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
     double area;
     area=fabs(cvContourArea(contours, CV_WHOLE_SEQ));
     if(area < 10000.0) {
	contours = contours->h_next;
         continue;
     }
     printf("area:%f\n",area);
//     cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );




    CvSeq *result;
double s,t;

		// approximate contour with accuracy proportional
		// to the contour perimeter

/********** IMPOTANT NUMBER TO TUNE **************/

		result = cvApproxPoly(contours, sizeof(CvContour), storage,
		    CV_POLY_APPROX_DP, cvContourPerimeter(contours) * 0.05,
		    0);


printf("result->total %d\n",result->total);
		// square contours should have 4 vertices after approximation
		// relatively large area (to filter out noisy contours)
		// and be convex.
		// Note: absolute value of an area is used because
		// area may be positive or negative - in accordance with the
		// contour orientation
		if (result->total == 4 &&
	//	    fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 500.0
	//	    && fabs(cvContourArea(result, CV_WHOLE_SEQ)) < 50000.0
		    cvCheckContourConvexity(result)) {
		    s = 0;


     cvDrawContours(
        img_8uc3,
        contours,
        CVX_RED,  //Yarg, these are defined above, but not in the book.  Oops
        CVX_BLUE,
        0,        // Try different values of max_level, and see what happens
        2,
        8
     );



int i;

		    for (i = 0; i < 5; i++) {
			// find minimum angle between joint
			// edges (maximum of cosine)
			if (i >= 2) {
			    t = fabs(angle(
				    (CvPoint *) cvGetSeqElem(result, i),
				    (CvPoint *) cvGetSeqElem(result, i - 2),
				    (CvPoint *) cvGetSeqElem(result, i - 1)));
			    s = s > t ? s : t;
			}
		    }

		    // if cosines of all angles are small
		    // (all angles are ~90 degree) then write quandrange
		    // vertices to resultant sequence 
//		    if (s < 0.3)
//			for (i = 0; i < 4; i++)
//			    cvSeqPush(squares,
//				(CvPoint *) cvGetSeqElem(result, i));
		}
		// take the next contour







 //    printf("Contour #%d\n", n );
     cvShowImage( argv[0], img_8uc3 );
//     printf(" %d elements:\n", c->total );
     for( int i=0; i<contours->total; ++i ) {
     CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, contours, i );
//        printf("    (%d,%d)\n", p->x, p->y );
     }
/*
     if((k = cvWaitKey()&0x7F) == 27)
       break;
*/
	contours = contours->h_next;
     n++;
  }
  printf("Finished all contours. Hit key to finish\n");
//  cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
  cvShowImage( argv[0], img_8uc3 );
  cvWaitKey(0);
  cvDestroyWindow( argv[0] );
  cvReleaseImage( &img_8uc1 );
  cvReleaseImage( &img_8uc3 );
  cvReleaseImage( &img_edge );
  return 0;
}

