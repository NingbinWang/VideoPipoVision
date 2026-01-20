#include "SysObject.h"

#define OBJ_CONTAINER_LIST_INIT(c) {&(object_container[c].list), &(object_container[c].list)}

#ifdef OBJECT_STATIC_ALLOC
#define DEFINED_OBJECT_POOL(name, type, depth) \
	static unsigned long name##_bitmap[BITS_TO_LONGS(depth)]; \
    static type name##_objects[depth]; 
DEFINED_OBJECT_POOL (thread, SysThread_t, CONFIG_THREAD_POOL_DEPTH);
DEFINED_OBJECT_POOL (mutex, SysMutex_t, CONFIG_MUTEX_POOL_DEPTH);
DEFINED_OBJECT_POOL (timer, SysTimer_t, CONFIG_TIMER_POOL_DEPTH);
DEFINED_OBJECT_POOL (mqueue, SysMqueue_t, CONFIG_MQUEUE_POOL_DEPTH);
#endif

SYSOBJECT_POOL_T object_container[SYSOBJECT_CLASS_UNKNOWN] = {
    [SYSOBJECT_CLASS_THREAD] = {
        .lock       = 0,
        .flag       = 0,
        .elem_size  = sizeof (SysThread_t),
#ifdef OBJECT_STATIC_ALLOC      
        .bitmap  = thread_bitmap,
        .objects = thread_objects,
        .depth   = CONFIG_THREAD_POOL_DEPTH,    
#else       
        .list    = OBJ_CONTAINER_LIST_INIT(SYSOBJECT_CLASS_THREAD),
#endif      
    },
    [SYSOBJECT_CLASS_MUTEX] = {
        .lock       = 0,
        .flag       = SYSOBJECT_POOL_FLAG_NOLOCK,
        .elem_size  = sizeof (SysMutex_t),
#ifdef OBJECT_STATIC_ALLOC                      
        .bitmap  = mutex_bitmap,
        .objects = mutex_objects,
        .depth   = CONFIG_MUTEX_POOL_DEPTH,
#else       
        .list    = OBJ_CONTAINER_LIST_INIT(SYSOBJECT_CLASS_MUTEX),             
#endif      
    }, 
    [SYSOBJECT_CLASS_TIMER] = {
        .lock       = 0,
        .flag       = 0,
        .elem_size  = sizeof (SysTimer_t),
#ifdef OBJECT_STATIC_ALLOC                                              
        .bitmap  = timer_bitmap,
        .objects = timer_objects,
        .depth   = CONFIG_TIMER_POOL_DEPTH,
#else 
        .list    = OBJ_CONTAINER_LIST_INIT(SYSOBJECT_CLASS_TIMER),                                     
#endif
    },  
    [SYSOBJECT_CLASS_MQUEUE] = {
        .lock       = 0,
        .flag       = 0,
        .elem_size  = sizeof (SysMqueue_t),
#ifdef OBJECT_STATIC_ALLOC                                              
        .bitmap  = mqueue_bitmap,
        .objects = mqueue_objects,
        .depth   = CONFIG_MQUEUE_POOL_DEPTH,
#else 
        .list    = OBJ_CONTAINER_LIST_INIT(SYSOBJECT_CLASS_MQUEUE),                                     
#endif
    },

};



