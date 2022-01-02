
#ifdef  __cplusplus
  #if  __cplusplus
       extern "C"{
  #endif
#endif

#include "product_config.h"
#if(FEATURE_ON == FEATURE_PPP)
/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "ppp_public.h"
#include "layer.h"
#include "ppp_mbuf.h"
#include "hdlc.h"
#include "throughput.h"
#include "proto.h"
#include "ppp_fsm.h"
#include "lcp.h"
#include "async.h"
#include "auth.h"
#include "ipcp.h"
#include "pppid.h"
#include "link.h"
#include "ppp_input.h"
#include "AdsDeviceInterface.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_PPP_INPUT_C

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
extern VOS_VOID   PPP_ClearDataQ(VOS_VOID);
extern VOS_VOID   Ppp_ProcConfigInfoInd(VOS_UINT16 usPppId);

/******************************************************************************
   3 私有定义
******************************************************************************/
#define PPP_ASYNC(PppId)  (PPP_LINK(PppId)->async)

/******************************************
向指针指向内存中放置，并更新该指针地址
注意:该指针必须是(u)char *型
*******************************************/
#define PPP_PUTCHAR(c, cp) {     \
	*(cp) = (VOS_UINT8)(c);     \
	(cp) ++;                     \
}

#define PPP_PUTSHORT(s, cp) {    \
    *((VOS_UINT16 *)(cp)) = (s); \
    (cp) += 2;                   \
}

#define PPP_INCPTR(len, cp) {   \
    (cp) += (len);              \
}

#define PPP_LENALIGNTO2BYTE(alignedlen, len){                 \
    (alignedlen) = ((((len) % 2) == 0) ? (len) : (len + 1));\
}

/******************************************************************************
   4 全局变量定义
******************************************************************************/
/*PPP的数据队列结构体,上下行数据都在同一个队列中*/
PPP_DATA_Q_CTRL_ST     g_PppDataQCtrl;


/******************************************************************************
   5 函数实现
******************************************************************************/

VOS_VOID PPP_INPUT_ResetStatInfo(VOS_VOID)
{
    VOS_MemSet(&(g_PppDataQCtrl.stStat), 0, sizeof(PPP_DATA_Q_STAT_ST));

    return;
} /* PPP_ResetDataQStatInfo */

/******************************************************************************
 Function:       PPP_GetDataCnt
 Description:    获取数据队列的数据包个数
 Calls:
 Data Accessed:
 Data Updated:
 Input:
 Output:
 Return:

 Others:
******************************************************************************/
VOS_UINT32 PPP_INPUT_GetDataCnt(VOS_VOID)
{
    VOS_UINT32  ulUlDataQCnt;

    ulUlDataQCnt    = PPP_ZC_GET_QUEUE_LEN(&g_PppDataQCtrl.stDataQ);

    return ulUlDataQCnt;
}


VOS_VOID    PPP_INPUT_ShowStatInfo(VOS_VOID)
{
    vos_printf("\n================PPP STAT INFO Begin==========================\n");

    vos_printf("队列中当前结点个数         = %d\n", PPP_INPUT_GetDataCnt());
    vos_printf("DataNotify消息发送次数     = %d\n", g_PppDataQCtrl.stStat.ulSndMsgCnt);
    vos_printf("DataNotify消息处理次数     = %d\n", g_PppDataQCtrl.stStat.ulProcMsgCnt);
    vos_printf("队列中出现过的最大节点个数 = %d\n", g_PppDataQCtrl.stStat.ulQMaxCnt);
    vos_printf("一次最多允许处理的节点个数 = %d\n", PPP_ONCE_DEAL_MAX_CNT);

    vos_printf("上行内存申请成功次数       = %d\n", g_PppDataQCtrl.stStat.ulMemAllocUplinkCnt);
    vos_printf("上行内存申请失败次数       = %d\n", g_PppDataQCtrl.stStat.ulMemAllocUplinkFailCnt);

    vos_printf("下行内存申请成功次数       = %d\n", g_PppDataQCtrl.stStat.ulMemAllocDownlinkCnt);
    vos_printf("下行内存申请失败次数       = %d\n", g_PppDataQCtrl.stStat.ulMemAllocDownlinkFailCnt);
    vos_printf("内存释放次数               = %d\n", g_PppDataQCtrl.stStat.ulMemFreeCnt);

    vos_printf("上行数据包总个数           = %d\n", g_PppDataQCtrl.stStat.ulUplinkCnt);
    vos_printf("上行丢包数                 = %d\n", g_PppDataQCtrl.stStat.ulUplinkDropCnt);
    vos_printf("上行发包数                 = %d\n", g_PppDataQCtrl.stStat.ulUplinkSndDataCnt);

    vos_printf("下行数据包总个数           = %d\n", g_PppDataQCtrl.stStat.ulDownlinkCnt);
    vos_printf("下行丢包数                 = %d\n", g_PppDataQCtrl.stStat.ulDownlinkDropCnt);
    vos_printf("下行发包数                 = %d\n", g_PppDataQCtrl.stStat.ulDownlinkSndDataCnt);
    vos_printf("ulNotifyMsgCnt             = %d\n", g_PppDataQCtrl.ulNotifyMsgCnt);

    vos_printf("================PPP STAT INFO End==========================\n");
}


VOS_UINT32  PPP_Snd1stDataNotify(VOS_VOID)
{
    PPP_DATA_PROC_NOTIFY_MSG    *pMsg;
    VOS_INT32                    lLockKey;


    /*申请消息内存:*/
    pMsg = (PPP_DATA_PROC_NOTIFY_MSG *) PS_ALLOC_MSG( PS_PID_APP_PPP,
        sizeof(PPP_DATA_PROC_NOTIFY_MSG) - VOS_MSG_HEAD_LENGTH );

    if (VOS_NULL_PTR == pMsg)
    {
        /*打印出错信息---申请消息包失败:*/
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_Snd1stDataNotify:WARNING:Allocates message for PPP_DATA_PROC_NOTIFY FAIL!\r\n" );
        return PS_FAIL;
    }

    /*填写消息内容:*/
    pMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pMsg->ulReceiverPid   = PS_PID_APP_PPP;
    pMsg->ulMsgType       = PPP_DATA_PROC_NOTIFY;

    lLockKey = VOS_SplIMP();
    g_PppDataQCtrl.ulNotifyMsgCnt++;
    VOS_Splx(lLockKey);

    /*发送消息:*/
    if (VOS_OK != PS_SEND_MSG(PS_PID_APP_PPP, pMsg))
    {
        /*打印警告信息---发送消息失败:*/
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "SEND PPP_DATA_PROC_NOTIFY msg FAIL!\r\n" );
        return PS_FAIL;
    }

    return PS_SUCC;
} /* PPP_Snd1stDataNotify */



VOS_UINT32  PPP_EnqueueData(PPP_ZC_STRU *pstImmZc)
{
    VOS_UINT32                          ulNonEmptyEvent = PS_FALSE;    /* 记录队列是否发生了由空到非空的转变 */
    PPP_ZC_QUEUE_STRU                   *pstDataQ;
    VOS_INT32                           lLockKey;


    pstDataQ   = &g_PppDataQCtrl.stDataQ;

    /* A核任务调试不够实时，之前获取长度和入列分开锁中断，会出现上行数据入队时
       判断队列不为空，但紧接着PPP任务得到调度，把队列取空后再接着入队的情况。
       因为将获取队列数据个数和入队包含在一个锁中断中 */
    lLockKey = VOS_SplIMP();

    if ( (0 == PPP_ZC_GET_QUEUE_LEN(pstDataQ)) && (0 == g_PppDataQCtrl.ulNotifyMsgCnt))
    {
        ulNonEmptyEvent = PS_TRUE;
    }

    /*将数据结点插入队列尾部*/
    PPP_ZC_ENQUEUE_TAIL(pstDataQ, pstImmZc);
    VOS_Splx(lLockKey);

    if (PPP_ZC_GET_QUEUE_LEN(pstDataQ) > g_PppDataQCtrl.stStat.ulQMaxCnt)
    {
        g_PppDataQCtrl.stStat.ulQMaxCnt = PPP_ZC_GET_QUEUE_LEN(pstDataQ);
    }

    if (PS_TRUE == ulNonEmptyEvent)
    {
        /*向PPP发送数据处理指示*/
       if (PS_SUCC != PPP_Snd1stDataNotify())
       {
            /* 发送消息通知失败，需要清空整个队列 */
            PPP_ClearDataQ();
            return PS_FAIL;
       }

       g_PppDataQCtrl.stStat.ulSndMsgCnt++;
    }

    return PS_SUCC;
} /* PPP_EnqueueData */



VOS_UINT32 PPP_PullPacketEvent(VOS_UINT16 usPppId, PPP_ZC_STRU *pstImmZc)
{
    if(VOS_NULL_PTR == pstImmZc)
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PullPacketEvent, WARNING, pstImmZc is NULL!\r\n" );

        return PS_FAIL;
    }

    /* 参考V3R1实现，入口处不检查PPP ID对应实体是否存在，这样在网侧断开(此时PPP ID已经释放)，
    　　也能接收PC发来的IPCP协商包 */
    if((PPP_MAX_ID_NUM < usPppId)
        || (0 == usPppId))
    {
        g_PppDataQCtrl.stStat.ulUplinkDropCnt++;
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                     "PPP, PPP_PullPacketEvent, NORMAL, usPppId %d Wrong\r\n", usPppId);
        PPP_MemFree(pstImmZc);

        return PS_FAIL;
    }

    /*填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型*/
    PPP_ZC_SET_DATA_APP(pstImmZc, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PULL_PACKET_TYPE);

    if ( PS_SUCC != PPP_EnqueueData(pstImmZc) )
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PullPacketEvent, WARNING, Enqueue Data Fail!\r\n" );

        return PS_FAIL;
    }

    g_PppDataQCtrl.stStat.ulUplinkCnt++;

    return PS_SUCC;
} /* PPP_PullPacketEvent */


VOS_UINT32 PPP_PushPacketEvent(VOS_UINT8 ucRabId, PPP_ZC_STRU *pstImmZc, ADS_PKT_TYPE_ENUM_UINT8 enPktType)
{
    PPP_ID                              usPppId = PPP_INVLAID_PPP_ID;

    if(VOS_NULL_PTR == pstImmZc)
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PushPacketEvent, WARNING, pstImmZc is NULL!\r\n" );

        return PS_FAIL;
    }

    /* 通过RabId，寻找到PPP ID和相应的实体 */
#ifndef WTTF_PS_FUSION_PC_ST
    if ( !PPP_RAB_TO_PPPID(&usPppId, ucRabId) )
    {
        g_PppDataQCtrl.stStat.ulDownlinkDropCnt++;
        PPP_MemFree(pstImmZc);
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                      "PPP, PPP_PushPacketEvent, NORMAL, Can not get PPP Id, RabId <1>", ucRabId);

        return PS_FAIL;
    }

    /*如果该链接还没建立起来*/
    if((VOS_OK != PppIsIdValid(usPppId))
        || (PPP_LINK(usPppId)->phase != PHASE_NETWORK)
        || (PPP_LINK(usPppId)->ipcp.fsm.state != ST_OPENED))
    {
        /*该变量需要被初始化为0*/
        g_PppDataQCtrl.stStat.ulDownlinkDropCnt++;
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                      "PPP, PPP_PushPacket, WARNING, packet from GGSN droped, packet num = <1>\r\n",
                      (VOS_INT32)g_PppDataQCtrl.stStat.ulDownlinkDropCnt);

        PPP_MemFree(pstImmZc);

        return PS_FAIL;
    }
#endif

#ifdef WTTF_PS_FUSION_PC_ST
    usPppId = 1;/*PC上没有使用PPP拨号，打桩
 */
#endif
    /*填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型*/
    PPP_ZC_SET_DATA_APP(pstImmZc, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PUSH_PACKET_TYPE);

    if ( PS_SUCC != PPP_EnqueueData(pstImmZc) )
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PushPacketEvent, WARNING, Enqueue Data Fail!\r\n" );

        return PS_FAIL;
    }

    g_PppDataQCtrl.stStat.ulDownlinkCnt++;

    return PS_SUCC;
}
VOS_UINT32 PPP_PullRawDataEvent(VOS_UINT16 usPppId, PPP_ZC_STRU *pstImmZc)
{
    if(VOS_NULL_PTR == pstImmZc)
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PullRawDataEvent, WARNING, Alloc TTF mem fail!\r\n" );

        return PS_FAIL;
    }

    if((PPP_MAX_ID_NUM < usPppId)
        || (0 == usPppId))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "PPP, PPP_PullRawDataEvent, WARNING, usPppId %d Wrong\r\n", usPppId);
        PPP_MemFree(pstImmZc);

        return PS_FAIL;
    }

    /*填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型*/
    PPP_ZC_SET_DATA_APP(pstImmZc, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PULL_RAW_DATA_TYPE);

    if ( PS_SUCC != PPP_EnqueueData(pstImmZc) )
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PullRawDataEvent, WARNING, Enqueue Data Fail!\r\n" );

        return PS_FAIL;
    }

    g_PppDataQCtrl.stStat.ulUplinkCnt++;

    return PS_SUCC;
} /* PPP_PullRawEvent */


VOS_UINT32 PPP_PushRawDataEvent(VOS_UINT8 ucRabId, PPP_ZC_STRU *pstImmZc, ADS_PKT_TYPE_ENUM_UINT8 enPktType)
{
    PPP_ID                              usPppId = PPP_INVLAID_PPP_ID;

    if(VOS_NULL_PTR == pstImmZc)
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PushRawDataEvent, WARNING, pstImmZc is NULL!\r\n" );

        return PS_FAIL;
    }

    /* 通过RabId，寻找到PPP ID和相应的实体 */
    if ( !PPP_RAB_TO_PPPID(&usPppId, ucRabId) )
    {
        PPP_MemFree(pstImmZc);
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP, PPP_PushRawDataEvent, WARNING, Can not get PPP Id, RabId <1>", ucRabId);

        return PS_FAIL;
    }

    if(VOS_OK != PppIsIdValid(usPppId))
    {
        PPP_MemFree(pstImmZc);
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "PPP_PushRawData, WARNING, Invalid PPP id, packet from GGSN droped\r\n");

        return PS_FAIL;
    }

    /*填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型*/
    PPP_ZC_SET_DATA_APP(pstImmZc, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PUSH_RAW_DATA_TYPE);

    if ( PS_SUCC != PPP_EnqueueData(pstImmZc) )
    {
        PPP_MNTN_LOG( PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_PushRawDataEvent, WARNING, Enqueue Data Fail!\r\n" );
        return PS_FAIL;
    }

    g_PppDataQCtrl.stStat.ulDownlinkCnt++;

    return PS_SUCC;
}



VOS_VOID  PPP_ProcDataNotify(VOS_VOID)
{
    PPP_ZC_STRU                        *pstMem;
    PPP_ID                              usPppId;
    PPP_HDLC_CONFIG_STRU               *pstHdlcConfig;
    PPP_HDLC_RESULT_TYPE_ENUM_UINT32    ulResult;
    VOS_INT32                           lLockKey;


    g_PppDataQCtrl.stStat.ulProcMsgCnt++;
    lLockKey = VOS_SplIMP();
    g_PppDataQCtrl.ulNotifyMsgCnt--;
    VOS_Splx(lLockKey);

    pstMem  = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(&g_PppDataQCtrl.stDataQ);

    /* 队列为空的时候返回空指针 */
    if ( VOS_NULL_PTR == pstMem )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, LOG_LEVEL_NORMAL,
                      "PPP_ProcDataNotify, NORMAL, Data queue is empty!\r\n");
        return;
    }

    /* 处理该结点(结点的释放动作已经在各处理函数内部完成，无需再释放结点) */
    usPppId = (PPP_ZC_GET_DATA_APP(pstMem) & 0xFF00) >> 8;

    if ((usPppId == 0) || (PPP_MAX_ID_NUM < usPppId))
    {
        PPP_MemFree(pstMem);
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                      "PPP_ProcDataNotify, WARNING, usPppID %d is invalid!", usPppId);
        return;
    }

    pstHdlcConfig = PPP_CONFIG(usPppId);

    if (VOS_NULL_PTR == pstHdlcConfig->pFunProcData)
    {
        PPP_MemFree(pstMem);
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, LOG_LEVEL_ERROR,
                      "PPP_ProcDataNotify, ERROR, pstHdlcConfig->pFunProcData is NULL!");
        return;
    }

    ulResult = pstHdlcConfig->pFunProcData(usPppId, PPP_LINK(usPppId), &g_PppDataQCtrl.stDataQ);

    if ((PPP_HDLC_RESULT_COMM_CONTINUE == ulResult)&&(0 == g_PppDataQCtrl.ulNotifyMsgCnt))
    {
        PPP_Snd1stDataNotify();
    }

    return;
} /* PPP_ProcDataNotify */

VOS_VOID PPP_ProcAsFrmDataInd(struct MsgCB * pMsg)
{
    HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU    *pstHdlcEnable;
    PPP_HDLC_CONFIG_STRU                    *pstHdlcConfig;
    VOS_UINT16                               usPppId;
    VOS_UINT16                               usProtocol;
    PPP_ZC_STRU                             *pstMem;


    pstHdlcEnable = (HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU*)pMsg;

    usPppId       = pstHdlcEnable->usPppId;
    usProtocol    = pstHdlcEnable->usProtocol;
    pstMem        = pstHdlcEnable->pstMem;

    pstHdlcConfig = PPP_CONFIG(usPppId);

    if (VOS_NULL_PTR == pstHdlcConfig->pFunProcAsFrmData)
    {
        PPP_MemFree(pstMem);
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, LOG_LEVEL_ERROR,
                      "PPP_ProcHdlcEnable, ERROR, pstHdlcConfig->pFunProcAsFrmData is NULL!");
        return;
    }

    pstHdlcConfig->pFunProcAsFrmData(usPppId, usProtocol, pstMem);

    return;
}

/*****************************************************************************
 Prototype      : PPP_ProcHdlcDisable
 Description    : 处理HDLC去使能请求

 Input          : ---PPP链路对应的PPP ID
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_VOID PPP_ProcHdlcDisable ( PPP_ID usPppId)
{
    PPP_HDLC_CONFIG_STRU               *pstHdlcConfig;


    if ((usPppId == 0) || (PPP_MAX_ID_NUM < usPppId))
    {
        PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                      "PPP_ProcHdlcDisable, WARNING, usPppID %d is invalid!", usPppId);
        return;
    }

    pstHdlcConfig = PPP_CONFIG(usPppId);

    if (VOS_NULL_PTR != pstHdlcConfig->pFunDisable)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, LOG_LEVEL_INFO,
                      "PPP_ProcHdlcDisable, INFO, Invoke HDLC disable function!");

        pstHdlcConfig->pFunDisable();
    }

    return;
}


VOS_UINT32 PPP_ProcAtCtrlOper(struct MsgCB * pMsg)
{
    PPP_AT_CTRL_OPERATION_MSG *pCtrlOperMsg;

    pCtrlOperMsg = (PPP_AT_CTRL_OPERATION_MSG *)pMsg;

    switch(pCtrlOperMsg->ulCtrlOpType)
    {
        case PPP_AT_CTRL_REL_PPP_REQ:
            Ppp_ReleasePppReq(pCtrlOperMsg->usPppId);
            break;
        case PPP_AT_CTRL_REL_PPP_RAW_REQ:
            Ppp_ReleaseRawDataPppReq(pCtrlOperMsg->usPppId);
            break;
        case PPP_AT_CTRL_HDLC_DISABLE:
            PPP_ProcHdlcDisable(pCtrlOperMsg->usPppId);
            break;
        case PPP_AT_CTRL_CONFIG_INFO_IND:
            Ppp_ProcConfigInfoInd(pCtrlOperMsg->usPppId);
         break;

        default:
            PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                          "PPP, PPP_ProcAtCtrlOper, ulCtrlOpType %d is ERROR!",
                          (VOS_INT32)(pCtrlOperMsg->ulCtrlOpType));
            return PS_FAIL;
    }

    return PS_SUCC;
}


/*****************************************************************************
    PPP勾包原始内存中数据的结构
    -PPP_Frame_MNTN_Info_STRU    struct
    -PPP_Proto                   2 byte NetWork Order
    -PPP_Frame                   (without PPP Proto)
*****************************************************************************/
VOS_VOID Ppp_MBufFrameMntnInfo
(
    struct ppp_mbuf *bp,
    VOS_UINT16       usProto,
    VOS_UINT32       ulDir
)
{
    VOS_UINT16                ulFrameLen        = 0;
    PPP_FRAME_MNTN_INFO_STRU *ptrPppFrameMntnSt = VOS_NULL;
    VOS_UINT8                *pucBuff           = VOS_NULL;
    VOS_INT32                 ulRet             = VOS_ERR;


    /* PPP 帧长度*/
    ulFrameLen = (VOS_UINT16)ppp_m_length(bp);

    ptrPppFrameMntnSt = (PPP_FRAME_MNTN_INFO_STRU *)PS_MEM_ALLOC(PS_PID_APP_PPP,
                        ulFrameLen + sizeof(PPP_FRAME_MNTN_INFO_STRU) + sizeof(usProto));
    if (VOS_NULL_PTR == ptrPppFrameMntnSt)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "PPP, Ppp_MBufFrameMntnInfo, ERROR, Call VOS_MemAlloc fail!\n");
        return;
    }

    /* 填入PPP帧协议类型,使用网络字节序*/
    pucBuff = (VOS_UINT8 *)(ptrPppFrameMntnSt + 1);
    PPP_PUTSHORT(VOS_HTONS(usProto), pucBuff);

    /* 填入帧内容,原始复制即可*/
    ulRet = ppp_mbuf_View(bp, pucBuff, ulFrameLen);
    if (ulFrameLen != ulRet)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "PPP, Ppp_MBufFrameMntnInfo, ERROR, ppp_mbuf_View Fail!\n");
        PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);
        return ;
    }

    /****************************************************************
       发送可维可测信息
       长度: 除可维可测信息头部外数据部分载荷
             涵盖帧长 + 2byte protocol字段
    *****************************************************************/
    Ppp_FrameMntnInfo(ptrPppFrameMntnSt, ulDir, ulFrameLen + sizeof(usProto));

    /* 释放内存*/
    PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);
    return;
}
VOS_VOID Ppp_TtfMemFrameMntnInfo
(
    PPP_ZC_STRU *pstMem,
    VOS_UINT16  usProto,
    VOS_UINT32  ulDir
)
{
    VOS_UINT16                ulFrameLen        = 0;
    PPP_FRAME_MNTN_INFO_STRU *ptrPppFrameMntnSt = VOS_NULL;
    VOS_UINT8                *pucBuff           = VOS_NULL;
    VOS_UINT32                ulRet             = PS_FAIL;

    /* PPP 帧长度*/
    ulFrameLen = (VOS_UINT16)PPP_ZC_GET_DATA_LEN(pstMem);

    ptrPppFrameMntnSt = (PPP_FRAME_MNTN_INFO_STRU *)PS_MEM_ALLOC(PS_PID_APP_PPP,
                        ulFrameLen + sizeof(PPP_FRAME_MNTN_INFO_STRU) + sizeof(usProto));
    if (VOS_NULL_PTR == ptrPppFrameMntnSt)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "PPP, Ppp_TtfMemFrameMntnInfo, ERROR, Call VOS_MemAlloc fail!\n");
        return;
    }

    /* 填入PPP帧协议类型,网络字节序*/
    pucBuff = (VOS_UINT8 *)(ptrPppFrameMntnSt + 1);
    PPP_PUTSHORT(VOS_HTONS(usProto), pucBuff);

    /* 填入帧内容,原始复制即可*/
    ulRet = PPP_MemGet(pstMem, 0, pucBuff, ulFrameLen);
    if (PS_SUCC != ulRet)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
                     "PPP, Ppp_frame_MntnInfo, ERROR, TTF_MemGet Fail!\n");
        PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);

        return ;
    }

    /****************************************************************
       发送可维可测信息
       长度: 除可维可测信息头部外数据部分载荷
             涵盖帧长 + 2byte protocol字段
    *****************************************************************/
    Ppp_FrameMntnInfo(ptrPppFrameMntnSt, ulDir, ulFrameLen + sizeof(usProto));

    /* 释放内存*/
    PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);

    return;
}
VOS_VOID Ppp_FrameMntnInfo
(
    PPP_FRAME_MNTN_INFO_STRU *ptrPppMntnSt,
    VOS_UINT32                ulDir,
    VOS_UINT16                ulDataLen
)
{
    ptrPppMntnSt->ulReceiverCpuId = VOS_LOCAL_CPUID;
    ptrPppMntnSt->ulReceiverPid   = PS_PID_APP_PPP;
    ptrPppMntnSt->ulSenderCpuId   = VOS_LOCAL_CPUID;
    ptrPppMntnSt->ulSenderPid     = PS_PID_APP_PPP;
    ptrPppMntnSt->ulLength        = (ulDataLen + sizeof(PPP_FRAME_MNTN_INFO_STRU))
                                        - VOS_MSG_HEAD_LENGTH;
    /* PPP 帧方向*/
    if (PPP_RECV_IN_PROTOCOL_FRAME == ulDir)
    {
        ptrPppMntnSt->ulMsgname = PPP_RECV_PROTO_PACKET_TYPE;
    }
    if (PPP_SEND_OUT_PROTOCOL_FRAME == ulDir)
    {
        ptrPppMntnSt->ulMsgname = PPP_SEND_PROTO_PACKET_TYPE;
    }

    ptrPppMntnSt->ulPppPhase  = pgPppLink->phase;
    ptrPppMntnSt->ulIpcpState = pgPppLink->ipcp.fsm.state;
    ptrPppMntnSt->ulLcpState  = pgPppLink->lcp.fsm.state;
    ptrPppMntnSt->usPppId     = (VOS_UINT16)(PPP_LINK_TO_ID(pgPppLink));
    ptrPppMntnSt->ulDataLen  = ulDataLen;

    PPP_MNTN_TRACE_MSG(ptrPppMntnSt);

    return;
}
VOS_VOID Ppp_FillEventMntnInfo
(
    PPP_EVENT_MNTN_INFO_STRU  *ptrPppEveMntnSt,
    VOS_UINT16                 usPppID,
    VOS_UINT32                 ulEvent,
    VOS_UINT32                 ulEventLen
)
{
    ptrPppEveMntnSt->ulReceiverCpuId = VOS_LOCAL_CPUID;
    ptrPppEveMntnSt->ulReceiverPid   = PS_PID_APP_PPP;
    ptrPppEveMntnSt->ulSenderCpuId   = VOS_LOCAL_CPUID;
    ptrPppEveMntnSt->ulSenderPid     = PS_PID_APP_PPP;
    ptrPppEveMntnSt->ulLength        = ulEventLen - VOS_MSG_HEAD_LENGTH;

    ptrPppEveMntnSt->ulMsgname      = ulEvent;
    ptrPppEveMntnSt->usPppId        = usPppID;
    ptrPppEveMntnSt->usReserved     = 0;
    ptrPppEveMntnSt->ulPppPhase     = pgPppLink->phase;
    ptrPppEveMntnSt->ulIpcpState    = pgPppLink->ipcp.fsm.state;
    ptrPppEveMntnSt->ulLcpState     = pgPppLink->lcp.fsm.state;
}


VOS_VOID Ppp_EventMntnInfo
(
    VOS_UINT16                usPppID,
    VOS_UINT32                ulEvent
)
{
    PPP_EVENT_MNTN_INFO_STRU    stPppEveMntnSt;

    /*填入公共信息字段*/
    Ppp_FillEventMntnInfo(&stPppEveMntnSt, usPppID, ulEvent, sizeof(PPP_EVENT_MNTN_INFO_STRU));

    PPP_MNTN_TRACE_MSG(&stPppEveMntnSt);

    return;
}
/*****************************************************************************
 PPP_ind_config_info可维可测信息勾包原始内存中的数据结构
 需要确保LEN在2BYTE对齐
-PPP_Event_MNTN_Info_STRU   struct
-LEN   IP_ADDR_LEN             2 byte
-IP_ADDR                      16 byte(PPP_MAX_IPV4_ADDR_LEN + 1)
-LEN   AUTH_LEN                2 byte
-VALUE ...                  0..x byte
-LEN   IPCP_LEN                2 byte
-VALUE ...                  0..x byte
*****************************************************************************/
VOS_VOID Ppp_RcvConfigInfoIndMntnInfo
(
    VOS_UINT16  usPppID,
    AT_PPP_IND_CONFIG_INFO_STRU *ptrIndConfigInfo
)
{
    VOS_UINT16                          ulDataLen;
    PPP_EVENT_MNTN_INFO_STRU           *ptrPppFrameMntnSt = VOS_NULL;
    VOS_UINT8                          *pucBuff;

    /*************************************************************
    CONFIG info 部分数据长度
    总长度 = aucIpAddr + aucPriDns + aucSecDns + aucGateWay + aucPriNbns + aucSecNbns + 6个长度字段
    **************************************************************/
    ulDataLen = (IPV4_ADDR_LEN * 6) + (sizeof(VOS_UINT16) * 6) + sizeof(PPP_EVENT_MNTN_INFO_STRU);

    /* 申请内存,数据长度+事件消息上报头部长度*/
    ptrPppFrameMntnSt = (PPP_EVENT_MNTN_INFO_STRU *)PS_MEM_ALLOC(PS_PID_APP_PPP, ulDataLen);

    if (VOS_NULL_PTR == ptrPppFrameMntnSt)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
                     "PPP, Ppp_RcvConfigInfoIndMntnInfo, ERROR, Call VOS_MemAlloc fail!\n");
        return;
    }

    /* 偏移sizeof(PPP_EVENT_MNTN_INFO_STRU) */
    pucBuff = (VOS_UINT8 *)(ptrPppFrameMntnSt + 1);

    /*lint -e661 -e662 -e669*/
    PPP_PUTSHORT(IPV4_ADDR_LEN, pucBuff);
    PS_MEM_CPY(pucBuff, ptrIndConfigInfo->aucIpAddr, IPV4_ADDR_LEN);
    PPP_INCPTR(IPV4_ADDR_LEN, pucBuff);

    /* 填入 aucPriDns 长度和内容*/
    PPP_PUTSHORT(IPV4_ADDR_LEN, pucBuff);
    PS_MEM_CPY(pucBuff, ptrIndConfigInfo->stPcoIpv4Item.aucPriDns, IPV4_ADDR_LEN);
    PPP_INCPTR(IPV4_ADDR_LEN, pucBuff);

    /* 填入 aucSecDns 长度和内容*/
    PPP_PUTSHORT(IPV4_ADDR_LEN, pucBuff);
    PS_MEM_CPY(pucBuff, ptrIndConfigInfo->stPcoIpv4Item.aucSecDns, IPV4_ADDR_LEN);
    PPP_INCPTR(IPV4_ADDR_LEN, pucBuff);

    /* 填入 aucGateWay 长度和内容*/
    PPP_PUTSHORT(IPV4_ADDR_LEN, pucBuff);
    PS_MEM_CPY(pucBuff, ptrIndConfigInfo->stPcoIpv4Item.aucGateWay, IPV4_ADDR_LEN);
    PPP_INCPTR(IPV4_ADDR_LEN, pucBuff);

    /* 填入 aucPriNbns 长度和内容*/
    PPP_PUTSHORT(IPV4_ADDR_LEN, pucBuff);
    PS_MEM_CPY(pucBuff, ptrIndConfigInfo->stPcoIpv4Item.aucPriNbns, IPV4_ADDR_LEN);
    PPP_INCPTR(IPV4_ADDR_LEN, pucBuff);

    /* 填入 aucSecNbns 长度和内容*/
    PPP_PUTSHORT(IPV4_ADDR_LEN, pucBuff);
    PS_MEM_CPY(pucBuff, ptrIndConfigInfo->stPcoIpv4Item.aucSecNbns, IPV4_ADDR_LEN);
    PPP_INCPTR(IPV4_ADDR_LEN, pucBuff);
    /*lint +e661 +e662 +e669*/

    /* 填入公共信息字段*/
    Ppp_FillEventMntnInfo(ptrPppFrameMntnSt, usPppID, AT_PPP_RECV_CONFIG_INFO_IND, ulDataLen);

    PPP_MNTN_TRACE_MSG(ptrPppFrameMntnSt);

    PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);

    return;
}


/*****************************************************************************
 PPP_ind_config_info可维可测信息勾包原始内存中的数据结构

-PPP_Event_MNTN_Info_STRU   struct
-AUTH_TYPE                     1 byte
-Empty Aligned                 1 byte (1 byte填充，确保LEN在2byte对齐)
   (AUTH_TYPE == PAP)
     -LEN      PAP             2 byte
     -VALUE    ...          0..x byte
   (AUTH_TYPE == CHAP)
     -LEN      CHALLENGE       2 byte
     -VALUE    ...          0..x byte
     -LEN      RESPOSNE        2 byte
     -VALUE    ...          0..x byte
   (AUTH_TYPE == OTHER)
     -NULL
-IPCP LEN                      2 byte
-IPCP VAULE ...             0..x byte
*****************************************************************************/
VOS_VOID Ppp_RcvConfigInfoReqMntnInfo(VOS_UINT16  usPppID, PPP_REQ_CONFIG_INFO_STRU *ptrReqConfigInfo)
{
    VOS_UINT16 ulDataLen                       = 0;
    PPP_EVENT_MNTN_INFO_STRU *ptrPppFrameMntnSt = VOS_NULL;
    VOS_UINT8  *pucBuff                         = VOS_NULL;

    VOS_UINT16 usChapChallengeLen               = 0;
    VOS_UINT16 usChapChallengeLenAligned        = 0;

    VOS_UINT16 usChapResponseLen                = 0;
    VOS_UINT16 usChapResponseLenAligned         = 0;

    VOS_UINT16 usPapLen                         = 0;
    VOS_UINT16 usPapLenAligned                  = 0;

    VOS_UINT16 usIpcpLen                        = 0;
    VOS_UINT16 usIpcpLenAligned                 = 0;

    /* 事件消息内容长度计算 + 1byte 验证类型 + 1byte 填充*/
    ulDataLen += (sizeof(VOS_UINT8)*2);

    /* 参数检查*/
    if (PPP_PAP_AUTH_TYPE == ptrReqConfigInfo->stAuth.ucAuthType)
    {
        if ((0 != ptrReqConfigInfo->stAuth.AuthContent.PapContent.usPapReqLen)
            && (VOS_NULL == ptrReqConfigInfo->stAuth.AuthContent.PapContent.pPapReq))
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
                "PPP, Ppp_RcvConfigInfoReqMntnInfo, ERROR, Param PAP Error!\n");
            return;
        }
        usPapLen = ptrReqConfigInfo->stAuth.AuthContent.PapContent.usPapReqLen;
        PPP_LENALIGNTO2BYTE(usPapLenAligned, usPapLen);

        /* 事件消息内容长度计算 + 2byte PAP长度字段 + PAP长度*/
        ulDataLen += (usPapLenAligned + sizeof(VOS_UINT16));
    }
    if (PPP_CHAP_AUTH_TYPE == ptrReqConfigInfo->stAuth.ucAuthType)
    {
        if ((0 != ptrReqConfigInfo->stAuth.AuthContent.ChapContent.usChapChallengeLen)
            && (VOS_NULL == ptrReqConfigInfo->stAuth.AuthContent.ChapContent.pChapChallenge))
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
                "PPP, Ppp_RcvConfigInfoReqMntnInfo, ERROR, Param CHAP Challenge Error!\n");
            return;
        }
        if ((0 != ptrReqConfigInfo->stAuth.AuthContent.ChapContent.usChapResponseLen)
            && (VOS_NULL == ptrReqConfigInfo->stAuth.AuthContent.ChapContent.pChapResponse))
        {
            PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
                "PPP, Ppp_RcvConfigInfoReqMntnInfo, ERROR, Param CHAP Response Error!\n");
            return;
        }
        usChapChallengeLen = ptrReqConfigInfo->stAuth.AuthContent.ChapContent.usChapChallengeLen;
        PPP_LENALIGNTO2BYTE(usChapChallengeLenAligned, usChapChallengeLen);

        usChapResponseLen  = ptrReqConfigInfo->stAuth.AuthContent.ChapContent.usChapResponseLen;
        PPP_LENALIGNTO2BYTE(usChapResponseLenAligned, usChapResponseLen);

        /* 事件消息内容长度计算 + 2byte challenge长度字段 + challenge长度 + 2byte response 长度字段 + response长度*/
        ulDataLen += (usChapChallengeLenAligned + usChapResponseLenAligned + (sizeof(VOS_UINT16)*2));
    }

    if ((0 != ptrReqConfigInfo->stIPCP.usIpcpLen) && (VOS_NULL == ptrReqConfigInfo->stIPCP.pIpcp))
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "PPP, Ppp_RcvConfigInfoReqMntnInfo, ERROR, Param IPCP Error!\n");
        return;
    }
    usIpcpLen = ptrReqConfigInfo->stIPCP.usIpcpLen;
    PPP_LENALIGNTO2BYTE(usIpcpLenAligned, usIpcpLen);

    /* 事件消息内容长度计算 + 2byte IPCP长度字段 + IPCP长度*/
    ulDataLen += (usIpcpLenAligned + sizeof(VOS_UINT16));

    ptrPppFrameMntnSt = (PPP_EVENT_MNTN_INFO_STRU *)PS_MEM_ALLOC(PS_PID_APP_PPP,
                        ulDataLen + sizeof(PPP_EVENT_MNTN_INFO_STRU));
    if (VOS_NULL_PTR == ptrPppFrameMntnSt)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, PS_SUBMOD_NULL, PS_PRINT_ERROR,
            "PPP, Ppp_RcvConfigInfoReqMntnInfo, ERROR, Call VOS_MemAlloc fail!\n");
        return;
    }

    /* 填入 验证类型 + padding*/
    pucBuff = (VOS_UINT8 *)(ptrPppFrameMntnSt + 1);
    PPP_PUTCHAR(ptrReqConfigInfo->stAuth.ucAuthType, pucBuff);
    PPP_PUTCHAR(0, pucBuff);

    /* 填入 pap 长度 pap内容*/
    if(PPP_PAP_AUTH_TYPE == ptrReqConfigInfo->stAuth.ucAuthType)
    {
        PPP_PUTSHORT(usPapLenAligned, pucBuff);
        if (0 != usPapLenAligned)
        {
            PS_MEM_CPY(pucBuff, ptrReqConfigInfo->stAuth.AuthContent.PapContent.pPapReq, usPapLen);
            PPP_INCPTR(usPapLenAligned, pucBuff);
        }
    }
    /* 填入 chap 长度 chap内容*/
    if(PPP_CHAP_AUTH_TYPE == ptrReqConfigInfo->stAuth.ucAuthType)
    {
        PPP_PUTSHORT(usChapChallengeLenAligned, pucBuff);
        if (0 != usChapChallengeLenAligned)
        {
            PS_MEM_CPY(pucBuff, ptrReqConfigInfo->stAuth.AuthContent.ChapContent.pChapChallenge, usChapChallengeLen);
            PPP_INCPTR(usChapChallengeLenAligned, pucBuff);
        }

        PPP_PUTSHORT(usChapResponseLenAligned, pucBuff);
        if (0 != usChapResponseLenAligned)
        {
            PS_MEM_CPY(pucBuff, ptrReqConfigInfo->stAuth.AuthContent.ChapContent.pChapResponse, usChapResponseLen);
            PPP_INCPTR(usChapResponseLenAligned, pucBuff);
        }
    }

    /*填入IPCP长度,内容*/
    PPP_PUTSHORT(usIpcpLenAligned, pucBuff);
    if (0 != usIpcpLenAligned)
    {
       PS_MEM_CPY(pucBuff, ptrReqConfigInfo->stIPCP.pIpcp, usIpcpLen);
       PPP_INCPTR(usIpcpLenAligned, pucBuff);
    }

    /*填入公共信息字段*/
    Ppp_FillEventMntnInfo(ptrPppFrameMntnSt, usPppID, PPP_AT_RECV_CONFIG_INFO_REQ,
                                (ulDataLen + sizeof(PPP_EVENT_MNTN_INFO_STRU)));

    PPP_MNTN_TRACE_MSG(ptrPppFrameMntnSt);

    PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);

    return;
}

#else

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ppp_public.h"

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 PPP_PullPacketEvent(VOS_UINT16 usPppId, PPP_ZC_STRU *pstImmZc)
{
    return PS_SUCC;
}


VOS_UINT32 PPP_PullRawDataEvent(VOS_UINT16 usPppId, PPP_ZC_STRU *pstImmZc)
{
    return PS_SUCC;
}

#endif /* #if(FEATURE_ON == FEATURE_PPP) */

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

