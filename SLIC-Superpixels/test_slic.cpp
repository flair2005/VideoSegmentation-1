/*
 * test_slic.cpp.
 *
 * Written by: Pascal Mettes.
 *
 * This file creates an over-segmentation of a provided image based on the SLIC
 * superpixel algorithm, as implemented in slic.h and slic.cpp.
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <float.h>
using namespace std;

#include "slic.h"



//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//string filename;
//Mat image;
//string winName = "show";
//enum{NOT_SET = 0, IN_PROCESS = 1, SET = 2};
//uchar rectState;
//Rect rect;
//Mat mask;
//const Scalar GREEN = Scalar(0,255,0);
//Mat bgdModel, fgdModel;
//
//void setRectInMask(){
//	rect.x = max(0, rect.x);
//	rect.y = max(0, rect.y);
//	rect.width = min(rect.width, image.cols-rect.x);
//	rect.height = min(rect.height, image.rows-rect.y);
//
//}
//
//static void getBinMask( const Mat& comMask, Mat& binMask ){
//	binMask.create( comMask.size(), CV_8UC1 );
//	binMask = comMask & 1;
//}
//
//void on_mouse( int event, int x, int y, int flags, void* )
//{
//	Mat res;
//	Mat binMask;
//
//	switch( event ){
//		case CV_EVENT_LBUTTONDOWN:
//			if( rectState == NOT_SET){
//				rectState = IN_PROCESS;
//				rect = Rect( x, y, 1, 1 );
//			}
//			break;
//		case CV_EVENT_LBUTTONUP:
//			if( rectState == IN_PROCESS ){
//				rect = Rect( Point(rect.x, rect.y), Point(x,y) );
//				rectState = SET;
//				(mask(rect)).setTo( Scalar(GC_PR_FGD));
//				image = imread( filename, 1 );
//				grabCut(image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);
//				getBinMask( mask, binMask );
//				image.copyTo(res, binMask );
//				imshow("11", res);
//			}
//			break;
//		case CV_EVENT_MOUSEMOVE:
//			if( rectState == IN_PROCESS ){
//				rect = Rect( Point(rect.x, rect.y), Point(x,y) );
//				image = imread( filename, 1 );
//				rectangle(image, Point( rect.x, rect.y ), Point(rect.x + rect.width, rect.y + rect.height ), GREEN, 2);
//				imshow(winName, image);
//			}
//			break;
//	}
//}
//
//int main(int argc, char* argv[]){
//	filename = argv[1];
//	image = imread( filename, 1 );
//	imshow(winName, image);
//	mask.create(image.size(), CV_8UC1);
//	rectState = NOT_SET;
//	mask.setTo(GC_BGD);
//
//	setMouseCallback(winName, on_mouse, 0);
//	waitKey(0);
//
//	return 0;
//}



int main(int argc, char *argv[]) {
    /* Load the image and convert to Lab colour space. */
    char *filename = "./0.jpg\0";
    IplImage *image = cvLoadImage(filename, 1);
    IplImage *lab_image = cvCloneImage(image);
    IplImage *gray_image = cvCreateImage(cvGetSize(image),image->depth,1);
    IplImage *edge_imag = cvCreateImage(cvGetSize(image),image->depth,1);

    cvCvtColor(image, gray_image, CV_RGB2GRAY);
    cvCanny(gray_image, edge_imag, 150, 33);
    cvShowImage("aa", edge_imag);

//    cvSmooth(image, lab_image, CV_MEDIAN, 5);
//    cvShowImage("temp", lab_image);

    cvCvtColor(image, lab_image, CV_BGR2Lab);

    /* Yield the number of superpixels and weight-factors from the user. */
    int w = image->width, h = image->height;
    // 超参数,超像素数目K, 设图像总像素为N,则一个分割区域的超像素点大致为N/K个
    int nr_superpixels = 800;
    // weight-factors
    int nc = 40;

    /*计算一个分割区域的超像素点*/
    double step = sqrt((w * h) / (double) nr_superpixels);

    /* Perform the SLIC superpixel algorithm. */
    Slic slic;
    slic.generate_superpixels(lab_image, step, nc);
    slic.create_connectivity(lab_image);

    /* Display the contours and show the result. */
    slic.display_new_contours(image, CV_RGB(255,0,0));
    slic.display_centers(image, CV_RGB(0,255,0));
    slic.compute_feature(image);
    cvShowImage("result", image);
    cvWaitKey(0);
}
