#include "ImgSeqProcessor.h"

int main(int argc, char* argv[])
{
    ImgSeqProcessor imp("/home/yangzheng/testData/ucsd/vidf1_33_001.y");
    imp.processImgSeq();

    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}
