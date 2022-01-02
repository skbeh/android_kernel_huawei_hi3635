

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "ATCmdProc.h"
#include "AtCheckFunc.h"
#include "AtParseCmd.h"
#include "at_common.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_AT_PARSECMD_C

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/

/*****************************************************************************
   3 函数、变量声明
*****************************************************************************/

/*****************************************************************************
   4 函数实现
*****************************************************************************/


AT_STATE_TYPE_ENUM atFindNextSubState( AT_SUB_STATE_STRU *pSubStateTab,VOS_UINT8 ucInputChar)
{
    VOS_UINT16 usTabIndex = 0;                            /* 子状态表索引 */

    /* 依次比较子状态的每一项直至结束 */
    while(AT_BUTT_STATE != pSubStateTab[usTabIndex].next_state)
    {
        if( AT_SUCCESS == pSubStateTab[usTabIndex].pFuncName(ucInputChar))    /* 判断输入字符是否匹配 */
        {
            return pSubStateTab[usTabIndex].next_state;     /* 返回匹配的子状态 */
        }
        usTabIndex++;                                               /* 子状态表索引递增 */
    }
    return AT_BUTT_STATE;
}
AT_STATE_TYPE_ENUM atFindNextMainState(AT_MAIN_STATE_STRU *pMainStateTab,
    VOS_UINT8 ucInputChar,  AT_STATE_TYPE_ENUM InputState)
{
    VOS_UINT16 usTabIndex = 0;                            /* 子状态表索引 */

    /* 依次比较主状态的每一项直至结束 */
    while(AT_BUTT_STATE != pMainStateTab[usTabIndex].curr_state)
    {
        if( InputState == pMainStateTab[usTabIndex].curr_state)    /* 判断输入状态是否匹配 */
        {
            /* 如果状态匹配,则根据输入字符寻找下一个子状态 */
            return atFindNextSubState(pMainStateTab[usTabIndex].pSubStateTab,ucInputChar);
        }
        usTabIndex++;
    }
    return AT_BUTT_STATE;
}



TAF_UINT32 At_Auc2ul(TAF_UINT8 *nptr,TAF_UINT16 usLen,TAF_UINT32 *pRtn)
{
    TAF_UINT32 c     = 0;         /* current Char */
    TAF_UINT32 total = 0;         /* current total */
    TAF_UINT8 Length = 0;         /* current Length */

    c = (TAF_UINT32)*nptr++;

    while(Length++ < usLen)
    {
        if((c >= '0') && (c <= '9'))                /* 字符检查 */
        {
            /* 0xFFFFFFFF = 4294967295 */
            if(((total == 429496729) && (c > '5')) || (total > 429496729))
            {
                return AT_FAILURE;
            }
            total = (10 * total) + (c - '0');        /* accumulate digit */
            c = (TAF_UINT32)(TAF_UINT8)*nptr++;    /* get next Char */
        }
        else
        {
            return AT_FAILURE;
        }
    }

    *pRtn = total;   /* return result, negated if necessary */
    return AT_SUCCESS;
}

TAF_UINT32 At_String2Hex(TAF_UINT8 *nptr,TAF_UINT16 usLen,TAF_UINT32 *pRtn)
{
    TAF_UINT32 c     = 0;         /* current Char */
    TAF_UINT32 total = 0;         /* current total */
    TAF_UINT8 Length = 0;         /* current Length */

    c = (TAF_UINT32)*nptr++;

    while(Length++ < usLen)
    {
        if( (c  >= '0') && (c  <= '9') )
        {
            c  = c  - '0';
        }
        else if( (c  >= 'a') && (c  <= 'f') )
        {
            c  = (c  - 'a') + 10;
        }
        else if( (c  >= 'A') && (c  <= 'F') )
        {
            c  = (c  - 'A') + 10;
        }
        else
        {
            return AT_FAILURE;
        }

        if(total > 0x0FFFFFFF)              /* 发生反转 */
        {
            return AT_FAILURE;
        }
        else
        {
            total = (total << 4) + c;              /* accumulate digit */
            c = (TAF_UINT32)(TAF_UINT8)*nptr++;    /* get next Char */
        }
    }

    *pRtn = total;   /* return result, negated if necessary */
    return AT_SUCCESS;
}
TAF_UINT32 At_RangeToU32(TAF_UINT8 * pucBegain, TAF_UINT8 * pucEnd)
{
    TAF_UINT32 c;                                   /* current Char */
    TAF_UINT32 total = 0;                           /* current total */

    /* 输入参数检查 */
    if(pucBegain >= pucEnd)
    {
        return total;
    }

    /* 从第一个字符开始 */
    c = (TAF_UINT32)*pucBegain;

    /* 依次累加*10结果,直至结束 */
    while( (pucBegain != pucEnd) && ( (c >= '0') && (c <= '9') ))
    {
        total = (10 * total) + (c - '0');             /* accumulate digit */
        pucBegain++;                                /* 注意，必须在赋值之前移位，否则，被赋值两遍 */
        c = (TAF_UINT32)(TAF_UINT8)*pucBegain;      /* get next Char */

        if(total >= 0x19999998)                     /* 如果大于0x19999998，直接返回，否则反转 */
        {
            return total;
        }
    }

    return total;
}
/*****************************************************************************
 Prototype      : At_RangeCopy
 Description    : 把字符串中的某一段拷贝到指定地址,pDst指示目的地址,pucBegain
                  指示开始地址,pEnd指示结束地址
 Input          : pucDst    --- 目的地址
                  pucBegain --- 被转换字串的开始地址
                  pucEnd    --- 被转换字串的结束地址
 Output         : ---
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_RangeCopy(TAF_UINT8 *pucDst,TAF_UINT8 * pucBegain, TAF_UINT8 * pucEnd)
{
    /* 依次拷贝到目的地址,直至结束 */
    while(pucBegain < pucEnd)
    {
        *pucDst++ = *pucBegain++;
    }
}
TAF_UINT32 At_UpString(TAF_UINT8 *pData,TAF_UINT16 usLen)
{
    TAF_UINT8  *pTmp  = pData;                 /* current Char */
    TAF_UINT16 ChkLen = 0;

    if(0 == usLen)
    {
        return AT_FAILURE;
    }

    while(ChkLen++ < usLen)
    {
        if ( (*pTmp >= 'a') && (*pTmp <= 'z'))
        {
            *pTmp = *pTmp - 0x20;
        }
        pTmp++;
    }
    return AT_SUCCESS;
}
VOS_UINT32 atRangeToU32( VOS_UINT8 *pucBegain, VOS_UINT8 *pucEnd)
{
    VOS_UINT32 total = 0;                           /* current total */
    VOS_UINT32 ulRst;

    /* 输入参数检查 */
    if(pucBegain >= pucEnd)
    {
        return total;
    }

    ulRst = atAuc2ul(pucBegain, (VOS_UINT16)(pucEnd - pucBegain), &total);

    if(AT_SUCCESS != ulRst)
    {
        total = 0;
    }

    return total;
}


VOS_VOID atRangeCopy( VOS_UINT8 *pucDst, VOS_UINT8 * pucBegain, VOS_UINT8 * pucEnd)
{
    /* 依次拷贝到目的地址,直至结束 */
    while(pucBegain < pucEnd)
    {
        *pucDst++ = *pucBegain++;
    }
}

/******************************************************************************
 功能描述: 把十六进制字符串转成无符号整型值

 参数说明:
   nptr [in/out] 输入的字符串内容指针
   usLen [in] 输入的字符串长度
   pRtn [in/out] 由字符串转换所得整型值

 返 回 值:
    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
    AT_SUCCESS: 成功
******************************************************************************/
static VOS_UINT32 auc2ulHex( VOS_UINT8 *nptr, VOS_UINT16 usLen,  VOS_UINT32 *pRtn)
{
    VOS_UINT8 c         = 0;         /* current Char */
    VOS_UINT32 total    = 0;         /* current total */
    VOS_UINT16 usLength = 2;         /* current Length */
    VOS_UINT8 *pcTmp    = nptr + 2;  /* 从0x后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    c = *pcTmp++;

    while(usLength++ < usLen)
    {
        /* 0xFFFFFFFF */
        if(total > 0xFFFFFFF)
        {
            return AT_FAILURE;
        }

        /* 字符检查 */
        if(isdigit(c))
        {
            total = AT_CHECK_BASE_HEX * total + (c - '0');        /* accumulate digit */
            c = *pcTmp++;    /* get next Char */
        }
        else if('A' <= c && 'F' >= c)
        {
            total = AT_CHECK_BASE_HEX * total + (c - 'A' + 10);        /* accumulate digit */
            c = *pcTmp++;    /* get next Char */
        }
        else if('a' <= c && 'f' >= c)
        {
            total = AT_CHECK_BASE_HEX * total + (c - 'a' + 10);        /* accumulate digit */
            c = *pcTmp++;    /* get next Char */
        }
        else
        {
            return AT_FAILURE;
        }
    }

    *pRtn = total;   /* return result, negated if necessary */

    return AT_SUCCESS;
}

#if 0
/******************************************************************************
 功能描述: 把八进制字符串转成无符号整型值

 参数说明:
   nptr [in/out] 输入的字符串内容指针
   usLen [in] 输入的字符串长度
   pRtn [in/out] 由字符串转换所得整型值

 返 回 值:
    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
    AT_SUCCESS: 成功
******************************************************************************/
static VOS_UINT32 auc2ulOct( VOS_UINT8 *nptr, VOS_UINT16 usLen,  VOS_UINT32 *pRtn)
{
    VOS_UINT8 c         = 0;         /* current Char */
    VOS_UINT32 total    = 0;         /* current total */
    VOS_UINT16 usLength = 1;         /* current Length */
    VOS_UINT8 *pcTmp    = nptr + 1;  /* 从0x后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    c = *pcTmp++;

    while(usLength++ < usLen)
    {
        /* 字符检查 */
        if('0' <= c && '7' >= c)
        {
            /* 0xFFFFFFFF = 037777777777 */
            /* 0x1FFFFFFF   03777777777 */
            if( total     > 0x1FFFFFFF )
            {
                return AT_FAILURE;
            }

            total = AT_CHECK_BASE_OCT * total + (c - '0');        /* accumulate digit */
            c = *pcTmp++;    /* get next Char */
        }
        else
        {
            return AT_FAILURE;
        }
    }

    *pRtn = total;   /* return result, negated if necessary */

    return AT_SUCCESS;
}
#endif
/******************************************************************************
 功能描述: 把十进制字符串转成无符号整型值

 参数说明:
   nptr [in/out] 输入的字符串内容指针
   usLen [in] 输入的字符串长度
   pRtn [in/out] 由字符串转换所得整型值

 返 回 值:
    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
    AT_SUCCESS: 成功
******************************************************************************/
static VOS_UINT32 auc2ulDec( VOS_UINT8 *nptr, VOS_UINT16 usLen,  VOS_UINT32 *pRtn)
{
    VOS_UINT32 c        = 0;         /* current Char */
    VOS_UINT32 total    = 0;         /* current total */
    VOS_UINT16 usLength = 0;         /* current Length */
    VOS_UINT8 *pcTmp    = nptr;      /* 从0x后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    c = (VOS_UINT32)*pcTmp++;

    while(usLength++ < usLen)
    {
        /* 字符检查 */
        if(isdigit(c))
        {
            /* 0xFFFFFFFF = 4294967295 */
            if(((total == 429496729) && (c > '5')) || (total > 429496729))
            {
                return AT_FAILURE;
            }

            total = AT_CHECK_BASE_DEC * total + (c - '0');        /* accumulate digit */
            c = (VOS_UINT32)(VOS_UINT8)*pcTmp++;    /* get next Char */
        }
        else
        {
            return AT_FAILURE;
        }
    }

    *pRtn = total;   /* return result, negated if necessary */

    return AT_SUCCESS;
}

/******************************************************************************
 功能描述: 把字符串转成无符号整型值

 参数说明:
   nptr [in/out] 输入的字符串内容指针
   usLen [in] 输入的字符串长度
   pRtn [in/out] 由字符串转换所得整型值

 返 回 值:
    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
    AT_SUCCESS: 成功
******************************************************************************/
VOS_UINT32 atAuc2ul( VOS_UINT8 *nptr,VOS_UINT16 usLen, VOS_UINT32 *pRtn)
{
    /* 进入该函数前，所有参数已进行检查，保证不为NULL */

    if(NULL == nptr || 0 == usLen || NULL == pRtn)
    {
        return AT_FAILURE;
    }

    if('0' == *nptr)
    {
        if(2 < usLen && (('x' == *(nptr + 1)) || ('X' == *(nptr + 1))))
        {
            return auc2ulHex(nptr, usLen, pRtn);
        }
        else
        {
        }
    }

    return auc2ulDec(nptr, usLen, pRtn);
}
VOS_VOID* At_HeapAllocD(VOS_UINT32 ulSize)
{
    VOS_VOID* ret = NULL;

    if((ulSize == 0) || (ulSize > (1024*1024)))
    {
        return NULL;
    }

#if (VOS_VXWORKS == VOS_OS_VER)
    ret = (VOS_VOID *)malloc(ulSize);
#elif (VOS_LINUX == VOS_OS_VER)
    ret = (VOS_VOID *)kmalloc(ulSize, GFP_KERNEL);
#else
    ret = (VOS_VOID *)malloc(ulSize);
#endif

    return ret;
}


VOS_VOID At_HeapFreeD(VOS_VOID *pAddr)
{
    if(pAddr == NULL)
    {
        return ;
    }

#if (VOS_VXWORKS == VOS_OS_VER)
    free(pAddr);
#elif (VOS_LINUX == VOS_OS_VER)
    kfree(pAddr);
#else
    free(pAddr);
#endif

    return;
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif
