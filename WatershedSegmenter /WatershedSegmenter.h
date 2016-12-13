#ifndef WATERSHEDSEGMENTER_H
#define WATERSHEDSEGMENTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class WatershedSegmenter {

  private:
      //用来表示标记（图）
      cv::Mat markers;

  public:
      WatershedSegmenter();
      ~WatershedSegmenter();
    //设置标记图
    void setMarkers(const cv::Mat& markerImage) {
        //watershed()的输入参数必须为一个32位有符号的标记，所以要先进行转换
        markerImage.convertTo(markers,CV_32S);
      }
    //执行watershed（）
    cv::Mat process(const cv::Mat &image) {
        // Apply watershed
        cv::watershed(image,markers);

        return markers;
      }

    // 以图像形式返回结果
    cv::Mat getSegmentation() {
        cv::Mat tmp;
        // 从32S到8U（0-255）会进行饱和运算，所以像素高于255的一律复制为255
        markers.convertTo(tmp,CV_8U);//

        return tmp;
      }

    // 以图像形式返回分水岭（我理解的是分割线）
    cv::Mat getWatersheds() {
        cv::Mat tmp;
        //在设置标记图像，即执行setMarkers（）后，边缘的像素会被赋值为-1，其他的用正整数表示
        //下面的这个转换可以让边缘像素变为-1*255+255=0，即黑色，其余的溢出，赋值为255，即白色。
        markers.convertTo(tmp,CV_8U,255,255);
        return tmp;
      }
};

#endif // WATERSHEDSEGMENTER_H
