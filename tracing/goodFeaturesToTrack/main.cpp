#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>

using namespace cv;
using namespace std;


class Tracker {
    vector<Point2f> trackedFeatures;
    Mat             prevGray;
public:
    bool            freshStart;
    Mat_<float>     rigidTransform;

    Tracker():freshStart(true) {
        rigidTransform = Mat::eye(3,3,CV_32FC1); //affine 2x3 in a 3x3 matrix
    }

    void processImage(Mat& img) {
        Mat gray; cvtColor(img,gray,CV_BGR2GRAY);
        vector<Point2f> corners;
        if(trackedFeatures.size() < 200) {
            goodFeaturesToTrack(gray,corners,300,0.01,10);//寻找强角点
            cout << "found " << corners.size() << " features\n";
            for (int i = 0; i < corners.size(); ++i) {
                //保存强角点
                trackedFeatures.push_back(corners[i]);
            }
        }

        if(!prevGray.empty()) {
            vector<uchar> status; vector<float> errors;
            //计算前后两帧稀疏光流，基于强角点。trackedFeatures中角点的新位置位置放入corners
            calcOpticalFlowPyrLK(prevGray,gray,trackedFeatures,corners,status,errors,Size(10,10));

            if(countNonZero(status) < status.size() * 0.8) {
                cout << "cataclysmic error \n";
                rigidTransform = Mat::eye(3,3,CV_32FC1);
                trackedFeatures.clear();
                prevGray.release();
                freshStart = true;
                return;
            } else
                freshStart = false;
            //计算trackedFeatures->corners放射变换的变换矩阵
            Mat_<float> newRigidTransform = estimateRigidTransform(trackedFeatures,corners,false);
            Mat_<float> nrt33 = Mat_<float>::eye(3,3);
            newRigidTransform.copyTo(nrt33.rowRange(0,2));
            rigidTransform *= nrt33;

            trackedFeatures.clear();
            for (int i = 0; i < status.size(); ++i) {
                if(status[i]) {
                    trackedFeatures.push_back(corners[i]);
                }
            }
        }

        for (int i = 0; i < trackedFeatures.size(); ++i) {
            //在原图中绘制角点
            circle(img,trackedFeatures[i],3,Scalar(0,0,255),CV_FILLED);
        }

        gray.copyTo(prevGray);
    }
};


int main() {
    VideoCapture vc;

    vc.open("/home/yangzheng/testData/test.mp4");
    Mat frame,orig,orig_warped,tmp;

    Tracker tracker;

    while(vc.isOpened()) {
        vc >> frame;
        if(frame.empty()) break;
        frame.copyTo(orig);

        tracker.processImage(orig);
        //矩阵求逆
        Mat invTrans = tracker.rigidTransform.inv(DECOMP_SVD);
        warpAffine(orig,orig_warped,invTrans.rowRange(0,2),Size());

        imshow("orig",orig);
        imshow("orig_warped",orig_warped);

        int key = waitKey(0);
        if(key == 'q') {
            break;
        }
    }
    vc.release();

}
