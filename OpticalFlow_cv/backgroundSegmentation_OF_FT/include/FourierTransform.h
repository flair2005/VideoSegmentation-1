#ifndef FOURIERTRANSFORM_H
#define FOURIERTRANSFORM_H

#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class FourierTransform
{
    public:
        FourierTransform();
        virtual ~FourierTransform();
        bool DFT(Mat src);
        inline Mat *GetCompleteImg(){return &m_completeI;};
        inline Mat *GetMagImg(){return &m_magI;};

    protected:

    private:
        Mat m_completeI;
        Mat m_magI;
};

#endif // FOURIERTRANSFORM_H
