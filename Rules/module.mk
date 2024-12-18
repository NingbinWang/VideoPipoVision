#角色 统一模块编译makefile
#1.基础组件库 不能单独编译成进程,只能编译成库
#2.业务组件库,依赖基础组件库,既可以编译成单独的库,也可以编译成执行程序,
#3.主函数组件库,连接所有库
#模式1库模式
#模式2测试模式
#在子目录Makefile被定义
MODULE_SUB_DIR ?= ERROR
MODULE_CUR_PATH?= ERROR
LINK_SHARE?=ERROR
#--------------------模块参数-------------------------#
#模块名称 输出模块目标
MODULE_NAME ?= $(notdir $(MODULE_CUR_PATH))

ifeq (,$(findstring y,$(LINK_SHARE)))
MODULE_TARGET := lib$(MODULE_NAME).a
else
MODULE_TARGET := lib$(MODULE_NAME).so
endif

MODULE_TEST   := test_$(MODULE_NAME)
#生成.o目录
MODULE_OBJDIR := $(MODULE_CUR_PATH)/obj
#模块头文件目录
MODULE_INC_PATH:= $(MODULE_SUB_INC)
#生成执行文件目录
MODULE_BIN_PATH:= $(MODULE_CUR_PATH)/bin
#生成库目录
MODULE_LIB_PATH:= $(MODULE_CUR_PATH)/Lib/$(TOOL_CHAIN_HOST)

#遍历子目录文件后缀
ifneq ($(MODULE_SUB_DIR),"")
MODULE_SRCS_CPP += $(foreach dir, $(MODULE_SUB_DIR), $(wildcard $(dir)/*.cpp))  
MODULE_SRCS_CC += $(foreach dir, $(MODULE_SUB_DIR), $(wildcard $(dir)/*.cc))  
MODULE_SRCS_C += $(foreach dir, $(MODULE_SUB_DIR), $(wildcard $(dir)/*.c))
INC_PATH += $(patsubst %,-I%,$(MODULE_SUB_DIR))
endif

MOD_INC_PATH += $(MODULE_INC_PATH)

ifneq ($(MOD_INC_PATH), "")
MOD_INC_DIRS = $(shell find $(MOD_INC_PATH) -maxdepth 3 -type d)
INC_PATH += $(patsubst %,-I%,$(MOD_INC_DIRS))
endif

#汇编暂不考虑.asm
MODULE_OBJS_CPP:= $(patsubst %.cpp, %.o, $(MODULE_SRCS_CPP))
MODULE_OBJS_CC := $(patsubst %.cc, %.o,  $(MODULE_SRCS_CC))
MODULE_OBJS_C  := $(patsubst %.c, %.o,  $(MODULE_SRCS_C))

MODULE_CPP_DEPS  := $(MODULE_OBJS_CPP:.o=.d)
MODULE_CPP_DEPS  += $(MODULE_OBJS_CC:.o=.d)
MODULE_DEPS  := $(MODULE_OBJS_C:.o=.d)

#添加库参数路径
#andriod不支持 lm lrt等库
ifneq (,$(findstring android,$(TOOL_CHAIN_HOST)))
LD_C_FLAGS   += --std=c99
else
LD_C_FLAGS   += -ldl -lm -lpthread -lrt 
endif

LD_CPP_FLAGS += -lstdc++  #C++参数
#动态库链接则增加选项LINK_SHARE=y
ifneq (,$(findstring y,$(LINK_SHARE)))
SUPPORT_FPIC=y
endif

#静态库外界有重定向动态库需求可以加上参数 SUPPORT_FPIC=y
ifneq (,$(findstring y,$(SUPPORT_FPIC)))
C_FLAGS += -fPIC
endif

#添加参数CFLAGS

#目标编译  支持1.编译资源库 2.编译测试进程
.PHONY: all clean

all:$(MODULE_TARGET)
#目标库依赖 .c .cpp .cc 
$(MODULE_TARGET) : $(MODULE_OBJS_CPP) $(MODULE_OBJS_CC) $(MODULE_OBJS_C)
	$(Q)@test -d $(MODULE_LIB_PATH) | mkdir -p $(MODULE_LIB_PATH)   			
	$(Q)echo -e "\033[34m CREATE $(notdir $@)\033[0m"
	$(Q)rm -f $(MODULE_LIB_PATH)/$@ -rf
	$(Q)echo -e $@
	$(Q)echo -e $(MODULE_LIB_PATH)/$@
	$(Q)#编译选项带LINK_SHARE=y则为编译动态库
ifeq (,$(findstring y,$(LINK_SHARE)))
	$(Q)$(AR) rcs -o $(MODULE_LIB_PATH)/$@ $(MODULE_OBJDIR)/*
else
	$(Q)$(CC) -shared -fPIC -o $(MODULE_LIB_PATH)/$@ $(MODULE_OBJDIR)/*
endif
	$(Q)-$(RM) -f $(MODULE_OBJDIR)/ -rf

$(MODULE_OBJS_CPP): %.o : %.cpp
	$(Q)@test -d $(MODULE_OBJDIR) | mkdir -p $(MODULE_OBJDIR)
	$(Q)echo -e "\033[35m CREATE $(notdir $@)\033[0m"
	$(Q)$(CXX) -c $(INC_PATH) $(C_FLAGS) $(LD_CPP_FLAGS) $(<) -o $@ 
	$(Q)mv $*.o $(MODULE_OBJDIR)

#cc后缀文件编译.o
$(MODULE_OBJS_CC): %.o : %.cc
	$(Q)@test -d $(MODULE_OBJDIR) | mkdir -p $(MODULE_OBJDIR)
	$(Q)echo -e "\033[35m CREATE $(notdir $@)\033[0m"
	$(Q)$(CXX) -c $(INC_PATH) $(C_FLAGS) $(LD_CPP_FLAGS) $(<) -o $@ 
	$(Q)mv $*.o $(MODULE_OBJDIR)

#c后缀文件编译.o
$(MODULE_OBJS_C): %.o : %.c
	$(Q)@test -d $(MODULE_OBJDIR) | mkdir -p $(MODULE_OBJDIR)
	$(Q)echo -e "\033[35m CREATE $(notdir $@)\033[0m"
	$(Q)$(CC) -c $(INC_PATH) $(C_FLAGS) $(LD_C_FLAGS) $(<) -o $@ 
	$(Q)mv $@ $(MODULE_OBJDIR)
	
ifneq ($(MAKECMDGOALS), clean)
-include $(MODULE_DEPS)
endif

clean:	#资源清空
	$(Q)-$(RM) -f $(MODULE_OBJDIR)/ -rf
	$(Q)-$(RM) -f $(MODULE_OBJS_C) -rf
	$(Q)-$(RM) -f $(MODULE_LIB_PATH) -rf
