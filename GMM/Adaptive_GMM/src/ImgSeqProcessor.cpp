#include "ImgSeqProcessor.h"

ImgSeqProcessor::ImgSeqProcessor(string _DirPath):DirPath(_DirPath),rectDirCount(0),n_frame(0)
{
    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
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
    n_frame+=1;
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

string itos(int l)
{
    ostringstream os;
    os<<l;
    string result;
    istringstream is(os.str());
    is>>result;
    return result;

}

void ImgSeqProcessor::saveBoundingRect(const Rect& r, const int r_num)
{
    if(0 == r_num)
    {
        curr_frameDir = m_DB.GetDirPath()+"/"+itos(rectDirCount);
        rectDirCount+=1;
        std::cout<<curr_frameDir<<std::endl;
        m_DB.CreateDirectory(curr_frameDir);
    }
    std::string subframe_path = curr_frameDir+"/"+itos(r_num)+".png";
    imwrite(subframe_path, Mat(frame, r));
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
    // 清空上一帧boundingRect
    v_boundingRect.clear();
    // 轮廓表示为一个矩形
    int t_nrect = 0; //记录满足尺寸要求的矩形数量
    for(int i=0;i<contoursPoint.size();i++)
    {
//        Rect r = boundingRect(Mat(*it));
        Rect r = boundingRect(Mat(contoursPoint[i]));
//        v_boundingRect.push_back(r);
        if(!(r.width<7||r.height<7))
            saveBoundingRect(r, t_nrect);
            rectangle(outArr, r, Scalar(255), 1);
            t_nrect+=1;
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
        pMOG2->apply(frame, fgMaskMOG2);
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
        //medianBlur(fgMaskMOG2, fgMaskMOG2, 9);//中值滤波160309
        //erode(fgMaskMOG2, fgMaskMOG2, NULL);
        //dilate(fgMaskMOG2, fgMaskMOG2,NULL);
        imshow("FG Mask MOG 2", fgMaskMOG2);
        if ((char)keyboard != 's')
        {
            imwrite("src.jpg", frame);
            IplImage imgTmp = fgMaskMOG2;
            IplImage *fgMaskMOG2_ip = cvCloneImage(&imgTmp);
            cvSaveImage("dst.jpg", fgMaskMOG2_ip);
            cvReleaseImage(&fgMaskMOG2_ip);
        }
        //get the input from the keyboard
        keyboard = waitKey(1);
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
        pMOG2->apply(frame, fgMaskMOG2);

        //show the current frame and the fg masks
        imshow("Frame", frame);
//        medianBlur(fgMaskMOG2, fgMaskMOG2, 9);//中值滤波160309
//        erode(fgMaskMOG2, fgMaskMOG2, cv::Mat());
        Dilate(fgMaskMOG2, 0,1);
        Dilate(fgMaskMOG2, 0,2);
        Erode(fgMaskMOG2, 1,2);
        Erode(fgMaskMOG2, 1,1);

        FindContours(fgMaskMOG2, frame);

//        imshow("FG Mask MOG 2", fgMaskMOG2);
//        imshow("tracing img", frame);
        //save frame as jpg
//        if ((char)keyboard != 's')
//        {
//            imwrite("src.jpg", frame);
//        }
        //get the input from the keyboard
//        keyboard = waitKey( 1000 );
        keyboard = waitKey(1);
    }
}
