#ifndef FFT_SEGMENTATION_H
#define FFT_SEGMENTATION_H

 /// inlcuding files
#include <stdio.h>
#include <vector>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

typedef vector<IplImage*> vec_images;

class FFT_segmentation
{
    public:
        explicit FFT_segmentation();
        virtual ~FFT_segmentation();
        /*傅里叶正变换*/
        void fft2(IplImage *src, IplImage *dst);
        /*傅利叶反变换*/
        void ifft2(IplImage *src, IplImage *dst);
        /*中心化*/
        void centralization(IplImage *src);
        /*归一化*/
        void normalize(IplImage *src, IplImage *dst);
        /*计算幅度普*/
        void compute_spectrogram(IplImage *src,  IplImage *dst);
        /*截取不同频率的子图像*/
        void segmentation(IplImage *src,vec_images *vimgs);
        /*计算利于显示的幅度普*/
        void fft2shift(IplImage *src, IplImage *dst);

    protected:

    private:
};

#endif // FFT_SEGMENTATION_H
