#ifndef _SOCKET_DEFINE_H_
#define _SOCKET_DEFINE_H_
#include "Common.h"
/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* Define some BSD protocol constants.  */
#define SOCK_SOCK_STREAM                     (1)                    /**< TCP Socket */
#define SOCK_SOCK_DGRAM                      (2)                    /**< UDP Socket */

 #define SOCK_IPPROTO_IP                     (0)                    /**< dummy for IP */
 #define SOCK_IPPROTO_ICMP                   (1)                    /**< control message protocol */
 #define SOCK_IPPROTO_IGMP                   (2)                    /**< internet group management protocol */
 #define SOCK_IPPROTO_GGP                    (3)                    /**< gateway^2 (deprecated) */
 #define SOCK_IPPROTO_TCP                    (6)                    /**< tcp */
 #define SOCK_IPPROTO_PUP                    (12)                   /**< pup */
 #define SOCK_IPPROTO_UDP                    (17)                   /**< user datagram protocol */
 #define SOCK_IPPROTO_IDP                    (22)                   /**< xns idp */
 #define SOCK_IPPROTO_IPV6                   (41)                   /**< IPv6-in-IPv4 tunnelling */
 
 #define SOCK_IPPROTO_ND                     (77)                   /**< UNOFFICIAL net disk proto */
 #define SOCK_IPPROTO_RAW                    (255)                  /**< raw IP packet */
 #define SOCK_IPPROTO_MAX                    (256)                  /**< max ip proto */

#define SOCK_IP_TOS                          (1)                    /**< tos */
#define SOCK_IP_TTL                          (2)                    /**< time to live */
#define SOCK_IP_HDRINCL                      (3)                    /**< hdincl */
#define SOCK_IP_OPTIONS                      (4)                    /**< options */
#define SOCK_IP_ROUTER_ALERT                 (5)                    /**< route alert */
#define SOCK_IP_RECVOPTS                     (6)                    /**< recv ops */
#define SOCK_IP_RETOPTS                      (7)                    /**< retopts */
#define SOCK_IP_PKTINFO                      (8)                    /**< pktinfo */
#define SOCK_IP_PKTOPTIONS                   (9)                    /**< packet options */
#define SOCK_IP_MTU_DISCOVER                 (10)                   /**< mtu discovery */
#define SOCK_IP_RECVERR                      (11)                   /**< recv error */
#define SOCK_IP_RECVTTL                      (12)                   /**< recv ttl */
#define SOCK_IP_RECVTOS                      (13)                   /**< recv tos */
#define SOCK_IP_MTU                          (14)                   /**< mtu */
#define SOCK_IP_FREEBIND                     (15)                   /**< freebind */
#define SOCK_IP_IPSEC_POLICY                 (16)                   /**< ipsec policy */
#define SOCK_IP_XFRM_POLICY                  (17)                   /**< xfrm policy */
#define SOCK_IP_PASSSEC                      (18)                   /**< passsec */
#define SOCK_IP_TRANSPARENT                  (19)                   /**< transparent */
#define SOCK_IP_MULTICAST_IF                 (32)                   /**< multicast if */
#define SOCK_IP_MULTICAST_TTL                (33)                   /**< multicast ttl */
#define SOCK_IP_MULTICAST_LOOP               (34)                   /**< multicast loop */
#define SOCK_IP_ADD_MEMBERSHIP               (35)                   /**< add membership */
#define SOCK_IP_DROP_MEMBERSHIP              (36)                   /**< drop membership */
#define SOCK_IP_UNBLOCK_SOURCE               (37)                   /**< unblock source */
#define SOCK_IP_BLOCK_SOURCE                 (38)                   /**< block source */
#define SOCK_IP_ADD_SOURCE_MEMBERSHIP        (39)                   /**< add source membership */
#define SOCK_IP_DROP_SOURCE_MEMBERSHIP       (40)                   /**< drop source membership */
#define SOCK_IP_MSFILTER                     (41)                   /**< msfilter */

/* IPV6 */
#define SOCK_IPV6_MULTICAST_IF               (17)                   /**< multicast if */
#define SOCK_IPV6_MULTICAST_HOPS             (18)                   /**< multicast ttl */
#define SOCK_IPV6_MULTICAST_LOOP             (19)                   /**< multicast loop */
#define SOCK_IPV6_ADD_MEMBERSHIP             (20)                   /**< add membership */
#define SOCK_IPV6_DROP_MEMBERSHIP            (21)                   /**< drop membership */

#define SOCK_TCP_MAXSEG                      (50)                   /**< tcp max seg */

#define SOCK_PF_INET                         (2)                    /**< IP protocol family.  */
#define SOCK_PF_INET6                        (10)                   /**< IP version 6.  */

#define SOCK_AF_INET                         (SOCK_PF_INET)          /**< mtu discovery */
#define SOCK_AF_INET6                        (SOCK_PF_INET6)         /**< mtu discovery */                
#define SOCK_SOL_SOCKET                      (1)                    /**< mtu discovery */
#define SOCK_SO_REUSEADDR                    (2)                    /**< Allow local address reuse */
#define SOCK_SO_ERROR                        (4)                    /**< error */
#define SOCK_SO_BROADCAST                    (6)                    /**< broadcast */
#define SOCK_SO_SNDBUF                       (7)                    /**< send buffer */
#define SOCK_SO_RCVBUF                       (8)                    /**< recv buffer */
#define SOCK_SO_KEEPALIVE                    (9)                    /**< kepp alive */
#define SOCK_SO_RCVTIMEO                     (20)                   /**< recv timeout */
#define SOCK_SO_SNDTIMEO                     (21)                   /**< send timeout */
#define SOCK_SO_LINGER                       (22)                   /**< linkger */
#define SOCK_SO_NO_CHECK                     (23)                   /**< no check */
#define SOCK_SO_BINDTODEVICE                 (25)                   /**< bind device */

#define SOCK_F_GETFL                          (3)                   /**< SOCK_F_GETFL */
#define SOCK_F_SETFL                          (4)                   /**< SOCK_F_SETFL */
#define SOCK_O_NONBLOCK                       (1)                   /**< SOCK_O_NONBLOCK */

#define SOCK_TCP_NODELAY   (0x01)                                   /**< SOCK_O_NONBLOCK */
#define SOCK_TCP_KEEPIDLE                     (4)                   /**< TCP Keep idle */
#define SOCK_TCP_KEEPINTVL                    (5)                   /**< TCP Keep intvl */
#define SOCK_TCP_KEEPCNT                      (6)                   /**< TCP Keep cnt */

#define SOCK_FD_SETSIZE    (1024)                                   /**< SOCK_FD_SETSIZE */
#define SOCK_FD_SET(n, p)  ((p)->fd_bits[(n)/8u] |=  (1u << ((n) & 7u))) /**< SOCK_FD_SET */
#define SOCK_FD_CLR(n, p)  ((p)->fd_bits[(n)/8u] &= ~(1u << ((n) & 7u))) /**< SOCK_FD_CLR */
#define SOCK_FD_ISSET(n,p) ((p)->fd_bits[(n)/8u] &   (1u << ((n) & 7u))) /**< SOCK_FD_ISSET */
#define SOCK_FD_ZERO(p)    memset((VOID*)(p),0,sizeof(*(p)))               /**< SOCK_FD_ZERO */

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**
 *  @enum   SOCK_TCP_STATE_E
 *  @brief  tcp状态枚举/tcp state
 */
typedef enum
{
    SOCK_TCP_ESTABLISHED = 1,
    SOCK_TCP_SYN_SENT,
    SOCK_TCP_SYN_RECV,
    SOCK_TCP_FIN_WAIT1,
    SOCK_TCP_FIN_WAIT2,
    SOCK_TCP_TIME_WAIT,
    SOCK_TCP_CLOSE,
    SOCK_TCP_CLOSE_WAIT,
    SOCK_TCP_LAST_ACK,
    SOCK_TCP_LISTEN,
    SOCK_TCP_CLOSING   /* now a valid state */
} SOCK_TCP_STATE_E;

/**
 *  @struct   SOCK_IN_ADDR
 *  @brief    网络地址结构体/Internet address
 */
typedef struct 
{
    UINT32           u32addr;               /**< Internet address (32 bits).                      */
}SOCK_IN_ADDR_T;

/**
 *  @struct   SOCK_SOCKADDR_IN
 *  @brief    套接字网络地址结构体/socket Internet address
 */
typedef struct
{
    UINT16              u16Sin_family;         /**< Internet Protocol (AF_INET).                    */
    UINT16              u16Sin_port;           /**< Address port (16 bits).                         */
    SOCK_IN_ADDR_T      stSin_addr;           /**< Internet address (32 bits).                     */
    UINT8               u8Sin_zero[8];        /**< Not used.                                       */
}SOCK_SOCKADDR_IN_T;


/**
 *  @struct   SOCK_SOCKADDR
 *  @brief    套接字地址结构体/socket address
 */
typedef struct 
{
    UINT16          u16Sa_family;              /**< Address family (e.g. , AF_INET).                 */
    UINT8           u8Sa_data[14];            /**< Protocol- specific address information.          */
}SOCK_SOCKADDR_T;


/**
 *  @struct   SOCK_LINGER
 *  @brief    套接字linger结构体/socket linger
 */
typedef struct 
{
    INT32 i32l_onoff;                            /**< 0 = disabled; 1 = enabled; default = 0;*/
    INT32 i32l_linger;                           /**< linger time in seconds; default = 0;*/
}SOCK_LINGER_T;

/**
 *  @struct   SOCK_IP_MREQ
 *  @brief    IP地址结构体/ip addr
 */
typedef struct 
{
   SOCK_IN_ADDR_T stImr_multiaddr; /**< 多播组的IP地址*/
   SOCK_IN_ADDR_T stImr_interface; /**< 加入的客服端主机IP地址*/
}SOCK_IP_MREQ_T;


/**
 *  @struct   SOCK_IN6_ADDR_T
 *  @brief    IPv6地址结构体/IPv6 addr
 */
typedef struct
{
    union {
        UINT8  u6_addr8[16];    /**< 8位地址 */
        UINT16 u6_addr16[8];    /**< 16位地址 */
        UINT32 u6_addr32[4];    /**< 32位地址 */
    } SOCK_IN6_U;                 /**< ipv6 地址定义 */
    #define SOCK_S6_ADDR SOCK_IN6_U.u6_addr8     /**< SOCK_S6_ADDR */
    #define SOCK_S6_ADDR16 SOCK_IN6_U.u6_addr16  /**< SOCK_S6_ADDR16 */
    #define SOCK_S6_ADDR32 SOCK_IN6_U.u6_addr32  /**< SOCK_S6_ADDR32 */
}SOCK_IN6_ADDR_T;

/**
 *  @struct   SOCK_SOCKADDR_IN6_T
 *  @brief    套接字IPv6地址结构体/socket IPv6 addr
 */
typedef struct
{
    UINT16 sin6_family;            /**< AF_INET6 */
    UINT16 sin6_port;              /**< Transport layer port # */
    UINT32 sin6_flowinfo;          /**< IPv6 flow information */
    SOCK_IN6_ADDR_T sin6_addr;      /**< IPv6 address */
    UINT32 sin6_scope_id;          /**< scope id (new in RFC2553) */
}SOCK_SOCKADDR_IN6_T;

/**
 *  @struct   SOCK_ipv6_mreq
 *  @brief    IPv6 IP地址结构体/IPv6 ip addr
 */
typedef struct{
      SOCK_IN6_ADDR_T ipv6mr_multiaddr; /*  IPv6 multicast addr */
      UINT32        ipv6mr_interface; /*  interface index, or 0 */
}SOCK_IPV6_MREQ_T;

#ifndef socklen_t
#define socklen_t UINT32                   /**< socklen_t */
#endif /* socklen_t */

#define UNIX_PATH_MAX 108

/**
 *  @struct   SOCK_sockaddr_un
 *  @brief    套接字地址结构体/socket address
 */
typedef struct{
    UINT16  sa_family;
    UINT8   sun_path[UNIX_PATH_MAX];       
}SOCK_SOCKADDR_UN_T;

/**
 *  @struct   SOCK_address_t
 *  @brief    套接字地址结构体/socket address
 */
typedef struct {
  socklen_t size;           /**< size of addr */
  union {
    SOCK_SOCKADDR_T     sa;
    SOCK_SOCKADDR_IN_T  sin;
    SOCK_SOCKADDR_IN6_T sin6;
    SOCK_SOCKADDR_UN_T  su;
  } addr;
} SOCK_ADDRESS_T;


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
 * @enum  SOCK_SOCKET_SHUTDONW_TYPE_E 
 * @brief   关机类型
 */
typedef enum
{
    SOCK_SOCKET_SHUTDONW_TYPE_INPUT, /*断开输入流or接收流*/
    SOCK_SOCKET_SHUTDONW_TYPE_OUTPUT,/*断开输出流or发送流*/
    SOCK_SOCKET_SHUTDONW_TYPE_BOTH   /*同时关闭输入输出*/
} SOCK_SOCKET_SHUTDONW_TYPE_E;

#endif /* SOCK_SOCKET_DEFINE_H */

