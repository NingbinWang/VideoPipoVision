VIDEOPIPOVISION_VERSION = 1.0.0
# common
CURDIR = $(shell pwd)
export ROOT_PATH = $(CURDIR)

#misc
BUILD_COMPLETE_STRING ?= $(shell date "+%a, %d %b %Y %T %z")
UID := $(shell id -u)
GID := $(shell id -g)


OUTPUT_DIR= $(CURDIR)/output


# media view

TARGET_MEDIA = mpp
TARGET_SOC = rk3566
TARGET_OPENCV = y
TARGET_ROCKCHIP = y
export SOC     := $(TARGET_SOC)
export $(TARGET_MEDIA)
export $(TARGET_OPENCV)
export $(TARGET_ROCKCHIP)
# cross host
TARGET_CROSS_HOST = $(ROOT_PATH)/../prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu//bin/aarch64-linux-gnu

export $(TARGET_CROSS_HOST)

# rules
include $(ROOT_PATH)/linux.mk

#path
APP_DIR=$(ROOT_PATH)/App
LIB_DIR=$(ROOT_PATH)/Libs
MEDIA_DIR = $(ROOT_PATH)/Media
UTILS_DIR = $(ROOT_PATH)/Utils



CROSS_COMPILE=$(TARGET_CROSS_HOST)- 

LD_C_FLAGS   +=  -ldl -lm -lpthread -lrt  -std=c99
LD_CPP_FLAGS +=  -ldl -lm -lpthread -lrt  -lstdc++  #C++参数

LIB_VAR = App
LIB_VAR += Media
LIB_VAR += Utils

ifneq (,$(findstring y,$(TARGET_OPENCV)))
LIB_VAR += opencv_calib3d
LIB_VAR += opencv_core
LIB_VAR += opencv_dnn
LIB_VAR += opencv_features2d
LIB_VAR += opencv_flann
LIB_VAR += opencv_highgui
LIB_VAR += opencv_imgcodecs
LIB_VAR += opencv_imgproc
LIB_VAR += opencv_ml
LIB_VAR += opencv_objdetect
LIB_VAR += opencv_photo
LIB_VAR += opencv_stitching
LIB_VAR += opencv_video
LIB_VAR += opencv_videoio
endif
ifneq (,$(findstring y,$(TARGET_ROCKCHIP)))
LIB_VAR += rga
LIB_VAR += rockchip_mpp
LIB_VAR += rockchip_vpu
LIB_VAR += x264
LIB_VAR += avcodec
LIB_VAR += avdevice
LIB_VAR += avfilter
LIB_VAR += avformat
LIB_VAR += avutil
LIB_VAR += postproc
LIB_VAR += swresample
LIB_VAR += swscale
endif

PROJECT_LIB_VAR += $(LIB_VAR)
ifneq ($(PROJECT_LIB_VAR), "")
PROJECTLIB_VAR += $(patsubst %,-l%,$(PROJECT_LIB_VAR))
endif

INC_PATH = $(APP_DIR)/inc
INC_PATH += $(LIB_DIR)/$(TARGET_SOC)/mpp/include
INC_PATH += $(LIB_DIR)/$(TARGET_SOC)/rga/include
ifneq (,$(findstring y,$(TARGET_OPENCV)))
INC_PATH += $(LIB_DIR)/opencv/include
endif
INC_PATH += $(UTILS_DIR)/inc
INC_PATH += $(UTILS_DIR)/logger


PROJECT_INC_PATH += $(INC_PATH)

ifneq ($(PROJECT_INC_PATH), "")
PROJECT_INC_DIRS = $(shell find $(PROJECT_INC_PATH) -maxdepth 0 -type d)
PROJECTINC_PATH += $(patsubst %,-I%,$(PROJECT_INC_DIRS))
endif
BUILD_ALL = Utils
BUILD_ALL += Media
BUILD_ALL += App


.PHONY: App App_Clean  Media  Media_Clean Utils Utils_Clean

all: $(BUILD_ALL)
	$(CXX) -o $(OUTPUT_DIR)/VideoVision  $(PROJECTINC_PATH) -L $(OUTPUT_DIR)/lib/ $(PROJECTLIB_VAR) $(LD_CPP_FLAGS) main.cpp
	$(RM)  *.o -rf
	$(ECHO) "Finish generating images at $(BUILD_COMPLETE_STRING)"
	

checkenv:
	@if [ ! -e $(OUTPUT_DIR)/lib ]; then \
		mkdir $(OUTPUT_DIR); \
		mkdir $(OUTPUT_DIR)/lib; \
	fi
	if [ y = $(TARGET_OPENCV) ]; then \
		find $(LIB_DIR)/opencv -name "*.so*" |xargs -i cp {} $(OUTPUT_DIR)/lib/; \
	fi
	find $(LIB_DIR)/$(SOC) -name "*.so*" |xargs -i cp {} $(OUTPUT_DIR)/lib/


Utils: checkenv
	@$(ECHO) "##### Build utils ####"
	make -C $(UTILS_DIR)
	@if [ -f $(UTILS_DIR)/Lib/libUtils.so ]; then \
		cp $(UTILS_DIR)/Lib/libUtils.so $(OUTPUT_DIR)/lib/; \
	fi

Utils_Clean:
	@$(ECHO) "##### Build utils clean ####"
	make -C $(UTILS_DIR) clean


App: checkenv
	@$(ECHO) "##### Build app ####"
	make -C $(APP_DIR)
	@if [ -f $(APP_DIR)/Lib/libApp.so ]; then \
		cp $(APP_DIR)/Lib/libApp.so $(OUTPUT_DIR)/lib/; \
	fi

App_Clean:
	@$(ECHO) "##### Build app clean ####"
	make -C $(APP_DIR) clean

Media: checkenv
	@$(ECHO) "##### Build Media ####"
	@make -C $(MEDIA_DIR) TARGET_MEDIA=$(TARGET_MEDIA)
	@if [ -f $(MEDIA_DIR)/Lib/libMedia.so ]; then \
		cp $(MEDIA_DIR)/Lib/libMedia.so $(OUTPUT_DIR)/lib/; \
	fi

Media_Clean: 
	@$(ECHO) "##### Build media clean ####"
	make -C $(MEDIA_DIR) clean

clean: App_Clean Media_Clean Utils_Clean
	@$(RM)  *.o -rf
	@$(ECHO) "RM  $(OUTPUT_DIR)"
	@$(RM)  $(OUTPUT_DIR)

