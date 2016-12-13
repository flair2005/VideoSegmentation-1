#include <opencv2/opencv.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "ImgSequenceCapture.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
//    if(argc<2)
//    {
//        std::cout<<"Parameter error: User should input Directory Path!"
//        return 0;
//    }
    ImgSequenceCapture isc("/home/yangzheng/testData/ucsd/vidf1_33_001.y");
    char t_key;

//    while(isc.GetFrameAndCoculateOFlowByEdge(EDGE_SOBEL)&&t_key!='q')
//    {
//        imshow("orgImage", *(isc.GetCurrentFrame()));
//        if(isc.CaculateModOfOFlow(OFMOD_SHRESHOLD,1,0.25))
//            imshow("modOFlow", *(isc.GetModOptFLow()));
//        imshow("Edge", *(isc.GetPreviousframe()));
//
//        t_key = (char)waitKey(300);
//    }

//    while(isc.GetFrameAndCoculateOFlow(OF_EDGEOVERLAPPING)&&t_key!='q')
//    {
//        imshow("orgImage", *(isc.GetPreviousframeGray()));
//        if(isc.CaculateModOfOFlow(OFMOD_SHRESHOLD,1,0.22))
//        {
//            isc.FillInternalContours(3000);
//            imshow("modOFlow", *(isc.GetModOptFLow()));
//            imshow("filled mod OFlow", *(isc.GetFilledModOFLow()));
//        }
////
////        if(isc.ConvertToMunsellColorSystem())
////        {
////            imshow("MunsellColorOFlow", *(isc.GetMunsellColorOptFLow()));
////        }
////
////       if(isc.CaculateEdge(EDGE_SOBEL))
////        imshow("Edge", *(isc.GetEdge()));
//
//        t_key = (char)waitKey(2000);
//    }

        while(isc.VideoSeparation()&&t_key!='q')
        {
            imshow("Org-Image", *(isc.GetPreviousframeGray()));
            imshow("Edge", *(isc.GetEdge()));
            if(isc.GetModOptFLow()->data)
                imshow("Mod OFlow", *(isc.GetModOptFLow()));
            if(isc.GetFilledModOFLow()->data)
                imshow("Filled mod OFlow", *(isc.GetFilledModOFLow()));
            if(isc.GetContours()->data)
            {
                imshow("Contours",*(isc.GetContours()));
                imshow("Contours in org-image", *(isc.GetPreviousframeGrayShow()));
            }
            if(isc.GetFTMagImg()->data)
            {
                imshow("FTMagImg",*(isc.GetFTMagImg()));
            }
            t_key = (char)waitKey(2000);
        }
}
