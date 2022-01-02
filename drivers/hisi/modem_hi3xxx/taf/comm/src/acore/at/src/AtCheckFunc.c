

#include "AtCheckFunc.h"
#include "AtTypeDef.h"
#include "at_common.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*lint -e960*/
#define    THIS_FILE_ID        PS_FILE_ID_AT_CHECKFUNC_C
/*lint +e960*/


VOS_UINT32 atCheckBasicCmdName( VOS_UINT8 Char )
{
    if(    ('e' == Char) || ('E' == Char) || ('v' == Char) || ('V' == Char)
        || ('a' == Char) || ('A' == Char) || ('h' == Char) || ('H' == Char)
        || ('&' == Char) || ('F' == Char) || ('f' == Char)
        || ('i' == Char) || ('I' == Char) || ('t' == Char) || ('T' == Char)
        || ('p' == Char) || ('P' == Char) || ('x' == Char) || ('X' == Char)
        || ('z' == Char) || ('Z' == Char) || ('c' == Char) || ('C' == Char)
        || ('d' == Char) || ('D' == Char) || ('q' == Char) || ('Q' == Char)
        || ('s' == Char) || ('S' == Char) || ('o' == Char) || ('O' == Char)
      )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}





/******************************************************************************
 功能描述: 比较、匹配十六进制(0x或0X)

 参数说明:
   Char [in] 需检查的字符

 返 回 值: AT_SUCCESS --- 成功
           AT_FAILURE --- 失败
******************************************************************************/
VOS_UINT32 atCheckHex(VOS_UINT8 Char)
{
    if('x' == Char || 'X' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}


/******************************************************************************
 功能描述: 比较、匹配十六进制数字

 参数说明:
   Char [in] 需检查的字符

 返 回 值: AT_SUCCESS --- 成功
           AT_FAILURE --- 失败
******************************************************************************/
VOS_UINT32 atCheckHexNum(VOS_UINT8 Char)
{
    if(isdigit(Char)
        || ('a' <= Char && 'f' >= Char)
        || ('A' <= Char && 'F' >= Char))
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}



/******************************************************************************
 功能描述: 比较、匹配无双引号包括字母起始

 参数说明:
   Char [in] 需检查的字符

 返 回 值: AT_SUCCESS --- 成功
           AT_FAILURE --- 失败
******************************************************************************/
VOS_UINT32 atNoQuotLetter(VOS_UINT8 Char)
{
    if('@' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}



VOS_UINT32 atCheckblank( VOS_UINT8 Char )
{
    /* 添加-支持作为字符 */
    if(' ' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
VOS_UINT32 atCheckComma( VOS_UINT8 Char )
{
    if( ',' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}


VOS_UINT32 atCheckColon( VOS_UINT8 Char )
{
    if( ':' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}



VOS_UINT32 atCheckChar0x2f( VOS_UINT8 Char )
{
    if( '/' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}


/******************************************************************************
 功能描述: 比较匹配十六进制数字

 参数说明:
   usLen [in]     数字长度
   pcTmp [in] 数字串

 返 回 值:
    AT_SUCCESS : 匹配
    AT_FAILURE : 不匹配
******************************************************************************/
static VOS_UINT32 checkHexNumString(VOS_UINT16 usLen,  VOS_UINT8 *pcData)
{
    VOS_UINT16 usLength = 2;          /* 从0x后开始比较 */
    VOS_UINT8 *pcTmp    = pcData + 2; /* 从0x后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    while(usLength++ < usLen)
    {
        /* 判断是否是数字 */
        if(isdigit(*pcTmp)
            || ('A' <= *pcTmp && 'F' >= *pcTmp)
            || ('a' <= *pcTmp && 'f' >= *pcTmp))
        {
            pcTmp++;
        }
        else
        {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}

#if 0
/******************************************************************************
 功能描述: 比较匹配八进制数字

 参数说明:
   usLen [in]     数字长度
   pcTmp [in] 数字串

 返 回 值:
    AT_SUCCESS : 匹配
    AT_FAILURE : 不匹配
******************************************************************************/
static VOS_UINT32 checkOctNumString(VOS_UINT16 usLen,  VOS_UINT8 *pcData)
{
    VOS_UINT16 usLength = 1;          /* 从0后开始比较 */
    VOS_UINT8 *pcTmp    = pcData + 1; /* 从0后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    while(usLength++ < usLen)
    {
        /* 判断是否是数字 */
        if((*pcTmp >= '0') && (*pcTmp <= '7'))
        {
            pcTmp++;
        }
        else
        {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}
#endif

/******************************************************************************
 功能描述: 比较匹配十进制数字

 参数说明:
   usLen [in]     数字长度
   pcTmp [in] 数字串

 返 回 值:
    AT_SUCCESS : 匹配
    AT_FAILURE : 不匹配
******************************************************************************/
static VOS_UINT32 checkDecNumString(VOS_UINT16 usLen,  VOS_UINT8 *pcData)
{
    VOS_UINT16 usLength = 0;      /* 从0后开始比较 */
    VOS_UINT8 *pcTmp    = pcData; /* 从0后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    while(usLength++ < usLen)
    {
        /* 判断是否是数字 */
        if(isdigit(*pcTmp))
        {
            pcTmp++;
        }
        else
        {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}


/******************************************************************************
 功能描述: 比较、匹配数字

 参数说明:
   pData [in] 需检查的字符串
   usLen [in] 需检查的长度

 返 回 值: AT_SUCCESS --- 成功
           AT_FAILURE --- 失败
******************************************************************************/
VOS_UINT32 atCheckNumString( VOS_UINT8 *pData, VOS_UINT16 usLen)
{
    VOS_UINT8 *pcTmp    = pData;

    if((NULL == pData) || (0 == usLen))
    {
        return AT_FAILURE;
    }

    pcTmp = pData;

    if('0' == *pcTmp)
    {
        if(2 < usLen && (('x' == *(pcTmp + 1)) || ('X' == *(pcTmp + 1))))
        {
            return checkHexNumString(usLen, pcTmp);
        }
#if 0
        else if(1 < usLen)
        {
            return checkOctNumString(usLen, pcTmp);
        }
#endif
        else
        {
        }
    }

    return checkDecNumString(usLen, pcTmp);
}



VOS_UINT32 atCheckRightArrowStr( VOS_UINT8 *pData,VOS_UINT16 usLen )
{
    VOS_UINT16 usLength = 0;
    VOS_UINT8 *pcTmp    = pData;

    while( usLength++ < usLen )
    {
        /* 判断是否是MMI字符 */
        if(*pcTmp == '>')
        {
            return AT_SUCCESS;
        }
        else
        {
            pcTmp++;
        }
    }
    return AT_FAILURE;
}
VOS_UINT32 atCheckMmiString( VOS_UINT8 *pData,VOS_UINT16 usLen )
{
    VOS_UINT16 usLength = 0;
    VOS_UINT8 *pcTmp    = pData;

    while( usLength++ < usLen )
    {
        /* 判断是否是MMI字符 */
        if((*pcTmp == '*') || (*pcTmp == '#') )
        {
            return AT_SUCCESS;
        }
        else
        {
            pcTmp++;
        }
    }
    return AT_FAILURE;
}
VOS_UINT32 At_CheckStringPara( AT_PARSE_PARA_TYPE_STRU *pPara)
{
    VOS_UINT8 *pData = pPara->aucPara;

    /* 检查是否是字符串 */
    if (pPara->usParaLen < 2)
    {
        return AT_FAILURE;
    }

    if ( ('"' != *pData) || ('"' != *((pData + pPara->usParaLen) - 1)))
    {
        return AT_FAILURE;
    }

    /* 去除两头的双引号 */
    pPara->usParaLen -= 2;       /*修改参数长度*/
    PS_MEM_CPY(pData, pData+1, pPara->usParaLen);

    pData[pPara->usParaLen] = '\0';

    return AT_SUCCESS;
}


/******************************************************************************
 功能描述: 比较、匹配无双引号包括字符串类型

 参数说明:
   pPara [in/out] 需检查的字符串

 返 回 值: AT_SUCCESS --- 字符串的首尾都是 "
           AT_FAILURE --- 字符串的首尾不全是 "
******************************************************************************/
VOS_UINT32 atCheckNoQuotStringPara( AT_PARSE_PARA_TYPE_STRU *pPara)
{
    VOS_UINT8 *pData = pPara->aucPara;

    /* 检查是否是带引号的字符串 */
    if ( ('"' == *pData) && ('"' == *((pData + pPara->usParaLen) - 1)))
    {
        if (pPara->usParaLen < 2)
        {
            return AT_FAILURE;
        }

        /* 去除两头的双引号 */
        pPara->usParaLen -= 2;       /*修改参数长度*/
        PS_MEM_CPY(pData, pData+1, pPara->usParaLen);

        pData[pPara->usParaLen] = '\0';
    }

    return AT_SUCCESS;
}
VOS_UINT32 atCheckNumPara(AT_PARSE_PARA_TYPE_STRU *pPara)
{
    VOS_UINT32 i  = 0;

    /* 检查字符串 */
    if(AT_SUCCESS == atCheckNumString(pPara->aucPara,pPara->usParaLen))
    {
        if(AT_SUCCESS != atAuc2ul(pPara->aucPara,pPara->usParaLen,&pPara->ulParaValue))
        {
            return AT_FAILURE;
        }

        for(i = 0; i< g_stATParseCmd.ucParaNumRangeIndex; i++)
        {
            /* 如果参数值匹配 */
            if( (g_stATParseCmd.astParaNumRange[i].ulSmall <= pPara->ulParaValue)
                && (pPara->ulParaValue <= g_stATParseCmd.astParaNumRange[i].ulBig) )
            {
                return AT_SUCCESS;
            }
        }
    }

    return AT_FAILURE;
}


VOS_UINT32 atCheckCharPara( AT_PARSE_PARA_TYPE_STRU *pPara)
{
    VOS_UINT8 ucIndex  = 0;
    VOS_UINT8 pBuf[AT_PARA_MAX_LEN + 1] = {0};

    PS_MEM_CPY(pBuf, pPara->aucPara, pPara->usParaLen);

    At_UpString(pBuf, pPara->usParaLen);

    pBuf[pPara->usParaLen] = '\0';

    for(ucIndex = 0; ucIndex < g_stATParseCmd.ucParaStrRangeIndex; ucIndex++)
    {
        if(0 == AT_STRCMP((VOS_CHAR *)g_stATParseCmd.auStrRange[ucIndex],(VOS_CHAR *)pBuf))
        {
            pPara->ulParaValue = ucIndex;

            return AT_SUCCESS;
        }
    }

    return AT_FAILURE;
}


/*****************************************************************************
 Prototype      : At_CheckSemicolon
 Description    : 比较、匹配分号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckSemicolon( TAF_UINT8 Char )
{
    if( ';' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckDialNum( TAF_UINT8 Char )
{
    if ((Char >= '0') && (Char <= '9'))
    {
        return AT_SUCCESS;
    }
    else if ((Char >= 'a') && (Char <= 'c'))
    {
        return AT_SUCCESS;
    }
    else if ((Char >= 'A') && (Char <= 'C'))
    {
        return AT_SUCCESS;
    }
    else if (('*' == Char) || ('#' == Char) || ('+' == Char))
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckDmChar( TAF_UINT8 Char )
{
    if( ('*' != Char) && ('#' != Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckCharStar
 Description    : 比较、匹配字符*
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckCharStar( TAF_UINT8 Char )
{
    if( '*' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckCharWell
 Description    : 比较、匹配字符#
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckCharWell( TAF_UINT8 Char )
{
    if( '#' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckCharRightArrow
 Description    : 比较、匹配字符>
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckCharRightArrow( TAF_UINT8 Char )
{
    if( '>' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharG( TAF_UINT8 Char )
{
    if( ('G' == Char) || ('g' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharI( TAF_UINT8 Char )
{
    if( ('I' == Char) || ('i' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharS( TAF_UINT8 Char )
{
    if( ('s' == Char) || ('S' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharD( TAF_UINT8 Char )
{
    if( ('d' == Char) || ('D' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckBasicCmdName( TAF_UINT8 Char )
{
    if( ('e' == Char) || ('E' == Char) || ('v' == Char) || ('V' == Char)
        || ('a' == Char) || ('A' == Char) || ('h' == Char) || ('H' == Char)
        || ('&' == Char) || ('F' == Char) || ('f' == Char) || ('i' == Char)
        || ('I' == Char) || ('T' == Char) || ('t' == Char) || ('P' == Char)
        || ('p' == Char) || ('X' == Char) || ('x' == Char) || ('Z' == Char)
        || ('z' == Char) || ('C' == Char) || ('c' == Char) || ('D' == Char)
        || ('d' == Char) || ('Q' == Char) || ('q' == Char)
        )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharA( TAF_UINT8 Char )
{
    if( ('a' == Char) || ('A' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharT( TAF_UINT8 Char )
{
    if( ('t' == Char) || ('T' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharPlus( TAF_UINT8 Char )
{
    if( ('+' == Char) || ('^' == Char) || ('$' == Char))
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckLeftBracket
 Description    : 比较、匹配字符(
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckLeftBracket( TAF_UINT8 Char )
{
    if( '(' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckRightBracket
 Description    : 比较、匹配字符)
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckRightBracket( TAF_UINT8 Char )
{
    if( ')' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckDigit( TAF_UINT8 Char )
{
    if( (Char >= '0') && (Char <= '9') )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckLetter( TAF_UINT8 Char )
{
    if( ((Char >= 'a') && (Char <= 'z')) || ((Char >= 'A') && (Char <= 'Z')) || ('_' == Char) || ('&' == Char))
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckUpLetter( TAF_UINT8 Char )
{
    if( (Char >= 'A') && (Char <= 'Z') )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckChar( TAF_UINT8 Char )
{
    if( ((Char >= 'a') && (Char <= 'z')) || ((Char >= 'A') && (Char <= 'Z')) || ((Char >= '0') && (Char <= '9')) || ('*' == Char) || ('#' == Char) || ('-' == Char) || ('.' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

/*****************************************************************************
 Prototype      : At_CheckAlpha
 Description    : 比较、匹配字符('a'~'z' 和'A'~'Z')
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2010-07-1
    Author      : ---
    Modification: Created function
*****************************************************************************/

TAF_UINT32 At_CheckAlpha( TAF_UINT8 Char )
{
    if( ((Char >= 'a') && (Char <= 'z')) || ((Char >= 'A') && (Char <= 'Z')) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckColon
 Description    : 比较、匹配逗号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckColon( TAF_UINT8 Char )
{
    if( ',' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckCharSub
 Description    : 比较、匹配减号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckCharSub( TAF_UINT8 Char )
{
    if( '-' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckQuot
 Description    : 比较、匹配引号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckQuot( TAF_UINT8 Char )
{
    if( '"' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckHorizontalLine( TAF_UINT8 Char )
{
    if( '-' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

/*****************************************************************************
 Prototype      : At_CheckEqu
 Description    : 比较、匹配等号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckEqu( TAF_UINT8 Char )
{
    if( '=' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckReq
 Description    : 比较、匹配问号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckReq( TAF_UINT8 Char )
{
    if( '?' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckNoQuot
 Description    : 比较、匹配非引号
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckNoQuot( TAF_UINT8 Char )
{
    if( '"' != Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharE( TAF_UINT8 Char )
{
    if( ('e' == Char) || ('E' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckChar1
 Description    : 比较、匹配字符1
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckChar1( TAF_UINT8 Char )
{
    if( '1' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckChar0
 Description    : 比较、匹配字符0
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckChar0( TAF_UINT8 Char )
{
    if( '0' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharV( TAF_UINT8 Char )
{
    if( ('v' == Char) || ('V' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckCharF( TAF_UINT8 Char )
{
    if( ('f' == Char) || ('F' == Char) )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}
/*****************************************************************************
 Prototype      : At_CheckCharE
 Description    : 比较、匹配字符&
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckChar0x26( TAF_UINT8 Char )
{
    if( '&' == Char)
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

TAF_UINT32 At_CheckNumString( TAF_UINT8 *pData,TAF_UINT16 usLen )
{
    TAF_UINT16 usLength = 0;
    TAF_UINT8 *pcTmp    = pData;

    if( (TAF_NULL_PTR == pData) || (0 == usLen) )
    {
        return AT_FAILURE;
    }

    pcTmp = pData;

    while( usLength++ < usLen )
    {
        /*判断是否是数字*/
        if( (*pcTmp >= '0') && (*pcTmp <= '9') )
        {
            pcTmp++;
        }
        else
        {
            return AT_FAILURE;
        }
    }
    return AT_SUCCESS;
}
TAF_UINT32 At_CheckNumCharString( TAF_UINT8 *pData,TAF_UINT16 usLen )
{
    TAF_UINT16 usLength = 0;
    TAF_UINT8 *pcTmp    = pData;
    TAF_UINT32 ulCheckDigitRslt;
    TAF_UINT32 ulCheckCharRslt;

    if( (TAF_NULL_PTR == pData) || (0 == usLen) )
    {
        return AT_FAILURE;
    }

    pcTmp = pData;

    while( usLength++ < usLen )
    {
       ulCheckDigitRslt = At_CheckDigit(*pcTmp);
       ulCheckCharRslt = At_CheckAlpha(*pcTmp);

       if ((AT_SUCCESS == ulCheckDigitRslt)|| (AT_SUCCESS ==ulCheckCharRslt))
       {
            pcTmp++;
       }
       else
       {
            return AT_FAILURE;
       }
    }
    return AT_SUCCESS;
}

/*****************************************************************************
 Prototype      : At_CheckRightArrowStr
 Description    : 比较、匹配数字
 Input          : pData --- 需检查的字符串
                  usLen --- 需检查的长度
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CheckRightArrowStr( TAF_UINT8 *pData,TAF_UINT16 usLen )
{
    TAF_UINT16 usLength = 0;
    TAF_UINT8 *pcTmp    = pData;

    while( usLength++ < usLen )
    {
        /*判断是否是MMI字符*/
        if(*pcTmp == '>')
        {
            return AT_SUCCESS;
        }
        else
        {
            pcTmp++;
        }
    }
    return AT_FAILURE;
}
TAF_UINT32 At_CheckMmiString( TAF_UINT8 *pData,TAF_UINT16 usLen )
{
    TAF_UINT16 usLength = 0;
    TAF_UINT8 *pcTmp    = pData;

    while( usLength++ < usLen )
    {
        /*判断是否是MMI字符*/
        if((*pcTmp == '*') || (*pcTmp == '#') )
        {
            return AT_SUCCESS;
        }
        else
        {
            pcTmp++;
        }
    }
    return AT_FAILURE;
}

/*****************************************************************************
 Prototype      : At_CheckJuncture
 Description    : 比较、匹配连接符
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 At_CheckJuncture( VOS_UINT8 Char )
{
    if( '-' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}

/*****************************************************************************
 Prototype      : At_CheckEnd
 Description    : 比较、匹配结束符
 Input          : Char---需检查的字符
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 At_CheckEnd( VOS_UINT8 Char )
{
    if( '\0' == Char )
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}


VOS_UINT32 AT_CheckDateFormat(
    VOS_UINT8                           *pucDateStr,
    VOS_UINT32                          ulDateStrLen
)
{
    VOS_UINT8                           ucFirstJuncture;
    VOS_UINT8                           ucSecondJuncture;
    VOS_UINT8                           ucFirstColon;
    VOS_UINT8                           ucSecondColon;
    VOS_UINT8                           ucSpace;

    /* 格式为2009-12-31 10:10:10的日期字符串: 总长度必须为19 */
    if (19 != ulDateStrLen)
    {
        AT_NORM_LOG1("AT_CheckDateFormat: length error.", (VOS_INT32)ulDateStrLen);
        return AT_ERROR;
    }

    /* 格式为2009-12-31 10:10:10的日期字符串: 第5个字节为'-', 第8个字节为'-' */
    ucFirstJuncture     = *(pucDateStr + 4);
    ucSecondJuncture    = *(pucDateStr + 7);
    if (('-' != ucFirstJuncture) || ('-' != ucSecondJuncture))
    {
        AT_NORM_LOG("AT_CheckDateFormat: Fail to check juncture.");
        return AT_ERROR;
    }

    /* 格式为2009-12-31 10:10:10的日期字符串: 第11个字节为' ' */
    ucSpace = *(pucDateStr + 10);
    if (' ' != ucSpace)
    {
        AT_NORM_LOG("AT_CheckDateFormat: Fail to check space.");
        return AT_ERROR;
    }

    /* 格式为2009-12-31 10:10:10的日期字符串: 第14个字节为':' 第17个字节为':' */
    ucFirstColon    = *(pucDateStr + 13);
    ucSecondColon   = *(pucDateStr + 16);
    if ((':' != ucFirstColon) || (':' != ucSecondColon))
    {
        AT_NORM_LOG("AT_CheckDateFormat: Fail to check colon.");
        return AT_ERROR;
    }

    return AT_OK;
}


VOS_UINT32 AT_GetDaysForEachMonth(
    VOS_UINT32                          ulYear,
    VOS_UINT32                          ulMonth
)
{
    if ((1 == ulMonth) || (3 == ulMonth) || (5 == ulMonth) || (7 == ulMonth)
     || (8 == ulMonth) || (10 == ulMonth) || (12 == ulMonth) )
    {
        /* 1,3,5,7,8,10,12月有31天 */
        return 31;
    }
    else if ((4 == ulMonth) || (6 == ulMonth) || (9 == ulMonth) || (11 == ulMonth))
    {
        /* 4,6,9,11月有30天 */
        return 30;
    }
    else
    {
        /* 2月看是否为闰年，是则为29天，否则为28天 */
        if ( ((0 == (ulYear % 4)) && (0 != (ulYear % 100))) || (0 == (ulYear % 400)))
        {
            /* 润年 */
            return 29;
        }
        else
        {
            /* 非润年 */
            return 28;
        }
    }
}
VOS_UINT32  AT_CheckDate(
    AT_DATE_STRU                        *pstDate
)
{
    VOS_UINT32                          ulDaysEachMonth;

    /* 月份有效性检查: 月份数值必须不为0，且小于等于12 */
    if ((pstDate->ulMonth > 12)
     || (0 == pstDate->ulMonth))
    {
        AT_NORM_LOG("AT_CheckDate: Fail to check month.");
        return AT_ERROR;
    }

    /* 日期有效性检查: 日期数值必须不为0，且小于等于当月的总天数 */
    ulDaysEachMonth = AT_GetDaysForEachMonth(pstDate->ulYear, pstDate->ulMonth);
    if ((pstDate->ulDay > ulDaysEachMonth)
     || (0 == pstDate->ulDay))
    {
        AT_NORM_LOG("AT_CheckDate: Fail to check day.");
        return AT_ERROR;
    }

    /* 时间有效性检查: 小时数必须小于24，分钟或秒数必须小于60 */
    if ((pstDate->ulHour >= 24)
     || (pstDate->ulMunite >= 60)
     || (pstDate->ulSecond >= 60))
    {
        AT_NORM_LOG("AT_CheckDate: Fail to check time.");
        return AT_ERROR;
    }

    return AT_OK;
}
VOS_UINT32  AT_GetDate(
    VOS_UINT8                           *pucDateStr,
    VOS_UINT32                          ulDateStrLen,
    AT_DATE_STRU                        *pstDate
)
{
    VOS_UINT32                          ulRet;

    /* 获取日期中的年份: 年份占用4个字节 */
    ulRet = At_Auc2ul(pucDateStr, 4, &pstDate->ulYear);
    if (AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG("AT_GetDate: Fail to get year.");
        return AT_ERROR;
    }

    /* 获取日期中的月份: 月份占用2个字节，偏移年份占用的4个字节和'-'一个字节 */
    ulRet = At_Auc2ul((pucDateStr + 5), 2, &pstDate->ulMonth);
    if (AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG("AT_GetDate: Fail to get month.");
        return AT_ERROR;
    }

    /* 获取日期中的日期: 日期占用2个字节，偏移月份占用的2个字节和'-'一个字节 */
    ulRet = At_Auc2ul((pucDateStr + 8), 2, &pstDate->ulDay);
    if (AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG("AT_GetDate: Fail to get day.");
        return AT_ERROR;
    }

    /* 获取日期中的小时: 小时占用2个字节，偏移日期占用的2个字节和' '一个字节 */
    ulRet = At_Auc2ul((pucDateStr + 11), 2, &pstDate->ulHour);
    if (AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG("AT_GetDate: Fail to get hour.");
        return AT_ERROR;
    }

    /* 获取日期中的分钟: 分钟占用2个字节，偏移小时占用的2个字节和':'一个字节 */
    ulRet = At_Auc2ul((pucDateStr + 14), 2, &pstDate->ulMunite);
    if (AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG("AT_GetDate: Fail to get minute.");
        return AT_ERROR;
    }

    /* 获取日期中的秒钟: 秒钟占用2个字节，偏移分钟占用的2个字节和':'一个字节 */
    ulRet = At_Auc2ul((pucDateStr + 17), 2, &pstDate->ulSecond);
    if (AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG("AT_GetDate: Fail to get second.");
        return AT_ERROR;
    }

    return AT_OK;
}


VOS_UINT32 At_CheckAndParsePara(
    AT_PARSE_PARA_TYPE_STRU             *pstAtPara,
    VOS_UINT8                           *pucBegain,
    VOS_UINT8                           *pucEnd
)
{
    VOS_UINT32                          ulParaLen;

    ulParaLen  = (VOS_UINT32)(pucEnd - pucBegain);

    if ((0 == ulParaLen)
     || (AT_PARA_MAX_LEN < ulParaLen))
    {
        return AT_ERROR;
    }

    pstAtPara->usParaLen = (VOS_UINT16)ulParaLen;

    At_RangeCopy(pstAtPara->aucPara, pucBegain, pucEnd);

    return AT_OK;

}


VOS_UINT32 AT_CheckStrStartWith(
    VOS_UINT8                          *pucLineStr,
    VOS_UINT8                          *pucPefixStr
)
{
    for ( ; (*pucLineStr != '\0') && (*pucPefixStr != '\0'); pucLineStr++, pucPefixStr++)
    {
        if (*pucLineStr != *pucPefixStr)
        {
            return VOS_FALSE;
        }
    }

    if ('\0' == *pucPefixStr)
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

