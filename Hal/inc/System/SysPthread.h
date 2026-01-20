#ifndef _SYSTHREAD_H_
#define _SYSTHREAD_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
/** NORMAL 调度 */
#define SCHED_OTHER  0
/** FIFO 调度, 实时调度 */
#define SCHED_FIFO   1
/** 时间片轮转调度, 实时调度 */
#define SCHED_RR     2

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**
 *  @enum   TASK_PRIORITY_E
 *  @brief  TASK_PRIORITY_0最高优先级 , 下面只是通用定义，各平台独立进行映射
 */
typedef enum  {
    TASK_PRIORITY_0 = 0,    /* 任务优先级 0 */
    TASK_PRIORITY_1 = 1,    /* 任务优先级 1 */
    TASK_PRIORITY_2 = 2,    /* 任务优先级 2 */
    TASK_PRIORITY_3 = 3,    /* 任务优先级 3 */
    TASK_PRIORITY_4 = 4,    /* 任务优先级      4 */
    TASK_PRIORITY_5 = 5,    /* 任务优先级 5 */
    TASK_PRIORITY_6 = 6,    /* 任务优先级 6 */
    TASK_PRIORITY_7 = 7,    /* 任务优先级 7 */
    TASK_PRIORITY_8 = 8,    /* 任务优先级 8 */
    TASK_PRIORITY_9 = 9,    /* 任务优先级 9 */
    TASK_PRIORITY_10 = 10,  /* 任务优先级 10 */
    TASK_PRIORITY_11 = 11,  /* 任务优先级 11 */
    TASK_PRIORITY_12 = 12,  /* 任务优先级 12 */
    TASK_PRIORITY_13 = 13,  /* 任务优先级 13 */
    TASK_PRIORITY_14 = 14,  /* 任务优先级 14 */
    TASK_PRIORITY_15 = 15,  /* 任务优先级 15 */
    TASK_PRIORITY_NUM
}TASK_PRIORITY_E;
    
/**
 *  @struct THREAD_STATE_T
 *  @brief  线程状态描述符，用于诊断/Descriptor of thread state, for diagnosis
 */
typedef struct {
    CHAR   strName[32];          /**< 线程名 */
    UINT32 uStacksize;           /**< 线程栈大小 */
    UINT32 uPriority;            /**< 优先级 */
    UINT64 uRuntime;             /**< 运行时间 */
    UINT64 uTotaltime;           /**< 系统总运行时间 */
    UINT32 uCurTaskRuntime;      /**< 当前运行时间 */
    UINT32 uMaxTaskRuntime;      /**< 最大运行时间 */
    UINT32 uMinTaskRuntime;      /**< 最小运行时间 */
    UINT32 uReserve;             /**< 保留 */
} THREAD_STATE_T;

/** TASK_ID 仅仅一个指针值，具体指向什么各平台内部处理*/
typedef LONG  TASK_ID;

/** FUNCPTR_L 线程指针 */
typedef VOID (*FUNCPTR_L)(VOID*);
/** FUNCPTR_L 线程指针 */
typedef VOID (*FUNCPTR)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*);

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      创建线程
 * @param[in]  pStTid      任务ID句柄指针
 * @param[in]  strTaskName 任务名，包含字符串结束符长度不超过16字节
 * @param[in]  uPriority   任务优先级 TASK_PRIORITY_E
 * @param[in]  uStackSize  创建任务栈大小
 * @param[in]  pFunc       创建任务函数
 * @param[in]  uArgs       参数个数
 * @return     成功返回 0  错误返回 其他
 */
INT32 SysPthread_create(TASK_ID *pStTid, CHAR *strTaskName, UINT32 uPriority, UINT32 uStackSize,
        FUNCPTR pFunc,
        UINT32 uArgs, ...);

/**
 * @brief      等待线程结束
 * @param[in]  stTaskId   任务ID句柄
 * @return     成功返回 0  错误返回 其他
 */
INT32 SysPthread_join(TASK_ID stTaskId);

/**
 * @brief      取消线程并删除相关资源
 * @param[in]  stTaskId   任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_cancel(TASK_ID stTaskId);

/**
 * @brief      取消线程并删除相关资源
 * @return     无
 */
VOID SysPthread_exit(VOID);

/**
 * @brief      验证线程是否存在
 * @param[in]  pStTid     任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_verify(TASK_ID *pStTid);

/**
 * @brief      阻塞线程
 * @param[in]  pStTid        任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_suspend(TASK_ID *pStTid);

/**
 * @brief      获取调用线程的ID
 * @return     成功返回 0  返回线程对象ID
 */
TASK_ID SysPthread_self(VOID); 

/**
 * @brief      恢复阻塞的线程
 * @param[in]  pStTid      任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_reume(TASK_ID *pStTid);

/**
 * @brief      获取当前调用线程线程名信息
 * @param[out] strName     线程任务名称
 * @param[in]  uNameLen    线程名称还清楚大小
 * @return     成功返回0,错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_get_name( CHAR *strName, UINT16 uNameLen);

/**
 * @brief      设置当前调用线程线程名信息
 * @param[in]  strName     线程任务名称
 * @return     成功返回0,错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_set_name( CHAR *strName);

/**
 * @brief      设置线程绑定CPU运行
 * @param[in]  stTaskId    任务句柄
 * @param[in]  uCpuset     线程绑定的CPU子集掩码, 每一位对应一个CPU
 *                         (0x1 对应 cpu0, 0x3 对应 cpu0和cpu1 ...)
 * @return     成功返回0,错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_setaffinity(TASK_ID stTaskId, ULONG uCpuset);

/**
 * @brief      获取线程绑定的CPU
 * @param[in]  stTaskId    任务句柄
 * @param[out] pCpuset     线程绑定的CPU子集掩码, 每一位对应一个CPU
 *                         (0x1 对应 cpu0, 0x3 对应 cpu0和cpu1 ...)
 * @return     成功返回0,错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysPthread_getaffinity(TASK_ID stTaskId, ULONG *pCpuset);

/**
 * @brief  设置任务的优先级/Set a task's priority
 * @param[in]   stTaskId     任务句柄
 * @param[in]   uPriority    优先级 @ref TASK_PRIORITY_E
 * @return      无
 */
VOID SysPthread_set_priority(TASK_ID stTaskId, UINT32 uPriority);

/**
 * @brief       启动bios调度
 * @return      无
 */
VOID SysPthread_schdule(VOID);

/**
 *  @brief      获取要创建的线程对象的数量/
 *              Get number of thread object be created
 *  @return     要创建的线程对象的数量/
 *              Number of thread object be created
 */
INT32 SysPthread_number(VOID);

/**
 *  @brief      完成并释放这个先前分配的任务实例对象/
 *              Finalize and free this previously allocated task instance object
 *  @param[in]  stTaskId  先前创建的任务实例对象的句柄/
 *                        Handle of a previously-created task instance object  
 *  @return     无
 */
VOID SysPthread_delete (TASK_ID stTaskId);

/**
 * @brief       延迟执行当前任务
 * @param[in]   uMs     延迟时间，单位ms
 * @return      无
 */
VOID SysPthread_sleep(UINT32 uMs);

/**
 * @brief       延迟执行当前任务
 * @param[in]   uUs     延迟时间，单位us
 * @return      无
 */
VOID SysPthread_sleep_us(UINT32 uUs);

/**
 * @brief       挂起调度器而不禁用中断。 
 *              在调度器暂停时，不会发生上下文切换。
 * @return      无
 */
VOID SysPthread_suspend_all(VOID);

/**
 * @brief       在暂停调度程序活动后恢复调度程序活动/
 *              Resumes scheduler activity after it was suspended
 * @return      无
 */
VOID SysPthread_resume_all(VOID);

/**
 *  @brief      获取任务tick值/     Get number of task tick
 *  @return     任务的tick值/
 *              Number of task tick
 */
UINT32 SysPthread_tick(VOID);

/*
 * @brief      获取线程TID
 * @return     成功返回线程ID, 错误返回0
 */
UINT32 SysPthread_get_id(VOID);

/**
 * @brief       获取指定线程的状态/
 *              Gets the status of the specified thread
 * @param[in]   stTaskId 先前创建的任务实例对象的句柄/
 * @param[out]  pState   指向线程状态结构体的指针 
 * @return      返回 0 表示成功，返回负值表示失败
 */
INT32 SysPthread_state(TASK_ID stTaskId, THREAD_STATE_T *pState);

/**
 * @brief       获取所有线程的状态
 * @param[out]  pStatus     指向线程状态结构体数组的指针, @ref THREAD_STATE_T 
 * @return      成功返回任务的数量，返回负值表示不成功
 */
INT32 SysPthread_status(THREAD_STATE_T *pStatus);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* SYS_THREAD_H */

