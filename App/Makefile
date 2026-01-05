MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Config/inc

#App Com
MODULE_SUB_INC += inc/Rtsp
MODULE_SUB_INC += inc/Media
MODULE_SUB_DIR += src/Rtsp
MODULE_SUB_DIR += src/Media
MODULE_SUB_INC += inc/Record
MODULE_SUB_INC += inc/Replay
MODULE_SUB_DIR += src/Record
MODULE_SUB_DIR += src/Replay
MODULE_SUB_INC += inc/WebSocket
MODULE_SUB_DIR += src/WebSocket
#Config
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Config/inc
#Hal部分
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/Base
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/Base/Poller
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/System
ifneq (,$(findstring y,$(TARGET_LVGL)))
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/Notification
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/PageManager
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/ResourceManager
endif

#Hardware
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hardware/inc

#Media
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Media/inc

#Utils
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Utils/inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Utils/Thirdparty/
#外部的一些头文件
MODULE_SUB_INC += inc/Utils/Filters

#LVGL
ifneq (,$(findstring y,$(TARGET_LVGL)))
#Libs
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/libdrm/include
#page
MODULE_SUB_INC += inc/Pages
MODULE_SUB_INC += inc/Notify
MODULE_SUB_INC += inc/Utils/Lvallocator
MODULE_SUB_INC += inc/Utils/Lvext
MODULE_SUB_INC += inc/Utils/LvThread
#notify
MODULE_SUB_DIR += src/Notify
#pages
MODULE_SUB_DIR += src/Pages
MODULE_SUB_DIR += src/Pages/StartUp
MODULE_SUB_DIR += src/Pages/StatusBar
MODULE_SUB_DIR += src/Pages/Template
MODULE_SUB_DIR += src/Pages/VideoStream
#resource
MODULE_SUB_DIR += src/Resources
MODULE_SUB_DIR += src/Resources/Font
MODULE_SUB_DIR += src/Resources/Image
#utils
MODULE_SUB_DIR += src/Utils/Lvext
MODULE_SUB_DIR += src/Utils/LvThread
LD_C_FLAGS += -DUSE_LVGL
LD_CPP_FLAGS += -DUSE_LVGL
endif

#App use Ai
ifneq (,$(findstring y,$(TARGET_AI)))
LD_C_FLAGS += -DUSE_AI
LD_CPP_FLAGS += -DUSE_AI
MODULE_SUB_INC += inc/AI
MODULE_SUB_DIR += src/AI
endif

#App use mpp
ifeq ($(TARGET_MEDIA),mpp)
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/mpp/include
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/rga/include
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/x264/include
LD_C_FLAGS += -DMEDIARKMPP
LD_CPP_FLAGS += -DMEDIARKMPP
endif

#App use opencv
ifneq (,$(findstring y,$(TARGET_OPENCV)))
MODULE_SUB_INC += $(CURDIR)/../Libs/opencv/include
OPENCV_LIB_PATH = $(CURDIR)/../Libs/opencv/lib
LD_C_FLAGS += -L$(OPENCV_LIB_PATH) -DUSE_OPENCV
LD_CPP_FLAGS += -L$(OPENCV_LIB_PATH) -DUSE_OPENCV
endif

#App use ffmpeg
ifneq (,$(findstring y,$(TARGET_FFMPEG)))
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/ffmpeg/include
FFMPEG_LIB_PATH = $(CURDIR)/../Libs/$(SOC)/ffmpeg/lib
LD_C_FLAGS += -L$(FFMPEG_LIB_PATH) 
LD_CPP_FLAGS += -L$(FFMPEG_LIB_PATH)
MODULE_SUB_INC += inc/Media/ffmpeg
MODULE_SUB_DIR += src/Media/ffmpeg
endif 

#App use audio
ifneq (,$(findstring y,$(TARGET_ALSA)))
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/alsa/include
ALSA_LIB_PATH = $(CURDIR)/../Libs/$(SOC)/alsa/lib
LD_C_FLAGS += -L$(ALSA_LIB_PATH) 
LD_CPP_FLAGS += -L$(ALSA_LIB_PATH)
endif

MODULE_NAME = App

LINK_SHARE=y
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk
