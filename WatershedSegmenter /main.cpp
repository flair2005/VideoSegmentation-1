#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "WatershedSegmenter.h"

using namespace std;
using namespace cv;


/***
* 分水岭分割算法
***/
int main()
{
    std::string imgPath("/home/yangzheng/testData/crow2.jpg");
    // Read input image 原图
    cv::Mat image= cv::imread(imgPath);
    if (!image.data)
        return 0;

    // Display the image
    cv::namedWindow("Original Image");
    cv::imshow("Original Image",image);
    // 二值图 这里进行了像素反转，因为一般我们用255白色表示前景（物体），用0黑色表示背景
    cv::Mat binary, gray, morGradient;
//    binary= cv::imread(imgPath,CV_8UC1);
    cvtColor(image, gray, CV_RGB2GRAY);
    //中值滤波
    medianBlur(gray,gray,7);
    // Display the binary image
    cv::namedWindow("gray Image");
    cv::imshow("gray Image",gray);

    morphologyEx(gray, morGradient, MORPH_GRADIENT, cv::Mat());
    // Display the binary image
    cv::namedWindow("morGradient Image");
    cv::imshow("morGradient Image",morGradient);


    cv::threshold(gray,binary,60,255,CV_THRESH_BINARY_INV);  //阈值化操作，参数thresh根据具体情况取
//    binary = 255 - binary;

    // Display the binary image
    cv::namedWindow("Binary Image");
    cv::imshow("Binary Image",binary);

    // 由二值图像获得前景。腐蚀。移除噪点与微小物体
    cv::Mat fg;
    cv::erode(binary,fg,cv::Mat(),cv::Point(-1,-1),6);

    // Display the foreground image
    cv::namedWindow("Foreground Image");
    cv::imshow("Foreground Image",fg);

    //膨胀二值图来获取背景（只有草地，没有树林）
    cv::Mat bg;
    cv::dilate(binary,bg,cv::Mat(),cv::Point(-1,-1),6);
    cv::threshold(bg,bg,1,128,cv::THRESH_BINARY_INV);
    //最后一个参数的表示 ifsrc>1,dst=0,else dst=128。这样就使背景全为灰色（128）
    // Display the background image
    cv::namedWindow("Background Image");
    cv::imshow("Background Image",bg);

    // Show markers image
    cv::Mat markers(binary.size(),CV_8U,cv::Scalar(0));
    markers= fg+bg;//使用重载操作符+
    cv::namedWindow("Markers");
    cv::imshow("Markers",markers);

    // Create watershed segmentation object
    WatershedSegmenter segmenter;

    // Set markers and process
    segmenter.setMarkers(markers);
    segmenter.process(image);

    // Display segmentation result
    cv::namedWindow("Segmentation");
    cv::imshow("Segmentation",segmenter.getSegmentation());

    // Display watersheds
    cv::namedWindow("Watersheds");
    cv::imshow("Watersheds",segmenter.getWatersheds());
    waitKey(0);
}


/***
*漫水填充算法实现
***/
//int main( )
//{
//    Mat src = imread("/home/yangzheng/testData/cows.jpg");
//
//    imshow("org imag",src);
//
//    Rect ccomp;
//
//    floodFill(src, Point(50,300), Scalar(155, 255,55), &ccomp, Scalar(20, 20, 20),Scalar(20, 20, 20));
//
//    imshow("output image",src);
//
//    waitKey(0);
//    return 0;
//}
