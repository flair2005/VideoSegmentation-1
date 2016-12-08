#include "ImgSeqProcessor.h"

ImgSeqProcessor::ImgSeqProcessor(string _DirPath):DirPath(_DirPath)
{
    //create Background Subtractor objects
    pKNN = createBackgroundSubtractorKNN(); //KNN approach
    m_DB.SetDirPath(_DirPath);
    m_DB.GetAllFilesName();
}
bool ImgSeqProcessor::ReadNextFrame(Mat &frame)
{
    string framePath = m_DB.GetNextFramePath();
    if(framePath.empty())
    {
        return false;
    }
    frame = imread(framePath, CV_8UC1);
    if(!frame.data)
        return false;
    return true;
}

bool ImgSeqProcessor::Dilate(Mat &workMat, int erosion_elem, int erosion_size)
{
    if(!workMat.data)
        return false;
    Mat tmpMat;
    int erosion_type;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
    dilate(workMat, tmpMat, element);
    workMat=tmpMat;
    return true;
}

bool ImgSeqProcessor::Erode(Mat &workMat, int erosion_elem, int erosion_size)
{
    if(!workMat.data)
        return false;
    Mat tmpMat;
    int erosion_type;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
    erode(workMat, tmpMat, element);
    workMat=tmpMat;
    return true;
}

bool ImgSeqProcessor::FindContours(Mat inArr, Mat &outArr)
{
    // find
    Mat tmpMat;
    vector<vector<Point> > contoursPoint;
    inArr.convertTo(tmpMat, CV_8UC1);
    findContours(tmpMat,contoursPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    // draw
    Mat result(tmpMat.size(),CV_8U,Scalar(0));
    drawContours(result,contoursPoint,-1,Scalar(255),1);
    // 轮廓表示为一个矩形
    for(int i=0;i<contoursPoint.size();i++)
    {
//        Rect r = boundingRect(Mat(*it));
        Rect r = boundingRect(Mat(contoursPoint[i]));
        if(!(r.width<7||r.height<7))
            rectangle(outArr, r, Scalar(255), 1);
    }
    return true;
}

void ImgSeqProcessor::processVideo() {
    //create the capture object
    VideoCapture capture(DirPath);
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << DirPath << endl;
        exit(EXIT_FAILURE);
    }
    int keyboard;
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        //update the background model
        pKNN->apply(frame, fgMaskKNN);
        //get the frame number and write it on the current frame
        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);//绘制方块


        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));//方块中写入帧数



        //show the current frame and the fg masks
        imshow("Frame", frame);
        //medianBlur(fgMaskKNN, fgMaskKNN, 9);//中值滤波160309
        //erode(fgMaskKNN, fgMaskKNN, NULL);
        //dilate(fgMaskKNN, fgMaskKNN,NULL);
        imshow("FG Mask MOG 2", fgMaskKNN);
        if ((char)keyboard != 's')
        {
            imwrite("src.jpg", frame);
            IplImage imgTmp = fgMaskKNN;
            IplImage *fgMaskKNN_ip = cvCloneImage(&imgTmp);
            cvSaveImage("dst.jpg", fgMaskKNN_ip);
            cvReleaseImage(&fgMaskKNN_ip);
        }
        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    //delete capture object
    capture.release();
}

void ImgSeqProcessor::processImgSeq() {
    //read input data. ESC or 'q' for quitting
    int keyboard;
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!ReadNextFrame(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        //update the background model
        pKNN->apply(frame, fgMaskKNN);

        //show the current frame and the fg masks
        imshow("Frame", frame);

        Dilate(fgMaskKNN, 0,1);
        Dilate(fgMaskKNN, 0,4);
//        Erode(fgMaskKNN, 1,2);
//        Erode(fgMaskKNN, 1,1);

        FindContours(fgMaskKNN, frame);

        imshow("FG Mask KNN", fgMaskKNN);
        imshow("tracing img", frame);
        //save frame as jpg
//        if ((char)keyboard != 's')
//        {
//            imwrite("src.jpg", frame);
//        }
        //get the input from the keyboard
        keyboard = waitKey( 1000 );
    }
}
