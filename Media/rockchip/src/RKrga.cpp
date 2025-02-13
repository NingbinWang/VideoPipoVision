#include "RKrga.h"
#include "Logger.h"
void* rgactx;
RKrga::RKrga()
{
    // 初始化 RGA
    RgaInit(&rgactx);
    //c_RkRgaInit();
}

RKrga::~RKrga() 
{
	   // 释放资源
       RgaDeInit(&rgactx);
}

int RKrga::overlay_osd(IMAGE_T osd_bitmap,IMAGE_T background,int x_pos,int y_pos) {
     int ret = -1;
     // 定义源和目标图像参数
     rga_info_t src, dst;

     // 源图像（OSD 图层，带 Alpha 通道）
     memset(&src, 0, sizeof(rga_info_t));

     src.fd = osd_bitmap.fd; // 若为物理内存则填写 fd
     src.virAddr = osd_bitmap.virt_addr; // OSD 图像数据指针（如 RGBA8888 格式）
     src.mmuFlag = 1;
     src.rotation = 0;
     ret = rga_set_rect(&src.rect, 
         0, 0,             // 源图像起始坐标
         osd_bitmap.width,        // OSD 宽度
         osd_bitmap.height,       // OSD 高度
         osd_bitmap.width_stride,    // 源图像 stride（RGBA8888 为 width*4）
         osd_bitmap.height_stride,
         osd_bitmap.format);

    if(ret != 0)
    {
        LOG_ERROR("osd_bitmap failed:%d \n",ret);
        return ret;
    }
 
     // 目标图像（背景视频帧）
     memset(&dst, 0, sizeof(rga_info_t));
     dst.fd = background.fd;
     dst.virAddr = background.virt_addr; // 目标缓冲区地址（如 NV12 格式）
     dst.mmuFlag = 1;
     ret = rga_set_rect(&dst.rect,
         x_pos, y_pos,     // OSD 叠加位置
         background.width, 
         background.height,
         background.width_stride, 
         background.height_stride, 
         background.format);  // 根据实际格式调整
    if (ret != 0) {
        LOG_ERROR("RGA blit failed: %d\n", ret);
        return ret;
    }
 
     // 设置混合模式（Alpha 混合）
     src.blend = 0x405;    // 常用混合模式：SRC_OVER + 全局 Alpha
     src.rgba5551_alpha0 = 255; // Alpha 值（0~255）
 
     // 执行 RGA 操作
     ret = RgaBlit(&src, &dst, NULL);
     if (ret != 0) {
         LOG_ERROR("RGA blit failed: %d\n", ret);
         return ret;
     }
     return ret;
}

bool RKrga::img_resize_virt(IMAGE_T *srcimg,IMAGE_T *dstimg)
{
    IM_STATUS ret = IM_STATUS_SUCCESS;
    // init rga context
    rga_buffer_t src;
    rga_buffer_t dst;
    im_rect src_rect;
    im_rect dst_rect;
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    src = wrapbuffer_virtualaddr((void *)srcimg->virt_addr, srcimg->width, srcimg->height, srcimg->format,srcimg->width_stride,srcimg->height_stride);
    dst = wrapbuffer_virtualaddr((void *)dstimg->virt_addr, dstimg->width, dstimg->height, dstimg->format,dstimg->width_stride,dstimg->height_stride);
    ret = imcheck(src, dst, src_rect, dst_rect);
    if (IM_STATUS_NOERROR != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    ret = imresize(src, dst);
    if (IM_STATUS_SUCCESS != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    return true;
}


bool RKrga::img_resize_fd(IMAGE_T *srcimg,IMAGE_T *dstimg)
{
    IM_STATUS ret = IM_STATUS_SUCCESS;
    // init rga context
    rga_buffer_t src;
    rga_buffer_t dst;
    im_rect src_rect;
    im_rect dst_rect;
    memset(&src_rect, 0, sizeof(src_rect));
    memset(&dst_rect, 0, sizeof(dst_rect));
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    src = wrapbuffer_fd(srcimg->fd, srcimg->width, srcimg->height, srcimg->format,srcimg->width_stride,srcimg->height_stride);
    dst = wrapbuffer_fd(dstimg->fd, dstimg->width, dstimg->height, dstimg->format);
    ret = imcheck(src, dst, src_rect, dst_rect);
    if (IM_STATUS_NOERROR != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
  //IM_STATUS STATUS = imresize(src, dst);
    ret = imresize(src, dst);
    if (IM_STATUS_SUCCESS != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    return true;
}


bool RKrga::img_copy_fd(IMAGE_T *srcimg,IMAGE_T *dstimg)
{
    IM_STATUS ret = IM_STATUS_SUCCESS;
    // init rga context
    rga_buffer_t src;
    rga_buffer_t dst;
    im_rect src_rect;
    im_rect dst_rect;
    memset(&src_rect, 0, sizeof(src_rect));
    memset(&dst_rect, 0, sizeof(dst_rect));
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    // Copy To another buffer avoid to modify mpp decoder buffer
    src = wrapbuffer_fd(srcimg->fd, srcimg->width, srcimg->height, srcimg->format,srcimg->width_stride,srcimg->height_stride);
    dst = wrapbuffer_fd(dstimg->fd, dstimg->width, dstimg->height, dstimg->format,dstimg->width_stride,dstimg->height_stride);
    ret = imcheck(src, dst, src_rect, dst_rect);
    if (IM_STATUS_NOERROR != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }

    ret = imcopy(src, dst);
    if (IM_STATUS_SUCCESS != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    return true;
}

bool RKrga::img_imcvtcolor_virt(IMAGE_T *srcimg,IMAGE_T *dstimg)
{
     IM_STATUS ret = IM_STATUS_SUCCESS;
  // init rga context
     rga_buffer_t src;
     rga_buffer_t dst;
     im_rect src_rect;
     im_rect dst_rect;
     memset(&src_rect, 0, sizeof(src_rect));
     memset(&dst_rect, 0, sizeof(dst_rect));
     memset(&src, 0, sizeof(src));
     memset(&dst, 0, sizeof(dst));
     src = wrapbuffer_virtualaddr((void *)srcimg->virt_addr, srcimg->width, srcimg->height, srcimg->format,srcimg->width_stride,srcimg->height_stride);
     dst = wrapbuffer_virtualaddr((void *)dstimg->virt_addr, dstimg->width, dstimg->height, dstimg->format,dstimg->width_stride,dstimg->height_stride);
     //检查
     ret = imcheck(src, dst, src_rect, dst_rect);
     if (IM_STATUS_NOERROR != ret)
     {
       LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
       return false;
     } 
     // 执行格式转换
     ret = imcvtcolor(src, dst,src.format, dst.format);
     if (IM_STATUS_SUCCESS != ret)
     {
       LOG_ERROR("%d, imcvtcolor error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
       return false;
     }
     return true;
}