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

/*
bool RKrga::img_fillrectangle_task(IMAGE_T* background,OSD_T* osddata) 
{
  int ret = 0;
  int dst_width, dst_height, dst_format;
  int dst_buf_size;
  char *dst_buf;
  int dst_dma_fd;
  rga_buffer_t dst = {};
  im_rect dst_rect[2] = {};
  rga_buffer_handle_t dst_handle;
  im_job_handle_t job_handle;

  dst_width = 1280;
  dst_height = 720;
  dst_format = RK_FORMAT_RGBA_8888;

  dst_buf_size = dst_width * dst_height * get_bpp_from_format(dst_format);

  
// Allocate dma_buf within 4G from dma32_heap,
// return dma_fd and virtual address.
// ColorFill can only be used on buffers within 4G.

  ret = dma_buf_alloc(DMA_HEAP_DMA32_UNCACHE_PATCH, dst_buf_size, &dst_dma_fd, (void **)&dst_buf);
  if (ret < 0) {
      printf("alloc dma32_heap buffer failed!\n");
      return -1;
  }

  memset(dst_buf, 0x33, dst_buf_size);


   // Import the allocated dma_fd into RGA by calling
  // importbuffer_fd, and use the returned buffer_handle
  // to call RGA to process the image.
  //
  dst_handle = importbuffer_fd(dst_dma_fd, dst_buf_size);
  if (dst_handle == 0) {
      printf("import dma_fd error!\n");
      ret = -1;
      goto free_buf;
  }

  dst = wrapbuffer_handle(dst_handle, dst_width, dst_height, dst_format);


  //Fills multiple rectangular areas on the dst image with the specified color.
  //       dst_image
  //    --------------
 //   | -------    |
  //    | |   --|--  |
  //    | ----|-- |  |
  //    |     -----  |
 //     --------------
 

  //Create a job handle.
  job_handle = imbeginJob();
  if (job_handle <= 0) {
      printf("job begin failed![%d], %s\n", job_handle, imStrError());
      goto release_buffer;
  }

 //Add a task to fill a filled rectangle.
  dst_rect[0] = {0, 0, 300, 200};

  ret = imcheck({}, dst, {}, dst_rect[0], IM_COLOR_FILL);
  if (IM_STATUS_NOERROR != ret) {
      printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
      goto cancel_job;
  }

  ret = imrectangleTask(job_handle, dst, dst_rect[0], 0xff00ff00, -1);
  if (ret == IM_STATUS_SUCCESS) {
      printf("%s job[%d] add fill task success!\n", LOG_TAG, job_handle);
  } else {
      printf("%s job[%d] add fill task failed, %s\n", LOG_TAG, job_handle, imStrError((IM_STATUS)ret));
      goto cancel_job;
  }

 // Add a task to fill the rectangle border.
  dst_rect[0] = {100, 100, 300, 200};

  ret = imcheck({}, dst, {}, dst_rect[0], IM_COLOR_FILL);
  if (IM_STATUS_NOERROR != ret) {
      printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
      goto cancel_job;
  }

  ret = imrectangleTask(job_handle, dst, dst_rect[0], 0xffff0000, 4);
  if (ret == IM_STATUS_SUCCESS) {
      printf("%s job[%d] add fill task success!\n", LOG_TAG, job_handle);
  } else {
      printf("%s job[%d] add fill task failed, %s\n", LOG_TAG, job_handle, imStrError((IM_STATUS)ret));
      goto cancel_job;
  }

 // Submit and wait for the job to complete.
  ret = imendJob(job_handle);
  if (ret == IM_STATUS_SUCCESS) {
      printf("%s job[%d] running success!\n", LOG_TAG, job_handle);
  } else {
      printf("%s job[%d] running failed, %s\n", LOG_TAG, job_handle, imStrError((IM_STATUS)ret));
      goto release_buffer;
  }

  printf("output [0x%x, 0x%x, 0x%x, 0x%x]\n", dst_buf[0], dst_buf[1], dst_buf[2], dst_buf[3]);
  write_image_to_file(dst_buf, LOCAL_FILE_PATH, dst_width, dst_height, dst_format, 0);

cancel_job:
  imcancelJob(job_handle);

release_buffer:
  if (dst_handle > 0)
      releasebuffer_handle(dst_handle);

free_buf:
  dma_buf_free(dst_buf_size, &dst_dma_fd, dst_buf);

  return 0;
}
*/
bool RKrga::img_fillrectangle_array_virt(IMAGE_T* background,OSD_RECT_T osdrect[],int osdrectnum)
{
  IM_STATUS ret = IM_STATUS_SUCCESS;
  rga_buffer_t bg;
  im_rect bg_rect[osdrectnum];
  unsigned int color = 0;
  int thickness = 0;
  memset(&bg, 0, sizeof(bg));
  memset(&bg_rect, 0, sizeof(bg_rect));
  bg = wrapbuffer_virtualaddr((void *)background->virt_addr, background->width, background->height, background->format,background->width_stride,background->height_stride);
  for (int i = 0; i < osdrectnum; i++) {
    bg_rect[i].x = osdrect[i].x_pos;
    bg_rect[i].y = osdrect[i].y_pos;
    bg_rect[i].width = osdrect[i].witdh;
    bg_rect[i].height = osdrect[i].height;
    ret = imcheck({}, bg, {}, bg_rect[i], IM_COLOR_FILL);
    if (IM_STATUS_NOERROR != ret)
    {
        LOG_ERROR("%d, rect num:%d check error! %s \n", __LINE__, i, imStrError((IM_STATUS)ret));
       return false;
   }
  }
  if(osdrect[0].color == 0)
  {
    color = 0xff00ff00;
  }else{
    color = osdrect[0].color;
  }
  if(osdrect[0].thickness == 0)
  {
    thickness = 2;
  }else{
    thickness = osdrect[0].thickness;
  }
  ret = imrectangleArray(bg, bg_rect, osdrectnum, color,thickness);
  if (IM_STATUS_SUCCESS != ret)
  {
    LOG_ERROR("%d, imrectangle error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
    return false;
  }
  return true;
}

bool RKrga::img_fillrectangle_virt(IMAGE_T* background,OSD_RECT_T osdrect)
{
  IM_STATUS ret = IM_STATUS_SUCCESS;
  rga_buffer_t bg;
  im_rect bg_rect;
  unsigned int color = 0;
  int thickness = 0;
  memset(&bg, 0, sizeof(bg));
  memset(&bg_rect, 0, sizeof(bg_rect));
  bg = wrapbuffer_virtualaddr((void *)background->virt_addr, background->width, background->height, background->format,background->width_stride,background->height_stride);
  bg_rect.x = osdrect.x_pos;
  bg_rect.y = osdrect.y_pos;
  bg_rect.width = osdrect.witdh;
  bg_rect.height = osdrect.height;

  ret = imcheck({}, bg, {}, bg_rect, IM_COLOR_FILL);
  if (IM_STATUS_NOERROR != ret)
  {
    LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
    return false;
  }
  if(osdrect.color == 0)
  {
    color = 0xff00ff00;
  }else{
    color = osdrect.color;
  }
  if(osdrect.thickness == 0)
  {
    thickness = 2;
  }else{
    thickness = osdrect.thickness;
  }
  ret = imrectangle(bg, bg_rect, color, thickness);
  if (IM_STATUS_SUCCESS != ret)
  {
    LOG_ERROR("%d, imrectangle error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
    return false;
  }
  return true;
}



bool RKrga::img_osd_virt(IMAGE_T* background,OSD_T* osddata) 
{
     IM_STATUS ret = IM_STATUS_SUCCESS;
     // 定义源和目标图像参数
     rga_buffer_t osd;
     rga_buffer_t bg;
     im_rect bg_rect;
     im_osd_t osd_config;

     memset(&bg_rect, 0, sizeof(bg_rect));
     memset(&osd_config, 0, sizeof(osd_config));
     memset(&osd, 0, sizeof(osd));
     memset(&bg, 0, sizeof(bg));

     osd_config.osd_mode = IM_OSD_MODE_STATISTICS | IM_OSD_MODE_AUTO_INVERT;

     osd_config.block_parm.width_mode = IM_OSD_BLOCK_MODE_NORMAL;
     osd_config.block_parm.width = osddata->block_witdh;
     osd_config.block_parm.block_count = osddata->block_count;
     osd_config.block_parm.background_config = IM_OSD_BACKGROUND_DEFAULT_BRIGHT;
     osd_config.block_parm.direction = IM_OSD_MODE_VERTICAL;
     osd_config.block_parm.color_mode = IM_OSD_COLOR_PIXEL;
 
     osd_config.invert_config.invert_channel = IM_OSD_INVERT_CHANNEL_COLOR;
     osd_config.invert_config.flags_mode = IM_OSD_FLAGS_EXTERNAL;
     osd_config.invert_config.invert_flags = 0x000000000000002a;
     osd_config.invert_config.flags_index = 1;
     osd_config.invert_config.threash = 40;
     osd_config.invert_config.invert_mode = IM_OSD_INVERT_USE_SWAP;

 
     bg = wrapbuffer_virtualaddr((void *)background->virt_addr, background->width, background->height, background->format,background->width_stride,background->height_stride);
     osd = wrapbuffer_virtualaddr((void *)osddata->osdimg.virt_addr, osddata->osdimg.width, osddata->osdimg.height, osddata->osdimg.format,osddata->osdimg.width_stride,osddata->osdimg.height_stride);
     bg_rect.x = osddata->x_pos;
     bg_rect.y = osddata->y_pos;
     bg_rect.width = osddata->osdimg.width;
     bg_rect.height = osddata->osdimg.height;
 
   /*
     * Overlay multiple blocks on the background image and guide the color of
     * the blocks according to the external inversion flag.
        ----     ---------------------    ---------------------
        |  |     |                   |    | ----              |
        ----     |                   |    | |  |              |
        |  |     |                   |    | ----              |
        ----  +  |                   | => | |  |              |
        |  |     |                   |    | ----              |
        ----     |                   |    | |  |              |
                 |                   |    | ----              |
                 ---------------------    ---------------------

     */
    ret = imcheck(osd, bg, {}, bg_rect);
    if (IM_STATUS_NOERROR != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    ret = imosd(osd, bg, bg_rect, &osd_config);
    if (IM_STATUS_SUCCESS != ret)
    {
      LOG_ERROR("%d, imosd error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    if(osddata->osdimg.virt_addr != nullptr)
    {
       free(osddata->osdimg.virt_addr);
    }
    return true;
}

bool RKrga::img_resize_virt(IMAGE_T *srcimg,IMAGE_T *dstimg)
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
    ret = imcheck(src, dst, src_rect, dst_rect);
    if (IM_STATUS_NOERROR != ret)
    {
      LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    ret = imresize(src, dst);
    if (IM_STATUS_SUCCESS != ret)
    {
      LOG_ERROR("%d, imresize error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
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
    dst = wrapbuffer_fd(dstimg->fd, dstimg->width, dstimg->height, dstimg->format,dstimg->width_stride,dstimg->height_stride);
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
      LOG_ERROR("%d, imresize error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
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
      LOG_ERROR("%d, imcopy error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
      return false;
    }
    return true;
}

bool RKrga::img_cvtcolor_virt(IMAGE_T *srcimg,IMAGE_T *dstimg)
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