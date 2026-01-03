MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_INC = inc
MODULE_SUB_DIR = src
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Config/inc

MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Utils/inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/Base


MODULE_SUB_DIR += src/WiFi
MODULE_SUB_DIR += src/Gsensor
MODULE_SUB_DIR += src/Gps
MODULE_SUB_DIR += src/InputDevices

MODULE_NAME = Hardware
LINK_SHARE=y
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk
