// Farneback dense optical flow calculate and show in Munsell system of colors
// Author : Zouxy
// Date   : 2013-3-15
// HomePage : http://blog.csdn.net/zouxy09
// Email  : zouxy09@qq.com

// API calcOpticalFlowFarneback() comes from OpenCV, and this
// 2D dense optical flow algorithm from the following paper:
// Gunnar Farneback. "Two-Frame Motion Estimation Based on Polynomial Expansion".
// And the OpenCV source code locate in ..\opencv2.4.3\modules\video\src\optflowgf.cpp

#include <iostream>
#include <cmath>
#include "opencv2/opencv.hpp"
extern "C"
{
#include <vl/kmeans.h>
#include <stdlib.h>
}

using namespace cv;
using namespace std;

#define UNKNOWN_FLOW_THRESH 1e9

// Color encoding of flow vectors from:
// http://members.shaw.ca/quadibloc/other/colint.htm
// This code is modified from:
// http://vision.middlebury.edu/flow/data/
void makecolorwheel(vector<Scalar> &colorwheel)
{
    int RY = 15;
    int YG = 6;
    int GC = 4;
    int CB = 11;
    int BM = 13;
    int MR = 6;

    int i;

    for (i = 0; i < RY; i++) colorwheel.push_back(Scalar(255,       255*i/RY,     0));
    for (i = 0; i < YG; i++) colorwheel.push_back(Scalar(255-255*i/YG, 255,       0));
    for (i = 0; i < GC; i++) colorwheel.push_back(Scalar(0,         255,      255*i/GC));
    for (i = 0; i < CB; i++) colorwheel.push_back(Scalar(0,         255-255*i/CB, 255));
    for (i = 0; i < BM; i++) colorwheel.push_back(Scalar(255*i/BM,      0,        255));
    for (i = 0; i < MR; i++) colorwheel.push_back(Scalar(255,       0,        255-255*i/MR));
}

void motionToColor(Mat flow, Mat &color)
{
    if (color.empty())
        color.create(flow.rows, flow.cols, CV_8UC3);

    static vector<Scalar> colorwheel; //Scalar r,g,b
    if (colorwheel.empty())
        makecolorwheel(colorwheel);

    // determine motion range:
    float maxrad = -1;

    // Find max flow to normalize fx and fy
    for (int i= 0; i < flow.rows; ++i)
    {
        for (int j = 0; j < flow.cols; ++j)
        {
            Vec2f flow_at_point = flow.at<Vec2f>(i, j);
            float fx = flow_at_point[0];
            float fy = flow_at_point[1];
            if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
                continue;
            float rad = sqrt(fx * fx + fy * fy);
            maxrad = maxrad > rad ? maxrad : rad;
        }
    }

    for (int i= 0; i < flow.rows; ++i)
    {
        for (int j = 0; j < flow.cols; ++j)
        {
            uchar *data = color.data + color.step[0] * i + color.step[1] * j;
            Vec2f flow_at_point = flow.at<Vec2f>(i, j);

            float fx = flow_at_point[0] / maxrad;
            float fy = flow_at_point[1] / maxrad;
            if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
            {
                data[0] = data[1] = data[2] = 0;
                continue;
            }
            float rad = sqrt(fx * fx + fy * fy);

            float angle = atan2(-fy, -fx) / CV_PI;
            float fk = (angle + 1.0) / 2.0 * (colorwheel.size()-1);
            int k0 = (int)fk;
            int k1 = (k0 + 1) % colorwheel.size();
            float f = fk - k0;
            //f = 0; // uncomment to see original color wheel

            for (int b = 0; b < 3; b++)
            {
                float col0 = colorwheel[k0][b] / 255.0;
                float col1 = colorwheel[k1][b] / 255.0;
                float col = (1 - f) * col0 + f * col1;
                if (rad <= 1)
                    col = 1 - rad * (1 - col); // increase saturation with radius
                else
                    col *= .75; // out of range
                data[2 - b] = (int)(255.0 * col);
            }
        }
    }
}

void CovtcvMat2vlMat(Mat *cvmat, double *outmat)
{
    int rows = cvmat->rows;
    int cols = cvmat->cols;
    int channels = cvmat->channels();
    outmat = (double*)malloc(sizeof(double)*rows*cols*channels);
    //outmat = new double[rows*cols*channels];
    for(int i=0;i<rows;i++)
        for(int j = 0;j<cols;j++)
        {
            outmat[(i*rows+j)*channels] = cvmat->at<Vec2f>(i,j)[0];
            outmat[(i*rows+j)*channels+1] = cvmat->at<Vec2f>(i,j)[1];
            //std::cout<<outmat[(i*rows+j)*channels]<<","<<outmat[(i*rows+j)*channels+1]<<std::endl;
        }
}

void OpticalFlowNormalization(Mat *ofMat)
{
    double temp;
    for(int i=0;i<ofMat->rows;i++)
        for(int j = 0;j<ofMat->cols;j++)
        {
            temp = ::sqrt(::pow(ofMat->at<Vec2f>(i,j)[0],2)+::pow(ofMat->at<Vec2f>(i,j)[1],2));
            ofMat->at<Vec2f>(i,j)[0] = ofMat->at<Vec2f>(i,j)[0]/temp;
            ofMat->at<Vec2f>(i,j)[1] = ofMat->at<Vec2f>(i,j)[1]/temp;
            //std::cout<<outmat[(i*rows+j)*channels]<<","<<outmat[(i*rows+j)*channels+1]<<std::endl;
        }
}

void DrawOpticalFlow(Mat *ofMat, Mat *showImg)
{
    int x,y;
    for(int i=0;i<ofMat->rows;i++)
        for(int j = 0;j<ofMat->cols;j++)
        {
            cv::Vec2f point = ofMat->at<Vec2f>(i,j);
            x = cvRound(point[0]);
            y = cvRound(point[1]);
            std::cout<<point[0]<<","<<point[1]<<std::endl;
            showImg->at<uchar>(x, y) = 255;
            //std::cout<<outmat[(i*rows+j)*channels]<<","<<outmat[(i*rows+j)*channels+1]<<std::endl;
        }
//    std::cout<<showImg<<std::endl;
//    std::cout<<*ofMat<<std::endl;

}

//void vlClustering(Mat *cvmat, vl_uint K, double *centers)
//{
//    double energy;
//    float *data;
//    vl_size numData, dimension, numCenters;
//    numData = cvmat->cols*cvmat->rows;
//    dimension = cvmat->channels();
//    numCenters = K;
//    CovtcvMat2vlMat(cvmat, data);
//    KMeans * kmeans = vl_kmeans_new(VLDistanceL2, VL_TYPE_FLOAT);
//    vl_kmeans_set_algorithm(kmeans, VlKMeansLloyd);
//    vl_kmeans_init_centers_with_rand_data(kmeans, data, dimension, numData, numCenters);
//    vl_kmeans_set_max_num_iterations(kmeans, 100);
//    vl_kmeans_refine_centers(kmeans, data, numData);
//    energy = vl_kmeans_get_energy(kmeans);
//    centers = vl_kmeans_get_centers(kmeans);
//}

/*
*计算视频光流
*/
int main(int, char**)
{
    VideoCapture cap;
    cap.open("/home/yangzheng/testData/dance.mp4");

    if( !cap.isOpened() )
        return -1;

    Mat prevgray, gray, flow, cflow, frame;
    namedWindow("flow", 1);
    cap >> frame;

    Mat motion2color;
    for(;;)
    {
        double t = (double)cvGetTickCount();

        cap >> frame;
        cvtColor(frame, gray, CV_BGR2GRAY);
        imshow("original", frame);
        Mat showOFMat(gray.rows,gray.cols,CV_8UC1,Scalar(0));
        std::cout<<showOFMat<<std::endl;

        if( prevgray.data )
        {
            calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
            //motionToColor(flow, motion2color);
            //imshow("flow", motion2color);
            std::cout<<flow.rows<<","<<flow.cols<<std::endl;
            DrawOpticalFlow(&flow, &showOFMat);
            //imshow("opticalFlow",*showOFMat);
        }
        if(waitKey(10)>=0)
            break;
        std::swap(prevgray, gray);

        t = (double)cvGetTickCount() - t;
        cout << "cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;
    }
    return 0;
}

/*
*计算两张图像间光流
*/
//int main(int, char**)
//{
//    Mat prevgray = imread("./OpticalFlow0.jpg",CV_LOAD_IMAGE_GRAYSCALE); //第k帧
//    Mat gray = imread("./OpticalFlow1.jpg",CV_LOAD_IMAGE_GRAYSCALE); //第k+1帧
//
//    Mat flow, cflow, frame;
//    namedWindow("flow", 1);
//
//    Mat motion2color;
//    double t = (double)cvGetTickCount();
//
//    imshow("pregray", prevgray);
//    imshow("oftergray", gray);
//
//    calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
//    motionToColor(flow, motion2color);
//
//    imshow("flow", motion2color);
//    waitKey(0);
//
//    t = (double)cvGetTickCount() - t;
//    cout << "cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;
//    return 0;
//}
