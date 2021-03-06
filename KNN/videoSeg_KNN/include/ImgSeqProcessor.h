#ifndef IMGSEQPROCESSOR_H
#define IMGSEQPROCESSOR_H

//opencv
#include "opencv2/imgcodecs.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
#include <vector>
//user-define
#include "myDirBrowser.h"

using namespace cv;
using namespace std;

class ImgSeqProcessor
{
public:
    ImgSeqProcessor(){};
    ImgSeqProcessor(string filePath);
    ~ImgSeqProcessor(){};
    bool ReadNextFrame(Mat &frame);
    void processVideo();
    void processImgSeq();
    bool Dilate(Mat &workMat, int erosion_elem, int erosion_size);
    bool Erode(Mat &workMat, int erosion_elem, int erosion_size);
    bool FindContours(Mat inArr, Mat &outArr);
private:
    myDirBrowser m_DB;
    Mat frame; //current frame
    Mat fgMaskKNN; //fg mask fg mask generated by KNNmethod
    Ptr<BackgroundSubtractorKNN> pKNN; //KNN Background subtractor
    string DirPath;
protected:
};

#endif // IMGSEQPROCESSOR_H
