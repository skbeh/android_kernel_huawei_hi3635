/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : IpsMntnCCore.h
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2013年7月23日
  最近修改   :
  功能描述   : IPS MNTN模块头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/

#ifndef __IPSMNTNCCORE_H__
#define __IPSMNTNCCORE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include  "product_config.h"
#include  "vos.h"

#if( FEATURE_ON == FEATURE_IMS )

#include  "PsTypeDef.h"
#include  "InterpeakMntnInterface.h"


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define IPS_CCORE_IFNAMSIZ                    (16)                    /*虚拟网卡名称长度*/
#define IPS_CCORE_END_NAME                    (8)
#define IPS_CCORE_APP_CMD_INFO_LEN            (200)                   /*捕获APP控制命令的最长长度*/
#define IPS_MNTN_CCORE_TRACE_MAX_BYTE_LEN     (1514)                  /*捕获TCP/IP协议栈报文最长长度 */
#define IPS_MNTN_CCORE_NAT_MAPPING_INFO_LEN   (40)                    /*捕获TCP/IP协议栈NAT映射信息基本长度*/


/*填充消息头*/
#define IPS_CCORE_FILL_MSG_HEADER(pSendMsg, RecvPid)    \
        (pSendMsg)->ulReceiverCpuId  = VOS_LOCAL_CPUID; \
        (pSendMsg)->ulReceiverPid    = RecvPid;


#define IPS_CCORE_OM_TRANS_DATA_OFFSET_LEN        /*lint -e(545) -e(413)*/offsetof(OM_APP_TRACE_STRU, aucPara)

#define IPS_CCORE_PS_OM_TRANS_DATA_OFFSET_LEN     /*lint -e(545) -e(413)*/offsetof(PS_OM_TRANS_IND_STRU, aucData)

#define IPS_CCORE_PS_OM_TRANS_OM_HEADER_LEN       /*lint -e(545) -e(413)*/offsetof(PS_OM_TRANS_IND_STRU, ulSn)

#define IPS_CCORE_PKT_INFO_OFFSET_LEN             /*lint -e(545) -e(413)*/offsetof(IPS_MNTN_CCORE_PKT_INFO_STRU, aucData)

#define IPS_CCORE_BRIDGE_PKT_INFO_OFFSET_LEN      /*lint -e(545) -e(413)*/offsetof(IPS_MNTN_CCORE_BRIDGE_PKT_INFO_STRU, aucData)

#define IPS_CCORE_APP_CMD_INFO_OFFSET_LEN         /*lint -e(545) -e(413)*/offsetof(IPS_MNTN_CCORE_APP_CMD_INFO_STRU, aucData)

#if 0
#define MNTN_PKT_INFO_LEN               (OM_TRANS_DATA_OFFSET_LEN + MNTN_PKT_INFO_OFFSET_LEN + IPS_MNTN_CCORE_TRACE_MAX_BYTE_LEN)

#define MNTN_BRIDGE_PKT_INFO_LEN        (OM_TRANS_DATA_OFFSET_LEN + MNTN_BRIDGE_PKT_INFO_OFFSET_LEN + IPS_MNTN_CCORE_TRACE_MAX_BYTE_LEN)

#define MNTN_APP_CMD_INFO_LEN           (OM_TRANS_DATA_OFFSET_LEN + MNTN_APP_CMD_INFO_OFFSET_LEN + IPS_CCORE_APP_CMD_INFO_LEN)
#endif

#define IPS_CCORE_INFO_LEN                      (IPS_CCORE_OM_TRANS_DATA_OFFSET_LEN + IPS_MNTN_CCORE_INFO_LEN)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum IPS_MNTN_CCORE_TRACE_CONFIG_TYPE_ENUM
{
    ID_OM_IPS_CCORE_ADVANCED_TRACE_CONFIG_REQ = 0xD513,               /* 捕获TCP/IP协议栈 报文配置*/
    ID_IPS_CCORE_OM_ADVANCED_TRACE_CONFIG_CNF = 0xD514,               /* 捕获TCP/IP协议栈 报文指示*/

    ID_OM_IPS_MNTN_CCORE_INFO_CONFIG_REQ      = 0xD515,               /* 周期性捕获TCP/IP协议栈 基础信息配置*/
    ID_IPS_CCORE_OM_MNTN_INFO_CONFIG_CNF      = 0xD516,               /* 周期性捕获TCP/IP协议栈 基础信息配置指示*/

    ID_IPS_MNTN_CCORE_TRACE_CONFIG_TYPE_BUTT
};
typedef VOS_UINT16 IPS_MNTN_CCORE_TRACE_CONFIG_TYPE_ENUM_UINT16;

enum IPS_MNTN_CCORE_INFO_ACTION_ENUM
{
    IPS_MNTN_CCORE_INFO_REPORT_STOP           = 0,                    /*开启周期性捕获TCP/IP协议栈基础信息*/
    IPS_MNTN_CCORE_INFO_REPORT_START          = 1,                    /*关闭周期性捕获TCP/IP协议栈基础信息*/

    IPS_MNTN_CCORE_INFO_REPORT_BUTT
};
typedef VOS_UINT16 IPS_MNTN_CCORE_INFO_ACTION_ENUM_UINT16;

enum IPS_MNTN_CCORE_RESULT_TYPE_ENUM
{
    IPS_MNTN_CCORE_RESULT_OK                  = 0,
    IPS_MNTN_CCORE_RESULT_START_TIMER_FAIL    = 1,
    IPS_MNTN_CCORE_RESULT_ALLOC_MEM_FAIL      = 2,
    IPS_MNTN_CCORE_RESULT_ALLOC_MSG_FAIL      = 3,
    IPS_MNTN_CCORE_RESULT_INPUT_PARAM_ERR     = 4,
    IPS_MNTN_CCORE_RESULT_LOCAL_PARAM_ERR     = 5,

    IPS_MNTN_RESULT_BUTT
};
typedef VOS_UINT16  IPS_MNTN_CCORE_RESULT_TYPE_ENUM_UINT16;

enum IPS_MNTN_CCORE_TIMER_NAME_ENUM
{
    IPS_MNTN_CCORE_INFO_TIMER                 = 0x00000001,           /*周期性捕获TCP/IP协议栈基础信息定时器*/

    IPS_MNTN_CCORE_TIMER_NAME_BUTT
};
typedef VOS_UINT32 IPS_MNTN_CCORE_TIMER_NAME_ENUM_UINT32;

enum IPS_MNTN_CCORE_TRACE_CHOSEN_ENUM
{
    IPS_MNTN_CCORE_TRACE_NULL_CHOSEN               =   0,             /*不捕获报文信息*/
    IPS_MNTN_CCORE_TRACE_MSG_HEADER_CHOSEN         =   1,             /*捕获报文头部*/
    IPS_MNTN_CCORE_TRACE_CONFIGURABLE_LEN_CHOSEN   =   2,             /*按照配置捕获报文*/
    IPS_MNTN_CCORE_TRACE_WHOLE_DATA_LEN_CHOSEN     =   3,             /*捕获报文全部内容*/

    IPS_MNTN_CCORE_TRACE_MSG_CHOSEN_BUTT
};
typedef VOS_UINT32   IPS_MNTN_CCORE_TRACE_CHOSEN_ENUM_UINT32;

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_CCORE_TRACE_MSG_TYPE_ENUM
{
    ID_IPS_CCORE_TRACE_INPUT_DATA_INFO        = 0xD517,               /*TCP/IP可维可测接收报文*/
    ID_IPS_CCORE_TRACE_OUTPUT_DATA_INFO       = 0xD518,               /*TCP/IP可维可测发送报文*/
    ID_IPS_CCORE_TRACE_BRIDGE_DATA_INFO       = 0xD519,               /*TCP/IP网桥中转报文*/
    ID_IPS_CCORE_TRACE_RECV_ARP_PKT           = 0xD51A,               /*TCP/IP协议栈接收的ARP控制报文*/
    ID_IPS_CCORE_TRACE_SEND_ARP_PKT           = 0xD51B,               /*TCP/IP协议栈发送的ARP控制报文*/
    ID_IPS_CCORE_TRACE_RECV_DHCPC_PKT         = 0xD51C,               /*TCP/IP协议栈接收的DHCP控制报文*/
    ID_IPS_CCORE_TRACE_SEND_DHCPC_PKT         = 0xD51D,               /*TCP/IP协议栈发送的DHCP控制报文*/
    ID_IPS_CCORE_TRACE_RECV_DHCPS_PKT         = 0xD51E,               /*TCP/IP协议栈接收的DHCP控制报文*/
    ID_IPS_CCORE_TRACE_SEND_DHCPS_PKT         = 0xD51F,               /*TCP/IP协议栈发送的DHCP控制报文*/
    ID_IPS_CCORE_TRACE_APP_CMD                = 0xD520,               /*APP控制命令信息*/
    ID_IPS_CCORE_TRACE_MNTN_INFO              = 0xD521,               /*TCP/IP协议栈基础信息*/
    ID_IPS_MNTN_CCORE_IMS_NIC_INFO            = 0xD522,               /*IMS虚拟网卡统计信息*/

    ID_IPS_CCORE_TRACE_MSG_TYPE_BUTT          = 0xffff
};
typedef VOS_UINT16  IPS_MNTN_CCORE_TRACE_MSG_TYPE_ENUM_UINT16;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
#pragma pack(4)

typedef struct
{
    IPS_MNTN_CCORE_TRACE_CHOSEN_ENUM_UINT32   ulChoice;
    VOS_UINT32                                ulTraceDataLen;         /* 当选择TRACE_CONFIGURABLE_LEN_CHOSEN时，此值有效 */

}IPS_MNTN_CCORE_TRACE_CONFIG_STRU;

typedef IPS_MNTN_CCORE_TRACE_CONFIG_STRU  IPS_MNTN_CCORE_BRIDGE_TRACE_CFG_STRU;

typedef IPS_MNTN_CCORE_TRACE_CONFIG_STRU  IPS_MNTN_CCORE_INPUT_TRACE_CFG_STRU;

typedef IPS_MNTN_CCORE_TRACE_CONFIG_STRU  IPS_MNTN_CCORE_OUTPUT_TRACE_CFG_STRU;

typedef struct
{
    IPS_MNTN_CCORE_BRIDGE_TRACE_CFG_STRU      stBridgeTraceCfg;       /*配置捕获网桥消息*/
    IPS_MNTN_CCORE_INPUT_TRACE_CFG_STRU       stInputTraceCfg;        /*配置捕获TCP/IP协议栈接收消息*/
    IPS_MNTN_CCORE_OUTPUT_TRACE_CFG_STRU      stOutputTraceCfg;       /*配置捕获TCP/IP协议栈发送消息*/
    VOS_UINT8                                 aucRsv[100];            /* 保留位，为后期可扩展 */
}IPS_MNTN_CCORE_TRACE_CONFIG_REQ_STRU;

typedef struct
{
    PS_RSLT_CODE_ENUM_UINT32            enRslt;                 /* 返回查询结构成功或失败，PS_SUCC表示成功，PS_FAIL表示失败 */

}IPS_MNTN_CCORE_TRACE_CONFIG_CNF_STRU;

typedef struct
{
    IPS_MNTN_CCORE_INFO_ACTION_ENUM_UINT16    enCommand;
    VOS_UINT16                                usTimeLen;              /*单位：秒 */
}IPS_MNTN_CCORE_INFO_CFG_REQ_STRU;

typedef struct
{
    IPS_MNTN_CCORE_INFO_ACTION_ENUM_UINT16    enCommand;
    IPS_MNTN_CCORE_RESULT_TYPE_ENUM_UINT16    enRslt;
}IPS_MNTN_CCORE_INFO_CFG_CNF_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_CCORE_INFO_ENTITY
 结构说明  : 捕获TCP/IP协议栈基本信息实体
*****************************************************************************/
typedef struct
{
    HTIMER                                    hIpsMntnTimer;          /*定时器*/
    VOS_UINT32                                ulIpsMntnTimerLen;      /*定时器长度，固定1s*/
    IPS_MNTN_CCORE_INFO_ACTION_ENUM_UINT16    enIpsMntnFlag;          /*捕获TCP/IP协议栈基本信息开启标志*/
    VOS_UINT8                                 aucRsv[2];
}IPS_MNTN_CCORE_INFO_ENTITY;

/*****************************************************************************
 结构名    : IPS_MNTN_CCORE_PKT_INFO_STRU
 结构说明  : TCP/IP协议栈报文捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               aucNetIfName[IPS_CCORE_IFNAMSIZ]; /* 虚拟网卡名称 */
    VOS_UINT16                              usLen;
    VOS_UINT8                               aucData[2];
} IPS_MNTN_CCORE_PKT_INFO_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_CCORE_BRIDGE_PKT_INFO_STRU
 结构说明  : 网桥中转报文捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               aucSrcPort[IPS_CCORE_END_NAME];   /* 发送虚拟网卡信息 */
    VOS_UINT8                               aucDestPort[IPS_CCORE_END_NAME];  /* 接收虚拟网卡名称 */
    VOS_UINT16                              usLen;
    VOS_UINT8                               aucData[2];
} IPS_MNTN_CCORE_BRIDGE_PKT_INFO_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_CCORE_APP_CMD_INFO_STRU
 结构说明  : APP控制命令捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usLen;
    VOS_UINT8                               aucData[2];
} IPS_MNTN_CCORE_APP_CMD_INFO_STRU;

/*****************************************************************************
 结构名    : IPS_MNTN_CCORE_INFO_STRU
 结构说明  : APP控制命令捕获
*****************************************************************************/
typedef struct
{
    VOS_UINT16                              usType;
    VOS_UINT8                               aucData[2];
    VOS_UINT32                              ulLen;
} IPS_MNTN_CCORE_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

#endif  /*FEATURE_ON == FEATURE_IMS*/

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of IpsMntnCCore.h */



