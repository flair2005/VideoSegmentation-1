#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
//傅里叶正变换
void fft2(IplImage *src, IplImage *dst){
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

void centralization(IplImage *src){
//    //Rearrange the quadrants of Fourier image so that the origin is at the image center
//     double tmp13, tmp24;
//     int nRow = src->height;
//     int nCol = src->width;
//     int cy = nRow/2; // image center
//     int cx = nCol/2;
//    //CV_IMAGE_ELEM为OpenCV定义的宏，用来读取图像的像素值，这一部分就是进行中心变换
//     for( int j = 0; j < cy; j++ ){
//        for(int i = 0; i < cx; i++ ){
//            //中心化，将整体份成四块进行对角交换
//            tmp13 = CV_IMAGE_ELEM( src, double, j, i);
//            CV_IMAGE_ELEM( src, double, j, i) = CV_IMAGE_ELEM(src, double, j+cy, i+cx);
//            CV_IMAGE_ELEM( src, double, j+cy, i+cx) = tmp13;
//
//            tmp24 = CV_IMAGE_ELEM( src, double, j, i+cx);
//            CV_IMAGE_ELEM( src, double, j, i+cx) = CV_IMAGE_ELEM( src, double, j+cy, i);
//            CV_IMAGE_ELEM( src, double, j+cy, i) = tmp24;
//        }
//    }

    //Rearrange the quadrants of Fourier image so that the origin is at the image center
     int nRow = src->height;
     int nCol = src->width;
     int cy = nRow/2; // image center
     int cx = nCol/2;

    //CV_IMAGE_ELEM为OpenCV定义的宏，用来读取图像的像素值，这一部分就是进行中心变换
     for( int j = 0; j < cy; j++ ){
        for(int i = 0; i < cx; i++ ){
            //中心化，将整体份成四块进行对角交换
            CvScalar tmp13, tmp24;
            tmp13 = cvGet2D(src, j, i);
            cvSet2D(src, j, i, cvGet2D(src,j+cy, i+cx));
            cvSet2D(src,j+cy, i+cx, tmp13);

            tmp24 = cvGet2D(src, j, i+cx);
            cvSet2D(src, j, i+cx, cvGet2D(src,j+cy, i));
            cvSet2D( src,j+cy, i, tmp24);
        }
    }
}

void fft2shift(IplImage *src, IplImage *dst){
    IplImage *image_Re = 0, *image_Im = 0;
    double scale, shift;
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
    cvPow( image_Re, image_Re, 0.5 );
    //对数变换以增强灰度级细节(这种变换使以窄带低灰度输入图像值映射
    //一宽带输出值，具体可见冈萨雷斯数字图像处理p62)
    // Compute log(1 + Mag);
    cvAddS( image_Re, cvScalar(1.0), image_Re ); // 1 + Mag
    cvLog( image_Re, image_Re ); // log(1 + Mag)
    //Rearrange the quadrants of Fourier image so that the origin is at the image center
    centralization(image_Re);
    //归一化处理将矩阵的元素值归一为[0,255]
    //[(f(x,y)-minVal)/(maxVal-minVal)]*255
    double minVal = 0, maxVal = 0;
    // Localize minimum and maximum values
    cvMinMaxLoc( image_Re, &minVal, &maxVal );
    // Normalize image (0 - 255) to be observed as an u8 image
    scale = 255/(maxVal - minVal);
    shift = -minVal * scale;
    cvConvertScale(image_Re, dst, scale, shift);
    cvReleaseImage(&image_Re);
    cvReleaseImage(&image_Im);
}

int main(){
    IplImage *src;  //源图像
    IplImage *Fourier;   //傅里叶系数
    IplImage *dst ;
    IplImage *ImageRe;
    IplImage *ImageIm;
    IplImage *Image;
//    IplImage *cImage
    IplImage *ImageDst;
    double m,M;
    double scale;
    double shift;
    src = cvLoadImage("test.png",0);   //加载源图像，第二个参数表示将输入的图片转为单信道
    Fourier = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,2);
    dst = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,2);
    ImageRe = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
    ImageIm = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
//    cImage = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
    Image = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
    ImageDst = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
    fft2(src,Fourier);                 //傅里叶变换
    fft2shift(Fourier, Image);        //中心化
    centralization(Image);
    cvDFT(Fourier,dst,CV_DXT_INV_SCALE);//实现傅里叶逆变换，并对结果进行缩放
    cvSplit(dst,ImageRe,ImageIm,0,0);
    cvNamedWindow("input",0);
    cvShowImage("input",src);
    //对数组每个元素平方并存储在第二个参数中
    cvPow(ImageRe,ImageRe,2);
    cvPow(ImageIm,ImageIm,2);
    cvAdd(ImageRe,ImageIm,ImageRe,NULL);
    cvPow(ImageRe,ImageRe,0.5);
    cvMinMaxLoc(ImageRe,&m,&M,NULL,NULL);
    scale = 255/(M - m);
    shift = -m * scale;
    //将shift加在ImageRe各元素按比例缩放的结果上，存储为ImageDst
    cvConvertScale(ImageRe,ImageDst,scale,shift);
    cvNamedWindow("spectrogram",0);
    cvShowImage("spectrogram",Image);
    cvNamedWindow("output",0);
    cvShowImage("output",ImageDst);
    //释放图像
    cvWaitKey(0);
    cvReleaseImage(&src);
    cvReleaseImage(&Image);
    cvReleaseImage(&ImageIm);
    cvReleaseImage(&ImageRe);
    cvReleaseImage(&Fourier);
    cvReleaseImage(&dst);
    cvReleaseImage(&ImageDst);
    return 0;
}
