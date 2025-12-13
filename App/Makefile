MODULE_SUB_DIR = src
MODULE_SUB_INC = inc

MODULE_SUB_DIR += src/Base
MODULE_SUB_DIR += src/Net/Poller
MODULE_SUB_DIR += src/Net
MODULE_SUB_DIR += src/Media

LINK_SHARE=y
#外部的一些头文件
MODULE_SUB_INC += $(CURDIR)/../Media/inc
MODULE_SUB_INC += $(CURDIR)/../Utils/inc
MODULE_SUB_INC += $(CURDIR)/../Utils/logger
MODULE_SUB_INC += $(CURDIR)/../Utils/Thirdparty/
MODULE_SUB_INC += $(CURDIR)/../Config/
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/libdrm/include
MODULE_SUB_INC += $(CURDIR)/../Hardware/include
ifneq (,$(findstring y,$(TARGET_LVGL)))
MODULE_SUB_INC += inc/PageManager
MODULE_SUB_INC += inc/ResourceManager
MODULE_SUB_INC += inc/Pages
MODULE_SUB_INC += inc/Notification

MODULE_SUB_INC += inc/Utils/Filters
MODULE_SUB_INC += inc/Utils/Lvallocator
MODULE_SUB_INC += inc/Utils/Lvext
MODULE_SUB_INC += inc/Utils/LvThread

MODULE_SUB_DIR += src/PageManager
MODULE_SUB_DIR += src/Pages
MODULE_SUB_DIR += src/Pages/StartUp
MODULE_SUB_DIR += src/Pages/StatusBar
MODULE_SUB_DIR += src/Pages/Template
MODULE_SUB_DIR += src/Pages/VideoStream

MODULE_SUB_DIR += src/ResourceManager
MODULE_SUB_DIR += src/Notification
MODULE_SUB_DIR += src/Utils/Lvext
MODULE_SUB_DIR += src/Resources
MODULE_SUB_DIR += src/Resources/Font
MODULE_SUB_DIR += src/Resources/Image
MODULE_SUB_DIR += src/Utils/LvThread
LD_C_FLAGS += -DLVGL
LD_CPP_FLAGS += -DLVGL
endif


ifeq ($(TARGET_MEDIA),mpp)
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/mpp/include
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/rga/include
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/x264/include
LD_C_FLAGS += -DMEDIARKMPP
LD_CPP_FLAGS += -DMEDIARKMPP
#LD_C_FLAGS += -DMEDIARKAI
#LD_CPP_FLAGS += -DMEDIARKAI
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

ifneq (,$(findstring y,$(TARGET_ALSA)))
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/alsa/include
ALSA_LIB_PATH = $(CURDIR)/../Libs/$(SOC)/alsa/lib
LD_C_FLAGS += -L$(ALSA_LIB_PATH) 
LD_CPP_FLAGS += -L$(ALSA_LIB_PATH)
endif

MODULE_NAME = App
MODULE_CUR_PATH = $(CURDIR)
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk
