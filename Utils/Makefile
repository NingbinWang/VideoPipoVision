MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Config/inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Hal/inc/Base

# logger
MODULE_SUB_DIR += src/Logger
#Libs
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Libs/$(SOC)/libdrm/include


MODULE_NAME = Utils
LINK_SHARE=y
include $(MODULE_CUR_PATH)/Thirdparty/utils.mk
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk