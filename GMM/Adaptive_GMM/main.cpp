#include "ImgSeqProcessor.h"

int main(int argc, char* argv[])
{
    ImgSeqProcessor imp("/home/yangzheng/testData/ucsd/vidf5_33_005.y_back");
    imp.processImgSeq();

    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}
