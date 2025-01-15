MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Utils/inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Utils/logger
LINK_SHARE=y
MPP=mpp
ifeq ($(TARGET_MEDIA),$(MPP))
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/mpp/include
MPP_LIB_PATH = $(MODULE_CUR_PATH)/../Libs/$(SOC)/mpp/
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/rga/include
RGA_LIB_PATH = $(MODULE_CUR_PATH)/../Libs/$(SOC)/rga/

MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/ffmpeg/include
FFMPEG_LIB_PATH = $(MODULE_CUR_PATH)/../Libs/$(SOC)/ffmpeg/
#-ldl -lm -lpthread -lrt --std=c99 -D _GNU_SOURCE
WARNING		= -Wall -Wundef -Wsign-compare -Wno-missing-braces -Wstrict-prototypes -Werror
LD_C_FLAGS += -L$(MPP_LIB_PATH)
LD_CPP_FLAGS += $(WARNING) -L$(MPP_LIB_PATH) -L$(RGA_LIB_PATH) -L$(FFMPEG_LIB_PATH)
MODULE_SUB_INC += mpp/inc
MODULE_SUB_DIR += mpp/src
endif

#libv4l2
MODULE_SUB_DIR += libv4l2/src
MODULE_SUB_INC += libv4l2/inc


MODULE_NAME = Media

include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk