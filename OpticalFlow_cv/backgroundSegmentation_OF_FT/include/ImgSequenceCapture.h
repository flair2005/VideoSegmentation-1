#ifndef IMGSEQUENCECAPTURE_H
#define IMGSEQUENCECAPTURE_H

#include <cmath>
#include <vector>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "myDirBrowser.h"
#include "FourierTransform.h"

using namespace std;
using namespace cv;

typedef vector<vector<Point> > vec_point;

#define UNKNOWN_FLOW_THRESH 1e9
#define EDGE_SOBEL (10000+1)
#define EDGE_CANNY (10000+2)
#define EDGE_LAPLACIAN (10000+3)
#define OFMOD_SHRESHOLD (10000+11)
#define OFMOD_NONSHRESHOLD (10000+12)
//#define OF_EDGEOVERLAPPING (10000+15)
//#define OF_NONEDGEOVERLAPPING (10000+16)

void makecolorwheel(vector<Scalar> &colorwheel);
void motionToColor(Mat flow, Mat &color);
Mat _FillInternalContours(Mat Binary, double dAreaThre);
void fillHole(const Mat srcBw, Mat &dstBw);


class ImgSequenceCapture
{
    public:
        ImgSequenceCapture();
        ImgSequenceCapture(std::string DirPath);
        virtual ~ImgSequenceCapture();
        /*获取下一帧*/
        bool GetNextFrame();
        /*计算当前帧的稠密光流场*/
        bool CaculateOpticalFlow();
        /*计算光流场的模*/
        bool CaculateModOfOFlow(int _type, float alpha=1.0, float beta=0.0);
        /*将光流场转化到梦塞尔系*/
        bool ConvertToMunsellColorSystem();
        /*提取边缘，参数选择边缘提取算法*/
        bool CaculateEdge(int _type);
         /*计算边缘的光流*/
        bool CaculateOFlowByEdge(int _type);
        /*孔洞填充*/
        bool FillInternalContours(double dAreaThre=1000);
        /*边缘叠加*/
        bool EdgeOverlapping();
        /*腐蚀运算*/
        bool Erode(int erosion_elem=1, int erosion_size=1);
        /*膨胀运算*/
        bool Dilate(int erosion_elem=1, int erosion_size=1);
         /**/
        bool DFT();
        bool FindContours();
        /*背景分割*/
        bool VideoSeparation();

        inline bool GetFrameAndCoculateOFlow(int _type)
        {
            if(GetNextFrame())
            {
                return CaculateOpticalFlow();
            }
            return false;
        }
        inline bool GetFrameAndCoculateOFlowByEdge(int _type)
        {
            if(GetNextFrame())
            {
                return CaculateOFlowByEdge(_type);
            }
            return false;
        }
        inline Mat *GetCurrentFrameGray(){ return &m_currentFrameGray; };
        inline Mat *GetOpticalFlow(){ return &m_opticalFlow; };
        inline Mat *GetModOptFLow(){ return &m_modOptFlow; };
        inline Mat *GetFilledModOFLow(){ return &m_FilledmodOFLow; };
        inline Mat *GetMunsellColorOptFLow(){ return &m_MunsellColorOFlow; };
        inline Mat *GetEdge(){ return &m_edge; };
        inline Mat *GetContours(){ return &m_ContoursMap; };
        inline vector<vector<Point> > *GetContoursPoint(){ return &contoursPoint; };
        inline Mat *GetPreviousframeGray(){ return &m_previousFrameGray; };
        inline Mat *GetPreviousframeGrayShow(){ return &m_previousFrameGrayShow; };
        inline Mat *GetFTMagImg(){return ft.GetMagImg();};

    protected:

    private:
        myDirBrowser m_dbrowser;
        FourierTransform ft;
        vec_point contoursPoint;
        std::string m_DirPath;
        Mat m_currentFrame;
        Mat m_currentFrameGray;
        Mat m_previousFrameGray;
        Mat m_previousFrameGrayShow;
        Mat m_opticalFlow;
        Mat m_modOptFlow;
        Mat m_FilledmodOFLow;
        Mat m_MunsellColorOFlow;
        Mat m_edge;
        Mat m_ContoursMap;
};

#endif // IMGSEQUENCECAPTURE_H
