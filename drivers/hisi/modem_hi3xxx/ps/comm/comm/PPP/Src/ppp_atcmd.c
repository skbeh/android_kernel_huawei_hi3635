

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_PPP_ATCMD_C

#include "product_config.h"
#if(FEATURE_ON == FEATURE_PPP)

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "layer.h"
#include "ppp_mbuf.h"
#include "hdlc.h"
#include "ppp_fsm.h"
#include "lcp.h"
#include "async.h"
#include "auth.h"
#include "ipcp.h"
#include "pppid.h"
#include "link.h"
#include "ppp_atcmd.h"
#include "ppp_input.h"
#include "ppp_public.h"
#include "hdlc_interface.h"
#include "hdlc_hardware.h"
#include "hdlc_software.h"
#include "NasNvInterface.h"
#include "TafNvInterface.h"

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
extern VOS_VOID PPP_ClearDataQ(VOS_VOID);
extern VOS_VOID Ppp_ProcConfigInfoInd(VOS_UINT16 usPppId);

extern VOS_UINT32 PPP_SavePcoInfo
(
    PPP_ID usPppId,
    AT_PPP_IND_CONFIG_INFO_STRU *pstAtPppIndConfigInfo
);

extern VOS_VOID PPP_GetReqConfigInfo
(
    PPP_REQ_CONFIG_INFO_STRU    *pstPppReqConfigInfo,
    AT_PPP_REQ_CONFIG_INFO_STRU *pstPppAtReqConfigInfo
);
/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/
/* 保存从NV项中读取的WINS特性开关值*/
extern VOS_UINT8                        g_ucPppConfigWins;

/* 配置使用软件还是硬件HDLC，默认使用硬件 */
VOS_UINT32                              g_ulHDLCConfig = PPP_HDLC_BY_HARDWARE;

/******************************************************************************
   5 函数实现
******************************************************************************/

VOS_UINT32  PPP_SetHdlcConfig(VOS_UINT32    ulConfig)
{
    g_ulHDLCConfig = ulConfig;

    return g_ulHDLCConfig;
}


VOS_UINT32  PPP_GetHdlcConfig()
{
    return g_ulHDLCConfig;
}


VOS_UINT32 PPP_InitHdlcConfig(PPP_ID usPppId)
{
    PPP_HDLC_CONFIG_STRU               *pstHdlcConfig;


    if ((usPppId == 0) || (PPP_MAX_ID_NUM < usPppId))
    {
        return VOS_ERR;
    }

    pstHdlcConfig = PPP_CONFIG(usPppId);

    if (PPP_HDLC_BY_SOFTWARE == g_ulHDLCConfig)
    {
        pstHdlcConfig->pFunProcData            = PPP_HDLC_SOFT_ProcData;
        pstHdlcConfig->pFunProcProtocolPacket  = PPP_HDLC_SOFT_ProcProtocolPacket;
        pstHdlcConfig->pFunDisable             = VOS_NULL_PTR;
        pstHdlcConfig->pFunProcAsFrmData       = VOS_NULL_PTR;

        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "\r\nPPP, PPP_InitHdlcConfig, INFO, Soft HDLC.\r\n");
    }
    else
    {
        pstHdlcConfig->pFunProcData            = PPP_HDLC_HARD_ProcData;
        pstHdlcConfig->pFunProcProtocolPacket  = PPP_HDLC_HARD_ProcProtocolPacket;
        pstHdlcConfig->pFunDisable             = PPP_HDLC_HARD_Disable;
        pstHdlcConfig->pFunProcAsFrmData       = PPP_HDLC_HARD_ProcAsFrmPacket;

        PPP_HDLC_HARD_SetUp(usPppId);

        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "\r\nPPP, PPP_InitHdlcConfig, INFO, Hard HDLC.\r\n");
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype      : Ppp_CreatePppReq
 Description    : 为AT模块"创建PPP链路"提供对应的API函数。

 Input          : ---
 Output         : ---创建成功后返回的PPP ID
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_CreatePppReq ( PPP_ID *pusPppId)
{
    PPP_ID pppid_get;


    if(pusPppId == VOS_NULL)
    {
        return VOS_ERR;
    }

    /*从PPP ID数组中得到一个空闲的PPP ID*/
    pppid_get = PppGetId();

    /*如果没有空闲的PPP ID*/
    if(pppid_get == 0)
    {
        return VOS_ERR;
    }

    /*如果有空闲的PPP ID，首先将申请得到的PPP ID赋值给AT_CMD*/
    *pusPppId = pppid_get;

    /*然后调用PPP模块对应的函数*/
    link_Init(PPP_LINK(pppid_get));
    PPP_LINK(pppid_get)->phase = PHASE_ESTABLISH;
    PPP_LINK(pppid_get)->lcp.fsm.state = ST_CLOSED;

    fsm_Open(&(PPP_LINK(pppid_get)->lcp.fsm));

    /*释放PPP数据队列*/
    PPP_ClearDataQ();

    /* 初始化HDLC相关配置 */
    PPP_InitHdlcConfig(pppid_get);

    /* 可维可测信息上报*/
    Ppp_EventMntnInfo(pppid_get, AT_PPP_CREATE_PPP_REQ);

    /*返回正确*/
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : Ppp_CreateRawDataPppReq
 Description    : 创建PDP类型为PPP的PPP实体，但不做链路管理，只作数据的封装和解封装

 Input          : ---
 Output         : ---创建成功后返回的PPP ID
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_CreateRawDataPppReq ( PPP_ID *pusPppId)
{
    PPP_ID  pppid_get;


    if (VOS_NULL_PTR == pusPppId)
    {
        return VOS_ERR;
    }

    /*从PPP ID数组中得到一个空闲的PPP ID*/
    pppid_get = PppGetId();

    /*如果没有空闲的PPP ID*/
    if (0 == pppid_get)
    {
        return VOS_ERR;
    }

    /* PPP类型PDP激活时，无法得知TE和网络端的协商结果，强制赋值 */
    PPP_LINK(pppid_get)->lcp.his_accmap = 0xffffffff;

    /*如果有空闲的PPP ID，首先将申请得到的PPP ID赋值给AT_CMD*/
    *pusPppId = pppid_get;

    /* 初始化HDLC相关配置 */
    PPP_InitHdlcConfig(pppid_get);

    /* 可维可测信息上报*/
    Ppp_EventMntnInfo(pppid_get, AT_PPP_CREATE_RAW_PPP_REQ);

    /*返回正确*/
    return VOS_OK;
} /* Ppp_CreateRawDataPppReq */

/*****************************************************************************
 Prototype      : Ppp_ReleasePppReq
 Description    : 为AT模块"释放PPP链路"提供对应的API函数。

 Input          : ---要释放的PPP链路对应的PPP ID
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_ReleasePppReq ( PPP_ID usPppId)
{
    VOS_UINT32                          ulRet;


    /* 可维可测信息上报*/
    Ppp_EventMntnInfo(usPppId, AT_PPP_RELEASE_PPP_REQ);

    if(VOS_OK != PppIsIdValid(usPppId))
    {
        return VOS_ERR;
    }

    /* 如果当前PPP在PHASE_NETWORK阶段,属于网侧主动去激活
       此时PPP等待和PC间PPP协议结束后通知AT拉管脚信号,并起定时器保护*/
    if (PHASE_NETWORK == (PPP_LINK(usPppId)->phase))
    {
        if (VOS_NULL_PTR != (PPP_LINK(usPppId)->lcp.hLcpCloseTimer))
        {
            PS_STOP_REL_TIMER(&(PPP_LINK(usPppId)->lcp.hLcpCloseTimer));
            PPP_LINK(usPppId)->lcp.hLcpCloseTimer= VOS_NULL_PTR;
        }

        /*起定时器,确保通知拉AT管脚信号*/
        ulRet = VOS_StartRelTimer(&(PPP_LINK(usPppId)->lcp.hLcpCloseTimer),  PS_PID_APP_PPP,
            1000,  usPppId,  PHASE_TERMINATE_PENDING,  VOS_RELTIMER_NOLOOP,  VOS_TIMER_PRECISION_5 );

        if (VOS_OK != ulRet)
        {
            PPP_LINK(usPppId)->lcp.hLcpCloseTimer = VOS_NULL_PTR;
            PPP_ProcPppDisconnEvent(usPppId);
        }
    }

    /*首先调用PPP模块对应的函数*/
    fsm_Close(&(PPP_LINK(usPppId)->ipcp.fsm));
    fsm_Close(&(PPP_LINK(usPppId)->lcp.fsm));

    /*停止IPCP状态机定时器:*/
    if( VOS_NULL_PTR !=((PPP_LINK(usPppId))->ipcp.fsm.timer) )
    {
        VOS_StopRelTimer(&((PPP_LINK(usPppId))->ipcp.fsm.timer));
        (PPP_LINK(usPppId))->ipcp.fsm.timer = VOS_NULL_PTR;
    }

    /*停止CHAP状态机定时器:*/
    if( VOS_NULL_PTR !=((PPP_LINK(usPppId))->chap.auth.hAuthTimer) )
    {
        VOS_StopRelTimer(&((PPP_LINK(usPppId))->chap.auth.hAuthTimer));
        (PPP_LINK(usPppId))->chap.auth.hAuthTimer = VOS_NULL_PTR;
    }

    /*停止LCP状态机定时器:*/
    if( VOS_NULL_PTR !=((PPP_LINK(usPppId))->lcp.fsm.timer) )
    {
        VOS_StopRelTimer(&((PPP_LINK(usPppId))->lcp.fsm.timer));
        (PPP_LINK(usPppId))->lcp.fsm.timer = VOS_NULL_PTR;
    }

    /*释放待PDP激活定时器*/
    if (VOS_NULL_PTR != (PPP_LINK(usPppId)->ipcp.hIpcpPendTimer))
    {
        PS_STOP_REL_TIMER(&(PPP_LINK(usPppId)->ipcp.hIpcpPendTimer));
        PPP_LINK(usPppId)->ipcp.hIpcpPendTimer = VOS_NULL_PTR;
    }

    /*释放待处理IPCP帧*/
    if (VOS_NULL_PTR != (PPP_LINK(usPppId)->ipcp.pstIpcpPendFrame))
    {
        ppp_m_freem(PPP_LINK(usPppId)->ipcp.pstIpcpPendFrame);
        PPP_LINK(usPppId)->ipcp.pstIpcpPendFrame = VOS_NULL_PTR;
    }

    PppFreeId(usPppId);

    /* 不用释放PPP数据队列，因为只要队列里面有数据，PPP任务就会被调度起来处理，
       如果HDLC处理完成而PPP实体已经释放，那么封装或解封装出来的数据自然会被丢弃。
       这个API会在AT任务里被调用，如果这里把数据放掉，PPP任务有可能正在使用 */
    /* PPP_ClearDataQ(); */

    /*返回正确*/
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : Ppp_ReleaseRawDataPppReq
 Description    : 为AT模块"释放PDP类型为PPP的PPP链路"提供对应的API函数。

 Input          : ---要释放的PPP链路对应的PPP ID
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_ReleaseRawDataPppReq ( PPP_ID usPppId)
{
    /* 可维可测信息上报*/
    Ppp_EventMntnInfo(usPppId, AT_PPP_RELEASE_RAW_PPP_REQ);

    if(VOS_OK != PppIsIdValid(usPppId))
    {
        return VOS_ERR;
    }

    PppFreeId(usPppId);

    /*返回正确*/
    return VOS_OK;
}


VOS_UINT32 PPP_ProcTeConfigInfo (VOS_UINT16 usPppId, PPP_REQ_CONFIG_INFO_STRU *pPppReqConfigInfo)
{
    AT_PPP_REQ_CONFIG_INFO_STRU         stPppAtReqConfigInfo;


    PPP_GetReqConfigInfo(pPppReqConfigInfo, &stPppAtReqConfigInfo);

    return At_RcvTeConfigInfoReq(usPppId,&stPppAtReqConfigInfo);
}


VOS_UINT32 PPP_ProcPppRelEvent (VOS_UINT16 usPppId)
{
    return At_RcvPppReleaseInd (usPppId);
}


VOS_UINT32 PPP_ProcPppDisconnEvent (VOS_UINT16 usPppId)
{
    return Ppp_SndPPPDisconnIndtoAT(usPppId);
}

/*****************************************************************************
 Prototype      : Ppp_RcvConfigInfoInd
 Description    : 为AT模块"PPP模块接收网侧指示的配置信息"提供对应的API函数。
                  当AT向GGSN认证成功后，调用此函数向PPP发指示。

 Input          : usPppId---要发指示的PPP链路所在的PPP ID
                  pPppIndConfigInfo---从GGSN发来的该PPP链路的IP地址等信息
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_RcvConfigInfoInd
(
    PPP_ID usPppId,
    AT_PPP_IND_CONFIG_INFO_STRU         *pstAtPppIndConfigInfo
)
{
    VOS_UINT8                               ucRabId;
    VOS_UINT32                              ulResult;


    if(VOS_OK != PppIsIdValid(usPppId))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP, Ppp_RcvConfigInfoInd, WARNING, Invalid PPP Id %d",
                      usPppId);
        return VOS_ERR;
    }


    /* 通过usPppId，寻找到usRabId */
    if ( !PPP_PPPID_TO_RAB(usPppId, &ucRabId) )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                      "PPP, Ppp_RcvConfigInfoInd, WARNING, Can not get PPP Id %d, RabId %d",
                      usPppId, ucRabId);

        return VOS_ERR;
    }

    /* 这个时候PDP已经激活，注册上行数据接收接口。另外当前不支持PPP类型拨号。 */
    ulResult= ADS_DL_RegDlDataCallback(ucRabId, (RCV_DL_DATA_FUNC)PPP_PushPacketEvent);

    if ( VOS_OK != ulResult )
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP, Ppp_RcvConfigInfoInd, WARNING, Register DL CB failed! RabId %d",
                      ucRabId);

        return VOS_ERR;
    }

    /* 保存PCO信息 */
    PPP_SavePcoInfo(usPppId, pstAtPppIndConfigInfo);

    Ppp_RcvConfigInfoIndMntnInfo(usPppId, pstAtPppIndConfigInfo);

    PPP_RcvAtCtrlOperEvent(usPppId, PPP_AT_CTRL_CONFIG_INFO_IND);

    /*返回正确*/
    return VOS_OK;
}


VOS_UINT32 PPP_RcvAtCtrlOperEvent(VOS_UINT16 usPppId, PPP_AT_CTRL_OPER_TYPE_ENUM_UINT32 ulCtrlOperType)
{
    PPP_AT_CTRL_OPERATION_MSG *pSndMsg;

    if(PPP_AT_CTRL_BUTT <= ulCtrlOperType)
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,
                "PPP_RcvAtCtrlOperEvent:ulCtrlOperType ERROR!",(VOS_INT32)ulCtrlOperType);
        return VOS_ERR;
    }

    pSndMsg = (PPP_AT_CTRL_OPERATION_MSG *)PS_ALLOC_MSG(PS_PID_APP_PPP, sizeof(PPP_AT_CTRL_OPERATION_MSG) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pSndMsg)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
            "PPP, PPP_RcvAtCtrlOperEvent, WARNING, Alloc msg fail!\r\n");
        return VOS_ERR;
    }

    pSndMsg->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pSndMsg->ulReceiverPid      = PS_PID_APP_PPP;
    pSndMsg->ulMsgType          = PPP_AT_CTRL_OPERATION;
    pSndMsg->usPppId            = usPppId;
    pSndMsg->ulCtrlOpType       = ulCtrlOperType;

    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG(PS_PID_APP_PPP, pSndMsg) )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "PPP, PPP_RcvAtCtrlOperEvent, WARNING, Send msg fail!\r\n");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 Ppp_SndPPPDisconnIndtoAT(VOS_UINT16 usPppId)
{
    AT_PPP_PROTOCOL_REL_IND_MSG_STRU    *pstMsg;
    VOS_UINT32                           ulLength;


    /* 可维可测信息上报*/
    Ppp_EventMntnInfo(usPppId, PPP_PROTOCOL_RELEASED_IND);

    if(PPP_MAX_ID_NUM < usPppId)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,"Ppp_SndPPPDisconnIndtoAT usPppId Wrong!\r\n");
        return VOS_ERR;
    }

    /*向AT模块发送AT_PPP_RELEASE_IND_MSG*/
    ulLength    = sizeof(AT_PPP_PROTOCOL_REL_IND_MSG_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (AT_PPP_PROTOCOL_REL_IND_MSG_STRU *)PS_ALLOC_MSG(PS_PID_APP_PPP, ulLength);
    if (VOS_NULL_PTR == pstMsg)
    {
        /*打印出错信息---申请消息包失败:*/
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,"Ppp_SndPPPDisconnIndtoAT : PS_ALLOC_MSG Failed!\r\n");
        return VOS_ERR;
    }
    /*填写消息头:*/
    pstMsg->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid     = PS_PID_APP_PPP;
    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = WUEPS_PID_AT;
    pstMsg->MsgHeader.ulLength        = ulLength;

    pstMsg->MsgHeader.ulMsgName       = AT_PPP_PROTOCOL_REL_IND_MSG;
    /*填写消息体:*/
    pstMsg->usPppId                   = usPppId;

    /*发送该消息*/
    if (VOS_OK != PS_SEND_MSG(PS_PID_APP_PPP, pstMsg))
    {
        /*打印警告信息---发送消息失败:*/
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,"Ppp_SndPPPDisconnIndtoAT : PS_SEND_MSG Failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 PPP_SavePcoInfo
(
    PPP_ID usPppId,
    AT_PPP_IND_CONFIG_INFO_STRU *pstAtPppIndConfigInfo
)
{
    struct ipcp                         *pstIpcp;
    AT_PPP_PCO_IPV4_ITEM_STRU           *pstPcoIpv4Item;


    pstIpcp        = &(PPP_LINK(usPppId)->ipcp);
    pstPcoIpv4Item = &(pstAtPppIndConfigInfo->stPcoIpv4Item);


    PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
                  "PPP_SavePcoInfo, aucIpAddr %d\r\n",
                  (VOS_INT)pstAtPppIndConfigInfo->aucIpAddr);
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
                  "PPP_SavePcoInfo, bitOpPriDns %d, aucPriDns %d\r\n",
                  (VOS_INT)pstPcoIpv4Item->bitOpPriDns, (VOS_INT)pstPcoIpv4Item->aucPriDns);
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
                  "PPP_SavePcoInfo, bitOpSecDns %d, aucSecDns %d\r\n",
                  (VOS_INT)pstPcoIpv4Item->bitOpSecDns, (VOS_INT)pstPcoIpv4Item->aucSecDns);
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
                  "PPP_SavePcoInfo, bitOpPriNbns %d, aucPriNbns %d\r\n",
                  (VOS_INT)pstPcoIpv4Item->bitOpPriNbns, (VOS_INT)pstPcoIpv4Item->aucPriNbns);
    PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
                  "PPP_SavePcoInfo, bitOpSecNbns %d, aucSecNbns %d\r\n",
                  (VOS_INT)pstPcoIpv4Item->bitOpSecNbns, (VOS_INT)pstPcoIpv4Item->aucSecNbns);

    /* 保存主DNS服务器地址 */
    if ( pstPcoIpv4Item->bitOpPriDns )
    {
        PS_MEM_CPY(&(pstIpcp->PriDnsAddr.s_addr), pstPcoIpv4Item->aucPriDns, IPV4_ADDR_LEN);
        pstIpcp->PriDns_neg |= NEG_ACCEPTED;
    }

    /* 保存辅DNS服务器地址 */
    if ( pstPcoIpv4Item->bitOpSecDns )
    {
        PS_MEM_CPY(&(pstIpcp->SecDnsAddr.s_addr), pstPcoIpv4Item->aucSecDns, IPV4_ADDR_LEN);
        pstIpcp->SecDns_neg |= NEG_ACCEPTED;
    }

    /* 保存主NBNS服务器地址 */
    if ( (pstPcoIpv4Item->bitOpPriNbns)
         && (WINS_CONFIG_ENABLE == g_ucPppConfigWins))
    {
        PS_MEM_CPY(&(pstIpcp->PriNbnsAddr.s_addr), pstPcoIpv4Item->aucPriNbns, IPV4_ADDR_LEN);
        pstIpcp->PriNbns_neg |= NEG_ACCEPTED;
    }

    /* 保存辅NBNS服务器地址 */
    if ( (pstPcoIpv4Item->bitOpSecNbns)
         && (WINS_CONFIG_ENABLE == g_ucPppConfigWins))
    {
        PS_MEM_CPY(&(pstIpcp->SecNbnsAddr.s_addr), pstPcoIpv4Item->aucSecNbns, IPV4_ADDR_LEN);
        pstIpcp->SecNbns_neg |= NEG_ACCEPTED;
    }

    /* 参考Ppp_RcvConfigInfoInd实现，peer_ip填主机地址aucIpAddr */
    PS_MEM_CPY(&(pstIpcp->peer_ip.s_addr), pstAtPppIndConfigInfo->aucIpAddr, IPV4_ADDR_LEN);
    pstIpcp->IpAddr_neg |= NEG_ACCEPTED;

    /* 切换IPCP协商状态 */
    if(pstIpcp->stage == IPCP_REQ_RECEIVED)
    {
        pstIpcp->stage = IPCP_SUCCESS_FROM_GGSN;
    }

    return VOS_OK;
}


VOS_VOID PPP_GetReqConfigInfo
(
    PPP_REQ_CONFIG_INFO_STRU    *pstPppReqConfigInfo,
    AT_PPP_REQ_CONFIG_INFO_STRU *pstPppAtReqConfigInfo
)
{
    PPP_AUTH_PAP_CONTENT_STRU           *pstSrcPapContent;
    PPP_AUTH_CHAP_CONTENT_STRU          *pstSrcChapContent;
    AT_PPP_AUTH_PAP_CONTENT_STRU        *pstDestPapContent;
    AT_PPP_AUTH_CHAP_CONTENT_STRU       *pstDestChapContent;


    pstPppAtReqConfigInfo->stAuth.ucAuthType = pstPppReqConfigInfo->stAuth.ucAuthType;

    /* 获取PPP_AT_AUTH_CHAP_CONTENT_STRU */
    if ( PPP_CHAP_AUTH_TYPE == pstPppReqConfigInfo->stAuth.ucAuthType )
    {
        pstDestChapContent = &(pstPppAtReqConfigInfo->stAuth.AuthContent.ChapContent);
        pstSrcChapContent  = &(pstPppReqConfigInfo->stAuth.AuthContent.ChapContent);

        pstDestChapContent->usChapChallengeLen = pstSrcChapContent->usChapChallengeLen;
        pstDestChapContent->usChapResponseLen  = pstSrcChapContent->usChapResponseLen;

        PS_MEM_CPY(pstDestChapContent->aucChapChallengeBuf, pstSrcChapContent->pChapChallenge,
                   pstSrcChapContent->usChapChallengeLen);

        PS_MEM_CPY(pstDestChapContent->aucChapResponseBuf, pstSrcChapContent->pChapResponse,
                   pstSrcChapContent->usChapResponseLen);
    }
    /* 获取PPP_AT_AUTH_PAP_CONTENT_STRU */
    else if ( PPP_PAP_AUTH_TYPE == pstPppReqConfigInfo->stAuth.ucAuthType )
    {
        pstDestPapContent = &(pstPppAtReqConfigInfo->stAuth.AuthContent.PapContent);
        pstSrcPapContent  = &(pstPppReqConfigInfo->stAuth.AuthContent.PapContent);

        pstDestPapContent->usPapReqLen = pstSrcPapContent->usPapReqLen;

        PS_MEM_CPY(pstDestPapContent->aucPapReqBuf, pstSrcPapContent->pPapReq,
                   pstSrcPapContent->usPapReqLen);
    }
    else
    {
        /* PPP_NO_AUTH_TYPE */
    }

    /* 获取PPP_AT_REQ_IPCP_CONFIG_INFO_STRU */
    pstPppAtReqConfigInfo->stIPCP.usIpcpLen = pstPppReqConfigInfo->stIPCP.usIpcpLen;

    PS_MEM_CPY(pstPppAtReqConfigInfo->stIPCP.aucIpcp,
               pstPppReqConfigInfo->stIPCP.pIpcp,
               pstPppReqConfigInfo->stIPCP.usIpcpLen);

    return;
}

/*****************************************************************************
 Prototype      : Ppp_RegDlDataCallback
 Description    : 为AT模块提供注册下行发送数据的API

 Input          : usPppId---要发指示的PPP链路所在的PPP ID
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2013-06-04
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_RegDlDataCallback(PPP_ID usPppId)
{
    VOS_UINT8                               ucRabId;
    VOS_UINT32                              ulResult;


    if(VOS_OK != PppIsIdValid(usPppId))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP, Ppp_RegDlDataCallback, WARNING, Invalid PPP Id %d",
                      usPppId);
        return VOS_ERR;
    }


    /* 通过usPppId，寻找到usRabId */
    if (!PPP_PPPID_TO_RAB(usPppId, &ucRabId))
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                      "PPP, Ppp_RegDlDataCallback, WARNING, Can not get PPP Id %d, RabId %d",
                      usPppId, ucRabId);

        return VOS_ERR;
    }

    /* 这个时候PDP已经激活，注册上行数据接收接口 */
    ulResult= ADS_DL_RegDlDataCallback(ucRabId, (RCV_DL_DATA_FUNC)PPP_PushRawDataEvent);

    if ( VOS_OK != ulResult )
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP, Ppp_RegDlDataCallback, WARNING, Register DL CB failed! RabId %d",
                      ucRabId);

        return VOS_ERR;
    }

    return VOS_OK;
}

#else    /* for feature */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AtPppInterface.h"

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 Prototype      : Ppp_CreatePppReq
 Description    : 为AT模块"创建PPP链路"提供对应的API函数。

 Input          : ---
 Output         : ---创建成功后返回的PPP ID
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_CreatePppReq ( VOS_UINT16 *pusPppId)
{
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : Ppp_CreateRawDataPppReq
 Description    : 创建PDP类型为PPP的PPP实体，但不做链路管理，只作数据的封装和解封装

 Input          : ---
 Output         : ---创建成功后返回的PPP ID
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_CreateRawDataPppReq ( VOS_UINT16 *pusPppId)
{
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : Ppp_RcvConfigInfoInd
 Description    : 为AT模块"PPP模块接收网侧指示的配置信息"提供对应的API函数。
                  当AT向GGSN认证成功后，调用此函数向PPP发指示。

 Input          : usPppId---要发指示的PPP链路所在的PPP ID
                  pPppIndConfigInfo---从GGSN发来的该PPP链路的IP地址等信息
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_RcvConfigInfoInd
(
    PPP_ID usPppId,
    AT_PPP_IND_CONFIG_INFO_STRU         *pstAtPppIndConfigInfo
)
{
    return VOS_OK;
}



VOS_UINT32 PPP_RcvAtCtrlOperEvent(VOS_UINT16 usPppId, VOS_UINT32 ulCtrlOperType)
{
    return VOS_OK;
}

/*****************************************************************************
 Prototype      : Ppp_RegDlDataCallback
 Description    : 为AT模块提供注册下行发送数据的API

 Input          : usPppId---要发指示的PPP链路所在的PPP ID
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2013-06-04
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_RegDlDataCallback(PPP_ID usPppId)
{
    return VOS_OK;
}
#endif /* #if(FEATURE_ON == FEATURE_PPP) */

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

