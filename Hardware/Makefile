MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_INC = include
MODULE_SUB_INC += $(CURDIR)/../Utils/inc
MODULE_SUB_INC += $(CURDIR)/../Hal/inc


MODULE_SUB_DIR =  WiFi
MODULE_SUB_DIR += Gsensor
MODULE_SUB_DIR += Gps
MODULE_SUB_DIR += InputDevices

LINK_SHARE=y
#外部的一些头文件

MODULE_NAME = Hardware
MODULE_CUR_PATH = $(CURDIR)
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk
