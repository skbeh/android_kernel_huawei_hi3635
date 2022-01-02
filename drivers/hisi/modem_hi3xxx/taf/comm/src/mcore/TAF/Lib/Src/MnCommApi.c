

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MnCommApi.h"
#include "AtMnInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_MN_COMM_API_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
extern VOS_UINT32 AT_GetDestPid(
    MN_CLIENT_ID_T                      usClientId,
    VOS_UINT32                          ulRcvPid
);
#endif


VOS_UINT32  MN_GetAppReqMsgLen(
    VOS_UINT32 ulParaLen,
    VOS_UINT32 *pulMsgLen
)
{
    VOS_UINT32 ulMsgLen;

    if (VOS_NULL_PTR == pulMsgLen)
    {
        return VOS_ERR;
    }

    if (ulParaLen <= 4)
    {
        ulMsgLen = sizeof(MN_APP_REQ_MSG_STRU);
    }
    else
    {
        ulMsgLen = (sizeof(MN_APP_REQ_MSG_STRU) + ulParaLen) - 4;
    }

    *pulMsgLen = ulMsgLen;

    return VOS_OK;
}


VOS_UINT32  MN_FillAppReqMsgHeader(
    MN_APP_REQ_MSG_STRU                 *pMsg,
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT16                          usMsgType,
    VOS_UINT32                          ulReceiverPid
 )
{
    MN_APP_REQ_MSG_STRU *pAppMsgHeader;

    pAppMsgHeader                       = pMsg;
    pAppMsgHeader->clientId             = ClientId;
    pAppMsgHeader->opId                 = OpId;
    pAppMsgHeader->usMsgName            = usMsgType;
    /* pAppMsgHeader->ulSenderCpuId        = VOS_LOCAL_CPUID; */
    /* pAppMsgHeader->ulReceiverCpuId      = VOS_LOCAL_CPUID; */
    /* pAppMsgHeader->ulSenderPid          = WUEPS_PID_AT; */
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    pAppMsgHeader->ulReceiverPid        = AT_GetDestPid(ClientId, ulReceiverPid);
#else
    pAppMsgHeader->ulReceiverPid        = ulReceiverPid;
#endif

    return VOS_OK;
}
VOS_UINT32  MN_FillAppReqMsgPara(
    VOS_VOID                            *pSndMsgPara,
    VOS_VOID                            *pPara,
    VOS_UINT32                          ulLen
)
{
    if (VOS_NULL_PTR == pSndMsgPara)
    {
        return VOS_ERR;
    }

    if (ulLen > 0)
    {
        PS_MEM_CPY(pSndMsgPara, pPara, ulLen);
    }

    return VOS_OK;
}



VOS_UINT32  MN_FillAndSndAppReqMsg(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT16                          usMsgType,
    VOS_VOID                            *pPara,
    VOS_UINT32                          ulParaLen,
    VOS_UINT32                          ulReceiverPid
)
{
    VOS_UINT8                          *pMsg;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulMsgLen;
    VOS_UINT8                          *pMsgPara;

    if ((VOS_NULL_PTR == pPara) && (0 < ulParaLen))
    {
        return TAF_FAILURE;
    }

    pMsgPara = (VOS_UINT8 *)pPara;

    /* 获取消息长度 */
    MN_GetAppReqMsgLen( ulParaLen, &ulMsgLen);

    /* 申请消息 */
    pMsg = (VOS_UINT8 *)PS_ALLOC_MSG(WUEPS_PID_AT, ulMsgLen - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        return TAF_FAILURE;
    }

    PS_MEM_SET( (pMsg + VOS_MSG_HEAD_LENGTH), 0, (ulMsgLen - VOS_MSG_HEAD_LENGTH));

    /* 填充消息头 */
    MN_FillAppReqMsgHeader((MN_APP_REQ_MSG_STRU *)pMsg, ClientId, OpId, usMsgType, ulReceiverPid );

    /* 填充消息参数 */
    MN_FillAppReqMsgPara( &pMsg[sizeof(MN_APP_REQ_MSG_STRU) - 4], pMsgPara, ulParaLen );

    ulRet = PS_SEND_MSG(WUEPS_PID_AT, pMsg);

    if (VOS_OK != ulRet)
    {
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
