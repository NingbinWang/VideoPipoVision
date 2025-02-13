#ifndef _RKRGA_H_
#define _RKRGA_H_
#include <dlfcn.h> 
#include "RgaApi.h"
#include "im2d.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
  int width;
  int height;
  int width_stride;
  int height_stride;
  int format;
  char *virt_addr;
  int fd;
} IMAGE_T;


typedef int(* FUNC_RGA_INIT)();
typedef void(* FUNC_RGA_DEINIT)();
typedef int(* FUNC_RGA_BLIT)(rga_info_t *, rga_info_t *, rga_info_t *);

typedef struct _rga_context{
    void *rga_handle;
    FUNC_RGA_INIT init_func;
    FUNC_RGA_DEINIT deinit_func;
    FUNC_RGA_BLIT blit_func;
} rga_context;

//int RGA_init(rga_context* rga_ctx);

#ifdef __cplusplus
}
#endif


class RKrga{
 public:
    RKrga();
    ~RKrga();
    int overlay_osd(IMAGE_T osd_bitmap,IMAGE_T background,int x_pos,int y_pos);
    bool img_resize_virt(IMAGE_T *srcimg,IMAGE_T *dstimg);
    bool img_resize_fd(IMAGE_T *srcimg,IMAGE_T *dstimg);
    bool img_copy_fd(IMAGE_T *srcimg,IMAGE_T *dstimg);
    bool img_imcvtcolor_virt(IMAGE_T *srcimg,IMAGE_T *dstimg);
};





#endif