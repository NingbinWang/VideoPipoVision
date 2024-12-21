MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
MPP=mpp
LINK_SHARE=y
#外部的一些头文件
MODULE_SUB_INC += $(CURDIR)/../Media/inc
MODULE_SUB_INC += $(CURDIR)/../Utils/inc
ifeq ($(TARGET_MEDIA),$(MPP))
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/mpp/include
MODULE_SUB_INC += $(CURDIR)/../Libs/$(SOC)/rga/include
endif
ifneq (,$(findstring y,$(TARGET_OPENCV)))
MODULE_SUB_INC += $(CURDIR)/../Libs/opencv/include
OPENCV_LIB_PATH = $(CURDIR)/../Libs/opencv/
LD_C_FLAGS += -L$(OPENCV_LIB_PATH) 
LD_CPP_FLAGS += -L$(OPENCV_LIB_PATH) 
endif

MODULE_NAME = App
MODULE_CUR_PATH = $(CURDIR)
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk
