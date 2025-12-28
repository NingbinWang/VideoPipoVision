MODULE_CUR_PATH = $(CURDIR)
MODULE_SUB_DIR = src
MODULE_SUB_INC = inc
LINK_SHARE=y

MODULE_SUB_INC += $(CURDIR)/../Utils/inc
MODULE_SUB_INC += $(CURDIR)/inc/netlink
MODULE_SUB_INC += $(CURDIR)/src/sys/include
MODULE_SUB_INC += $(CURDIR)/src/sys/medium/fdisk

MODULE_SUB_DIR += $(CURDIR)/src/sys/
MODULE_SUB_DIR += $(CURDIR)/src/sys/cache
MODULE_SUB_DIR += $(CURDIR)/src/sys/cmd
MODULE_SUB_DIR += $(CURDIR)/src/sys/compress
MODULE_SUB_DIR += $(CURDIR)/src/sys/cond
MODULE_SUB_DIR += $(CURDIR)/src/sys/mailbox
MODULE_SUB_DIR += $(CURDIR)/src/sys/medium
MODULE_SUB_DIR += $(CURDIR)/src/sys/medium/fdisk
MODULE_SUB_DIR += $(CURDIR)/src/sys/mem
MODULE_SUB_DIR += $(CURDIR)/src/sys/mutex
MODULE_SUB_DIR += $(CURDIR)/src/sys/net
MODULE_SUB_DIR += $(CURDIR)/src/sys/netlink
MODULE_SUB_DIR += $(CURDIR)/src/sys/netlink/generic
MODULE_SUB_DIR += $(CURDIR)/src/sys/netlink/uevent
MODULE_SUB_DIR += $(CURDIR)/src/sys/queue
MODULE_SUB_DIR += $(CURDIR)/src/sys/semphore
MODULE_SUB_DIR += $(CURDIR)/src/sys/share_memory
MODULE_SUB_DIR += $(CURDIR)/src/sys/socket
MODULE_SUB_DIR += $(CURDIR)/src/sys/thread
MODULE_SUB_DIR += $(CURDIR)/src/sys/time
MODULE_SUB_DIR += $(CURDIR)/src/sys/timer
MODULE_SUB_DIR += $(CURDIR)/src/sys/utils

MODULE_NAME = Hal

include $(ROOT_PATH)/Rules/platform.mk
#模块库mk  调试执行程序mk
include $(ROOT_PATH)/Rules/module.mk