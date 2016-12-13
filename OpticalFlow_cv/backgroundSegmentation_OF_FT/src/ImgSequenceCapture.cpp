#include "ImgSequenceCapture.h"

ImgSequenceCapture::ImgSequenceCapture()
{
    //ctor
}

ImgSequenceCapture::ImgSequenceCapture(std::string DirPath)
{
    m_DirPath = DirPath;
    m_dbrowser.SetDirPath(DirPath);
    m_dbrowser.GetAllFilesName();
}

ImgSequenceCapture::~ImgSequenceCapture()
{
    //dtor
}

bool ImgSequenceCapture::GetNextFrame()
{
    std::string t_Str;
    if(m_dbrowser.GetNextFramePath(&t_Str))
    {
        m_currentFrame = imread(t_Str);
        cvtColor(m_currentFrame, m_currentFrameGray, CV_BGR2GRAY);
        return true;
    }
    return false;
}

bool ImgSequenceCapture::CaculateOpticalFlow()
{
    if( m_previousFrameGray.data )
    {
        calcOpticalFlowFarneback(m_previousFrameGray, m_currentFrameGray, m_opticalFlow, 0.5, 3, 15, 3, 5, 1.2, 0);
        std::swap(m_previousFrameGray, m_currentFrameGray);
        return true;
    }
    else
    {
        std::swap(m_previousFrameGray, m_currentFrameGray);
        return false;
    }
}

bool ImgSequenceCapture::CaculateModOfOFlow(int _type, float alpha, float beta)
{
    if(!m_opticalFlow.data)
    {
        return false;
    }
    Mat tmpMat(m_opticalFlow.rows, m_opticalFlow.cols, CV_32FC1);
    m_modOptFlow.create(m_opticalFlow.rows, m_opticalFlow.cols, CV_32FC1);

    for(int i=0 ; i<m_opticalFlow.rows ; i++)
        for(int j=0;j<m_opticalFlow.cols;j++)
        {
            Vec2f flow_at_point = m_opticalFlow.at<Vec2f>(i, j);
            //std::cout<<flow_at_point<<std::endl;
            float t_modOf = ::sqrt(::pow(flow_at_point[0],2)+::pow(flow_at_point[1],2));
            switch(_type)
            {
                case OFMOD_SHRESHOLD:
                {
                    //设置阈值
                    if(t_modOf<=alpha &&t_modOf>=beta)
                        m_modOptFlow.at<float>(i, j) = 1;
                    else
                        m_modOptFlow.at<float>(i, j) = 0;
                }break;
                case OFMOD_NONSHRESHOLD:
                {
                    //不设阈值直接求模
                    m_modOptFlow.at<float>(i, j) = t_modOf;
                }
                default:break;
            }
        }
    //std::cout<<tmpMat<<std::endl;
    //normalize(tmpMat, m_modOptFlow, 255, 0, NORM_MINMAX);
    return true;
}

bool ImgSequenceCapture::ConvertToMunsellColorSystem()
{
    if(!m_opticalFlow.data)
    {
        return false;
    }
    motionToColor(m_opticalFlow, m_MunsellColorOFlow);
    return true;
}

bool ImgSequenceCapture::CaculateEdge(int _type)
{
    switch(_type)
    {
        case EDGE_SOBEL:Sobel(m_currentFrameGray,m_edge,m_currentFrameGray.depth(),2,2);break;
        case EDGE_CANNY:Canny(m_currentFrameGray,m_edge,50,150,3); break;
        case EDGE_LAPLACIAN:Laplacian(m_currentFrameGray,m_edge,m_currentFrameGray.depth());break;
        default:break;
    }

    if(!m_edge.data)
        return false;
    return true;
}

bool  ImgSequenceCapture::CaculateOFlowByEdge(int _type)
{
    if(!CaculateEdge(_type))
        return false;
//    Mat gray;
//    cvtColor(m_edge, gray,CV_RGB2GRAY);
    if( m_previousFrameGray.data )
    {
        calcOpticalFlowFarneback(m_previousFrameGray, m_edge, m_opticalFlow, 0.5, 3, 15, 3, 5, 1.2, 0);
    }
    //“交换”两个参数的数据
    std::swap(m_previousFrameGray, m_edge);
    return true;
}

bool ImgSequenceCapture::FillInternalContours(double dAreaThre)
{
    if(!m_modOptFlow.data)
        return false;
    Mat tmpMat;
    m_modOptFlow.convertTo(tmpMat, CV_8UC1);
    fillHole(tmpMat, m_FilledmodOFLow);
    return true;
}

bool ImgSequenceCapture::EdgeOverlapping()
{
    if(!m_edge.data)
        return false;
//    std::cout<<m_edge<<std::endl;
    for(int i=0;i<m_currentFrameGray.rows;i++)
        for(int j=0;j<m_currentFrameGray.cols;j++)
        {
            if(m_edge.at<uchar>(i, j)!=0)
                m_currentFrameGray.at<uchar>(i, j) = 0;
//            m_currentFrameGray.at<uchar>(i, j) = (uchar)(m_edge.at<uchar>(i, j)!=0?0:m_currentFrameGray.at<uchar>(i, j));
//            m_currentFrameGray.at<uchar>(i, j) = (uchar)(0);
        }
        return true;
}

bool ImgSequenceCapture::Erode(int erosion_elem, int erosion_size)
{
    if(!m_modOptFlow.data)
        return false;
    Mat tmpMat;
    int erosion_type;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
    erode(m_modOptFlow, tmpMat, element);
    m_modOptFlow=tmpMat;
    return true;
}

bool ImgSequenceCapture::Dilate(int erosion_elem, int erosion_size)
{
    if(!m_modOptFlow.data)
        return false;
    Mat tmpMat;
    int erosion_type;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
    dilate(m_modOptFlow, tmpMat, element);
    m_modOptFlow=tmpMat;
    return true;
}

bool ImgSequenceCapture::DFT()
{
    if(!m_currentFrame.data)
        return false;
    return ft.DFT(m_currentFrame);
}

bool ImgSequenceCapture::FindContours()
{
    // find
    Mat tmpMat;
    m_modOptFlow.convertTo(tmpMat, CV_8UC1);
    findContours(tmpMat,contoursPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    // draw
    Mat result(tmpMat.size(),CV_8U,Scalar(0));
    drawContours(result,contoursPoint,-1,Scalar(255),1);
    m_ContoursMap = result;
    // 轮廓表示为一个矩形
//    for(vec_point::iterator it=contoursPoint.begin();it!=contoursPoint.begin();it++)
    m_previousFrameGrayShow = m_previousFrameGray.clone();
    for(int i=0;i<contoursPoint.size();i++)
    {
//        Rect r = boundingRect(Mat(*it));
        Rect r = boundingRect(Mat(contoursPoint[i]));
        rectangle(m_previousFrameGrayShow, r, Scalar(255), 1);
    }
    return true;
}

bool ImgSequenceCapture::VideoSeparation()
{
    if(!GetNextFrame())
        return false;
    if(!CaculateEdge(EDGE_SOBEL))
        return false;
//    if(!EdgeOverlapping())
//        return false;
//    if(!CaculateEdge(EDGE_SOBEL))
//        return false;
    if(CaculateOpticalFlow())
    {
        if(CaculateModOfOFlow(OFMOD_SHRESHOLD,1,0.3))
        {
            if(!Dilate(2))
                return false;
            if(!Erode(1))
                return false;
            if(!Dilate(2))
                return false;
            if(!Erode(1))
                return false;
            if(!FillInternalContours(700))
                return false;
            if(!FindContours())
                return false;
            if(!DFT())
                return false;
        }
        else return false;
    }
    return true;
}


/************************************
*将光流转换为孟塞尔颜色系统（MunsellColor System）方便观察运动状况
*************************************/

/*
*构建MunsellColor System
*/
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

/*
*将光流场转换为梦塞尔颜色系
*/
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

/*
*空洞填充算法，内轮廓填充
*parameter:
*pBinary: 输入二值图像，单通道，位深IPL_DEPTH_8U
*dAreaThre: 面积阈值，当内轮廓面积小于等于dAreaThre时，进行填充
*/
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

Mat _FillInternalContours(Mat Binary, double dAreaThre)
{
    double dConArea;
    IplImage IplBinary;
    Mat tmpMat;
    Binary.convertTo(tmpMat, CV_8UC1);
//    IplBinary = IplImage(tmpMat);
//    IplImage *pBinary  =  &IplBinary;
//    CvSeq *pContour = NULL;
//    CvSeq *pConInner = NULL;
//    CvMemStorage *pStorage = NULL;
    // 执行条件
//    if (pBinary)
//    {
//        // 查找所有轮廓
//        pStorage = cvCreateMemStorage(0);
//        cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
//        // 填充所有轮廓
//        cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
//        // 外轮廓循环
//        for (; pContour != NULL; pContour = pContour->h_next)
//        {
//            // 内轮廓循环
//            for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
//            {
//                // 内轮廓面积
//                dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ));
//                if (dConArea <= dAreaThre)
//                {
//                    cvDrawContours(pBinary, pConInner, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, CV_FILLED, 8, cvPoint(0, 0));
//                }
//            }
//        }
//        cvReleaseMemStorage(&pStorage);
//        pStorage = NULL;
//        tmpMat = cvarrToMat(pBinary);
//    }
//    tmpMat = cvarrToMat(pBinary);
    return tmpMat;
}
