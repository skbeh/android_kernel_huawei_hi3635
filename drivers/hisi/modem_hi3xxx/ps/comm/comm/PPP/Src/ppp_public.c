

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "ppp_public.h"
#include "om.h"
#include "hdlc_interface.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767  打点日志文件宏ID定义 */
#define    THIS_FILE_ID                 PS_FILE_ID_PPP_PUBLIC_C
/*lint +e767   */


/******************************************************************************
   2 外部函数变量声明
******************************************************************************/



/******************************************************************************
   3 私有定义
******************************************************************************/


#if (FEATURE_ON == FEATURE_PPP)
/******************************************************************************
   4 全局变量定义
******************************************************************************/
extern PPP_DATA_Q_CTRL_ST               g_PppDataQCtrl;

VOS_UINT32                              g_ulPppDebugLevel = PS_PRINT_WARNING;
/******************************************************************************
   5 函数实现
******************************************************************************/

PPP_ZC_STRU * PPP_MemAlloc(VOS_UINT16 usLen, VOS_UINT16 usReserveLen)
{
    /* 该接口用在上行时需要保留MAC头长度，
      上行与ADS收发数为IP包，为与NDIS、E5保持数据结构统一，需要保留MAC头，
      零拷贝指针从C核返回的时候统一偏移固定字节，找到零拷贝头部。
    */
    /*
       用在下行时保留长度填0，下行与USB收发数据为字节流形式的PPP帧，无MAC头
    */
    PPP_ZC_STRU *pstMem = PPP_ZC_MEM_ALLOC(usLen + usReserveLen);


    if (VOS_NULL_PTR != pstMem)
    {
        if ( usReserveLen > 0)
        {
            /* 空出保留长度，对PPP模块而言数据长度是usLen，这个函数必须在未赋值前调用 */
            PPP_ZC_RESERVE(pstMem, usReserveLen);

            /* 更新上行申请总次数 */
            g_PppDataQCtrl.stStat.ulMemAllocUplinkCnt++;

            /* 用于区分协商阶段释放的数据来源 */
            PPP_ZC_SET_DATA_APP(pstMem, (VOS_UINT16)(1 << 8) | (VOS_UINT16)PPP_PULL_PACKET_TYPE);
        }
        else
        {
            /* 更新下行申请总次数 */
            g_PppDataQCtrl.stStat.ulMemAllocDownlinkCnt++;

            /* 用于区分协商阶段释放的数据来源 */
            PPP_ZC_SET_DATA_APP(pstMem, (VOS_UINT16)(1 << 8) | (VOS_UINT16)PPP_PUSH_PACKET_TYPE);
        }
    }
    else
    {
        if ( usReserveLen > 0)
        {
            /* 更新上行申请失败次数 */
            g_PppDataQCtrl.stStat.ulMemAllocUplinkFailCnt++;
        }
        else
        {
            /* 更新下行申请失败次数 */
            g_PppDataQCtrl.stStat.ulMemAllocDownlinkFailCnt++;
        }
    }

    return pstMem;
}
VOS_VOID PPP_MemWriteData(PPP_ZC_STRU *pstMem, VOS_UINT8 *pucSrc, VOS_UINT16 usLen)
{
    /* 设置好将要写入零拷贝内存数据内容长度 */
    PPP_ZC_SET_DATA_LEN(pstMem, usLen);

    /* 拷贝至内存数据部分 */
    PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), pucSrc, usLen);

    return;
}


PPP_ZC_STRU * PPP_MemCopyAlloc(VOS_UINT8 *pSrc, VOS_UINT16 usLen, VOS_UINT16 usReserveLen)
{
    PPP_ZC_STRU                        *pstMem = VOS_NULL_PTR;


    pstMem = PPP_MemAlloc(usLen, usReserveLen);

    if ( VOS_NULL_PTR != pstMem )
    {
        /* 拷贝至内存数据部分 */
        PPP_MemWriteData(pstMem, pSrc, usLen);
    }

    return pstMem;
}
VOS_UINT32 PPP_MemCutTailData
(
    PPP_ZC_STRU **ppMemSrc,
    VOS_UINT8 *pucDest,
    VOS_UINT16 usLen,
    VOS_UINT16 usReserveLen
)
{
    PPP_ZC_STRU                        *pCurrMem;
    VOS_UINT16                          usCurrLen;
    VOS_UINT16                          usCurrOffset;


    /* 参数检查 */
    if ( (VOS_NULL_PTR == ppMemSrc) ||
         (VOS_NULL_PTR == *ppMemSrc) ||
         (VOS_NULL_PTR == pucDest))
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_MemCutTailData input parameters error, \
                      src addr'addr: 0X%x, dest addr: 0X%x\r\n",
                      (VOS_UINT32)ppMemSrc, (VOS_UINT32)pucDest);

        return PS_FAIL;
    }

    pCurrMem    = (PPP_ZC_STRU *)(*ppMemSrc);
    usCurrLen   = PPP_ZC_GET_DATA_LEN(pCurrMem);

    if ( ( 0 == usLen) || (usCurrLen < usLen) )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_MemCutTailData, Warning, usCurrLen %d Less Than usLen %d!\r\n",
                      usCurrLen, usLen);

        return PS_FAIL;
    }

    /* 从尾部拷贝定长数据，只会有一个节点 */
    usCurrOffset = usCurrLen - usLen;

    DRV_RT_MEMCPY(pucDest, &(PPP_ZC_GET_DATA_PTR(pCurrMem)[usCurrOffset]), usLen);

    if ( usCurrOffset > 0 )
    {
        /* 还有剩余数据，目前没有重算长度并将Tail指针前移的接口，重新申请 */
        (*ppMemSrc) = PPP_MemCopyAlloc(PPP_ZC_GET_DATA_PTR(pCurrMem), usCurrOffset, usReserveLen);
    }
    else
    {
        (*ppMemSrc) = VOS_NULL_PTR;
    }

    /* 释放内存 */
    PPP_MemFree(pCurrMem);

    return PS_SUCC;
}


VOS_UINT32 PPP_MemCutHeadData
(
    PPP_ZC_STRU **ppMemSrc,
    VOS_UINT8 *pucDest,
    VOS_UINT16 usDataLen
)
{
    PPP_ZC_STRU                        *pCurrMem;
    VOS_UINT16                          usMemSrcLen;


    if ( (VOS_NULL_PTR == ppMemSrc) ||
         (VOS_NULL_PTR == *ppMemSrc) ||
         (VOS_NULL_PTR == pucDest) )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                     "PPP_MemCutHeadData input parameters error, \
                     src addr'addr: 0x%x, dest addr: 0x%x\r\n",
                    (VOS_UINT32)ppMemSrc, (VOS_UINT32)pucDest);

        return PS_FAIL;
    }

    /* 判断TTF内存块的长度是否符合要求 */
    pCurrMem        = (PPP_ZC_STRU *)(*ppMemSrc);
    usMemSrcLen     = PPP_ZC_GET_DATA_LEN(pCurrMem);

    if ( ( 0 == usDataLen) || (usMemSrcLen < usDataLen) )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                      "PPP_MemCutHeadData, Warning: usMemSrcLen: %d Less Than usDataLen: %d!\r\n",
                      usMemSrcLen, usDataLen);

        return PS_FAIL;
    }

    /* 从头部拷贝定长数据，只会有一个节点 */
    DRV_RT_MEMCPY(pucDest, PPP_ZC_GET_DATA_PTR(pCurrMem), usDataLen);

    if ( usMemSrcLen >  usDataLen)
    {
        /* 还有剩余数据，更新数据指针和长度 */
        PPP_ZC_REMOVE_HDR(pCurrMem, usDataLen);
    }
    else
    {
        /* 释放原始内存 */
        PPP_MemFree(pCurrMem);
        (*ppMemSrc) = VOS_NULL_PTR;
    }

    return PS_SUCC;
}


VOS_UINT32 PPP_MemGet(PPP_ZC_STRU *pMemSrc, VOS_UINT16 usOffset, VOS_UINT8 *pDest, VOS_UINT16 usLen)
{
    VOS_UINT16                          usMemSrcLen;


    /* 参数检查 */
    if ( (VOS_NULL_PTR == pMemSrc)||(VOS_NULL_PTR == pDest) )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "PPP_MemGet, Warning, Input Par pMemSrc Or pDest is Null!\r\n");

        return PS_FAIL;
    }

    if ( 0 == usLen )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                     "PPP_MemGet, Warning, Input Par usLen is 0!\r\n");

        return PS_FAIL;
    }

    /* 判断TTF内存块的长度是否符合要求 */
    usMemSrcLen = PPP_ZC_GET_DATA_LEN(pMemSrc);

    if ( usMemSrcLen < (usOffset + usLen) )
    {
        PPP_MNTN_LOG2(PS_PID_APP_PPP, 0, PS_PRINT_WARNING,
                      "PPP_MemGet, Warning, MemSrcLen %d Less Than (Offset + Len) %d!\r\n",
                      usMemSrcLen, (usOffset + usLen));

        return PS_FAIL;
    }

    DRV_RT_MEMCPY(pDest, PPP_ZC_GET_DATA_PTR(pMemSrc) + usOffset, usLen);

    return PS_SUCC;
}


VOS_VOID PPP_MemFree(PPP_ZC_STRU *pstMem)
{
    /* 释放零拷贝内存 */
    PPP_ZC_MEM_FREE(pstMem);

    g_PppDataQCtrl.stStat.ulMemFreeCnt++;

    return;
}
VOS_VOID PPP_MemSingleCopy(VOS_UINT8 *pucDest, VOS_UINT8 *pucSrc, VOS_UINT32 ulLen)
{
    /* 待修改为EDMA拷贝 */
    DRV_RT_MEMCPY(pucDest, pucSrc, ulLen);

    return;
}

VOS_VOID     PPP_SetPrintLevel
(
    VOS_UINT32      ulLevel
)
{
    g_ulPppDebugLevel = ulLevel;
}

VOS_VOID     PPP_PrintLog
(
    VOS_UINT32      ulModuleId,
    VOS_UINT32      ulSubMod,
    VOS_UINT32      ulLevel,
    VOS_CHAR       *pcString
)
{
    if ( g_ulPppDebugLevel >=  ulLevel)
    {
        vos_printf(pcString);
    }

    return;
}

VOS_VOID     PPP_PrintLog1
(
    VOS_UINT32      ulModuleId,
    VOS_UINT32      ulSubMod,
    VOS_UINT32      ulLevel,
    VOS_CHAR       *pcString,
    VOS_INT32       lPara1
)
{
    if ( g_ulPppDebugLevel >=  ulLevel)
    {
        vos_printf(pcString, lPara1);
    }

    return;
}
VOS_VOID     PPP_PrintLog2
(
    VOS_UINT32      ulModuleId,
    VOS_UINT32      ulSubMod,
    VOS_UINT32      ulLevel,
    VOS_CHAR       *pcString,
    VOS_INT32       lPara1,
    VOS_INT32       lPara2
)
{
    if ( g_ulPppDebugLevel >=  ulLevel)
    {
        vos_printf(pcString, lPara1, lPara2);
    }

    return;
}

VOS_VOID     PPP_PrintLog3
(
    VOS_UINT32      ulModuleId,
    VOS_UINT32      ulSubMod,
    VOS_UINT32      ulLevel,
    VOS_CHAR       *pcString,
    VOS_INT32       lPara1,
    VOS_INT32       lPara2,
    VOS_INT32       lPara3
)
{
    if ( g_ulPppDebugLevel >=  ulLevel)
    {
        vos_printf(pcString, lPara1, lPara2, lPara3);
    }

    return;
}
VOS_VOID     PPP_PrintLog4
(
    VOS_UINT32      ulModuleId,
    VOS_UINT32      ulSubMod,
    VOS_UINT32      ulLevel,
    VOS_CHAR       *pcString,
    VOS_INT32       lPara1,
    VOS_INT32       lPara2,
    VOS_INT32       lPara3,
    VOS_INT32       lPara4
)
{
    if ( g_ulPppDebugLevel >=  ulLevel)
    {
        vos_printf(pcString, lPara1, lPara2, lPara3, lPara4);
    }

    return;
}
#else
VOS_VOID PPP_MemFree(PPP_ZC_STRU *pstMem)
{
    /* 释放零拷贝内存 */
    PPP_ZC_MEM_FREE(pstMem);

    return;
}

#endif


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

