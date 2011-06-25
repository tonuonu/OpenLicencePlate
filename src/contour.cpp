/*
 *  Copyright (c) 2011, Tõnu Samuel
 *  All rights reserved.
 *
 *  This file is part of OpenLicencePlate.
 *
 *  OpenLicencePlate is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TYROS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenLicencePlate.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <legacy/compat.hpp>
#include <highgui/highgui.hpp>
#include <contrib/contrib.hpp>
#include <imgproc/imgproc.hpp>
#include <imgproc/imgproc_c.h>

#include <stdio.h>

#define RED	CV_RGB(0xff,0x00,0x00)
#define GREEN	CV_RGB(0x00,0xff,0x00)
#define BLUE	CV_RGB(0x00,0x00,0xff)

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

int high_switch_value = 35;
float perimeter_constant = high_switch_value/1000.0;
int minimum_area= 600.0;
int maximum_area= 6000.0;

void switch_callback_h( int position ){
	perimeter_constant = position/1000.0;
}


int main(int argc, char* argv[]) {
    IplImage* img_8uc1 = NULL;


#ifdef IMAGE
    if( argc != 2 || !(img_8uc1 = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE )) ){
        printf("%s image\n",argv[0]);
        return -1;
    }
#else
    CvCapture* capture = cvCreateFileCapture( argv[1] );
    IplImage* frame;
    if( argc != 2 || !(frame = cvQueryFrame( capture )) ){
        printf("%s image\n",argv[0]);
        return -1;
    }
#endif
  
    const char* name = "Edge Detection Window";
    cvNamedWindow( name, 0 );
    cvCreateTrackbar( "Contour perimeter", name, &high_switch_value, 100, switch_callback_h );
    cvCreateTrackbar( "Min area", name, &minimum_area, 100000, NULL);
    cvCreateTrackbar( "Max area", name, &maximum_area, 100000, NULL);

    while(1) {
#ifndef IMAGE
        frame = cvQueryFrame( capture );
        img_8uc1=cvCreateImage( cvGetSize(frame), 8, 1 );
        cvCvtColor(frame,img_8uc1,CV_BGR2GRAY);
#endif
        IplImage* img_edge = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );
        IplImage* img_8uc3 = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
        cvThreshold( img_8uc1, img_edge, 128, 255, CV_THRESH_BINARY );
        CvMemStorage* storage = cvCreateMemStorage();
        CvSeq* first_contour = NULL;
        cvFindContours(
           img_edge,
           storage,
           &first_contour,
           sizeof(CvContour),
           CV_RETR_CCOMP
       );

       int n=0;
       cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
       CvSeq* contours=first_contour;
       while (contours) {
           double area=fabs(cvContourArea(contours, CV_WHOLE_SEQ));
           if(area < minimum_area || area > maximum_area) {
               contours = contours->h_next;
               continue;
           }
           CvSeq *result;
           double s,t;

           result = cvApproxPoly(contours, sizeof(CvContour), storage,
               CV_POLY_APPROX_DP, cvContourPerimeter(contours) * perimeter_constant, 0);

           if (result->total == 4 && cvCheckContourConvexity(result)) {
               s = 0;
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
                cvDrawContours(img_8uc3, contours, RED, BLUE, 0, 2, 8);
                // if cosines of all angles are small
                // (all angles are ~90 degree) then write quandrange
                // vertices to resultant sequence 
                // printf("s=%f\n",s);
	        if (s > 0.3) {
	  	/*for (i = 0; i < 4; i++) {
		    cvSeqPush(squares,(CvPoint *) cvGetSeqElem(result, i));
	        }*/
                }
            }
            contours = contours->h_next;
            n++;
        }
        cvShowImage( name, img_8uc3 );
        cvWaitKey(200);
        cvReleaseImage( &img_8uc3 );
        cvReleaseImage( &img_edge );
#ifndef IMAGE
        cvReleaseImage( &img_8uc1 );
#endif
    }
    cvDestroyWindow( argv[0] );
    return 0;
}

