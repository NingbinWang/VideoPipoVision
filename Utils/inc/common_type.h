#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifndef TYPE_BOOLEAN
#define TYPE_BOOLEAN
typedef int         BOOLEAN;
#endif

#ifndef TYPE_UINT64
#define TYPE_UINT64
typedef unsigned long long UINT64;
#endif

#ifndef TYPE_INT64
#define TYPE_INT64
typedef signed long long INT64;
#endif

#ifndef TYPE_UINT32
#define TYPE_UINT32
typedef unsigned long   UINT32;
#endif

#ifndef TYPE_INT32
#define TYPE_INT32
typedef signed  long  INT32;
#endif

#ifndef TYPE_UINT16
#define TYPE_UINT16
typedef  unsigned short    UINT16;
#endif

#ifndef TYPE_INT16
#define TYPE_INT16
typedef signed short    INT16;
#endif


#ifndef TYPE_UINT8
#define TYPE_UINT8
typedef unsigned char     UINT8;
#endif

#ifndef TYPE_INT8
#define TYPE_INT8
typedef signed char     INT8;
#endif

#ifndef TYPE_UINT
#define TYPE_UINT
#ifndef UINT
typedef unsigned int    UINT;
#endif
#endif

#ifndef TYPE_INT
#define TYPE_INT
typedef int             INT;
#endif



#ifndef TYPE_CHAR
#define TYPE_CHAR
typedef char     CHAR;
#endif




#ifndef TYPE_FLOAT64
#define TYPE_FLOAT64
typedef double   FLOAT64;
#endif

#ifndef TYPE_FLOAT32
#define TYPE_FLOAT32
typedef float   FLOAT32;
#endif

#ifndef TYPE_DOUBLE
#define TYPE_DOUBLE
typedef double   DOUBLE;
#endif

#ifndef TYPE_FLOAT
#define TYPE_FLOAT
typedef float   FLOAT;
#endif

#ifndef TYPE_VOID
#define TYPE_VOID
#define VOID    void
#endif

#ifndef TYPE_BOOL
#define TYPE_BOOL
typedef bool BOOL;
#endif

#ifndef TYPE_LONG
#define TYPE_LONG
typedef long    LONG;
#endif

#ifndef TYPE_ULONG
#define TYPE_ULONG
typedef unsigned long ULONG;
#endif

#ifndef TYPE_TRUE
#define TYPE_TRUE
#define TRUE true
#endif

#ifndef TYPE_FALSE
#define TYPE_FALSE
#define FALSE false
#endif

#ifndef STATUS
#define STATUS    int
#endif

#ifndef MIN
#define MIN(a, b)    ((a) > (b) ? (b) : (a))
#endif/* MIN*/

#ifndef MAX
#define MAX(a,b)  (((a)>(b))?(a):(b))
#endif/* MAX*/

#define SIZE_1KB                           0x00000400
#define SIZE_2KB                           0x00000800
#define SIZE_4KB                           0x00001000
#define SIZE_8KB                           0x00002000
#define SIZE_16KB                          0x00004000
#define SIZE_32KB                          0x00008000
#define SIZE_64KB                          0x00010000
#define SIZE_128KB                         0x00020000
#define SIZE_256KB                         0x00040000
#define SIZE_512KB                         0x00080000

#define SIZE_1MB                           0x00100000
#define SIZE_2MB                           0x00200000
#define SIZE_3MB                           0x00300000
#define SIZE_4MB                           0x00400000
#define SIZE_5MB                           0x00500000
#define SIZE_6MB                           0x00600000
#define SIZE_8MB                           0x00800000
#define SIZE_10MB                          0x00A00000
#define SIZE_12MB                          0x00C00000
#define SIZE_16MB                          0x01000000
#define SIZE_18MB                          0x01200000
#define SIZE_20MB                          0x01400000
#define SIZE_22MB                          0x01600000
#define SIZE_24MB                          0x01800000
#define SIZE_30MB                          0x01E00000
#define SIZE_32MB                          0x02000000
#define SIZE_64MB                          0x04000000
#define SIZE_128MB                         0x08000000
#define SIZE_256MB                         0x10000000
#define SIZE_512MB                         0x20000000

#define SIZE_1GB                           0x40000000
#define SIZE_2GB                           0x80000000

