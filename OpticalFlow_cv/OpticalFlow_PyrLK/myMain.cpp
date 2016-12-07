
// Example 10-1. Pyramid Lucas-Kanade optical flow code
//
/* *************** License:**************************
   Oct. 3, 2008
   Right to use this code in any way you want without warrenty, support or any guarentee of it working.

   BOOK: It would be nice if you cited it:
   Learning OpenCV: Computer Vision with the OpenCV Library
     by Gary Bradski and Adrian Kaehler
     Published by O'Reilly Media, October 3, 2008

   AVAILABLE AT:
     http://www.amazon.com/Learning-OpenCV-Computer-Vision-Library/dp/0596516134
     Or: http://oreilly.com/catalog/9780596516130/
     ISBN-10: 0596516134 or: ISBN-13: 978-0596516130

   OTHER OPENCV SITES:
   * The source code is on sourceforge at:
     http://sourceforge.net/projects/opencvlibrary/
   * The OpenCV wiki page (As of Oct 1, 2008 this is down for changing over servers, but should come back):
     http://opencvlibrary.sourceforge.net/
   * An active user group is at:
     http://tech.groups.yahoo.com/group/OpenCV/
   * The minutes of weekly OpenCV development meetings are at:
     http://pr.willowgarage.com/wiki/OpenCV
   ************************************************** */

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>

const int MAX_CORNERS = 500;
int main(int argc, char** argv) {
   // Initialize, load two images from the file system, and
   // allocate the images and other structures we will need for
   // results.
    //
    IplImage* imgA = cvLoadImage("OpticalFlow0.jpg",CV_LOAD_IMAGE_GRAYSCALE); //第k帧
    IplImage* imgB = cvLoadImage("OpticalFlow1.jpg",CV_LOAD_IMAGE_GRAYSCALE); //第k+1帧
    CvSize      img_sz    = cvGetSize( imgA );
    int         win_size = 10;
    IplImage* imgC = cvLoadImage("OpticalFlow1.jpg",CV_LOAD_IMAGE_UNCHANGED);

    // The first thing we need to do is get the features
    // we want to track.
    //
    IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
    IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
    int              corner_count = MAX_CORNERS;
    CvPoint2D32f* cornersA        = new CvPoint2D32f[ MAX_CORNERS ];
    cvGoodFeaturesToTrack( //确定图像的强角点
        imgA,           //原图
        eig_image,      //临时浮点32-位图像
        tmp_image,      //临时浮点32-位图像
        cornersA,       //输出，用于存放探测出的强角点的CvPoint2D32f对象指针
        &corner_count,  //输出，检测到的角点数
        0.01,
        5.0,
        0,
        3,
        0,
        0.04
    );
    cvFindCornerSubPix( //用于发现亚像素精度的角点位置
        imgA, //输入的图像，必须是8位的灰度或者彩色图像。
        cornersA, //输入角点的初始坐标，也存储精确的输出坐标。
        corner_count,  //角点数目
        cvSize(win_size,win_size), //搜索窗口的一半尺寸。如果win=（5,5）那么使用（5*2+1）×（5*2+1）=11×11大小的搜索窗口
        cvSize(-1,-1), //死区的一半尺寸，死区为不对搜索区的中央位置做求和运算的区域。
        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03) //求角点的迭代过程的终止条件。
    );
    // Call the Lucas Kanade algorithm
    //
    char features_found[ MAX_CORNERS ];
    float feature_errors[ MAX_CORNERS ];
    CvSize pyr_sz = cvSize( imgA->width+8, imgB->height/3 );
    IplImage* pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
  IplImage* pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
  CvPoint2D32f* cornersB        = new CvPoint2D32f[ MAX_CORNERS ];
  cvCalcOpticalFlowPyrLK(
     imgA,
     imgB,
     pyrA,      /*第一帧的金字塔缓存. 如果指针非 NULL , 则缓存必须有足够的空间来存储金字塔从层 1 到层 #level 的内容。
                尺寸 (image_width+8)*image_height/3 比特足够了*/
     pyrB,      //第二帧的金字塔缓存
     cornersA,  //需要发现光流的点集
     cornersB,  //包含新计算出来的位置的点集
     corner_count,//特征点的数目
     cvSize( win_size,win_size ),//每个金字塔层的搜索窗口尺寸
     5, //最大的金字塔层数。如果为 0 , 不使用金字塔 (即金字塔为单层), 如果为 1 , 使用两层，下面依次类推
     features_found, //数组。如果对应特征的光流被发现，数组中的每一个元素都被设置为 1， 否则设置为 0。
     feature_errors, //双精度数组，包含原始图像碎片与移动点之间的差。为可选参数，可以是 NULL
     cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 ), //准则，指定在每个金字塔层，为某点寻找光流的迭代过程的终止条件。
     0  //flag参数
  );
  // Now make some image of what we are looking at:
  //在展示图像imgC上会指出光流的轨迹
  for( int i=0; i<corner_count; i++ ) {
     if( features_found[i]==0|| feature_errors[i]>550 ) {
 //       printf("Error is %f/n",feature_errors[i]);
        continue;
     }
 //    printf("Got it/n");
     CvPoint p0 = cvPoint(
        cvRound( cornersA[i].x ),
        cvRound( cornersA[i].y )
     );
     CvPoint p1 = cvPoint(
        cvRound( cornersB[i].x ),
        cvRound( cornersB[i].y )
     );
     cvLine( imgC, p0, p1, CV_RGB(255,0,0),2 );
  }
  cvNamedWindow("ImageA",0);
  cvNamedWindow("ImageB",0);
  cvNamedWindow("LKpyr_OpticalFlow",0);
  cvShowImage("ImageA",imgA);
  cvShowImage("ImageB",imgB);
  cvShowImage("LKpyr_OpticalFlow",imgC);
  cvWaitKey(0);
  return 0;
}
