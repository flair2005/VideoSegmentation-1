#include "FFT_segmentation.h"

FFT_segmentation::FFT_segmentation()
{
    //ctor
}

FFT_segmentation::~FFT_segmentation()
{
    //dtor
}

/*傅里叶正变换*/
void FFT_segmentation::fft2(IplImage *src, IplImage *dst){
    //实部、虚部
     IplImage *image_Re = 0, *image_Im = 0, *Fourier = 0;
     //   int i, j;
     image_Re = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);  //实部
     //Imaginary part
     image_Im = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);  //虚部
     //2 channels (image_Re, image_Im)
     Fourier = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 2);
     // Real part conversion from u8 to 64f (double)
     cvConvertScale(src, image_Re, 1, 0);
     // Imaginary part (zeros)
     cvZero(image_Im);
     // Join real and imaginary parts and stock them in Fourier image
     cvMerge(image_Re, image_Im, 0, 0, Fourier);
     // Application of the forward Fourier transform
     cvDFT(Fourier, dst, CV_DXT_FORWARD);
     cvReleaseImage(&image_Re);
     cvReleaseImage(&image_Im);
     cvReleaseImage(&Fourier);
}
/*傅利叶反变换*/
void FFT_segmentation::ifft2(IplImage *src, IplImage *dst){
    cvDFT(src,dst,CV_DXT_INV_SCALE);
}
/*中心化*/
void FFT_segmentation::centralization(IplImage *src){
    //Rearrange the quadrants of Fourier image so that the origin is at the image center
     int nRow = src->height;
     int nCol = src->width;
     int cy = nRow/2; // image center
     int cx = nCol/2;
     CvScalar tmp13, tmp24;

    //CV_IMAGE_ELEM为OpenCV定义的宏，用来读取图像的像素值，这一部分就是进行中心变换
     for( int j = 0; j < cy; j++ ){
        for(int i = 0; i < cx; i++ ){
            //中心化，将整体份成四块进行对角交换
            tmp13 = cvGet2D(src, j, i);
            cvSet2D(src, j, i, cvGet2D(src,j+cy, i+cx));
            cvSet2D(src,j+cy, i+cx, tmp13);

            tmp24 = cvGet2D(src, j, i+cx);
            cvSet2D(src, j, i+cx, cvGet2D(src,j+cy, i));
            cvSet2D( src,j+cy, i, tmp24);
        }
    }
}

/*归一化*/
void FFT_segmentation::normalize(IplImage *src, IplImage *dst){
    //归一化处理将矩阵的元素值归一为[0,255]
    //[(f(x,y)-minVal)/(maxVal-minVal)]*255
    double minVal = 0, maxVal = 0;
    // Localize minimum and maximum values
    cvMinMaxLoc( src, &minVal, &maxVal );
    // Normalize image (0 - 255) to be observed as an u8 image
    scale = 255/(maxVal - minVal);
    shift = -minVal * scale;
    cvConvertScale(src, dst, scale, shift);
}

/*计算幅度普*/
void FFT_segmentation::compute_spectrogram(IplImage *src,  IplImage *dst){
    IplImage *image_Re, *image_Im;
    //real part
    image_Re = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
    //Imaginary part
    image_Im = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
    cvSplit( src, image_Re, image_Im, 0, 0 );
    //具体原理见冈萨雷斯数字图像处理p123
    // Compute the magnitude of the spectrum Mag = sqrt(Re^2 + Im^2)
    //计算傅里叶谱
    cvPow( image_Re, image_Re, 2.0);
    cvPow( image_Im, image_Im, 2.0);
    cvAdd( image_Re, image_Im, image_Re);
    cvPow( image_Re, dst, 0.5 );
    cvReleaseImage(image_Re, image_Im);
}
/*截取不同频率的子图像*/
void FFT_segmentation::segmentation(IplImage *src,vec_images *vimgs){
}
/*计算利于显示的幅度普*/
void FFT_segmentation::fft2shift(IplImage *src, IplImage *dst){
    IplImage *work = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
    compute_spectrogram(src,work);
    //对数变换以增强灰度级细节(这种变换使以窄带低灰度输入图像值映射
    //一宽带输出值，具体可见冈萨雷斯数字图像处理p62)
    // Compute log(1 + Mag);
    cvAddS( work, cvScalar(1.0), work ); // 1 + Mag
    cvLog( work, work ); // log(1 + Mag)
    double scale, shift;
    //Rearrange the quadrants of Fourier image so that the origin is at the image center
    centralization(work);
    normalize(work,dst);

    cvReleaseImage(&work);
}
