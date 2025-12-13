#include "MediaPriv.h"
#include "MediaDec.h"
int MediaDecInit(const DEC_STATUS_T* status)
{
    return 0;
}


int MediaDecConvertBGRA8888(IMAGE_FRAME_T* srcimg,IMAGE_FRAME_T *outimg)
{
    srcimg->format = RK_FORMAT_YCbCr_420_SP;
    RKrga * Rkrga = Media_GetRkrga();
    outimg->format = RK_FORMAT_BGRA_8888;
    Rkrga->img_cvtcolor_virt((IMAGE_T *)srcimg,(IMAGE_T *)outimg);
    return 0;
}

int MediaDecReSize(IMAGE_FRAME_T* srcimg,IMAGE_FRAME_T *outimg)
{
    srcimg->format = RK_FORMAT_YCbCr_420_SP;
    RKrga * Rkrga = Media_GetRkrga();
    outimg->format = RK_FORMAT_YCbCr_420_SP;
    Rkrga->img_resize_virt((IMAGE_T *)srcimg,(IMAGE_T *)outimg);
    return 0;
}

