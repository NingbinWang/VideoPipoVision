MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
LINK_SHARE=y
# logger
MODULE_SUB_DIR += logger
MODULE_SUB_INC += logger

MODULE_SUB_INC += inc/netlink
MODULE_SUB_INC += src/sys/include
MODULE_SUB_INC += src/sys/medium/fdisk

MODULE_SUB_DIR += src/sys/
MODULE_SUB_DIR += src/sys/cache
MODULE_SUB_DIR += src/sys/cmd
MODULE_SUB_DIR += src/sys/compress
MODULE_SUB_DIR += src/sys/cond
MODULE_SUB_DIR += src/sys/mailbox
MODULE_SUB_DIR += src/sys/medium
MODULE_SUB_DIR += src/sys/medium/fdisk
MODULE_SUB_DIR += src/sys/mem
MODULE_SUB_DIR += src/sys/mutex
MODULE_SUB_DIR += src/sys/net
MODULE_SUB_DIR += src/sys/netlink
MODULE_SUB_DIR += src/sys/netlink/generic
MODULE_SUB_DIR += src/sys/netlink/uevent
MODULE_SUB_DIR += src/sys/queue
MODULE_SUB_DIR += src/sys/semphore
MODULE_SUB_DIR += src/sys/share_memory
MODULE_SUB_DIR += src/sys/socket
MODULE_SUB_DIR += src/sys/thread
MODULE_SUB_DIR += src/sys/time
MODULE_SUB_DIR += src/sys/timer
MODULE_SUB_DIR += src/sys/utils

MODULE_NAME = Utils

include $(MODULE_CUR_PATH)/Thirdparty/utils.mk
include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk