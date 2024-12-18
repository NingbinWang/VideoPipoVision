MODULE_BUILD_CUR_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MODULE_BUILD_PATH := $(patsubst %/, %, $(dir $(MODULE_BUILD_CUR_PATH)))

PALTFORM_PATH = $(MODULE_BUILD_PATH)/linux.mk

-include $(PALTFORM_PATH)