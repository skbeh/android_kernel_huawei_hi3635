

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
#include "product_config.h"
#if(FEATURE_ON == FEATURE_PPP)
/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "ppp_public.h"
#include "pppid.h"
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
#include "link.h"
#include "pap.h"
#include "ppp_input.h"
#include "ppp_atcmd.h"
#include "NVIM_Interface.h"
#include "hdlc_hardware.h"
#include "TtfNvInterface.h"
#include "NasNvInterface.h"
#include "TafNvInterface.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_PPP_INIT_C

/******************************************************************************
   PPP任务优先级高于modem_send高，比modem_recv任务低
******************************************************************************/
#define PPP_TASK_PRI                                  143

extern VOS_VOID PPP_ProcDataNotify(VOS_VOID);
extern VOS_UINT32  PPP_Snd1stDataNotify(VOS_VOID);
extern VOS_VOID PPP_ProcAsFrmDataInd(struct MsgCB * pMsg);

#ifdef WTTF_PS_FUSION_PC_ST
extern PPP_ID   PPP_STUB_SetUp();
extern VOS_VOID PPP_STUB_Config(PPP_ID usPppId, PPP_HDLC_CTRL_STRU *pstParam);
#endif

#if (VOS_OS_VER == VOS_WIN32)
extern VOS_UINT32  VHW_HDLC_Init( VOS_VOID );
#endif

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/
/* 保存从NV项中读取的WINS特性开关值*/
VOS_UINT8  g_ucPppConfigWins = WINS_CONFIG_ENABLE;

/******************************************************************************
   5 函数实现
******************************************************************************/

VOS_VOID PPP_UpdateWinsConfig(VOS_UINT8 ucWins)
{
    if ((WINS_CONFIG_DISABLE != ucWins) && (WINS_CONFIG_ENABLE != ucWins))
    {
        /* ucWins值无效 */
        return;
    }

    /* 更新g_ucPppConfigWins */
    g_ucPppConfigWins = ucWins;

    return;
}


VOS_VOID PPP_DataQInit(VOS_VOID)
{
    PPP_ZC_QUEUE_STRU                    *pstDataQ;


    pstDataQ    = &(g_PppDataQCtrl.stDataQ);

    VOS_MemSet(&g_PppDataQCtrl, 0, sizeof(g_PppDataQCtrl));

    PPP_ZC_QUEUE_INIT(pstDataQ);

    return;
}



VOS_VOID PPP_ClearDataQ(VOS_VOID)
{
    PPP_ZC_STRU    *pstMem;
    VOS_INT32       lLockKey;


    for(;;)
    {
        lLockKey    = VOS_SplIMP();

        if ( 0 == PPP_ZC_GET_QUEUE_LEN(&g_PppDataQCtrl.stDataQ) )
        {
            VOS_Splx(lLockKey);
            break;
        }

        VOS_Splx(lLockKey);

        pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(&g_PppDataQCtrl.stDataQ);

        PPP_MemFree(pstMem);
    }

    PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,
                                  "PPP, PPP_ClearDataQ, NORMAL, Clear Stat Info\n");

    return;
} /* PPP_ClearDataQ */

VOS_UINT16 PPP_GetMruConfig(VOS_VOID)
{
    VOS_UINT16                      usPppConfigMru  = DEF_MRU;
    VOS_UINT32                      ulRslt;
    PPP_CONFIG_MRU_TYPE_NV_STRU     stPppConfigMruType;


    /* 为客户定制PPP Default MRU而使用NV_Item，由于此NV结构为16bit，考虑到字节对齐因素，
       长度固定写sizeof(VOS_UINT16) */
    ulRslt = NV_ReadEx(MODEM_ID_0, en_NV_Item_PPP_CONFIG_MRU_Type, &stPppConfigMruType, sizeof(VOS_UINT16));
    usPppConfigMru  = stPppConfigMruType.usPppConfigType;

    if (NV_OK != ulRslt)
    {
        usPppConfigMru = DEF_MRU;
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "Warning: Read en_NV_Item_PPP_DEFAULT_MRU_Type Error!");
    }

    if (usPppConfigMru > MAX_MRU)
    {
        usPppConfigMru = MAX_MRU;
    }

    if (usPppConfigMru < MIN_MRU)
    {
        usPppConfigMru = MIN_MRU;
    }

    PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
        "[INFO] PPP -- NV config MRU : <1>", (VOS_INT32)usPppConfigMru);

    return usPppConfigMru;
}



VOS_UINT8 PPP_GetWinsConfig(VOS_VOID)
{
    WINS_CONFIG_STRU    stWins;

    /* 定制需求,读取WINS特性控制开关NV项 */

    /* 若读取en_NV_Item_WINS失败,则默认WINS设置为使能 */
    if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_WINS_Config, &stWins, sizeof(WINS_CONFIG_STRU)))
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "Warning: Read en_NV_Item_WINS_Config Error!");
        return WINS_CONFIG_ENABLE;
    }

    /* 若en_NV_Item_WINS未设置，则默认WINS设置为使能 */
    if (0 == stWins.ucStatus)
    {
        return WINS_CONFIG_ENABLE;
    }

    /* 若en_NV_Item_WINS设置值无效，则默认WINS设置为使能 */
    if ((WINS_CONFIG_ENABLE != stWins.ucWins) && (WINS_CONFIG_DISABLE != stWins.ucWins))
    {
        return WINS_CONFIG_ENABLE;
    }

    PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_INFO,
                  "PPP_GetWinsConfig,[INFO] PPP -- NV config WINS : %d", (VOS_INT32)stWins.ucWins);

    return stWins.ucWins;
}



VOS_UINT32    PppInit(enum VOS_INIT_PHASE_DEFINE InitPhase )
{
    VOS_INT32               i;

    #ifdef WTTF_PS_FUSION_PC_ST
    VOS_UINT16              usPppId;
    PPP_HDLC_CTRL_STRU      stParam;
    #endif

    switch( InitPhase )
    {
    case   VOS_IP_LOAD_CONFIG:


           /*定制需求,通过NV项,获取用户配置MRU以及WINS协商开关*/
           /*读取NV项,获取用户配置MRU*/
           g_usPppConfigMru = PPP_GetMruConfig();

           /*读取NV项,获取用户配置WINS使能开关*/
           g_ucPppConfigWins = PPP_GetWinsConfig();


            /*向系统申请分配一块全局所用的link数组的空间*/
            /*lint -e433*/
            pgPppLink = (struct link *)VOS_MemAlloc(PS_PID_APP_PPP, STATIC_MEM_PT, sizeof(struct link)*PPP_MAX_ID_NUM);
            /*lint +e433*/
            if (VOS_NULL_PTR == pgPppLink)
            {
                /*输出错误信息*/
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,"PppInit, malloc of  memory fail\r\n");
                return VOS_ERR;
            }

            /*向系统申请分配一块全局所用的TAF_PPP_PUBLIC_STRU的空间*/
            pgPppId = (PPP_ID *)VOS_MemAlloc(PS_PID_APP_PPP, STATIC_MEM_PT, sizeof(PPP_ID)*PPP_MAX_ID_NUM_ALLOC);
            if (VOS_NULL_PTR == pgPppId)
            {
                /*释放申请的全局所用的TAF_PPP_PUBLIC_STRU的空间*/
                VOS_MemFree(PS_PID_APP_PPP, pgPppLink);
                pgPppLink = VOS_NULL_PTR;


                /*输出错误信息*/
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_ERROR,"PppInit, malloc of  memory fail\r\n");
                return VOS_ERR;
            }

            /*对于系统所用到的所有数据结构都初始化*/
            for(i = 0;i < PPP_MAX_ID_NUM; i++)
            {
                link_Init((pgPppLink + i));
            }

            PppIdInit();

            /*初始化PPP的数据队列*/
            PPP_DataQInit();

            /* 初始化HDLC */
            if (VOS_OK != PPP_HDLC_HARD_Init())
            {
                return VOS_ERR;
            }

            #if (VOS_OS_VER == VOS_WIN32)
            VHW_HDLC_Init();
            #endif

            #ifdef WTTF_PS_FUSION_PC_ST
            usPppId = PPP_STUB_SetUp();
            stParam.ulAccm                =   0xFFFFFFFF;
            stParam.ulAcfComp          =   0;
            stParam.ulProtoComp      =   0;

            PPP_STUB_Config( usPppId, &stParam);
            #endif
            break;

    case   VOS_IP_FARMALLOC:
    case   VOS_IP_INITIAL:
    case   VOS_IP_ENROLLMENT:
    case   VOS_IP_LOAD_DATA:
    case   VOS_IP_FETCH_DATA:
    case   VOS_IP_STARTUP:
    case   VOS_IP_RIVAL:
    case   VOS_IP_KICKOFF:
    case   VOS_IP_STANDBY:
    case   VOS_IP_BROADCAST_STATE:
    case   VOS_IP_RESTART:
    case   VOS_IP_BUTT:
           break;
    }

    return VOS_OK;
}
VOS_VOID PppMsgTimerProc( struct MsgCB * pMsg )
{
    REL_TIMER_MSG  *pPsMsg = (REL_TIMER_MSG  *)pMsg;
    VOS_UINT16      usPppId = (VOS_UINT16)(pPsMsg->ulName);

    /*判断是那个阶段启动的定时器*/
    switch(pPsMsg->ulPara)
    {
        /*如果是LCP阶段启动的定时器*/
        case PHASE_ESTABLISH:
            if (PPP_LINK(usPppId)->phase == PHASE_ESTABLISH)
            {
                FsmTimeout(&(PPP_LINK(usPppId)->lcp.fsm),
                    pPsMsg->ulPara);
            }

            break;

        /*如果是终止阶段启动的定时器*/
        case PHASE_TERMINATE:
            if (PPP_LINK(usPppId)->phase == PHASE_TERMINATE)
            {
                FsmTimeout(&(PPP_LINK(usPppId)->lcp.fsm),
                    pPsMsg->ulPara);
            }

            break;

        /*如果是认证阶段启动的定时器*/
        case PHASE_AUTHENTICATE:
            {
                if (PPP_LINK(usPppId)->phase == PHASE_AUTHENTICATE)
                {
                    AuthTimeout(PPP_LINK(usPppId));
                }

                break;
            }

        /*如果是IPCP阶段启动的定时器*/
        case PHASE_NETWORK:
            if (PPP_LINK(pPsMsg->ulName)->phase == PHASE_NETWORK)
            {
                FsmTimeout(&(PPP_LINK(usPppId)->ipcp.fsm),
                    pPsMsg->ulPara);
            }

            break;

        /*如果是IPCP阶段待PDP激活的定时器,则处理待处理的IPCP帧*/
        case PHASE_PDP_ACT_PENDING:
            if (VOS_NULL_PTR != PPP_LINK(usPppId)->ipcp.pstIpcpPendFrame)
            {
                fsm_Input(&(PPP_LINK(usPppId)->ipcp.fsm), PPP_LINK(usPppId)->ipcp.pstIpcpPendFrame);
                PPP_LINK(usPppId)->ipcp.pstIpcpPendFrame = VOS_NULL_PTR;
            }
            PPP_LINK(usPppId)->ipcp.hIpcpPendTimer = VOS_NULL_PTR;
            break;

        case PHASE_TERMINATE_PENDING:
            PPP_LINK(usPppId)->lcp.hLcpCloseTimer = VOS_NULL_PTR;
            PPP_ProcPppDisconnEvent(usPppId);
            break;

        case HDLC_FRM_OUTPUT_SPACE_ALLOC_FAIL:
            g_stHdlcConfigInfo.stHdlcFrmTimerHandle = VOS_NULL_PTR;
            PPP_Snd1stDataNotify();
            break;

        default:
            {
                PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,"unknow timer!\r\n");
            }
    }


    return;
}



VOS_VOID PppMsgProc( struct MsgCB * pMsg )
{
    PPP_MSG    *pPsMsg  = (PPP_MSG *)pMsg;


    /*PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_INFO, "\r\nPppMsgProc, INFO, Enter!\r\n");
 */

    if(pMsg == VOS_NULL_PTR)
    {
        return;
    }

    PPP_HDLC_HARD_PeriphClkOpen();

    /*如果是定时器发来的消息*/
    if (VOS_PID_TIMER == pMsg->ulSenderPid)
    {
        PppMsgTimerProc(pMsg);
        PPP_HDLC_HARD_PeriphClkClose();

        return ;
    }

    /*如果是接收到从TE发送来的数据帧*/
    switch(pPsMsg->ulMsgType)
    {
        #if (PPP_FEATURE == PPP_FEATURE_PPP)
        case PPP_DATA_PROC_NOTIFY:
            PPP_ProcDataNotify();
            break;

        case PPP_AT_CTRL_OPERATION:
            PPP_ProcAtCtrlOper(pMsg);
            break;

        case PPP_HDLC_PROC_AS_FRM_PACKET_IND:
            PPP_ProcAsFrmDataInd(pMsg);
            break;

        #endif
#ifdef WTTF_PS_FUSION_PC_ST
        case AT_PPP_UL_DATA_REQ:
           PPP_STUB_RcvUlData(pMsg);
           break;
        case PPP_AT_DL_DATA_IND:
              break;
#endif
        default:
            break;
    }

    PPP_HDLC_HARD_PeriphClkClose();

    return;
}


VOS_VOID    PPP_PowerOff(VOS_VOID)
{
    VOS_UINT8   ucPppId;

    /*软关机时若定时器还在运行，则停止该定时器:*/
    for(ucPppId = 1; ucPppId <= PPP_MAX_ID_NUM; ucPppId++)
    {
        /*LCP状态机定时器:*/
        if( VOS_NULL_PTR !=((PPP_LINK(ucPppId))->lcp.fsm.timer) )
        {
            VOS_StopRelTimer(&((PPP_LINK(ucPppId))->lcp.fsm.timer));
        }

        /*IPCP状态机定时器:*/
        if( VOS_NULL_PTR !=((PPP_LINK(ucPppId))->ipcp.fsm.timer) )
        {
            VOS_StopRelTimer(&((PPP_LINK(ucPppId))->ipcp.fsm.timer));
        }
    }

    /*释放PPP数据队列*/
    PPP_ClearDataQ();
    return;
}


/*****************************************************************************
 Prototype      : PppStop
 Description    : TAF PPP模块中的结束函数,该函数负责在系统重新启动时释放PPP模
                  块向系统申请的资源
 Input          : ---
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
#define PPP_FREE(point)\
    if(point!=VOS_NULL_PTR)         \
    {                                   \
        /*释放申请的全局所用的空间*/    \
        VOS_MemFree(PS_PID_APP_PPP, point);   \
        point = VOS_NULL_PTR;  \
    }\

VOS_VOID    PppStop(VOS_VOID)
{
    /*VOS_UINT32    dis_interrupt;*/
    VOS_INT32 i;



    /*对于系统所用到的throughout结构都释放掉它申请的内存*/
    for(i = 0;i < PPP_MAX_ID_NUM; i++)
    {
        throughput_destroy(&((pgPppLink + i)->stats.total));
    }

    PPP_FREE(pgPppLink)
    PPP_FREE(pgPppId)

    return;
}

#else

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AtPppInterface.h"
#include "ppp_init.h"

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID PPP_UpdateWinsConfig(VOS_UINT8 ucWins)
{
    return;
}

VOS_UINT32    PppInit(enum VOS_INIT_PHASE_DEFINE InitPhase )
{
    switch (InitPhase)
    {
        case VOS_IP_INITIAL:
            break;

        default:
            break;
    }

    return VOS_OK;
}

VOS_VOID PppMsgProc( struct MsgCB * pMsg )
{
    /*消息为空，直接返回*/
    if (VOS_NULL_PTR == pMsg)
    {
        return;
    }

    return;
}

#endif /*#if(FEATURE_ON == FEATURE_PPP)*/



#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

