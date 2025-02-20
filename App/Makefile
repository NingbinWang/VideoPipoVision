MODULE_SUB_DIR = src
MODULE_SUB_INC = inc

MODULE_SUB_DIR += src/base
MODULE_SUB_DIR += src/net/poller
MODULE_SUB_DIR += src/net
MODULE_SUB_DIR += src/media

LINK_SHARE=y
#外部的一些头文件
MODULE_SUB_INC += $(CURDIR)/../Media/inc
MODULE_SUB_INC += $(CURDIR)/../Utils/inc
MODULE_SUB_INC += $(CURDIR)/../Utils/logger

ifeq ($(TARGET_MEDIA),mpp)
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/mpp/include
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/rga/include
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/x264/include
LD_C_FLAGS += -DMEDIARKMPP -DMEDIARKAI
LD_CPP_FLAGS += -DMEDIARKMPP -DMEDIARKAI
#LD_C_FLAGS += -DMEDIARKMPP
#LD_CPP_FLAGS += -DMEDIARKMPP
endif

ifneq (,$(findstring y,$(TARGET_OPENCV)))
MODULE_SUB_INC += $(CURDIR)/../Libs/opencv/include
OPENCV_LIB_PATH = $(CURDIR)/../Libs/opencv/lib
LD_C_FLAGS += -L$(OPENCV_LIB_PATH) 
LD_CPP_FLAGS += -L$(OPENCV_LIB_PATH) 
endif

ifneq (,$(findstring y,$(TARGET_FFMPEG)))
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/ffmpeg/include
FFMPEG_LIB_PATH = $(CURDIR)/../Libs/$(SOC)/ffmpeg/lib
LD_C_FLAGS += -L$(FFMPEG_LIB_PATH) 
LD_CPP_FLAGS += -L$(FFMPEG_LIB_PATH)
endif 


MODULE_NAME = App
MODULE_CUR_PATH = $(CURDIR)
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk
