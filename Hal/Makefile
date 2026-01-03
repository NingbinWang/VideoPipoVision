MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Config/inc
MODULE_SUB_INC += inc/Base
MODULE_SUB_INC += $(MODULE_CUR_PATH)/../Utils/inc
#src
MODULE_SUB_DIR += src/Base
MODULE_SUB_DIR += src/System



ifneq (,$(findstring y,$(TARGET_LVGL)))
MODULE_SUB_INC += inc/PageManager
MODULE_SUB_INC += inc/ResourceManager
MODULE_SUB_INC += inc/Notification
MODULE_SUB_DIR += src/PageManager
MODULE_SUB_DIR += src/ResourceManager
MODULE_SUB_DIR += src/Notification
endif


MODULE_NAME = Hal
LINK_SHARE=y
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk