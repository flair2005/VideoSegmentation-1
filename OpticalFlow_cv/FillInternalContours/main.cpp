#include <iostream>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

void fillHole(const Mat srcBw, Mat &dstBw)
{
    Size m_Size = srcBw.size();
    Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//延展图像
    srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

    cv::floodFill(Temp, Point(0, 0), Scalar(255));

    Mat cutImg;//裁剪延展的图像
    Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

    dstBw = srcBw | (~cutImg);
}

// 内轮廓填充
// 参数:
// 1. pBinary: 输入二值图像，单通道，位深IPL_DEPTH_8U。
// 2. dAreaThre: 面积阈值，当内轮廓面积小于等于dAreaThre时，进行填充。
void FillInternalContours(IplImage *pBinary, double dAreaThre)
{
    double dConArea;
    CvSeq *pContour = NULL;
    CvSeq *pConInner = NULL;
    CvMemStorage *pStorage = NULL;
    // 执行条件
    if (pBinary)
    {
        // 查找所有轮廓
        pStorage = cvCreateMemStorage(0);
        cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        // 填充所有轮廓
        cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
        // 外轮廓循环
        for (; pContour != NULL; pContour = pContour->h_next)
        {
            // 内轮廓循环
            for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
            {
                // 内轮廓面积
                dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ));
                if (dConArea <= dAreaThre)
                {
                    cvDrawContours(pBinary, pConInner, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, CV_FILLED, 8, cvPoint(0, 0));
                }
            }
        }
        cvReleaseMemStorage(&pStorage);
        pStorage = NULL;
    }
}
//int main()
//{
//    IplImage *img = cvLoadImage("/home/yangzheng/testData/test.jpg", 0);
//    IplImage *bin = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
//    cvCopy(img, bin);
//
//    FillInternalContours(bin, 200);
//
//    cvNamedWindow("img");
//    cvShowImage("img", img);
//
//    cvNamedWindow("result");
//    cvShowImage("result", bin);
//
//    cvWaitKey(-1);
//
//    cvReleaseImage(&img);
//    cvReleaseImage(&bin);
//
//    return 0;
//}

int main()
{
    Mat img = imread("/home/yangzheng/testData/test.jpg", CV_8UC1);
    Mat bin;

    fillHole(img, bin);
    imshow("org", img);
    imshow("fill", bin);

    waitKey(0);

    return 0;
}
