#ifndef SOCKET_DEFINE_H
#define SOCKET_DEFINE_H
#include "Common.h"
/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* Define some BSD protocol constants.  */
#define SYS_SOCK_STREAM                     (1)                    /**< TCP Socket */
#define SYS_SOCK_DGRAM                      (2)                    /**< UDP Socket */

 #define SYS_IPPROTO_IP                     (0)                    /**< dummy for IP */
 #define SYS_IPPROTO_ICMP                   (1)                    /**< control message protocol */
 #define SYS_IPPROTO_IGMP                   (2)                    /**< internet group management protocol */
 #define SYS_IPPROTO_GGP                    (3)                    /**< gateway^2 (deprecated) */
 #define SYS_IPPROTO_TCP                    (6)                    /**< tcp */
 #define SYS_IPPROTO_PUP                    (12)                   /**< pup */
 #define SYS_IPPROTO_UDP                    (17)                   /**< user datagram protocol */
 #define SYS_IPPROTO_IDP                    (22)                   /**< xns idp */
 #define SYS_IPPROTO_IPV6                   (41)                   /**< IPv6-in-IPv4 tunnelling */
 
 #define SYS_IPPROTO_ND                     (77)                   /**< UNOFFICIAL net disk proto */
 #define SYS_IPPROTO_RAW                    (255)                  /**< raw IP packet */
 #define SYS_IPPROTO_MAX                    (256)                  /**< max ip proto */

#define SYS_IP_TOS                          (1)                    /**< tos */
#define SYS_IP_TTL                          (2)                    /**< time to live */
#define SYS_IP_HDRINCL                      (3)                    /**< hdincl */
#define SYS_IP_OPTIONS                      (4)                    /**< options */
#define SYS_IP_ROUTER_ALERT                 (5)                    /**< route alert */
#define SYS_IP_RECVOPTS                     (6)                    /**< recv ops */
#define SYS_IP_RETOPTS                      (7)                    /**< retopts */
#define SYS_IP_PKTINFO                      (8)                    /**< pktinfo */
#define SYS_IP_PKTOPTIONS                   (9)                    /**< packet options */
#define SYS_IP_MTU_DISCOVER                 (10)                   /**< mtu discovery */
#define SYS_IP_RECVERR                      (11)                   /**< recv error */
#define SYS_IP_RECVTTL                      (12)                   /**< recv ttl */
#define SYS_IP_RECVTOS                      (13)                   /**< recv tos */
#define SYS_IP_MTU                          (14)                   /**< mtu */
#define SYS_IP_FREEBIND                     (15)                   /**< freebind */
#define SYS_IP_IPSEC_POLICY                 (16)                   /**< ipsec policy */
#define SYS_IP_XFRM_POLICY                  (17)                   /**< xfrm policy */
#define SYS_IP_PASSSEC                      (18)                   /**< passsec */
#define SYS_IP_TRANSPARENT                  (19)                   /**< transparent */
#define SYS_IP_MULTICAST_IF                 (32)                   /**< multicast if */
#define SYS_IP_MULTICAST_TTL                (33)                   /**< multicast ttl */
#define SYS_IP_MULTICAST_LOOP               (34)                   /**< multicast loop */
#define SYS_IP_ADD_MEMBERSHIP               (35)                   /**< add membership */
#define SYS_IP_DROP_MEMBERSHIP              (36)                   /**< drop membership */
#define SYS_IP_UNBLOCK_SOURCE               (37)                   /**< unblock source */
#define SYS_IP_BLOCK_SOURCE                 (38)                   /**< block source */
#define SYS_IP_ADD_SOURCE_MEMBERSHIP        (39)                   /**< add source membership */
#define SYS_IP_DROP_SOURCE_MEMBERSHIP       (40)                   /**< drop source membership */
#define SYS_IP_MSFILTER                     (41)                   /**< msfilter */

/* IPV6 */
#define SYS_IPV6_MULTICAST_IF               (17)                   /**< multicast if */
#define SYS_IPV6_MULTICAST_HOPS             (18)                   /**< multicast ttl */
#define SYS_IPV6_MULTICAST_LOOP             (19)                   /**< multicast loop */
#define SYS_IPV6_ADD_MEMBERSHIP             (20)                   /**< add membership */
#define SYS_IPV6_DROP_MEMBERSHIP            (21)                   /**< drop membership */

#define SYS_TCP_MAXSEG                      (50)                   /**< tcp max seg */

#define SYS_PF_INET                         (2)                    /**< IP protocol family.  */
#define SYS_PF_INET6                        (10)                   /**< IP version 6.  */

#define SYS_AF_INET                         (SYS_PF_INET)          /**< mtu discovery */
#define SYS_AF_INET6                        (SYS_PF_INET6)         /**< mtu discovery */                
#define SYS_SOL_SOCKET                      (1)                    /**< mtu discovery */
#define SYS_SO_REUSEADDR                    (2)                    /**< Allow local address reuse */
#define SYS_SO_ERROR                        (4)                    /**< error */
#define SYS_SO_BROADCAST                    (6)                    /**< broadcast */
#define SYS_SO_SNDBUF                       (7)                    /**< send buffer */
#define SYS_SO_RCVBUF                       (8)                    /**< recv buffer */
#define SYS_SO_KEEPALIVE                    (9)                    /**< kepp alive */
#define SYS_SO_RCVTIMEO                     (20)                   /**< recv timeout */
#define SYS_SO_SNDTIMEO                     (21)                   /**< send timeout */
#define SYS_SO_LINGER                       (22)                   /**< linkger */
#define SYS_SO_NO_CHECK                     (23)                   /**< no check */
#define SYS_SO_BINDTODEVICE                 (25)                   /**< bind device */

#define SYS_F_GETFL                          (3)                   /**< SYS_F_GETFL */
#define SYS_F_SETFL                          (4)                   /**< SYS_F_SETFL */
#define SYS_O_NONBLOCK                       (1)                   /**< SYS_O_NONBLOCK */

#define SYS_TCP_NODELAY   (0x01)                                   /**< SYS_O_NONBLOCK */
#define SYS_TCP_KEEPIDLE                     (4)                   /**< TCP Keep idle */
#define SYS_TCP_KEEPINTVL                    (5)                   /**< TCP Keep intvl */
#define SYS_TCP_KEEPCNT                      (6)                   /**< TCP Keep cnt */

#define SYS_FD_SETSIZE    (1024)                                   /**< SYS_FD_SETSIZE */
#define SYS_FD_SET(n, p)  ((p)->fd_bits[(n)/8u] |=  (1u << ((n) & 7u))) /**< SYS_FD_SET */
#define SYS_FD_CLR(n, p)  ((p)->fd_bits[(n)/8u] &= ~(1u << ((n) & 7u))) /**< SYS_FD_CLR */
#define SYS_FD_ISSET(n,p) ((p)->fd_bits[(n)/8u] &   (1u << ((n) & 7u))) /**< SYS_FD_ISSET */
#define SYS_FD_ZERO(p)    memset((VOID*)(p),0,sizeof(*(p)))               /**< SYS_FD_ZERO */

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**
 *  @enum   SYS_TCP_STATE_E
 *  @brief  tcp状态枚举/tcp state
 */
typedef enum
{
    SYS_TCP_ESTABLISHED = 1,
    SYS_TCP_SYN_SENT,
    SYS_TCP_SYN_RECV,
    SYS_TCP_FIN_WAIT1,
    SYS_TCP_FIN_WAIT2,
    SYS_TCP_TIME_WAIT,
    SYS_TCP_CLOSE,
    SYS_TCP_CLOSE_WAIT,
    SYS_TCP_LAST_ACK,
    SYS_TCP_LISTEN,
    SYS_TCP_CLOSING   /* now a valid state */
} SYS_TCP_STATE_E;

typedef struct sys_in_addr sys_in_addr;

/**
 *  @struct   sys_in_addr
 *  @brief    网络地址结构体/Internet address
 */
struct sys_in_addr
{
    UINT32           s_addr;               /**< Internet address (32 bits).                      */
};

/**
 *  @struct   sys_sockaddr_in
 *  @brief    套接字网络地址结构体/socket Internet address
 */
struct sys_sockaddr_in
{
    UINT16              sin_family;         /**< Internet Protocol (AF_INET).                    */
    UINT16              sin_port;           /**< Address port (16 bits).                         */
    struct sys_in_addr  sin_addr;           /**< Internet address (32 bits).                     */
    UINT8               sin_zero[8];        /**< Not used.                                       */
};


/**
 *  @struct   sys_sockaddr
 *  @brief    套接字地址结构体/socket address
 */
struct sys_sockaddr
{
    UINT16          sa_family;              /**< Address family (e.g. , AF_INET).                 */
    UINT8           sa_data[14];            /**< Protocol- specific address information.          */
};


/**
 *  @struct   sys_linger
 *  @brief    套接字linger结构体/socket linger
 */
struct sys_linger
{
    INT32 l_onoff;                            /**< 0 = disabled; 1 = enabled; default = 0;*/
    INT32 l_linger;                           /**< linger time in seconds; default = 0;*/
};

/**
 *  @struct   sys_ip_mreq
 *  @brief    IP地址结构体/ip addr
 */
struct sys_ip_mreq
{
    struct sys_in_addr imr_multiaddr; /**< 多播组的IP地址*/
    struct sys_in_addr imr_interface; /**< 加入的客服端主机IP地址*/
};

typedef struct sys_in6_addr sys_in6_u;
typedef struct sys_sockaddr_in6 sys_sockaddr_in6;

/**
 *  @struct   SYS_IN6_ADDR_T
 *  @brief    IPv6地址结构体/IPv6 addr
 */
struct sys_in6_addr{
    union {
        UINT8  u6_addr8[16];    /**< 8位地址 */
        UINT16 u6_addr16[8];    /**< 16位地址 */
        UINT32 u6_addr32[4];    /**< 32位地址 */
    } sys_in6_u;                 /**< ipv6 地址定义 */
    #define sys_s6_addr sys_in6_u.u6_addr8     /**< sys_s6_addr */
    #define sys_s6_addr16 sys_in6_u.u6_addr16  /**< sys_s6_addr16 */
    #define sys_s6_addr32 sys_in6_u.u6_addr32  /**< sys_s6_addr32 */
};

/**
 *  @struct   sys_sockaddr_in6
 *  @brief    套接字IPv6地址结构体/socket IPv6 addr
 */
struct sys_sockaddr_in6 {
    UINT16 sin6_family;            /**< AF_INET6 */
    UINT16 sin6_port;              /**< Transport layer port # */
    UINT32 sin6_flowinfo;          /**< IPv6 flow information */
    struct sys_in6_addr sin6_addr; /**< IPv6 address */
    UINT32 sin6_scope_id;          /**< scope id (new in RFC2553) */
};

/**
 *  @struct   sys_ipv6_mreq
 *  @brief    IPv6 IP地址结构体/IPv6 ip addr
 */
struct sys_ipv6_mreq
{
  struct sys_in6_addr ipv6mr_multiaddr; /*  IPv6 multicast addr */
  UINT32              ipv6mr_interface; /*  interface index, or 0 */
};

#ifndef socklen_t
#define socklen_t UINT32                   /**< socklen_t */
#endif /* socklen_t */

#define UNIX_PATH_MAX 108

/**
 *  @struct   sys_sockaddr_un
 *  @brief    套接字地址结构体/socket address
 */
struct sys_sockaddr_un 
{
    UINT16  sa_family;
    UINT8   sun_path[UNIX_PATH_MAX];       
};

/**
 *  @struct   sys_address_t
 *  @brief    套接字地址结构体/socket address
 */
typedef struct sys_address_t {
  socklen_t size;           /**< size of addr */
  union {
    struct sys_sockaddr     sa;
    struct sys_sockaddr_in  sin;
    struct sys_sockaddr_in6 sin6;
    struct sys_sockaddr_un  su;
  } addr;
} sys_address_t;


#if !defined(FIONREAD) || !defined(FIONBIO)
#define IOCPARM_MASK    (0x7fU)           /**< parameters must be < 128 bytes */
#define IOC_VOID        (0x20000000UL)    /**< no parameters */
#define IOC_OUT         (0x40000000UL)    /**< copy out parameters */
#define IOC_IN          (0x80000000UL)    /**< copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)  /**< 0x20000000 distinguishes new & old ioctl's */
/** _IO cmd */
#define _IO(x,y)        ((LONG)(IOC_VOID|((x)<<8)|(y)))
/** _IOR cmd */
#define _IOR(x,y,t)     ((LONG)(IOC_OUT|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y)))
/** _IOW cmd */
#define _IOW(x,y,t)     ((LONG)(IOC_IN|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y)))
#endif /* !defined(FIONREAD) || !defined(FIONBIO) */

#ifndef FIONREAD
/** _IOR cmd */
#define FIONREAD    _IOR('f', 127, ULONG) /* get # bytes to read */
#endif /* FIONREAD */
#ifndef FIONBIO
/** _IOW cmd */
#define FIONBIO     _IOW('f', 126, ULONG) /* set/clear non-blocking i/o */
#endif /* FIONBIO */

/**
 * @enum  SYS_SOCKET_SHUTDONW_TYPE_E 
 * @brief   关机类型
 */
typedef enum
{
    SYS_SOCKET_SHUTDONW_TYPE_INPUT, /*断开输入流or接收流*/
    SYS_SOCKET_SHUTDONW_TYPE_OUTPUT,/*断开输出流or发送流*/
    SYS_SOCKET_SHUTDONW_TYPE_BOTH   /*同时关闭输入输出*/
} SYS_SOCKET_SHUTDONW_TYPE_E;

/* msg相关操作封装 */
#define SYS_CMSG_DATA(cmsg)         ((cmsg)->uCmsgData)
#define SYS_CMSG_NXTHDR(mhdr, cmsg) sys_socket_cmsg_nxthdr(mhdr, cmsg)
#define SYS_CMSG_FIRSTHDR(mhdr)                                  \
    ((size_t)(mhdr)->uMsgControllen >= sizeof(SYS_CMSG_HEADER_T) \
         ? (SYS_CMSG_HEADER_T *)(mhdr)->pMsgControl              \
         : (SYS_CMSG_HEADER_T *)0)
#define SYS_CMSG_ALIGN(len) (((len) + sizeof(size_t) - 1) & (size_t)~(sizeof(size_t) - 1))
#define SYS_CMSG_SPACE(len) (SYS_CMSG_ALIGN(len) + SYS_CMSG_ALIGN(sizeof(SYS_CMSG_HEADER_T)))
#define SYS_CMSG_LEN(len)   (SYS_CMSG_ALIGN(sizeof(SYS_CMSG_HEADER_T)) + (len))

/* 后续需要将此定义移动至sys层 */
typedef struct
{
    VOID   *pIovBase;      /* buffer空间的基地址 */
    size_t  uIovLen;       /* 该buffer空间的长度 */
} SYS_IOVEC_T;

typedef struct
{
    VOID        *pMsgName;       /* optional address */
    socklen_t    uMsgNameLen;    /* size of address */
    SYS_IOVEC_T *aMsgIov;        /* scatter/gather array */
    size_t       uMsgIovSize;    /* # elements in aMsgIov */
    VOID        *pMsgControl;    /* ancillary data, see below */
    size_t       uMsgControllen; /* ancillary data buffer len */
    INT32        uMsgFlags;      /* flags on received message */
} SYS_IMSG_HEADER_T;

/* Structure used for storage of ancillary data object information.  */
typedef struct
{
    size_t uCmsgLen;  /* Length of data in cmsg_data plus length
                         of cmsghdr structure.
                         !! The type should be socklen_t but the
                         definition of the kernel is incompatible
                         with this.  */
    INT32 iCmsgLevel; /* Originating protocol.  */
    INT32 iCmsgType;  /* Protocol specific type.  */
#if (!defined __STRICT_ANSI__ && __GNUC__ >= 2) || __STDC_VERSION__ >= 199901L
    __extension__ UINT8 uCmsgData[]; /* Ancillary data. __flexarr */
#endif
} SYS_CMSG_HEADER_T;

typedef struct
{
    INT32               iIFIndex;
    struct sys_in_addr  stSpecDesAddr;
    struct sys_in_addr  stAddr;
}SYS_IN_PKTINFO_T;

/* 事件flag */
/* MSG标志枚举 (用于send/recv等套接字操作) */
typedef enum {
    SYS_MSG_OOB          = 0x01,       /* 发送/接收带外数据 (TCP紧急指针) */
    SYS_MSG_PEEK         = 0x02,       /* 查看数据但不移出接收缓冲区 */
    SYS_MSG_DONTROUTE    = 0x04,       /* 绕过路由表直接发送到直连主机 */
    SYS_MSG_CTRUNC       = 0x08,       /* 控制数据(ancillary data)被截断 */
    SYS_MSG_PROXY        = 0x10,       /* 提供或请求代理地址 (透明代理场景) */
    SYS_MSG_TRUNC        = 0x20,       /* 普通数据被截断 (数据超出缓冲区) */
    SYS_MSG_DONTWAIT     = 0x40,       /* 非阻塞操作 (等价于O_NONBLOCK) */
    SYS_MSG_EOR          = 0x80,       /* 结束记录 (用于SOCK_SEQPACKET类型) */
    SYS_MSG_WAITALL      = 0x100,      /* 等待直到填满完整请求缓冲区 */
    SYS_MSG_FIN          = 0x200,      /* 发送TCP FIN标志 (用于原始套接字) */
    SYS_MSG_SYN          = 0x400,      /* 发送TCP SYN标志 (用于原始套接字) */
    SYS_MSG_CONFIRM      = 0x800,      /* 确认路由有效性 (防止ARP缓存失效) */
    SYS_MSG_RST          = 0x1000,     /* 发送TCP RST标志 (用于原始套接字) */
    SYS_MSG_ERRQUEUE     = 0x2000,     /* 从错误队列获取错误信息 (如IP_RECVERR) */
    SYS_MSG_NOSIGNAL     = 0x4000,     /* 不发送SIGPIPE信号 (对方关闭连接时) */
    SYS_MSG_MORE         = 0x8000,     /* 延迟发送数据 (等待后续数据聚合) */
    SYS_MSG_WAITFORONE   = 0x10000,    /* 等待至少一个数据包返回 (recvmmsg专用) */
    SYS_MSG_BATCH        = 0x40000,    /* 批量发送消息 (sendmmsg专用) */
    SYS_MSG_ZEROCOPY     = 0x4000000,  /* 零拷贝发送 (需要内核3.0+和适当配置) */
    SYS_MSG_FASTOPEN     = 0x20000000, /* TCP快速打开 (在SYN包携带数据) */
    SYS_MSG_CMSG_CLOEXEC = 0x40000000  /* 通过SCM_RIGHTS接收的文件描述符设置O_CLOEXEC */
} SYS_MSG_FLAGS_E;
#endif /* SOCK_SOCKET_DEFINE_H */

