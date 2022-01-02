

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "DrvInterface.h"
#include "ATCmdProc.h"
#include "AtInputProc.h"
#include "AtCheckFunc.h"
#include "AtParseCmd.h"
#include "PppInterface.h"
#include "AtMsgPrint.h"
#include "Taf_MmiStrParse.h"
#include "siappstk.h"
#include "siapppih.h"
#include "siapppb.h"
#include "omrl.h"
#include "AtPhyInterface.h"
#include "AtDataProc.h"
#include "AtInputProc.h"
#include "TafDrvAgent.h"
#include "MnCommApi.h"
#include "AtCmdMsgProc.h"
#include "UsimmApi.h"
#include "AtRabmInterface.h"
#include "AtSndMsg.h"
#include "AtDeviceCmd.h"
#include "AtRnicInterface.h"
#include "AtTafAgentInterface.h"
#include "TafAgentInterface.h"
#include "spysystem.h"
#include "ipcmailboxinterface.h"

#include "AtMtaInterface.h"
#include "phyoaminterface.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#if (FEATURE_ON==FEATURE_LTE)
#include "msp_nvim.h"
#include "msp_nv_def.h"
#include "msp_nv_id.h"
#include "at_common.h"
#include "gen_msg.h"
#include "at_lte_common.h"
#endif
#include "MnCallApi.h"
#include "AtCtx.h"
#include "AppVcApi.h"
#include "AtImsaInterface.h"
#if (FEATURE_ON == FEATURE_AGPS)
#if (VOS_OS_VER == VOS_LINUX)
#include <linux/huawei/gps/huawei_gps.h>
#include <linux/clk.h>
#else
#include <Linuxstub.h>
#endif
#endif

#include "AtCmdSimProc.h"
#include "TafAppMma.h"

#include "TafMtcApi.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_AT_SETPARACMD_C


/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
AT_SET_PORT_PARA_MAP_STRU    g_astSetPortParaMap[AT_SETPORT_DEV_MAP_LEN]
                                = {{"A1", AT_DEV_CDROM,        "CDROM"        },
                                   {"A2", AT_DEV_SD,           "SD"           },
                                   {"A3", AT_DEV_RNDIS,        "RNDIS"        },
                                   {"A",  AT_DEV_BLUE_TOOTH,   "BLUE TOOTH"   },
                                   {"B",  AT_DEV_FINGER_PRINT, "FINGER PRINT" },
                                   {"D",  AT_DEV_MMS,          "MMS"          },
#if(FEATURE_ON == FEATURE_LTE)
                                   {"E",  AT_DEV_PC_VOICE,     "3G PC VOICE"  },
                                   {"1",  AT_DEV_MODEM,        "3G MODEM"     },
                                   {"2",  AT_DEV_PCUI,         "3G PCUI"      },
                                   {"3",  AT_DEV_DIAG,         "3G DIAG"      },
#else
                                   {"E",  AT_DEV_PC_VOICE,     "PC VOICE"     },
                                   {"1",  AT_DEV_MODEM,        "MODEM"        },
                                   {"2",  AT_DEV_PCUI,         "PCUI"         },
                                   {"3",  AT_DEV_DIAG,         "DIAG"         },
#endif
                                   {"4",  AT_DEV_PCSC,         "PCSC"         },
#if(FEATURE_ON == FEATURE_LTE)
                                   {"5",  AT_DEV_GPS,          "3G GPS"       },
#else
                                   {"5",  AT_DEV_GPS,          "GPS"          },
#endif
                                   {"6",  AT_DEV_GPS_CONTROL,  "GPS CONTROL"  },
                                   {"7",  AT_DEV_NDIS,         "3G NDIS"      },
                                   {"16", AT_DEV_NCM,          "NCM"          },
                                   {"10",AT_DEV_4G_MODEM,      "4G MODEM"        },
                                   {"11",AT_DEV_4G_NDIS,       "4G NDIS"         },
                                   {"12",AT_DEV_4G_PCUI,       "4G PCUI"         },
                                   {"13",AT_DEV_4G_DIAG,       "4G DIAG"         },
                                   {"14",AT_DEV_4G_GPS,        "4G GPS"          },
                                   {"15",AT_DEV_4G_PCVOICE,    "4G PCVOICE"      },
                                   {"FF", AT_DEV_NONE,         "NO FIRST PORT"}
                                  };

TAF_UINT8                               gucSTKCmdQualify    =0x0;

/*AT/OM通道的链路索引*/
TAF_UINT8                               gucAtOmIndex        = AT_MAX_CLIENT_NUM;

/*纪录查询错误码的类型*/
TAF_UINT32                              gulErrType          = 1;


/*NAS收到APP下发NDIS拨号命令的slice*/
VOS_UINT32                              g_ulRcvAppNdisdupSlice;

VOS_BOOL                                g_bSetFlg = VOS_FALSE;

VOS_UINT32                              g_ulWifiFreq;
VOS_UINT32                              g_ulWifiRate;
VOS_UINT32                              g_ulWifiMode;
VOS_INT32                               g_lWifiPower;
#if(FEATURE_ON == FEATURE_LTE)
VOS_UINT32                              g_ulWifiRF = 0xffffffffU;
AT_TMODE_RAT_FLAG_STRU g_stTmodeRat = {0};
VOS_UINT32 g_ulTmodeNum             = 0;
VOS_UINT32 g_ulGuTmodeCnfNum        = 0;
VOS_UINT32 g_ulLteOnly              = 0;
VOS_UINT32 g_ulGuOnly               = 0;
VOS_UINT32 g_ulLteIsSend2Dsp        = 0;

#endif
VOS_UINT                                g_ulUcastWifiRxPkts;
VOS_UINT                                g_ulMcastWifiRxPkts;

/* +CLCK命令参数CLASS与Service Type Code对应表 */
AT_CLCK_CLASS_SERVICE_TBL_STRU          g_astClckClassServiceTbl[] = {
    {AT_CLCK_PARA_CLASS_VOICE,                      TAF_SS_TELE_SERVICE,        TAF_ALL_SPEECH_TRANSMISSION_SERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_DATA,                       TAF_SS_BEARER_SERVICE,      TAF_ALL_BEARERSERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_FAX,                        TAF_SS_TELE_SERVICE,        TAF_ALL_FACSIMILE_TRANSMISSION_SERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE_FAX,                  TAF_SS_TELE_SERVICE,        TAF_ALL_TELESERVICES_EXEPTSMS_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE_DATA_FAX,             TAF_SS_TELE_SERVICE,        TAF_ALL_TELESERVICES_EXEPTSMS_TSCODE},
    {AT_CLCK_PARA_CLASS_SMS,                        TAF_SS_TELE_SERVICE,        TAF_ALL_SMS_SERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE_FAX_SMS,              TAF_SS_TELE_SERVICE,        TAF_ALL_TELESERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE_DATA_FAX_SMS,         TAF_SS_TELE_SERVICE,        TAF_ALL_TELESERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_ALL_DATA_CIRCUIT_SYNCHRONOUS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_ALL_DATA_CIRCUIT_ASYNCHRONOUS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PKT,                   TAF_SS_BEARER_SERVICE,      TAF_ALL_DATAPDS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC_PKT,              TAF_SS_BEARER_SERVICE,      TAF_ALL_SYNCHRONOUS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_ALL_PADACCESSCA_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC_PAD,             TAF_SS_BEARER_SERVICE,      TAF_ALL_ASYNCHRONOUS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC_ASYNC_PKT_PKT,    TAF_SS_BEARER_SERVICE,      TAF_ALL_BEARERSERVICES_BSCODE},
};

MTA_BODY_SAR_STATE_ENUM_UINT16          g_enAtBodySarState = MTA_BODY_SAR_OFF;

/*****************************************************************************
   3 函数、变量声明
*****************************************************************************/

#ifndef _PS_COMPILE_EDGE_ADAPT_MOIRI_B073_
extern void       TTF_SetGcfTestFlag(VOS_UINT32 ulGctTestFlag);
#endif

extern VOS_VOID GU_OamCtrlPortInit(VOS_VOID);

#if (VOS_WIN32 == VOS_OS_VER)
/* 输出TAF的回放数据 */
#ifndef __PS_WIN32_RECUR__
VOS_VOID NAS_MMA_SndNVData(VOS_VOID);
VOS_UINT32 NAS_MSG_SndOutsideContextData_Part1(VOS_VOID);
VOS_UINT32 NAS_MSG_SndOutsideContextData_Part2(VOS_VOID);
VOS_UINT32 NAS_MSG_SndOutsideContextData_Part3(VOS_VOID);
#endif

extern VOS_UINT32 NAS_SetLteRplmnId(TAF_PLMN_ID_STRU *pstPlmnId,
                                                                      VOS_UINT32               ulLai,
                                                                      VOS_UINT32 ulRac);


/* Modified by c00318887 for DPlmn扩容和优先接入HPLMN, 2015-5-18, begin */
VOS_VOID NAS_STUB_AT_ResetNplmn(VOS_VOID);
/* Modified by c00318887 for DPlmn扩容和优先接入HPLMN, 2015-5-18, end */

extern VOS_UINT32 NAS_SetDamCfg(
     VOS_UINT8 ucEnableLteTrigPlmnSearch,
     VOS_UINT8 ucAddDamPlmnInDisabledPlmnList,
     VOS_UINT8 ucImsiPlmnNum,
     TAF_PLMN_ID_STRU *pstImsiPlmnList,
     VOS_UINT8 ucDamPlmnNum,
     TAF_PLMN_ID_STRU *pstDamPlmnList);
extern  VOS_UINT32 NAS_SetTinType(VOS_UINT8  ucTinType);
extern VOS_UINT32 NAS_SetPsBearerIsrFlg(VOS_UINT8  ucNsapi,VOS_UINT8  ucPdpIsrFlg,VOS_UINT8 ucPdpStatus);

/* Added by c00318887 for file refresh需要触发背景搜, 2015-4-28, begin */
extern VOS_UINT32 NAS_SetHighPrioPlmnRefreshTriggerBGSearchCfg(VOS_UINT8 ulOnOff, VOS_UINT32 ulDelayLen);
/* Added by c00318887 for file refresh需要触发背景搜, 2015-4-28, end */


extern VOS_UINT32 NAS_SetPsRegContainDrx(VOS_UINT8 ucDrx);
extern  VOS_UINT32 NAS_SetLteImsSupportFlag(VOS_UINT8 ucLteImsSupportFlg, VOS_UINT8 ucLteEmsSupportFlg);
extern  VOS_UINT32 NAS_SetImsCapabilitySupportFlag(
    VOS_UINT8                           ucVoiceCallOnImsSupportFlg,
    VOS_UINT8                           ucVideoCallOnImsSupportFlg,
    VOS_UINT8                           ucSmsOnImsSupportFlg
);
extern  VOS_UINT32 NAS_SetPreferDomain(VOS_UINT8 ucVoiceDomain, VOS_UINT8 ucSmsDomain);

extern VOS_UINT32 NAS_SetImsRoaming(VOS_UINT8 ucImsRoamingFlag);
extern VOS_UINT32 NAS_SetRedialCfg(
    VOS_UINT8                           ucCallRedialImsToCs,
    VOS_UINT8                           ucSmsRedialImsToCs,
    VOS_UINT8                           ucSsRedialImsToCs,
    VOS_UINT8                           ucCallRedialCsToIms,
    VOS_UINT8                           ucSmsRedialCsToIms,
    VOS_UINT8                           ucSsRedialCsToIms
);

extern  VOS_UINT32 NAS_SetImsVoiceInterSysLauEnable(VOS_UINT8 ucLauFlag);
extern  VOS_UINT32 NAS_SetImsVoiceMMEnableFlg(VOS_UINT8 ucMMFlag);


extern VOS_UINT32 NAS_SetImsUssd(VOS_UINT8 ucImsUssdFlag);
#endif


/*****************************************************************************
   4 函数实现
*****************************************************************************/

VOS_UINT32 At_SetSecuBootPara(VOS_UINT8 ucIndex)
{

    if ((1 != gucAtParaIndex)
     || (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (SECURE_ENABLE != gastAtParaList[0].ulParaValue)
    {
        return AT_ERROR;
    }

    /* 只有启动硬件加密的情况下，才能设置 */
    if (TAF_SUCCESS == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_SECUBOOT_SET_REQ,
                                      VOS_NULL_PTR,
                                      0,
                                      I0_WUEPS_PID_DRV_AGENT))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SECUBOOT_SET;           /*设置当前操作模式 */
        return AT_WAIT_ASYNC_RETURN;                                            /* 等待异步事件返回 */
    }
    else
    {
        return AT_ERROR;
    }
}
PPP_AUTH_TYPE_ENUM_UINT8 AT_CtrlGetPDPAuthType(
    VOS_UINT32                          Value,
    VOS_UINT16                          usTotalLen
)
{

    /*获取验证方法*/
    if (0 == usTotalLen)
    {
        return PPP_NO_AUTH_TYPE;
    }
    else
    {
        if(1 == Value)
        {
            return PPP_PAP_AUTH_TYPE;
        }
        else
        {
            return PPP_CHAP_AUTH_TYPE;
        }
    }
}
TAF_UINT32 At_SsPrint2Class(TAF_SS_BASIC_SERVICE_STRU *pBsService,TAF_UINT8 ucClass)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulItemsNum;

    ulItemsNum = sizeof(g_astClckClassServiceTbl) / sizeof(AT_CLCK_CLASS_SERVICE_TBL_STRU);

    /* 查表填写对应Class的服务类型及服务码 */
    for (ulLoop = 0; ulLoop < ulItemsNum; ulLoop++)
    {
        if (g_astClckClassServiceTbl[ulLoop].enClass == ucClass)
        {
            pBsService->BsType = g_astClckClassServiceTbl[ulLoop].enServiceType;
            pBsService->BsServiceCode = g_astClckClassServiceTbl[ulLoop].enServiceCode;
            return AT_SUCCESS;
        }
    }

    return AT_FAILURE;
}

/*****************************************************************************
 Prototype      : At_SsPrint2Code
 Description    :
 Input          : ucClass --- SSA的Code
 Output         : ---
 Return Value   : ulRtn输出结果
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_SsPrint2Code(TAF_SS_CODE *pSsCode,TAF_UINT8 ucType)
{
    switch(ucType)
    {
    case AT_CLCK_AO_TYPE:
        *pSsCode = TAF_BAOC_SS_CODE;
        break;

    case AT_CLCK_OI_TYPE:
        *pSsCode = TAF_BOIC_SS_CODE;
        break;

    case AT_CLCK_OX_TYPE:
        *pSsCode = TAF_BOICEXHC_SS_CODE;
        break;

    case AT_CLCK_AI_TYPE:
        *pSsCode = TAF_BAIC_SS_CODE;
        break;

    case AT_CLCK_IR_TYPE:
        *pSsCode = TAF_BICROAM_SS_CODE;
        break;

    case AT_CLCK_AB_TYPE:
        *pSsCode = TAF_ALL_BARRING_SS_CODE;
        break;

    case AT_CLCK_AG_TYPE:
        *pSsCode = TAF_BARRING_OF_OUTGOING_CALLS_SS_CODE;
        break;

    default:
        *pSsCode = TAF_BARRING_OF_INCOMING_CALLS_SS_CODE;
        break;
    }
}

#if ( VOS_WIN32 == VOS_OS_VER )

VOS_UINT32 AT_ReadValueFromNvim(
    const NV_ID_ENUM_U16                enNVItemType,
    VOS_VOID                           *pData,
    VOS_UINT32                          usDataLen,
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    VOS_UINT32                          ulRst;
    ulRst = NV_ReadEx(enModemId, enNVItemType, pData, usDataLen);
    switch ( ulRst)
    {
        case NV_OK :
            return AT_OK;
        case NV_ID_NOT_EXIST:
        case NV_BUFFER_TOO_LONG :
            return AT_CME_INCORRECT_PARAMETERS;
        default:
            return AT_ERROR ;
    }
}
#endif

TAF_UINT32 At_AsciiNum2HexString(TAF_UINT8 *pucSrc, TAF_UINT16 *pusSrcLen)
{
    TAF_UINT16 usChkLen = 0;
    TAF_UINT16 usTmp = 0;
    TAF_UINT8  ucLeft = 0;
    TAF_UINT16 usSrcLen = *pusSrcLen;
    TAF_UINT8 *pucDst = pucSrc;

    /* 如果是奇数个半字节则返回错误 */
    if(0 != (usSrcLen % 2))
    {
        return AT_FAILURE;
    }

    while(usChkLen < usSrcLen)
    {
        if( (pucSrc[usChkLen] >= '0') && (pucSrc[usChkLen] <= '9') ) /* the number is 0-9 */
        {
            ucLeft = pucSrc[usChkLen] - '0';
        }
        else if( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') ) /* the number is 0-9 */
        {
            ucLeft = (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        else if( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') ) /* the number is 0-9 */
        {
            ucLeft = (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return AT_FAILURE;
        }

        pucDst[usTmp] = 0xf0 & (ucLeft << 4);
        usChkLen += 1;

        if( (pucSrc[usChkLen] >= '0') && (pucSrc[usChkLen] <= '9') ) /* the number is 0-9 */
        {
            pucDst[usTmp] |= pucSrc[usChkLen] - '0';
        }
        else if( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') ) /* the number is 0-9 */
        {
            pucDst[usTmp] |= (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        else if( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') ) /* the number is 0-9 */
        {
            pucDst[usTmp] |= (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return AT_FAILURE;
        }

        usChkLen += 1;
        usTmp += 1;
    }

    *pusSrcLen = usSrcLen / 2;
    return AT_SUCCESS;
}
TAF_UINT32 At_AsciiString2HexText(SI_STK_TEXT_STRING_STRU *pTextStr,TAF_UINT8 *pucSrc,TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucTmp = 0;
    TAF_UINT8  ucLeft = 0;
    TAF_UINT8 *pucDst = pTextStr->pucText;

    if(usSrcLen < 2)
    {
        return AT_FAILURE;
    }

    while(usChkLen < usSrcLen)
    {
        if( (pucSrc[usChkLen] >= '0') && (pucSrc[usChkLen] <= '9') ) /* the number is 0-9 */
        {
            ucLeft = pucSrc[usChkLen] - '0';
        }
        else if( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') ) /* the number is 0-9 */
        {
            ucLeft = (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        else if( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') ) /* the number is 0-9 */
        {
            ucLeft = (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return AT_FAILURE;
        }

        pucDst[ucTmp] = 0xf0 & (ucLeft << 4);

        usChkLen += 1;

        if( (pucSrc[usChkLen] >= '0') && (pucSrc[usChkLen] <= '9') ) /* the number is 0-9 */
        {
            pucDst[ucTmp] |= pucSrc[usChkLen] - '0';
        }
        else if( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') ) /* the number is 0-9 */
        {
            pucDst[ucTmp] |= (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        else if( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') ) /* the number is 0-9 */
        {
            pucDst[ucTmp] |= (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return AT_FAILURE;
        }

        usChkLen += 1;

        ucTmp += 1;
    }

    pTextStr->ucLen = ucTmp;

    return AT_SUCCESS;
}


TAF_UINT32 At_AsciiString2HexSimple(TAF_UINT8 *pTextStr,TAF_UINT8 *pucSrc,TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucTmp = 0;
    TAF_UINT8  ucLeft = 0;
    TAF_UINT8 *pucDst = pTextStr;

    if(usSrcLen == 0)
    {
        return AT_FAILURE;
    }

    while(usChkLen < usSrcLen)
    {
        if ( (pucSrc[usChkLen] >= '0') && (pucSrc[usChkLen] <= '9') ) /* the number is 0-9 */
        {
            ucLeft = pucSrc[usChkLen] - '0';
        }
        else if ( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') ) /* the number is 0-9 */
        {
            ucLeft = (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        else if ( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') ) /* the number is 0-9 */
        {
            ucLeft = (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return AT_FAILURE;
        }

        pucDst[ucTmp] = 0xf0 & (ucLeft << 4);

        usChkLen += 1;

        if ( (pucSrc[usChkLen] >= '0') && (pucSrc[usChkLen] <= '9') ) /* the number is 0-9 */
        {
            pucDst[ucTmp] |= pucSrc[usChkLen] - '0';
        }
        else if ( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') ) /* the number is 0-9 */
        {
            pucDst[ucTmp] |= (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        else if ( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') ) /* the number is 0-9 */
        {
            pucDst[ucTmp] |= (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return AT_FAILURE;
        }

        usChkLen += 1;

        ucTmp += 1;
    }

    return AT_SUCCESS;
}

/* Del At_AbortCmdProc */

TAF_UINT32 At_AsciiNum2BcdNum (TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucTmp = 0;
    TAF_UINT8  ucBcdCode;

    for(usChkLen = 0; usChkLen < usSrcLen; usChkLen++)
    {
        ucTmp = usChkLen % 2;   /* 判断高低位 */

        if ((pucSrc[usChkLen] >= 0x30) && (pucSrc[usChkLen] <= 0x39)) /* the number is 0-9 */
        {
            ucBcdCode = pucSrc[usChkLen] - 0x30;
        }
        else if ('*' == pucSrc[usChkLen])
        {
            ucBcdCode = 0x0a;
        }
        else if ('#' == pucSrc[usChkLen])
        {
            ucBcdCode = 0x0b;
        }
        else if (('a' == pucSrc[usChkLen])
              || ('b' == pucSrc[usChkLen])
              || ('c' == pucSrc[usChkLen]))
        {
            ucBcdCode = (VOS_UINT8)((pucSrc[usChkLen] - 'a') + 0x0c);
        }
        else
        {
            return AT_FAILURE;
        }

        ucTmp = usChkLen % 2;   /* 判断高低位 */
        if(0 == ucTmp)
        {
            pucDst[usChkLen/2] = ucBcdCode;   /* 低位 */
        }
        else
        {
            pucDst[usChkLen/2] |= (TAF_UINT8)(ucBcdCode << 4);   /* 高位 */
        }
    }

    if(1 == (usSrcLen % 2))
    {
        pucDst[usSrcLen/2] |= 0xf0; /* 高位 */
    }

    return AT_SUCCESS;
}

TAF_UINT32 At_AsciiNum2Num (TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usChkLen = 0;

    for(usChkLen = 0; usChkLen < usSrcLen; usChkLen++)
    {

        if( (pucSrc[usChkLen] >= 0x30) && (pucSrc[usChkLen] <= 0x39) ) /* the number is 0-9 */
        {
                pucDst[usChkLen] = pucSrc[usChkLen] - 0x30;   /* 低位 */
        }
        else
        {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}

TAF_UINT32 At_CheckDialString(TAF_UINT8 *pData, TAF_UINT16 usLen)
{
    TAF_UINT16 usCount = 0;
    TAF_UINT8 *pTmp = pData;

    if ('+' == *pTmp)
    {
        pTmp++;
        usCount++;
    }

    while(usCount++ < usLen)
    {
        if ((*pTmp >= '0') && (*pTmp <= '9'))
        {
        }
        else if ((*pTmp >= 'a') && (*pTmp <= 'c'))
        {
        }
        else if ((*pTmp >= 'A') && (*pTmp <= 'C'))
        {
        }
        else if (('*' == *pTmp) || ('#' == *pTmp))
        {
        }
        else
        {
            return AT_FAILURE;
        }
        pTmp++;
    }
    return AT_SUCCESS;
}


TAF_UINT32 At_CheckPBString(TAF_UINT8 *pData, TAF_UINT16 *pusLen)
{
    TAF_UINT16 usCount = 0;
    TAF_UINT8 *pTmp = pData;
    TAF_UINT8  usInvalidNum = 0;

    while(usCount++ < *pusLen)
    {
        if(('(' == *pTmp)||(')' == *pTmp)||('-' == *pTmp)||('"' == *pTmp))
        {
            VOS_MemCpy(pTmp, pTmp+1, (VOS_SIZE_T)(*pusLen - usCount));
            usInvalidNum++;
        }
        else
        {
            pTmp++;
        }
    }

    usCount = 0;
    pTmp = pData;
    *pusLen -= usInvalidNum;

    while(usCount++ < *pusLen)
    {
        if(!( (('0' <= *pTmp) && ('9' >= *pTmp))
           || ('*' == *pTmp) || ('#' == *pTmp) || ('?' == *pTmp)
           || (',' == *pTmp) || ('P' == *pTmp) || ('p' == *pTmp)))
        {
            return AT_FAILURE;
        }
        pTmp++;
    }
    return AT_SUCCESS;
}

/*****************************************************************************
 Prototype      : At_CheckNumLen
 Description    :
 Input          :
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32  At_CheckNumLen(TAF_UINT16 usMax,TAF_UINT16 usLen)
{
    /* 如果号码过长，直接返回错误 */
    if(AT_CSCS_UCS2_CODE == gucAtCscsType)
    {
        if((usMax * 4) < usLen)
        {
            return AT_FAILURE;
        }
    }
    else
    {
        if(usMax < usLen)
        {
            return AT_FAILURE;
        }
    }
    return AT_SUCCESS;
}
TAF_UINT32  At_CheckUssdNumLen(
    VOS_UINT8                           ucIndex,
    TAF_SS_DATA_CODING_SCHEME           dcs,
    TAF_UINT16                          usLen
)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);




    if(AT_USSD_NON_TRAN_MODE == pstSsCtx->usUssdTransMode)
    {
        if(TAF_SS_MAX_USSDSTRING_LEN < usLen)
        {
            return AT_FAILURE;
        }
    }
    else
    {
        if((TAF_SS_MAX_USS_CHAR * 2) < usLen)
        {
            return AT_FAILURE;
        }

    }

    return AT_SUCCESS;
}
TAF_UINT32 At_UnicodeTransferTo81CalcLength(const TAF_UINT8 *pucData,
                                            TAF_UINT16      usLen,
                                            TAF_UINT16      *pusBaseCode,
                                            TAF_UINT16      *pusDstLen)
{
    TAF_UINT16      usUnicodeChar;
    TAF_UINT16      usBaseCode = 0;
    TAF_UINT16      usIndex;
    TAF_BOOL        bIsBaseSet = TAF_FALSE;
    TAF_UINT16      usDstLen = 0;
    TAF_UINT8       aucGsmData[2];
    TAF_UINT16      usGsmDataLen = 0;

    /* 输入参数赋初值 */
    *pusDstLen = 0;
    *pusBaseCode = 0;

    for (usIndex = 0; usIndex < (usLen >> 1); usIndex++)
    {
        usUnicodeChar = (*pucData << 8) | (*(pucData + 1));

        pucData += 2;

        /* 反向查找GSM到UCS的基本表和扩展表 */
        if (AT_SUCCESS == At_OneUnicodeToGsm(usUnicodeChar, aucGsmData, &usGsmDataLen))
        {
            usDstLen += usGsmDataLen;

            continue;
        }

        /* 81编码用第8到第15bit进行编码，即0hhh hhhh hXXX XXXX，因此第16bit为1一定不能进行81编码 */
        if (0 != (usUnicodeChar & 0x8000))
        {
            AT_ERR_LOG("At_UnicodeTransferTo81CalcLength error: no16 bit is 1");

            return AT_FAILURE;
        }

        if (TAF_FALSE == bIsBaseSet)
        {
            /* 取第一个UCS2的第8到第15bit作为BaseCode，BaseCode右移7位即为基址针 */
            bIsBaseSet = TAF_TRUE;
            usBaseCode = usUnicodeChar & AT_PB_81_CODE_BASE_POINTER;
        }
        else
        {
            if (usBaseCode != (usUnicodeChar & AT_PB_81_CODE_BASE_POINTER))
            {
                AT_ERR_LOG("At_UnicodeTransferTo81CalcLength error: code base error");

               return AT_FAILURE;
            }
        }

        usDstLen++;
    }

    /* 能进行81编码，返回基本码和81编码总长度 */
    *pusBaseCode = usBaseCode;
    *pusDstLen   = usDstLen + AT_PB_81CODE_HEADER_LEN;

    return AT_SUCCESS;
}
TAF_UINT32 At_UnicodeTransferTo82CalcLength(const TAF_UINT8 *pucData,
                                            TAF_UINT16      usLen,
                                            TAF_UINT16      *pusBaseCode,
                                            TAF_UINT16      *pusDstLen)
{
    TAF_UINT16      usUnicodeChar;
    TAF_UINT16      usIndex;
    TAF_BOOL        bIsBaseSet = TAF_FALSE;
    TAF_UINT16      us82BaseHigh = 0;
    TAF_UINT16      us82BaseLow = 0;
    TAF_UINT16      usDstLen = 0;
    TAF_UINT8       aucGsmData[2];
    TAF_UINT16      usGsmDataLen = 0;

    /* 输入参数赋初值 */
    *pusDstLen = 0;
    *pusBaseCode = 0;

    for (usIndex = 0; usIndex < (usLen >> 1); usIndex++)
    {
        usUnicodeChar = (*pucData << 8) | (*(pucData + 1));

        pucData += 2;

        /* 反向查找GSM到UCS的基本表和扩展表 */
        if (AT_SUCCESS == At_OneUnicodeToGsm(usUnicodeChar, aucGsmData, &usGsmDataLen))
        {
            usDstLen += usGsmDataLen;

            continue;
        }


        if (TAF_FALSE == bIsBaseSet)
        {
            bIsBaseSet = TAF_TRUE;
            us82BaseHigh = usUnicodeChar;
            us82BaseLow  = usUnicodeChar;
        }
        else
        {
            if (usUnicodeChar < us82BaseLow)
            {
                us82BaseLow = usUnicodeChar;
            }

            if (usUnicodeChar > us82BaseHigh)
            {
                us82BaseHigh = usUnicodeChar;
            }

            /* UCS2码流中最大编码和最小编码差值超过127就不能进行82编码 */
            if (AT_PB_GSM7_CODE_MAX_VALUE < (us82BaseHigh - us82BaseLow))
            {
                 AT_ERR_LOG("At_UnicodeTransferTo82CalcLength error: code base error");

                return AT_FAILURE;
            }
        }

        usDstLen++;
    }

    /* 能进行82编码，返回基本码和82编码总长度 */
    *pusBaseCode = us82BaseLow;
    *pusDstLen   = usDstLen + AT_PB_82CODE_HEADER_LEN;

    return AT_SUCCESS;
}


TAF_UINT32 At_UnicodeTransferTo81(const TAF_UINT8 *pucData,
                                  TAF_UINT16      usLen,
                                  TAF_UINT16      usCodeBase,
                                  TAF_UINT8       *pucDest)
{
    TAF_UINT16      usUnicodeChar;
    TAF_UINT16      i;
    TAF_UINT8       aucGsmData[2];
    TAF_UINT16      usGsmDataLen = 0;
    TAF_UINT16      usDstLen     = AT_PB_81CODE_HEADER_LEN;

    for (i = 0; i < (usLen >> 1); i++)
    {
        usUnicodeChar = (*pucData << 8) | (*(pucData + 1));

        pucData += 2;

        /* 反向查找GSM到UCS的基本表和扩展表 */
        if (AT_SUCCESS == At_OneUnicodeToGsm(usUnicodeChar, aucGsmData, &usGsmDataLen))
        {
            VOS_MemCpy((pucDest + usDstLen), aucGsmData, usGsmDataLen);
            usDstLen += usGsmDataLen;

            continue;
        }

        /* 容错处理 */
        if ((usCodeBase | (usUnicodeChar & AT_PB_GSM7_CODE_MAX_VALUE)) == usUnicodeChar)
        {
            /* 转为81的编码方式 */
            pucDest[usDstLen++] =(TAF_UINT8)( AT_PB_CODE_NO8_BIT | (usUnicodeChar & AT_PB_GSM7_CODE_MAX_VALUE));
        }
        else
        {
            AT_ERR_LOG("At_UnicodeTransferTo81 error");

            return AT_FAILURE;
        }
    }

    /* 设置81编码header信息 */
    pucDest[0] = SI_PB_ALPHATAG_TYPE_UCS2_81;
    pucDest[1] = (TAF_UINT8)(usDstLen - AT_PB_81CODE_HEADER_LEN);
    pucDest[2] = (TAF_UINT8)((usCodeBase & AT_PB_81_CODE_BASE_POINTER)>>7);

    return AT_SUCCESS;
}


TAF_UINT32 At_UnicodeTransferTo82(const TAF_UINT8 *pucData,
                                  TAF_UINT16      usLen,
                                  TAF_UINT16      usCodeBase,
                                  TAF_UINT8       *pucDest)
{
    TAF_UINT16      usUnicodeChar;
    TAF_UINT16      i;
    TAF_UINT8       aucGsmData[2];
    TAF_UINT16      usGsmDataLen = 0;
    TAF_UINT16      usDstLen     = AT_PB_82CODE_HEADER_LEN;

    for (i = 0; i < (usLen >> 1); i++)
    {
        usUnicodeChar = (*pucData << 8) | (*(pucData + 1));

        pucData += 2;

        /* 反向查找GSM到UCS的基本表和扩展表 */
        if (AT_SUCCESS == At_OneUnicodeToGsm(usUnicodeChar, aucGsmData, &usGsmDataLen))
        {
            VOS_MemCpy(pucDest + usDstLen, aucGsmData, usGsmDataLen);
            usDstLen += usGsmDataLen;

            continue;
        }

        /* 容错处理 */
        if ((usUnicodeChar - usCodeBase) <= AT_PB_GSM7_CODE_MAX_VALUE)
        {
            /* 转为82的编码方式 */
            pucDest[usDstLen++] = ((TAF_UINT8)(usUnicodeChar - usCodeBase)) |  AT_PB_CODE_NO8_BIT;
        }
        else
        {
            AT_ERR_LOG("At_UnicodeTransferTo81 error");

            return AT_FAILURE;
        }
    }

    /* 设置82编码header信息 */
    pucDest[0] = SI_PB_ALPHATAG_TYPE_UCS2_82;
    pucDest[1] = (TAF_UINT8)(usDstLen - AT_PB_82CODE_HEADER_LEN);
    pucDest[2] = (TAF_UINT8)((usCodeBase & 0xff00)>> 8);
    pucDest[3] = (TAF_UINT8)(usCodeBase & 0x00ff);

    return AT_SUCCESS;
}


TAF_UINT32 At_Gsm7BitFormat(TAF_UINT8   *pucSrc,
                            TAF_UINT16  usSrcLen,
                            TAF_UINT8   *pucDst,
                            TAF_UINT16  *pusDstLen)
{
   TAF_UINT16       usIndex;

   for (usIndex = 0; usIndex < usSrcLen; usIndex++)
   {
        /* GSM模式下输入码流大于0x7f时提示用户输入中含有非法字符 */
        if (AT_PB_GSM7_CODE_MAX_VALUE < pucSrc[usIndex])
        {
            return AT_FAILURE;
        }

        pucDst[usIndex] = pucSrc[usIndex];
   }

   *pusDstLen = usSrcLen;     /* [false alarm]: 不会越界 */

   return AT_SUCCESS;
}


TAF_UINT32 At_UnicodePrint2Unicode(TAF_UINT8 *pData,TAF_UINT16 *pLen)
{
    TAF_UINT8 *pCheck   = pData;
    TAF_UINT8 *pWrite   = pData;
    TAF_UINT8 *pRead    = pData;
    TAF_UINT16 usLen    = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucHigh   = 0;
    TAF_UINT8  ucLow    = 0;

    /* 字符均为'0'-'9','a'-'f','A'-'F' */
    while( usChkLen++ < *pLen )
    {
        if( (*pCheck >= '0') && (*pCheck <= '9') )
        {
            *pCheck = *pCheck - '0';
        }
        else if( (*pCheck >= 'a') && (*pCheck <= 'f') )
        {
            *pCheck = (*pCheck - 'a') + 10;
        }
        else if( (*pCheck >= 'A') && (*pCheck <= 'F') )
        {
            *pCheck = (*pCheck - 'A') + 10;
        }
        else
        {
            return AT_FAILURE;
        }
        pCheck++;
    }

    while(usLen < *pLen)                    /* 判断结尾 */
    {
        ucHigh = *pRead++;                  /* 高位 */
        usLen++;
        ucLow  = *pRead++;                  /* 低位 */
        usLen++;

        *pWrite++ = (TAF_UINT8)(ucHigh * 16) + ucLow;    /* 写入UNICODE的一个字节 */
    }

    *pLen = usLen >> 1;
    return AT_SUCCESS;
}
TAF_UINT32 At_UnicodePrintToAscii(TAF_UINT8 *pData,TAF_UINT16 *pLen)
{
    TAF_UINT8 *pCheck        = pData;
    TAF_UINT8 *pWrite        = pData;
    TAF_UINT8 *pRead         = pData;
    TAF_UINT16 usLen         = 0;
    TAF_UINT16 usChkLen      = 0;
    TAF_UINT8  ucFirstByte   = 0;
    TAF_UINT8  ucSecondByte  = 0;
    TAF_UINT8  ucHigh        = 0;
    TAF_UINT8  ucLow         = 0;

    /* 字符均为'0'-'9','a'-'f','A'-'F' */
    while (usChkLen++ < *pLen)
    {
        if ((*pCheck >= '0') && (*pCheck <= '9'))
        {
            *pCheck = *pCheck - '0';
        }
        else if ((*pCheck >= 'a') && (*pCheck <= 'f'))
        {
            *pCheck = (*pCheck - 'a') + 10;
        }
        else if ((*pCheck >= 'A') && (*pCheck <= 'F'))
        {
            *pCheck = (*pCheck - 'A') + 10;
        }
        else
        {
            return AT_FAILURE;
        }
        pCheck++;
    }

    while(usLen < *pLen)                                                        /* 判断结尾 */
    {
        ucFirstByte  = *pRead++;                                                /* 前两位必须为0 */
        usLen++;
        ucSecondByte = *pRead++;
        usLen++;

        if ((0 != ucFirstByte) || (0 != ucSecondByte))
        {
            return AT_FAILURE;
        }

        ucHigh       = *pRead++;                                                /* 高位 */
        usLen++;
        ucLow        = *pRead++;                                                /* 低位 */
        usLen++;

        *pWrite++ = (TAF_UINT8)(ucHigh * 16) + ucLow;                          /* 写入Unicode的一个字节 */
    }

    *pLen = usLen >> 2;
    return AT_SUCCESS;
}
TAF_UINT32 At_UnicodePrint2Ascii(TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT8 *pWrite   = pucDst;
    TAF_UINT8 *pRead    = pucSrc;
    TAF_UINT8  aucTmp[4];
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucIndex = 0;

    /* 每4个字符可以转成一个ASCII码 */
    if(0 != (usSrcLen % 4))
    {
        return AT_FAILURE;
    }

    /* 清空临时缓存 */
    PS_MEM_SET(aucTmp,0x00,4);

    /* 字符均为'0'-'9','a'-'f','A'-'F' */
    while( usChkLen++ < usSrcLen )
    {
        if( (*pRead >= '0') && (*pRead <= '9') )
        {
            aucTmp[ucIndex++] = *pRead - '0';
        }
        else if( (*pRead >= 'a') && (*pRead <= 'f') )
        {
            aucTmp[ucIndex++] = (*pRead - 'a') + 10;
        }
        else if( (*pRead >= 'A') && (*pRead <= 'F') )
        {
            aucTmp[ucIndex++] = (*pRead - 'A') + 10;
        }
        else
        {
            return AT_FAILURE;
        }

        /* 每四个字符得出一个字符 */
        if(4 == ucIndex)
        {
            /* 必须是数字类型 */
            if((0 != aucTmp[0]) || (0 != aucTmp[1]))
            {
                return AT_FAILURE;
            }

            *pWrite++ = (TAF_UINT8)(aucTmp[2] * 16) + aucTmp[3];    /* 写入一个字节 */
            if(AT_FAILURE == At_CheckDialNum(*(pWrite-1)))
            {
                return AT_FAILURE;
            }
            /* 重新开始 */
            ucIndex = 0;
        }

        /* 下一个字符 */
        pRead++;
    }

    return AT_SUCCESS;
}


TAF_UINT32 At_CheckNameUCS2Code(TAF_UINT8 *pucData, TAF_UINT16 *pusLen)
{
    TAF_UINT8 ucAlphType = *pucData;
    TAF_UINT16 usAlphLen = *(pucData+1);

    if ((SI_PB_ALPHATAG_TYPE_UCS2_80 == ucAlphType) && (((*pusLen-1)%2) == 0))
    {
        /* 80编码，除了标记字节其余长度必须为偶数字节 */
        return AT_SUCCESS;
    }

    if ((SI_PB_ALPHATAG_TYPE_UCS2_81 == ucAlphType) && ((*pusLen-3) >= usAlphLen))
    {
        /* 81编码，实际字符长度必须大于码流中长度字节的值 */
        *pusLen = usAlphLen + 3;
        return AT_SUCCESS;
    }

    if ((SI_PB_ALPHATAG_TYPE_UCS2_82 == ucAlphType) && ((*pusLen-4) >= usAlphLen))
    {
        /* 82编码，同上，区别是基址是2个字节 */
        *pusLen = usAlphLen + 4;
        return AT_SUCCESS;
    }

    return AT_FAILURE;

}

/*****************************************************************************
 函 数 名  : AT_PhSendRestoreFactParm
 功能描述  : E5形态时，恢复用户参数配置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年11月23日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_PhSendRestoreFactParm( VOS_VOID )
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;

    for(i = 0; i < AT_MAX_CLIENT_NUM; i++)
    {
        if (AT_APP_USER == gastAtClientTab[i].UserType)
        {
            break;
        }
    }

    /* 未找到E5 User,则不用上报 */
    if ( i >= AT_MAX_CLIENT_NUM )
    {
        return ;
    }

    usLength = 0;
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "^FACTORY");
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)i,pgucAtSndCodeAddr,usLength);
}
VOS_VOID AT_PhSendRestoreFactParmNoReset( VOS_VOID )
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;

    for(i = 0; i < AT_MAX_CLIENT_NUM; i++)
    {
        if (AT_APP_USER == gastAtClientTab[i].UserType)
        {
            break;
        }
    }

    /* 未找到E5 User,则不用上报 */
    if ( i >= AT_MAX_CLIENT_NUM )
    {
        return ;
    }

    usLength = 0;
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "^NORSTFACT");
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)i,pgucAtSndCodeAddr,usLength);
}


/*****************************************************************************
 Prototype      : At_SetNumTypePara
 Description    : 设置UNICODE或者ASCII类型的号码
 Input          : pucDst   --- 目的字串
                  pucSrc   --- 源字串
                  usSrcLen --- 源字串长度
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetNumTypePara(TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT32                          ulRet;
    TAF_UINT16                          usAsciiStrLen;

    if(AT_CSCS_UCS2_CODE == gucAtCscsType)
    {
        ulRet = At_UnicodePrint2Ascii(pucDst,pucSrc,usSrcLen);
        if (AT_SUCCESS != ulRet)
        {
            return ulRet;
        }
    }
    else
    {
        PS_MEM_CPY(pucDst,pucSrc,usSrcLen);
    }

    usAsciiStrLen = (TAF_UINT16)VOS_StrLen((TAF_CHAR *)pucDst);
    if(AT_SUCCESS == At_CheckDialString(pucDst, usAsciiStrLen))
    {
        return AT_SUCCESS;
    }
    else
    {
        return AT_FAILURE;
    }
}


TAF_UINT32 At_UpdateMsgRcvAct(
    TAF_UINT8                           ucIndex,
    AT_CSMS_MSG_VERSION_ENUM_U8         ucCsmsVersion,
    AT_CNMI_MT_TYPE                     CnmiMtType,
    AT_CNMI_DS_TYPE                     CnmiDsType
)
{
    MN_MSG_SET_RCVMSG_PATH_PARM_STRU    stRcvPath;
    MN_MSG_RCVMSG_ACT_ENUM_U8           enRcvSmAct[AT_CNMI_MT_TYPE_MAX] = {MN_MSG_RCVMSG_ACT_STORE,
                                                                           MN_MSG_RCVMSG_ACT_STORE,
                                                                           MN_MSG_RCVMSG_ACT_TRANSFER_AND_ACK,
                                                                           MN_MSG_RCVMSG_ACT_STORE};
    MN_MSG_RCVMSG_ACT_ENUM_U8           enRcvStaRptAct[AT_CNMI_DS_TYPE_MAX] = {MN_MSG_RCVMSG_ACT_STORE,
                                                                               MN_MSG_RCVMSG_ACT_TRANSFER_AND_ACK,
                                                                               MN_MSG_RCVMSG_ACT_STORE};

    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if ((CnmiMtType >= AT_CNMI_MT_TYPE_MAX)
     || (CnmiDsType >= AT_CNMI_DS_TYPE_MAX))
    {
        return AT_ERROR;
    }

    PS_MEM_CPY(&stRcvPath, &(pstSmsCtx->stCpmsInfo.stRcvPath), sizeof(stRcvPath));
    if (AT_CSMS_MSG_VERSION_PHASE2_PLUS == ucCsmsVersion)
    {
        enRcvSmAct[2] = MN_MSG_RCVMSG_ACT_TRANSFER_ONLY;
        enRcvStaRptAct[1] = MN_MSG_RCVMSG_ACT_TRANSFER_ONLY;
    }

    stRcvPath.enRcvSmAct        = enRcvSmAct[CnmiMtType];
    stRcvPath.enRcvStaRptAct    = enRcvStaRptAct[CnmiDsType];
    stRcvPath.enSmsServVersion  = ucCsmsVersion;

    PS_MEM_SET(stRcvPath.aucReserve1, 0, sizeof(stRcvPath.aucReserve1));


    /* 将CNMI设置的MT type类型传输给TAF,在<MT>=3，收到CLASS3短信时候，按照CMT方式上报 */
    stRcvPath.enCnmiMtType      = CnmiMtType;

    if (MN_ERR_NO_ERROR != MN_MSG_SetRcvMsgPath(gastAtClientTab[ucIndex].usClientId,
                                                gastAtClientTab[ucIndex].opId,
                                                &stRcvPath))
    {
        return AT_ERROR;
    }

    return AT_OK;
}
VOS_UINT32 AT_ReadActiveMessage(
    MODEM_ID_ENUM_UINT16                enModemId,
    MN_MSG_ACTIVE_MESSAGE_STRU         *pstActiveMessage
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                          *pucActiveMessageInfo;
    VOS_UINT8                          *pucEvaluate;

    pucActiveMessageInfo = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT,
                                                     MN_MSG_ACTIVE_MESSAGE_PARA_LEN);
    if (VOS_NULL_PTR == pucActiveMessageInfo)
    {
        AT_WARN_LOG("AT_ReadActiveMessage : fail to alloc memory . ");
        return MN_ERR_NOMEM;
    }

    ulRet = NV_ReadEx(enModemId,
                      en_NV_Item_SMS_ActiveMessage_Para,
                      pucActiveMessageInfo,
                      MN_MSG_ACTIVE_MESSAGE_PARA_LEN);
    if (NV_OK != ulRet)
    {
        AT_WARN_LOG("AT_ReadActiveMessage : fail to read NVIM . ");
        PS_MEM_FREE(WUEPS_PID_AT, pucActiveMessageInfo);
        return MN_ERR_CLASS_SMS_NVIM;
    }

    pucEvaluate                         = pucActiveMessageInfo;

    pstActiveMessage->enActiveStatus    = *pucEvaluate;
    pucEvaluate++;

    pstActiveMessage->enMsgCoding       = *pucEvaluate;
    pucEvaluate++;

    pstActiveMessage->stUrl.ulLen       = *pucEvaluate;
    pucEvaluate++;
    pstActiveMessage->stUrl.ulLen      |= *pucEvaluate << 8;
    pucEvaluate++;
    pstActiveMessage->stUrl.ulLen      |= *pucEvaluate << 16;
    pucEvaluate++;
    pstActiveMessage->stUrl.ulLen      |= *pucEvaluate << 24;
    pucEvaluate++;

    if (pstActiveMessage->stUrl.ulLen > MN_MSG_ACTIVE_MESSAGE_MAX_URL_LEN)
    {
        AT_WARN_LOG("AT_ReadActiveMessage : fail to read NVIM . ");
        pstActiveMessage->stUrl.ulLen = MN_MSG_ACTIVE_MESSAGE_MAX_URL_LEN;
    }

    PS_MEM_CPY(pstActiveMessage->stUrl.aucUrl,
               pucEvaluate,
               (VOS_UINT16)pstActiveMessage->stUrl.ulLen);

    PS_MEM_FREE(WUEPS_PID_AT, pucActiveMessageInfo);
    return MN_ERR_NO_ERROR;
}


TAF_UINT32 AT_SetRstriggerPara(
    TAF_UINT8                           ucIndex
)
{
    MN_MSG_ACTIVE_MESSAGE_STRU          stActiveMessage;
    MN_MSG_ACTIVE_MESSAGE_STRU          stOrgActiveMessage;
    TAF_UINT32                          ulRet;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    /* 参数检查 ：
       个数为0返回AT_CME_INCORRECT_PARAMETERS
       个数为2返回AT_TOO_MANY_PARA
    */
    if (gucAtParaIndex > 2)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_SetRstriggerPara: Get modem id fail.");
        return AT_ERROR;
    }

    ulRet = AT_ReadActiveMessage(enModemId, &stOrgActiveMessage);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        AT_LOG1("AT_SetRstriggerPara: fail to get active message information , cause is %d",
                ulRet);
        return AT_ERROR;
    }

    /*URL:用户没有设置URL,则保持NVIM中的URL不变;否则，用户设置的URL覆盖NVIM的数据*/
    if (2 == gucAtParaIndex)
    {
        if (gastAtParaList[1].usParaLen > MN_MSG_ACTIVE_MESSAGE_MAX_URL_LEN)
        {
            stActiveMessage.stUrl.ulLen = MN_MSG_ACTIVE_MESSAGE_MAX_URL_LEN;
        }
        else
        {
            stActiveMessage.stUrl.ulLen = gastAtParaList[1].usParaLen;
        }
        PS_MEM_CPY(stActiveMessage.stUrl.aucUrl,
                   gastAtParaList[1].aucPara,
                   (VOS_UINT16)stActiveMessage.stUrl.ulLen);

        stActiveMessage.enMsgCoding    = MN_MSG_MSG_CODING_8_BIT;
    }
    else
    {
        PS_MEM_CPY(&stActiveMessage.stUrl, &stOrgActiveMessage.stUrl, sizeof(stActiveMessage.stUrl));
        stActiveMessage.enMsgCoding    = stOrgActiveMessage.enMsgCoding;
    }

    /*ACTIVE STATUS:*/
    stActiveMessage.enActiveStatus = (MN_MSG_ACTIVE_MESSAGE_STATUS_ENUM_UINT8)gastAtParaList[0].ulParaValue;

    /*调用AT_WriteActiveMessage写激活短信的激活状态和URL信息到NVIM；*/
    ulRet = AT_WriteActiveMessage(enModemId, &stOrgActiveMessage, &stActiveMessage);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return AT_ERROR;
    }

    return AT_OK;

}
TAF_UINT32 At_SetCsmsPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查: 此处相对于旧代码有修改，待确认 */
    if ((gucAtParaIndex != 1)
     || (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    gastAtClientTab[ucIndex].opId = At_GetOpId();
    ulRet = At_UpdateMsgRcvAct(ucIndex,
                                (AT_CSMS_MSG_VERSION_ENUM_U8)gastAtParaList[0].ulParaValue,
                                pstSmsCtx->stCnmiType.CnmiMtType,
                                pstSmsCtx->stCnmiType.CnmiDsType);

    if (AT_OK != ulRet)
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSMS_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */

}
TAF_UINT32 At_SetCgsmsPara(TAF_UINT8 ucIndex)
{
    AT_NVIM_SEND_DOMAIN_STRU            stSendDomain;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    enModemId = MODEM_ID_0;
    ulLength  = 0;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        gastAtParaList[0].ulParaValue = MN_MSG_SEND_DOMAIN_CS;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_SetCgsmsPara:Get modem id fail");
        return AT_ERROR;
    }

    pstSmsCtx = AT_GetModemSmsCtxAddrFromModemId(enModemId);

    pstSmsCtx->stCgsmsSendDomain.enSendDomain = (AT_CGSMS_SEND_DOMAIN_ENUM_U8)gastAtParaList[0].ulParaValue;

    /* 设置CGSMS时,如果NVIM中已经激活,将协议中的短信默认值修改为NV对应的值 */
    if (VOS_TRUE  == pstSmsCtx->stCgsmsSendDomain.ucActFlg)
    {
        PS_MEM_SET(&stSendDomain,0,sizeof(stSendDomain));
        NV_GetLength(en_NV_Item_SMS_SEND_DOMAIN, &ulLength);
        if (NV_OK == NV_ReadEx(enModemId, en_NV_Item_SMS_SEND_DOMAIN, &stSendDomain, ulLength))
        {
            if ( (stSendDomain.ucSendDomain == At_SendDomainProtoToNvim(pstSmsCtx->stCgsmsSendDomain.enSendDomain))
                && (stSendDomain.ucActFlg == pstSmsCtx->stCgsmsSendDomain.ucActFlg) )
            {
                AT_INFO_LOG("At_SetCgsmsPara():The content to write is same as NV's");
                return AT_OK;
            }
        }
        stSendDomain.ucSendDomain = (VOS_UINT8)At_SendDomainProtoToNvim(pstSmsCtx->stCgsmsSendDomain.enSendDomain);
        stSendDomain.ucActFlg = pstSmsCtx->stCgsmsSendDomain.ucActFlg;

        NV_GetLength(en_NV_Item_SMS_SEND_DOMAIN, &ulLength);
        if (NV_OK != NV_WriteEx(enModemId, en_NV_Item_SMS_SEND_DOMAIN, &stSendDomain, ulLength))
        {
            AT_INFO_LOG("At_SetCgsmsPara():ERROR: en_NV_Item_SMS_SEND_DOMAIN wirte failed");
            return AT_ERROR;
        }
    }
    return AT_OK;
}
VOS_UINT32 At_GetSmsStorage(
    VOS_UINT8                           ucIndex,
    MN_MSG_MEM_STORE_ENUM_U8            enMemReadorDelete,
    MN_MSG_MEM_STORE_ENUM_U8            enMemSendorWrite,
    MN_MSG_MEM_STORE_ENUM_U8            enMemRcv
)
{
    MN_MSG_GET_STORAGE_STATUS_PARM_STRU stMemParm;

    if ((MN_MSG_MEM_STORE_SIM == enMemReadorDelete)
     || (MN_MSG_MEM_STORE_SIM == enMemSendorWrite)
     || (MN_MSG_MEM_STORE_SIM == enMemRcv))
    {
        stMemParm.enMem1Store = MN_MSG_MEM_STORE_SIM;
        gastAtClientTab[ucIndex].AtSmsData.bWaitForUsimStorageStatus = VOS_TRUE;
    }
    else
    {
        stMemParm.enMem1Store = MN_MSG_MEM_STORE_NONE;
        gastAtClientTab[ucIndex].AtSmsData.bWaitForUsimStorageStatus = VOS_FALSE;
    }

#if (NAS_FEATURE_SMS_NVIM_SMSEXIST == FEATURE_ON)
    if ((MN_MSG_MEM_STORE_NV == enMemReadorDelete)
     || (MN_MSG_MEM_STORE_NV == enMemSendorWrite)
     || (MN_MSG_MEM_STORE_NV == enMemRcv))
    {
        stMemParm.enMem2Store = MN_MSG_MEM_STORE_NV;
        gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus = VOS_TRUE;
    }
    else
    {
        stMemParm.enMem2Store = MN_MSG_MEM_STORE_NONE;
        gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus = VOS_FALSE;
    }
#else
    stMemParm.enMem2Store = MN_MSG_MEM_STORE_NONE;
    gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus = VOS_FALSE;
#endif

    if (MN_ERR_NO_ERROR != MN_MSG_GetStorageStatus(gastAtClientTab[ucIndex].usClientId,
                                                   gastAtClientTab[ucIndex].opId,
                                                   &stMemParm))
    {
        return AT_ERROR;
    }

    return AT_OK;
}


TAF_UINT32 At_SetCpmsPara(TAF_UINT8 ucIndex)
{
    MN_MSG_SET_RCVMSG_PATH_PARM_STRU    stRcvPath;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    PS_MEM_SET(&stRcvPath, 0, sizeof(MN_MSG_SET_RCVMSG_PATH_PARM_STRU));
    /* 参数检查 */
    if ((gucAtParaIndex > 3)
     || (0 == gucAtParaIndex)
     || (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    pstSmsCtx->stCpmsInfo.enTmpMemReadorDelete = (MN_MSG_MEM_STORE_ENUM_U8)(gastAtParaList[0].ulParaValue + 1);

    if (0 != gastAtParaList[1].usParaLen)
    {
        pstSmsCtx->stCpmsInfo.enTmpMemSendorWrite = (MN_MSG_MEM_STORE_ENUM_U8)(gastAtParaList[1].ulParaValue + 1);
    }
    else
    {
        pstSmsCtx->stCpmsInfo.enTmpMemSendorWrite = pstSmsCtx->stCpmsInfo.enMemSendorWrite;
    }

    /* 执行命令操作 */
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    PS_MEM_CPY(&stRcvPath, &(pstSmsCtx->stCpmsInfo.stRcvPath), sizeof(stRcvPath));


    /* 将CNMI设置的MT type类型传输给TAF,在<MT>=3，收到CLASS3短信时候，按照CMT方式上报 */
    stRcvPath.enCnmiMtType = pstSmsCtx->stCnmiType.CnmiMtType;

    if (0 != gastAtParaList[2].usParaLen)
    {
        stRcvPath.enStaRptMemStore = (MN_MSG_MEM_STORE_ENUM_U8)(gastAtParaList[2].ulParaValue + 1);
        stRcvPath.enSmMemStore = (MN_MSG_MEM_STORE_ENUM_U8)(gastAtParaList[2].ulParaValue + 1);
    }

    if ((MN_MSG_MEM_STORE_NV == pstSmsCtx->stCpmsInfo.enTmpMemReadorDelete)
     || (MN_MSG_MEM_STORE_NV == pstSmsCtx->stCpmsInfo.enTmpMemSendorWrite)
     || (MN_MSG_MEM_STORE_NV == stRcvPath.enSmMemStore))
    {
#if (NAS_FEATURE_SMS_NVIM_SMSEXIST != FEATURE_ON)
        return AT_ERROR;
#else
        if (MN_MSG_ME_STORAGE_ENABLE != pstSmsCtx->enMsgMeStorageStatus)
        {
            return AT_ERROR;
        }
#endif
    }

    if (MN_ERR_NO_ERROR != MN_MSG_SetRcvMsgPath(gastAtClientTab[ucIndex].usClientId,
                                                gastAtClientTab[ucIndex].opId,
                                                &stRcvPath))
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].AtSmsData.bWaitForCpmsSetRsp = TAF_TRUE;

    if (AT_OK != At_GetSmsStorage(ucIndex,
                                  pstSmsCtx->stCpmsInfo.enTmpMemReadorDelete,
                                  pstSmsCtx->stCpmsInfo.enTmpMemSendorWrite,
                                  stRcvPath.enSmMemStore))
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPMS_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */

}


TAF_UINT32 At_SetCmgfPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if ((AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
     || (gucAtParaIndex > 1))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        gastAtParaList[0].ulParaValue = AT_CMGF_MSG_FORMAT_PDU;
    }

    pstSmsCtx->enCmgfMsgFormat = (AT_CMGF_MSG_FORMAT_ENUM_U8)gastAtParaList[0].ulParaValue;
    return AT_OK;
}
/*end add by zhoujun40661 2006-10-24 for CNMA*/

TAF_UINT32 At_SetCscaPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8                           aucAsciiNum[MN_MAX_ASCII_ADDRESS_NUM + 2];   /*array  of ASCII Num*/
    TAF_UINT8                           ucScaType;
    TAF_UINT32                          ulRet;
    MN_MSG_WRITE_SRV_PARAM_STRU         stServParm;
    TAF_UINT8                          *pucNum;                                /*指向实际号码（不包括+号）的指针*/
    MN_MSG_SRV_PARAM_STRU               stParmInUsim;
    MN_OPERATION_ID_T                   opId                = At_GetOpId();
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (gucAtParaIndex > 2))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 如果<sca>号码过长，直接返回错误 */
    if (AT_FAILURE == At_CheckNumLen((MN_MAX_ASCII_ADDRESS_NUM + 1), gastAtParaList[0].usParaLen))
    {
        return AT_ERROR;
    }

    /* 初始化 */
    PS_MEM_SET(aucAsciiNum, 0x00, sizeof(aucAsciiNum));
    /* 设置<sca> , 将UCS2码转换成ASCII码*/
    ulRet = At_SetNumTypePara(aucAsciiNum, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);
    if (AT_SUCCESS != ulRet)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    ucScaType = (TAF_UINT8)At_GetCodeType(aucAsciiNum[0]);
    if (AT_MSG_INTERNAL_ISDN_ADDR_TYPE == ucScaType)
    {
        pucNum = &aucAsciiNum[1];
    }
    else
    {
        pucNum = aucAsciiNum;
    }

    /* 设置<tosca> */
    if (0 != gastAtParaList[1].usParaLen)
    {
        if (AT_MSG_INTERNAL_ISDN_ADDR_TYPE == ucScaType)
        {
            if (ucScaType != ((TAF_UINT8)gastAtParaList[1].ulParaValue))
            {
                return AT_CMS_OPERATION_NOT_ALLOWED;
            }
        }
        else
        {
            ucScaType = (TAF_UINT8)gastAtParaList[1].ulParaValue;
        }
    }

    /* 执行命令操作 */
    PS_MEM_CPY(&stParmInUsim, &(pstSmsCtx->stCscaCsmpInfo.stParmInUsim), sizeof(stParmInUsim));

    stParmInUsim.ucParmInd &= ~MN_MSG_SRV_PARM_MASK_SC_ADDR;
    stParmInUsim.stScAddr.addrType = ucScaType;
    ulRet = AT_AsciiNumberToBcd((TAF_CHAR *)pucNum,
                                stParmInUsim.stScAddr.aucBcdNum,
                                &stParmInUsim.stScAddr.ucBcdLen);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return AT_ERROR;
    }
    stServParm.enMemStore = MN_MSG_MEM_STORE_SIM;
    stServParm.enWriteMode = MN_MSG_WRITE_MODE_REPLACE;
    stServParm.ulIndex = AT_CSCA_CSMP_STORAGE_INDEX;
    PS_MEM_CPY(&stServParm.stSrvParm, &stParmInUsim, sizeof(stServParm.stSrvParm));
    gastAtClientTab[ucIndex].opId = opId;
    if (MN_ERR_NO_ERROR == MN_MSG_WriteSrvParam(gastAtClientTab[ucIndex].usClientId, opId, &stServParm))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSCA_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetCsmpPara(TAF_UINT8 ucIndex)
{
    MN_MSG_WRITE_SRV_PARAM_STRU         stServParm;
    MN_MSG_SRV_PARAM_STRU               stParmInUsim;
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if ((AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
     || (gucAtParaIndex > 4))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 设置<fo> */
    if(0 != gastAtParaList[0].usParaLen)
    {
        ulRet = At_ParseCsmpFo(&(pstSmsCtx->stCscaCsmpInfo.ucTmpFo));
        if (AT_SUCCESS != ulRet)
        {
            return ulRet;
        }
    }
    else
    {
        pstSmsCtx->stCscaCsmpInfo.ucTmpFo = pstSmsCtx->stCscaCsmpInfo.ucFo;
    }

    PS_MEM_CPY(&stParmInUsim, &(pstSmsCtx->stCscaCsmpInfo.stParmInUsim), sizeof(stParmInUsim));

    /* 设置<vp> */
    ulRet =  At_ParseCsmpVp(ucIndex, &(pstSmsCtx->stCscaCsmpInfo.stTmpVp));
    if (AT_SUCCESS != ulRet)
    {
        return ulRet;
    }

    /* 设置<pid> */
    if(0 != gastAtParaList[2].usParaLen)
    {
        /* 注意: gastAtParaList[2].ulParaValue此时尚未未转换，检查其它命令的这种情况 */
        if(AT_FAILURE == At_Auc2ul(gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen,&gastAtParaList[2].ulParaValue))
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if(gastAtParaList[2].ulParaValue <= 255)
        {
            stParmInUsim.ucParmInd &= ~MN_MSG_SRV_PARM_MASK_PID;
            stParmInUsim.ucPid = (TAF_UINT8)gastAtParaList[2].ulParaValue;
        }
        else
        {
            AT_NORM_LOG("At_SetCsmpPara: unsupport VP type.");
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }

    /* 设置<dcs> */
    if(0 != gastAtParaList[3].usParaLen)
    {
        /* 注意: gastAtParaList[3].ulParaValue此时尚未未转换，检查其它命令的这种情况 */
        if(AT_FAILURE == At_Auc2ul(gastAtParaList[3].aucPara,gastAtParaList[3].usParaLen,&gastAtParaList[3].ulParaValue))
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if(gastAtParaList[3].ulParaValue <= 255)
        {
            stParmInUsim.ucParmInd &= ~MN_MSG_SRV_PARM_MASK_DCS;
            stParmInUsim.ucDcs = (TAF_UINT8)gastAtParaList[3].ulParaValue;
        }
        else
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }

    /* 执行命令操作 */
    stServParm.enMemStore = MN_MSG_MEM_STORE_SIM;
    stServParm.enWriteMode = MN_MSG_WRITE_MODE_REPLACE;
    stServParm.ulIndex = AT_CSCA_CSMP_STORAGE_INDEX;
    PS_MEM_CPY(&stServParm.stSrvParm, &stParmInUsim, sizeof(stServParm.stSrvParm));
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if (MN_ERR_NO_ERROR == MN_MSG_WriteSrvParam(gastAtClientTab[ucIndex].usClientId, gastAtClientTab[ucIndex].opId, &stServParm))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSMP_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetCsdhPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSmsCtx->ucCsdhType = (AT_CSDH_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSmsCtx->ucCsdhType = AT_CSDH_NOT_SHOW_TYPE;
    }
    return AT_OK;
}
TAF_UINT32 At_SetCnmiPara(TAF_UINT8 ucIndex)
{
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查和缺省参数配置 */
    /* 不带参数的设置命令，清空所有参数值为默认值 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        pstSmsCtx->stCnmiType.CnmiTmpModeType = AT_CNMI_MODE_BUFFER_TYPE;
        pstSmsCtx->stCnmiType.CnmiTmpMtType   = AT_CNMI_MT_NO_SEND_TYPE;
        pstSmsCtx->stCnmiType.CnmiTmpBmType   = AT_CNMI_BM_NO_SEND_TYPE;
        pstSmsCtx->stCnmiType.CnmiTmpDsType   = AT_CNMI_DS_NO_SEND_TYPE;
        pstSmsCtx->stCnmiType.CnmiTmpBfrType  = AT_CNMI_BFR_SEND_TYPE;
    }

    if (AT_CMD_OPT_SET_PARA_CMD == g_stATParseCmd.ucCmdOptType)
    {
        /* 带参数的设置命令，参数个数不能为0且不能超过最大参数个数 */
        if ((0 == gucAtParaIndex)
         || (gucAtParaIndex > 5))
        {
            return AT_CMS_OPERATION_NOT_SUPPORTED;
        }

        /* 带参数的设置命令，用户未配置的参数不做修改 */
        pstSmsCtx->stCnmiType.CnmiTmpModeType = pstSmsCtx->stCnmiType.CnmiModeType;
        pstSmsCtx->stCnmiType.CnmiTmpMtType   = pstSmsCtx->stCnmiType.CnmiMtType;
        pstSmsCtx->stCnmiType.CnmiTmpBmType   = pstSmsCtx->stCnmiType.CnmiBmType;
        pstSmsCtx->stCnmiType.CnmiTmpDsType   = pstSmsCtx->stCnmiType.CnmiDsType;
        pstSmsCtx->stCnmiType.CnmiTmpBfrType  = pstSmsCtx->stCnmiType.CnmiBfrType;

        if (0 != gastAtParaList[0].usParaLen)
        {
            pstSmsCtx->stCnmiType.CnmiTmpModeType = (AT_CNMI_MODE_TYPE)gastAtParaList[0].ulParaValue;
        }

        if (0 != gastAtParaList[1].usParaLen)
        {
            pstSmsCtx->stCnmiType.CnmiTmpMtType = (AT_CNMI_MT_TYPE)gastAtParaList[1].ulParaValue;
        }

        if (0 != gastAtParaList[2].usParaLen)
        {
            pstSmsCtx->stCnmiType.CnmiTmpBmType = (AT_CNMI_BM_TYPE)gastAtParaList[2].ulParaValue;
            if ( ( AT_CNMI_BM_CBM_TYPE != pstSmsCtx->stCnmiType.CnmiTmpBmType )
               && (AT_CNMI_BM_NO_SEND_TYPE != pstSmsCtx->stCnmiType.CnmiTmpBmType ))
            {
                return AT_CMS_OPERATION_NOT_SUPPORTED;
            }
        }

        if (0 != gastAtParaList[3].usParaLen)
        {
            pstSmsCtx->stCnmiType.CnmiTmpDsType = (AT_CNMI_DS_TYPE)gastAtParaList[3].ulParaValue;
        }

        if (0 != gastAtParaList[4].usParaLen)
        {
            pstSmsCtx->stCnmiType.CnmiTmpBfrType = (AT_CNMI_BFR_TYPE)gastAtParaList[4].ulParaValue;
        }

        if ((AT_CNMI_MODE_BUFFER_TYPE == pstSmsCtx->stCnmiType.CnmiTmpModeType)
         || (AT_CNMI_MODE_SEND_OR_BUFFER_TYPE == pstSmsCtx->stCnmiType.CnmiTmpModeType))
        {
            if ((AT_CNMI_MT_CMT_TYPE == pstSmsCtx->stCnmiType.CnmiTmpMtType)
             || (AT_CNMI_MT_CLASS3_TYPE == pstSmsCtx->stCnmiType.CnmiTmpMtType))
            {
                return AT_CMS_OPERATION_NOT_SUPPORTED;
            }

            if (AT_CNMI_DS_CDS_TYPE == pstSmsCtx->stCnmiType.CnmiTmpDsType)
            {
                return AT_CMS_OPERATION_NOT_SUPPORTED;
            }

        }
    }

    gastAtClientTab[ucIndex].opId = At_GetOpId();
    ulRet = At_UpdateMsgRcvAct(ucIndex,
                               pstSmsCtx->enCsmsMsgVersion,
                               pstSmsCtx->stCnmiType.CnmiTmpMtType,
                               pstSmsCtx->stCnmiType.CnmiTmpDsType);
    if (AT_OK != ulRet)
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CNMI_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
}

TAF_UINT32 At_SetCmglPara(TAF_UINT8 ucIndex)
{
    MN_MSG_LIST_PARM_STRU               stListParm;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if ((AT_CMD_OPT_SET_PARA_CMD == g_stATParseCmd.ucCmdOptType) && (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 初始化 */
    PS_MEM_SET(&stListParm,0x00,sizeof(stListParm));

    stListParm.ucIsFirstTimeReq = VOS_TRUE;

    /* 设置<stat> */
    if (0 != gastAtParaList[0].usParaLen)
    {
        stListParm.enStatus = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        stListParm.enStatus = (TAF_UINT8)MN_MSG_STATUS_MT_NOT_READ;
    }

    /* 设置读取存储器 */
    stListParm.enMemStore = pstSmsCtx->stCpmsInfo.enMemReadorDelete;

    if(0 == pstSmsCtx->ucParaCmsr)
    {
        stListParm.bChangeFlag = TAF_TRUE;
    }
    else
    {
        stListParm.bChangeFlag = TAF_FALSE;
        if(gucAtParaIndex > 1)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }

    /* 执行命令操作 */
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if (MN_ERR_NO_ERROR == MN_MSG_List(gastAtClientTab[ucIndex].usClientId, gastAtClientTab[ucIndex].opId,&stListParm))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGL_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetCmgrPara(TAF_UINT8 ucIndex)
{
    MN_MSG_READ_PARM_STRU               stReadParm;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 初始化 */
    PS_MEM_SET(&stReadParm,0x00,sizeof(stReadParm));

    /* 参数设置 */
    stReadParm.ulIndex = gastAtParaList[0].ulParaValue;

    /* 设置读取存储器 */
    stReadParm.enMemStore = pstSmsCtx->stCpmsInfo.enMemReadorDelete;

    if(0 == pstSmsCtx->ucParaCmsr)
    {
        stReadParm.bChangeFlag = TAF_TRUE;
    }
    else
    {
        stReadParm.bChangeFlag = TAF_FALSE;
        if(gucAtParaIndex > 1)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }

    /* 执行命令操作 */
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if(MN_ERR_NO_ERROR == MN_MSG_Read(gastAtClientTab[ucIndex].usClientId, gastAtClientTab[ucIndex].opId, &stReadParm))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGR_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
/*begin add by zhoujun40661 2006-10-10 for cnma*/

TAF_UINT32 At_SetCnmaPara(TAF_UINT8 ucIndex)
{
    TAF_UINT32                          ulRet;
    TAF_BOOL                            bRpAck              = TAF_TRUE;
    MN_MSG_SEND_ACK_PARM_STRU           *pstAckParm;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if (gucAtParaIndex > 2)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 初始化 */
    PS_MEM_SET(&gastAtClientTab[ucIndex].AtSmsData, 0x00, sizeof(gastAtClientTab[ucIndex].AtSmsData));
    if (AT_CMGF_MSG_FORMAT_TEXT == pstSmsCtx->enCmgfMsgFormat)    /* TEXT方式 */
    {
        if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }
    else    /* PDU方式 */
    {
        if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
        {
            if(0 == gastAtParaList[0].usParaLen)
            {
                return AT_CMS_OPERATION_NOT_ALLOWED;
            }
            else
            {
                /* 设置<n> */
                gastAtClientTab[ucIndex].AtSmsData.ucCnmaType = (TAF_UINT8)gastAtParaList[0].ulParaValue;
            }

            /*只有参数<n>,决定是否发送RP-ACK或RP-ERROR*/
            if ((0 != gastAtParaList[1].usParaLen)
             && (0 != gastAtParaList[1].ulParaValue))
            {
                if (0 == gastAtClientTab[ucIndex].AtSmsData.ucCnmaType)
                {
                    return AT_ERROR;
                }

                /* 设置<length> */
                if (gastAtParaList[1].ulParaValue > AT_MSG_MAX_TPDU_LEN)
                {
                    AT_NORM_LOG("At_SetCnmaPara: the length of PDU is not support.");
                    return AT_CMS_OPERATION_NOT_ALLOWED;    /* 输入字串太长 */
                }

                gastAtClientTab[ucIndex].AtSmsData.ucPduLen = (TAF_UINT8)gastAtParaList[1].ulParaValue;
                At_SetMode(ucIndex,AT_CMD_MODE,AT_SMS_MODE);         /* 切换为短信模式 */
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CNMA_PDU_SET;
                return AT_WAIT_SMS_INPUT;   /* 返回提示短信内容输入状态 */
            }
            else
            {
                if (2 == gastAtClientTab[ucIndex].AtSmsData.ucCnmaType)
                {
                    bRpAck = TAF_FALSE;
                }
            }
        }
    }

    /*CMGF设置了文本格式, 或CNMA无参数，或CNMA参数中<length>为0*/
    pstAckParm = At_GetAckMsgMem();
    PS_MEM_SET(pstAckParm, 0x00, sizeof(MN_MSG_SEND_ACK_PARM_STRU));

    if (TAF_TRUE == bRpAck)
    {
        pstAckParm->bRpAck = TAF_TRUE;
        pstAckParm->stTsRawData.enTpduType = MN_MSG_TPDU_DELIVER_RPT_ACK;
    }
    else
    {
        pstAckParm->bRpAck = TAF_FALSE;
        pstAckParm->enRpCause = MN_MSG_RP_CAUSE_PROTOCOL_ERR_UNSPECIFIED;
        pstAckParm->stTsRawData.enTpduType = MN_MSG_TPDU_DELIVER_RPT_ERR;
        pstAckParm->stTsRawData.ulLen = 2;
        pstAckParm->stTsRawData.aucData[0] = 0;
        pstAckParm->stTsRawData.aucData[1] = 0xff;
    }

    /* 执行命令操作 */
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if (MN_ERR_NO_ERROR == MN_MSG_SendAck(gastAtClientTab[ucIndex].usClientId, gastAtClientTab[ucIndex].opId, pstAckParm))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CNMA_TEXT_SET;
        ulRet = AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        ulRet = AT_ERROR;
    }

    return ulRet;
}

/*end add by zhoujun40661 2006-10-10 for cnma*/

TAF_UINT32 At_SetCmgsPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8                           ucSmSFormat;
    TAF_UINT32                          ulRet;

    ucSmSFormat = AT_GetModemSmsCtxAddrFromClientId(ucIndex)->enCmgfMsgFormat;

    /* 参数检查，如果<da>号码或者<length>不存在，直接返回错误 */
    if (0 == gastAtParaList[0].usParaLen )
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 初始化 */
    PS_MEM_SET(&gastAtClientTab[ucIndex].AtSmsData,0x00,sizeof(gastAtClientTab[ucIndex].AtSmsData));

    if (AT_CMGF_MSG_FORMAT_TEXT == ucSmSFormat)    /* TEXT方式参数检查 */
    {
        if (gucAtParaIndex > 2)  /* 参数过多 */
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        /* 参数设置 */
        /* 设置<da> <toda>*/
        ulRet = At_GetAsciiOrBcdAddr(gastAtParaList[0].aucPara,
                                     gastAtParaList[0].usParaLen,
                                     (TAF_UINT8)gastAtParaList[1].ulParaValue,
                                     gastAtParaList[1].usParaLen,
                                     &gastAtClientTab[ucIndex].AtSmsData.stAsciiAddr,
                                     TAF_NULL_PTR);
        if ((AT_OK != ulRet)
         || (0 == gastAtClientTab[ucIndex].AtSmsData.stAsciiAddr.ulLen))
        {
            AT_NORM_LOG("At_SetCmgsPara: no da.");
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }


    }
    else    /* PDU方式参数检查 */
    {
        if (gucAtParaIndex > 1)  /* 参数过多 */
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
        /* 设置<length> */
        gastAtClientTab[ucIndex].AtSmsData.ucPduLen = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }

    At_SetMode(ucIndex,AT_CMD_MODE,AT_SMS_MODE);         /* 切换为短信模式 */
    /* 设置当前操作类型 */
    if (AT_CMGF_MSG_FORMAT_TEXT == ucSmSFormat)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGS_TEXT_SET;
    }
    else
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGS_PDU_SET;
    }
    return AT_WAIT_SMS_INPUT;   /* 返回提示短信内容输入状态 */
}


TAF_UINT32 At_SetCmssPara(TAF_UINT8 ucIndex)
{
    MN_MSG_SEND_FROMMEM_PARM_STRU       stSendFromMemParm;
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查，如果<index>不存在，直接返回错误 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (gucAtParaIndex > 3))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 设置<da> */
    ulRet = At_GetAsciiOrBcdAddr(gastAtParaList[1].aucPara,
                                 gastAtParaList[1].usParaLen,
                                 (TAF_UINT8)gastAtParaList[2].ulParaValue,
                                 gastAtParaList[2].usParaLen,
                                 VOS_NULL_PTR,
                                 &stSendFromMemParm.stDestAddr);
    if (AT_OK != ulRet)
    {
        return AT_ERROR;
    }

    stSendFromMemParm.enDomain = pstSmsCtx->stCgsmsSendDomain.enSendDomain;
    /* 设置发送存储器 */
    stSendFromMemParm.enMemStore = pstSmsCtx->stCpmsInfo.enMemSendorWrite;
    stSendFromMemParm.ulIndex = gastAtParaList[0].ulParaValue;

    /* 执行命令操作 */
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if (MN_ERR_NO_ERROR == MN_MSG_SendFromMem(gastAtClientTab[ucIndex].usClientId, gastAtClientTab[ucIndex].opId, &stSendFromMemParm))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMSS_SET;
        gastAtClientTab[ucIndex].AtSmsData.ucMsgSentSmNum = 1;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetCmgwPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8                           ucSmSFormat;
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    ucSmSFormat = pstSmsCtx->enCmgfMsgFormat;

    /* 初始化 */
    PS_MEM_SET(&gastAtClientTab[ucIndex].AtSmsData,0x00,sizeof(gastAtClientTab[ucIndex].AtSmsData));
    if (AT_CMGF_MSG_FORMAT_TEXT == ucSmSFormat)    /* TEXT方式参数检查 */
    {
        /* 参数检查 */
        if ((AT_CMD_OPT_SET_PARA_CMD == g_stATParseCmd.ucCmdOptType)
         && (0 == gastAtParaList[0].usParaLen))
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if (gucAtParaIndex > 3)  /* 参数过多 */
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        /* 设置<oa/da> */
        ulRet = At_GetAsciiOrBcdAddr(gastAtParaList[0].aucPara,
                                     gastAtParaList[0].usParaLen,
                                     (TAF_UINT8)gastAtParaList[1].ulParaValue,
                                     gastAtParaList[1].usParaLen,
                                     &gastAtClientTab[ucIndex].AtSmsData.stAsciiAddr,
                                     TAF_NULL_PTR);
        if (AT_OK != ulRet)
        {
            return ulRet;
        }

        /* 设置<stat> */
        if (0 != gastAtParaList[2].usParaLen)
        {
            gastAtClientTab[ucIndex].AtSmsData.SmState = (TAF_UINT8)gastAtParaList[2].ulParaValue;
        }
        else
        {
            gastAtClientTab[ucIndex].AtSmsData.SmState = MN_MSG_STATUS_MO_NOT_SENT;
        }

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGW_TEXT_SET;
    }
    else    /* PDU方式参数检查 */
    {
        /* 参数检查 */
        if (0 == gastAtParaList[0].usParaLen)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if (gucAtParaIndex > 2)  /* 参数过多 */
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if (gastAtParaList[0].ulParaValue > AT_MSG_MAX_TPDU_LEN)
        {
            AT_NORM_LOG("At_SendPduMsgOrCmd: invalid tpdu data length.");
            return AT_ERROR;
        }
        gastAtClientTab[ucIndex].AtSmsData.ucPduLen = (TAF_UINT8)gastAtParaList[0].ulParaValue;

        /* 设置<stat> */
        if (0 != gastAtParaList[1].usParaLen)
        {
            gastAtClientTab[ucIndex].AtSmsData.SmState = (TAF_UINT8)gastAtParaList[1].ulParaValue;
        }
        else
        {
            gastAtClientTab[ucIndex].AtSmsData.SmState = MN_MSG_STATUS_MO_NOT_SENT;
        }

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGW_PDU_SET;
    }

    if ((MN_MSG_STATUS_MT_NOT_READ != gastAtClientTab[ucIndex].AtSmsData.SmState)
     && (MN_MSG_STATUS_MT_READ != gastAtClientTab[ucIndex].AtSmsData.SmState)
     && (MN_MSG_STATUS_MO_NOT_SENT != gastAtClientTab[ucIndex].AtSmsData.SmState)
     && (MN_MSG_STATUS_MO_SENT != gastAtClientTab[ucIndex].AtSmsData.SmState))
    {
        AT_NORM_LOG("At_SetCmgwPara: invalid sm status.");
        return AT_ERROR;
    }

    At_SetMode(ucIndex,AT_CMD_MODE,AT_SMS_MODE);         /* 切换为短信模式 */

    return AT_WAIT_SMS_INPUT;   /* 返回提示短信内容输入状态 */
}



TAF_UINT32 At_SetCmgdPara(TAF_UINT8 ucIndex)
{
    MN_MSG_DELETE_PARAM_STRU            stDelete;
    MN_OPERATION_ID_T                   opId                = At_GetOpId();
    TAF_UINT32                          ulDeleteTypeIndex   = 0;
    TAF_UINT32                          ulDeleteTypes[5]    = {AT_MSG_DELETE_SINGLE,
                                                               AT_MSG_DELETE_READ,
                                                               (AT_MSG_DELETE_READ + AT_MSG_DELETE_SENT),
                                                               (AT_MSG_DELETE_READ + AT_MSG_DELETE_SENT + AT_MSG_DELETE_UNSENT),
                                                               AT_MSG_DELETE_ALL};
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);


    PS_MEM_SET(&stDelete, 0x00, sizeof(stDelete));


    /* 参数检查 */
    if (gucAtParaIndex > 2)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 设置<index> */
    stDelete.ulIndex = gastAtParaList[0].ulParaValue;

    /* 设置删除存储器 */
    stDelete.enMemStore = pstSmsCtx->stCpmsInfo.enMemReadorDelete;

    /* 设置<delflag> */
    if (0 != gastAtParaList[1].usParaLen)
    {
        ulDeleteTypeIndex = gastAtParaList[1].ulParaValue;
    }
    else
    {
        ulDeleteTypeIndex = 0;
    }

    gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes = (TAF_UINT8)ulDeleteTypes[ulDeleteTypeIndex];
    if (AT_MSG_DELETE_SINGLE == gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes)
    {
        if(0 == gastAtParaList[0].usParaLen)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }

    /* 执行命令操作 */
    ulRet = At_MsgDeleteCmdProc(ucIndex, opId, stDelete, gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes);
    if (AT_OK != ulRet)
    {
        return ulRet;
    }

    gastAtClientTab[ucIndex].opId = opId;
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGD_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
}


TAF_UINT32 At_SetCmgcPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8                           ucSmSFormat;
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    ucSmSFormat = pstSmsCtx->enCmgfMsgFormat;

    /* 初始化 */
    PS_MEM_SET(&gastAtClientTab[ucIndex].AtSmsData,0x00,sizeof(gastAtClientTab[ucIndex].AtSmsData));

    if (AT_CMGF_MSG_FORMAT_TEXT == ucSmSFormat)    /* TEXT方式*/
    {
        /* 如果<ct>或<fo>不存在，直接返回错误 */
        if ((0 == gastAtParaList[1].usParaLen)
         || (0 == gastAtParaList[0].usParaLen))
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if (gucAtParaIndex > 6)  /* 参数过多 */
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        /* 设置<fo>.SRR */
        gastAtClientTab[ucIndex].AtSmsData.ucFo         = (TAF_UINT8)gastAtParaList[0].ulParaValue;

        /* 设置<ct> */
        gastAtClientTab[ucIndex].AtSmsData.CommandType  = (TAF_UINT8)gastAtParaList[1].ulParaValue;

        /* 设置<pid> */
        if (0 != gastAtParaList[2].usParaLen)
        {
            gastAtClientTab[ucIndex].AtSmsData.enPid    = (TAF_UINT8)gastAtParaList[2].ulParaValue;
        }
        else
        {
            gastAtClientTab[ucIndex].AtSmsData.enPid    = 0;
        }

        /* 设置<mn> */
        if (0 == gastAtParaList[3].usParaLen)
        {
            gastAtClientTab[ucIndex].AtSmsData.ucMessageNumber  = 0;
        }
        else
        {
            gastAtClientTab[ucIndex].AtSmsData.ucMessageNumber  = (TAF_UINT8)gastAtParaList[3].ulParaValue;
        }

        /* 设置<oa/da> */
        ulRet = At_GetAsciiOrBcdAddr(gastAtParaList[4].aucPara,
                                     gastAtParaList[4].usParaLen,
                                     (TAF_UINT8)gastAtParaList[5].ulParaValue,
                                     gastAtParaList[5].usParaLen,
                                     &gastAtClientTab[ucIndex].AtSmsData.stAsciiAddr,
                                     TAF_NULL_PTR);
        if (AT_OK != ulRet)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGC_TEXT_SET;
    }
    else    /* PDU方式*/
    {
        /* 如果<length>不存在，直接返回错误 */
        if (0 == gastAtParaList[0].usParaLen)
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        if (gucAtParaIndex > 1)  /* 参数过多 */
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        gastAtClientTab[ucIndex].AtSmsData.ucPduLen = (TAF_UINT8)gastAtParaList[0].ulParaValue;
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGC_PDU_SET;
    }

    At_SetMode(ucIndex,AT_CMD_MODE,AT_SMS_MODE);         /* 切换为短信模式 */

    return AT_WAIT_SMS_INPUT;   /* 返回提示短信内容输入状态 */
}

TAF_UINT32 At_SetCmstPara(TAF_UINT8 ucIndex)
{
    MN_MSG_SEND_FROMMEM_PARM_STRU       stSendFromMemParm;
    TAF_UINT32                          ulRet;
    TAF_UINT32                          ulLoop;
    MN_OPERATION_ID_T                   opId                = At_GetOpId();
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查，如果<index>不存在，直接返回错误 */
    if((0 == gastAtParaList[0].usParaLen)
    || (0 == gastAtParaList[3].usParaLen)
    || (gucAtParaIndex > 4)
    || (gastAtParaList[3].ulParaValue < 1))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 设置<da> */
    ulRet = At_GetAsciiOrBcdAddr(gastAtParaList[1].aucPara,
                                 gastAtParaList[1].usParaLen,
                                 (TAF_UINT8)gastAtParaList[2].ulParaValue,
                                 gastAtParaList[2].usParaLen,
                                 VOS_NULL_PTR,
                                 &stSendFromMemParm.stDestAddr);
    if (AT_OK != ulRet)
    {
        return AT_ERROR;
    }

    stSendFromMemParm.enDomain = pstSmsCtx->stCgsmsSendDomain.enSendDomain;
    /* 设置发送存储器 */
    stSendFromMemParm.enMemStore = pstSmsCtx->stCpmsInfo.enMemSendorWrite;
    stSendFromMemParm.ulIndex = gastAtParaList[0].ulParaValue;

    /* 执行命令操作 */
    gastAtClientTab[ucIndex].AtSmsData.ucMsgSentSmNum = (TAF_UINT8)gastAtParaList[3].ulParaValue;
    for(ulLoop = 0; ulLoop < gastAtClientTab[ucIndex].AtSmsData.ucMsgSentSmNum; ulLoop++)
    {
        if(AT_SUCCESS != MN_MSG_SendFromMem(gastAtClientTab[ucIndex].usClientId, opId, &stSendFromMemParm))
        {
            return AT_ERROR;
        }
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMST_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
}


VOS_UINT32 At_SetCmStubPara(VOS_UINT8 ucIndex)
{
    MN_MSG_STUB_MSG_STRU                stStubParam;
    VOS_UINT32                          ulRet;

    /* 输入参数检查: <stubtype>类型是否有效，参数个数不允许超过2个 */
    if ((2 != gucAtParaIndex)
     || (0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen))
    {
        AT_LOG1("At_SetCmStubPara: invalid parameter.", gucAtParaIndex);
        AT_LOG1("At_SetCmStubPara: invalid parameter.", gastAtParaList[0].ulParaValue);
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* A核能处理的桩直接处理并退出: 自动回复功能不需要通知C核 */
    if (MN_MSG_STUB_TYPE_SMS_AUTO_REPLY == gastAtParaList[0].ulParaValue)
    {
        AT_StubTriggerAutoReply(ucIndex, (VOS_UINT8)gastAtParaList[1].ulParaValue);
        return AT_OK;
    }

#if ( VOS_WIN32 == VOS_OS_VER )
    if (MN_MSG_STUB_TYPE_CLASS0_TAILOR == gastAtParaList[0].ulParaValue)
    {
        /* CLASS0类短信接收方式涉及A核和C核两部分，所以此处不能直接返回； */
        g_enClass0Tailor = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    }
#endif

    /* 需要C核处理的桩: 发送消息给C核 */
    stStubParam.enStubType = (MN_MSG_STUB_TYPE_ENUM_U32)gastAtParaList[0].ulParaValue;
    stStubParam.ulValue    = gastAtParaList[1].ulParaValue;

    gastAtClientTab[ucIndex].opId = At_GetOpId();
    ulRet = MN_MSG_ReqStub(gastAtClientTab[ucIndex].usClientId,
                           gastAtClientTab[ucIndex].opId,
                           &stStubParam);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return ulRet;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMSTUB_SET;
    return AT_WAIT_ASYNC_RETURN;
}

/*****************************************************************************
 函 数 名  : At_SetCmmsPara
 功能描述  : +CMMS=[<n>]
 输入参数  : TAF_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : AT_XXX  --- ATC返回码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年07月21日
    作    者   : Fu Yingjun id:62575
    修改内容   : 新生成函数
*****************************************************************************/
TAF_UINT32 At_SetCmmsPara(
    TAF_UINT8                           ucIndex
)
{
    MN_MSG_SET_LINK_CTRL_STRU     stSetParam;
    TAF_UINT32                          ulRet;
    MN_OPERATION_ID_T                   opId                = At_GetOpId();

    if (0 == gucAtParaIndex)
    {
        stSetParam.enRelayLinkCtrl = MN_MSG_LINK_CTRL_ENABLE;
    }
    else
    {
        stSetParam.enRelayLinkCtrl = (MN_MSG_LINK_CTRL_U8)gastAtParaList[0].ulParaValue;
    }

    ulRet = MN_MSG_SetLinkCtrl(gastAtClientTab[ucIndex].usClientId, opId, &stSetParam);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].opId = opId;
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMMS_SET;
    return AT_WAIT_ASYNC_RETURN;
}
#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))
VOS_UINT32 AT_ParseCbParm(
    VOS_UINT8                           *pucInPara,
    TAF_CBA_CBMI_RANGE_LIST_STRU        *pstCbRange
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          iLoop;
    VOS_UINT32                          ulParm_count;
    TAF_UINT8                           aucTemp[TAF_CBA_MAX_CBMID_RANGE_NUM];
    VOS_BOOL                            bRange;
    VOS_UINT32                          ulValue;

    pstCbRange->usCbmirNum      = 0;
    iLoop                       = 0;
    bRange                      = VOS_FALSE;
    ulParm_count                = 0;
    i                           = 0;

    while ( ( pucInPara[iLoop] != '\0' )
         && ( i < TAF_CBA_MAX_CBMID_RANGE_NUM))
    {
        i = 0;
        while ( ( pucInPara[iLoop] != ',' )
             && ( pucInPara[iLoop] != '-' )
             && ( pucInPara[iLoop] != '\0' ) )
        {
            aucTemp[i++] = pucInPara[iLoop++];
            if ( i >= 6 )
            {
                return VOS_ERR;
            }
        }

        aucTemp[i++] = '\0';

        if ( 0 == VOS_StrLen((VOS_CHAR*)aucTemp) )
        {
            return VOS_ERR;
        }

        if ( bRange )
        {
            if(AT_SUCCESS != At_Auc2ul(aucTemp,
                                        (VOS_UINT16)VOS_StrLen((VOS_CHAR*)aucTemp),
                                        &ulValue))
            {
                return VOS_ERR;
            }

            if ( ulValue > 0xFFFF )
            {
                return VOS_ERR;
            }

            if ( ulParm_count >= 1 )
            {
                pstCbRange->astCbmiRangeInfo[ulParm_count - 1].usMsgIdTo = (VOS_UINT16)ulValue;

                bRange = VOS_FALSE;
            }

        }
        else
        {
            if(AT_SUCCESS != At_Auc2ul(aucTemp,
                                       (VOS_UINT16)VOS_StrLen((VOS_CHAR*)aucTemp),
                                       &ulValue))
            {
                return VOS_ERR;
            }

            if ( ulValue > 0xFFFF )
            {
                return VOS_ERR;
            }

            pstCbRange->astCbmiRangeInfo[ulParm_count].usMsgIdFrom = (VOS_UINT16)ulValue;
            pstCbRange->astCbmiRangeInfo[ulParm_count].usMsgIdTo = (VOS_UINT16)ulValue;
            (pstCbRange->usCbmirNum)++;
            ulParm_count++;
        }

        if ( '-' == pucInPara[iLoop])
        {
            bRange = VOS_TRUE;
        }
        else if ( '\0' == pucInPara[iLoop] )
        {
            return VOS_OK;
        }
        else
        {
            bRange = VOS_FALSE;
        }

        iLoop++;

        if ( ulParm_count >= AT_MAX_CBS_MSGID_NUM )
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_ProcCscbWithParm(
    TAF_UINT8                           ucIndex,
    TAF_CBA_CBMI_RANGE_LIST_STRU       *pstCbmirList
)
{
    VOS_UINT32                          ulRet;
    TAF_CBA_CBMI_RANGE_LIST_STRU       *pstDcssInfo;
    VOS_UINT32                          ulAppRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    PS_MEM_SET(pstCbmirList, 0x00, sizeof(TAF_CBA_CBMI_RANGE_LIST_STRU));

    ulRet = VOS_ERR;

    /* 表明 <mids> 有效*/
    if ( gastAtParaList[1].usParaLen > 0 )
    {
        gastAtParaList[1].aucPara[gastAtParaList[1].usParaLen] = '\0';
        ulRet = AT_ParseCbParm(gastAtParaList[1].aucPara,pstCbmirList);
    }

    /* 多个参数,MID必须存在,而且还必须合法,否则返回出错 */
    if ( VOS_OK != ulRet )
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 表明 <dcss> 有效*/
    if ( gastAtParaList[2].usParaLen > 0 )
    {
        pstDcssInfo = (TAF_CBA_CBMI_RANGE_LIST_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT,
                            sizeof(TAF_CBA_CBMI_RANGE_LIST_STRU));

        if ( VOS_NULL == pstDcssInfo )
        {
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }

        gastAtParaList[2].aucPara[gastAtParaList[2].usParaLen] = '\0';
        ulRet = AT_ParseCbParm(gastAtParaList[2].aucPara,pstDcssInfo);
        if ( VOS_OK ==  ulRet)
        {
            PS_MEM_CPY(&(pstSmsCtx->stCbsDcssInfo),pstDcssInfo,sizeof(pstSmsCtx->stCbsDcssInfo));
        }
        else
        {
            PS_MEM_FREE(WUEPS_PID_AT, pstDcssInfo);
            return AT_CMS_OPERATION_NOT_ALLOWED;
        }
        PS_MEM_FREE(WUEPS_PID_AT, pstDcssInfo);
    }

    if( MN_MSG_CBMODE_ACCEPT  ==  gastAtParaList[0].ulParaValue)
    {
        ulAppRet = MN_MSG_AddCbMids(gastAtClientTab[ucIndex].usClientId,
                                 gastAtClientTab[ucIndex].opId,
                                 pstCbmirList);
    }
    else
    {
        ulAppRet = MN_MSG_DelCbMids(gastAtClientTab[ucIndex].usClientId,
                                 gastAtClientTab[ucIndex].opId,
                                 pstCbmirList);
    }

    ulRet = VOS_OK;
    /*转换API调用结果*/
    if ( MN_ERR_NO_ERROR != ulAppRet )
    {
        ulRet = VOS_ERR;
    }

    if ( VOS_OK != ulRet )
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    return AT_OK;
}


TAF_UINT32 At_SetCscbPara(
    TAF_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulRet;
    TAF_CBA_CBMI_RANGE_LIST_STRU       *pstCbmirList;
    VOS_UINT32                          ulApiRet;

    /* 如果参数个数大于1,则第二个参数必须存在 */
    if (( gucAtParaIndex > 1 )
      && ( 0 == gastAtParaList[1].usParaLen ))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    pstCbmirList = (TAF_CBA_CBMI_RANGE_LIST_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT,
                        sizeof(TAF_CBA_CBMI_RANGE_LIST_STRU));

    if ( VOS_NULL_PTR == pstCbmirList )
    {
        AT_WARN_LOG("At_SetCscbPara: Fail to alloc memory.");
        return AT_CMS_ERROR_IN_MS;
    }

    gastAtClientTab[ucIndex].opId = At_GetOpId();

    ulRet = AT_OK;
    if ( ( 1 == gucAtParaIndex )
      && ( MN_MSG_CBMODE_ACCEPT  ==  gastAtParaList[0].ulParaValue))
    {
        /* 当前仅存在一个参数,表明是激活所有CBS消息 */
        pstCbmirList->usCbmirNum = 1;
        pstCbmirList->astCbmiRangeInfo[0].usMsgIdFrom = 0;
        pstCbmirList->astCbmiRangeInfo[0].usMsgIdTo = 0xFFFF;
        ulApiRet = MN_MSG_AddCbMids(gastAtClientTab[ucIndex].usClientId,
                                 gastAtClientTab[ucIndex].opId,
                                 pstCbmirList);

        /*转换API调用结果*/
        if ( MN_ERR_NO_ERROR != ulApiRet )
        {
            ulRet = AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }
    else if ( ( 1 == gucAtParaIndex )
      && ( MN_MSG_CBMODE_REJECT  ==  gastAtParaList[0].ulParaValue ))
    {
        /* 当前仅存在一个参数,表明是去激活所有CBS消息 */
        ulApiRet = MN_MSG_EmptyCbMids(gastAtClientTab[ucIndex].usClientId,
                                    gastAtClientTab[ucIndex].opId);

        /*转换API调用结果*/
        if ( MN_ERR_NO_ERROR != ulApiRet )
        {
            ulRet = AT_CMS_OPERATION_NOT_ALLOWED;
        }
    }
    else
    {
        /* 处理带有参数等情况 */
        ulRet = AT_ProcCscbWithParm(ucIndex,pstCbmirList);
    }

    PS_MEM_FREE(WUEPS_PID_AT, pstCbmirList);

    if (AT_OK != ulRet)
    {
        return ulRet;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSCB_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
}

#endif
TAF_UINT32 At_SetClipPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSsCtx->ucClipType = (AT_CLIP_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSsCtx->ucClipType = AT_CLIP_DISABLE_TYPE;
    }

    return AT_OK;
}
TAF_UINT32 At_SetClirPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        switch (gastAtParaList[0].ulParaValue)
        {
            case AT_CLIR_AS_SUBSCRIPT:
                pstSsCtx->ucClirType = AT_CLIR_AS_SUBSCRIPT;
                break;

            case AT_CLIR_INVOKE:
                pstSsCtx->ucClirType = AT_CLIR_INVOKE;
                break;

            case AT_CLIR_SUPPRESS:
                pstSsCtx->ucClirType = AT_CLIR_SUPPRESS;
                break;

            default:
                return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        pstSsCtx->ucClirType = AT_CLIR_AS_SUBSCRIPT;
    }
    return AT_OK;
}
VOS_UINT32 At_SetColpPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSsCtx->ucColpType = (AT_COLP_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSsCtx->ucColpType = AT_COLP_DISABLE_TYPE;
    }

    return AT_OK;
}
/*****************************************************************************
 Prototype      : At_SetCscsPara
 Description    : +CSCS=[<chset>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCscsPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        gucAtCscsType = (AT_CSCS_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        gucAtCscsType = AT_CSCS_IRA_CODE;
    }
    return AT_OK;
}
/*****************************************************************************
 Prototype      : At_SetCmeePara
 Description    : +CMEE=[<n>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCmeePara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        gucAtCmeeType = (AT_CMEE_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        gucAtCmeeType = AT_CMEE_ONLY_ERROR;
    }
    return AT_OK;
}
/*****************************************************************************
 Prototype      : At_SetCeerPara
 Description    : +CEER
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCeerPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    gstAtSendData.usBufLen  = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,gastAtClientTab[ucIndex].ulCause);
    return AT_OK;
}

TAF_UINT32 At_SetCregPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU                  *pstNetCtx = VOS_NULL_PTR;

    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if (0 != gastAtParaList[0].usParaLen)
    {
        pstNetCtx->ucCregType   = (AT_CREG_TYPE)gastAtParaList[0].ulParaValue;
        stAtCmd.u.ucCregRptFlg  = (VOS_UINT8)gastAtParaList[0].ulParaValue;

        /* 开机默认TAF_EVENT_INFO_CELL_SIGN+TAF_EVENT_INFO_CELL_ID + TAF_EVENT_INFO_CELL_BLER
           都打开主动上报，此处无需再通知接入层开启主动上报，目前没有停止，停止了会影响CSQ查询*/
    }
    else
    {
        pstNetCtx->ucCregType   = AT_CREG_RESULT_CODE_NOT_REPORT_TYPE;
        stAtCmd.u.ucCregRptFlg  = AT_CREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    stAtCmd.enReqType = AT_MTA_SET_CREG_RPT_TYPE;

    /* 给MTA发送+creg设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                      &stAtCmd,
                                      sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                      I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 At_SetCgregPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU                  *pstNetCtx = VOS_NULL_PTR;

    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstNetCtx->ucCgregType  = (AT_CGREG_TYPE)gastAtParaList[0].ulParaValue;
        stAtCmd.u.ucCgregRptFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

        /* 开机默认TAF_EVENT_INFO_CELL_SIGN+TAF_EVENT_INFO_CELL_ID + TAF_EVENT_INFO_CELL_BLER
           都打开主动上报，此处无需再通知接入层开启主动上报，目前没有停止，停止了会影响CSQ查询*/
    }
    else
    {
        pstNetCtx->ucCgregType  = AT_CGREG_RESULT_CODE_NOT_REPORT_TYPE;
        stAtCmd.u.ucCgregRptFlg = AT_CGREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    stAtCmd.enReqType       = AT_MTA_SET_CGREG_RPT_TYPE;

    /* 给MTA发送+cgreg设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

#if(FEATURE_ON == FEATURE_LTE)
VOS_UINT32 AT_SetCeregPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU                  *pstNetCtx = VOS_NULL_PTR;

    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if (0 != gastAtParaList[0].usParaLen)
    {
        pstNetCtx->ucCeregType  = (AT_CEREG_TYPE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
        stAtCmd.u.ucCeregRptFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

        /* 开机默认TAF_EVENT_INFO_CELL_SIGN+TAF_EVENT_INFO_CELL_ID + TAF_EVENT_INFO_CELL_BLER
           都打开主动上报，此处无需再通知接入层开启主动上报，目前没有停止，停止了会影响CSQ查询*/
    }
    else
    {
        pstNetCtx->ucCeregType  = AT_CEREG_RESULT_CODE_NOT_REPORT_TYPE;
        stAtCmd.u.ucCeregRptFlg = AT_CEREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    stAtCmd.enReqType       = AT_MTA_SET_CEREG_RPT_TYPE;

    /* 给MTA发送+cgreg设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

#endif
VOS_UINT32 At_SetClccPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发消息到C核获取当前所有通话信息 */
    ulRet = MN_CALL_GetCallInfos(gastAtClientTab[ucIndex].usClientId,
                                 gastAtClientTab[ucIndex].opId,
                                 0);
    if (VOS_OK != ulRet)
    {
        AT_WARN_LOG("At_SetClccPara: MN_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCC_SET;

    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 At_SetCpasPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    AT_CPAS_STATUS_ENUM_U8              enCpas;
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity;

    pstAppPdpEntity                     = AT_APP_GetPdpEntInfoAddr();

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4State)
      || (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv6State)
      || (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4v6State) )
    {
        enCpas = AT_CPAS_STATUS_RING;

        gstAtSendData.usBufLen  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         "%s: %d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         enCpas);
        return AT_OK;
    }
    else if ( (AT_PDP_STATE_ACTED    == pstAppPdpEntity->enIpv4State)
           || (AT_PDP_STATE_DEACTING == pstAppPdpEntity->enIpv4State)
           || (AT_PDP_STATE_ACTED    == pstAppPdpEntity->enIpv6State)
           || (AT_PDP_STATE_DEACTING == pstAppPdpEntity->enIpv6State)
           || (AT_PDP_STATE_ACTED    == pstAppPdpEntity->enIpv4v6State)
           || (AT_PDP_STATE_DEACTING == pstAppPdpEntity->enIpv4v6State) )

    {
        enCpas = AT_CPAS_STATUS_CALL_IN_PROGRESS;

        gstAtSendData.usBufLen  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         "%s: %d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         enCpas);
        return AT_OK;
    }
    else
    {
        /* for pclint */
    }

    /* 发消息到C核获取当前所有通话信息 */
    ulRet = MN_CALL_GetCallInfos(gastAtClientTab[ucIndex].usClientId,
                                 gastAtClientTab[ucIndex].opId,
                                 0);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("At_SetCpasPara: MN_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPAS_SET;

    return AT_WAIT_ASYNC_RETURN;


}
/*****************************************************************************
 Prototype      : At_SetCstaPara
 Description    : +CSTA=[<type>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCstaPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    g_enAtCstaNumType = (AT_CSTA_NUM_TYPE_ENUM_U8)gastAtParaList[0].ulParaValue;

    return AT_OK;
}
TAF_UINT32 At_SetCcugPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    /* 设置<n> */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSsCtx->stCcugCfg.bEnable = gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSsCtx->stCcugCfg.bEnable = 0;
    }

    /* 设置<index> */
    if(0 != gastAtParaList[1].usParaLen)
    {
        if (10 == gastAtParaList[1].ulParaValue)
        {
            pstSsCtx->stCcugCfg.bIndexPresent = 0;
        }
        else
        {
            pstSsCtx->stCcugCfg.bIndexPresent = 1;
            pstSsCtx->stCcugCfg.ulIndex = (TAF_UINT8)gastAtParaList[1].ulParaValue;
        }
    }
    else
    {
        pstSsCtx->stCcugCfg.bIndexPresent = 0;
    }

    /* 设置<info> */
    if(0 != gastAtParaList[2].usParaLen)
    {
        pstSsCtx->stCcugCfg.bSuppressPrefer = (gastAtParaList[2].ulParaValue & 0x02) >> 1;
        pstSsCtx->stCcugCfg.bSuppressOA     = gastAtParaList[2].ulParaValue & 0x01;
    }
    else
    {
        pstSsCtx->stCcugCfg.bSuppressPrefer = 0;
        pstSsCtx->stCcugCfg.bSuppressOA     = 0;
    }
    return AT_OK;
}
TAF_UINT32 At_SetCssnPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    MN_CALL_SET_CSSN_REQ_STRU           stCssnReq;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stCssnReq, 0, sizeof(stCssnReq));
    stCssnReq.ulActNum      = MN_CALL_MAX_CSSN_MSG_NUM;
    stCssnReq.aenSetType[0] = MN_CALL_SET_CSSN_DEACT;
    stCssnReq.aenSetType[1] = MN_CALL_SET_CSSN_DEACT;

    if(0 != gastAtParaList[0].usParaLen)
    {
        stCssnReq.aenSetType[0] = (MN_CALL_SET_CSSN_TYPE_ENUM_U32)gastAtParaList[0].ulParaValue;
    }

    if(0 != gastAtParaList[1].usParaLen)
    {
        stCssnReq.aenSetType[1] = (MN_CALL_SET_CSSN_TYPE_ENUM_U32)gastAtParaList[1].ulParaValue;
    }

    ulRst = MN_CALL_SendAppRequest(MN_CALL_APP_SET_CSSN_REQ,
                                   gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   0,
                                   (MN_CALL_APP_REQ_PARM_UNION*)&stCssnReq);
    if (VOS_OK == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSSN_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

TAF_BOOL At_ParseSingleChldPara(
    TAF_UINT32                          ulParaValue,
    MN_CALL_SUPS_PARAM_STRU            *pstCallSupsCmd
    )
{

    switch ( ulParaValue )
    {
    case 0:
        pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_REL_HELD_OR_UDUB;
        break;

    case 1:
        pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_REL_ACT_ACPT_OTH;
        break;

    case 2:
        pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH;
        break;

    case 3:
        pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_BUILD_MPTY;
        break;

    case 4:
        pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_ECT;
        break;

    case 5:
       pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_ACT_CCBS;
       return AT_OK;

    case 6:
       pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_ECONF_REL_USER;
       break;

    case 7:
       pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_ECONF_MERGE_CALL;
       break;

    default:
        return AT_ERROR;
    }
    return AT_OK;
}


TAF_BOOL At_ParseChldPara(
    TAF_UINT32                          ulParaValue,
    MN_CALL_SUPS_PARAM_STRU            *pstCallSupsCmd
    )
{
    TAF_BOOL                            bRetRst;
    TAF_UINT8                           ucTmp;

    ucTmp = (TAF_UINT8)(ulParaValue / 10);

    if (ulParaValue < 10)
    {
        bRetRst = At_ParseSingleChldPara(ulParaValue,pstCallSupsCmd);
    }
    else
    {
        switch(ucTmp)
        {
        case 1:
            pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_REL_CALL_X;
            pstCallSupsCmd->callId = (MN_CALL_ID_T)(ulParaValue % 10);
            bRetRst = AT_OK;
            break;

        case 2:
            pstCallSupsCmd->enCallSupsCmd = MN_CALL_SUPS_CMD_HOLD_ALL_EXCPT_X;
            pstCallSupsCmd->callId = (MN_CALL_ID_T)(ulParaValue % 10);
            bRetRst = AT_OK;
            break;

        default:
            bRetRst = AT_ERROR;
            break;
        }
    }
    return bRetRst;
}


TAF_UINT32 At_SetChldPara(TAF_UINT8 ucIndex)
{
    MN_CALL_SUPS_PARAM_STRU             stCallChldParam;
    VOS_UINT32                          ulRst;


    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stCallChldParam,0,sizeof(stCallChldParam));

    ulRst = At_ParseChldPara(gastAtParaList[0].ulParaValue,&stCallChldParam);
    if (AT_ERROR == ulRst)
    {
        return AT_ERROR;
    }

    if(AT_SUCCESS == MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId,0, &stCallChldParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CHLD_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 At_SetChldExPara(VOS_UINT8 ucIndex)
{
    MN_CALL_SUPS_PARAM_STRU             stCallChldParam;
    VOS_UINT32                          ulRst;

    /* 参数过多 */
    if (gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 只有增强型多方通话踢人的场景下，才有第3个参数，即chld=6,0,"135xxxxxxxx" */
    if ((3 == gucAtParaIndex)
     && (gastAtParaList[0].ulParaValue != 6))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stCallChldParam, 0, sizeof(stCallChldParam));

    stCallChldParam.enCallType = (MN_CALL_TYPE_ENUM_U8)gastAtParaList[1].ulParaValue;

    ulRst = At_ParseChldPara(gastAtParaList[0].ulParaValue, &stCallChldParam);
    if (AT_ERROR == ulRst)
    {
        return AT_ERROR;
    }

    /* 如果是需要释放多方通话中成员，需要携带电话号码 */
    if (MN_CALL_SUPS_CMD_ECONF_REL_USER == stCallChldParam.enCallSupsCmd)
    {
        /* 没有携带号码 */
        if (0 == gastAtParaList[2].usParaLen)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 检查并转换电话号码 */
        if (VOS_OK !=  AT_FillCalledNumPara((VOS_UINT8 *)gastAtParaList[2].aucPara,
                                            gastAtParaList[2].usParaLen,
                                            &stCallChldParam.stRemoveNum))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if (AT_SUCCESS == MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId, 0, &stCallChldParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CHLD_EX_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 At_SetChupPara(VOS_UINT8 ucIndex)
{
    MN_CALL_SUPS_PARAM_STRU             stCallMgmtParam;

    PS_MEM_SET(&stCallMgmtParam, 0x0, sizeof(stCallMgmtParam));

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stCallMgmtParam.enCallSupsCmd = MN_CALL_SUPS_CMD_REL_ALL_CALL;
    if(AT_SUCCESS == MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId, 0, &stCallMgmtParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CHUP_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_CheckDtmfKey(AT_PARSE_PARA_TYPE_STRU *pstAtPara)
{
    VOS_CHAR                            cKey;

    if ( 1 != pstAtPara->usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SUCCESS != At_UpString(pstAtPara->aucPara, pstAtPara->usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    cKey = (VOS_CHAR)pstAtPara->aucPara[0];
    if (!( (('0' <= cKey) && ('9' >= cKey))
         || ('*' == cKey) || ('#' == cKey)
         || ('A' == cKey) || ('B' == cKey)
         || ('C' == cKey) || ('D' == cKey) ))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}


TAF_UINT32 At_SetVtsPara(TAF_UINT8 ucIndex)
{
    TAF_CALL_DTMF_PARAM_STRU            stDtmfParam;
    VOS_UINT32                          ulResult;

    /* 参数类型个数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数值有效性检查 */
    ulResult = AT_CheckDtmfKey(&gastAtParaList[0]);

    if (AT_SUCCESS != ulResult)
    {
        return ulResult;
    }

    /* 参数初始化 */
    PS_MEM_SET(&stDtmfParam, 0, sizeof(stDtmfParam));

    /* 发送START DTMF请求 */
    stDtmfParam.cKey        = (VOS_CHAR)gastAtParaList[0].aucPara[0];
    stDtmfParam.usOnLength  = AT_VTS_DEFAULT_DTMF_LENGTH;

    stDtmfParam.usOffLength = 0;

    ulResult = TAF_CALL_SendDtmf(MN_CALL_APP_START_DTMF_REQ,
                                gastAtClientTab[ucIndex].usClientId,
                                gastAtClientTab[ucIndex].opId,
                                &stDtmfParam);

    if (VOS_OK == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VTS_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
VOS_UINT32 At_CheckDtmfPara(VOS_VOID)
{
    /*参数有效性检查*/
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((gucAtParaIndex > 4)
     || (0 == gastAtParaList[2].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送DTMF时,需要带参数ckey,需要检查参数ckey是否合法
       停止DTMF时,不需要带参数ckey,不需要检查参数ckey是否合法 */
    if (AT_DTMF_STOP != gastAtParaList[2].ulParaValue)
    {
        if (AT_SUCCESS != AT_CheckDtmfKey(&gastAtParaList[1]))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}



VOS_UINT32 At_SetDtmfPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    TAF_CALL_DTMF_PARAM_STRU            stDtmfParam;

    /*参数有效性检查*/
    ulResult = At_CheckDtmfPara();
    if (AT_SUCCESS != ulResult)
    {
        return ulResult;
    }

    PS_MEM_SET(&stDtmfParam, 0, sizeof(stDtmfParam));

    stDtmfParam.CallId  = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stDtmfParam.cKey    = (VOS_CHAR)gastAtParaList[1].aucPara[0];

    /* 如果时长为1，则使用默认DTMF音时长 */
    if (AT_DTMF_START == gastAtParaList[2].ulParaValue)
    {
        stDtmfParam.usOnLength = AT_DTMF_DEFAULT_DTMF_LENGTH;
    }
    else
    {
        stDtmfParam.usOnLength = (VOS_UINT16)gastAtParaList[2].ulParaValue;
    }

    if ((0 == gastAtParaList[3].ulParaValue)
     || (gucAtParaIndex < 4))
    {
        stDtmfParam.usOffLength = 0;
    }
    else
    {
        stDtmfParam.usOffLength   = (VOS_UINT16)gastAtParaList[3].ulParaValue;

        if (stDtmfParam.usOffLength < AT_DTMF_MIN_DTMF_OFF_LENGTH)
        {
            /* offlength取值为0或者10ms-60000ms，小于10ms，osa无法启动小于10ms定时器，返回参数错误 */
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if (AT_DTMF_STOP != stDtmfParam.usOnLength)
    {
        ulResult = TAF_CALL_SendDtmf(MN_CALL_APP_START_DTMF_REQ,
                                    gastAtClientTab[ucIndex].usClientId,
                                    gastAtClientTab[ucIndex].opId,
                                    &stDtmfParam);
    }
    else
    {
        ulResult = TAF_CALL_SendDtmf(MN_CALL_APP_STOP_DTMF_REQ,
                                    gastAtClientTab[ucIndex].usClientId,
                                    gastAtClientTab[ucIndex].opId,
                                    &stDtmfParam);
    }

    if (VOS_OK == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DTMF_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}


TAF_UINT32 At_SetCrcPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSsCtx->ucCrcType = (AT_CRC_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSsCtx->ucCrcType = AT_CRC_DISABLE_TYPE;
    }

    return AT_OK;
}
TAF_UINT32 At_SetCbstPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSsCtx->stCbstDataCfg.enSpeed = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSsCtx->stCbstDataCfg.enSpeed = MN_CALL_CSD_SPD_64K_MULTI;
    }
    if(0 != gastAtParaList[1].usParaLen)
    {
        pstSsCtx->stCbstDataCfg.enName = (TAF_UINT8)gastAtParaList[1].ulParaValue;
    }
    else
    {
        pstSsCtx->stCbstDataCfg.enName = MN_CALL_CSD_NAME_SYNC_UDI;
    }
    if(0 != gastAtParaList[2].usParaLen)
    {
        pstSsCtx->stCbstDataCfg.enConnElem = (TAF_UINT8)gastAtParaList[2].ulParaValue;
    }
    else
    {
        pstSsCtx->stCbstDataCfg.enConnElem = MN_CALL_CSD_CE_T;
    }

    return AT_OK;
}

TAF_UINT32 At_SetCmodPara(TAF_UINT8 ucIndex)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    pstSsCtx->enCModType = (TAF_UINT8)gastAtParaList[0].ulParaValue;

    return AT_OK;

}
VOS_UINT32 At_SetCmmiPara(VOS_UINT8 ucIndex)
{
    VOS_BOOL                            inCall;
    MN_MMI_OPERATION_PARAM_STRU        *pMmiOpParam;
    VOS_CHAR                           *pcOutRestMmiStr;
    VOS_UINT32                          ulResult;
    AT_CMD_CURRENT_OPT_ENUM             enCmdCurrentOpt;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;

    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucIndex);

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 没有呼叫存在的情况 */
    if (VOS_FALSE == pstCcCtx->ulCurIsExistCallFlag)
    {
        inCall = VOS_FALSE;
    }
    else
    {
        inCall = VOS_TRUE;
    }

    pMmiOpParam = (MN_MMI_OPERATION_PARAM_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(MN_MMI_OPERATION_PARAM_STRU));
    if(TAF_NULL_PTR == pMmiOpParam)
    {
        AT_WARN_LOG("At_SetCmmiPara Mem Alloc FAILURE");
        return AT_ERROR;
    }

    f_enClirOperate = MN_CALL_CLIR_AS_SUBS;

    pcOutRestMmiStr = (VOS_CHAR *)gastAtParaList[0].aucPara;
    pcOutRestMmiStr[gastAtParaList[0].usParaLen] = '\0';
    ulResult = MN_MmiStringParse((VOS_CHAR *)(gastAtParaList[0].aucPara), inCall, pMmiOpParam, &pcOutRestMmiStr);
    pMmiOpParam->ProcessUssdReq.DatacodingScheme = TAF_SS_7bit_LANGUAGE_UNSPECIFIED;

    if(MN_ERR_NO_ERROR == ulResult)
    {
        switch(pMmiOpParam->MmiOperationType)
        {
        case TAF_MMI_SUPPRESS_CLIR:
        case TAF_MMI_INVOKE_CLIR:

            if(TAF_MMI_SUPPRESS_CLIR == pMmiOpParam->MmiOperationType)
            {
                f_enClirOperate = MN_CALL_CLIR_SUPPRESS;
            }
            else
            {
                f_enClirOperate = MN_CALL_CLIR_INVOKE;
            }

            if((VOS_UINT16)(pcOutRestMmiStr - (VOS_CHAR *)(gastAtParaList[0].aucPara)) < gastAtParaList[0].usParaLen)
            {
                ulResult = MN_MmiStringParse(pcOutRestMmiStr, inCall, pMmiOpParam, &pcOutRestMmiStr);
            }
            else
            {
                PS_MEM_FREE(WUEPS_PID_AT, pMmiOpParam);
                return AT_CME_INCORRECT_PARAMETERS;
            }
            break;
        default :
            break;
        }
        if(MN_ERR_NO_ERROR != ulResult)
        {
            PS_MEM_FREE(WUEPS_PID_AT, pMmiOpParam);
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 保存密码 */

        /* 调用TAF提供的接口，处理SS命令 */
        enCmdCurrentOpt = At_GetMnOptType(pMmiOpParam->MmiOperationType);
        if(AT_CMD_INVALID == enCmdCurrentOpt)
        {
            PS_MEM_FREE(WUEPS_PID_AT, pMmiOpParam);
            return AT_ERROR;
        }
        switch(enCmdCurrentOpt)
        {
            case AT_CMD_D_CS_VOICE_CALL_SET:
                if (VOS_OK != MN_CALL_Orig(gastAtClientTab[ucIndex].usClientId,
                    0,
                    &gastAtClientTab[ucIndex].AtCallInfo.CallId,
                    &(pMmiOpParam->MnCallOrig)))
                {
                    AT_WARN_LOG("At_SetCmmiPara: MN_CALL_Orig failed.");
                }
                break;
            case AT_CMD_CHLD_SET:
                if (VOS_OK != MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->MnCallSupsReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: MN_CALL_Sups failed.");
                }
                break;
            case AT_CMD_CPIN_UNBLOCK_SET:
                if (TAF_SUCCESS != Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->PinReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: Taf_PhonePinHandle failed.");
                }
                break;
            case AT_CMD_CPIN2_UNBLOCK_SET:
                if (TAF_SUCCESS != Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->PinReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: Taf_PhonePinHandle failed.");
                }
                break;
            case AT_CMD_CPIN_VERIFY_SET:
                if (TAF_SUCCESS != Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->PinReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: Taf_PhonePinHandle failed.");
                }

                break;
            case AT_CMD_CPIN2_VERIFY_SET:
                if (TAF_SUCCESS != Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->PinReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: Taf_PhonePinHandle failed.");
                }
                break;
            case AT_CMD_SS_REGISTER:
                if (TAF_SUCCESS != TAF_RegisterSSReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->RegisterSsReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_RegisterSSReq failed.");
                }
                break;
            case AT_CMD_SS_ERASE:
                if (TAF_SUCCESS != TAF_EraseSSReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->EraseSsReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_EraseSSReq failed.");
                }
                break;
            case AT_CMD_SS_ACTIVATE:
                if (TAF_SUCCESS != TAF_ActivateSSReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->ActivateSsReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_ActivateSSReq failed.");
                }
                break;
            case AT_CMD_SS_DEACTIVATE:
                if (TAF_SUCCESS != TAF_DeactivateSSReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->DeactivateSsReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_DeactivateSSReq failed.");
                }
                break;

             case AT_CMD_SS_DEACTIVE_CCBS:
                if (TAF_SUCCESS != TAF_EraseCCEntryReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->stCcbsEraseReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_EraseCCEntryReq failed.");
                }
                break;

            case AT_CMD_SS_INTERROGATE:
            case AT_CMD_SS_INTERROGATE_CCBS:
                if (TAF_SUCCESS != TAF_InterrogateSSReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->InterrogateSsReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_DeactivateSSReq failed.");
                }
                break;
            case AT_CMD_SS_REGISTER_PSWD:
                if (TAF_SUCCESS != TAF_RegisterPasswordReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->RegPwdReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_InterrogateSSReq failed.");
                }
                break;
            /* 删除AT_CMD_SS_GET_PSWD 分支 */
            case AT_CMD_SS_USSD:
                if (TAF_SUCCESS != TAF_ProcessUnstructuredSSReq(gastAtClientTab[ucIndex].usClientId,0, &pMmiOpParam->ProcessUssdReq))
                {
                    AT_WARN_LOG("At_SetCmmiPara: TAF_ProcessUnstructuredSSReq failed.");
                }
                break;
            case AT_CMD_CLIP_READ:
                pstSsCtx->ucClipType = (AT_CLIP_TYPE)gastAtParaList[0].ulParaValue;
                PS_MEM_FREE(WUEPS_PID_AT, pMmiOpParam);
                return AT_OK;
            default:
                break;
        }
        gastAtClientTab[ucIndex].CmdCurrentOpt = enCmdCurrentOpt;
        PS_MEM_FREE(WUEPS_PID_AT, pMmiOpParam);
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        PS_MEM_FREE(WUEPS_PID_AT, pMmiOpParam);
        return AT_CME_INCORRECT_PARAMETERS;
    }
}



VOS_UINT32 AT_CheckApnFirstAndLastChar(
    VOS_UINT8                           ucApnCharacter
)
{
    if ( ! ( (('A' <= ucApnCharacter) && ('Z' >= ucApnCharacter))
          || (('0' <= ucApnCharacter) && ('9' >= ucApnCharacter)) ) )
    {
        AT_WARN_LOG("AT_CheckApnFirstAndLastChar: WARNING: Invalid begin/end character in APN.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_CheckApnFormat(
    VOS_UINT8                          *pucApn,
    VOS_UINT16                          usApnLen
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucApnCharacter;

    for ( i = 0 ; i < usApnLen; i++ )
    {
        /* 转换APN字符为大写 */
        ucApnCharacter = AT_UPCASE(pucApn[i]);

        /* 检查APN字符有效性 */
        if ( ! ( (('A' <= ucApnCharacter) && ('Z' >= ucApnCharacter))
              || (('0' <= ucApnCharacter) && ('9' >= ucApnCharacter))
              || ('-' == ucApnCharacter)
              || ('.' == ucApnCharacter) ) )
        {
            AT_WARN_LOG("AT_CheckApn: WARNING: Invalid character in APN.");
            return VOS_ERR;
        }
    }

    /* 检查APN开始字符有效性 */
    ucApnCharacter = AT_UPCASE(pucApn[0]);
    if ( VOS_OK != AT_CheckApnFirstAndLastChar(ucApnCharacter) )
    {
        AT_WARN_LOG("AT_CheckApn: WARNING: Invalid begin/end character in APN.");
        return VOS_ERR;
    }

    /* 检查APN末尾字符有效性 */
    ucApnCharacter = AT_UPCASE(pucApn[usApnLen - 1]);
    if ( VOS_OK != AT_CheckApnFirstAndLastChar(ucApnCharacter) )
    {
        AT_WARN_LOG("AT_CheckApn: WARNING: Invalid begin/end character in APN.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_SetCgdcontPara(VOS_UINT8 ucIndex)
{
    TAF_PDP_PRIM_CONTEXT_EXT_STRU       stPdpCxtInfo;

    /* 初始化 */
    PS_MEM_SET(&stPdpCxtInfo, 0x00, sizeof(TAF_PDP_PRIM_CONTEXT_EXT_STRU));

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 10 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<CID> */
    stPdpCxtInfo.ucCid                  = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stPdpCxtInfo.ucDefined          = VOS_FALSE;
    }
    else
    {
        stPdpCxtInfo.ucDefined          = VOS_TRUE;

        /* 设置<PDP_type> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            if ( gastAtParaList[1].ulParaValue < TAF_PDP_PPP )
            {
                stPdpCxtInfo.bitOpPdpType   = VOS_TRUE;

                /* IP:1, IPV6:2, IPV4V6:3, PPP:4 */
                stPdpCxtInfo.enPdpType  = (VOS_UINT8)(gastAtParaList[1].ulParaValue + 1);

                if ( (TAF_PDP_IPV6   == stPdpCxtInfo.enPdpType)
                  || (TAF_PDP_IPV4V6 == stPdpCxtInfo.enPdpType) )
                {
#if (FEATURE_ON == FEATURE_IPV6)
                    if (AT_IPV6_CAPABILITY_IPV4_ONLY == AT_GetIpv6Capability())
                    {
                        return AT_CME_INCORRECT_PARAMETERS;
                    }
#else
                    return AT_CME_INCORRECT_PARAMETERS;
#endif
                }
            }
        }
        /* 对比终端的结果，在<PDP_type>参数为""(两个引号)时,则保留原先的值，
           若g_TafCidTab[ucCid].ucUsed指示上下文被删除，则使用之前保留下来的值 */

        /* 设置<APN> */
        if ( gastAtParaList[2].usParaLen > TAF_MAX_APN_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if ( 0 != gastAtParaList[2].usParaLen )
        {
            /* 检查 */
            if ( VOS_OK != AT_CheckApnFormat(gastAtParaList[2].aucPara,
                                             gastAtParaList[2].usParaLen) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stPdpCxtInfo.bitOpApn       = VOS_TRUE;
            PS_MEM_CPY((VOS_CHAR*)stPdpCxtInfo.aucApn,
                       (VOS_CHAR*)gastAtParaList[2].aucPara,
                       gastAtParaList[2].usParaLen);
            stPdpCxtInfo.aucApn[gastAtParaList[2].usParaLen] = 0;
        }

        /* 设置<PDP_addr> */
        /* 只处理IPV4的IP ADDR，IPV6不使用此AT命令设置的IP ADDR */
        if ( 0 != gastAtParaList[3].usParaLen )
        {
            if ( (TAF_PDP_IPV4 != (gastAtParaList[1].ulParaValue + 1))
              || (gastAtParaList[3].usParaLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1)) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stPdpCxtInfo.bitOpPdpAddr        = VOS_TRUE;
            stPdpCxtInfo.stPdpAddr.enPdpType = TAF_PDP_IPV4;

            /* 把IP地址字符串转换成数字 */
            if ( VOS_OK != AT_Ipv4AddrAtoi((VOS_CHAR*)gastAtParaList[3].aucPara,
                                           (VOS_UINT8*)stPdpCxtInfo.stPdpAddr.aucIpv4Addr) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }

        /* 设置<d_comp> */
        if ( 0 != gastAtParaList[4].usParaLen )
        {
            stPdpCxtInfo.bitOpPdpDcomp  = VOS_TRUE;
            stPdpCxtInfo.enPdpDcomp     = (VOS_UINT8)gastAtParaList[4].ulParaValue;
        }

        /* 设置<h_comp> */
        if ( 0 != gastAtParaList[5].usParaLen )
        {
            stPdpCxtInfo.bitOpPdpHcomp  = VOS_TRUE;
            stPdpCxtInfo.enPdpHcomp     = (VOS_UINT8)gastAtParaList[5].ulParaValue;
        }

        /* 设置<IPv4AddrAlloc> */
        if ( 0 != gastAtParaList[6].usParaLen )
        {
            stPdpCxtInfo.bitOpIpv4AddrAlloc = VOS_TRUE;
            stPdpCxtInfo.enIpv4AddrAlloc    = (VOS_UINT8)gastAtParaList[6].ulParaValue;
        }

        /* 设置<Emergency Indication> */
        if ( 0 != gastAtParaList[7].usParaLen )
        {
            stPdpCxtInfo.bitOpEmergencyInd = VOS_TRUE;
            stPdpCxtInfo.enEmergencyFlg    = (VOS_UINT8)gastAtParaList[7].ulParaValue;
        }
        /* 设置<P-CSCF_discovery> */
        if ( 0 != gastAtParaList[8].usParaLen )
        {
            stPdpCxtInfo.bitOpPcscfDiscovery = VOS_TRUE;
            stPdpCxtInfo.enPcscfDiscovery    = (VOS_UINT8)gastAtParaList[8].ulParaValue;
        }
        /* 设置<IM_CN_Signalling_Flag_Ind> */
        if ( 0 != gastAtParaList[9].usParaLen )
        {
            stPdpCxtInfo.bitOpImCnSignalFlg = VOS_TRUE;
            stPdpCxtInfo.enImCnSignalFlg    = (VOS_UINT8)gastAtParaList[9].ulParaValue;
        }
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetPrimPdpContextInfo(WUEPS_PID_AT,
                                                gastAtClientTab[ucIndex].usClientId,
                                                0,
                                                &stPdpCxtInfo) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGDCONT_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 AT_SetCgdscontPara(VOS_UINT8 ucIndex)
{
    TAF_PDP_SEC_CONTEXT_EXT_STRU        stPdpCxtInfo;

    /* 初始化 */
    PS_MEM_SET(&stPdpCxtInfo, 0x00, sizeof(TAF_PDP_SEC_CONTEXT_EXT_STRU));

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 4 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<s_cid> */
    stPdpCxtInfo.ucCid         = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stPdpCxtInfo.ucDefined          = VOS_FALSE;
    }
    else
    {
        stPdpCxtInfo.ucDefined          = VOS_TRUE;

        /* 设置<p_cid> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            stPdpCxtInfo.bitOpLinkdCid = VOS_TRUE;
            stPdpCxtInfo.ucLinkdCid    = (VOS_UINT8)gastAtParaList[1].ulParaValue;

            if (stPdpCxtInfo.ucCid == stPdpCxtInfo.ucLinkdCid)
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }
        else
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

#ifndef AT_OLNY_LTE_MODE
        /* 设置<d_comp> */
        if ( 0 != gastAtParaList[2].usParaLen )
        {
            stPdpCxtInfo.bitOpPdpDcomp  = VOS_TRUE;
            stPdpCxtInfo.enPdpDcomp     = (VOS_UINT8)gastAtParaList[2].ulParaValue;
        }

        /* 设置<h_comp> */
        if ( 0 != gastAtParaList[3].usParaLen )
        {
            stPdpCxtInfo.bitOpPdpHcomp  = VOS_TRUE;
            stPdpCxtInfo.enPdpHcomp     = (VOS_UINT8)gastAtParaList[3].ulParaValue;
        }
#endif
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetSecPdpContextInfo(WUEPS_PID_AT,
                                               gastAtClientTab[ucIndex].usClientId,
                                               0,
                                               &stPdpCxtInfo) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGDSCONT_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetCgeqreqPara(TAF_UINT8 ucIndex)
{
    TAF_UMTS_QOS_EXT_STRU               stUmtsQosInfo;

    /* 初始化 */
    PS_MEM_SET(&stUmtsQosInfo, 0x00, sizeof(TAF_UMTS_QOS_EXT_STRU));

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 13 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<cid> */
    stUmtsQosInfo.ucCid                     = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stUmtsQosInfo.ucDefined             = VOS_FALSE;
    }
    else
    {
        stUmtsQosInfo.ucDefined             = VOS_TRUE;

        /* 设置<Traffic class> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            stUmtsQosInfo.bitOpTrafficClass = VOS_TRUE;
            stUmtsQosInfo.ucTrafficClass    = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }

        /* 设置<Maximum bitrate UL> */
        if ( 0 != gastAtParaList[2].usParaLen )
        {
            stUmtsQosInfo.bitOpMaxBitUl     = VOS_TRUE;
            stUmtsQosInfo.ulMaxBitUl        = gastAtParaList[2].ulParaValue;
        }

        /* 设置<Maximum bitrate DL> */
        if ( 0 != gastAtParaList[3].usParaLen )
        {
            stUmtsQosInfo.bitOpMaxBitDl     = VOS_TRUE;
            stUmtsQosInfo.ulMaxBitDl        = gastAtParaList[3].ulParaValue;
        }

        /* 设置<Guaranteed bitrate UL> */
        if ( 0 != gastAtParaList[4].usParaLen )
        {
            stUmtsQosInfo.bitOpGtdBitUl     = VOS_TRUE;
            stUmtsQosInfo.ulGtdBitUl        = gastAtParaList[4].ulParaValue;
        }

        /* 设置<Guaranteed bitrate DL> */
        if ( 0 != gastAtParaList[5].usParaLen )
        {
            stUmtsQosInfo.bitOpGtdBitDl     = VOS_TRUE;
            stUmtsQosInfo.ulGtdBitDl        = gastAtParaList[5].ulParaValue;
        }

        /* 设置<Delivery order> */
        if ( 0 != gastAtParaList[6].usParaLen )
        {
            stUmtsQosInfo.bitOpDeliverOrder = VOS_TRUE;
            stUmtsQosInfo.ucDeliverOrder    = (VOS_UINT8)gastAtParaList[6].ulParaValue;
        }

        /* 设置<Maximum SDU size> */
        if ( 0 != gastAtParaList[7].usParaLen )
        {
            stUmtsQosInfo.bitOpMaxSduSize   = VOS_TRUE;
            stUmtsQosInfo.usMaxSduSize      = (VOS_UINT16)gastAtParaList[7].ulParaValue;
        }

        /* 设置<SDU error ratio> */
        if ( 0 != gastAtParaList[8].usParaLen )
        {
            stUmtsQosInfo.bitOpSduErrRatio  = VOS_TRUE;
            stUmtsQosInfo.ucSduErrRatio     = (VOS_UINT8)gastAtParaList[8].ulParaValue;
        }

        /* 设置<Residual bit error ratio> */
        if ( 0 != gastAtParaList[9].usParaLen )
        {
            stUmtsQosInfo.bitOpResidualBer  = VOS_TRUE;
            stUmtsQosInfo.ucResidualBer     = (VOS_UINT8)gastAtParaList[9].ulParaValue;
        }

        /* 设置<Delivery of erroneous SDUs> */
        if ( 0 != gastAtParaList[10].usParaLen )
        {
            stUmtsQosInfo.bitOpDeliverErrSdu = VOS_TRUE;
            stUmtsQosInfo.ucDeliverErrSdu    = (VOS_UINT8)gastAtParaList[10].ulParaValue;
        }

        /* 设置<Transfer delay> */
        if ( 0 != gastAtParaList[11].usParaLen )
        {
            stUmtsQosInfo.bitOpTransDelay   = VOS_TRUE;
            stUmtsQosInfo.usTransDelay      = (VOS_UINT16)gastAtParaList[11].ulParaValue;
        }

        /* 设置<Traffic handling priority> */
        if ( 0 != gastAtParaList[12].usParaLen )
        {
            stUmtsQosInfo.bitOpTraffHandlePrior = VOS_TRUE;
            stUmtsQosInfo.ucTraffHandlePrior    = (VOS_UINT8)gastAtParaList[12].ulParaValue;
        }
    }

    /* 执行命令操作 */
    if(AT_CMD_CGEQREQ == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
    {
        if ( VOS_OK != TAF_PS_SetUmtsQosInfo(WUEPS_PID_AT,
                                             gastAtClientTab[ucIndex].usClientId,
                                             0, &stUmtsQosInfo) )
        {
            return AT_ERROR;
        }

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt  = AT_CMD_CGEQREQ_SET;
    }
    else
    {
        if ( VOS_OK != TAF_PS_SetUmtsQosMinInfo(WUEPS_PID_AT,
                                                gastAtClientTab[ucIndex].usClientId,
                                                0, &stUmtsQosInfo) )
        {
            return AT_ERROR;
        }

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt  = AT_CMD_CGEQMIN_SET;

    }

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetCgeqminPara(TAF_UINT8 ucIndex)
{
    return AT_SetCgeqreqPara(ucIndex);
}


TAF_UINT32 AT_SetCgpaddrPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulCidIndex;
    TAF_CID_LIST_STRU                   stCidListInfo;
    VOS_UINT32                          ulIndex;

    /* 初始化 */
    ulCidIndex  = 0;
    PS_MEM_SET(&stCidListInfo, 0x00, sizeof(TAF_CID_LIST_STRU));

    /* 参数过多 */
    if ( gucAtParaIndex > TAF_MAX_CID )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_ON == FEATURE_LTE)
    ulIndex = AT_MAX_PARA_NUMBER;
#else
    ulIndex = TAF_MAX_CID;
#endif

    /* 参数设置 */
    if ( 0 != gucAtParaIndex )
    {
        for ( i = 0; i < ulIndex; i++ )
        {
            if ( 0 == gastAtParaList[i].usParaLen )
            {
                break;
            }
            else
            {
                ulCidIndex = gastAtParaList[i].ulParaValue;
            }

            /* 以CID作为下标, 标记对应的CID */
            stCidListInfo.aucCid[ulCidIndex] = 1;
        }
    }
    else
    {
        for ( i = 1; i <= TAF_MAX_CID; i++ )
        {
            stCidListInfo.aucCid[i] = 1;
        }
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_GetPdpIpAddrInfo(WUEPS_PID_AT,
                                           gastAtClientTab[ucIndex].usClientId,
                                           0, &stCidListInfo) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGPADDR_SET;

    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetCgeqnegPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulCidIndex;
    TAF_CID_LIST_STRU                   stCidListInfo;
    VOS_UINT32                          ulIndex;

    /* 初始化 */
    ulCidIndex  = 0;
    PS_MEM_SET(&stCidListInfo, 0x00, sizeof(TAF_CID_LIST_STRU));

    /* 参数过多 */
    if ( gucAtParaIndex > TAF_MAX_CID )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_ON == FEATURE_LTE)
    ulIndex = AT_MAX_PARA_NUMBER;
#else
    ulIndex = TAF_MAX_CID;
#endif

    /* 参数设置 */
    /* +CGEQNEG[=<cid>[,<cid>[,...]]] */
    if ( 0 != gucAtParaIndex )
    {
        for ( i = 0; i < ulIndex; i++ )
        {
            if ( 0 == gastAtParaList[i].usParaLen )
            {
                break;
            }
            else
            {
                ulCidIndex = gastAtParaList[i].ulParaValue;
            }

            /* 以CID作为下标, 标记对应的CID */
            stCidListInfo.aucCid[ulCidIndex] = 1;
        }
    }
    else
    {
        for ( i = 1; i <= TAF_MAX_CID; i++ )
        {
            stCidListInfo.aucCid[i] = 1;
        }
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_GetDynamicUmtsQosInfo(WUEPS_PID_AT,
                                                gastAtClientTab[ucIndex].usClientId,
                                                0,
                                                &stCidListInfo) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGEQNEG_SET;

    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetCgcmodPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulCidIndex;
    TAF_CID_LIST_STRU                   stCidListInfo;

    /* 初始化 */
    ulCidIndex  = 0;
    PS_MEM_SET(&stCidListInfo, 0x00, sizeof(TAF_CID_LIST_STRU));

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数设置 */
    if ( 0 != gucAtParaIndex )
    {
        for ( i = 0; i < AT_MAX_PARA_NUMBER; i++ )
        {
            if ( 0 == gastAtParaList[i].usParaLen )
            {
                break;
            }
            else
            {
                ulCidIndex = gastAtParaList[i].ulParaValue;
            }

            /* 以CID作为下标, 标记对应的CID */
            stCidListInfo.aucCid[ulCidIndex] = VOS_TRUE;
        }
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_CallModify(WUEPS_PID_AT,
                                     gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     &stCidListInfo) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCMOD_SET;

    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetCgactPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulCidIndex;
    TAF_CID_LIST_STATE_STRU             stCidListStateInfo;
    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx = VOS_NULL_PTR;

    /* 初始化 */
    ulCidIndex  = 0;
    PS_MEM_SET(&stCidListStateInfo, 0x00, sizeof(TAF_CID_LIST_STATE_STRU));

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex != 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(ucIndex);

    /* 参数设置 */
    stCidListStateInfo.ucState  = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    for ( i = 0; i < (AT_MAX_PARA_NUMBER - 1); i++ )
    {
        if ( 0 == gastAtParaList[i + 1UL].usParaLen )
        {
            break;
        }
        else
        {
            ulCidIndex = gastAtParaList[i + 1UL].ulParaValue;

            /* 若为AP-Modem形态，则需检查是否为HSIC AT通道下发的CGACT请求，
                且已经配置了CID与数传通道的映射关系，若否，则返回ERROR */
            if (VOS_TRUE == AT_CheckHsicUser(ucIndex))
            {
                if ((VOS_FALSE == pstPsModemCtx->astChannelCfg[ulCidIndex].ulUsed)
                  ||(UDI_INVAL_DEV_ID == (UDI_DEVICE_ID)pstPsModemCtx->astChannelCfg[ulCidIndex].ulRmNetId))
                {
                    return AT_ERROR;
                }
            }
        }

        /* 以CID作为下标, 标记对应的CID */
        stCidListStateInfo.aucCid[ulCidIndex] = 1;
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetPdpContextState(WUEPS_PID_AT,
                                             gastAtClientTab[ucIndex].usClientId,
                                             0,
                                             &stCidListStateInfo) )
    {
        return AT_ERROR;
    }

    if ( 1 == stCidListStateInfo.ucState )
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGACT_ORG_SET;
    }
    else
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGACT_END_SET;
    }

    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetCgansPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8                           ucRspType;
    TAF_PS_ANSWER_STRU                  stAnsInfo;

    /* 初始化 */
    PS_MEM_SET(&stAnsInfo, 0x00, sizeof(TAF_PS_ANSWER_STRU));

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 3 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数设置 */
    ucRspType           = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 设置<CID> */
    if ( 0 != gastAtParaList[2].usParaLen )
    {
        stAnsInfo.ucCid = (VOS_UINT8)gastAtParaList[2].ulParaValue;
    }

    /* 填充ans扩展标志 */
    if (AT_CMD_CGANS_EXT == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
    {
        stAnsInfo.ucAnsExtFlg = VOS_TRUE;
    }
    else
    {
        stAnsInfo.ucAnsExtFlg = VOS_FALSE;
    }

    /* 执行命令操作 */
    if ( 1 == ucRspType )
    {
        if ( VOS_OK == TAF_PS_CallAnswer(WUEPS_PID_AT,
                                         gastAtClientTab[ucIndex].usClientId,
                                         0,
                                         &stAnsInfo) )
        {
            /* 设置当前操作类型 */
            if (AT_CMD_CGANS_EXT == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
            {
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGANS_ANS_EXT_SET;
            }
            else
            {
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGANS_ANS_SET;
            }

            /* 返回命令处理挂起状态 */
            return AT_WAIT_ASYNC_RETURN;
        }
        else
        {
            return AT_ERROR;
        }
    }
    else
    {
        /* 0 --  拒绝网络发起的激活指示 */
        if ( VOS_OK == TAF_PS_CallHangup(WUEPS_PID_AT,
                                         gastAtClientTab[ucIndex].usClientId,
                                         0) )
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGANS_ANS_SET;

            /* 返回命令处理挂起状态 */
            return AT_WAIT_ASYNC_RETURN;
        }
        else
        {
            return AT_ERROR;
        }
    }
}


VOS_UINT32 AT_CountSpecCharInString(
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usLen,
    VOS_UINT8                           ucChar
)
{
    VOS_UINT8                          *pucRead;
    VOS_UINT32                          ulChkLen;
    VOS_UINT32                          ulCount;

    if(VOS_NULL_PTR == pucData)
    {
        return 0;
    }

    pucRead                             = pucData;
    ulChkLen                            = 0;
    ulCount                             = 0;

    /* 检查 */
    while( ulChkLen++ < usLen )
    {
        if(ucChar == *pucRead)
        {
            ulCount++;
        }
        pucRead++;
    }

    return ulCount;

}
VOS_UINT32 AT_FillTftIpv4Addr(
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usLen,
    TAF_TFT_EXT_STRU                   *pstTftInfo
)
{
    VOS_UINT32                          ulCount;
    VOS_UINT32                          ulRet;
    VOS_CHAR                           *ptrTmp;

    ulCount = 0;
    ptrTmp  = VOS_NULL_PTR;

    ulCount = AT_CountDigit(pucData, usLen, '.', 7);
    if ((0 == ulCount) || (0 == pucData[ulCount]))
    {
        return AT_FAILURE;
    }

    ulCount = AT_CountDigit(pucData, usLen, '.', 4);

    pstTftInfo->bitOpSrcIp = VOS_TRUE;
    pstTftInfo->stSourceIpaddr.enPdpType = TAF_PDP_IPV4;
    pstTftInfo->stSourceIpMask.enPdpType = TAF_PDP_IPV4;

    if ((usLen - ulCount) > TAF_MAX_IPV4_ADDR_STR_LEN)
    {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)&(pucData[ulCount]);

    ulRet = AT_Ipv4AddrAtoi(ptrTmp, pstTftInfo->stSourceIpMask.aucIpv4Addr);
    if(VOS_OK != ulRet)
    {
        return AT_FAILURE;
    }

    if ((ulCount - 1) > TAF_MAX_IPV4_ADDR_STR_LEN)
    {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)(pucData);
    ptrTmp[ulCount - 1] = '\0';

    ulRet = AT_Ipv4AddrAtoi(ptrTmp, pstTftInfo->stSourceIpaddr.aucIpv4Addr);
    if (VOS_OK != ulRet)
    {
        return AT_FAILURE;
    }

    return AT_SUCCESS;

}


VOS_UINT32 AT_FillTftIpv6Addr(
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usLen,
    TAF_TFT_EXT_STRU                   *pstTftInfo
)
{
    VOS_UINT32                          ulCount;
    VOS_UINT32                          ulRet;
    VOS_CHAR                           *ptrTmp;

    ulCount = 0;
    ptrTmp  = VOS_NULL_PTR;

    ulCount = AT_CountDigit(pucData, usLen, '.', 31);
    if ((0 == ulCount) || (0 == pucData[ulCount]))
    {
        return AT_FAILURE;
    }

    ulCount = AT_CountDigit(pucData, usLen, '.', 16);

    pstTftInfo->bitOpSrcIp = VOS_TRUE;
    pstTftInfo->stSourceIpaddr.enPdpType = TAF_PDP_IPV6;
    pstTftInfo->stSourceIpMask.enPdpType = TAF_PDP_IPV6;

    if ((usLen - ulCount) > TAF_MAX_IPV6_ADDR_DOT_STR_LEN)
    {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)&(pucData[ulCount]);

    ulRet = AT_Ipv6AddrAtoi(ptrTmp, pstTftInfo->stSourceIpMask.aucIpv6Addr);
    if(VOS_OK != ulRet)
    {
        return AT_FAILURE;
    }

    if ((ulCount - 1) > TAF_MAX_IPV6_ADDR_DOT_STR_LEN)
    {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)(pucData);
    ptrTmp[ulCount - 1] = '\0';

    ulRet = AT_Ipv6AddrAtoi(ptrTmp, pstTftInfo->stSourceIpaddr.aucIpv6Addr);
    if (VOS_OK != ulRet)
    {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_FillTftIpAddr(
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen,
    TAF_TFT_EXT_STRU                   *pstTftInfo
)
{
    VOS_UINT32                          ulCount;

    /* 获取输入的IP地址中包含的'.'的个数 */
    ulCount = AT_CountSpecCharInString(pData, usLen, '.');

    /* 如果输入为IPV4地址，地址加掩码会有7个'.' */
    /* 例如"10.140.23.144.255.255.255.0"，前4个是IPV4地址，后4个为掩码 */
    if ((TAF_IPV4_ADDR_LEN * 2 - 1) == ulCount)
    {
        return (AT_FillTftIpv4Addr(pData, usLen, pstTftInfo));
    }
    /* 如果输入为IPV6地址，地址加掩码会有31个'.' */
    /* 例如"32.8.0.2.0.2.0.1.0.2.0.1.0.3.21.111.255.255.255.255.255.255.255.255.255.255.255.255.255.255.255.255" */
    /* 前16个为IPV6地址，后16个为掩码 */
#if (FEATURE_ON == FEATURE_IPV6)
    else if ((AT_IPV6_CAPABILITY_IPV4_ONLY != AT_GetIpv6Capability())
           && ((TAF_IPV6_ADDR_LEN * 2 - 1) == ulCount))
    {
        return (AT_FillTftIpv6Addr(pData, usLen, pstTftInfo));
    }
#endif
    else
    {
        return AT_FAILURE;
    }
}


TAF_UINT32 AT_SetCgtftPara(TAF_UINT8 ucIndex)
{
    TAF_TFT_EXT_STRU                    stTftInfo;
    VOS_UINT32                          ulLow  = 0;
    VOS_UINT32                          ulHigh = 0;
    VOS_UINT32                          ulCount = 0;
    VOS_UINT32                          ulRet;

    /* 初始化 */
    PS_MEM_SET(&stTftInfo, 0x00, sizeof(TAF_TFT_EXT_STRU));

    /* 如果没有cid，则返回失败 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多,<flow label (ipv6)>不支持，<direction>也不再检查 */
    if ( gucAtParaIndex > 11 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<cid> */
    stTftInfo.ucCid = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stTftInfo.ucDefined             = VOS_FALSE;
    }
    else
    {
        stTftInfo.ucDefined             = VOS_TRUE;

        /* 设置<packet filter identifier> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            stTftInfo.bitOpPktFilterId  = 1;
            stTftInfo.ucPacketFilterId  = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }

        /* 设置<evaluation precedence index> */
        if ( 0 != gastAtParaList[2].usParaLen )
        {
            stTftInfo.bitOpPrecedence   = 1;
            stTftInfo.ucPrecedence      = (VOS_UINT8)gastAtParaList[2].ulParaValue;
        }

        /* 设置<source address and subnet mask> */
        if ( 0 != gastAtParaList[3].usParaLen )
        {
            ulRet = AT_FillTftIpAddr(gastAtParaList[3].aucPara,
                                     gastAtParaList[3].usParaLen,
                                     &stTftInfo);
            if (AT_FAILURE == ulRet)
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }

        /* 设置<protocol number (ipv4) / next header (ipv6)> */
        if(0 != gastAtParaList[4].usParaLen)
        {
            stTftInfo.bitOpProtocolId   = 1;
            stTftInfo.ucProtocolId      = (VOS_UINT8)gastAtParaList[4].ulParaValue;
        }

        /* 设置<destination port range> */
        if(0 != gastAtParaList[5].usParaLen)
        {
            ulCount = AT_CountDigit(gastAtParaList[5].aucPara,gastAtParaList[5].usParaLen,'.',1);
            if((0 == ulCount) || (0 == gastAtParaList[5].aucPara[ulCount]))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stTftInfo.bitOpDestPortRange= 1;

            if(AT_FAILURE == atAuc2ul(gastAtParaList[5].aucPara,(VOS_UINT16)(ulCount-1),&ulLow))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            if(AT_FAILURE == atAuc2ul(&gastAtParaList[5].aucPara[ulCount],(VOS_UINT16)(gastAtParaList[5].usParaLen - ulCount),&ulHigh))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            if((ulLow > 65535) || (ulHigh > 65535) || (ulLow > ulHigh))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            else
            {
                stTftInfo.usHighDestPort = (VOS_UINT16)ulHigh;
                stTftInfo.usLowDestPort = (VOS_UINT16)ulLow;
            }
        }

        /* 设置<source port range> */
        if(0 != gastAtParaList[6].usParaLen)
        {
            ulCount = AT_CountDigit(gastAtParaList[6].aucPara,gastAtParaList[6].usParaLen,'.',1);
            if((0 == ulCount) || (0 == gastAtParaList[6].aucPara[ulCount]))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stTftInfo.bitOpSrcPortRange = 1;

            if(AT_FAILURE == atAuc2ul(gastAtParaList[6].aucPara,(VOS_UINT16)(ulCount-1),&ulLow))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            if(AT_FAILURE == atAuc2ul(&gastAtParaList[6].aucPara[ulCount],(VOS_UINT16)(gastAtParaList[6].usParaLen - ulCount),&ulHigh))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            if((ulLow > 65535) || (ulHigh > 65535) || (ulLow > ulHigh))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            else
            {
                stTftInfo.usHighSourcePort = (VOS_UINT16)ulHigh;
                stTftInfo.usLowSourcePort = (VOS_UINT16)ulLow;
            }
        }

        /* 设置<ipsec security parameter index (spi)> */
        if(0 != gastAtParaList[7].usParaLen)
        {
            stTftInfo.bitOpSpi = 1;
            /* 需要增加判断大小 */
            if(gastAtParaList[7].usParaLen > 8) /*大于FFFFFFFF */
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            if(AT_FAILURE == At_String2Hex(gastAtParaList[7].aucPara,gastAtParaList[7].usParaLen,&gastAtParaList[7].ulParaValue))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            stTftInfo.ulSecuParaIndex = gastAtParaList[7].ulParaValue;
        }

        /* 设置<type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask> */
        if(0 != gastAtParaList[8].usParaLen)
        {
            ulCount = AT_CountDigit(gastAtParaList[8].aucPara,gastAtParaList[8].usParaLen,'.',1);
            if((0 == ulCount) || (0 == gastAtParaList[8].aucPara[ulCount]))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stTftInfo.bitOpTosMask = 1;

            if(AT_FAILURE == atAuc2ul(gastAtParaList[8].aucPara,(VOS_UINT16)(ulCount-1),&ulLow))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            if(AT_FAILURE == atAuc2ul(&gastAtParaList[8].aucPara[ulCount],(VOS_UINT16)(gastAtParaList[8].usParaLen - ulCount),&ulHigh))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            if((ulLow > 255) || (ulHigh > 255))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
            else
            {
                stTftInfo.ucTypeOfServiceMask = (VOS_UINT8)ulHigh;
                stTftInfo.ucTypeOfService = (VOS_UINT8)ulLow;
            }
        }

        /* <flow label (ipv6)> */
        if(0 != gastAtParaList[9].usParaLen)
        {
            stTftInfo.bitOpFlowLable = 1;
            stTftInfo.ulFlowLable = gastAtParaList[9].ulParaValue;
        }

        /* <direction> */
        if(0 != gastAtParaList[10].usParaLen)
        {
            stTftInfo.bitOpDirection = 1;
            stTftInfo.ucDirection = (VOS_UINT8)gastAtParaList[10].ulParaValue;
        }
    }

    if ( VOS_OK != TAF_PS_SetTftInfo(WUEPS_PID_AT,
                                     gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     &stTftInfo) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGTFT_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetCgauthPara(TAF_UINT8 ucIndex)

{
    TAF_PDP_AUTH_EXT_STRU               stPdpAuthInfo;

    /* 初始化 */
    PS_MEM_SET(&stPdpAuthInfo, 0x00, sizeof(TAF_PDP_AUTH_EXT_STRU));

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 4 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<cid> */
    stPdpAuthInfo.ucCid                 = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stPdpAuthInfo.ucDefined         = VOS_FALSE;
    }
    else
    {
        stPdpAuthInfo.ucDefined         = VOS_TRUE;

        /* 设置<Auth> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            stPdpAuthInfo.bitOpAuth     = VOS_TRUE;
            stPdpAuthInfo.ucAuth        = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }

        /* 设置<UserName> */
        if ( 0 != gastAtParaList[2].usParaLen )
        {
            /* 如果<UserName>过长，直接返回错误 */
            if ( gastAtParaList[2].usParaLen > TAF_MAX_PDP_AUTH_USERNAME_LEN )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stPdpAuthInfo.bitOpUserName = VOS_TRUE;
            PS_MEM_CPY((VOS_CHAR*)stPdpAuthInfo.aucUserName,
                       (VOS_CHAR*)gastAtParaList[2].aucPara,
                       gastAtParaList[2].usParaLen);
            stPdpAuthInfo.aucUserName[gastAtParaList[2].usParaLen] = 0;
        }

        /* 设置<PassWord> */
        if ( 0 != gastAtParaList[3].usParaLen )
        {
            /* 如果<PassWord>过长，直接返回错误 */
            if ( gastAtParaList[3].usParaLen > TAF_MAX_PDP_AUTH_PASSCODE_LEN )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stPdpAuthInfo.bitOpPassword = VOS_TRUE;
            PS_MEM_CPY((VOS_CHAR*)stPdpAuthInfo.aucPassWord,
                       (VOS_CHAR*)gastAtParaList[3].aucPara,
                       gastAtParaList[3].usParaLen);
            stPdpAuthInfo.aucPassWord[gastAtParaList[3].usParaLen] = 0;
        }
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetPdpAuthInfo(WUEPS_PID_AT,
                                         gastAtClientTab[ucIndex].usClientId,
                                         0, &stPdpAuthInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGAUTH_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetCgdnsPara(TAF_UINT8 ucIndex)
{
    TAF_PDP_DNS_EXT_STRU                stPdpDnsInfo;

    VOS_UINT8                           aucTempIpv4Addr[TAF_IPV4_ADDR_LEN];
    VOS_UINT32                          ulRslt;

    /* 初始化 */
    PS_MEM_SET(&stPdpDnsInfo, 0x00, sizeof(TAF_PDP_DNS_EXT_STRU));

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 3 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<cid> */
    stPdpDnsInfo.ucCid                  = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stPdpDnsInfo.ucDefined          = VOS_FALSE;
    }
    else
    {
        stPdpDnsInfo.ucDefined          = VOS_TRUE;

        /* 设置<PriDns> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            /* 如果<PriDns>过长，直接返回错误 */
            if ( gastAtParaList[1].usParaLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }


            ulRslt = AT_Ipv4AddrAtoi((VOS_CHAR*)gastAtParaList[1].aucPara, &aucTempIpv4Addr[0]);

            if ( VOS_OK != ulRslt )
            {
               return AT_CME_INCORRECT_PARAMETERS;
            }


            stPdpDnsInfo.bitOpPrimDnsAddr = VOS_TRUE;
            PS_MEM_CPY(stPdpDnsInfo.aucPrimDnsAddr,
                       aucTempIpv4Addr,
                       TAF_IPV4_ADDR_LEN);
        }

        /* 设置<SecDns> */
        if ( 0 != gastAtParaList[2].usParaLen )
        {
            /* 如果<SecDns>过长，直接返回错误 */
            if ( gastAtParaList[2].usParaLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }


            /*此处存储的是字符串格式的DNS所以转换出来结果不使用*/
            ulRslt = AT_Ipv4AddrAtoi((VOS_CHAR*)gastAtParaList[2].aucPara, &aucTempIpv4Addr[0]);

            if ( VOS_OK != ulRslt )
            {
               return AT_CME_INCORRECT_PARAMETERS;
            }


            stPdpDnsInfo.bitOpSecDnsAddr = VOS_TRUE;
            PS_MEM_CPY(stPdpDnsInfo.aucSecDnsAddr,
                       aucTempIpv4Addr,
                       TAF_IPV4_ADDR_LEN);
        }
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetPdpDnsInfo(WUEPS_PID_AT,
                                        gastAtClientTab[ucIndex].usClientId,
                                        0, &stPdpDnsInfo) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGDNS_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetCgautoPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulAnsMode;

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<n> */
    ulAnsMode = gastAtParaList[0].ulParaValue;

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetAnsModeInfo(WUEPS_PID_AT,
                                         gastAtClientTab[ucIndex].usClientId,
                                         0,
                                         ulAnsMode) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGAUTO_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 atSetCgeqosPara(VOS_UINT8 ucIndex)
{
    TAF_EPS_QOS_EXT_STRU                stEpsQosInfo;

    PS_MEM_SET(&stEpsQosInfo, 0x00, sizeof(TAF_EPS_QOS_EXT_STRU));

    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( gucAtParaIndex > 6 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stEpsQosInfo.ucCid = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    if ( 0 != gastAtParaList[1].usParaLen )
    {
        stEpsQosInfo.bitOpQCI   = 1;
        stEpsQosInfo.ucQCI      = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    }

    if ( 0 != gastAtParaList[2].usParaLen )
    {
        stEpsQosInfo.bitOpDLGBR = 1;
        stEpsQosInfo.ulDLGBR    = gastAtParaList[2].ulParaValue;
    }

    if (0 != gastAtParaList[3].usParaLen )
    {
        stEpsQosInfo.bitOpULGBR = 1;
        stEpsQosInfo.ulULGBR    = gastAtParaList[3].ulParaValue;
    }

    if (0 != gastAtParaList[4].usParaLen )
    {
        stEpsQosInfo.bitOpDLMBR = 1;
        stEpsQosInfo.ulDLMBR= gastAtParaList[4].ulParaValue;
    }

    if (0 != gastAtParaList[5].usParaLen )
    {
        stEpsQosInfo.bitOpULMBR = 1;
        stEpsQosInfo.ulULMBR    = gastAtParaList[5].ulParaValue;
    }

    if ( VOS_OK != TAF_PS_SetEpsQosInfo(WUEPS_PID_AT,
                                        gastAtClientTab[ucIndex].usClientId,
                                        0,
                                        &stEpsQosInfo) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGEQOS_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 atQryCgeqosPara(VOS_UINT8 ucIndex)
{
    if( VOS_OK != TAF_PS_GetEpsQosInfo(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId,0))
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGEQOS_READ;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 atSetCgeqosrdpPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                                   ucCid = 0;

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucCid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucCid = 0xff;
    }

    if ( VOS_OK != TAF_PS_GetDynamicEpsQosInfo(WUEPS_PID_AT,
                                               gastAtClientTab[ucIndex].usClientId,
                                               0,
                                               ucCid) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGEQOSRDP_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 atSetCgcontrdpPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8   ucCid = 0;

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucCid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucCid = 0xff;
    }

    if( VOS_OK != TAF_PS_GetDynamicPrimPdpContextInfo(WUEPS_PID_AT,
                                                      gastAtClientTab[ucIndex].usClientId,
                                                      0, ucCid) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCONTRDP_SET;

    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */

}


VOS_UINT32 atSetCgscontrdpPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8   ucCid = 0;

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucCid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucCid = 0xff;
    }

    if( VOS_OK != TAF_PS_GetDynamicSecPdpContextInfo(WUEPS_PID_AT,
                                                     gastAtClientTab[ucIndex].usClientId,
                                                     0,
                                                     ucCid) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGSCONTRDP_SET;

    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */

}


VOS_UINT32 atSetCgtftrdpPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8   ucCid = 0;

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucCid = (VOS_UINT8)(gastAtParaList[0].ulParaValue);
    }
    else
    {
        ucCid = 0xff;
    }

    if( VOS_OK != TAF_PS_GetDynamicTftInfo(WUEPS_PID_AT,
                                           gastAtClientTab[ucIndex].usClientId,
                                           0,
                                           ucCid) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGTFTRDP_SET;

    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
}


VOS_UINT32 At_SetCsndPara(VOS_UINT8 ucIndex)
{
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    VOS_UINT8                          *pucData = VOS_NULL_PTR;
    VOS_UINT8                           ucRabId;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulSrcAddr;
    VOS_UINT32                          ulDestAddr;
    VOS_UINT16                          usDestPort;
    VOS_UINT16                          usSrcPort;
    VOS_UINT16                          usTotalLen;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulIpAddr;

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* RABID有效性检查[5,15] */
    ucRabId     = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    if (!AT_PS_IS_RABID_VALID(ucRabId))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 计算UDP报文长度 */
    usLength    = (VOS_UINT16)gastAtParaList[1].ulParaValue;
    usLength    = (usLength > 1400) ? (1400) : usLength;
    usTotalLen  = usLength + AT_IP_HDR_LEN + AT_UDP_HDR_LEN;

    /* 申请跨核零拷贝内存 */
    pstImmZc    = IMM_ZcStaticAlloc(usTotalLen);
    if (VOS_NULL_PTR == pstImmZc)
    {
        AT_ERR_LOG("At_SetCsndPara: IMM_ZcStaticAlloc fail!");
        return AT_ERROR;
    }

    ulIpAddr    = AT_PS_GetIpAddrByRabId(ucIndex, ucRabId);
    ulSrcAddr   = ulIpAddr;
    ulDestAddr  = ulIpAddr + 1;
    usDestPort  = 9600;
    usSrcPort   = 9700;

    pucData     = (VOS_UINT8 *)IMM_ZcPut(pstImmZc, usTotalLen);
    if (VOS_NULL_PTR == pucData)
    {
        IMM_ZcFree(pstImmZc);

        AT_ERR_LOG("At_SetCsndPara: IMM_ZcPut fail!");

        return AT_ERROR;
    }

    /* 构造UDP头 */
    if (VOS_OK != AT_BuildUdpHdr((AT_UDP_PACKET_FORMAT_STRU *)pucData,
                                 usLength,
                                 ulSrcAddr,
                                 ulDestAddr,
                                 usSrcPort,
                                 usDestPort))
    {
        IMM_ZcFree(pstImmZc);

        AT_ERR_LOG("At_SetCsndPara: IMM_ZcPut fail!");

        return AT_ERROR;
    }

    enModemId       = MODEM_ID_0;
    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        IMM_ZcFree(pstImmZc);
        AT_ERR_LOG("At_SetCsndPara: Get modem id fail.");
        return AT_ERROR;
    }

    if (MODEM_ID_0 != enModemId)
    {
        ucRabId    |= AT_PS_RABID_MODEM_1_MASK;
    }

    /* 发送上行数据包 */
    if (VOS_OK != ADS_UL_SendPacket(pstImmZc, ucRabId))
    {
        IMM_ZcFree(pstImmZc);

        return AT_ERROR;
    }

    return AT_OK;
}
TAF_UINT32 At_SetCgdataPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulCidIndex;
    TAF_CID_LIST_STATE_STRU             stCidListStateInfo;

    /* 初始化 */
    ulCidIndex  = 0;
    PS_MEM_SET(&stCidListStateInfo, 0x00, sizeof(TAF_CID_LIST_STATE_STRU));

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多, 目前最多支持一个CID */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <L2P>不处理 */

    /* 参数设置 */
    stCidListStateInfo.ucState  = 1;

    if (0 == gastAtParaList[1].usParaLen)
    {
        ulCidIndex = 1;
    }
    else
    {
        ulCidIndex = gastAtParaList[1].ulParaValue;
    }

    /* 以CID作为下标, 标记对应的CID */
    stCidListStateInfo.aucCid[ulCidIndex] = 1;


    /* 执行命令操作 */
    if (VOS_OK != TAF_PS_SetPdpContextState(WUEPS_PID_AT,
                                            gastAtClientTab[ucIndex].usClientId,
                                            0,
                                            &stCidListStateInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGDATA_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;

}


TAF_UINT32 At_SetCgattPara(TAF_UINT8 ucIndex)
{
    TAF_MMA_DETACH_PARA_STRU           stDetachPara;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(0 == gastAtParaList[0].ulParaValue)
    {
        /* 执行Detach操作 */
        PS_MEM_SET(&stDetachPara, 0, sizeof(TAF_MMA_DETACH_PARA_STRU));
        stDetachPara.enDetachCause  = TAF_MMA_DETACH_CAUSE_NORMAL;
        stDetachPara.ucDetachDomain = TAF_PH_SERVICE_PS;

        if (VOS_TRUE == TAF_MMA_DetachReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stDetachPara))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGATT_DETAACH_SET;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }
    }
    else
    {
        /* 执行Attach操作 */
        if(AT_SUCCESS == Taf_PhoneAttach(gastAtClientTab[ucIndex].usClientId, 0,TAF_PH_PS_OPERATE))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGATT_ATTACH_SET;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }
    }
}


TAF_UINT32 At_SetCgcattPara(TAF_UINT8 ucIndex)
{
    TAF_MMA_DETACH_PARA_STRU           stDetachPara;

    /* 参数检查 */
    if((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(0 == gastAtParaList[0].ulParaValue)
    {
        /* 执行Detach操作 */
        PS_MEM_SET(&stDetachPara, 0, sizeof(TAF_MMA_DETACH_PARA_STRU));
        stDetachPara.enDetachCause  = TAF_MMA_DETACH_CAUSE_NORMAL;

        /* 转换用户的DETACH类型 */
        At_ConvertDetachTypeToServiceDomain((TAF_UINT8)gastAtParaList[1].ulParaValue,
                                            &stDetachPara.ucDetachDomain);

        if(VOS_TRUE == TAF_MMA_DetachReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stDetachPara))
        {
            /* 设置当前操作类型 */
            switch(gastAtParaList[1].ulParaValue)
            {
            case TAF_PH_PS_OPERATE:
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCATT_PS_DETAACH_SET;
                break;

            case TAF_PH_CS_OPERATE:
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCATT_CS_DETAACH_SET;
                break;

            default:
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCATT_PS_CS_DETAACH_SET;
                break;
            }
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }
    }
    else
    {
        /* 执行Attach操作 */
        if(AT_SUCCESS == Taf_PhoneAttach(gastAtClientTab[ucIndex].usClientId, 0,(TAF_UINT8)gastAtParaList[1].ulParaValue))
        {
            /* 设置当前操作类型 */
            switch(gastAtParaList[1].ulParaValue)
            {
            case TAF_PH_PS_OPERATE:
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCATT_PS_ATTACH_SET;
                break;

            case TAF_PH_CS_OPERATE:
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCATT_CS_ATTACH_SET;
                break;

            default:
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGCATT_PS_CS_ATTACH_SET;
                break;
            }
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }
    }
}

VOS_VOID At_SetCopsActPara(
    TAF_PH_RAT_TYPE_ENUM_UINT8         *penPhRat,
    VOS_UINT16                          usParaLen,
    VOS_UINT32                          ulParaValue
)
{
    /* 如果参数长度为0，使用无效值 */
    if (0 == usParaLen)
    {
        *penPhRat = TAF_PH_RAT_BUTT;

        return;
    }

    /* 根据用户输入的参数值,转换为AT与MMA的接口定义的接入技术参数 */
    if (AT_COPS_RAT_GSM == ulParaValue)
    {
        *penPhRat = TAF_PH_RAT_GSM;
    }
    else if (AT_COPS_RAT_WCDMA == ulParaValue)
    {
        *penPhRat = TAF_PH_RAT_WCDMA;
    }
#if(FEATURE_ON == FEATURE_LTE)
    else if (AT_COPS_RAT_LTE == ulParaValue)
    {
        *penPhRat = TAF_PH_RAT_LTE;
    }
#endif
    else
    {
        *penPhRat = TAF_PH_RAT_BUTT;
    }

    return;
}

VOS_UINT32 At_SetCopsPara(TAF_UINT8 ucIndex)
{
    TAF_PH_RAT_TYPE_ENUM_UINT8          enPhRat;
    VOS_UINT32                          ulRslt;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;


    TAF_PLMN_USER_SEL_STRU          stPlmnUserSel;
    VOS_UINT32                      ulGetOperNameRst;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 4)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        gastAtParaList[0].ulParaValue = 0;
    }

    /* 设置<mode> A32D07158
     * only in case of mode = 3(change the format), should the new format be saved.
     */
    /* 设置<format> */
    if(0 != gastAtParaList[1].usParaLen)
    {
        /* 在每次进行搜索时，不论是成功还是失败，PLMN要根据用户指定的最新的格式显示要求进行显示 */
        pstNetCtx->ucCopsFormatType = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    }

    /* 设置<AcT> */
    At_SetCopsActPara(&enPhRat, gastAtParaList[3].usParaLen, gastAtParaList[3].ulParaValue);

    /* 在每次进行手动搜索时，不论是成功还是失败，PLMN要根据用户指定的最新的格式显示要求进行显示 */
    switch(gastAtParaList[0].ulParaValue)
    {
    case 0: /* automatic */
        if(AT_SUCCESS == Taf_PhonePlmnResel(gastAtClientTab[ucIndex].usClientId, 0,TAF_PLMN_RESELETION_AUTO))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_COPS_SET_AUTOMATIC;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }

    case 1: /* manual */


        /* 参数为空 */
        if (0 == gastAtParaList[2].usParaLen)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        stPlmnUserSel.ucFormatType = pstNetCtx->ucCopsFormatType;
        stPlmnUserSel.ucAccessMode = enPhRat;

        /* 根据输入PLMN的类型来保存到TAF_PLMN_USER_SEL_STRU结构中，发送到MMA模块，有MMA进行转换并处理指定搜。 */
        ulGetOperNameRst = AT_GetOperatorNameFromParam(&stPlmnUserSel.usOperNameLen,
                                                        stPlmnUserSel.ucOperName,
                                                        sizeof(stPlmnUserSel.ucOperName),
                                                        stPlmnUserSel.ucFormatType);

        if (AT_OK != ulGetOperNameRst )
        {
            return ulGetOperNameRst;
        }

        ulRslt = TAF_PhonePlmnUserSel(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      &stPlmnUserSel);

        if (VOS_TRUE == ulRslt)
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_COPS_SET_MANUAL;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }

    case 3: /* set only <format> */
        return AT_OK;
    default: /* manual/automatic */
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetCgclassPara(TAF_UINT8 ucIndex)
{
    TAF_PH_MS_CLASS_TYPE MsClass;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<class> */
    switch(gastAtParaList[0].ulParaValue)
    {
    case 0:
        MsClass = TAF_PH_MS_CLASS_A;
        break;

    case 1:
        MsClass = TAF_PH_MS_CLASS_B;
        break;

    case 2:
        MsClass = TAF_PH_MS_CLASS_CG;
        break;

    default:
        MsClass = TAF_PH_MS_CLASS_CC;
        break;
    }

    if (TAF_PH_MS_CLASS_A == MsClass)
    {
        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }

}

TAF_UINT32 At_SetCfunPara(TAF_UINT8 ucIndex)
{
    TAF_MMA_PHONE_MODE_PARA_STRU       stPhoneModePara;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    if ((2 == gucAtParaIndex) && (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    /* 设置<fun> */
    if ( TAF_PH_MODE_NUM_MAX <= gastAtParaList[0].ulParaValue )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

/* 输出TAF的回放数据 */
#if (VOS_WIN32 == VOS_OS_VER)
#ifndef __PS_WIN32_RECUR__
    /* NAS_MMA_SndNVData(); */

    NAS_MSG_SndOutsideContextData_Part1();
    NAS_MSG_SndOutsideContextData_Part2();
    NAS_MSG_SndOutsideContextData_Part3();
#endif
#endif
/* 输出TAF的回放数据 */

    PS_MEM_SET(&stPhoneModePara, 0x00, sizeof(TAF_MMA_PHONE_MODE_PARA_STRU));
    stPhoneModePara.PhMode  = (TAF_UINT8)gastAtParaList[0].ulParaValue;;

    if (VOS_TRUE == TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPhoneModePara))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CFUN_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
/*****************************************************************************
 Prototype      : At_SetCgmiPara
 Description    : +CGMI
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCgmiPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_MFR_ID_PARA,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGMI_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
/*****************************************************************************
 Prototype      : At_SetCgmmPara
 Description    : +CGMM/+GMM
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCgmmPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_MODEL_ID_PARA,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGMM_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
/*****************************************************************************
 Prototype      : At_SetCgmrPara
 Description    : +CGMR
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCgmrPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_REVISION_ID_PARA,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGMR_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetCgsnPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    /* AT命令状态检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送消息 ID_AT_MTA_CGSN_QRY_REQ 给 MTA 处理， */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   ID_AT_MTA_CGSN_QRY_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_UEPS_PID_MTA);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGSN_READ;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetCimiPara(TAF_UINT8 ucIndex)
{
    TAF_AT_NVIM_CIMI_PORT_CFG_STRU      stCimiPortCfg;

    stCimiPortCfg.ulCimiPortCfg = 0;

    /* 根据NV项配置参数，对命令下发的通道进行判断 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_CIMI_PORT_CFG,
                         &stCimiPortCfg,
                         sizeof(stCimiPortCfg)))
    {
        stCimiPortCfg.ulCimiPortCfg  = 0;
    }

    /* 该通道对应的Bit位不为0，则直接返回AT_ERROR */
    if (0 != (stCimiPortCfg.ulCimiPortCfg & (1 << ucIndex)))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_IMSI_ID_PARA,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CIMI_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
/*****************************************************************************
 Prototype      : At_SetClipPara
 Description    : +CSQ
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCsqPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_RSSI_VALUE_PARA,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSQ_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

VOS_UINT32 At_SetCbcPara(VOS_UINT8 ucIndex)
{
    if (BSP_MODULE_SUPPORT == DRV_GET_BATTERY_SUPPORT() )
    {

        /* E5 直接调用驱动的接口实现 */
        VOS_UINT8                           ucBcs;
        VOS_UINT8                           ucBcl;

        /* 参数检查 */
        if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (VOS_OK != DRV_CHG_GET_CBC_STATE(&ucBcs, &ucBcl))
        {
            return AT_ERROR;
        }

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        ucBcs, ucBcl);
        return AT_OK;

    }
    else
    {
        /* 参数检查 */
        if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 执行命令操作 */
        if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_BATTERY_POWER_PARA,TAF_NULL_PTR))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CBC_SET;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }

    }

}


VOS_UINT32 At_SetCpdwPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                           ulRst;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 通过给C CPU发TAF_MSG_MMA_MT_POWER_DOWN消息通知MT下电 */
    ulRst = AT_FillAndSndAppReqMsg( gastAtClientTab[ucIndex].usClientId,
                                  At_GetOpId(),
                                  TAF_MSG_MMA_MT_POWER_DOWN,
                                  VOS_NULL_PTR,
                                  0,
                                  I0_WUEPS_PID_MMA);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MMA_MT_POWER_DOWN;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;

}

TAF_UINT32 At_SetSnPara(TAF_UINT8 ucIndex)
{
    TAF_PH_SERIAL_NUM_STRU stSerialNum;
    TAF_UINT16            usLength = 0;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD == g_stATParseCmd.ucCmdOptType)
    {
        return At_WriteSnPara(ucIndex);
    }
    /* 从NV中读取 Serial Num,上报显示，返回 */

    PS_MEM_SET(&stSerialNum, 0, sizeof(TAF_PH_SERIAL_NUM_STRU));

    usLength = TAF_SERIAL_NUM_NV_LEN;
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_Serial_Num, stSerialNum.aucSerialNum, usLength))
    {
        AT_WARN_LOG("At_SetSnPara:WARNING:NVIM Read en_NV_Item_Serial_Num falied!");
        return AT_ERROR;
    }
    else
    {
        PS_MEM_SET((stSerialNum.aucSerialNum+TAF_SERIAL_NUM_LEN), 0, (VOS_SIZE_T)(4*sizeof(stSerialNum.aucSerialNum[0])));
        usLength = 0;
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stSerialNum.aucSerialNum);
    }
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 At_WriteSnPara(VOS_UINT8 ucIndex)
{
    TAF_PH_SERIAL_NUM_STRU              stSerialNum;

    /* 参数个数不为1 */
    if (gucAtParaIndex != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 如果参数长度不等于16，直接返回错误 */
    if (TAF_SERIAL_NUM_LEN != gastAtParaList[0].usParaLen)
    {
        return AT_SN_LENGTH_ERROR;
    }

    /* 检查当前参数是否为数字字母字符串,不是则直接返回错误 */
    if (AT_FAILURE == At_CheckNumCharString(gastAtParaList[0].aucPara,
                                            gastAtParaList[0].usParaLen))
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 拷贝设置的16位SN参数到结构体变量stSerialNum.aucSerialNum中 */
    PS_MEM_CPY(stSerialNum.aucSerialNum, gastAtParaList[0].aucPara, TAF_SERIAL_NUM_LEN);

    /* 将stSerialNum.aucSerialNum后四位填充为0xFF */
    PS_MEM_SET((stSerialNum.aucSerialNum + TAF_SERIAL_NUM_LEN), 0xFF, (TAF_SERIAL_NUM_NV_LEN - TAF_SERIAL_NUM_LEN));

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Serial_Num,
                          stSerialNum.aucSerialNum,
                          TAF_SERIAL_NUM_NV_LEN))
    {
        AT_WARN_LOG("At_WriteSnPara:WARNING:NVIM Write en_NV_Item_Serial_Num failed!");
        return AT_DEVICE_OTHER_ERROR;
    }

    return AT_OK;
}
VOS_UINT32 At_SetHwVerPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送消息DRV_AGENT_HARDWARE_QRY给AT AGENT处理，该消息无参数结构 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_HARDWARE_QRY,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DRV_AGENT_HARDWARE_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 At_SetFHVerPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送消息DRV_AGENT_FULL_HARDWARE_QRY给AT AGENT处理，该消息无参数结构 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   DRV_AGENT_FULL_HARDWARE_QRY,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DRV_AGENT_FULL_HARDWARE_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;


}


VOS_UINT32 At_SetPfverPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;

    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 发消息到C核获取充电状态和电池电量 */
    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_PFVER_QRY_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("At_SetPfverPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PFVER_READ;
    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 At_SetCLACPara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    gstAtSendData.usBufLen = 0;

    /* 依次输出支持的可显示的AT命令 */
    AT_ClacCmdProc();

    return AT_OK;
}


TAF_UINT32 At_SetCpbsPara(TAF_UINT8 ucIndex)
{
    SI_PB_STORATE_TYPE  Storage = SI_PB_STORAGE_UNSPECIFIED;
    TAF_UINT32          i;

    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* VOS_StrCmp需要做指针非空检查，切记! */
    if(TAF_NULL_PTR == (TAF_CHAR*)gastAtStringTab[AT_STRING_SM].pucText)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(TAF_NULL_PTR == (TAF_CHAR*)gastAtStringTab[AT_STRING_ON].pucText)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(TAF_NULL_PTR == (TAF_CHAR*)gastAtStringTab[AT_STRING_EN].pucText)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(TAF_NULL_PTR == (TAF_CHAR*)gastAtStringTab[AT_STRING_FD].pucText)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(TAF_NULL_PTR == (TAF_CHAR*)gastAtStringTab[AT_STRING_BD].pucText)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    for(i = 0; i < gastAtParaList[0].usParaLen ; i++)
    {
        gastAtParaList[0].aucPara[i] = At_UpChar(gastAtParaList[0].aucPara[i]);
    }

    if(0 == VOS_StrCmp((TAF_CHAR*)gastAtStringTab[AT_STRING_ON].pucText,(TAF_CHAR*)gastAtParaList[0].aucPara))
    {
        Storage = SI_PB_STORAGE_ON;
    }
    else if(0 == VOS_StrCmp((TAF_CHAR*)gastAtStringTab[AT_STRING_SM].pucText,(TAF_CHAR*)gastAtParaList[0].aucPara))
    {
        Storage = SI_PB_STORAGE_SM;
    }
    else if(0 == VOS_StrCmp((TAF_CHAR*)gastAtStringTab[AT_STRING_FD].pucText,(TAF_CHAR*)gastAtParaList[0].aucPara))
    {
        Storage = SI_PB_STORAGE_FD;
    }
    else if(0 == VOS_StrCmp((TAF_CHAR*)gastAtStringTab[AT_STRING_EN].pucText,(TAF_CHAR*)gastAtParaList[0].aucPara))
    {
        Storage = SI_PB_STORAGE_EC;
    }
    else if(0 == VOS_StrCmp((TAF_CHAR*)gastAtStringTab[AT_STRING_BD].pucText,(TAF_CHAR*)gastAtParaList[0].aucPara))
    {
        Storage = SI_PB_STORAGE_BD;
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PB_Set(gastAtClientTab[ucIndex].usClientId, 0, Storage))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBS_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetCpbr2Para(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 == gastAtParaList[1].usParaLen)
    {
        gastAtParaList[1].ulParaValue = gastAtParaList[0].ulParaValue;
    }

    if (AT_SUCCESS == SI_PB_Read(gastAtClientTab[ucIndex].usClientId,
                                0, SI_PB_STORAGE_UNSPECIFIED,
                                (TAF_UINT16)gastAtParaList[0].ulParaValue,
                                (TAF_UINT16)gastAtParaList[1].ulParaValue))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBR2_SET;

        gulPBPrintTag = TAF_FALSE;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetCpbrPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 == gastAtParaList[1].usParaLen)
    {
        gastAtParaList[1].ulParaValue = gastAtParaList[0].ulParaValue;
    }

    if (AT_SUCCESS == SI_PB_Read(gastAtClientTab[ucIndex].usClientId,
                                0, SI_PB_STORAGE_UNSPECIFIED,
                                (TAF_UINT16)gastAtParaList[0].ulParaValue,
                                (TAF_UINT16)gastAtParaList[1].ulParaValue))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBR_SET;

        gulPBPrintTag = TAF_FALSE;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_PbDeleteProc(TAF_UINT8 ucIndex)
{
    /* 针对AT+CPBW=,和AT+CPBW=0的情况 */
    if(0 == gastAtParaList[0].usParaLen || 0 == gastAtParaList[0].ulParaValue)
    {
        return AT_CME_INVALID_INDEX;
    }

    if(AT_SUCCESS == SI_PB_Delete(gastAtClientTab[ucIndex].usClientId,
                                  0, SI_PB_STORAGE_UNSPECIFIED,
                                  (TAF_UINT16)gastAtParaList[0].ulParaValue))

    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    AT_ERR_LOG("At_PbDeleteProc error: delete pb record failed");

    return AT_ERROR;
}
TAF_UINT32 At_SetCpbwPara(TAF_UINT8 ucIndex)
{
    SI_PB_RECORD_STRU   stRecords;
    TAF_UINT8           *pucNumber;
    TAF_UINT16          usLen;
    TAF_UINT16          usAlphaTagLength;
    TAF_UINT32          ulResult;
    TAF_UINT32          ulResultTemp;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)    /* 参数检查 */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gucAtParaIndex > 5)/* 参数过多 */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 只有一个参数时为删除操作 */
    if (1 == gucAtParaIndex)
    {
        ulResult = At_PbDeleteProc(ucIndex);

        return ulResult;
    }

    if((0 == gastAtParaList[1].usParaLen)&&(0 == gastAtParaList[3].usParaLen))
    {
        return AT_ERROR;
    }

    /* 确认编码类型，UNICODE或是ASCII码 */
    PS_MEM_SET(&stRecords, 0x00, sizeof(stRecords));/* 初始化 */

    if((AT_CSCS_UCS2_CODE == gastAtParaList[4].ulParaValue) && (5 == gucAtParaIndex))
    {
        stRecords.AlphaTagType = AT_CSCS_UCS2_CODE;
    }
    else if((AT_CSCS_IRA_CODE == gastAtParaList[4].ulParaValue) && (5 == gucAtParaIndex))
    {
        stRecords.AlphaTagType = AT_CSCS_IRA_CODE;
    }
    else if(4 == gucAtParaIndex)
    {
        stRecords.AlphaTagType = AT_CSCS_IRA_CODE;
    }
    else
    {
        stRecords.AlphaTagType = AT_CSCS_IRA_CODE;
    }

    if(0 != gastAtParaList[0].usParaLen)/* 设置<index> */
    {
        stRecords.Index = (TAF_UINT16)gastAtParaList[0].ulParaValue;
    }

    if(0 != gastAtParaList[1].usParaLen)/* 设置<number> */
    {
        if('+' == gastAtParaList[1].aucPara[0])
        {
            pucNumber = gastAtParaList[1].aucPara   + 1;
            usLen     = gastAtParaList[1].usParaLen - 1;
        }
        else
        {
            pucNumber = gastAtParaList[1].aucPara;
            usLen     = gastAtParaList[1].usParaLen;
        }

        if(AT_FAILURE == At_CheckPBString(pucNumber,&usLen))
        {
            return AT_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
        }

        if(SI_PB_PHONENUM_MAX_LEN < usLen)
        {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }

        PS_MEM_CPY(stRecords.Number, pucNumber, usLen);

        stRecords.NumberLength = (TAF_UINT8)usLen;
    }

    if(0 != gastAtParaList[2].usParaLen)/* 设置<type> */
    {
        /* 号码类型待修改 */
        stRecords.NumberType = (PB_NUMBER_TYPE_INTERNATIONAL == At_GetCodeType(gastAtParaList[1].aucPara[0]))
                              ? PB_NUMBER_TYPE_INTERNATIONAL : (TAF_UINT8)gastAtParaList[2].ulParaValue;
    }
    else
    {
        stRecords.NumberType = (TAF_UINT8)At_GetCodeType(gastAtParaList[1].aucPara[0]);
    }

    if(stRecords.NumberType < 128)
    {
        return AT_CME_INVALID_INDEX;
    }

    if(0 != gastAtParaList[3].usParaLen) /* 设置<text> */
    {
        /* 如果输入的AT命令已经指示是UNICODE编码要进行编码转换成ASCII码 */
        usAlphaTagLength = gastAtParaList[3].usParaLen;


        if(AT_CSCS_UCS2_CODE == stRecords.AlphaTagType)
        {
            /* 当ASCII位数必须是UNICODE的偶数倍 */
            if ((usAlphaTagLength % 2) != 0)
            {
                return AT_ERROR;
            }

            if(AT_FAILURE == At_UnicodePrint2Unicode(gastAtParaList[3].aucPara,&usAlphaTagLength))
            {
                return AT_ERROR;
            }

            if (AT_FAILURE == At_CheckNameUCS2Code(gastAtParaList[3].aucPara, &usAlphaTagLength))
            {
                return AT_ERROR;
            }

            stRecords.AlphaTagType = gastAtParaList[3].aucPara[0];
        }

        /* 最终得到的姓名字段的长度不能超过TAF_PB_RECORD_STRU
           结构中的最大长度，否则会导致堆栈溢出 */
        if(usAlphaTagLength > SI_PB_ALPHATAG_MAX_LEN)
        {
            usAlphaTagLength = SI_PB_ALPHATAG_MAX_LEN;
        }


        PS_MEM_CPY(stRecords.AlphaTag, gastAtParaList[3].aucPara, usAlphaTagLength);

        stRecords.ucAlphaTagLength = (TAF_UINT8)usAlphaTagLength;
    }

    if(0 == gastAtParaList[0].usParaLen) /* 添加 */
    {
        if( AT_SUCCESS == SI_PB_Add(gastAtClientTab[ucIndex].usClientId,
                                                0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }

        return AT_ERROR;
    }

    ulResult      = SI_PB_GetSPBFlag();
    ulResultTemp  = SI_PB_GetStorateType();

    if((1 == ulResult)&&(SI_PB_STORAGE_SM == ulResultTemp))
    {
        if ( AT_SUCCESS == SI_PB_SModify(gastAtClientTab[ucIndex].usClientId,
                                            0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
    }


    if (AT_SUCCESS == SI_PB_Modify(gastAtClientTab[ucIndex].usClientId,
                                   0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_CME_INCORRECT_PARAMETERS;

}
TAF_VOID At_PbIraToUnicode(TAF_UINT8  *pucSrc,
                           TAF_UINT16 usSrcLen,
                           TAF_UINT8  *pucDst,
                           TAF_UINT16 *pusDstLen)
{
    TAF_UINT16      usIndex;
    TAF_UINT16      usUCS2;

    for (usIndex = 0; usIndex < usSrcLen; usIndex++)
    {
        /* 查IRA到UNICODE转义表， 将IRA字符转换为unicode编码 */
        usUCS2 = g_astIraToUnicode[pucSrc[usIndex]].usUnicode;
        *pucDst = (TAF_UINT8)(usUCS2 >> 8);
        pucDst++;
        *pucDst = (TAF_UINT8)usUCS2;
        pucDst++;
    }

    /* 转换后长度为原来的2倍 */
    *pusDstLen = (TAF_UINT16)(usSrcLen << 1);

    return;
}


TAF_UINT32 At_OneUnicodeToGsm(TAF_UINT16 usSrc, TAF_UINT8 *pucDst, TAF_UINT16 *pusDstLen)
{
    TAF_UINT16      usIndex;

    /* 避免输入码流为A0A1时存入SIM卡内容为1B40读出时上报为007C*/
    if (0x00A0 == usSrc)
    {
        AT_LOG1("At_OneUnicodeToGsm no found one unicode[0x%x] to gsm",usSrc);

        return AT_FAILURE;
    }

    /* 查找GSM 到 UNICODE基本表 */
    for(usIndex = 0; usIndex < AT_PB_GSM_MAX_NUM; usIndex++)
    {
        if(usSrc == g_astGsmToUnicode[usIndex].usUnicode)
        {
            *pucDst = g_astGsmToUnicode[usIndex].ucOctet;

            *pusDstLen = 1;

            return AT_SUCCESS;
        }
    }

    /* 查找GSM 到 UNICODE扩展表 */
    for(usIndex = 0; usIndex < AT_PB_GSM7EXT_MAX_NUM; usIndex++)
    {
        if(usSrc == g_astGsm7extToUnicode[usIndex].usUnicode)
        {
            *pucDst++ = AT_PB_GSM7EXT_SYMBOL;
            *pucDst = g_astGsm7extToUnicode[usIndex].ucOctet;

            *pusDstLen = 2;

            return AT_SUCCESS;
        }
    }

    /* 未查找到时返回失败 */
    *pusDstLen = 0;

    AT_LOG1("At_OneUnicodeToGsm no found one unicode[0x%x] to gsm",usSrc);

    return AT_FAILURE;
}


TAF_UINT32 At_UnicodeTransferToGsmCalcLength(TAF_UINT8  *pucSrc,
                                             TAF_UINT16 usSrcLen,
                                             TAF_UINT16 *pusDstLen)
{
     TAF_UINT16     usIndex;
     TAF_UINT16     usUnicodeChar;
     TAF_UINT8      aucGsmData[2];
     TAF_UINT16     usGsmDataLen;

     for (usIndex = 0; usIndex < (usSrcLen >> 1); usIndex++)
     {
        usUnicodeChar = (*pucSrc << 8) | (*(pucSrc + 1));

        /* 判断UNICODE是否可以转义为GSM7BIT */
        if (AT_SUCCESS != At_OneUnicodeToGsm(usUnicodeChar, aucGsmData, &usGsmDataLen))
        {
            AT_ERR_LOG("At_UnicodeTransferToGsmCalcLength error");

            return AT_FAILURE;
        }

        pucSrc     += 2;

        *pusDstLen += usGsmDataLen;
     }

     return AT_SUCCESS;
}
TAF_VOID At_PbUnicodeParse(TAF_UINT8  *pucSrc,
                           TAF_UINT16 usSrcLen,
                           TAF_UINT32 *pulAlphaTagType,
                           TAF_UINT16 *pusDstLen,
                           TAF_UINT16 *pusCodeBase)
{
    TAF_UINT16      usGsmCodeLen = 0;
    TAF_UINT16      us81CodeLen = 0;
    TAF_UINT16      us82CodeLen = 0;
    TAF_UINT16      us81Base;
    TAF_UINT16      us82Base;
    TAF_UINT16      usMinCodeLen;
    TAF_UINT32      ulAlphaTagType;
    TAF_UINT16      usCodeBase;

    /* 先尝试转成GSM模式,如果能转换成功，则返回 */
    if (AT_SUCCESS == At_UnicodeTransferToGsmCalcLength(pucSrc, usSrcLen, &usGsmCodeLen))
    {
        *pulAlphaTagType = SI_PB_ALPHATAG_TYPE_GSM;
        *pusDstLen       = usGsmCodeLen;
        *pusCodeBase     = 0;

        return;
    }

    /***************************************************************************
                            默认置为80编码
    ***************************************************************************/
    usMinCodeLen   = usSrcLen + 1;
    ulAlphaTagType = SI_PB_ALPHATAG_TYPE_UCS2_80;
    usCodeBase     = 0;

    /* 计算81编码长度,选定最小编码长度及相应的编码方式 */
    if (AT_SUCCESS == At_UnicodeTransferTo81CalcLength(pucSrc, usSrcLen, &us81Base, &us81CodeLen))
    {
        if (us81CodeLen <= usMinCodeLen)
        {
            usMinCodeLen   = us81CodeLen;
            ulAlphaTagType = SI_PB_ALPHATAG_TYPE_UCS2_81;
            usCodeBase     = us81Base;
        }
    }

    /* 计算82编码长度,选定最小编码长度及相应的编码方式 */
    if (AT_SUCCESS == At_UnicodeTransferTo82CalcLength(pucSrc, usSrcLen, &us82Base, &us82CodeLen))
    {
        if (us82CodeLen <= usMinCodeLen)
        {
            usMinCodeLen   = us82CodeLen;
            ulAlphaTagType = SI_PB_ALPHATAG_TYPE_UCS2_82;
            usCodeBase     = us82Base;
        }
    }

    *pusDstLen          = usMinCodeLen;
    *pusCodeBase        = usCodeBase;
    *pulAlphaTagType    = ulAlphaTagType;

    return;
}


TAF_UINT32 At_UnicodeTransferToGsm(TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen, TAF_UINT8 *pucDst)
{
    TAF_UINT16      usIndex;
    TAF_UINT16      usUnicodeChar;
    TAF_UINT16      usGsmDataLen = 0;

    for (usIndex = 0; usIndex < (usSrcLen >> 1); usIndex++)
    {
        usUnicodeChar = (*pucSrc << 8)|(*(pucSrc + 1));

        /* 逐个字符进行转换 */
        if (AT_SUCCESS != At_OneUnicodeToGsm(usUnicodeChar, pucDst, &usGsmDataLen))
        {
            AT_ERR_LOG("At_UnicodeTransferToGsm error");

            return AT_FAILURE;
        }

        pucDst += usGsmDataLen;
        pucSrc += 2;
    }

    return AT_SUCCESS;
}
TAF_VOID At_UnicodeTransferTo80(TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen, TAF_UINT8 *pucDst)
{
    pucDst[0] = SI_PB_ALPHATAG_TYPE_UCS2_80;

    VOS_MemCpy(&pucDst[1], pucSrc, usSrcLen);

    return;
}


TAF_UINT32 At_PbUnicodeTransfer(TAF_UINT8           *pucSrc,
                                TAF_UINT16          usSrcLen,
                                SI_PB_RECORD_STRU   *pstRecord,
                                TAF_UINT16          usCodeBase)
{
    TAF_UINT32      ulResult = AT_FAILURE;

    switch (pstRecord->AlphaTagType)
    {
        case SI_PB_ALPHATAG_TYPE_GSM:
            ulResult = At_UnicodeTransferToGsm(pucSrc, usSrcLen, pstRecord->AlphaTag);

            break;

        case SI_PB_ALPHATAG_TYPE_UCS2_81:
            ulResult = At_UnicodeTransferTo81(pucSrc, usSrcLen, usCodeBase, pstRecord->AlphaTag);

            break;

        case SI_PB_ALPHATAG_TYPE_UCS2_82:
            ulResult = At_UnicodeTransferTo82(pucSrc, usSrcLen, usCodeBase, pstRecord->AlphaTag);

            break;

        case SI_PB_ALPHATAG_TYPE_UCS2_80:
            At_UnicodeTransferTo80(pucSrc, usSrcLen, pstRecord->AlphaTag);
            ulResult = AT_SUCCESS;

            break;

        default:

            break;
    }

    if (AT_SUCCESS != ulResult)
    {
        AT_ERR_LOG("At_PbUnicodeTransfer: UCS2 transfer to gsm, 80, 81 or 82 error");
    }

    return ulResult;
}


TAF_UINT32 At_PbAlphaTagProc(SI_PB_RECORD_STRU *pstRecord, AT_PARSE_PARA_TYPE_STRU *pstAtParaList)
{
    TAF_UINT8       *pucAlphaTag;
    TAF_UINT16      usAlphaTagLen;
    TAF_UINT16      usAlphaCodeLen;
    TAF_UINT16      usCodeBase;

    /* 姓名字段为空时，不用处理姓名字段，但电话号码应该保存，所以返回成功 */
    if (0 == pstAtParaList->usParaLen)
    {
        return AT_SUCCESS;
    }

    if (AT_CSCS_GSM_7Bit_CODE == gucAtCscsType)
    {
        if (pstAtParaList->usParaLen > gstPBATInfo.usNameMaxLen)
        {
            AT_ERR_LOG("At_PbAlphaTagProc error: text string to long in gsm7");

            return AT_CME_TEXT_STRING_TOO_LONG;
        }

        pstRecord->AlphaTagType = SI_PB_ALPHATAG_TYPE_GSM;

        /* GSM模式下判断是否所有输入码流小于0x7f，若大于0x7f时提示用户输入中含有非法字符 */
        if (AT_SUCCESS != At_Gsm7BitFormat(pstAtParaList->aucPara,
                                           pstAtParaList->usParaLen,
                                           pstRecord->AlphaTag,
                                           (TAF_UINT16 *)&pstRecord->ucAlphaTagLength))
        {
            AT_ERR_LOG("At_PbAlphaTagProc: error:the eighth bit is 1 in gsm7");

            return AT_CME_INVALID_CHARACTERS_IN_TEXT_STRING;
        }

        return AT_SUCCESS;
    }

    /* 分配空间用于存储中间转换的姓名字段，最多保存为80编码，因此最大空间为输入长度2倍 */
    usAlphaTagLen = (TAF_UINT16)(pstAtParaList->usParaLen << 1);

    pucAlphaTag = (TAF_UINT8*)VOS_MemAlloc(WUEPS_PID_AT, DYNAMIC_MEM_PT, usAlphaTagLen);

    if (VOS_NULL_PTR == pucAlphaTag)
    {
        AT_ERR_LOG("At_PbAlphaTagProc error:malloc failed");

        return AT_FAILURE;
    }

    /* IRA模式输入时先查表转换为UCS2，再处理 */
    if (AT_CSCS_IRA_CODE == gucAtCscsType)
    {
        At_PbIraToUnicode(pstAtParaList->aucPara,
                          pstAtParaList->usParaLen,
                          pucAlphaTag,
                          &usAlphaTagLen);
    }
    else
    {
        /* UCS2编码方式下，两字节标识一个字符，即码流是四的倍数 */
        if (0 != (pstAtParaList->usParaLen % 4))
        {
            VOS_MemFree(WUEPS_PID_AT, pucAlphaTag);

            AT_LOG1("At_PbAlphaTagProc error: ucs2 codes length [%d] is nor 4 multiple",
                    pstAtParaList->usParaLen);

            return AT_ERROR;
        }

        if(AT_FAILURE == At_UnicodePrint2Unicode(pstAtParaList->aucPara, &(pstAtParaList->usParaLen)))
        {
            VOS_MemFree(WUEPS_PID_AT, pucAlphaTag);

            AT_ERR_LOG("At_PbAlphaTagProc error: Printunicode to unicode failed");

            return AT_ERROR;
        }

        usAlphaTagLen = pstAtParaList->usParaLen;

        VOS_MemCpy(pucAlphaTag, pstAtParaList->aucPara, usAlphaTagLen);
    }

    /* 对UCS2或IRA模式转换后的UCS2码流确定存储格式 */
    At_PbUnicodeParse(pucAlphaTag, usAlphaTagLen, &pstRecord->AlphaTagType, &usAlphaCodeLen, &usCodeBase);

    /* 最终确定的编码长度大于电话本姓名字段能存储的长度时，提醒用户输入太长 */
    if (usAlphaCodeLen > gstPBATInfo.usNameMaxLen)
    {
        VOS_MemFree(WUEPS_PID_AT, pucAlphaTag);

        AT_LOG1("At_PbAlphaTagProc error: name length [%d] too long", usAlphaCodeLen);

        return AT_CME_TEXT_STRING_TOO_LONG;
    }

    pstRecord->ucAlphaTagLength = (TAF_UINT8)usAlphaCodeLen;

    if (AT_SUCCESS != At_PbUnicodeTransfer(pucAlphaTag, usAlphaTagLen, pstRecord, usCodeBase))
    {
        VOS_MemFree(WUEPS_PID_AT, pucAlphaTag);

        AT_ERR_LOG("At_PbAlphaTagProc: fail to convert UCS2");

        return AT_ERROR;
    }

    VOS_MemFree(WUEPS_PID_AT, pucAlphaTag);

    return AT_SUCCESS;
}
TAF_UINT32 At_PbEmailTransferToGsm(TAF_UINT8  *pucSrc,
                                   TAF_UINT16 usSrcLen,
                                   TAF_UINT8  *pucDst,
                                   VOS_UINT32 *pulDstLen)
{
    TAF_UINT16      usGsmCodeLen = 0;
    TAF_UINT8       aucEmailUCS2[2 * SI_PB_EMAIL_MAX_LEN];
    TAF_UINT16      usUCS2Len;

    PS_MEM_SET(aucEmailUCS2, 0x0, sizeof(aucEmailUCS2));

    /* 先将输入码流转换为UCS2再做处理 */
    At_PbIraToUnicode(pucSrc, usSrcLen, aucEmailUCS2, &usUCS2Len);

    /* 判断UCS2能否转换为GSM7bit存储 */
    if (AT_SUCCESS != At_UnicodeTransferToGsmCalcLength(aucEmailUCS2, usUCS2Len, &usGsmCodeLen))
    {
        AT_ERR_LOG("At_PbEmailTransferToGsm error: failed to transfer to Gsm");

        return AT_CME_INVALID_CHARACTERS_IN_TEXT_STRING;
    }

    /* EMAIL的长度大于电话本EMAIL字段能存储的长度时，提醒用户输入太长 */
    if (usGsmCodeLen > gstPBATInfo.usEmailLen)
    {
        AT_LOG1("At_PbEmailTransferToGsm error: mail length [%d] too long", usGsmCodeLen);

        return AT_CME_TEXT_STRING_TOO_LONG;
    }

    *pulDstLen = usGsmCodeLen;

    /* 若能转换为GSM7bit则进行转换 */
    At_UnicodeTransferToGsm(aucEmailUCS2, usUCS2Len, pucDst);

    return AT_SUCCESS;
}


TAF_UINT32 At_SetCpbfPara(TAF_UINT8 ucIndex)
{
    SI_PB_STORATE_TYPE                  Storage = SI_PB_STORAGE_UNSPECIFIED;
    SI_PB_RECORD_STRU                   stRecords;
    TAF_UINT32                          ulResult;

    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 处理电话本姓名字段，从传输模式转换为存储模式GSM7BIT或UCS2 80/81/82 */
    ulResult = At_PbAlphaTagProc(&stRecords, &gastAtParaList[0]);

    if (AT_SUCCESS != ulResult)
    {
        AT_ERR_LOG("At_SetCpbfPara:something wrong in At_PbAlphaTagProc");
        return ulResult;
    }

    /* 执行命令操作 */
    if( AT_SUCCESS == SI_PB_Search ( gastAtClientTab[ucIndex].usClientId, 0,
                                      Storage, stRecords.ucAlphaTagLength,
                                      stRecords.AlphaTag)
        )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBF_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}



VOS_UINT32 AT_SetModemStatusPara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    /* 参数不符合 */
    if (2 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 内容长度不符合 */
    if((1 != gastAtParaList[0].usParaLen) || (1 != gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果是balong modem复位，不需要处理该命令 */
    if(0 == gastAtParaList[0].ulParaValue)
    {
        return AT_OK;
    }

    ulRst = AT_FillAndSndCSIMAMsg(gastAtClientTab[ucIndex].usClientId, gastAtParaList[1].ulParaValue);

    if (VOS_OK == ulRst)
    {
        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }

}


TAF_UINT32 At_SetCpbwPara2(TAF_UINT8 ucIndex)
{
    SI_PB_RECORD_STRU   stRecords;
    TAF_UINT8           *pucNumber;
    TAF_UINT16          usLen;
    TAF_UINT32          ulResult;
    TAF_UINT32          ulResultTemp;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType) /* 参数检查 */
    {
        AT_ERR_LOG("At_SetCpbwPara2 error:incorect parameters");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gucAtParaIndex > 4) /* 参数过多 */
    {
        AT_ERR_LOG("At_SetCpbwPara2 error: too many parameters");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 只有一个参数时为删除操作 */
    if (1 == gucAtParaIndex)
    {
        ulResult = At_PbDeleteProc(ucIndex);

        return ulResult;
    }

    if((0 == gastAtParaList[1].usParaLen)&&(0 == gastAtParaList[3].usParaLen))
    {
        return AT_ERROR;
    }

    PS_MEM_SET(&stRecords, 0x00, sizeof(stRecords));/* 初始化 */

    if(0 != gastAtParaList[0].usParaLen)/* 设置<index> */
    {
        stRecords.Index = (TAF_UINT16)gastAtParaList[0].ulParaValue;
    }

    if(0 != gastAtParaList[1].usParaLen)/* 设置<number> */
    {
        if('+' == gastAtParaList[1].aucPara[0])
        {
            pucNumber = gastAtParaList[1].aucPara   + 1;
            usLen     = gastAtParaList[1].usParaLen - 1;
        }
        else
        {
            pucNumber = gastAtParaList[1].aucPara;
            usLen     = gastAtParaList[1].usParaLen;
        }

        if(AT_FAILURE == At_CheckPBString(pucNumber,&usLen))
        {
            AT_ERR_LOG("At_SetCpbwPara2 error: invalid characters in dial string");

            return AT_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
        }

        PS_MEM_CPY(stRecords.Number, pucNumber, usLen);

        stRecords.NumberLength = (TAF_UINT8)usLen;
    }

    /* 处理电话本姓名字段，从传输模式转换为存储模式GSM7BIT或UCS2 80/81/82 */
    ulResult = At_PbAlphaTagProc(&stRecords, &gastAtParaList[3]);

    if (AT_SUCCESS != ulResult)
    {
        AT_ERR_LOG("At_SetCpbwPara2:something wrong in At_PbAlphaTagProc");
        return ulResult;
    }

    if (0 != gastAtParaList[2].usParaLen)/* 设置<type> */
    {
        /* 号码类型待修改 */
        stRecords.NumberType = (PB_NUMBER_TYPE_INTERNATIONAL == At_GetCodeType(gastAtParaList[1].aucPara[0]))
                              ? PB_NUMBER_TYPE_INTERNATIONAL : (TAF_UINT8)gastAtParaList[2].ulParaValue;
    }
    else
    {
        stRecords.NumberType = (TAF_UINT8)At_GetCodeType(gastAtParaList[1].aucPara[0]);
    }

    if (stRecords.NumberType < 128)
    {
        AT_ERR_LOG("At_SetCpbwPara2 error: invalid index");

        return AT_CME_INVALID_INDEX;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        /* 添加记录 */
        if (AT_SUCCESS == SI_PB_Add(gastAtClientTab[ucIndex].usClientId,
                                    0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }

        AT_ERR_LOG("At_SetCpbwPara2 error: add pb record failed");

        return AT_ERROR;
    }

    /* 修改记录 */
    ulResult      = SI_PB_GetSPBFlag();
    ulResultTemp  = SI_PB_GetStorateType();

    if((1 == ulResult)&&(SI_PB_STORAGE_SM == ulResultTemp))
    {
        if ( AT_SUCCESS == SI_PB_SModify(gastAtClientTab[ucIndex].usClientId,
                                            0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
    }

    if (AT_SUCCESS == SI_PB_Modify(gastAtClientTab[ucIndex].usClientId,
                                   0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    AT_ERR_LOG("At_SetCpbwPara2 error: modify pb record failed");

    return AT_ERROR;
}
TAF_UINT32 At_SetScpbwPara(TAF_UINT8 ucIndex)
{
    SI_PB_RECORD_STRU   stRecords;
    TAF_UINT8           *pucNumber;
    TAF_UINT16          usLen;
    TAF_UINT16          usAlphaTagLength;
    TAF_UINT32          ulResult;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)    /* 参数检查 */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gucAtParaIndex > 12)/* 参数过多 */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 只有一个参数时为删除操作 */
    if (1 == gucAtParaIndex)
    {
        ulResult = At_PbDeleteProc(ucIndex);

        return ulResult;
    }

    if ((0 == gastAtParaList[1].usParaLen) && (0 == gastAtParaList[3].usParaLen)
     && (0 == gastAtParaList[5].usParaLen) && (0 == gastAtParaList[7].usParaLen)
     && (0 == gastAtParaList[9].usParaLen) && (0 == gastAtParaList[11].usParaLen))
    {
        return AT_ERROR;
    }

    /* 确认编码类型，UNICODE或是ASCII码 */
    PS_MEM_SET(&stRecords, 0x00, sizeof(stRecords));/* 初始化 */

    if (0 != gastAtParaList[0].usParaLen)/* 设置<index> */
    {
        stRecords.Index = (TAF_UINT16)gastAtParaList[0].ulParaValue;
    }

    if (0 != gastAtParaList[1].usParaLen)    /* 设置<num1> */
    {
        if ('+' == gastAtParaList[1].aucPara[0])
        {
            pucNumber = gastAtParaList[1].aucPara   + 1;
            usLen     = gastAtParaList[1].usParaLen - 1;
        }
        else
        {
            pucNumber = gastAtParaList[1].aucPara;
            usLen     = gastAtParaList[1].usParaLen;
        }

        if (AT_FAILURE == At_CheckPBString(pucNumber,&usLen))
        {
            return AT_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
        }

        PS_MEM_CPY(stRecords.Number, pucNumber, usLen);

        stRecords.NumberLength = (TAF_UINT8)usLen;
    }

    /* 当号码为空时，类型字段按协议置为0xFF */
    if(0 == gastAtParaList[1].usParaLen)
    {
        stRecords.NumberType = 0xFF;
        stRecords.NumberLength = 0;
    }
    else
    {
        if (0 != gastAtParaList[2].usParaLen)/* 设置<type1> */
        {
            /* 号码类型待修改 */
            stRecords.NumberType = (PB_NUMBER_TYPE_INTERNATIONAL == At_GetCodeType(gastAtParaList[1].aucPara[0]))
                                  ? PB_NUMBER_TYPE_INTERNATIONAL : (TAF_UINT8)gastAtParaList[2].ulParaValue;
        }
        else
        {
            stRecords.NumberType = (TAF_UINT8)At_GetCodeType(gastAtParaList[1].aucPara[0]);
        }
    }

    if (0 != gastAtParaList[3].usParaLen)    /* 设置<num2> */
    {
        if ('+' == gastAtParaList[3].aucPara[0])
        {
            pucNumber = gastAtParaList[3].aucPara   + 1;
            usLen     = gastAtParaList[3].usParaLen - 1;
        }
        else
        {
            pucNumber = gastAtParaList[3].aucPara;
            usLen     = gastAtParaList[3].usParaLen;
        }

        if (AT_FAILURE == At_CheckPBString(pucNumber,&usLen))
        {
            return AT_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
        }

        PS_MEM_CPY(stRecords.AdditionNumber[0].Number, pucNumber, usLen);

        stRecords.AdditionNumber[0].NumberLength = (TAF_UINT8)usLen;
    }

    if(0 == gastAtParaList[3].usParaLen)
    {
        stRecords.AdditionNumber[0].NumberType = 0xFF;
        stRecords.AdditionNumber[0].NumberLength = 0;
    }
    else
    {
        if (0 != gastAtParaList[4].usParaLen)/* 设置<type2> */
        {
            /* 号码类型待修改 */
            stRecords.AdditionNumber[0].NumberType = (PB_NUMBER_TYPE_INTERNATIONAL == At_GetCodeType(gastAtParaList[3].aucPara[0]))
                                                    ? PB_NUMBER_TYPE_INTERNATIONAL : (TAF_UINT8)gastAtParaList[4].ulParaValue;
        }
        else
        {
            stRecords.AdditionNumber[0].NumberType = (TAF_UINT8)At_GetCodeType(gastAtParaList[3].aucPara[0]);
        }
    }

    if (0 != gastAtParaList[5].usParaLen)    /* 设置<num3> */
    {
        if ('+' == gastAtParaList[5].aucPara[0])
        {
            pucNumber = gastAtParaList[5].aucPara   + 1;
            usLen     = gastAtParaList[5].usParaLen - 1;
        }
        else
        {
            pucNumber = gastAtParaList[5].aucPara;
            usLen     = gastAtParaList[5].usParaLen;
        }

        if (AT_FAILURE == At_CheckPBString(pucNumber,&usLen))
        {
            return AT_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
        }

        PS_MEM_CPY(stRecords.AdditionNumber[1].Number, pucNumber, usLen);

        stRecords.AdditionNumber[1].NumberLength = (TAF_UINT8)usLen;
    }

    if(0 == gastAtParaList[5].usParaLen)
    {
        stRecords.AdditionNumber[1].NumberType = 0xFF;
        stRecords.AdditionNumber[1].NumberLength = 0;
    }
    else
    {
        if (0 != gastAtParaList[6].usParaLen)/* 设置<type3> */
        {
            /* 号码类型待修改 */
            stRecords.AdditionNumber[1].NumberType = (PB_NUMBER_TYPE_INTERNATIONAL == At_GetCodeType(gastAtParaList[5].aucPara[0]))
                                                    ? PB_NUMBER_TYPE_INTERNATIONAL : (TAF_UINT8)gastAtParaList[6].ulParaValue;
        }
        else
        {
            stRecords.AdditionNumber[1].NumberType = (TAF_UINT8)At_GetCodeType(gastAtParaList[5].aucPara[0]);
        }
    }

    if (0 != gastAtParaList[7].usParaLen)    /* 设置<num4> */
    {
        if ('+' == gastAtParaList[7].aucPara[0])
        {
            pucNumber = gastAtParaList[7].aucPara   + 1;
            usLen     = gastAtParaList[7].usParaLen - 1;
        }
        else
        {
            pucNumber = gastAtParaList[7].aucPara;
            usLen     = gastAtParaList[7].usParaLen;
        }

        if (AT_FAILURE == At_CheckPBString(pucNumber,&usLen))
        {
            return AT_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
        }

        PS_MEM_CPY(stRecords.AdditionNumber[2].Number, pucNumber, usLen);

        stRecords.AdditionNumber[2].NumberLength = (TAF_UINT8)usLen;
    }

    if(0 == gastAtParaList[7].usParaLen)
    {
        stRecords.AdditionNumber[2].NumberType = 0xFF;
        stRecords.AdditionNumber[2].NumberLength = 0;
    }
    else
    {
        if (0 != gastAtParaList[8].usParaLen)/* 设置<type4> */
        {
            /* 号码类型待修改 */
            stRecords.AdditionNumber[2].NumberType = (PB_NUMBER_TYPE_INTERNATIONAL == At_GetCodeType(gastAtParaList[7].aucPara[0]))
                                                    ? PB_NUMBER_TYPE_INTERNATIONAL : (TAF_UINT8)gastAtParaList[8].ulParaValue;
        }
        else
        {
            stRecords.AdditionNumber[2].NumberType = (TAF_UINT8)At_GetCodeType(gastAtParaList[7].aucPara[0]);
        }
    }

    /* 设置ucAlphaTagType */
    if (AT_CSCS_UCS2_CODE == gastAtParaList[10].ulParaValue)
    {
        stRecords.AlphaTagType = AT_CSCS_UCS2_CODE;
    }
    else if (AT_CSCS_IRA_CODE == gastAtParaList[10].ulParaValue)
    {
        stRecords.AlphaTagType = AT_CSCS_IRA_CODE;
    }
    else
    {
        stRecords.AlphaTagType = AT_CSCS_IRA_CODE;
    }

    if (0 != gastAtParaList[9].usParaLen) /* 设置<text> */
    {
        /* 如果输入的AT命令已经指示是UNICODE编码要进行编码转换成ASCII码 */
        usAlphaTagLength = gastAtParaList[9].usParaLen;

        if (AT_CSCS_UCS2_CODE == stRecords.AlphaTagType)
        {
            /* 当ASCII位数必须是UNICODE的偶数倍 */
            if ((usAlphaTagLength % 2) != 0)
            {
                return AT_ERROR;
            }

            if (AT_FAILURE == At_UnicodePrint2Unicode(gastAtParaList[9].aucPara,&usAlphaTagLength))
            {
                return AT_ERROR;
            }

            if (AT_FAILURE == At_CheckNameUCS2Code(gastAtParaList[9].aucPara, &usAlphaTagLength))
            {
                return AT_ERROR;
            }

            stRecords.AlphaTagType = gastAtParaList[9].aucPara[0];
        }

        /* 最终得到的姓名字段的长度不能超过TAF_PB_RECORD_STRU
        结构中的最大长度，否则会导致堆栈溢出 */
        if (usAlphaTagLength > SI_PB_ALPHATAG_MAX_LEN)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        PS_MEM_CPY(stRecords.AlphaTag, gastAtParaList[9].aucPara, usAlphaTagLength);
        stRecords.ucAlphaTagLength = (TAF_UINT8)usAlphaTagLength;
    }

    if (0 != gastAtParaList[11].usParaLen)  /* 设置<email> */
    {
        if (gastAtParaList[11].usParaLen > SI_PB_EMAIL_MAX_LEN)
        {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }

        ulResult = At_PbEmailTransferToGsm(gastAtParaList[11].aucPara,
                                           gastAtParaList[11].usParaLen,
                                           stRecords.Email.Email,
                                           &stRecords.Email.EmailLen);

        if (AT_SUCCESS != ulResult)
        {
            return ulResult;
        }
    }

    if (0 == gastAtParaList[0].usParaLen) /* 添加 */
    {
        if ( AT_SUCCESS == SI_PB_SAdd(gastAtClientTab[ucIndex].usClientId,
                                        0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }

        return AT_ERROR;

    }

    /* 修改 */
    if ( AT_SUCCESS == SI_PB_SModify(gastAtClientTab[ucIndex].usClientId,
                                        0, SI_PB_STORAGE_UNSPECIFIED, &stRecords))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPBW_SET;/* 设置当前操作类型 */

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;

}
TAF_UINT32 At_SetScpbrPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 == gastAtParaList[1].usParaLen)
    {
        gastAtParaList[1].ulParaValue = gastAtParaList[0].ulParaValue;
    }

    if (AT_SUCCESS == SI_PB_SRead(gastAtClientTab[ucIndex].usClientId,
                                0, SI_PB_STORAGE_UNSPECIFIED,
                                (TAF_UINT16)gastAtParaList[0].ulParaValue,
                                (TAF_UINT16)gastAtParaList[1].ulParaValue))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SCPBR_SET;

        gulPBPrintTag = TAF_FALSE;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}



TAF_UINT32 At_SetCnumPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PB_Read(gastAtClientTab[ucIndex].usClientId,0, SI_PB_STORAGE_ON, 0, 0))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CNUM_READ;

        gulPBPrintTag = TAF_FALSE;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}



TAF_UINT32 At_SetCsimPara(TAF_UINT8 ucIndex)
{
    SI_PIH_CSIM_COMMAND_STRU Command;

    /* 参数检查 */
    if((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 初始化 */
    PS_MEM_SET(&Command,0x00,sizeof(Command));

    if(0 != (gastAtParaList[0].ulParaValue % 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != (gastAtParaList[1].usParaLen % 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara,&gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gastAtParaList[0].ulParaValue != (TAF_UINT32)(gastAtParaList[1].usParaLen * 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<length> */
    Command.ulLen = gastAtParaList[1].usParaLen;

    /* 设置<command> */
    PS_MEM_CPY((TAF_VOID*)Command.aucCommand,(TAF_VOID*)gastAtParaList[1].aucPara,(VOS_UINT16)Command.ulLen);

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PIH_GenericAccessReq(gastAtClientTab[ucIndex].usClientId, 0, &Command))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSIM_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetCchoPara(TAF_UINT8 ucIndex)
{
    SI_PIH_CCHO_COMMAND_STRU    stCchoCmd;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 字符串长度不为2的整数倍 */
    if(0 != (gastAtParaList[0].usParaLen % 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 将字符串转换为16进制数组 */
    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[0].aucPara, &gastAtParaList[0].usParaLen))
    {
        AT_ERR_LOG("At_SetCchoPara: At_AsciiNum2HexString fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    stCchoCmd.ulAIDLen   = gastAtParaList[0].usParaLen;
    stCchoCmd.pucADFName = gastAtParaList[0].aucPara;

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PIH_CchoSetReq(gastAtClientTab[ucIndex].usClientId, 0, &stCchoCmd))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCHO_SET;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}
TAF_UINT32 At_SetCchcPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PIH_CchcSetReq(gastAtClientTab[ucIndex].usClientId,
                                    0,
                                    gastAtParaList[0].ulParaValue))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCHC_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}
TAF_UINT32 At_SetCglaPara(TAF_UINT8 ucIndex)
{
    SI_PIH_CGLA_COMMAND_STRU    stCglaCmd;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(3 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <length>需要为2的整数倍 */
    if(0 != (gastAtParaList[1].ulParaValue % 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 字符串长度不为2的整数倍 */
    if(0 != (gastAtParaList[2].usParaLen % 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 将字符串转换为16进制数组 */
    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[2].aucPara, &gastAtParaList[2].usParaLen))
    {
        AT_ERR_LOG("At_SetCglaPara: At_AsciiNum2HexString fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* length字段是实际命令长度的2倍 */
    if(gastAtParaList[1].ulParaValue != (TAF_UINT32)(gastAtParaList[2].usParaLen * 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stCglaCmd.ulSessionID   = gastAtParaList[0].ulParaValue;
    stCglaCmd.ulLen         = gastAtParaList[2].usParaLen;
    stCglaCmd.pucCommand    = gastAtParaList[2].aucPara;

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PIH_CglaSetReq(gastAtClientTab[ucIndex].usClientId, 0, &stCglaCmd))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGLA_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}
TAF_UINT32 At_SetCardATRPara(TAF_UINT8 ucIndex)
{
    if (AT_SUCCESS == SI_PIH_GetCardATRReq(gastAtClientTab[ucIndex].usClientId, 0))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CARD_ATR_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

/* Modified by c00318887 for file refresh需要触发背景搜, 2015-4-28, begin */

VOS_UINT32 At_SetRefreshStub(VOS_UINT8 ucIndex)
{
    TAF_MMA_REFRESH_STUB_SET_REQ_STRU                       stRefreshStub;
    VOS_UINT8                                               i;
    VOS_UINT32                                              ulTotalNum;
    VOS_UINT32                                              ulRefreshFileType;
    VOS_UINT32                                              ulReceivePid;
    VOS_UINT32                                              ulFileId;

    ulTotalNum        = 0;
    ulRefreshFileType = 0;
    ulReceivePid      = 0;
    ulFileId          = 0;
    PS_MEM_SET(&stRefreshStub, 0, sizeof(stRefreshStub));


    At_String2Hex(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen, &ulReceivePid);
    At_String2Hex(gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen, &ulRefreshFileType);
    At_String2Hex(gastAtParaList[2].aucPara, gastAtParaList[2].usParaLen, &ulTotalNum);

    stRefreshStub.ulReceivePid      = ulReceivePid;
    stRefreshStub.ucTotalNum        = (VOS_UINT8)ulTotalNum;
    stRefreshStub.usRefreshFileType = (VOS_UINT8)ulRefreshFileType;

    if (stRefreshStub.ucTotalNum > TAF_MMA_MAX_FILE_ID_NUM)
    {
        stRefreshStub.ucTotalNum = TAF_MMA_MAX_FILE_ID_NUM;
    }

    for (i = 0; i < stRefreshStub.ucTotalNum; i++)
    {
        At_String2Hex(gastAtParaList[i+3].aucPara, gastAtParaList[i+3].usParaLen, &ulFileId);
        stRefreshStub.ausEfId[i] = (VOS_UINT16)ulFileId;
    }

    TAF_SetRefreshStub(gastAtClientTab[ucIndex].usClientId,
                    gastAtClientTab[ucIndex].opId,
                    &stRefreshStub);

    return AT_OK;
}



#if ( VOS_WIN32 == VOS_OS_VER )
/*****************************************************************************
 函 数 名  : At_SetDelayBgStub
 功能描述  : 对 en_NV_Item_HIGH_PRIO_PLMN_REFRESH_TRIGGER_BG_SEARCH_CFG 打桩
             AT^DELAYBGSTUB=<OnOff>,<DelayLen>
             第1个参数为OnOff,1--使能，0--关闭
             第2个参数为DelayLen ---延时时长，单位; 秒；
 输出参数  : VOS_VOID
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月6日
    作    者   : c00318887
    修改内容   : 新增函数
*****************************************************************************/
VOS_UINT32 At_SetDelayBgStub(VOS_UINT8 ucIndex)
{
    VOS_UINT32                                              ulDelayLen;
    VOS_UINT32                                              ulOnOff;

    ulDelayLen        = 0;
    ulOnOff          = 0;

    At_String2Hex(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen, &ulOnOff);
    At_String2Hex(gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen, &ulDelayLen);

    NAS_SetHighPrioPlmnRefreshTriggerBGSearchCfg((VOS_UINT8)ulOnOff, ulDelayLen);

    return AT_OK;
}
#endif
/* Added by c00318887 for file refresh需要触发背景搜, 2015-4-28, end */

VOS_UINT32 At_SetAutoReselStub(VOS_UINT8 ucIndex)
{
    TAF_MMA_AUTO_RESEL_STUB_SET_REQ_STRU                    stAutoReselStub;

    PS_MEM_SET(&stAutoReselStub, 0, sizeof(stAutoReselStub));

    stAutoReselStub.ucActiveFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    TAF_SetAutoReselStub(gastAtClientTab[ucIndex].usClientId,
                    gastAtClientTab[ucIndex].opId,
                    &stAutoReselStub);

    return AT_OK;
}


TAF_UINT32  At_CrsmFilePathCheck(TAF_UINT16 usEfId, TAF_UINT8 *pucFilePath, TAF_UINT16 *pusPathLen)
{
    TAF_UINT16                          usLen;
    TAF_UINT16                          ausPath[USIMM_MAX_PATH_LEN]  = {0};
    TAF_UINT16                          ausTmpPath[USIMM_MAX_PATH_LEN]  = {0};
    TAF_UINT16                          usPathLen;
    TAF_UINT16                          i;


    usPathLen   = *pusPathLen;
    usLen       = 0;

    for (i = 0; i < (usPathLen/sizeof(TAF_UINT16)); i++)
    {
        ausTmpPath[i] = ((pucFilePath[i*2]<<0x08)&0xFF00) + pucFilePath[(i*2)+1];
    }

    /* 如果路径不是以3F00开始，需要添加3F00作开头 */
    if (MFID != ausTmpPath[0])
    {
        ausPath[0] = MFID;

        usLen++;
    }

    VOS_MemCpy(&ausPath[usLen], ausTmpPath, usPathLen);

    usLen += (usPathLen/sizeof(TAF_UINT16));

    if ((usEfId & 0xFF00) == EFIDUNDERMF)
    {
        usLen = 1;
    }
    /* 4F文件要在5F下，路径长度为3 */
    else if ((usEfId & 0xFF00) == EFIDUNDERMFDFDF)
    {
        if ((usLen != 3)
            ||((ausPath[1]&0xFF00) != DFIDUNDERMF)
            ||((ausPath[2]&0xFF00) != DFIDUNDERMFDF))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    /* 6F文件要在7F下，路径长度为2 */
    else if ((usEfId & 0xFF00) == EFIDUNDERMFDF)
    {
        if ((usLen != 2)
            ||((ausPath[1]&0xFF00) != DFIDUNDERMF))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        /* return AT_CME_INCORRECT_PARAMETERS; */
    }

    *pusPathLen  = usLen;

    VOS_MemCpy(pucFilePath, ausPath, (VOS_SIZE_T)(usLen*2));

    return AT_SUCCESS;
}


TAF_UINT32  At_CrsmApduParaCheck(VOS_VOID)
{
    TAF_UINT16                          usFileTag;

    /* 命令类型参数检查，第一个参数不能为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        AT_ERR_LOG("At_SetCrsmPara: command type null");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 除STATUS命令外，文件ID输入不能为空 */
    if ((0 == gastAtParaList[1].ulParaValue)
        && (USIMM_STATUS != gastAtParaList[0].ulParaValue))
    {
        AT_ERR_LOG("At_CrsmApduParaCheck: File Id null.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取文件ID前两位 */
    usFileTag   = (gastAtParaList[1].ulParaValue >> 8) & (0x00FF);

    /* 输入的文件ID必须是EF文件，前两位不可以是3F/5F/7F */
    if ((MFLAB == usFileTag)
       || (DFUNDERMFLAB == usFileTag)
       || (DFUNDERDFLAB == usFileTag))
    {
        AT_ERR_LOG("At_CrsmApduParaCheck: File Id error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <P1><P2><P3>这三个参数全部为空 */
    if ((0 == gastAtParaList[2].usParaLen)
        && (0 == gastAtParaList[3].usParaLen)
        && (0 == gastAtParaList[4].usParaLen))
    {
        return AT_SUCCESS;
    }

    /* <P1><P2><P3>这三个参数全部不为空 */
    if ((0 != gastAtParaList[2].usParaLen)
        && (0 != gastAtParaList[3].usParaLen)
        && (0 != gastAtParaList[4].usParaLen))
    {
        return AT_SUCCESS;
    }

    /* 其它情况下属于输入AT命令参数不完整 */
    return AT_CME_INCORRECT_PARAMETERS;

}
TAF_UINT32 At_CrsmFilePathParse(TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{
    TAF_UINT32                          ulResult;

    /* 如果词法解析器解析第七个参数为空，说明没有文件路径输入，直接返回成功 */
    if (0 == gastAtParaList[6].usParaLen)
    {
        return AT_SUCCESS;
    }

    /* 在转换前输入的文件路径长度必须是4的整数倍 */
    if (0 != (gastAtParaList[6].usParaLen % 4))
    {
        AT_ERR_LOG("At_CrsmFilePathParse: Path error");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*将输入的字符串转换成十六进制数组*/
    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[6].aucPara, &gastAtParaList[6].usParaLen))
    {
        AT_ERR_LOG("At_CrsmFilePathParse: At_AsciiNum2HexString error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果有填写文件ID和路径，要做文件路径检查，输入的路径长度以U16为单位 */
    ulResult = At_CrsmFilePathCheck((TAF_UINT16)gastAtParaList[1].ulParaValue,
                                    gastAtParaList[6].aucPara,
                                    &gastAtParaList[6].usParaLen);

    if (AT_SUCCESS != ulResult)
    {
        AT_ERR_LOG("At_CrsmFilePathParse: At_CrsmFilePathCheck error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 填充文件路径和长度 */
    pstCommand->usPathLen   = gastAtParaList[6].usParaLen;

    /* 文件路径长度是U16为单位的，路径拷贝的长度要乘2 */
    PS_MEM_CPY(pstCommand->ausPath, gastAtParaList[6].aucPara, (VOS_SIZE_T)(gastAtParaList[6].usParaLen*sizeof(VOS_UINT16)));

    return AT_SUCCESS;
}


TAF_UINT32 At_CrsmParaStatusCheck (TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{
    /* STATUS命令如果没有输入文件ID，就不需要做选文件操作，直接发STATUS命令 */
    if (0 == gastAtParaList[1].ulParaValue)
    {
        pstCommand->usEfId = VOS_NULL_WORD;
    }
    else
    {
        pstCommand->usEfId = (TAF_UINT16)gastAtParaList[1].ulParaValue;
    }

    /* 填写数据结构中的<P1><P2><P3>对应的IE项 */
    pstCommand->ucP1        =   (TAF_UINT8)gastAtParaList[2].ulParaValue;
    pstCommand->ucP2        =   (TAF_UINT8)gastAtParaList[3].ulParaValue;
    pstCommand->ucP3        =   (TAF_UINT8)gastAtParaList[4].ulParaValue;
    pstCommand->usCmdType   =   USIMM_STATUS;

    return At_CrsmFilePathParse(pstCommand);
}


TAF_UINT32 At_CrsmParaReadBinaryCheck (TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{

    /* 填写数据结构中的<P1><P2><P3>对应的IE项 */
    pstCommand->ucP1        =   (TAF_UINT8)gastAtParaList[2].ulParaValue;
    pstCommand->ucP2        =   (TAF_UINT8)gastAtParaList[3].ulParaValue;
    pstCommand->ucP3        =   (TAF_UINT8)gastAtParaList[4].ulParaValue;
    pstCommand->usEfId      =   (TAF_UINT16)gastAtParaList[1].ulParaValue;
    pstCommand->usCmdType   =   USIMM_READ_BINARY;

    /* 如果有输入文件路径需要检查输入参数 */
    return At_CrsmFilePathParse(pstCommand);
}



TAF_UINT32 At_CrsmParaReadRecordCheck(TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{

    /* 填写数据结构中的<P1><P2><P3>对应的IE项 */
    pstCommand->ucP1        =   (TAF_UINT8)gastAtParaList[2].ulParaValue;
    pstCommand->ucP2        =   (TAF_UINT8)gastAtParaList[3].ulParaValue;
    pstCommand->ucP3        =   (TAF_UINT8)gastAtParaList[4].ulParaValue;
    pstCommand->usEfId      =   (TAF_UINT16)gastAtParaList[1].ulParaValue;
    pstCommand->usCmdType   =   USIMM_READ_RECORD;

    /* 如果有输入文件路径需要检查输入参数 */
    return At_CrsmFilePathParse(pstCommand);
}


VOS_UINT32 At_CrsmParaGetRspCheck(TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{

    /* 参数个数不能少于2个，至少要有命令类型和文件ID */
    if (gucAtParaIndex < 2)
    {
        AT_ERR_LOG("At_CrsmParaGetRspCheck: Para less than 2.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 填写数据结构中的<P1><P2><P3>对应的IE项 */
    pstCommand->ucP1        =   (TAF_UINT8)gastAtParaList[2].ulParaValue;
    pstCommand->ucP2        =   (TAF_UINT8)gastAtParaList[3].ulParaValue;
    pstCommand->ucP3        =   (TAF_UINT8)gastAtParaList[4].ulParaValue;
    pstCommand->usEfId      =   (TAF_UINT16)gastAtParaList[1].ulParaValue;
    pstCommand->usCmdType   =   USIMM_GET_RESPONSE;

    /* 如果有输入文件路径需要检查输入参数 */
    return At_CrsmFilePathParse(pstCommand);
}
VOS_UINT32 At_CrsmParaUpdateBinaryCheck (TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{

    /* Update Binary命令至少要有6个参数，可以没有文件路径 */
    if (gucAtParaIndex < 6)
    {
        AT_ERR_LOG("At_CrsmParaUpdateBinaryCheck: Para less than 6.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 填写数据结构中的<P1><P2><P3>对应的IE项 */
    pstCommand->ucP1        =   (TAF_UINT8)gastAtParaList[2].ulParaValue;
    pstCommand->ucP2        =   (TAF_UINT8)gastAtParaList[3].ulParaValue;
    pstCommand->ucP3        =   (TAF_UINT8)gastAtParaList[4].ulParaValue;
    pstCommand->usEfId      =   (TAF_UINT16)gastAtParaList[1].ulParaValue;
    pstCommand->usCmdType   =   USIMM_UPDATE_BINARY;

    /* 第六个参数输入的<data>字符串在转换前数据长度必须是2的倍数且不能为0 */
    if ((0 != (gastAtParaList[5].usParaLen % 2))
        || (0 == gastAtParaList[5].usParaLen))
    {
        AT_ERR_LOG("At_CrsmParaUpdateBinaryCheck: <data> error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[5].aucPara, &gastAtParaList[5].usParaLen))
    {
        AT_ERR_LOG("At_CrsmParaUpdateBinaryCheck: At_AsciiNum2HexString fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<data>，其长度由<data>参数输入确定，P3参数照常下发，不关心<data>的长度是否和P3的值匹配 */
    PS_MEM_CPY((TAF_VOID*)pstCommand->aucCommand,
               (TAF_VOID*)gastAtParaList[5].aucPara,
               gastAtParaList[5].usParaLen);

    return At_CrsmFilePathParse(pstCommand);
}


VOS_UINT32 At_CrsmParaUpdateRecordCheck (TAF_SIM_RESTRIC_ACCESS_STRU *pstCommand)
{

    /* Update Binary命令至少要有6个参数，可以没有文件路径 */
    if (gucAtParaIndex < 6)
    {
        AT_ERR_LOG("At_CrsmParaUpdateRecordCheck: Para less than 6.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 填写数据结构中的<P1><P2><P3>对应的IE项 */
    pstCommand->ucP1        =   (TAF_UINT8)gastAtParaList[2].ulParaValue;
    pstCommand->ucP2        =   (TAF_UINT8)gastAtParaList[3].ulParaValue;
    pstCommand->ucP3        =   (TAF_UINT8)gastAtParaList[4].ulParaValue;
    pstCommand->usEfId      =   (TAF_UINT16)gastAtParaList[1].ulParaValue;
    pstCommand->usCmdType   =   USIMM_UPDATE_RECORD;

     /* 第六个参数输入的<data>字符串数据长度必须是2的倍数且不能为0 */
    if ((0 != (gastAtParaList[5].usParaLen % 2))
        || (0 == gastAtParaList[5].usParaLen))
    {
        AT_ERR_LOG("At_CrsmParaUpdateRecordCheck: <data> error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[5].aucPara, &gastAtParaList[5].usParaLen))
    {
        AT_ERR_LOG("At_CrsmParaUpdateRecordCheck: At_AsciiNum2HexString fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<data>，其长度由<data>参数输入确定，P3参数照常下发，不关心<data>的长度是否和P3的值匹配 */
    PS_MEM_CPY((TAF_VOID*)pstCommand->aucCommand,
               (TAF_VOID*)gastAtParaList[5].aucPara,
               gastAtParaList[5].usParaLen);

    return At_CrsmFilePathParse(pstCommand);
}


TAF_UINT32 At_SetCrsmPara(TAF_UINT8 ucIndex)
{
    TAF_SIM_RESTRIC_ACCESS_STRU         stCommand;
    TAF_UINT32                          ulResult;

    /* 参数过多 */
    if (gucAtParaIndex > 7)
    {
        AT_ERR_LOG("At_SetCrsmPara: too many para");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果有输入<P1><P2><P3>，这三个参数不能只填写部分参数 */
    if (AT_SUCCESS != At_CrsmApduParaCheck())
    {
       AT_ERR_LOG("At_SetCrsmPara: At_CrsmApduParaCheck fail.");

       return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 初始化 */
    PS_MEM_SET(&stCommand, 0x00, sizeof(TAF_SIM_RESTRIC_ACCESS_STRU));

    switch(gastAtParaList[0].ulParaValue)
    {
        case USIMM_STATUS:
            ulResult = At_CrsmParaStatusCheck(&stCommand);
            break;
        case USIMM_READ_BINARY:
            ulResult = At_CrsmParaReadBinaryCheck(&stCommand);
            break;
        case USIMM_READ_RECORD:
            ulResult = At_CrsmParaReadRecordCheck(&stCommand);
            break;
        case USIMM_GET_RESPONSE:
            ulResult = At_CrsmParaGetRspCheck(&stCommand);
            break;
        case USIMM_UPDATE_BINARY:
            ulResult = At_CrsmParaUpdateBinaryCheck(&stCommand);
            break;
        case USIMM_UPDATE_RECORD:
            ulResult = At_CrsmParaUpdateRecordCheck(&stCommand);
            break;
        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SUCCESS != ulResult )
    {
        AT_ERR_LOG("At_SetCrsmPara: para parse fail");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_UsimRestrictedAccessCommand(gastAtClientTab[ucIndex].usClientId, 0,&stCommand))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CRSM_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_CheckCpinPara(VOS_VOID)
{
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();


    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 如果<pin>过长或过短，直接返回错误 */
    if ( (TAF_PH_PINCODELENMAX < gastAtParaList[0].usParaLen)
      || (TAF_PH_PINCODELENMIN > gastAtParaList[0].usParaLen) )
    {
        if (SYSTEM_APP_ANDROID != *pucSystemAppConfig)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        else
        {
            /* AP-MODEM形态下，当参数个数为1时, 第一个参数允许长度为16 */
            if ( (TAF_PH_UNLOCKCODELEN != gastAtParaList[0].usParaLen)
              || (1 != gucAtParaIndex) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }
    }

    /* 如果两个参数时，<newpin>过长或过短，直接返回错误 */
    if (2 == gucAtParaIndex)
    {
        if ( (TAF_PH_PINCODELENMAX < gastAtParaList[1].usParaLen)
          || (TAF_PH_PINCODELENMIN > gastAtParaList[1].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;

}
TAF_UINT32 At_SetCpinPara(TAF_UINT8 ucIndex)
{
    TAF_UINT32                          ulResult;
    MN_PH_SET_CPIN_REQ_STRU             stSetCPinReq;

    /* 参数检查 */
    ulResult = At_CheckCpinPara();
    if(AT_SUCCESS != ulResult)
    {
        return ulResult;
    }

    stSetCPinReq.usPara0Len = gastAtParaList[0].usParaLen;
    stSetCPinReq.usPara1Len = gastAtParaList[1].usParaLen;
    PS_MEM_CPY((VOS_VOID*)stSetCPinReq.aucPara0,(VOS_VOID*)gastAtParaList[0].aucPara,stSetCPinReq.usPara0Len);
    PS_MEM_CPY((VOS_VOID*)stSetCPinReq.aucPara1,(VOS_VOID*)gastAtParaList[1].aucPara,stSetCPinReq.usPara1Len);


    /* 通过TAF_MSG_MMA_SET_PIN 消息来设置CPIN */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      At_GetOpId(),
                                      TAF_MSG_MMA_SET_PIN,
                                      (VOS_VOID*)&stSetCPinReq,
                                      sizeof(stSetCPinReq),
                                      I0_WUEPS_PID_MMA);

    if(AT_SUCCESS == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MSG_MMA_SET_PIN;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;

}

TAF_UINT32 At_SetCpin2Para(TAF_UINT8 ucIndex)
{
    TAF_PH_PIN_DATA_STRU stPinData;
    TAF_UINT32           ulResult;

    /* 参数检查 */
    ulResult = At_CheckCpinPara();
    if(AT_SUCCESS != ulResult)
    {
        return ulResult;
    }

    /* 初始化 */
    /* PS_MEM_SET(&stPinData,0x00,sizeof(stPinData)); */
    SI_PB_MemSet(sizeof(stPinData), 0xFF, (VOS_UINT8*)&stPinData);

    /* 设置<pin> */
    if(0 != gastAtParaList[0].usParaLen)
    {
        PS_MEM_CPY((TAF_VOID*)stPinData.aucOldPin,(TAF_VOID*)gastAtParaList[0].aucPara,gastAtParaList[0].usParaLen);
    }

    /* 设置<newpin> */
    if(0 != gastAtParaList[1].usParaLen)
    {
        PS_MEM_CPY((TAF_VOID*)stPinData.aucNewPin,(TAF_VOID*)gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen);
    }

    /* 设置CmdType */
    if(0 == gastAtParaList[1].usParaLen)
    {
        stPinData.PinType = TAF_SIM_PIN2;
        stPinData.CmdType = TAF_PIN_VERIFY;
    }
    else
    {
        stPinData.PinType = TAF_SIM_PUK2;
        stPinData.CmdType = TAF_PIN_UNBLOCK;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId, 0,&stPinData))
    {
        /* 设置当前操作类型 */
        if(0 == gastAtParaList[1].usParaLen)
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPIN2_VERIFY_SET;
        }
        else
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPIN2_UNBLOCK_SET;
        }
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 Prototype      : At_SetIccidPara
 Description    : ^ICCID
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetIccidPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参照标竿，直接返回AT_OK */
    return AT_OK;
}


TAF_UINT32 At_SetPNNPara(TAF_UINT8 ucIndex)
{
    TAF_PH_QRY_USIM_INFO_STRU           stUsimInfo;

    PS_MEM_SET(&stUsimInfo, 0x00, sizeof(stUsimInfo));

    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if((0 != gastAtParaList[0].ulParaValue)
        &&(1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    if (0 == gastAtParaList[0].ulParaValue)
    {
        stUsimInfo.Icctype = TAF_PH_ICC_SIM;
    }
    else
    {
        stUsimInfo.Icctype = TAF_PH_ICC_USIM;
    }
    stUsimInfo.enEfId = TAF_PH_PNN_FILE;
    /*linear fixed file only support 254 reocrd*/
    if(0 != gastAtParaList[1].usParaLen)
    {
        if( (gastAtParaList[1].ulParaValue > 0xFE)
         || (gastAtParaList[1].ulParaValue == 0))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        else
        {
            stUsimInfo.ulRecNum = gastAtParaList[1].ulParaValue;
        }
    }
    else
    {
        stUsimInfo.ulRecNum = 0;

        /* 在读取PNN列表的时候进行分次下发，每次只读取TAF_MMA_PNN_INFO_MAX_NUM个，以防止A核申请内存失败造成复位 */
        stUsimInfo.stPnnQryIndex.usPnnCurrIndex = 0;   /* 第一次下发的时候，Index为0，后面在收到TAF的CNF消息的时候再进行分批次下发 */
        stUsimInfo.stPnnQryIndex.usPnnNum       = TAF_MMA_PNN_INFO_MAX_NUM;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == TAF_QryUsimInfo(gastAtClientTab[ucIndex].usClientId,0,&stUsimInfo))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PNN_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32 At_SetCPNNPara(TAF_UINT8 ucIndex)
{
    TAF_PH_ICC_TYPE                     IccType;

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if((0 != gastAtParaList[0].ulParaValue)
        &&(1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    if (0 == gastAtParaList[0].ulParaValue)
    {
        IccType = TAF_PH_ICC_SIM;
    }
    else
    {
        IccType = TAF_PH_ICC_USIM;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == TAF_QryCpnnInfo(gastAtClientTab[ucIndex].usClientId,0,IccType))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPNN_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32 At_SetOPLPara (TAF_UINT8 ucIndex)
{
    TAF_PH_QRY_USIM_INFO_STRU           stUsimInfo;

    /* 参数检查 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if((0 != gastAtParaList[0].ulParaValue)
        &&(1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 == gastAtParaList[0].ulParaValue)
    {
        stUsimInfo.Icctype = TAF_PH_ICC_SIM;
    }
    else
    {
        stUsimInfo.Icctype = TAF_PH_ICC_USIM;
    }
    stUsimInfo.enEfId = TAF_PH_OPL_FILE;
    /*linear fixed file only support 254 reocrd*/
    if((0 != gastAtParaList[1].usParaLen))
    {
        if((gastAtParaList[1].ulParaValue > 0xFE)
        || (gastAtParaList[1].ulParaValue == 0))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        else
        {
            stUsimInfo.ulRecNum = gastAtParaList[1].ulParaValue;
        }
    }
    else
    {
        stUsimInfo.ulRecNum = 0;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == TAF_QryUsimInfo(gastAtClientTab[ucIndex].usClientId,0,&stUsimInfo))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PNN_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }

}
/*****************************************************************************
 Prototype      : At_SetCardlockPara
 Description    : ^CARDLOCK
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2008-03-15
    Author      : luojian
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCardlockPara(TAF_UINT8 ucIndex)
{
    TAF_ME_PERSONALISATION_DATA_STRU    stMePersonalisationData;

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <unlock_code>必须为长度为8的数字字符串 */
    if( TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX == gastAtParaList[0].usParaLen)
    {
        if ( AT_FAILURE == At_CheckNumString(gastAtParaList[0].aucPara,
                                             TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX)
           )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        else
        {
            PS_MEM_SET( &stMePersonalisationData,0,sizeof(TAF_ME_PERSONALISATION_DATA_STRU) );
            /* 复制用户解锁码 */
            PS_MEM_CPY( stMePersonalisationData.aucOldPwd,
                        gastAtParaList[0].aucPara,
                        TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);
        }
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    /* 安全命令类型为校验密码 */
    stMePersonalisationData.CmdType        = TAF_ME_PERSONALISATION_VERIFY;
    /* 锁卡操作为锁网络 */
    stMePersonalisationData.MePersonalType = TAF_OPERATOR_PERSONALISATION;
     /* 执行命令操作 */
    if(AT_SUCCESS == Taf_MePersonalisationHandle(gastAtClientTab[ucIndex].usClientId, 0,&stMePersonalisationData))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CARD_LOCK_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_FillSsBsService(
    TAF_SS_ERASESS_REQ_STRU            *pstSSPara,
    AT_PARSE_PARA_TYPE_STRU            *pstAtPara
)
{
    if ( (0 == pstAtPara->usParaLen )
      || (AT_CLCK_PARA_CLASS_ALL == pstAtPara->ulParaValue) )
    {
        /* 默认值 */
        pstSSPara->OP_BsService = 0;
    }
    else
    {
        if(AT_FAILURE == At_SsPrint2Class(&(pstSSPara->BsService),
                                          (VOS_UINT8)pstAtPara->ulParaValue))
        {
            return VOS_ERR;
        }
        pstSSPara->OP_BsService = 1;
    }

    return VOS_OK;
}
VOS_UINT32 AT_FillSsNoRepCondTime(
    VOS_UINT8                           ucSsCode,
    AT_PARSE_PARA_TYPE_STRU            *pstAtPara,
    TAF_SS_REGISTERSS_REQ_STRU         *pstSSPara
)
{
    if ((TAF_ALL_FORWARDING_SS_CODE      == ucSsCode)
     || (TAF_ALL_COND_FORWARDING_SS_CODE == ucSsCode)
     || (TAF_CFNRY_SS_CODE               == ucSsCode))
    {
        if (pstAtPara->usParaLen)
        {
            pstSSPara->OP_NoRepCondTime = 1;
            pstSSPara->NoRepCondTime    = (TAF_SS_NOREPLYCONDITIONTIME)pstAtPara->ulParaValue;
        }
        else
        {
            /* 默认值 */
            pstSSPara->OP_NoRepCondTime = 1;
            pstSSPara->NoRepCondTime    = 20;
        }
    }

    return VOS_OK;
}


TAF_UINT32 At_SetCcwaPara(TAF_UINT8 ucIndex)
{
    TAF_SS_ERASESS_REQ_STRU             para;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 初始化 */
    PS_MEM_SET(&para,0x00,sizeof(para));

    /* 设置SsCode */
    para.SsCode = TAF_CW_SS_CODE;

    /* 设置<n> */
    if(0 != gastAtParaList[0].usParaLen)
    {
        pstSsCtx->ucCcwaType = (AT_CCWA_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        pstSsCtx->ucCcwaType = AT_CCWA_DISABLE_TYPE;
    }

    if(0 == gastAtParaList[1].usParaLen)
    {
        return AT_OK;
    }

    /* 设置<class> */
    if (VOS_OK != AT_FillSsBsService(&para, &gastAtParaList[2]))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    switch(gastAtParaList[1].ulParaValue)
    {
    case 0: /* disable */
        if(AT_SUCCESS == TAF_DeactivateSSReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCWA_DISABLE;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }

    case 1: /* enable */
        if(AT_SUCCESS == TAF_ActivateSSReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCWA_ENABLE;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }

    default: /* query status */
        gastAtClientTab[ucIndex].ulTemp = gastAtParaList[2].ulParaValue;
        para.OP_BsService = 0;
        if(AT_SUCCESS == TAF_InterrogateSSReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
        {
            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCWA_QUERY;
            return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
        }
        else
        {
            return AT_ERROR;
        }
    }
}
VOS_UINT32 AT_CheckCcfcParaValid( VOS_VOID  )
{

    /* 参数检查 */
    if ( (0 == gastAtParaList[0].usParaLen)
        || (0 == gastAtParaList[1].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 8 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果<number>过长，直接返回错误 */
    if ( gastAtParaList[2].usParaLen > TAF_SS_MAX_FORWARDED_TO_NUM_LEN )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果<subaddr>过长，直接返回错误 */
    if ( gastAtParaList[5].usParaLen > TAF_SS_MAX_FORWARDED_TO_SUBADDRESS_LEN )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 检查NUM号码输入不正确 */
    if ( 0 != gastAtParaList[2].usParaLen )
    {
        if( AT_FAILURE == At_CheckDialString(gastAtParaList[2].aucPara,
            gastAtParaList[2].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /* 检查SubAddress号码输入不正确 */
    if ( 0 != gastAtParaList[5].usParaLen )
    {
        if( AT_FAILURE == At_CheckDialString(gastAtParaList[5].aucPara,
            gastAtParaList[5].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_OK;
}


TAF_SS_CODE AT_GetSsCodeFromCfccPara( VOS_VOID  )
{
    TAF_SS_CODE                         ssCode;

    /* 设置SsCode */
    switch( gastAtParaList[0].ulParaValue )
    {
    case 0:
        ssCode = TAF_CFU_SS_CODE;
        break;

    case 1:
        ssCode = TAF_CFB_SS_CODE;
        break;

    case 2:
        ssCode = TAF_CFNRY_SS_CODE;
        break;

    case 3:
        ssCode = TAF_CFNRC_SS_CODE;
        break;

    case 4:
        ssCode = TAF_ALL_FORWARDING_SS_CODE;
        break;

    default:
        ssCode = TAF_ALL_COND_FORWARDING_SS_CODE;
        break;
    }

    return ssCode;
}
VOS_VOID AT_SetNumAttribute(
    TAF_SS_NUMTYPE                      *pNumType,
    TAF_UINT8                           *pucFwdToNum
    )
{
    /* +表示国际号码，在号码类型中已经指示，不需要拷贝到消息中 */
    if ('+' == gastAtParaList[2].aucPara[0])
    {
        PS_MEM_CPY((TAF_CHAR*)pucFwdToNum,
            (TAF_CHAR*)(gastAtParaList[2].aucPara+1),
            (VOS_SIZE_T)(gastAtParaList[2].usParaLen-1));
    }
    else
    {
        PS_MEM_CPY((TAF_CHAR*)pucFwdToNum,
            (TAF_CHAR*)gastAtParaList[2].aucPara,
            gastAtParaList[2].usParaLen);
    }

    if( gastAtParaList[3].usParaLen )
    {
        *pNumType = (TAF_SS_NUMTYPE)gastAtParaList[3].ulParaValue;
    }
    else
    {
        *pNumType = At_GetCodeType(gastAtParaList[2].aucPara[0]);
    }
}
VOS_VOID AT_SetSubAddrAttribute(
    TAF_SS_SUBADDRTYPE                  *pSubAddrType,
    TAF_UINT8                           *pucFwdToSubAddr
)
{
    PS_MEM_CPY((TAF_CHAR*)pucFwdToSubAddr,
        (TAF_CHAR*)gastAtParaList[5].aucPara,
        gastAtParaList[5].usParaLen);

    if( gastAtParaList[6].usParaLen )
    {
        *pSubAddrType = (TAF_UINT8)gastAtParaList[6].ulParaValue;
    }
    else
    {
        *pSubAddrType = 128;
    }

}


VOS_UINT32 AT_DeactivateSSReq(
    TAF_UINT8                           ucIndex,
    TAF_SS_ERASESS_REQ_STRU             *pstEraseReq
)
{
    if( AT_SUCCESS == TAF_DeactivateSSReq(gastAtClientTab[ucIndex].usClientId,
        0,
        pstEraseReq) )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCFC_DISABLE;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}


VOS_UINT32 AT_ActivateSSReq(
    TAF_UINT8                           ucIndex,
    TAF_SS_ERASESS_REQ_STRU             *pstActivateReq
)
{
    if( AT_SUCCESS == TAF_ActivateSSReq(gastAtClientTab[ucIndex].usClientId,
        0,
        pstActivateReq) )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCFC_ENABLE;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}


VOS_UINT32  AT_InterrogateSSReq(
    TAF_UINT8                           ucIndex,
    TAF_SS_ERASESS_REQ_STRU             *pstInterrogateReq
)
{
    gastAtClientTab[ucIndex].ulTemp = gastAtParaList[4].ulParaValue;

    if ( AT_SUCCESS == TAF_InterrogateSSReq(gastAtClientTab[ucIndex].usClientId,
        0,
        pstInterrogateReq) )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCFC_QUERY;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}


VOS_UINT32 AT_RegisterSSReq(
    TAF_UINT8                           ucIndex,
    TAF_SS_REGISTERSS_REQ_STRU          *pstRegisterReq
)
{
    if ( AT_SUCCESS == TAF_RegisterSSReq(gastAtClientTab[ucIndex].usClientId,
        0,
        pstRegisterReq) )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCFC_REGISTRATION;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}


VOS_UINT32 AT_EraseSSReq(
    TAF_UINT8                           ucIndex,
    TAF_SS_ERASESS_REQ_STRU             *pstEraseSSReq
)
{
    if ( AT_SUCCESS == TAF_EraseSSReq(gastAtClientTab[ucIndex].usClientId,
        0,
        pstEraseSSReq) )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCFC_ERASURE;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_ERROR;
}


TAF_UINT32 At_SetCcfcPara(TAF_UINT8 ucIndex)
{
    TAF_SS_REGISTERSS_REQ_STRU          para;
    TAF_SS_ERASESS_REQ_STRU             sndpara;
    VOS_UINT32                          ulRet;

    ulRet = AT_CheckCcfcParaValid();
    if ( AT_OK != ulRet )
    {
        return ulRet;
    }

    /* 初始化 */
    PS_MEM_SET(&para,0x00,sizeof(para));
    PS_MEM_SET(&sndpara,0x00,sizeof(sndpara));

    para.SsCode = AT_GetSsCodeFromCfccPara();

    /* 设置<number>,<type> */
    if ( 0 != gastAtParaList[2].usParaLen )
    {
        para.OP_FwdToNum = 1;
        para.OP_NumType  = 1;
        AT_SetNumAttribute(&para.NumType,para.aucFwdToNum);
    }
    else
    {
        para.OP_FwdToNum = 0;
        para.OP_NumType  = 0;
    }

    /* 设置<subaddr>,<satype> */
    if ( 0 != gastAtParaList[5].usParaLen )
    {
        para.OP_FwdToAddr = 1;
        para.OP_SubAddrType = 1;
        AT_SetSubAddrAttribute(&(para.SubAddrType), para.aucFwdToSubAddr);
    }
    else
    {
        para.OP_FwdToAddr   = 0;
        para.OP_SubAddrType = 0;
    }

    /* 设置<time> */
    AT_FillSsNoRepCondTime(para.SsCode, &gastAtParaList[7], &para);

    /* 设置<class> */
    if (VOS_OK != AT_FillSsBsService(&sndpara, &gastAtParaList[4]))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    sndpara.SsCode    = para.SsCode;

    if (VOS_TRUE == sndpara.OP_BsService)
    {
        para.OP_BsService               = sndpara.OP_BsService;
        para.BsService.BsType           = sndpara.BsService.BsType;
        para.BsService.BsServiceCode    = sndpara.BsService.BsServiceCode;
    }

    /* 执行命令操作 */
    switch(gastAtParaList[1].ulParaValue)
    {
    case 0: /* <mode>: disable */
        ulRet = AT_DeactivateSSReq(ucIndex,&sndpara);
        break;

    case 1: /* <mode>: enable */
        ulRet = AT_ActivateSSReq(ucIndex,&sndpara);
        break;

    case 2: /* <mode>: query status */
        ulRet = AT_InterrogateSSReq(ucIndex,&sndpara);
        break;

    case 3: /* <mode>: registration */
        ulRet = AT_RegisterSSReq(ucIndex,&para);
        break;

    default:    /* <mode>: erasure */
        ulRet = AT_EraseSSReq(ucIndex,&sndpara);
        break;
    }

    return ulRet;
}


/*****************************************************************************
 Prototype      : At_SetCmlckPara
 Description    : +CLCK=<fac>,<action>[,<passwd>[,<data>]]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2007-10-06
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCmlckPara (TAF_UINT8 ucIndex)
{
    TAF_UINT32 ulresult;


    /* 参数检查 */
    if((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))

    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 4)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch(gastAtParaList[0].ulParaValue)
    {
        case AT_CMLCK_PS_TYPE:
        {
            TAF_ME_PERSONALISATION_DATA_STRU stMePersonalisationData;

            if(TAF_PH_PINCODELENMAX < gastAtParaList[2].usParaLen)
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 初始化 */
            PS_MEM_SET(&stMePersonalisationData,0x00,sizeof(TAF_ME_PERSONALISATION_DATA_STRU));

            /* 设置<passwd> */
            if(0 != gastAtParaList[2].usParaLen)
            {
                PS_MEM_CPY((TAF_CHAR*)stMePersonalisationData.aucOldPwd,(VOS_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
            }

            /* 设置PinType */
            stMePersonalisationData.MePersonalType = TAF_SIM_PERSONALISATION;

            if(0 != gastAtParaList[3].usParaLen)
            {
                ulresult = At_AsciiNum2Num(stMePersonalisationData.unMePersonalisationSet.SimPersionalisationStr.aucSimPersonalisationStr,
                                           gastAtParaList[3].aucPara,
                                           gastAtParaList[3].usParaLen);
                if (AT_SUCCESS != ulresult)
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
                /*
                At_AsciiNum2HexString(gastAtParaList[3].aucPara,&gastAtParaList[3].usParaLen);
                PS_MEM_CPY((VOS_CHAR*)stMePersonalisationData.unMePersonalisationSet.aucSimPersonalisationStr,
                          (VOS_CHAR*)gastAtParaList[3].aucPara,
                          gastAtParaList[3].usParaLen);
                */
                stMePersonalisationData.unMePersonalisationSet.SimPersionalisationStr.DataLen= (VOS_UINT8)gastAtParaList[3].usParaLen;
            }


            /* 设置CmdType */
            switch(gastAtParaList[1].ulParaValue)
            {
            case 0:
                if ((15 < gastAtParaList[3].usParaLen) || (0 == gastAtParaList[3].usParaLen))
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
                stMePersonalisationData.CmdType = TAF_ME_PERSONALISATION_SET;

                break;

            case 1:
                stMePersonalisationData.CmdType = TAF_ME_PERSONALISATION_RETRIEVE;

                break;

            default:
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 执行命令操作 */
            if(AT_SUCCESS == Taf_MePersonalisationHandle(gastAtClientTab[ucIndex].usClientId, 0,&stMePersonalisationData))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_PIN_HANDLE;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }

        }

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }


}


VOS_UINT32 AT_CheckPinLength(
    VOS_UINT16                          usLen,
    VOS_UINT8                          *pucPin
)
{
    if (VOS_NULL_PTR == pucPin)
    {
        AT_ERR_LOG("AT_CheckPinLength():PIN is NULL PTR");
        return VOS_ERR;
    }

    if ((usLen > TAF_PH_PINCODELENMAX)
     || (usLen < TAF_PH_PINCODELENMIN))
    {
        AT_ERR_LOG("AT_CheckPinLength():PIN LENGTH is Error");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_CheckPinValue(
    VOS_UINT16                          usLen,
    VOS_UINT8                          *pucPin
)
{
    VOS_UINT8                           i;

    if (VOS_NULL_PTR == pucPin)
    {
        AT_ERR_LOG("AT_CheckPinValue(): PIN is NULL PTR");
        return VOS_ERR;
    }

    for (i = 0; i < usLen ; i++ )
    {
        if ( (pucPin[i] >'9' )
           ||(pucPin[i] <'0'))
        {
            AT_ERR_LOG("AT_CheckPinValue(): PIN Value is Error");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_CheckPinAvail(
    VOS_UINT16                          usLen,
    VOS_UINT8                          *pucPin
)
{
    if (VOS_OK != AT_CheckPinLength(usLen, pucPin))
    {
        AT_ERR_LOG("AT_CheckPinAvail(): PIN Length is Error");
        return VOS_ERR;
    }
    else
    {
        if (VOS_OK != AT_CheckPinValue(usLen, pucPin))
        {
            AT_ERR_LOG("AT_CheckPinAvail():PIN Value is Error");
            return VOS_ERR;
        }
    }
    return VOS_OK;
}


TAF_UINT32 At_SetClckPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 4)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<fac> */
    switch(gastAtParaList[0].ulParaValue)
    {
    case AT_CLCK_P2_TYPE:
        return AT_CME_INCORRECT_PARAMETERS;

    case AT_CLCK_SC_TYPE:
        {
            TAF_PH_PIN_DATA_STRU stPinData;

            if (TAF_PH_PINCODELENMAX < gastAtParaList[2].usParaLen)
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 初始化 */
            PS_MEM_SET(&stPinData, 0xFF, sizeof(stPinData));

            /* 设置<passwd> */
            if(0 != gastAtParaList[2].usParaLen)
            {
                PS_MEM_CPY((TAF_CHAR*)stPinData.aucOldPin,(TAF_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
            }

            stPinData.PinType = TAF_SIM_PIN;

            /* 设置CmdType */
            switch(gastAtParaList[1].ulParaValue)
            {
            case 0:
                stPinData.CmdType = TAF_PIN_DISABLE;
                if (VOS_OK != AT_CheckPinAvail(gastAtParaList[2].usParaLen, gastAtParaList[2].aucPara))
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
                break;

            case 1:
                stPinData.CmdType = TAF_PIN_ENABLE;
                if (VOS_OK != AT_CheckPinAvail(gastAtParaList[2].usParaLen, gastAtParaList[2].aucPara))
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
                break;

            default:
                stPinData.CmdType = TAF_PIN_QUERY;
                break;
            }

            /* 执行命令操作 */
            if(AT_SUCCESS == Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId, 0,&stPinData))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_PIN_HANDLE;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        }
    case AT_CLCK_FD_TYPE:
        {

            TAF_UINT8   aucPin[TAF_PH_PINCODELENMAX];
            TAF_UINT32  CmdType;
            TAF_UINT32  Result;

            if(TAF_PH_PINCODELENMAX < gastAtParaList[2].usParaLen)
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 初始化 */
            PS_MEM_SET(aucPin, 0xFF, sizeof(aucPin));

            /* 设置<passwd> */
            if(0 != gastAtParaList[2].usParaLen)
            {
                PS_MEM_CPY((TAF_CHAR*)aucPin,(TAF_CHAR*)gastAtParaList[2].aucPara, gastAtParaList[2].usParaLen);
            }

            CmdType = gastAtParaList[1].ulParaValue;

            /* 执行命令操作，当前只有FDN相关的操作，AT命令并不支持BDN功能 */
            if(SI_PIH_FDN_BDN_ENABLE == CmdType)
            {
                Result = SI_PIH_FdnEnable(gastAtClientTab[ucIndex].usClientId, 0, aucPin);
            }
            else if(SI_PIH_FDN_BDN_DISABLE == CmdType)
            {
                Result = SI_PIH_FdnDisable(gastAtClientTab[ucIndex].usClientId, 0, aucPin);
            }
            else
            {
                Result = SI_PIH_FdnBdnQuery(gastAtClientTab[ucIndex].usClientId, 0, SI_PIH_FDN_QUERY);
            }

            if(AT_SUCCESS == Result)
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_PIN_HANDLE;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        }

    case AT_CLCK_PS_TYPE:
        {
            TAF_ME_PERSONALISATION_DATA_STRU stMePersonalisationData;

            if(TAF_PH_PINCODELENMAX < gastAtParaList[2].usParaLen)
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 初始化 */
            PS_MEM_SET(&stMePersonalisationData,0x00,sizeof(TAF_ME_PERSONALISATION_DATA_STRU));

            /* 设置<passwd> */
            if(0 != gastAtParaList[2].usParaLen)
            {
                PS_MEM_CPY((VOS_CHAR*)stMePersonalisationData.aucOldPwd,(VOS_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
            }

            /* 设置PinType */
            stMePersonalisationData.MePersonalType = TAF_SIM_PERSONALISATION;


            /* 设置CmdType */
            switch(gastAtParaList[1].ulParaValue)
            {
            case 0:
                stMePersonalisationData.CmdType = TAF_ME_PERSONALISATION_DEACTIVE;
                break;

            case 1:
                stMePersonalisationData.CmdType = TAF_ME_PERSONALISATION_ACTIVE;
                break;

            default:
                stMePersonalisationData.CmdType = TAF_ME_PERSONALISATION_QUERY;
                break;
            }

            /* 执行命令操作 */
            if(AT_SUCCESS == Taf_MePersonalisationHandle(gastAtClientTab[ucIndex].usClientId, 0,&stMePersonalisationData))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_PIN_HANDLE;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        }



    default:
        {
            TAF_SS_ERASESS_REQ_STRU para;

            /* 初始化 */
            PS_MEM_SET(&para,0x00,sizeof(para));

            /* 设置<password> */
            if (gastAtParaList[2].usParaLen > 0)
            {
                if (TAF_SS_MAX_PASSWORD_LEN != gastAtParaList[2].usParaLen)
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }

                para.OP_Password = VOS_TRUE;
                PS_MEM_CPY((VOS_CHAR *)para.aucPassword,
                           (VOS_CHAR *)gastAtParaList[2].aucPara,
                           gastAtParaList[2].usParaLen);
            }


            /* 设置SsCode */
            At_SsPrint2Code(&para.SsCode,(TAF_UINT8)gastAtParaList[0].ulParaValue);

            /* 设置<class> */
            if (VOS_OK != AT_FillSsBsService(&para, &gastAtParaList[3]))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 执行命令操作 */
            switch(gastAtParaList[1].ulParaValue)
            {
            case 0: /* <mode>: unlock */
                if(AT_SUCCESS == TAF_DeactivateSSReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
                {
                    /* 设置当前操作类型 */
                    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_LOCK;
                    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
                }
                else
                {
                    return AT_ERROR;
                }

            case 1: /* <mode>: lock */
                if(AT_SUCCESS == TAF_ActivateSSReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
                {
                    /* 设置当前操作类型 */
                    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_UNLOCK;
                    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
                }
                else
                {
                    return AT_ERROR;
                }

            default: /* <mode>: query status */
                para.OP_BsService = 0;
                gastAtClientTab[ucIndex].ulTemp = gastAtParaList[3].ulParaValue;
                if(AT_SUCCESS == TAF_InterrogateSSReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
                {
                    /* 设置当前操作类型 */
                    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLCK_QUERY;
                    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
                }
                else
                {
                    return AT_ERROR;
                }
            }
        }
    }
}
TAF_UINT32 At_SetCtfrPara(TAF_UINT8 ucIndex)
{
    MN_CALL_SUPS_PARAM_STRU             stCallSupsParam;
    VOS_UINT32                          ulRst;

    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 4)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果<number>过长，直接返回错误 */
    if(gastAtParaList[0].usParaLen > (MN_CALL_MAX_BCD_NUM_LEN * 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果<subaddr>过长，直接返回错误 */
    if(gastAtParaList[2].usParaLen > (MN_CALL_MAX_BCD_NUM_LEN * 2))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen]='\0';

    ulRst = AT_AsciiNumberToBcd((VOS_CHAR *)gastAtParaList[0].aucPara,
                                stCallSupsParam.stRedirectNum.aucBcdNum,
                                &stCallSupsParam.stRedirectNum.ucNumLen);
    if (MN_ERR_NO_ERROR != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stCallSupsParam.callId = 0;
    stCallSupsParam.enCallSupsCmd = MN_CALL_SUPS_CMD_DEFLECT_CALL;
    stCallSupsParam.stRedirectNum.enNumType = (TAF_UINT8)gastAtParaList[1].ulParaValue;

    if (AT_SUCCESS == MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId, 0,&stCallSupsParam))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CTFR_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 At_SetCusdPara(VOS_UINT8 ucIndex)
{
    TAF_SS_PROCESS_USS_REQ_STRU        *pPara;
    VOS_UINT32                          ulRet;
    TAF_SS_DATA_CODING_SCHEME           DatacodingScheme;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx;


    ulRet =  AT_ERROR;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 == gastAtParaList[2].usParaLen)
    {
        /* 默认为7bit编码 */
        DatacodingScheme = TAF_SS_7bit_LANGUAGE_UNSPECIFIED;
    }
    else
    {
        DatacodingScheme = (TAF_SS_DATA_CODING_SCHEME)gastAtParaList[2].ulParaValue;

        /*产品线要求支持uss2的ussd*/
    }

    /* 如果<str>号码过长，直接返回错误 */
    if(AT_FAILURE == At_CheckUssdNumLen(ucIndex, DatacodingScheme, gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    pPara    = (TAF_SS_PROCESS_USS_REQ_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_SS_PROCESS_USS_REQ_STRU));
    if (TAF_NULL_PTR == pPara)
    {
        return AT_ERROR;
    }

    /* 初始化 */
    PS_MEM_SET(pPara,0x00,sizeof(TAF_SS_PROCESS_USS_REQ_STRU));

    /* 设置<n> */
    if(0 != gastAtParaList[0].usParaLen)
    {
        if(2 != gastAtParaList[0].ulParaValue)
        {
            pPara->enCusdType    = (TAF_SS_CUSD_TYPE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
        }
    }
    else
    {
        pPara->enCusdType    = TAF_SS_CUSD_TYPE_DISABLE;
    }

    /* <n>: cancel session */
    if(2 == gastAtParaList[0].ulParaValue)
    {
        if(AT_SUCCESS == TAF_SsReleaseComplete(gastAtClientTab[ucIndex].usClientId, 0))
        {
            ulRet = AT_OK;

        }
        else
        {
            ulRet = AT_ERROR;
        }
    }
    else
    {
        /*  AT+CUSD=0时，也需要给C核发消息，但不用等C核回复 */
        if(0 == gastAtParaList[1].usParaLen)
        {
            PS_MEM_SET(&(pPara->UssdStr), 0, sizeof(pPara->UssdStr));
        }
        else
        {

            PS_MEM_CPY(pPara->UssdStr.aucUssdStr,gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen);
            pPara->UssdStr.usCnt = gastAtParaList[1].usParaLen;

            pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

            if ((0 == pstSsCtx->usUssdTransMode) && (AT_CSCS_UCS2_CODE == gucAtCscsType))
            {
                if ( (TAF_SS_7bit_LANGUAGE_UNSPECIFIED == DatacodingScheme)
                   ||(TAF_SS_8bit == DatacodingScheme))
                {
                    if(AT_FAILURE == At_UnicodePrintToAscii(pPara->UssdStr.aucUssdStr,&pPara->UssdStr.usCnt))
                    {
                        PS_MEM_FREE(WUEPS_PID_AT, pPara);
                        return AT_CME_INCORRECT_PARAMETERS;
                    }
                }
            }

            if (TAF_SS_UCS2 == DatacodingScheme)
            {
                if(AT_FAILURE == At_UnicodePrint2Unicode(pPara->UssdStr.aucUssdStr,&pPara->UssdStr.usCnt))
                {
                    PS_MEM_FREE(WUEPS_PID_AT, pPara);
                    return AT_CMS_INVALID_TEXT_MODE_PARAMETER;
                }
            }
        }
        /* 设置<dcs> */
        pPara->DatacodingScheme = DatacodingScheme;

        /* 执行命令操作 */
        if(AT_SUCCESS == TAF_ProcessUnstructuredSSReq(gastAtClientTab[ucIndex].usClientId, 0, pPara))
        {
            /*  AT+CUSD=0时，也需要给C核发消息，但不用等C核回复 */
            if(0 == gastAtParaList[1].usParaLen)
            {
                ulRet =  AT_OK;
            }
            else
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CUSD_REQ;

                /* 返回命令处理挂起状态 */
                ulRet =  AT_WAIT_ASYNC_RETURN;
            }
        }
        else
        {
            ulRet =  AT_ERROR;
        }

    }

    PS_MEM_FREE(WUEPS_PID_AT, pPara);
    return ulRet;
}
TAF_UINT32 At_SetCpwdPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32   i;
    /* 参数检查 */
    if((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen) || (0 == gastAtParaList[2].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<fac> */
    switch(gastAtParaList[0].ulParaValue)
    {
    case AT_CLCK_P2_TYPE:
    case AT_CLCK_SC_TYPE:
        {
            TAF_PH_PIN_DATA_STRU stPinData;

            if((TAF_PH_PINCODELENMAX < gastAtParaList[1].usParaLen) || (TAF_PH_PINCODELENMAX < gastAtParaList[2].usParaLen)
             ||(TAF_PH_PINCODELENMIN > gastAtParaList[1].usParaLen) || (TAF_PH_PINCODELENMIN > gastAtParaList[2].usParaLen))
            {
                return AT_ERROR;
            }

            /* 初始化 */
            PS_MEM_SET(&stPinData, 0xFF, sizeof(stPinData));

            /* 设置<pin> */
            if(0 != gastAtParaList[1].usParaLen)
            {
                for(i=0; i<gastAtParaList[1].usParaLen; i++)
                {
                    if((gastAtParaList[1].aucPara[i] > '9' )||(gastAtParaList[1].aucPara[i] < '0'))
                    {
                        return AT_CME_INCORRECT_PARAMETERS;
                    }
                }

                PS_MEM_CPY((TAF_VOID*)stPinData.aucOldPin,(TAF_VOID*)gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen);
            }

            /* 设置<newpin> */
            if(0 != gastAtParaList[2].usParaLen)
            {
                for(i=0; i<gastAtParaList[2].usParaLen; i++)
                {
                    if((gastAtParaList[2].aucPara[i] > '9' )||(gastAtParaList[2].aucPara[i] < '0'))
                    {
                        return AT_CME_INCORRECT_PARAMETERS;
                    }
                }

                PS_MEM_CPY((TAF_VOID*)stPinData.aucNewPin,(TAF_VOID*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
            }
            /* 设置PinType */
            if(AT_CLCK_P2_TYPE == gastAtParaList[0].ulParaValue)
            {
                stPinData.PinType = TAF_SIM_PIN2;
            }
            else
            {
                stPinData.PinType = TAF_SIM_PIN;
            }

            /* 设置CmdType */
            stPinData.CmdType = TAF_PIN_CHANGE;

            /* 执行命令操作 */
            if(AT_SUCCESS == Taf_PhonePinHandle(gastAtClientTab[ucIndex].usClientId, 0,&stPinData))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPWD_CHANGE_PIN;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        }

    case AT_CLCK_PS_TYPE:
        {
            TAF_ME_PERSONALISATION_DATA_STRU stMePersonalisationData;

            if((TAF_PH_PINCODELENMAX < gastAtParaList[1].usParaLen) || (TAF_PH_PINCODELENMAX < gastAtParaList[2].usParaLen))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 初始化 */
            PS_MEM_SET(&stMePersonalisationData,0x00,sizeof(stMePersonalisationData));

            /* 设置<oldpwd> */
            PS_MEM_CPY((VOS_VOID*)stMePersonalisationData.aucOldPwd,(VOS_VOID*)gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen);

            /* 设置<newpwd> */
            PS_MEM_CPY((VOS_CHAR*)stMePersonalisationData.aucNewPwd,(VOS_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);

            /* 设置MePersonalType */
            stMePersonalisationData.MePersonalType = TAF_SIM_PERSONALISATION;

            /* 设置CmdType */
            stMePersonalisationData.CmdType = TAF_ME_PERSONALISATION_PWD_CHANGE;

            /* 执行命令操作 */
            if(AT_SUCCESS == Taf_MePersonalisationHandle(gastAtClientTab[ucIndex].usClientId, 0,&stMePersonalisationData))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPWD_CHANGE_PIN;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        }


    default:
        {
            TAF_SS_REGPWD_REQ_STRU para;

            if((TAF_SS_MAX_PASSWORD_LEN < gastAtParaList[1].usParaLen) || (TAF_SS_MAX_PASSWORD_LEN < gastAtParaList[2].usParaLen))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* 初始化 */
            PS_MEM_SET(&para,0x00,sizeof(para));

            /* 设置<oldpwd> */
            PS_MEM_CPY((TAF_CHAR*)para.aucOldPwdStr,(TAF_CHAR*)gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen);

            /* 设置<newpwd> */
            PS_MEM_CPY((TAF_CHAR*)para.aucNewPwdStr,(TAF_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);

            /* 设置<newpwd> */
            PS_MEM_CPY((TAF_CHAR*)para.aucNewPwdStrCnf,(TAF_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);

            /* 设置SsCode */
            At_SsPrint2Code(&para.SsCode,(TAF_UINT8)gastAtParaList[0].ulParaValue);

            /* 执行命令操作 */
            if(AT_SUCCESS == TAF_RegisterPasswordReq(gastAtClientTab[ucIndex].usClientId, 0,&para))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPWD_SET;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        }
    }
}
TAF_VOID At_SetDialIGPara(TAF_UINT8 ucIndex,TAF_UINT8 ucParaIndex,TAF_UINT8 *pClir, MN_CALL_CUG_CFG_STRU *pstCcug)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /*->A32D08138*/
    /* 默认情况是使用CCA设置的默认值 */

    *pClir   = pstSsCtx->ucClirType;
    *pstCcug = pstSsCtx->stCcugCfg;
    /*<-A32D08138*/

    switch(gastAtParaList[ucParaIndex].aucPara[0])
    {
    case 'i':
        *pClir = MN_CALL_CLIR_SUPPRESS;
        break;

    case 'I':
        *pClir = MN_CALL_CLIR_INVOKE;
        break;

    case 'g':
    case 'G':
        pstCcug->bEnable = 1;
        break;

    default:
        break;
    }

    switch(gastAtParaList[ucParaIndex + 1].aucPara[0])
    {
    case 'g':
    case 'G':
        pstCcug->bEnable = 1;
        break;

    default:
        break;
    }
}
TAF_UINT32 At_SetDialGprsPara(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           ucCid,
    TAF_GPRS_ACTIVE_TYPE_UINT8          enActiveType
)
{
    TAF_PPP_REQ_CONFIG_INFO_STRU        stPppReqConfigInfo;
    VOS_UINT32                          ulErrInfo;
    VOS_UINT16                          usPppId = 0;
    VOS_UINT8                           aucEventInfo[4];

    PS_MEM_SET(&stPppReqConfigInfo, 0x00, sizeof(TAF_PPP_REQ_CONFIG_INFO_STRU));

    /* 进行激活操作 */
    if(TAF_IP_ACTIVE_TE_PPP_MT_PPP_TYPE == enActiveType)    /* MT中启动PPP */
    {
        gastAtClientTab[ucIndex].ucCid = ucCid;   /* 保存当前指定的cid */

        if(AT_SUCCESS == Ppp_CreatePppReq(&usPppId))    /* TE中启动了PPP，MT中启动PPP */
        {
            /*EVENT-At_SetDialGprsPara: usPppId/index*/
            aucEventInfo[0] = (TAF_UINT8)(usPppId >> 8);
            aucEventInfo[1] = (TAF_UINT8)usPppId;
            aucEventInfo[2] = ucIndex;
            AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DCE_CREATE_PPP,
                           aucEventInfo, sizeof(aucEventInfo));

            gastAtPppIndexTab[usPppId] = ucIndex;       /* 记录PPP id和Index的对应关系 */
            gastAtClientTab[ucIndex].usPppId = usPppId; /* 保存PPP id */

            At_SetMode(ucIndex,AT_DATA_MODE,AT_PPP_DATA_MODE);   /* 切换为PPP数传状态 */

            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_PPP_CALL_SET;

            return AT_CONNECT;
        }
        else
        {
            ulErrInfo = TAF_OM_GREENCHANNEL_PS_CREATE_PPP_REQ_ERR;
            OM_AppGreenChannel(OM_GREEN_FUNC, TAF_OM_GREEN_CHANNEL_PS, (VOS_UINT8 *)&ulErrInfo, sizeof(ulErrInfo));
            return AT_ERROR;
        }
    }
    else
    {
        stPppReqConfigInfo.stAuth.enAuthType = TAF_PDP_AUTH_TYPE_NONE;
        stPppReqConfigInfo.stIPCP.usIpcpLen  = 0;

        /* 创建透传的PPP实体 */
        if (AT_SUCCESS != Ppp_CreateRawDataPppReq(&usPppId))
        {

            ulErrInfo = TAF_OM_GREENCHANNEL_PS_CREATE_RAW_DATA_PPP_REQ_ERR;
            OM_AppGreenChannel(OM_GREEN_FUNC, TAF_OM_GREEN_CHANNEL_PS, (VOS_UINT8 *)&ulErrInfo, sizeof(ulErrInfo));
            return AT_ERROR;
        }

        if ( VOS_OK != TAF_PS_PppDialOrig(WUEPS_PID_AT,
                                          gastAtClientTab[ucIndex].usClientId,
                                          0,
                                          ucCid,
                                          &stPppReqConfigInfo) )
        {
            PPP_RcvAtCtrlOperEvent(usPppId, PPP_AT_CTRL_REL_PPP_RAW_REQ);

            /*向PPP发送HDLC去使能操作*/
            PPP_RcvAtCtrlOperEvent(usPppId, PPP_AT_CTRL_HDLC_DISABLE);
            return AT_ERROR;
        }

        gastAtPppIndexTab[usPppId]          = ucIndex;  /* 记录PPP id和Index的对应关系 */
        gastAtClientTab[ucIndex].usPppId    = usPppId;  /* 保存PPP id */

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_IP_CALL_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
}



TAF_UINT32 At_SetDial98Para(TAF_UINT8 ucIndex)
{
    TAF_ATD_PARA_STRU                   stAtdPara;

    /* 初始化 */
    PS_MEM_SET(&stAtdPara, 0x00, sizeof(TAF_ATD_PARA_STRU));

    /* CID存在 */
    if (0 != gastAtParaList[4].usParaLen)
    {
        if(AT_FAILURE == At_Auc2ul(gastAtParaList[4].aucPara,gastAtParaList[4].usParaLen,&gastAtParaList[4].ulParaValue))
        {
            return AT_ERROR;
        }

        if(gastAtParaList[4].ulParaValue > 0xFF)   /* 需要检查，否则过界 */
        {
            return AT_ERROR;
        }

        /* 获取当前用户输入的CID */
        stAtdPara.ucCid = (VOS_UINT8)gastAtParaList[4].ulParaValue;
    }
    else
    {
        /* 设置默认CID */
        stAtdPara.ucCid = AT_DIAL_DEFAULT_CID;
    }

    /* 获取GPRS激活类型 */
    if ( VOS_OK != TAF_PS_GetGprsActiveType(WUEPS_PID_AT,
                                            gastAtClientTab[ucIndex].usClientId,
                                            0,
                                            &stAtdPara) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_GPRS_SET;

    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 At_SetDial99Para(VOS_UINT8 ucIndex)
{
    VOS_INT32                           lParaPPPFlg;
    VOS_INT32                           lParaNULLFlg;
    VOS_UINT8                           ucTmp;
    VOS_UINT8                           ucStar;
    VOS_UINT8                           ucL2pIndex;
    VOS_UINT8                           ucCidIndex;
    TAF_ATD_PARA_STRU                   stAtdPara;

    /* 初始化 */
    ucTmp      = 0;
    ucStar     = 0;
    ucL2pIndex = 0;
    ucCidIndex = 0;
    PS_MEM_SET(&stAtdPara, 0x00, sizeof(TAF_ATD_PARA_STRU));

    /* 解析参数列表 */
    for (ucTmp = 1; ucTmp < gucAtParaIndex; ucTmp++)
    {
        if ('*' == gastAtParaList[ucTmp].aucPara[0])
        {
            ucStar++;
        }
        else
        {
            switch (ucStar)
            {
                case 1:
                case 2:
                    break;

                case 3:
                    ucL2pIndex = ucTmp;
                    break;

                case 4:
                    ucCidIndex = ucTmp;
                    break;

                default:
                    return AT_ERROR;
            }
        }
    }

    /* 获取CID: 不存在CID参数时, 取默认值1 */
    if (0 != ucCidIndex)
    {
        if (AT_FAILURE == At_Auc2ul(gastAtParaList[ucCidIndex].aucPara,gastAtParaList[ucCidIndex].usParaLen,&gastAtParaList[ucCidIndex].ulParaValue))
        {
            return AT_ERROR;
        }

        /* 需要检查，否则过界 */
        if (gastAtParaList[ucCidIndex].ulParaValue > 0xFF)
        {
            return AT_ERROR;
        }

        stAtdPara.ucCid = (VOS_UINT8)gastAtParaList[ucCidIndex].ulParaValue;
    }
    else
    {
        stAtdPara.ucCid = AT_DIAL_DEFAULT_CID;
    }

    /* 获取L2P协议类型: 兼容不支持的协议类型 */
    if (0 != ucL2pIndex)
    {
        lParaPPPFlg =  VOS_StrCmp((TAF_CHAR*)gastAtParaList[ucL2pIndex].aucPara,"1");
        lParaNULLFlg = VOS_StrCmp((TAF_CHAR*)gastAtParaList[ucL2pIndex].aucPara,"0");

        if ( (0 == VOS_StrCmp((TAF_CHAR*)gastAtParaList[ucL2pIndex].aucPara,"PPP"))
          || (0 == lParaPPPFlg) )
        {
            stAtdPara.bitOpL2p  = VOS_TRUE;
            stAtdPara.enL2p     = TAF_L2P_PPP;
        }
        else if ( (0 == VOS_StrCmp((TAF_CHAR*)gastAtParaList[ucL2pIndex].aucPara,"NULL"))
               || (0 == lParaNULLFlg) )
        {
            stAtdPara.bitOpL2p  = VOS_TRUE;
            stAtdPara.enL2p     = TAF_L2P_NULL;
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }

    /* 获取GPRS激活类型 */
    if ( VOS_OK != TAF_PS_GetGprsActiveType(WUEPS_PID_AT,
                                            gastAtClientTab[ucIndex].usClientId,
                                            0,
                                            &stAtdPara) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_GPRS_SET;

    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_FillCalledNumPara(
    VOS_UINT8                          *pucAtPara,
    VOS_UINT16                          usLen,
    MN_CALL_CALLED_NUM_STRU            *pstCalledNum
)
{
    VOS_UINT32                          ulRelt;

    /* 号码类型检查 */
    if ('+' == *pucAtPara)
    {
        /* 国际号码的第一个字符不需要编码,最多可以为81个字符 */
        if (usLen > (MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1))
        {
            return VOS_ERR;
        }

        *(pucAtPara + usLen) = '\0';
        pucAtPara  += 1;

        pstCalledNum->enNumType
            = (MN_CALL_IS_EXIT | (MN_CALL_TON_INTERNATIONAL << 4) | MN_CALL_NPI_ISDN);
    }
    else
    {
        /* 国内号码或未知号码最多可以为80个字符 */
        if (usLen > MN_CALL_MAX_CALLED_ASCII_NUM_LEN)
        {
            return VOS_ERR;
        }

        *(pucAtPara + usLen) = '\0';

        pstCalledNum->enNumType
            = (MN_CALL_IS_EXIT | (MN_CALL_TON_UNKNOWN << 4) | MN_CALL_NPI_ISDN);
    }

    ulRelt = AT_AsciiNumberToBcd((VOS_CHAR *)pucAtPara,
                                 pstCalledNum->aucBcdNum,
                                 &pstCalledNum->ucNumLen);

    if (MN_ERR_NO_ERROR != ulRelt)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


TAF_UINT32 At_SetDialNumPara(TAF_UINT8 ucIndex,MN_CALL_TYPE_ENUM_U8 CallType)
{
    MN_CALL_ORIG_PARAM_STRU             stOrigParam;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /*目前华为移动的AT命令的需求，应该只要满足一般的语音呼叫即可*/

    /* 初始化 */
    PS_MEM_SET(&stOrigParam,0x00,sizeof(stOrigParam));

    /* 设置<dial_string> */
    if (VOS_OK !=  AT_FillCalledNumPara(gastAtParaList[1].aucPara,
                         gastAtParaList[1].usParaLen,
                         &stOrigParam.stDialNumber))
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stOrigParam.enCallType = CallType;

    At_SetDialIGPara(ucIndex,2,&stOrigParam.enClirCfg,&stOrigParam.stCugCfg);

    stOrigParam.enCallMode = pstSsCtx->enCModType;

    stOrigParam.stDataCfg = pstSsCtx->stCbstDataCfg;

    /* 考虑数据卡的接口，这里的定义视频呼叫域为3GPP的 */
    if (MN_CALL_TYPE_VIDEO == CallType)
    {
        stOrigParam.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_3GPP;
    }
    else
    {
        stOrigParam.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_AUTO;
    }

    /* 遗留问题6、呼叫接口，需要增加设置I、G的参数；需要修改D命令实现 */
    if(AT_SUCCESS == MN_CALL_Orig(gastAtClientTab[ucIndex].usClientId,
                                  0,
                                  &gastAtClientTab[ucIndex].AtCallInfo.CallId,
                                  &stOrigParam))
    {
        if(MN_CALL_TYPE_VOICE == CallType)
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_CS_VOICE_CALL_SET;
        }
        else
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_CS_DATA_CALL_SET;
        }
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_UNKNOWN);

        return AT_ERROR;
    }
}
/*****************************************************************************
 函 数 名  : At_DialNumByIndexFromPb
 功能描述  : 从电话本读取号码后拨号
 输入参数  : VOS_UINT8 ucIndex
             SI_PB_EVENT_INFO_STRU  *pEvent
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月26日
    作    者   : 王毛
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 At_DialNumByIndexFromPb(VOS_UINT8              ucIndex,
                                            SI_PB_EVENT_INFO_STRU  *pEvent)
{

    /*将从电话本读到的号码写入gastAtParaList[1].aucPara*/
    VOS_MemCpy(&gastAtParaList[1].aucPara[0],
               pEvent->PBEvent.PBReadCnf.PBRecord.Number,
               pEvent->PBEvent.PBReadCnf.PBRecord.NumberLength);
    gastAtParaList[1].aucPara[pEvent->PBEvent.PBReadCnf.PBRecord.NumberLength] = '\0';
    gastAtParaList[1].usParaLen = pEvent->PBEvent.PBReadCnf.PBRecord.NumberLength;
    /* D[<dial_string>][I][G][;] */
    return At_SetDialNumPara(ucIndex,gastAtClientTab[ucIndex].AtCallInfo.CallType);
}

/*****************************************************************************
 Prototype      : At_SetDialVoiceIndexPara
 Description    : D><n>[I][G][;]
 Input          : ucIndex --- 用户索引
                  CallType --- 呼叫类型
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetDialVoiceIndexPara(TAF_UINT8 ucIndex, MN_CALL_TYPE_ENUM_U8 enCallType)
{
    TAF_UINT32 uLIndex = 0;
    SI_PB_STORATE_TYPE  Storage = SI_PB_STORAGE_UNSPECIFIED;

    if(AT_FAILURE == At_Auc2ul(gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen,&uLIndex))
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].AtCallInfo.CallType = enCallType;

    At_SetDialIGPara(ucIndex,3,&gastAtClientTab[ucIndex].AtCallInfo.ClirInfo,&gastAtClientTab[ucIndex].AtCallInfo.CugMode);

    if ( AT_SUCCESS == SI_PB_Read( gastAtClientTab[ucIndex].usClientId,
                                    0,
                                    Storage,
                                    (TAF_UINT16)uLIndex,
                                    (TAF_UINT16)uLIndex)
        )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_GET_NUMBER_BEFORE_CALL;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
/*****************************************************************************
 Prototype      : At_SetDialVoiceNamePara
 Description    : D><name>[I][G][;]
 Input          : ucIndex --- 用户索引
                  CallType --- 呼叫类型
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetDialVoiceNamePara(TAF_UINT8 ucIndex,MN_CALL_TYPE_ENUM_U8 CallType)
{
#if 0
    SI_PB_STORATE_TYPE      Storage = SI_PB_STORAGE_UNSPECIFIED;

    TAF_PB_SEARCH_TYPE  SearchType = TAF_PB_SEARCH_BY_NAME;
    TAF_PB_AlPHATAG_TYPE AlphaTagType = TAF_PB_ALPHATAG_TYPE_GSM;
#endif

    if(AT_FAILURE == At_CheckNumLen(SI_PB_ALPHATAG_MAX_LEN,gastAtParaList[2].usParaLen))
    {
        return AT_ERROR;
    }

    if(AT_FAILURE == At_CheckStringPara(&gastAtParaList[2]))
    {
        return AT_ERROR;
    }

    /* 设置ucAlphaTagType */
    if(AT_CSCS_UCS2_CODE == gucAtCscsType)
    {
        /* AlphaTagType = TAF_PB_ALPHATAG_TYPE_UCS2; */
        if(AT_FAILURE == At_UnicodePrint2Unicode(gastAtParaList[2].aucPara,&gastAtParaList[2].usParaLen))
        {
            return AT_ERROR;
        }
    }

    gastAtClientTab[ucIndex].AtCallInfo.CallType = CallType;

    At_SetDialIGPara(ucIndex,3,&gastAtClientTab[ucIndex].AtCallInfo.ClirInfo,&gastAtClientTab[ucIndex].AtCallInfo.CugMode);

#if 0
    if ( AT_SUCCESS == Taf_PBSearch ( gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      Storage,
                                      SearchType,
                                      AlphaTagType,
                                      (TAF_UINT8)gastAtParaList[2].usParaLen,
                                      gastAtParaList[2].aucPara
                                     )
        )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_GET_NUMBER_BEFORE_CALL;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
#endif

    return AT_ERROR;
}

TAF_UINT32 At_SetDialVoiceMemPara(TAF_UINT8 ucIndex, MN_CALL_TYPE_ENUM_U8 CallType)
{
    TAF_UINT32 uLIndex = 0;
    SI_PB_STORATE_TYPE  Storage = SI_PB_STORAGE_UNSPECIFIED;

    if(AT_FAILURE == At_Auc2ul(gastAtParaList[3].aucPara,gastAtParaList[3].usParaLen,&uLIndex))
    {
        return AT_ERROR;
    }

    if(0 == VOS_StrCmp((TAF_CHAR*)"SM",(TAF_CHAR*)gastAtParaList[2].aucPara))
    {
        Storage = SI_PB_STORAGE_SM;
    }
    else
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].AtCallInfo.CallType = CallType;

    At_SetDialIGPara(ucIndex,4,&gastAtClientTab[ucIndex].AtCallInfo.ClirInfo,&gastAtClientTab[ucIndex].AtCallInfo.CugMode);

    if ( AT_SUCCESS == SI_PB_Read ( gastAtClientTab[ucIndex].usClientId,
                                    0,
                                    Storage,
                                    (TAF_UINT16)uLIndex,
                                    (TAF_UINT16)uLIndex)
        )
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_GET_NUMBER_BEFORE_CALL;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetDMmiNumWithClirPara(
    TAF_UINT8                         ucIndex,
    MN_CALL_CLIR_CFG_ENUM_U8          enClirType,
    TAF_INT8                         *pcStrNum,
    TAF_UINT32                        ulStrLen
)
{
    MN_CALL_ORIG_PARAM_STRU             stOrigParam;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 长度检查 */
    if( (MN_CALL_MAX_BCD_NUM_LEN*2) < ulStrLen)
    {
        return AT_CME_DIAL_STRING_TOO_LONG;
    }

    /* 初始化 */
    PS_MEM_SET(&stOrigParam,0x00,sizeof(stOrigParam));

    /* 设置<dial_string> */
    if ('+' == pcStrNum[0])
    {
        stOrigParam.stDialNumber.enNumType
            = (0x80 | (MN_CALL_TON_INTERNATIONAL << 4) | MN_CALL_NPI_ISDN);
        if (MN_ERR_NO_ERROR != AT_AsciiNumberToBcd((TAF_CHAR *)pcStrNum + 1,
                            stOrigParam.stDialNumber.aucBcdNum,
                            &stOrigParam.stDialNumber.ucNumLen))
        {
            AT_WARN_LOG("At_SetDMmiNumWithClirPara():AT_AsciiNumberToBcd fail");
        }
    }
    else
    {
        stOrigParam.stDialNumber.enNumType
            = (0x80 | (MN_CALL_TON_UNKNOWN << 4) | MN_CALL_NPI_ISDN);
        if (MN_ERR_NO_ERROR != AT_AsciiNumberToBcd((TAF_CHAR *)pcStrNum,
                            stOrigParam.stDialNumber.aucBcdNum,
                            &stOrigParam.stDialNumber.ucNumLen))
        {
            AT_WARN_LOG("At_SetDMmiNumWithClirPara():AT_AsciiNumberToBcd fail");
        }
    }

    stOrigParam.enCallType = MN_CALL_TYPE_VOICE;
    stOrigParam.enClirCfg  = enClirType;

    stOrigParam.enVoiceDomain = TAF_CALL_VOICE_DOMAIN_AUTO;

    stOrigParam.enCallMode = pstSsCtx->enCModType;

    if(AT_SUCCESS == MN_CALL_Orig(gastAtClientTab[ucIndex].usClientId,
                                  0,
                                  &gastAtClientTab[ucIndex].AtCallInfo.CallId,
                                  &stOrigParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_D_CS_VOICE_CALL_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_SetDMmiPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8                           pucPara[6];

    VOS_MemSet(pucPara, 0 ,sizeof(pucPara));
    gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1] = '\0';

    VOS_StrNCpy((TAF_CHAR *)pucPara, (TAF_CHAR*)gastAtParaList[0].aucPara, sizeof(pucPara) - 1);

    if ((VOS_OK == VOS_StrCmp((TAF_CHAR*)pucPara,"D*31#")))
    {
        return At_SetDMmiNumWithClirPara(ucIndex,
                                         MN_CALL_CLIR_SUPPRESS,
                                         (VOS_INT8*)(gastAtParaList[0].aucPara + VOS_StrLen("D*31#")),
                                         VOS_StrLen((VOS_CHAR *)gastAtParaList[0].aucPara) - VOS_StrLen("D*31#"));
    }
    else if (VOS_OK == VOS_StrCmp((TAF_CHAR*)pucPara,"d*31#"))
    {
        return At_SetDMmiNumWithClirPara(ucIndex,
                                         MN_CALL_CLIR_SUPPRESS,
                                         (VOS_INT8*)(gastAtParaList[0].aucPara + VOS_StrLen("d*31#")),
                                         VOS_StrLen((VOS_CHAR *)gastAtParaList[0].aucPara) - VOS_StrLen("d*31#"));
    }
    else if ((VOS_OK == VOS_StrCmp((TAF_CHAR*)pucPara,"D#31#")))
    {
        return At_SetDMmiNumWithClirPara(ucIndex,
                                         MN_CALL_CLIR_INVOKE,
                                         (VOS_INT8*)(gastAtParaList[0].aucPara + VOS_StrLen("D#31#")),
                                         VOS_StrLen((VOS_CHAR *)gastAtParaList[0].aucPara) - VOS_StrLen("D#31#"));
    }
    else if ((VOS_OK == VOS_StrCmp((TAF_CHAR*)pucPara,"d#31#")))
    {
        return At_SetDMmiNumWithClirPara(ucIndex,
                                         MN_CALL_CLIR_INVOKE,
                                         (VOS_INT8*)(gastAtParaList[0].aucPara + VOS_StrLen("d#31#")),
                                         VOS_StrLen((VOS_CHAR *)gastAtParaList[0].aucPara) - VOS_StrLen("d#31#"));
    }
    else
    {
        /* D*752#+436644101453; */
        gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1] = 0;
        gastAtParaList[0].usParaLen--;
        return AT_ERROR;
    }

}
TAF_UINT32  At_CheckAtDPin(VOS_UINT8 ucIndex)
{
    TAF_UINT32                          ulRst;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulGetModemIdRslt;
    AT_USIM_INFO_CTX_STRU              *pstUsimInfoCtx = VOS_NULL_PTR;

    enModemId = MODEM_ID_0;

    ulGetModemIdRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulGetModemIdRslt)
    {
        AT_ERR_LOG("At_CheckAtDPin:Get Modem Id fail!");
        return AT_ERROR;
    }

    pstUsimInfoCtx = AT_GetUsimInfoCtxFromModemId(enModemId);


    /*SIM卡状态判断*/
    switch (pstUsimInfoCtx->enCardStatus)
    {
        case USIMM_CARD_SERVIC_AVAILABLE:
            ulRst = AT_SUCCESS;
            break;
        case USIMM_CARD_SERVIC_SIM_PIN:
            ulRst = AT_CME_SIM_PIN_REQUIRED;
            break;
        case USIMM_CARD_SERVIC_SIM_PUK:
            ulRst = AT_CME_SIM_PUK_REQUIRED;
            break;
        case USIMM_CARD_SERVIC_UNAVAILABLE:
        case USIMM_CARD_SERVIC_NET_LCOK:
        case USIMM_CARD_SERVIC_IMSI_LCOK:
            ulRst = AT_CME_SIM_FAILURE;
            break;
        case USIMM_CARD_SERVIC_ABSENT:
            ulRst = AT_CME_SIM_NOT_INSERTED;
            break;
        default:
            ulRst = AT_ERROR;
            break;
    }

    return ulRst;
}
TAF_UINT32  At_PsDPara(TAF_UINT8 ucIndex)
{
    TAF_UINT32    ulRst;

    ulRst = At_CheckAtDPin(ucIndex);
    if (AT_SUCCESS != ulRst)
    {
        return ulRst;
    }

    if(AT_SUCCESS == At_CheckSemicolon(gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1])) /* 如果带分号,D*752#+436644101453; */
    {

        /* D*752#+436644101453; */
        gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1] = 0;
        gastAtParaList[0].usParaLen--;
    }
    else if('#' == gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1])
    {
        if(0 == VOS_StrNiCmp((TAF_CHAR*)gastAtParaList[2].aucPara,"98", gastAtParaList[2].usParaLen))
        {
            /* D*<GPRS_SC_IP>[*<cid>[,<cid>]]# */
            return At_SetDial98Para(ucIndex);
        }
        else if(0 == VOS_StrNiCmp((TAF_CHAR*)gastAtParaList[2].aucPara,"99", gastAtParaList[2].usParaLen))
        {
            /* D*<GPRS_SC>[*[<called_address>][*[<L2P>][*[<cid>]]]]# */
            return At_SetDial99Para(ucIndex);
        }
        else
        {

        }
    }
    else
    {

    }
    return AT_ERROR;

}

VOS_VOID At_CheckDialStrPara(VOS_VOID)
{
    if (AT_SUCCESS == At_CheckMmiString(gastAtParaList[0].aucPara,gastAtParaList[0].usParaLen))
    {
        if(';' != gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1])
        {
            PS_MEM_CPY(gastAtParaList[1].aucPara, &gastAtParaList[0].aucPara[1], (VOS_SIZE_T)(gastAtParaList[0].usParaLen - 1));
            gastAtParaList[1].usParaLen = gastAtParaList[0].usParaLen - 1;
        }
        else
        {
            PS_MEM_CPY(gastAtParaList[1].aucPara, &gastAtParaList[0].aucPara[1], (VOS_SIZE_T)(gastAtParaList[0].usParaLen - 2));
            gastAtParaList[1].usParaLen = gastAtParaList[0].usParaLen - 2;
        }
    }

    return;
}


VOS_UINT8 At_JudgeIfIsPsCall(VOS_VOID)
{
    VOS_UINT8                           ucPsCallFlg;

    if ('#' != gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1])
    {
        ucPsCallFlg = VOS_FALSE;
    }
    else if (AT_CheckStrStartWith(&gastAtParaList[0].aucPara[1], (VOS_UINT8 *)"*98"))
    {
        ucPsCallFlg = VOS_TRUE;
    }
    else if (AT_CheckStrStartWith(&gastAtParaList[0].aucPara[1], (VOS_UINT8 *)"*99"))
    {
        ucPsCallFlg = VOS_TRUE;
    }
    else
    {
        ucPsCallFlg = VOS_FALSE;
    }

    return ucPsCallFlg;
}

TAF_UINT32 At_SetDPara(TAF_UINT8 ucIndex)
{
    /* cS call */
    if(VOS_TRUE != At_JudgeIfIsPsCall())
    {
        MN_CALL_TYPE_ENUM_U8 CallType = MN_CALL_TYPE_VOICE;

        g_ucDtrDownFlag = VOS_FALSE;

        if(';' != gastAtParaList[0].aucPara[gastAtParaList[0].usParaLen - 1])
        {
            CallType = MN_CALL_TYPE_VIDEO;
        }

        if('>' == gastAtParaList[1].aucPara[0])
        {
            if(('A' <= gastAtParaList[2].aucPara[0]) && (gastAtParaList[2].aucPara[0] <= 'Z'))
            {
                /* D>mem<n>[I][G][;] */
                return At_SetDialVoiceMemPara(ucIndex,CallType);
            }
            else if(('0' <= gastAtParaList[2].aucPara[0]) && (gastAtParaList[2].aucPara[0] <= '9'))
            {
                /* D><n>[I][G][;] */
                return At_SetDialVoiceIndexPara(ucIndex,CallType);
            }
            else
            {
                /* D><name>[I][G][;] */
                return At_SetDialVoiceNamePara(ucIndex,CallType);
            }
        }
        else
        {
            /* D[<dial_string>][I][G][;] */
            return At_SetDialNumPara(ucIndex,CallType);
        }
    }
    else
    {
        return At_PsDPara(ucIndex);
    }

}
TAF_UINT32 At_SetAPara(TAF_UINT8 ucIndex)
{
    MN_CALL_SUPS_PARAM_STRU   stCallMgmtParam;
    VOS_UINT32                ulTimerId;

    VOS_UINT8                           ucNumOfCalls;
    VOS_UINT8                           ucTmp;
    VOS_UINT32                          ulRlst;
    TAFAGERNT_MN_CALL_INFO_STRU         astCallInfos[MN_CALL_MAX_NUM];
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;

    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucIndex);


    PS_MEM_SET(astCallInfos, 0x00, sizeof(astCallInfos));


    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


#ifndef DMT
    /*从同步API获取通话信息*/
    ulRlst = TAF_AGENT_GetCallInfoReq(gastAtClientTab[ucIndex].usClientId,
                                      &ucNumOfCalls,
                                      astCallInfos);

    if (VOS_OK != ulRlst)
    {
        return AT_ERROR;
    }

    if (0 == ucNumOfCalls)
    {
        return AT_NO_CARRIER;
    }

    for (ucTmp = 0; ucTmp < ucNumOfCalls; ucTmp++)
    {
        /*设置了自动接听的情况下如果呼叫已经处于incoming态，再触发ATA操作，返回ok*/
        if ((MN_CALL_S_INCOMING == astCallInfos[ucTmp].enCallState)
             && (0 != pstCcCtx->stS0TimeInfo.ucS0TimerLen)
             && (TAF_FALSE == pstCcCtx->stS0TimeInfo.bTimerStart))
        {
            return AT_OK;
        }
        /*设置了自动接听的情况下如果呼叫已经处于非incoming态，再触发ATA操作，返回error*/
        else if ((0 != pstCcCtx->stS0TimeInfo.ucS0TimerLen)
              && (TAF_FALSE == pstCcCtx->stS0TimeInfo.bTimerStart))
        {
            return AT_NO_CARRIER;
        }
        /* 呼叫已经是active状态再收到ata直接回复ok */
        else if (MN_CALL_S_ACTIVE == astCallInfos[ucTmp].enCallState)
        {
            return AT_OK;
        }
        else
        {
        }
    }
#endif


    PS_MEM_SET(&stCallMgmtParam, 0, sizeof(stCallMgmtParam));
    stCallMgmtParam.enCallSupsCmd = MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH;

    if(AT_SUCCESS == MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId, 0, &stCallMgmtParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_A_SET;
        if (TAF_TRUE == pstCcCtx->stS0TimeInfo.bTimerStart)
        {
            ulTimerId = pstCcCtx->stS0TimeInfo.ulTimerName;

            AT_StopRelTimer(ulTimerId, &(pstCcCtx->stS0TimeInfo.s0Timer));
            pstCcCtx->stS0TimeInfo.bTimerStart = TAF_FALSE;
            pstCcCtx->stS0TimeInfo.ulTimerName = 0;
        }
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_NO_CARRIER;
    }


}


VOS_UINT32 AT_MODEM_HangupCall(VOS_UINT8 ucIndex)
{
    AT_UART_LINE_CTRL_STRU             *pstLineCtrl = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    pstLineCtrl = AT_GetUartLineCtrlInfo();

    /* ONLINE-COMMAND模式下, 需要断开PPP拨号连接 */
    if (AT_ONLINE_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        /* 正在处理PPP断开请求, 直接继承, 设置命令操作类型为AT_CMD_H_PS_SET */
        if (AT_CMD_PS_DATA_CALL_END_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_H_PS_SET;
            ulRslt = AT_WAIT_ASYNC_RETURN;
        }
        /* 正在等待PPP的释放响应, 直接释放, 返回OK */
        else if (AT_CMD_WAIT_PPP_PROTOCOL_REL_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);
            At_FormatResultData(ucIndex, AT_OK);

            if (AT_UART_DCD_MODE_CONNECT_ON == pstLineCtrl->enDcdMode)
            {
                AT_CtrlDCD(ucIndex, AT_IO_LEVEL_LOW);
            }

            ulRslt = AT_SUCCESS;
        }
        else
        {
            (VOS_VOID)TAF_PS_CallEnd(WUEPS_PID_AT,
									 gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     gastAtClientTab[ucIndex].ucCid);

            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_H_PS_SET;
            ulRslt = AT_WAIT_ASYNC_RETURN;
        }
    }
    /* 其他模式下, 默认直接返回OK */
    else
    {
        ulRslt = AT_OK;
    }

    return ulRslt;
}


VOS_UINT32 At_SetHPara(VOS_UINT8 ucIndex)
{
    MN_CALL_SUPS_PARAM_STRU             stCallMgmtParam;

    /* 删除通道数传模式标识存在直接返回OK的处理 */

    /*对Modem口进行特殊处理: 收到ATH直接返回OK，
      即使Modem口发起过呼叫也不执行挂断操作*/
    if ((AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
     && (AT_CLIENT_USED == gastAtClientTab[ucIndex].ucUsed))
    {
        return AT_OK;
    }

    /* 增加通道模式判断:
     * (1) ONLINE-COMMAND模式 - 断开当前通道的PPP拨号连接
     * (2) COMMAND模式        - 直接返回OK
     */
    if (AT_HSUART_USER == gastAtClientTab[ucIndex].UserType)
    {
        return AT_MODEM_HangupCall(ucIndex);
    }

    /*这里目前暂时不支持数据和语音的切换，华为移动也暂时没有这样的需求
      在这种情况下，ATH等同于CHUP的功能*/
    PS_MEM_SET(&stCallMgmtParam, 0, sizeof(stCallMgmtParam));
    stCallMgmtParam.enCallSupsCmd = MN_CALL_SUPS_CMD_REL_ALL_CALL;
    if(AT_SUCCESS == MN_CALL_Sups(gastAtClientTab[ucIndex].usClientId, 0, &stCallMgmtParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_H_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
/*****************************************************************************
 Prototype      : At_SetEPara
 Description    : E[<value>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetEPara(TAF_UINT8 ucIndex)
{
    if(0 != gastAtParaList[0].usParaLen)
    {
        gucAtEType = (AT_CMD_ECHO_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        gucAtEType = AT_E_ECHO_CMD;
    }
    return AT_OK;
}

TAF_UINT32 At_SetVPara(TAF_UINT8 ucIndex)
{
    if(0 != gastAtParaList[0].usParaLen)
    {
        gucAtVType = (AT_V_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        gucAtVType = AT_V_BREVITE_TYPE;
    }
    return AT_OK;
}

TAF_UINT32 At_SetTPara(TAF_UINT8 ucIndex)
{
    /* 参考Q的实现:This commands will have no effect in our code*/
    return AT_OK;
}
TAF_UINT32 At_SetPPara(TAF_UINT8 ucIndex)
{
    /* 参考Q的实现:This commands will have no effect in our code*/
    return AT_OK;
}
TAF_UINT32 At_SetXPara(TAF_UINT8 ucIndex)
{
    if(0 != gastAtParaList[0].usParaLen)
    {
        gucAtXType = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        gucAtXType = 0;
    }
    return AT_OK;
}
TAF_UINT32 At_SetZPara(TAF_UINT8 ucIndex)
{
    TAF_AT_TZ_STRU                      stTz;
    VOS_CHAR                            acTzStr[AT_SETZ_LEN+1];

    VOS_UINT16                          usLength;
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    PS_MEM_SET(&stTz, 0x0, sizeof(stTz));

    if ( SYSTEM_APP_WEBUI == *pucSystemAppConfig)
    {
        if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_ATSETZ_RET_VALUE, &stTz,
                             AT_SETZ_LEN))
        {
            PS_MEM_CPY(acTzStr, stTz.acTz, AT_SETZ_LEN);
            acTzStr[AT_SETZ_LEN] = 0;
            if ( VOS_StrLen(acTzStr) > 0 )
            {
                usLength  = 0;
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                 "%s",gaucAtCrLf);
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  "%s",acTzStr);
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                 "%s",gaucAtCrLf);

                At_SendResultData((VOS_UINT8)ucIndex,pgucAtSndCodeAddr,usLength);

                return AT_SUCCESS;
            }
        }
    }

    return AT_OK;
}

TAF_UINT32 At_SetQPara(TAF_UINT8 ucIndex)
{
    if(0 != gastAtParaList[0].usParaLen)
    {
        gucAtQType = (AT_CMD_ECHO_TYPE)gastAtParaList[0].ulParaValue;
    }
    else
    {
        gucAtQType = 0;
    }
    return AT_OK;
}


VOS_UINT32 At_SetAmpCPara(VOS_UINT8 ucIndex)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;
    AT_UART_DCD_MODE_ENUM_UINT8         enDcdMode;

    pstUartCtx = AT_GetUartCtxAddr();

    /* 只支持UART端口下发 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return AT_OK;
    }

    /* 参数检查 */
    if (0 != gastAtParaList[0].usParaLen)
    {
        if ((0 != gastAtParaList[0].ulParaValue)
         && (1 != gastAtParaList[0].ulParaValue))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        enDcdMode = (AT_UART_DCD_MODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        /* AT&C 等效于 AT&C1 */
        enDcdMode = AT_UART_DEFAULT_DCD_MODE;
    }

    /* 保存DCD Line Mode至上下文 */
    pstUartCtx->stLineCtrl.enDcdMode = enDcdMode;

    /* DCD管脚操作 */
    if(AT_UART_DCD_MODE_ALWAYS_ON == enDcdMode)
    {
        AT_CtrlDCD(ucIndex, AT_IO_LEVEL_HIGH);
    }
    else
    {
        if (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
        {
            AT_CtrlDCD(ucIndex, AT_IO_LEVEL_LOW);
        }
    }

    return AT_OK;
}
VOS_UINT32 At_SetAmpDPara(VOS_UINT8 ucIndex)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;
    AT_UART_DTR_MODE_ENUM_UINT8         enDtrMode;

    pstUartCtx = AT_GetUartCtxAddr();

    /* 只支持UART端口下发 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return AT_OK;
    }

    /* 参数检查 */
    if (0 != gastAtParaList[0].usParaLen)
    {
        enDtrMode = (AT_UART_DTR_MODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        /* AT&D 等效于 AT&D2 */
        enDtrMode = AT_UART_DEFAULT_DTR_MODE;
    }

    /* 保存DTR Line Mode至上下文 */
    pstUartCtx->stLineCtrl.enDtrMode = enDtrMode;

    return AT_OK;
}

#if (FEATURE_ON == FEATURE_AT_HSUART)

VOS_UINT32 At_SetAmpSPara(VOS_UINT8 ucIndex)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;
    AT_UART_DSR_MODE_ENUM_UINT8         enDsrMode;
    AT_IO_LEVEL_ENUM_UINT8              enIoLevel;

    pstUartCtx = AT_GetUartCtxAddr();

    /* 只支持UART端口下发 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return AT_OK;
    }

    /* 参数检查 */
    if (0 != gastAtParaList[0].usParaLen)
    {
        enDsrMode = (AT_UART_DSR_MODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        /* AT&S 等效于 AT&S0 */
        enDsrMode = AT_UART_DEFAULT_DSR_MODE;
    }

    /* 设置DSR 管脚信号 */
    enIoLevel = (AT_UART_DSR_MODE_ALWAYS_ON == enDsrMode) ? AT_IO_LEVEL_HIGH : AT_IO_LEVEL_LOW;
    AT_CtrlDSR(ucIndex, enIoLevel);

    /* 保存DSR Line Mode至上下文*/
    pstUartCtx->stLineCtrl.enDsrMode = enDsrMode;

    return AT_OK;
}
#endif


TAF_UINT32 At_SetFPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    /* 被if 0的代码为日本运营商的定制需求，恢复8229和8265两个NV项。但后来要求需求回退，考虑到后面又有可能需要该需求，就用#if 0关闭了。*/
#if 0
    /*NV4102的数据类型：16bit unsigned [3]*/
    VOS_UINT16                          ausSptBand[3];
    VOS_UINT32                          ulSptBandLen;
    /*NV8229*/
    NVIM_UE_SUPPORT_FREQ_BAND_STRU      stUserSetFreqBand;
    /*NV8265的数据类型：64bit unsigned*/
    AT_NV_User_Set_Freq_Band64_STRU     stBand64;
#endif

    /*因为MODEN拨号会下发该命令，所以在moden口不进行NV恢复，直接返回OK*/
    if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
      || (AT_HSUART_USER == gastAtClientTab[ucIndex].UserType) )
    {
        return AT_OK;
    }
    if ( VOS_TRUE == g_bSetFlg )
    {
        return AT_OK;
    }

    /* DPM_SleepDisable(DPM_SLEEP_NV); */


    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_NVRESTORE_MANU_DEFAULT_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRet)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_F_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

#if 0
    /*需要重新恢复频段相关的NV:
    读取NV项NV_RF_BAND(NV ID:4102)，将对应频段信息写入NV项Support_Freqbands(NV ID:8229),
    把NV项UserSetFreqbands(NV ID:8265)的内容重新置为0x3FFFFFFF.
    */
    /*读取NV4102获取频段信息*/
    NV_GetLength(en_NV_Item_UMTS_BAND, &ulSptBandLen);

    ulRet = NV_ReadEx(MODEM_ID_0, en_NV_Item_UMTS_BAND,ausSptBand,ulSptBandLen);

    if ( NV_OK != ulRet )
    {
        return AT_ERROR;
    }

    /*将频段信息写入NV8229对应的域(前四个字节)*/
    ulRet = NV_ReadEx(MODEM_ID_0, en_NV_Item_Support_Freqbands,
                    &stUserSetFreqBand, sizeof(stUserSetFreqBand));

    if ( NV_OK != ulRet )
    {
        return AT_ERROR;
    }

    stUserSetFreqBand.usWcdmaBand = ausSptBand[0];
    stUserSetFreqBand.usGsmBand   = ausSptBand[1];

    ulRet = NV_WriteEx(MODEM_ID_0, en_NV_Item_Support_Freqbands,
                          &(stUserSetFreqBand),
                          sizeof(stUserSetFreqBand));

    if ( NV_OK != ulRet )
    {
        return AT_ERROR;
    }

    /*将NV8265的值设置为0x3FFFFFFF*/
    stBand64.ulBandHigh = 0;
    stBand64.ulBandLow  = 0x3FFFFFFF;

    ulRet = NV_WriteEx(MODEM_ID_0, en_NV_Item_User_Set_Freqbands,
                          &stBand64,
                          sizeof(stBand64));

    if ( NV_OK != ulRet )
    {
        return AT_ERROR;
    }
#endif

}
TAF_UINT32 At_SetS0Para(TAF_UINT8 ucIndex)
{
    TAF_CCA_TELE_PARA_STRU              stTimeInfo;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 删除通道数传模式标识存在直接返回OK的处理 */

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_SetS0Para: Get modem id fail.");
        return AT_ERROR;
    }

    pstCcCtx = AT_GetModemCcCtxAddrFromModemId(enModemId);

    pstCcCtx->stS0TimeInfo.ucS0TimerLen = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    pstCcCtx->stS0TimeInfo.usClientId   = gastAtClientTab[ucIndex].usClientId;

    PS_MEM_SET(&stTimeInfo, 0x00, sizeof(stTimeInfo));

    if (NV_OK == NV_ReadEx(enModemId, en_NV_Item_CCA_TelePara, &stTimeInfo, sizeof(TAF_CCA_TELE_PARA_STRU)))
    {
        if ( stTimeInfo.ucS0TimerLen == pstCcCtx->stS0TimeInfo.ucS0TimerLen )
        {
            AT_INFO_LOG("At_SetS0Para():The content to write is same as NV's");
            return AT_OK;
        }
    }
    stTimeInfo.ucS0TimerLen = pstCcCtx->stS0TimeInfo.ucS0TimerLen;
    if (NV_OK != NV_WriteEx(enModemId, en_NV_Item_CCA_TelePara, &stTimeInfo, sizeof(TAF_CCA_TELE_PARA_STRU)))
    {
        return AT_ERROR;
    }

    return AT_OK;
}
/*****************************************************************************
 Prototype      : At_SetS3Para
 Description    : S3=[<value>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetS3Para(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucAtS3 = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucAtS3 = 13;
    }
    gaucAtCrLf[0] = ucAtS3;
    return AT_OK;
}
/*****************************************************************************
 Prototype      : At_SetS4Para
 Description    : S4=[<value>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetS4Para(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucAtS4 = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucAtS4 = 10;
    }
    gaucAtCrLf[1] = ucAtS4;
    return AT_OK;
}
/*****************************************************************************
 Prototype      : At_SetS5Para
 Description    : S5=[<value>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetS5Para(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucAtS5 = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucAtS5 = 8;
    }
    return AT_OK;
}

TAF_UINT32 At_SetS6Para(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        ucAtS6 = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        /* refer to V.250:
           Recommended default setting
           2       Wait two seconds before blind dialling
         */
        ucAtS6 = 2;
    }
    return AT_OK;
}

TAF_UINT32 At_SetS7Para(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*
      refer to V.250:
      S7:Number of seconds in which connection must be established or call will be disconnected
     */
    if(0 != gastAtParaList[0].usParaLen)
    {
        ucAtS7 = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        /* 参考Q实现，默认值填写为50 */
        ucAtS7 = 50;
    }
    return AT_OK;
}
VOS_VOID AT_SetDefaultRatPrioList(
    MODEM_ID_ENUM_UINT16                enModemId,
    TAF_PH_RAT_ORDER_STRU              *pstSysCfgRatOrder,
    VOS_UINT8                           ucUserSptLteFlag
)
{
    VOS_UINT8                           ucUserRatNum;

    ucUserRatNum = 0;

#if(FEATURE_ON == FEATURE_LTE)
    /* 当前命令是否支持LTE */
    if (VOS_TRUE == ucUserSptLteFlag)
    {
        /* 平台支持LTE */
        if (VOS_TRUE == AT_IsModemSupportRat(enModemId, TAF_PH_RAT_LTE))
        {
             pstSysCfgRatOrder->aenRatOrder[ucUserRatNum] = TAF_PH_RAT_LTE;
             ucUserRatNum++;
        }
    }
#endif

    /* 平台支持WCDMA */
    if (VOS_TRUE == AT_IsModemSupportRat(enModemId, TAF_PH_RAT_WCDMA))
    {
         pstSysCfgRatOrder->aenRatOrder[ucUserRatNum] = TAF_PH_RAT_WCDMA;
         ucUserRatNum++;
    }

    /* 平台支持GSM */
    if (VOS_TRUE == AT_IsModemSupportRat(enModemId, TAF_PH_RAT_GSM))
    {
         pstSysCfgRatOrder->aenRatOrder[ucUserRatNum] = TAF_PH_RAT_GSM;
         ucUserRatNum++;
    }

    pstSysCfgRatOrder->ucRatOrderNum = ucUserRatNum;
    return;
}


VOS_VOID AT_GetGuAccessPrioNvim (
    MODEM_ID_ENUM_UINT16                enModemId,
    AT_SYSCFG_RAT_PRIO_ENUM_UINT8      *penAcqorder
)
{
    NAS_MMA_NVIM_ACCESS_MODE_STRU        stAccessMode;
    VOS_UINT32                           ulLength;

    ulLength = 0;

    PS_MEM_SET(&stAccessMode, 0x00, sizeof(NAS_MMA_NVIM_ACCESS_MODE_STRU));

    NV_GetLength(en_NV_Item_MMA_AccessMode, &ulLength);
    if (NV_OK != NV_ReadEx(enModemId,
                            en_NV_Item_MMA_AccessMode ,
                            &stAccessMode,
                            ulLength))

    {
        *penAcqorder = AT_SYSCFG_RAT_PRIO_AUTO;
        return ;
    }

    *penAcqorder = stAccessMode.aucAccessMode[1];
}



VOS_VOID AT_GetRatOrderNvim(
    MODEM_ID_ENUM_UINT16                enModemId,
    TAF_PH_RAT_ORDER_STRU              *pstSysCfgRatOrder,
    VOS_UINT8                           ucUserSptLteFlag
)
{
    TAF_PH_NVIM_RAT_ORDER_STRU          stPrioRatList;
    VOS_UINT8                           i;
    VOS_UINT8                           ucIndex;
    ucIndex = 0;


    PS_MEM_SET(&stPrioRatList, 0x00, sizeof(stPrioRatList));


    /* 读en_NV_Item_RAT_PRIO_LIST，获取当前的stSysCfgRatOrder */
    if (NV_OK != NV_ReadEx(enModemId, en_NV_Item_RAT_PRIO_LIST, &stPrioRatList, sizeof(stPrioRatList)))
    {
        AT_SetDefaultRatPrioList(enModemId, pstSysCfgRatOrder, ucUserSptLteFlag);
        return;
    }

#if(FEATURE_ON == FEATURE_LTE)
    /* 判断接入技术个数是否有效 */
    if ((stPrioRatList.ucRatOrderNum > TAF_PH_MAX_GUL_RAT_NUM)
     || (0 == stPrioRatList.ucRatOrderNum))
    {
        AT_SetDefaultRatPrioList(enModemId, pstSysCfgRatOrder, ucUserSptLteFlag);
        return;
    }

    for (i = 0; i < stPrioRatList.ucRatOrderNum; i++)
    {
        pstSysCfgRatOrder->aenRatOrder[ucIndex] = stPrioRatList.aenRatOrder[i];
        ucIndex++;
    }

    pstSysCfgRatOrder->ucRatOrderNum = ucIndex;

#else
    /* 判断接入技术个数是否有效 */
    if ((stPrioRatList.ucRatOrderNum > TAF_PH_MAX_GU_RAT_NUM)
     || (0 == stPrioRatList.ucRatOrderNum))
    {
        AT_SetDefaultRatPrioList(enModemId, pstSysCfgRatOrder, ucUserSptLteFlag);
        return;
    }

    /* 获取GU的接入优先级 */
    for (i = 0; i < stPrioRatList.ucRatOrderNum; i++)
    {
        if ( (TAF_PH_RAT_WCDMA == stPrioRatList.aenRatOrder[i])
          || (TAF_PH_RAT_GSM   == stPrioRatList.aenRatOrder[i]) )
        {
            pstSysCfgRatOrder->aenRatOrder[ucIndex] = stPrioRatList.aenRatOrder[i];
            ucIndex++;
        }
    }
    pstSysCfgRatOrder->ucRatOrderNum = ucIndex;

#endif

    return;
}


VOS_UINT32 AT_IsSupportGMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    VOS_UINT32                          i;
    for (i = 0; i < pstRatOrder->ucRatOrderNum; i++)
    {
        if (TAF_PH_RAT_GSM == pstRatOrder->aenRatOrder[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 AT_IsSupportLMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    VOS_UINT32                          i;

    for (i = 0; i < pstRatOrder->ucRatOrderNum; i++)
    {
        if (TAF_PH_RAT_LTE == pstRatOrder->aenRatOrder[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 AT_IsSupportWMode(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    VOS_UINT32                          i;

    for (i = 0; i < pstRatOrder->ucRatOrderNum; i++)
    {
        if (TAF_PH_RAT_WCDMA == pstRatOrder->aenRatOrder[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 AT_ConvertCpamRatOrderPara(
    VOS_UINT8                           ucClientId,
    MN_MMA_CPAM_RAT_TYPE_ENUM_UINT8     enMode,
    MN_MMA_CPAM_RAT_PRIO_ENUM_UINT8     enAcqorder,
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder,
    AT_SYSCFG_RAT_PRIO_ENUM_UINT8      *penSyscfgRatPrio
)
{
    AT_SYSCFG_RAT_PRIO_ENUM_UINT8       enNvimRatPrio;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRst;

    enModemId = MODEM_ID_0;

    ulRst = AT_GetModemIdFromClient(ucClientId, &enModemId);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG1("AT_ConvertCpamRatOrderPara:Get ModemID From ClientID fail,ClientID=%d", ucClientId);
        return AT_ERROR;
    }

    /* 如果接入优先级不改变则默认从NVIM中获取 */
    /* 由于NVIM中的接入优先级和CPAM的设置的不一样需要转化一下 */
    if ( MN_MMA_CPAM_RAT_PRIO_NO_CHANGE == enAcqorder )
    {
        AT_GetGuAccessPrioNvim(enModemId, &enNvimRatPrio);
        *penSyscfgRatPrio = enNvimRatPrio;
    }
    else if ( MN_MMA_CPAM_RAT_PRIO_GSM == enAcqorder )
    {
        *penSyscfgRatPrio = AT_SYSCFG_RAT_PRIO_GSM;
    }
    else
    {
        *penSyscfgRatPrio = AT_SYSCFG_RAT_PRIO_WCDMA;
    }



    switch (enMode)
    {
        case MN_MMA_CPAM_RAT_TYPE_WCDMA_GSM:
            if (AT_SYSCFG_RAT_PRIO_GSM == *penSyscfgRatPrio)
            {
                /* GSM优先 */
                pstRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
                pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
                pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_WCDMA;
                pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            }
            else
            {
                /* WCDMA优先 */
                pstRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
                pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
                pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_GSM;
                pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            }

            break;

        case MN_MMA_CPAM_RAT_TYPE_GSM:
            /* G单模 */
            pstRatOrder->ucRatOrderNum = 1;
            pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
            pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

        case MN_MMA_CPAM_RAT_TYPE_WCDMA:
            /* W单模 */
            pstRatOrder->ucRatOrderNum = 1;
            pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
            pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

        case MN_MMA_CPAM_RAT_TYPE_NOCHANGE:
            AT_GetRatOrderNvim(enModemId, pstRatOrder, VOS_FALSE);
            AT_GetOnlyGURatOrder(pstRatOrder);

            if (TAF_PH_MAX_GU_RAT_NUM == pstRatOrder->ucRatOrderNum)
            {
                if (AT_SYSCFG_RAT_PRIO_WCDMA == *penSyscfgRatPrio)
                {
                    pstRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
                    pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
                    pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_GSM;
                    pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
                }
                else if (AT_SYSCFG_RAT_PRIO_GSM == *penSyscfgRatPrio)
                {
                    pstRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
                    pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
                    pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_WCDMA;
                    pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
                }
                else
                {
                }

            }

            /* 原单模无需考虑接入优先级 */

            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;

    }

    return AT_OK;

}


VOS_UINT32 At_SetCpamPara(VOS_UINT8 ucIndex)
{
    TAF_MMA_SYS_CFG_PARA_STRU           stSysCfgSetPara;

    VOS_UINT32                          ulRst;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    ulRst = AT_OK;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 == gastAtParaList[0].usParaLen )
    {
        gastAtParaList[0].ulParaValue = MN_MMA_CPAM_RAT_TYPE_NOCHANGE;
    }


    if ( 0 == gastAtParaList[1].usParaLen )
    {
        gastAtParaList[1].ulParaValue = MN_MMA_CPAM_RAT_PRIO_NO_CHANGE;
    }





    ulRst = AT_ConvertCpamRatOrderPara(ucIndex,
                                       (VOS_UINT8)gastAtParaList[0].ulParaValue,
                                       (VOS_UINT8)gastAtParaList[1].ulParaValue,
                                       &stSysCfgSetPara.stRatOrder,
                                       &stSysCfgSetPara.enUserPrio);

    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* stSysCfgSetPara其他参数赋值 */
    if (AT_ROAM_FEATURE_ON == pstNetCtx->ucRoamFeature)
    {
        stSysCfgSetPara.ucRoam = TAF_PH_ROAM_UNCHANGE;
    }
    else
    {
        stSysCfgSetPara.ucRoam = AT_ROAM_FEATURE_OFF_NOCHANGE;
    }
    stSysCfgSetPara.ucSrvDomain         = TAF_PH_SERVICE_NOCHANGE;
    stSysCfgSetPara.stGuBand.ulBandLow  = TAF_PH_BAND_NO_CHANGE;
    stSysCfgSetPara.stGuBand.ulBandHigh = 0;
    stSysCfgSetPara.stLBand.ulBandLow   = TAF_PH_BAND_NO_CHANGE;
    stSysCfgSetPara.stLBand.ulBandHigh  = 0;


    /* 执行命令操作 */
    if (VOS_TRUE == TAF_MMA_SetSysCfgReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stSysCfgSetPara))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPAM_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}


TAF_UINT32 At_SetStsfPara(TAF_UINT8 ucIndex)
{
    USIMM_STK_CFG_STRU                  ProfileContent;
    TAF_UINT32                          Result;
    TAF_UINT16                          Len = sizeof(USIMM_STK_CFG_STRU);
    MODEM_ID_ENUM_UINT16                usModemId;

    if(gastAtParaList[0].usParaLen != 1)        /* check the para */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gucAtParaIndex > 3)                          /* para too many */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    Result = NV_ReadEx(MODEM_ID_0, en_NV_Item_TerminalProfile_Set, &ProfileContent, Len);

    if(NV_OK != Result)
    {
        return AT_ERROR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &usModemId))
    {
        return AT_ERROR;
    }

    ProfileContent.ucFuncEnable = (TAF_UINT8)gastAtParaList[0].aucPara[0]-0x30;       /* change the string to Ascii value */

    if(gastAtParaList[1].usParaLen != 0x00)         /* have the <config> para */
    {
        if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara,&gastAtParaList[1].usParaLen)) /* change the string formate */
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if(gastAtParaList[1].usParaLen > 32)        /* according the etsi102.223,the most len or Terminal Profile is 32 */
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (USIMM_CARD_USIM == AT_GetUsimInfoCtxFromModemId(usModemId)->enCardType)
        {
            ProfileContent.stUsimProfile.ucProfileLen = (TAF_UINT8)gastAtParaList[1].usParaLen;

            VOS_MemCpy(ProfileContent.stUsimProfile.aucProfile, gastAtParaList[1].aucPara, ProfileContent.stUsimProfile.ucProfileLen);
        }
        else
        {
            ProfileContent.stSimProfile.ucProfileLen = (TAF_UINT8)gastAtParaList[1].usParaLen;

            VOS_MemCpy(ProfileContent.stSimProfile.aucProfile, gastAtParaList[1].aucPara, ProfileContent.stSimProfile.ucProfileLen);
        }
    }

    if(gastAtParaList[2].usParaLen != 0x00)     /* ave the <Timer>  para */
    {
        Result = At_Auc2ul(gastAtParaList[2].aucPara, gastAtParaList[2].usParaLen, &gastAtParaList[2].ulParaValue);
        if (AT_SUCCESS != Result)
        {
            AT_WARN_LOG("At_SetStsfPara():Warning: At_Auc2ul convert failed!");

            return AT_CME_INCORRECT_PARAMETERS;
        }

        if((gastAtParaList[2].ulParaValue> 0xFF)||(gastAtParaList[2].ulParaValue == 0))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        ProfileContent.ucTimer = (TAF_UINT8)gastAtParaList[2].ulParaValue;
    }

    Result = NV_WriteEx(MODEM_ID_0, en_NV_Item_TerminalProfile_Set, &ProfileContent, Len);

    if(Result != NV_OK)
    {
        return AT_ERROR;
    }

    return AT_OK;
}

/*****************************************************************************
 Prototype      : At_ChangeSATCmdType
 Description    : chang the cmdNo to cmdType
 Input          :
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_ChangeSATCmdType(TAF_UINT32 cmdNo, TAF_UINT32 *CmdType)
{
    TAF_UINT32 Result = AT_SUCCESS;

    switch(cmdNo)
    {
        case SI_AT_CMD_DISPLAY_TEXT:
            *CmdType = SI_STK_DISPLAYTET;
            break;
        case SI_AT_CMD_GET_INKEY:
            *CmdType = SI_STK_GETINKEY;
            break;
        case SI_AT_CMD_GET_INPUT:
            *CmdType = SI_STK_GETINPUT;
            break;
        case SI_AT_CMD_SETUP_CALL:
            *CmdType = SI_STK_SETUPCALL;
            break;
        case SI_AT_CMD_PLAY_TONE:
            *CmdType = SI_STK_PLAYTONE;
            break;
        case SI_AT_CMD_SELECT_ITEM:
            *CmdType = SI_STK_SELECTITEM;
            break;
        case SI_AT_CMD_REFRESH:
            *CmdType = SI_STK_REFRESH;
            break;
        case SI_AT_CMD_SEND_SS:
            *CmdType = SI_STK_SENDSS;
            break;
        case SI_AT_CMD_SEND_SMS:
            *CmdType = SI_STK_SENDSMS;
            break;
        case SI_AT_CMD_SEND_USSD:
            *CmdType = SI_STK_SENDUSSD;
            break;
        case SI_AT_CMD_LAUNCH_BROWSER:
            *CmdType = SI_STK_LAUNCHBROWSER;
            break;
        case SI_AT_CMD_SETUP_IDLE_MODE_TEXT:
            *CmdType = SI_STK_SETUPIDLETEXT;
            break;
        case SI_AT_CMD_LANGUAGENOTIFICATION:
            *CmdType = SI_STK_LANGUAGENOTIFICATION;
            break;
        case SI_AT_CMD_SETFRAMES:
            *CmdType = SI_STK_SETFRAMES;
            break;
        case SI_AT_CMD_GETFRAMESSTATUS:
            *CmdType = SI_STK_GETFRAMESSTATUS;
            break;
        default:
            Result = AT_ERROR;
            break;
    }

    return Result;
}


/*****************************************************************************
 Prototype      : At_SetStgiPara
 Description    : ^STGI
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetStgiPara(TAF_UINT8 ucIndex)
{
    TAF_UINT32 Result;
    TAF_UINT32 CmdType;

    if(TAF_FALSE == g_ulSTKFunctionFlag)
    {
        return AT_ERROR;
    }

    if(0 == gastAtParaList[0].usParaLen)        /* check the para */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gucAtParaIndex != 2)                         /* the para number is must be 2 */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gastAtParaList[0].ulParaValue > 0x0C)        /* the <cmdType> is not more the 12 */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gastAtParaList[0].ulParaValue== 0x00)        /* get the main menu content */
    {
        Result = SI_STK_GetMainMenu(gastAtClientTab[ucIndex].usClientId,0);
    }
    else  if(AT_ERROR == At_ChangeSATCmdType(gastAtParaList[0].ulParaValue, &CmdType))  /* get other SAT content */
    {
        return AT_ERROR;
    }
    else
    {
        Result = SI_STK_GetSTKCommand(gastAtClientTab[ucIndex].usClientId, 0, CmdType);
    }

    if(AT_SUCCESS == Result)
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_STGI_SET;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 At_SetClvlPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8           aucIntraVolume[] = {AT_CMD_CLVL_LEV_0,AT_CMD_CLVL_LEV_1,
                                            AT_CMD_CLVL_LEV_2,AT_CMD_CLVL_LEV_3,
                                            AT_CMD_CLVL_LEV_4,AT_CMD_CLVL_LEV_5};


    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多或者错误 */
    if ((1 != gucAtParaIndex) || (1 != gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 这里做了一个转换，目前后台要求的范围是0~5，而我们对应的范围是0~92，
       根据和媒体的确认，设定范围是0~80，线性对应 */

    /* 返回值为AT_SUCCESS改为VOS_OK，对应起来  */
    if (VOS_OK == APP_VC_SetVoiceVolume(gastAtClientTab[ucIndex].usClientId, 0, aucIntraVolume[gastAtParaList[0].ulParaValue]))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLVL_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 At_SetVMSETPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                          ucVoiceMode;


    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if((1 != gucAtParaIndex) || (1 != gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucVoiceMode = (VOS_UINT8)gastAtParaList[0].ulParaValue;

#if (FEATURE_ON == FEATURE_MULTI_MODEM)
    /* 先设置副卡 */
    if (VOS_OK != APP_VC_SetVoiceMode(gastAtClientTab[ucIndex].usClientId, 0, ucVoiceMode, I1_WUEPS_PID_VC))
    {
        return AT_ERROR;
    }
#endif

    /* 后设置主卡 */
    if (VOS_OK == APP_VC_SetVoiceMode(gastAtClientTab[ucIndex].usClientId, 0, ucVoiceMode, I0_WUEPS_PID_VC))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VMSET_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 Prototype      : At_SetTestPara
 Description    : +TEST=[<n>]
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetTestPara(TAF_UINT8 ucIndex)
{
    return AT_OK;
}


TAF_UINT32 At_SetCwasPara(TAF_UINT8 ucIndex)
{
    AT_MTA_WRR_AUTOTEST_QRY_PARA_STRU   stAtCmd;
    VOS_UINT32                          ulRst;


    /* 参数过多 */
    if ((gucAtParaIndex > 4) || (gucAtParaIndex < 1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    /* 初始化 */
    PS_MEM_SET(&stAtCmd, 0, sizeof(AT_MTA_WRR_AUTOTEST_QRY_PARA_STRU));
    stAtCmd.ucCmd = (TAF_UINT8)gastAtParaList[0].ulParaValue;

    for ( stAtCmd.ucParaNum = 0;
          stAtCmd.ucParaNum < (gucAtParaIndex-1);
          stAtCmd.ucParaNum++)
    {
        stAtCmd.aulPara[stAtCmd.ucParaNum]
            = gastAtParaList[stAtCmd.ucParaNum+1].ulParaValue;
    }


    /* 发送消息给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_WRR_AUTOTEST_QRY_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_WRR_AUTOTEST_QRY_PARA_STRU),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CWAS_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }


}
TAF_UINT32 At_SetCGasPara(TAF_UINT8 ucIndex)
{
    GAS_AT_CMD_STRU                     stAtCmd;
    VOS_UINT32                          ulRst;

    /* 参数过多 */
    if ((gucAtParaIndex > 2) || (gucAtParaIndex < 1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    if ((7 != gastAtParaList[0].ulParaValue)
       &&(gucAtParaIndex > 1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }



    /* 获取命令参数 */
    stAtCmd.ucCmd = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    for (stAtCmd.ucParaNum = 0; stAtCmd.ucParaNum < (gucAtParaIndex-1); stAtCmd.ucParaNum++)
    {
        stAtCmd.aulPara[stAtCmd.ucParaNum] = gastAtParaList[stAtCmd.ucParaNum+1].ulParaValue;
    }

    /* 发送消息DRV_AGENT_GAS_MNTN_CMD给C核处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_GAS_MNTN_CMD,
                                   &stAtCmd,
                                   sizeof(GAS_AT_CMD_STRU),
                                   I0_WUEPS_PID_DRV_AGENT);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGAS_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
VOS_UINT32 At_SetAppDialModePara(VOS_UINT8 ucIndex)
{
     VOS_UINT32                         ulStatus;
     VOS_UINT8                         *pucSystemAppConfig;

     pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    if (SYSTEM_APP_WEBUI != *pucSystemAppConfig)
    {
        return AT_ERROR;
    }


     /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    else
    {
        if (1 == gastAtParaList[0].ulParaValue)
        {
            /*调用底软接口通知应用进行拨号*/
            ulStatus = AT_USB_ENABLE_EVENT;
        }
        else if(0 == gastAtParaList[0].ulParaValue)
        {
            /*调用底软接口通知应用进行断开拨号*/
            ulStatus = AT_USB_SUSPEND_EVENT;
        }
        else
        {
             return AT_CME_INCORRECT_PARAMETERS;
        }

    }

    DRV_AT_SETAPPDAILMODE(ulStatus);

    return AT_OK;
}
TAF_UINT32   At_SetCmmPara(TAF_UINT8 ucIndex)
{
    MM_TEST_AT_CMD_STRU                    stAtCmd;

    /* 参数过多 */
    if( gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(MM_TEST_AT_CMD_STRU));

    stAtCmd.ucCmd = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    for (stAtCmd.ucParaNum = 0; stAtCmd.ucParaNum < (gucAtParaIndex-1); stAtCmd.ucParaNum++)
    {
        stAtCmd.aulPara[stAtCmd.ucParaNum]=gastAtParaList[stAtCmd.ucParaNum+1].ulParaValue;
    }

    /* 发送TAF_MSG_MMA_CRPN_HANDLE消息给MMA处理 */
    if(VOS_OK == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                        0,
                                        TAF_MSG_MM_TEST_HANDLE,
                                        &stAtCmd,
                                        sizeof(MM_TEST_AT_CMD_STRU),
                                        I0_WUEPS_PID_MMA))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MM_TEST_CMD_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}


VOS_VOID AT_ShowCccRst(
    NAS_CC_STATE_INFO_STRU              *pstCcState,
    VOS_UINT16                          *pusLength
)
{
    VOS_UINT16                          usLength;

    usLength = *pusLength;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                      "%d,",
                                      pstCcState->ucCallId);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                      "%d,",
                                      pstCcState->ucTi);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                      "%d,",
                                      pstCcState->enCallState);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                      "%d,",
                                      pstCcState->enHoldAuxState);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                      "%d",
                                      pstCcState->enMptyAuxState);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

    *pusLength = usLength;
}


VOS_UINT32  AT_SetCccPara( TAF_UINT8 ucIndex )
{
    VOS_UINT8                           ucCallId;
    VOS_UINT32                          ulRst;

    if ((gucAtParaIndex > 2) || (0 == gucAtParaIndex))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 == gastAtParaList[0].ulParaValue )
    {
        ucCallId = 0;   /* 0表示查询所有call Id */
    }
    else
    {
        /* gastAtParaList[1].ulParaValue取值无则填取默认值,获取CallId = 1的呼叫状态 */
        if ( 0 == gastAtParaList[1].usParaLen )
        {
            ucCallId = 1;
        }
        else
        {
            ucCallId = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }

    }

    /* 发送消息AT_CC_MSG_STATE_QRY_REQ给CC获取CC协议状态 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   AT_CC_MSG_STATE_QRY_REQ,
                                   &ucCallId,
                                   sizeof(VOS_UINT8),
                                   I0_WUEPS_PID_CC);

    if (VOS_OK == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CC_STATE_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
TAF_UINT32   At_SetSPNPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8                           ucSpnType;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /* 参数过多 */
    if( gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 输入参数取值错误 */
    ucSpnType = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    if ( ucSpnType > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 记录AT^SPN查询的SPN类型 */
    pstNetCtx->ucSpnType = ucSpnType;

    /* 给MMA发送消息，查询新参数类型TAF_PH_SPN_VALUE_PARA */
    if (VOS_OK == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,
                                0,
                                TAF_PH_SPN_VALUE_PARA,
                                TAF_NULL_PTR))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SPN_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32   At_SetCardModePara(TAF_UINT8 ucIndex)
{
    TAF_UINT8               ucCardMode = 0;   /* 0: card not exists, 1: SIM card, 2: USIM card */
    TAF_UINT16              usLength = 0;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulGetModemIdRslt;
    AT_USIM_INFO_CTX_STRU              *pstUsimInfoCtx = VOS_NULL_PTR;

    enModemId = MODEM_ID_0;

    /* 参数过多 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulGetModemIdRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulGetModemIdRslt)
    {
        AT_ERR_LOG("At_SetCardModePara:Get Modem Id fail!");
        return AT_ERROR;
    }

    pstUsimInfoCtx = AT_GetUsimInfoCtxFromModemId(enModemId);

    if (USIMM_CARD_SERVIC_BUTT == pstUsimInfoCtx->enCardStatus)
    {
        AT_ERR_LOG("At_SetCardModePara:Sim card status butt!");
        return AT_CME_SIM_BUSY;
    }

    /* 获取卡模式 */
    if ( USIMM_CARD_SERVIC_ABSENT != pstUsimInfoCtx->enCardStatus)
    {
        if (USIMM_CARD_SIM == pstUsimInfoCtx->enCardType)
        {
            ucCardMode = 1;
        }
        else if (USIMM_CARD_USIM == pstUsimInfoCtx->enCardType)
        {
            ucCardMode = 2;
        }
        else
        {
            ucCardMode = 0;
        }
    }
    else
    {
        ucCardMode = 0;
    }

    /* 输出结果: ^CARDMODE:cardmode */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                       "%s:",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR*)pgucAtSndCodeAddr+usLength,
                                       "%d",
                                       ucCardMode );

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}




TAF_UINT32   At_SetDialModePara(TAF_UINT8 ucIndex)
{
    /*根据终端对该命令的要求，设置命令均返回ERROR*/
    return AT_ERROR;
}
TAF_UINT32   At_SetPortSelPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8               ucPortSel = 0;   /* 0: PCUI, 1: Modem */

    /* 参数过多 */
    if( gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 输入参数取值错误 */
    ucPortSel = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    if ( ucPortSel > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    gucAtPortSel = ucPortSel;

    return AT_OK;
}
TAF_UINT32   At_SetCurcPara(TAF_UINT8 ucIndex)
{
    AT_MTA_CURC_SET_NOTIFY_STRU         stAtCmd;
    VOS_UINT32                          ulResult;

#if  (FEATURE_ON == FEATURE_LTE)
    L4A_IND_CFG_STRU                    stL4AIndCfgReq;
    MODEM_ID_ENUM_UINT16                enModemId;

    enModemId  = MODEM_ID_0;
#endif

    /* 参数检查 */
    if ( (gucAtParaIndex > 2)
      || (gucAtParaIndex < 1) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0, sizeof(stAtCmd));
    stAtCmd.enCurcRptType = (AT_MTA_RPT_GENERAL_CTRL_TYPE_ENUM_UINT8)gastAtParaList[0].ulParaValue;

    /* mode=0时，禁止所有的主动上报 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        PS_MEM_SET(gastAtParaList[1].aucPara, 0x00, sizeof(gastAtParaList[1].aucPara));
    }
    else if (1 == gastAtParaList[0].ulParaValue)
    {
        /* mode=1时，打开所有的主动上报 */
        PS_MEM_SET(gastAtParaList[1].aucPara, 0xFF, sizeof(gastAtParaList[1].aucPara));
    }
    else if (2 == gastAtParaList[0].ulParaValue)
    {
        /* mode=2时，按BIT位控制对应的主动上报,自定义配置主动上报命令需要<report_cfg>参数 */
        if ( (2 != gucAtParaIndex)
          || (AT_CURC_RPT_CFG_PARA_LEN != gastAtParaList[1].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        ulResult = At_AsciiNum2HexString(gastAtParaList[1].aucPara, &gastAtParaList[1].usParaLen);
        if (AT_SUCCESS != ulResult)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_CPY(stAtCmd.aucRptCfg, gastAtParaList[1].aucPara, (AT_CURC_RPT_CFG_MAX_SIZE));

    /* 通知MTA此次curc的设置 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_CURC_SET_NOTIFY,
                                   &stAtCmd,
                                   sizeof(AT_MTA_CURC_SET_NOTIFY_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

#if  (FEATURE_ON == FEATURE_LTE)
    ulResult = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulResult)
    {
        AT_ERR_LOG1("At_SetCurcPara:Get ModemID From ClientID fail,ClientID=%d", ucIndex);
        return AT_ERROR;
    }

    if (VOS_TRUE == AT_IsModemSupportRat(enModemId, TAF_PH_RAT_LTE))
    {
        /* 平台能力支持LTE */
        PS_MEM_SET(&stL4AIndCfgReq, 0, sizeof(stL4AIndCfgReq));
        stL4AIndCfgReq.curc_bit_valid  = 1;
        PS_MEM_CPY(stL4AIndCfgReq.aucCurcCfgReq, gastAtParaList[1].aucPara,
                   sizeof(stL4AIndCfgReq.aucCurcCfgReq));

        /* 通知L此次curc的设置 */
        return AT_SetLIndCfgReq(ucIndex, &stL4AIndCfgReq);
    }
    else
    {
        return AT_OK;
    }
#else
    return AT_OK;
#endif
}


TAF_UINT32   At_SetBOOTPara(TAF_UINT8 ucIndex)
{

    /* 参数过多 */
    if( gucAtParaIndex > 2)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 参数过少 */
    if( gucAtParaIndex < 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 输入参数取值检查, para2必须为 0 */
    if ( (gastAtParaList[1].usParaLen != 1) || (gastAtParaList[1].aucPara[0]!= '0') )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 假命令，直接返回OK. */
    return AT_OK;
}


TAF_UINT32 At_SetCellInfoPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulAtCmdCellInfo;
    VOS_UINT32                          ulRst;

    /* 参数过多 */
    if(gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulAtCmdCellInfo = gastAtParaList[0].ulParaValue;


    /* 发送消息ID_AT_MTA_WRR_CELLINFO_QRY_REQ到C核AT AGENT处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_WRR_CELLINFO_QRY_REQ,
                                   &ulAtCmdCellInfo,
                                   sizeof(VOS_UINT32),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CELLINFO_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }


}


TAF_UINT32  At_GetMeanRpt( TAF_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    /* 参数过多 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送消息ID_AT_MTA_WRR_MEANRPT_QRY_REQ给AT AGENT处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_WRR_MEANRPT_QRY_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MEANRPT_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

/*<--W101153 For Was AutoTest*/

TAF_UINT32 At_SetRRCVersion(TAF_UINT8 ucIndex)
{
    VOS_UINT8                           ucRRCVersion;
    VOS_UINT32                          ulRst;

    /* 参数过多返回参数错误 */
    if(gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucRRCVersion = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 发送消息DRV_AGENT_RRC_VERSION_SET_REQ给AT代理处理 */

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_WRR_RRC_VERSION_SET_REQ,
                                   &ucRRCVersion,
                                   sizeof(VOS_UINT8),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_RRC_VERSION_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}


TAF_UINT32 At_SetCSNR(TAF_UINT8 ucIndex)
{
    /* 参数过多 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}
TAF_UINT32 At_SetFreqLock(TAF_UINT8 ucIndex)
{
    MTA_AT_WRR_FREQLOCK_CTRL_STRU       stFreqLock;
    VOS_UINT32                          ulRst;

    /* 参数个数不正确则返回参数错误 */
    if ( (gucAtParaIndex != 1) && (gucAtParaIndex != 2) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果第一个参数是锁频,但第二个参数频率值没有设置或
       设置的值无效则返回错误*/
    if ( (gastAtParaList[0].ulParaValue == 1)
      && (gastAtParaList[1].usParaLen == 0) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    /* 发送消息赋值 */
    PS_MEM_SET(&stFreqLock, 0, sizeof(MTA_AT_WRR_FREQLOCK_CTRL_STRU));
    stFreqLock.ucFreqLockEnable = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stFreqLock.usLockedFreq     = (VOS_UINT16)gastAtParaList[1].ulParaValue;

    /* 发送消息 ID_AT_MTA_WRR_FREQLOCK_SET_REQ 给C核 AT 代理处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_WRR_FREQLOCK_SET_REQ,
                                   &stFreqLock,
                                   sizeof(MTA_AT_WRR_FREQLOCK_CTRL_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_FREQLOCK_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}


TAF_UINT32 At_SetParaCmd(TAF_UINT8 ucIndex)
{
    AT_RRETURN_CODE_ENUM_UINT32         ulResult = AT_FAILURE;

    /*
    vts这样的命令需要注意，它在参数表中的参数定义为TAF_NULL_PTR，而实际设置参数时，取出设置的参数
    而对于其它无参数命令，需要判断，如果有效参数个数不为0，则返回错误
    */

    if(TAF_NULL_PTR != g_stParseContext[ucIndex].pstCmdElement->pfnSetProc)
    {
        ulResult = (AT_RRETURN_CODE_ENUM_UINT32)g_stParseContext[ucIndex].pstCmdElement->pfnSetProc(ucIndex);

        if(AT_WAIT_ASYNC_RETURN == ulResult)
        {
            g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;

            /* 开定时器 */
            if(AT_SUCCESS != At_StartTimer(g_stParseContext[ucIndex].pstCmdElement->ulSetTimeOut,ucIndex))
            {
                AT_ERR_LOG("At_SetParaCmd:ERROR:Start Timer");
                return AT_ERROR;
            }
        }
        return ulResult;
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32 At_StgrSelMenu(TAF_UINT8 ucIndex)
{
    SI_STK_MENU_SELECTION_STRU *pMenu;
    SI_STK_ENVELOPE_STRU        stENStru;
    VOS_UINT32                  ulRslt;

    /* 参数过多 */
    if(gucAtParaIndex > 4)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stENStru.EnvelopeType    = SI_STK_ENVELOPE_MENUSEL;
    stENStru.DeviceId.ucSDId = SI_DEVICE_KEYPAD;
    stENStru.DeviceId.ucDDId = SI_DEVICE_UICC;

    pMenu = &stENStru.uEnvelope.MenuSelect;
    PS_MEM_SET(pMenu, 0x00, sizeof(SI_STK_MENU_SELECTION_STRU));

    if(SI_AT_RESULT_HELP_REQUIRED == gastAtParaList[2].ulParaValue)
    {
        pMenu->OP_Help        = 1;
        pMenu->Help.aucRsv[0] = 1;
    }

    /*select item*/
    if(0 == gastAtParaList[3].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    else
    {
        ulRslt = At_Auc2ul(gastAtParaList[3].aucPara, gastAtParaList[3].usParaLen, &gastAtParaList[3].ulParaValue);
        if (AT_SUCCESS != ulRslt)
        {
            AT_WARN_LOG("At_StgiSelectItem():Warning: At_Auc2ul convert failed!");
            return AT_ERROR;
        }

        pMenu->OP_ItemIp       = 1;
        pMenu->ItemId.ucItemId = (TAF_UINT8)gastAtParaList[3].ulParaValue;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_STK_MenuSelection(gastAtClientTab[ucIndex].usClientId, 0,&stENStru))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_STGR_SET;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;    /* 返回命令处理挂起状态 */
    }
}

/*****************************************************************************
 Prototype      : At_ChangeCmdResult
 Description    :
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_ChangeCmdResult(TAF_UINT32 resultNo,TAF_UINT8 *cmdResult)
{
    TAF_UINT32 ulResult;

    ulResult = AT_SUCCESS;

    switch(resultNo)
    {
        case SI_AT_RESULT_END_SESSION:
            *cmdResult = PROACTIVE_UICC_SESSION_TERMINATED_BY_THE_USER;
            break;
        case SI_AT_RESULT_PERFORM_SUCC:
            *cmdResult = COMMAND_PERFORMED_SUCCESSFULLY;
            break;
        case SI_AT_RESULT_HELP_REQUIRED:
            *cmdResult = HELP_INFORMATION_REQUIRED_BY_USER;
            break;
        case SI_AT_RESULT_BACKWARD_MOVE:
            *cmdResult = BACKWARD_MOVE_IN_THE_PROACTIVE_UICC_SESSION_REQUESTED_BY_USER;
            break;
        case SI_AT_RESULT_ME_NOT_SUPPORT:
            *cmdResult = COMMAND_BEYOND_TERMINALS_CAPABILITIES;
            break;
        case SI_AT_RESULT_ME_BROWSER_BUSY:
            *cmdResult = TERMINAL_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
        case SI_AT_RESULT_ME_SS_BUSY:
            *cmdResult = TERMINAL_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
        default:
            ulResult= AT_ERROR;
    }

    return ulResult;
}

/*****************************************************************************
 Prototype      : At_StgiGetKey
 Description    :
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_StgiGetKey(TAF_UINT8 ucIndex, SI_STK_TERMINAL_RSP_STRU *pRsp)
{
    TAF_UINT8 cmdResult;
    TAF_UINT8 InputKey[AT_PARA_MAX_LEN + 1];

    if(AT_ERROR == At_ChangeCmdResult(gastAtParaList[2].ulParaValue, &cmdResult))
    {
        return AT_ERROR;
    }

    pRsp->Result.Result = cmdResult;

    if(COMMAND_PERFORMED_SUCCESSFULLY == cmdResult)
    {
        /* 专有参数 */
        pRsp->uResp.GetInkeyRsp.TextStr.pucText = pRsp->aucData;

        VOS_MemCpy(InputKey, gastAtParaList[3].aucPara, gastAtParaList[3].usParaLen);

        /*由于第四个参数是变参的，AT词法解析器无法解析，必须在代码中自行解析处理，
        解析时需要将引号去除，长度减2*/
        if(AT_FAILURE == At_AsciiString2HexText(&pRsp->uResp.GetInkeyRsp.TextStr,\
                                                    InputKey+1, gastAtParaList[3].usParaLen-2))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pRsp->uResp.GetInkeyRsp.OP_TextStr = 1;

        if((gucSTKCmdQualify&0x02)== 0x00)
        {
            pRsp->uResp.GetInkeyRsp.TextStr.ucDCS = 0x04;
        }
        else
        {
            pRsp->uResp.GetInkeyRsp.TextStr.ucDCS = 0x08;
        }

        if((gucSTKCmdQualify&0x04)== 0x04)
        {
            *pRsp->uResp.GetInkeyRsp.TextStr.pucText -=0x30;
        }
    }

    return AT_SUCCESS;
}
/*****************************************************************************
 Prototype      : At_StgiGetInput
 Description    :
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_StgiGetInput(TAF_UINT8 ucIndex,SI_STK_TERMINAL_RSP_STRU *pRsp)
{
    TAF_UINT8 cmdResult;
    TAF_UINT8 InputString[AT_PARA_MAX_LEN + 1];

    if(AT_ERROR == At_ChangeCmdResult(gastAtParaList[2].ulParaValue, &cmdResult))
    {
        return AT_ERROR;
    }

    pRsp->Result.Result = cmdResult;

    if(COMMAND_PERFORMED_SUCCESSFULLY == cmdResult)
    {
        /* 专有参数 */
        if(0x03 >  gastAtParaList[3].usParaLen)
        {
            pRsp->uResp.GetInkeyRsp.TextStr.pucText = TAF_NULL_PTR;

            pRsp->uResp.GetInkeyRsp.TextStr.ucLen = 0;
        }
        else
        {
            pRsp->uResp.GetInkeyRsp.TextStr.pucText = pRsp->aucData;

            VOS_MemCpy(InputString, gastAtParaList[3].aucPara, gastAtParaList[3].usParaLen);

            /*由于第四个参数是变参的，AT词法解析器无法解析，必须在代码中自行解析处理，解析时需要将引号去除，长度减2*/
            if(AT_FAILURE == At_AsciiString2HexText(&pRsp->uResp.GetInputRsp.TextStr,\
                                            InputString+1, gastAtParaList[3].usParaLen-2))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }

        pRsp->uResp.GetInputRsp.OP_TextStr = 1;

        if((gucSTKCmdQualify&0x02)== 0x00)
        {
            if((gucSTKCmdQualify&0x08)== 0x00)
            {
                pRsp->uResp.GetInkeyRsp.TextStr.ucDCS = 0x04;
            }
            else
            {
                pRsp->uResp.GetInkeyRsp.TextStr.ucDCS = 0x00;
            }
        }
        else
        {
            pRsp->uResp.GetInkeyRsp.TextStr.ucDCS = 0x08;
        }
    }

    return AT_SUCCESS;
}

TAF_UINT32 At_StgiSelectItem(TAF_UINT8 ucIndex,SI_STK_TERMINAL_RSP_STRU *pRsp)
{
    TAF_UINT8       cmdResult;
    VOS_UINT32      ulRslt;

    if(AT_ERROR == At_ChangeCmdResult(gastAtParaList[2].ulParaValue, &cmdResult))
    {
        return AT_ERROR;
    }

    pRsp->Result.Result = cmdResult;

    if(COMMAND_PERFORMED_SUCCESSFULLY == cmdResult)
    {
        ulRslt = At_Auc2ul(gastAtParaList[3].aucPara, gastAtParaList[3].usParaLen, &gastAtParaList[3].ulParaValue);
        if (AT_SUCCESS != ulRslt)
        {
            AT_WARN_LOG("At_StgiSelectItem():Warning: At_Auc2ul convert failed!");
            return AT_ERROR;
        }

        /* 专有参数 */
        pRsp->uResp.SelectItemRsp.ItemId.ucItemId = (TAF_UINT8)gastAtParaList[3].ulParaValue;
        pRsp->uResp.SelectItemRsp.OP_ItemID       = 1;
    }

    return AT_SUCCESS;
}

/*****************************************************************************
 Prototype      : At_SetStgrPara
 Description    : ^STGR=<cmdnum>,<cmdtype>,<result>,<data>
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetStgrPara(TAF_UINT8 ucIndex)
{
    SI_STK_TERMINAL_RSP_STRU Rsponse;
    TAF_UINT32 Result = 0;

#if ( VOS_WIN32 != VOS_OS_VER )
    if(TAF_FALSE == g_ulSTKFunctionFlag)
    {
        return AT_ERROR;
    }
#endif

    if((gucAtParaIndex > 4)||(gucAtParaIndex < 3))          /* the para munber is too many or too few */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if((0 == gastAtParaList[0].usParaLen)||(0 == gastAtParaList[1].usParaLen))  /* the para1 and para2 is must be inputed */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    Rsponse.DeviceId.ucSDId = SI_DEVICE_KEYPAD;
    Rsponse.DeviceId.ucDDId = SI_DEVICE_UICC;

    /* At_Auc2ul(gastAtParaList[3].aucPara, gastAtParaList[3].usParaLen, &gastAtParaList[3].ulParaValue); */

    if(gastAtParaList[0].ulParaValue == 0x00)
    {
        if((gastAtParaList[2].ulParaValue== 1)||(gastAtParaList[2].ulParaValue == 2))   /* menu selection or need the help infomation */
        {
            return At_StgrSelMenu(ucIndex);
        }
        else
        {
            return AT_OK;
        }
    }

    PS_MEM_SET(&Rsponse, 0, sizeof(SI_STK_TERMINAL_RSP_STRU));

    if(SI_AT_CMD_END_SESSION == gastAtParaList[0].ulParaValue)
    {
        Rsponse.Result.Result = PROACTIVE_UICC_SESSION_TERMINATED_BY_THE_USER;
    }
    else if(AT_SUCCESS != At_ChangeSATCmdType(gastAtParaList[0].ulParaValue, &Rsponse.CmdType))
    {
        return AT_ERROR;
    }
    else
    {

    }

    switch(gastAtParaList[0].ulParaValue)
    {
        case SI_AT_CMD_GET_INKEY:
            Result = At_StgiGetKey(ucIndex, &Rsponse);       /* change the GetInkey response content */
            break;
        case SI_AT_CMD_GET_INPUT:
            Result = At_StgiGetInput(ucIndex, &Rsponse);     /* change the GetInput response content */
            break;
        case SI_AT_CMD_SELECT_ITEM:
            Result = At_StgiSelectItem(ucIndex, &Rsponse);   /* change the SelectItem response content */
            break;
        default:
            Result = At_ChangeCmdResult(gastAtParaList[2].ulParaValue, &Rsponse.Result.Result);
            break;
    }

    if(AT_SUCCESS != Result)
    {
        return Result;    /* return the error */
    }

    if(AT_SUCCESS == SI_STK_TerminalResponse(gastAtClientTab[ucIndex].usClientId, 0,&Rsponse))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_STGR_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 Prototype      : At_SetCstrPara
 Description    : ^STGR=<Len>,<data>
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCstrPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8   aucData[256];
    VOS_UINT8   ucDataLen;
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();



    if ( SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        return AT_ERROR;
    }

    if (gucAtParaIndex != 2)          /* the para munber is too many or too few */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((0 == gastAtParaList[0].usParaLen)||(0 == gastAtParaList[1].usParaLen))  /* the para1 and para2 is must be inputed */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_FAILURE == At_AsciiString2HexSimple(aucData, gastAtParaList[1].aucPara, (VOS_UINT16)gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucDataLen = (TAF_UINT8)(gastAtParaList[0].ulParaValue/2);

    if (AT_SUCCESS == SI_STK_DataSendSimple(gastAtClientTab[ucIndex].usClientId, 0, SI_STK_TRSEND, ucDataLen, aucData))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSTR_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


/*****************************************************************************
 Prototype      : At_SetCsinPara
 Description    : ^CSIN命令处理函数
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2012-05-17
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCsinPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 非ANDROID系统不支持 */
    if ( SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        return AT_ERROR;
    }

    /* 获取最后一次收到的主动命令，不将命令类型传入 */
    if (AT_SUCCESS == SI_STK_GetSTKCommand(gastAtClientTab[ucIndex].usClientId, 0, 0))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSEN_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 Prototype      : At_SetCstrPara
 Description    : ^STGR=<Len>,<data>
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCsenPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8   aucData[256];
    VOS_UINT8   ucDataLen;
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();



    if ( SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        return AT_ERROR;
    }

    if (gucAtParaIndex != 2)          /* the para munber is too many or too few */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((0 == gastAtParaList[0].usParaLen)||(0 == gastAtParaList[1].usParaLen))  /* the para1 and para2 is must be inputed */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_FAILURE == At_AsciiString2HexSimple(aucData, gastAtParaList[1].aucPara, (VOS_UINT16)gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucDataLen = (TAF_UINT8)(gastAtParaList[0].ulParaValue/2);

    if (AT_SUCCESS == SI_STK_DataSendSimple(gastAtClientTab[ucIndex].usClientId, 0, SI_STK_ENVELOPESEND, ucDataLen, aucData))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSEN_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 Prototype      : At_SetCsmnPara
 Description    : ^CSMN
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCsmnPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();



    if ( SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        return AT_ERROR;
    }

    if (gucAtParaIndex != 0)          /* the para munber is too many or too few */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SUCCESS == SI_STK_GetMainMenu(gastAtClientTab[ucIndex].usClientId, 0))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSEN_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32 At_SetCstcPara(TAF_UINT8 ucIndex)
{
    if (gucAtParaIndex != 1)          /* the para munber is too many or too few */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SUCCESS == SI_STK_SetUpCallConfirm(gastAtClientTab[ucIndex].usClientId, gastAtParaList[0].ulParaValue))
    {
        return AT_OK;
    }

    return AT_ERROR;
}


/*****************************************************************************
 Prototype      : At_SetImsichgPara
 Description    : ^STGR=<cmdnum>,<cmdtype>,<result>,<data>
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2010-02-11
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetImsichgPara(TAF_UINT8 ucIndex)
{
    if (VOS_FALSE == SI_STKIsDualImsiSupport())
    {
        return AT_CMD_NOT_SUPPORT;
    }

    if (1 != gucAtParaIndex)          /* the para munber is too many or too few */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 == gastAtParaList[0].usParaLen)  /* the para1 must be inputed */
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SUCCESS == SI_STKDualIMSIChangeReq(gastAtClientTab[ucIndex].usClientId, 0))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_IMSICHG_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32  AT_PhyNumIsNull(
    MODEM_ID_ENUM_UINT16                enModemId,
    AT_PHYNUM_TYPE_ENUM_UINT32          enSetType,
    VOS_BOOL                           *pbPhyNumIsNull
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulTotalNum;
    IMEI_STRU                           stImei;
    TAF_SVN_DATA_STRU                   stSvnData;
    VOS_UINT8                           *pucCheckNum;
    VOS_UINT8                           aucMac[AT_PHYNUM_MAC_LEN + 1]; /* MAC地址*/


    PS_MEM_SET(&stSvnData, 0x00, sizeof(stSvnData));


    /* 从获取NV中IMEI或SVN物理号，获取操作失败，则返回AT_ERROR */
    if (AT_PHYNUM_TYPE_IMEI == enSetType)
    {
        ulRet = NV_ReadEx(enModemId, en_NV_Item_IMEI, &stImei, NV_ITEM_IMEI_SIZE);
        if (NV_OK != ulRet)
        {
            AT_WARN_LOG("AT_PhyNumIsNull: Fail to read en_NV_Item_IMEI");
            return AT_ERROR;
        }

        /*NV项en_NV_Item_IMEI中仅15位是UE的IMEI号，最后一位是无效位不作比较*/
        ulTotalNum  = TAF_IMEI_DATA_LENGTH;
        pucCheckNum = stImei.aucImei;
    }
    else if (AT_PHYNUM_TYPE_SVN == enSetType)
    {
        ulRet = NV_ReadEx(enModemId, en_NV_Item_Imei_Svn, &stSvnData, sizeof(stSvnData));
        if (NV_OK != ulRet)
        {
            AT_WARN_LOG("AT_PhyNumIsNull: Fail to read en_Nv_Item_Imei_Svn");
            return AT_ERROR;
        }

        /* SVN未激活按物理号为空处理 */
        if (NV_ITEM_ACTIVE != stSvnData.ucActiveFlag)
        {
            *pbPhyNumIsNull = VOS_TRUE;
            return AT_OK;
        }

        ulTotalNum  = TAF_SVN_DATA_LENGTH;
        pucCheckNum = stSvnData.aucSvn;
    }
    else if (AT_PHYNUM_TYPE_MAC == enSetType)
    {
        /* 输出当前MAC地址是否为空状态 */
        AT_GetPhynumMac(aucMac);
        ulTotalNum  = AT_PHYNUM_MAC_LEN;
        pucCheckNum = aucMac;
    }
    else
    {
        AT_WARN_LOG("AT_PhyNumIsNull: the type of physical is error.");
        return AT_ERROR;
    }

    /* NV中IMEI或SVN物理号为全0认为物理号为空；否则，非空 */
    for (ulLoop = 0; ulLoop < ulTotalNum; ulLoop++)
    {
        if (0 != *pucCheckNum)
        {
            *pbPhyNumIsNull = VOS_FALSE;
            return AT_OK;
        }

        pucCheckNum++;
    }

    *pbPhyNumIsNull = VOS_TRUE;

    return AT_OK;
}


VOS_UINT32 AT_UpdateImei(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           aucImei[],
    VOS_UINT16                          usImeiLength
)
{
    VOS_UINT8                           aucBcdNum[TAF_PH_IMEI_LEN];
    VOS_UINT8                           ucCheckData;
    VOS_UINT32                          i;

    /* IMEI 长度检查: 用户输入的IMEI长度应为14位的IMEI和1位的CHECK NUM，共15位 */
    if ((TAF_PH_IMEI_LEN - 1) != usImeiLength)
    {
        AT_NORM_LOG1("AT_UpdateImei: the length of imei is error ", (VOS_INT32)usImeiLength);
        return AT_PHYNUM_LENGTH_ERR;
    }

    /* IMEI 号码检查: 确认<imei>为数字字符串, 并将ASCII码转换成BCD码 */
    if (AT_SUCCESS != At_AsciiNum2Num(aucBcdNum, aucImei, usImeiLength))
    {
        AT_NORM_LOG("AT_UpdateImei: the number of imei is error.");
        return AT_PHYNUM_NUMBER_ERR;
    }
    aucBcdNum[TAF_PH_IMEI_LEN - 1] = 0;

    /* IMEI 校验检查: IMEI信息由IMEI(TAC8位,SNR6位)和校验位两部分组成
       参考协议: 3GPP 23003 B.2 Computation of CD for an IMEI和B.3 Example of computation */
    ucCheckData = 0;
    for (i = 0; i < (TAF_IMEI_DATA_LENGTH - 1); i += 2)
    {
        ucCheckData += aucBcdNum[i]
                       + ((aucBcdNum[i + 1UL] + aucBcdNum[i + 1UL]) / 10)
                       + ((aucBcdNum[i + 1UL] + aucBcdNum[i + 1UL]) % 10);
    }
    ucCheckData = (10 - (ucCheckData % 10)) % 10;

    if (ucCheckData != aucBcdNum[TAF_PH_IMEI_LEN - 2])
    {
        AT_NORM_LOG("AT_UpdateImei: the checkdata of imei is error.");
        return AT_PHYNUM_NUMBER_ERR;
    }

    if (NV_OK != NV_WriteEx(enModemId, en_NV_Item_IMEI, aucBcdNum, TAF_PH_IMEI_LEN))
    {
         AT_WARN_LOG("AT_UpdateImei: Fail to write NV.");
         return AT_ERROR;
    }

    return AT_OK;
}



TAF_UINT32 At_SetImeiPara(TAF_UINT8 ucIndex)
{
    VOS_BOOL                            bPhyNumIsNull;
    VOS_UINT32                          ulRet;
    MODEM_ID_ENUM_UINT16                enModemId;

    enModemId = MODEM_ID_0;

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        AT_NORM_LOG1("At_SetImeiPara: the number of parameter is error ",
                     (VOS_INT32)gucAtParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("At_SetImeiPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* 物理号不为空，数据保护未解锁，直接返回错误，错误码为1物理号不合法。 */
    if (AT_OK != AT_PhyNumIsNull(enModemId, AT_PHYNUM_TYPE_IMEI, &bPhyNumIsNull))
    {
        return AT_ERROR;
    }

    if ((VOS_FALSE == bPhyNumIsNull) && (VOS_TRUE == g_bAtDataLocked))
    {
        AT_NORM_LOG("At_SetImeiPara: physical number is not null and data locked.");
        return AT_PHYNUM_TYPE_ERR;
    }

    /* 更新数据到NV项en_NV_Item_IMEI并返回更新结果 */
    ulRet = AT_UpdateImei(enModemId, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    return ulRet;
}
TAF_UINT32 At_SetFPlmnPara(TAF_UINT8 ucIndex)
{
    TAF_PH_FPLMN_OPERATE_STRU stFplmnOperate;


    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if( gucAtParaIndex > 2 )

    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 1 == gucAtParaIndex )
    {
        if ( 0 != gastAtParaList[0].ulParaValue )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        else
        {
            stFplmnOperate.stPlmn.Mcc = 0;
            stFplmnOperate.stPlmn.Mnc = 0;
            stFplmnOperate.ulCmdType = TAF_PH_FPLMN_DEL_ALL;
        }
    }
    else if ( 2 == gucAtParaIndex )
    {
        if(gastAtParaList[1].usParaLen > 6)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        /* Mcc */
        if(AT_FAILURE == At_String2Hex(gastAtParaList[1].aucPara,3,&stFplmnOperate.stPlmn.Mcc))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        /* Mnc */
        if(AT_FAILURE == At_String2Hex(&gastAtParaList[1].aucPara[3],
                                        gastAtParaList[1].usParaLen-3,
                                        &stFplmnOperate.stPlmn.Mnc))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        stFplmnOperate.stPlmn.Mcc &= 0x0FFF;

        if (5==gastAtParaList[1].usParaLen)
        {
            stFplmnOperate.stPlmn.Mnc |= 0x0F00;
        }

        stFplmnOperate.stPlmn.Mnc &= 0x0FFF;

        if ( 0 == gastAtParaList[0].ulParaValue)
        {
            stFplmnOperate.ulCmdType = TAF_PH_FPLMN_DEL_ONE;
        }
        else if ( 1 == gastAtParaList[0].ulParaValue)
        {
            stFplmnOperate.ulCmdType = TAF_PH_FPLMN_ADD_ONE;
        }
        else
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_FplmnHandle(gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     stFplmnOperate))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CFPLMN_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }

}
TAF_UINT32 At_SetQuickStart(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if(1 != gucAtParaIndex)
    {
        return AT_ERROR;
    }

    if (gastAtParaList[0].ulParaValue > 1)
    {
        return AT_ERROR;
    }

    /* 执行命令操作 */
    if (AT_SUCCESS == TAF_DefQuickStart(gastAtClientTab[ucIndex].usClientId,
                      0,
                      gastAtParaList[0].ulParaValue))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CQST_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetAutoAttach(TAF_UINT8 ucIndex)
{
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {                                                                           /* 参数过多 */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gastAtParaList[0].ulParaValue > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SUCCESS == Taf_DefPhAutoAttach(gastAtClientTab[ucIndex].usClientId,
                      0,
                      gastAtParaList[0].ulParaValue))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CAATT_SET;
        return AT_WAIT_ASYNC_RETURN;                                            /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 At_SetSystemInfo(TAF_UINT8 ucIndex)
{
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   TAF_PH_SYSINFO_VALUE_PARA,
                                   TAF_NULL_PTR))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SYSINFO_READ;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_GetBandPara(TAF_UINT8 *nptr,TAF_UINT16 usLen,TAF_USER_SET_PREF_BAND64 * pStBand)
{
    const TAF_UINT8 ucHexLenOfUlongType = 8;
    TAF_UINT32  ulAtRslt1 = AT_SUCCESS;
    TAF_UINT32  ulAtRslt2 = AT_SUCCESS;

    pStBand->ulBandHigh = 0;
    pStBand->ulBandLow = 0;

    /* 频段项的设置  */
    if (usLen <= ucHexLenOfUlongType)
    {
         ulAtRslt1 = At_String2Hex(nptr,usLen,&(pStBand->ulBandLow));
    }
    else
    {
        ulAtRslt1 = At_String2Hex(nptr,usLen-ucHexLenOfUlongType,&(pStBand->ulBandHigh));
        ulAtRslt2 = At_String2Hex(nptr+(usLen-ucHexLenOfUlongType),ucHexLenOfUlongType,&(pStBand->ulBandLow));
    }

    if ( ( AT_FAILURE == ulAtRslt1 ) || (  AT_FAILURE == ulAtRslt2))
    {
        return  AT_FAILURE;
    }

    return AT_SUCCESS;

}



VOS_VOID At_ConvertSysCfgExRat(
    VOS_UINT8                           ucClientId,
    AT_SYSCFGEX_RAT_ORDER_STRU         *pstSysCfgExRatOrder,
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    VOS_UINT32                          i;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRst;

    enModemId = MODEM_ID_0;

    /* 获取ModemID*/
    ulRst = AT_GetModemIdFromClient(ucClientId, &enModemId);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG1("At_ConvertSysCfgExRat:Get ModemID From ClientID fail,ClientID=%d", ucClientId);
        return;
    }

    /* 组合原因值00的接入技术转换 */
    if ( (1 == pstSysCfgExRatOrder->ucRatOrderNum)
      && (AT_SYSCFGEX_RAT_AUTO == pstSysCfgExRatOrder->aenRatOrder[0]))
    {
        AT_SetDefaultRatPrioList(enModemId, pstRatOrder, VOS_TRUE);
        return ;
    }

    /* 组合原因值99的接入技术转换 */
    if ( (1 == pstSysCfgExRatOrder->ucRatOrderNum)
      && (AT_SYSCFGEX_RAT_NO_CHANGE == pstSysCfgExRatOrder->aenRatOrder[0]) )
    {
        AT_GetRatOrderNvim(enModemId, pstRatOrder, VOS_TRUE);
        return ;
    }

    /* 非组合原因值的接入技术转换 */
    pstRatOrder->ucRatOrderNum = pstSysCfgExRatOrder->ucRatOrderNum;
    for (i = 0; i < pstSysCfgExRatOrder->ucRatOrderNum; i++)
    {
        switch ( pstSysCfgExRatOrder->aenRatOrder[i] )
        {
#if (FEATURE_ON == FEATURE_LTE)
            case AT_SYSCFGEX_RAT_LTE:
                pstRatOrder->aenRatOrder[i] = TAF_PH_RAT_LTE;
                break;
#endif

            case AT_SYSCFGEX_RAT_WCDMA:
                pstRatOrder->aenRatOrder[i] = TAF_PH_RAT_WCDMA;
                break;

            case AT_SYSCFGEX_RAT_GSM:
                pstRatOrder->aenRatOrder[i] = TAF_PH_RAT_GSM;
                break;

            default:
                AT_ERR_LOG("At_ConvertSysCfgExRat");
                break;
        }

    }

}
VOS_VOID At_ConvertDetachTypeToServiceDomain(
    TAF_PH_DETACH_TYPE                  ucUserDetachType,
    TAF_PH_SERVICE_DOMAIN              *pucDomain
)
{
    switch ( ucUserDetachType )
    {
        case TAF_PH_PS_OPERATE :
            *pucDomain = TAF_PH_SERVICE_PS;
            break;

        case TAF_PH_CS_OPERATE :
            *pucDomain = TAF_PH_SERVICE_CS;
            break;

        case TAF_PH_PS_CS_OPERATE :
            *pucDomain = TAF_PH_SERVICE_CS_PS;
            break;

        default:

            AT_ERR_LOG("At_ConvertDetachTypeToServiceDomain Detach Type ERROR");
            *pucDomain = TAF_PH_SERVICE_CS_PS;
            break;
    }

    return;
}





VOS_VOID AT_ReadRoamCfgInfoNvim(
    VOS_UINT8                           ucIndex
)
{
    NAS_NVIM_ROAM_CFG_INFO_STRU          stRoamFeature;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRst;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    enModemId = MODEM_ID_0;
    ulLength  = 0;

    /* 获取ModemId */
    ulRst = AT_GetModemIdFromClient(ucIndex, &enModemId);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG1("AT_ReadRoamCfgInfoNvim:Get ModemID From ClientID fail,ucIndex=%d", ucIndex);
        return;
    }


    stRoamFeature.ucRoamFeatureFlg = NV_ITEM_ACTIVE;
    stRoamFeature.ucRoamCapability = 0;

    NV_GetLength(en_NV_Item_Roam_Capa, &ulLength);
    if (NV_OK != NV_ReadEx(enModemId, en_NV_Item_Roam_Capa,
                         &stRoamFeature, ulLength))
    {
        AT_WARN_LOG("AT_ReadRoamCfgInfoNvim():WARNING: en_NV_Item_Roam_Capa Error");

        return;
    }

    if ((NV_ITEM_ACTIVE != stRoamFeature.ucRoamFeatureFlg)
     && (NV_ITEM_DEACTIVE != stRoamFeature.ucRoamFeatureFlg))
    {
        return;
    }

    pstNetCtx                = AT_GetModemNetCtxAddrFromModemId(enModemId);
    pstNetCtx->ucRoamFeature = stRoamFeature.ucRoamFeatureFlg;
    return;
}
VOS_UINT32 AT_ConvertSysCfgExRatOrderPara(
    VOS_UINT8                           ucClientId,
    TAF_PH_RAT_ORDER_STRU              *pstSysCfgExRatOrder
)
{
    VOS_UINT32                          ulRst;
    VOS_UINT32                          i;
    VOS_UINT32                          ulRatOrder;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRatAutoNoChangeExistFlag;
    AT_SYSCFGEX_RAT_ORDER_STRU          stSyscfgExRatOrder;

    ucIndex                    = 0;
    ulRatAutoNoChangeExistFlag = VOS_FALSE;

    /* acqorder字符串长度为奇数 */
    if ((gastAtParaList[0].usParaLen % 2) != 0)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* acqorder字符串含非数字 */
    ulRst = At_CheckNumString(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);
    if (ulRst != AT_SUCCESS)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* acqorder字符串长度是偶数但大于6也非法,是01/02/03的组合 */
    if ((gastAtParaList[0].usParaLen / 2) > TAF_PH_MAX_GUL_RAT_NUM)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    for (i = 0; i < gastAtParaList[0].usParaLen; i += AT_SYSCFGEX_RAT_MODE_STR_LEN)
    {
        ulRst = At_String2Hex(&gastAtParaList[0].aucPara[i], AT_SYSCFGEX_RAT_MODE_STR_LEN, (VOS_UINT32 *)&ulRatOrder);
        if (AT_SUCCESS != ulRst)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 如果ulRatOrder是0x99十六进制数，需要转换为十进制的99 */
        if (AT_SYSCFGEX_RAT_NO_CHANGE_HEX_VALUE == ulRatOrder)
        {
            ulRatOrder = AT_SYSCFGEX_RAT_NO_CHANGE;
        }

        /* acqorder字符串含非00/01/02/03/99的组合 */
        if ( VOS_FALSE == At_CheckSysCfgExRatTypeValid((AT_SYSCFGEX_RAT_TYPE_ENUM_UINT8)ulRatOrder) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if ((AT_SYSCFGEX_RAT_AUTO == (AT_SYSCFGEX_RAT_TYPE_ENUM_UINT8)ulRatOrder)
         || (AT_SYSCFGEX_RAT_NO_CHANGE == (AT_SYSCFGEX_RAT_TYPE_ENUM_UINT8)ulRatOrder))
        {
            ulRatAutoNoChangeExistFlag = VOS_TRUE;
        }

        stSyscfgExRatOrder.aenRatOrder[ucIndex++] = (AT_SYSCFGEX_RAT_TYPE_ENUM_UINT8)ulRatOrder;

    }

    stSyscfgExRatOrder.ucRatOrderNum = ucIndex;

    /* acqorder字符串是00，99和其他值的组合 */
    if ((VOS_TRUE == ulRatAutoNoChangeExistFlag)
     && (stSyscfgExRatOrder.ucRatOrderNum > 1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* acqorder字符串模式设置有重复 */
    if ( VOS_TRUE == At_CheckRepeatedRatOrder(&stSyscfgExRatOrder) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 将SYSCFGEX的接入技术形式转换为TAF_PH_RAT_ORDER_STRU结构格式 */
    At_ConvertSysCfgExRat(ucClientId, &stSyscfgExRatOrder, pstSysCfgExRatOrder);

    return AT_OK;
}


VOS_UINT32 At_CheckRepeatedRatOrder(
    AT_SYSCFGEX_RAT_ORDER_STRU         *pstSyscfgExRatOrder
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;

    for (i = 0; i < pstSyscfgExRatOrder->ucRatOrderNum; i++)
    {
        for (j = i + 1; j < pstSyscfgExRatOrder->ucRatOrderNum; j++)
        {
            if (pstSyscfgExRatOrder->aenRatOrder[i] == pstSyscfgExRatOrder->aenRatOrder[j])
            {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 At_CheckSysCfgExRatTypeValid(
    VOS_UINT8                           ucRatOrder
)
{
    switch ( ucRatOrder )
    {
        case AT_SYSCFGEX_RAT_AUTO:
        case AT_SYSCFGEX_RAT_NO_CHANGE:
        case AT_SYSCFGEX_RAT_LTE:
        case AT_SYSCFGEX_RAT_WCDMA:
        case AT_SYSCFGEX_RAT_GSM:
            return VOS_TRUE;

        default:
            return VOS_FALSE;
    }

}



VOS_UINT32 AT_ConvertSysCfgExBandPara(
    VOS_UINT8                          *pucPara,
    VOS_UINT16                          usLen,
    TAF_USER_SET_PREF_BAND64           *pstPrefBandPara
)
{
    VOS_UINT32                          ulRslt;

    pstPrefBandPara->ulBandHigh = 0;
    pstPrefBandPara->ulBandLow  = 0;

    /* 如果频段为空，返回失败*/
    if (0 == usLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 频段值小于32位时 */
    if (usLen <= AT_HEX_LEN_OF_ULONG_TYPE)
    {
        ulRslt = At_String2Hex(pucPara, usLen, &(pstPrefBandPara->ulBandLow));
    }
    else
    {
        /* 64位频段的高32位字符串转换为16进制数，转换失败返回 */
        ulRslt = At_String2Hex(pucPara, (usLen - AT_HEX_LEN_OF_ULONG_TYPE),
                               &(pstPrefBandPara->ulBandHigh));
        if (AT_FAILURE == ulRslt)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 64位频段的低32位字符串转换为16进制数，转换失败返回  */
        ulRslt = At_String2Hex(pucPara + (usLen - AT_HEX_LEN_OF_ULONG_TYPE),
                               AT_HEX_LEN_OF_ULONG_TYPE, &(pstPrefBandPara->ulBandLow));
    }

    if (AT_FAILURE == ulRslt)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;

}


VOS_UINT32 AT_CheckSysCfgExPara(
    VOS_UINT8                           ucClientId,
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgExSetPara
)
{
    VOS_UINT32                          ulRst;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucClientId);

    /* 检查参数个数,如果不为7个，返回AT_CME_INCORRECT_PARAMETERS*/
    if ( AT_SYSCFGEX_PARA_NUM != gucAtParaIndex )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 检查acqorder参数合法性,如果合法,进行转换并记录,如果不合法直接返回失败*/
    ulRst = AT_ConvertSysCfgExRatOrderPara(ucClientId, &pstSysCfgExSetPara->stRatOrder);
    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    pstSysCfgExSetPara->enUserPrio = TAF_MMA_USER_SET_PRIO_NOCHANGE;
    if ( VOS_FALSE == AT_IsSupportLMode(&pstSysCfgExSetPara->stRatOrder))
    {
        if ( TAF_PH_RAT_GSM == pstSysCfgExSetPara->stRatOrder.aenRatOrder[0])
        {
            pstSysCfgExSetPara->enUserPrio = TAF_MMA_USER_SET_PRIO_GSM_PREFER;
        }
        else
        {
            pstSysCfgExSetPara->enUserPrio = TAF_MMA_USER_SET_PRIO_AUTO;
        }
    }

    /* 如果设置的模式里有L,且服务域是cs only，返回失败 */
    if ((VOS_TRUE == AT_IsSupportLMode(&pstSysCfgExSetPara->stRatOrder))
     && ((TAF_PH_SERVICE_CS  == gastAtParaList[3].ulParaValue)
      || (TAF_PH_SERVICE_ANY == gastAtParaList[3].ulParaValue)))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 检查band参数合法性,如果合法,进行转换并记录,如果不合法直接返回失败*/
    ulRst = AT_ConvertSysCfgExBandPara(gastAtParaList[1].aucPara,
                                       gastAtParaList[1].usParaLen,
                                       &pstSysCfgExSetPara->stGuBand);
    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 漫游特性未激活,漫游能力取值范围0-2; 漫游特性激活,漫游能力取值范围0-3 */
    AT_ReadRoamCfgInfoNvim(ucClientId);
    if (((AT_ROAM_FEATURE_OFF == pstNetCtx->ucRoamFeature) && (gastAtParaList[2].ulParaValue > AT_ROAM_FEATURE_OFF_NOCHANGE))
     || ((AT_ROAM_FEATURE_ON == pstNetCtx->ucRoamFeature) && (gastAtParaList[2].ulParaValue > TAF_PH_ROAM_NATIONAL_OFF_INTERNATIONAL_OFF)))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    pstSysCfgExSetPara->ucRoam = (VOS_UINT8)gastAtParaList[2].ulParaValue;

    /* 检查lteband参数合法性,如果合法,进行转换并记录,如果不合法直接返回失败*/
    ulRst = AT_ConvertSysCfgExBandPara(gastAtParaList[4].aucPara,
                                       gastAtParaList[4].usParaLen,
                                       &pstSysCfgExSetPara->stLBand);
    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    pstSysCfgExSetPara->ucSrvDomain = (VOS_UINT8)gastAtParaList[3].ulParaValue;

    return ulRst;

}



VOS_UINT32 AT_SetSysCfgExPara(
    VOS_UINT8                           ucIndex
)
{
    TAF_MMA_SYS_CFG_PARA_STRU           stSysCfgExSetPara;

    VOS_UINT32                          ulRst;

    PS_MEM_SET(&stSysCfgExSetPara, 0, sizeof(stSysCfgExSetPara));


    /* 参数个数和合法性检查,不合法直接返回失败 */
    ulRst = AT_CheckSysCfgExPara(ucIndex, &stSysCfgExSetPara);

    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    /* 与MMA进行交互不成功返回失败，交互成功返回命令处理挂起状态 */
    ulRst = TAF_MMA_SetSysCfgReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stSysCfgExSetPara);

    if (VOS_TRUE == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SYSCFG_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
VOS_UINT32 AT_GetValidNetScanPowerParam(
    VOS_INT16                          *psCellPower
)
{
    VOS_UINT32                          ulCellPower;

    ulCellPower = 0;

    /* power[-110,-47]长度大于4或小于3表示无效参数 */
    if ( (AT_NETSCAN_POWER_PARA_MAX_LEN < gastAtParaList[1].usParaLen)
      || (AT_NETSCAN_POWER_PARA_MIN_LEN > gastAtParaList[1].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 字符串第一个参数不为'-'，即为无效参数 */
    if ( '-' != gastAtParaList[1].aucPara[0] )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* POWER首字符为'-',获取后面的值 */
    if ( AT_SUCCESS != atAuc2ul( &gastAtParaList[1].aucPara[1],
                                 gastAtParaList[1].usParaLen - 1,
                                 &ulCellPower) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 由于POWER值为负，所以转换下 */
    *psCellPower = -(VOS_INT16)ulCellPower;

    /* power取值判断[-110,-47] */
    if ( (AT_NETSCAN_POWER_MIN_VALUE > *psCellPower)
      || (AT_NETSCAN_POWER_MAX_VALUE < *psCellPower) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}
VOS_UINT32 AT_GetValidNetScanBandPara(
    TAF_USER_SET_PREF_BAND64           *pstPrefBandPara
)
{
    VOS_UINT8                          *pucPara             = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usLen;

    pucPara         = gastAtParaList[3].aucPara;
    usLen           = gastAtParaList[3].usParaLen;

    pstPrefBandPara->ulBandHigh = 0;
    pstPrefBandPara->ulBandLow  = 0;

    /* 如果频段为空，则认为是全频段搜索 */
    if (0 == usLen)
    {
        pstPrefBandPara->ulBandHigh = 0;
        pstPrefBandPara->ulBandLow  = AT_ALL_GU_BAND;

        return AT_OK;
    }

    /* 频段值小于32位时 */
    if (usLen <= AT_HEX_LEN_OF_ULONG_TYPE)
    {
        ulRslt = At_String2Hex(pucPara, usLen, &(pstPrefBandPara->ulBandLow));
    }
    else
    {
        /* 64位频段的高32位字符串转换为16进制数，转换失败返回 */
        ulRslt = At_String2Hex(pucPara, (usLen - AT_HEX_LEN_OF_ULONG_TYPE),
                               &(pstPrefBandPara->ulBandHigh));
        if (AT_FAILURE == ulRslt)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 64位频段的低32位字符串转换为16进制数，转换失败返回  */
        ulRslt = At_String2Hex(pucPara + (usLen - AT_HEX_LEN_OF_ULONG_TYPE),
                               AT_HEX_LEN_OF_ULONG_TYPE, &(pstPrefBandPara->ulBandLow));
    }

    if (AT_FAILURE == ulRslt)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}



VOS_UINT32 AT_GetValidNetScanPara(
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScanSetPara
)
{
    VOS_UINT32                          ulRst;

    /* 检查参数个数,如果超过4个，返回AT_CME_INCORRECT_PARAMETERS*/
    if ( (AT_NETSCAN_PARA_MAX_NUM < gucAtParaIndex)
      || (AT_NETSCAN_PARA_MIN_NUM > gucAtParaIndex) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数校验 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置CellNum */
    pstNetScanSetPara->usCellNum = (VOS_UINT16)gastAtParaList[0].ulParaValue;

    /* 设置CellPow */
    if (AT_OK != AT_GetValidNetScanPowerParam(&(pstNetScanSetPara->sCellPow)))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 缺省<mode>参数，但不缺省<band>参数，则认为参数错误 */
    if ( (0 == gastAtParaList[2].usParaLen)
      && (0 != gastAtParaList[3].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果用户没输入接入技术，该gastAtParaList[2].ulParaValue就是0 */
    if ( 0 == gastAtParaList[2].usParaLen )
    {
        pstNetScanSetPara->ucRat     = TAF_PH_RAT_GSM;
    }
    else
    {
        pstNetScanSetPara->ucRat     = (VOS_UINT8)gastAtParaList[2].ulParaValue;
    }


    /* 检查band参数合法性,如果合法,进行转换并记录,如果不合法直接返回失败*/
    ulRst = AT_GetValidNetScanBandPara(&pstNetScanSetPara->stBand);

    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return ulRst;
}
VOS_UINT32 AT_SetNetScan(
    VOS_UINT8                           ucIndex
)
{
    TAF_MMA_NET_SCAN_REQ_STRU           stNetScanSetPara;
    VOS_UINT32                          ulRst;

    PS_MEM_SET(&stNetScanSetPara, 0, sizeof(stNetScanSetPara));

    /* 参数个数和合法性检查,不合法直接返回失败 */
    ulRst = AT_GetValidNetScanPara(&stNetScanSetPara);

    if (AT_OK != ulRst)
    {
        return ulRst;
    }

    /* 与MMA进行交互不成功返回失败，交互成功返回命令处理挂起状态 */
    ulRst = Taf_NetScanHandle(gastAtClientTab[ucIndex].usClientId, 0, &stNetScanSetPara);
    if (AT_SUCCESS == ulRst)
    {
        /* 指示当前用户的命令操作类型为设置命令 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NETSCAN_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_AbortNetScan(
    VOS_UINT8                           ucIndex
)
{
    /* 当前只能处理NETSCAN设置的打断, 其它命令则不进行打断操作 */
    if ( AT_CMD_NETSCAN_SET == gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        /* AT向MMA发送打断NETSCAN的请求 */
        if ( AT_SUCCESS == Taf_AbortNetScan(gastAtClientTab[ucIndex].usClientId, 0) )
        {
            /* 停止NETSCANAT的保护定时器 */
            AT_StopRelTimer(ucIndex, &gastAtClientTab[ucIndex].hTimer);

            /* 更新当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ABORT_NETSCAN;

            return AT_WAIT_ASYNC_RETURN;
        }

        /* AT向MMA发送打断NETSCAN的请求失败，返回失败 */
        return AT_FAILURE;
    }

    /* 当前非设置命令，返回失败 */
    return AT_FAILURE;
}




VOS_VOID AT_ChangeSysCfgRatPrio(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder,
    AT_SYSCFG_RAT_PRIO_ENUM_UINT8       enAcqorder

)
{
    if (TAF_PH_MAX_GU_RAT_NUM == pstRatOrder->ucRatOrderNum)
    {
        if (((AT_SYSCFG_RAT_PRIO_AUTO == enAcqorder)
         || (AT_SYSCFG_RAT_PRIO_WCDMA == enAcqorder)))
        {
            pstRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
            pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
            pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_GSM;
            pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
        }
        else if (AT_SYSCFG_RAT_PRIO_GSM == enAcqorder)
        {
            pstRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
            pstRatOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
            pstRatOrder->aenRatOrder[1] = TAF_PH_RAT_WCDMA;
            pstRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
        }
        else
        {
        }

    }
    else
    {
        /* 原单模无需考虑接入优先级 */
    }

}
VOS_UINT32 AT_ConvertSysCfgRatOrderPara(
    VOS_UINT8                           ucClientId,
    AT_SYSCFG_RAT_TYPE_ENUM_UINT8       enRatMode,
    AT_SYSCFG_RAT_PRIO_ENUM_UINT8       enAcqorder,
    TAF_PH_RAT_ORDER_STRU              *pstSysCfgRatOrder
)
{
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRst;

    enModemId = MODEM_ID_0;
    /* 获取ModemId */
    ulRst = AT_GetModemIdFromClient(ucClientId, &enModemId);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG1("AT_ConvertSysCfgRatOrderPara:Get ModemID From ClientID fail,ClientID=%d", ucClientId);
        return AT_ERROR;
    }
    switch (enRatMode)
    {
        case AT_SYSCFG_RAT_AUTO:
            /* 如果接入优先级不改变则默认从NVIM中获取 */
            if ( AT_SYSCFG_RAT_PRIO_NO_CHANGE == enAcqorder )
            {
                AT_GetGuAccessPrioNvim(enModemId, &enAcqorder);
            }


            /* ucAcqorder为0自动或先WCDMA后GSM*/

            if (AT_SYSCFG_RAT_PRIO_AUTO == enAcqorder)
            {
                AT_SetDefaultRatPrioList(enModemId, pstSysCfgRatOrder, VOS_FALSE);
            }
            else if (AT_SYSCFG_RAT_PRIO_WCDMA == enAcqorder)
            {
                pstSysCfgRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
                pstSysCfgRatOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
                pstSysCfgRatOrder->aenRatOrder[1] = TAF_PH_RAT_GSM;
                pstSysCfgRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            }
            else if (AT_SYSCFG_RAT_PRIO_GSM == enAcqorder)
            {
                pstSysCfgRatOrder->ucRatOrderNum  = TAF_PH_MAX_GU_RAT_NUM;
                pstSysCfgRatOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
                pstSysCfgRatOrder->aenRatOrder[1] = TAF_PH_RAT_WCDMA;
                pstSysCfgRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            }
            else
            {

            }

            break;

        case AT_SYSCFG_RAT_GSM:
            /* G单模 */
            pstSysCfgRatOrder->ucRatOrderNum  = 1;
            pstSysCfgRatOrder->aenRatOrder[0] = TAF_PH_RAT_GSM;
            pstSysCfgRatOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstSysCfgRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

        case AT_SYSCFG_RAT_WCDMA:
            /* W单模*/
            pstSysCfgRatOrder->ucRatOrderNum  = 1;
            pstSysCfgRatOrder->aenRatOrder[0] = TAF_PH_RAT_WCDMA;
            pstSysCfgRatOrder->aenRatOrder[1] = TAF_PH_RAT_BUTT;
            pstSysCfgRatOrder->aenRatOrder[2] = TAF_PH_RAT_BUTT;
            break;

        case AT_SYSCFG_RAT_NO_CHANGE:
            AT_GetRatOrderNvim(enModemId, pstSysCfgRatOrder, VOS_FALSE);
            AT_GetOnlyGURatOrder(pstSysCfgRatOrder);
            AT_ChangeSysCfgRatPrio(pstSysCfgRatOrder, enAcqorder);
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;

    }


    /*返回成功*/
    return AT_OK;
}


VOS_UINT32 At_SetSysCfgPara(VOS_UINT8 ucIndex)
{
    TAF_MMA_SYS_CFG_PARA_STRU           stSysCfgPara;

    VOS_UINT32                          ulRst;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if ( 5 != gucAtParaIndex )
    {                                                                           /* 参数过多 */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((gastAtParaList[0].usParaLen == 0)
     || (gastAtParaList[1].usParaLen == 0)
     || (gastAtParaList[2].usParaLen == 0)
     || (gastAtParaList[3].usParaLen == 0)
     || (gastAtParaList[4].usParaLen == 0))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }




    ulRst = AT_ConvertSysCfgRatOrderPara(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue,
                                         (VOS_UINT8)gastAtParaList[1].ulParaValue,
                                         &stSysCfgPara.stRatOrder);
    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }



    ulRst = AT_ConvertSysCfgExBandPara(gastAtParaList[2].aucPara,
                                       gastAtParaList[2].usParaLen,
                                       &stSysCfgPara.stGuBand);
    if (AT_OK != ulRst)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 需要将L的频段设置为不改变 */
    stSysCfgPara.enUserPrio         = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    stSysCfgPara.stLBand.ulBandLow  = TAF_PH_BAND_NO_CHANGE;
    stSysCfgPara.stLBand.ulBandHigh = 0;
    AT_ReadRoamCfgInfoNvim(ucIndex);

    if (((AT_ROAM_FEATURE_OFF == pstNetCtx->ucRoamFeature) && (gastAtParaList[3].ulParaValue > 2))
     || ((AT_ROAM_FEATURE_ON == pstNetCtx->ucRoamFeature) && (gastAtParaList[3].ulParaValue > 3)))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    stSysCfgPara.ucRoam       = (VOS_UINT8) gastAtParaList[3].ulParaValue;
    stSysCfgPara.ucSrvDomain  = (VOS_UINT8) gastAtParaList[4].ulParaValue;




    /* 执行命令操作 */
    if (VOS_TRUE == TAF_MMA_SetSysCfgReq(WUEPS_PID_AT,gastAtClientTab[ucIndex].usClientId, 0, &stSysCfgPara))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SYSCFG_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32 At_CheckDataState(TAF_UINT8 ucDataMode)
{
    TAF_UINT8 ucIndex = 0;

    for(ucIndex = 0; ucIndex < AT_MAX_CLIENT_NUM; ucIndex++)
    {
        if( (AT_CLIENT_USED == gastAtClientTab[ucIndex].ucUsed)
         && (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
         && (ucDataMode == gastAtClientTab[ucIndex].DataMode))
        {
            return AT_SUCCESS;
        }
    }

    return AT_FAILURE;
}


/*****************************************************************************
 Prototype      : At_SetCcinPara
 Description    :
 Input          : ucIndex --- 用户索引
 Output         :
 Return Value   : AT_XXX  --- ATC返回码
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_SetCcinPara(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 如果 参数是配置为启动主动上报模式；
       返回OK，并切换当前用户为主动上报模式；
       如果 参数是配置为停止主动上报模式；
       返回OK，并切换当前用户为普通命令模式；*/

    gastAtClientTab[ucIndex].IndMode = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    return AT_OK;
}

TAF_UINT32 At_SetTrigPara (TAF_UINT8 ucIndex)
{
    TAF_UINT8                           i;
    TAF_GPRS_DATA_STRU                  stGprsDataInfo;


    PS_MEM_SET(&stGprsDataInfo, 0x00, sizeof(stGprsDataInfo));


    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(5 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    for(i=0;i<5;i++)
    {
        if (0==gastAtParaList[i].usParaLen)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    stGprsDataInfo.ucMode = (TAF_UINT8)gastAtParaList[0].ulParaValue;
    if ((stGprsDataInfo.ucMode!=0) && (stGprsDataInfo.ucMode!=1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    if((5 <= gastAtParaList[1].ulParaValue) && (gastAtParaList[1].ulParaValue <= 15))
    {
        stGprsDataInfo.ucNsapi = (TAF_UINT8)gastAtParaList[1].ulParaValue;
    }
    else
    {
        return AT_ERROR;
    }

    stGprsDataInfo.ulLength      = (TAF_UINT32)gastAtParaList[2].ulParaValue;
    stGprsDataInfo.ucTimes       = (TAF_UINT8)gastAtParaList[3].ulParaValue;
    stGprsDataInfo.ulMillisecond = (TAF_UINT32)gastAtParaList[4].ulParaValue;

    /* 执行命令操作 */
    if ( TAF_PS_TrigGprsData(WUEPS_PID_AT,
                             gastAtClientTab[ucIndex].usClientId,
                             gastAtClientTab[ucIndex].opId,
                             &stGprsDataInfo) )
    {
        return AT_ERROR;
    }
    /* Modified by A00165503for AT Project，2011-10-17, end */

    return AT_OK;
}
TAF_UINT32 At_SetGcfIndPara (TAF_UINT8 ucIndex)
{
    VOS_UINT8                           ucGctTestFlag;
    VOS_UINT32                          ulRst;

    ucGctTestFlag = 0;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucGctTestFlag = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    if ((ucGctTestFlag != 0) && (ucGctTestFlag != 1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_GCF_IND,
                                   &ucGctTestFlag,
                                   sizeof(VOS_UINT8),
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }

}
VOS_UINT32 At_SetGcapPara(TAF_UINT8 ucIndex)
{
    TAF_UINT16           usLength = 0;

    if(0 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %s",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,"+CGSM,+DS,+ES");

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 At_SetMsIdInfo(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_MSID_QRY_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MSID_READ;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}


TAF_UINT32 AT_SetDsFlowClrPara(TAF_UINT8 ucIndex)
{
    TAF_DSFLOW_CLEAR_CONFIG_STRU        stClearConfigInfo;

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 初始化 */
    PS_MEM_SET(&stClearConfigInfo, 0x00, sizeof(TAF_DSFLOW_CLEAR_CONFIG_STRU));

    /* 设置参数 */
    stClearConfigInfo.enClearMode       = TAF_DSFLOW_CLEAR_TOTAL;

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_ClearDsFlowInfo(WUEPS_PID_AT,
                                          gastAtClientTab[ucIndex].usClientId,
                                          0, &stClearConfigInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt  = AT_CMD_DSFLOWCLR_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetDsFlowQryPara(TAF_UINT8 ucIndex)
{

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_GetDsFlowInfo(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt  = AT_CMD_DSFLOWQRY_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetDsFlowRptPara(TAF_UINT8 ucIndex)
{
    TAF_DSFLOW_REPORT_CONFIG_STRU       stReportConfigInfo;

    /* 初始化 */
    PS_MEM_SET(&stReportConfigInfo, 0x00, sizeof(TAF_DSFLOW_REPORT_CONFIG_STRU));

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置参数 */
    stReportConfigInfo.ulRptEnabled         = gastAtParaList[0].ulParaValue;


    if ( VOS_TRUE == stReportConfigInfo.ulRptEnabled )
    {
        /* 设置流量上报周期时长 */
        stReportConfigInfo.ulTimerLength    = 2000;
    }


    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_ConfigDsFlowRpt(WUEPS_PID_AT,
                                          gastAtClientTab[ucIndex].usClientId,
                                          0, &stReportConfigInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt  = AT_CMD_DSFLOWRPT_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 At_SetSrvstPara(TAF_UINT8 ucIndex)
{
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.enReqType       = AT_MTA_SET_SRVST_RPT_TYPE;
    stAtCmd.u.ucSrvstRptFlg = 0;

    if (0 != gastAtParaList[0].usParaLen)
    {
        stAtCmd.u.ucSrvstRptFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }

    /* 给MTA发送^srvst设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 At_SetRssiPara(TAF_UINT8 ucIndex)
{
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.enReqType       = AT_MTA_SET_RSSI_RPT_TYPE;
    stAtCmd.u.ucRssiRptFlg  = AT_RSSI_RESULT_CODE_NOT_REPORT_TYPE;

    if (0 != gastAtParaList[0].usParaLen)
    {
        stAtCmd.u.ucRssiRptFlg  = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }

    /* 给MTA发送^rssi设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    /* AT不需要发给L4A，由MMC发给LMM,再由LMM通知LRRC */

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 At_SetCerssiPara(VOS_UINT8 ucIndex)
{
    TAF_START_INFO_IND_STRU             stStartInfoInd;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;


    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);


    PS_MEM_SET(&stStartInfoInd, 0x00, sizeof(stStartInfoInd));

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((gucAtParaIndex > 2)
     || (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }




    if (1 == gucAtParaIndex)
    {
        pstNetCtx->ucCerssiReportType        = 0;
        pstNetCtx->ucCerssiMinTimerInterval  = 0;
        stStartInfoInd.ucMinRptTimerInterval = 0;
    }

    if (2 == gucAtParaIndex)
    {
        pstNetCtx->ucCerssiMinTimerInterval  = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        stStartInfoInd.ucMinRptTimerInterval = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    }

    if (0 != gastAtParaList[0].ulParaValue)
    {
        pstNetCtx->ucCerssiReportType  = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        stStartInfoInd.ucActionType    = TAF_START_EVENT_INFO_FOREVER;
        stStartInfoInd.ucSignThreshold = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        stStartInfoInd.ucRrcMsgType    = TAF_EVENT_INFO_CELL_SIGN;
    }
    else
    {
        /* 由于+creg/+cgreg/+csq/^rssi都是通过RRMM_AT_MSG_REQ或GRRMM_AT_MSG_REQ通知接入层主动上报，
           所以不能通知接入层停止，设置成默认rssi 5db改变上报，AT_GetModemNetCtxAddrFromClientId(ucIndex)->ucCerssiReportType全局变量控制^cerssi是否上报 */
        stStartInfoInd.ucActionType     = TAF_START_EVENT_INFO_FOREVER;
        stStartInfoInd.ucRrcMsgType     = TAF_EVENT_INFO_CELL_SIGN;
        stStartInfoInd.ucSignThreshold  = 0;
        pstNetCtx->ucCerssiReportType   = AT_CERSSI_REPORT_TYPE_NOT_REPORT;
        pstNetCtx->ucCerssiMinTimerInterval  = 0;
        stStartInfoInd.ucMinRptTimerInterval = 0;
    }

    Taf_PhoneStartInfoInd(gastAtClientTab[ucIndex].usClientId, 0, stStartInfoInd);

    /* AT不需要发给L4A，由MMC发给LMM,再由LMM通知LRRC */

    return AT_OK;
}
VOS_UINT32 At_SetCnmrPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    AT_MTA_QRY_NMR_REQ_STRU             stQryNmrReq;

    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

#if  (FEATURE_ON == FEATURE_LTE)
    /* 查询4g的nmr数据直接调用L的接口 */
    if (AT_CNMR_QRY_LTE_NMR_DATA == gastAtParaList[0].ulParaValue)
    {
        return At_QryCnmrPara(ucIndex);
    }
#else
    /* 查询4g的nmr数据直接返回失败 */
    if (AT_CNMR_QRY_LTE_NMR_DATA == gastAtParaList[0].ulParaValue)
    {
        return AT_ERROR;
    }
#endif

    /* 发送消息DRV_AGENT_AS_QRY_NMR_REQ给C核处理 */
    PS_MEM_SET(&stQryNmrReq, 0x0, sizeof(stQryNmrReq));

    if (AT_CNMR_QRY_WAS_NMR_DATA == gastAtParaList[0].ulParaValue)
    {
        stQryNmrReq.ucRatType = 0;
    }
    else
    {
        stQryNmrReq.ucRatType = 1;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   ID_AT_MTA_QRY_NMR_REQ,
                                   &stQryNmrReq,
                                   sizeof(stQryNmrReq),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CNMR_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;

}





TAF_UINT32  At_SetTimePara(TAF_UINT8 ucIndex)
{
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if ((0 == gastAtParaList[0].usParaLen) || (2 < gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.enReqType       = AT_MTA_SET_TIME_RPT_TYPE;
    stAtCmd.u.ucTimeRptFlg  = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 给MTA发送^time设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;

}
TAF_UINT32  At_SetCtzrPara(TAF_UINT8 ucIndex)
{
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if ((0 == gastAtParaList[0].usParaLen) || (2 < gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.enReqType       = AT_MTA_SET_CTZR_RPT_TYPE;
    stAtCmd.u.ucCtzrRptFlg  = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 给MTA发送^ctzr设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;

}
TAF_UINT32 At_SetModePara(TAF_UINT8 ucIndex)
{
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;
 #if (FEATURE_ON==FEATURE_LTE)
    L4A_IND_CFG_STRU                        stL4AIndCfgReq;
    MODEM_ID_ENUM_UINT16                    enModemId;

    enModemId  = MODEM_ID_0;
#endif

    /* AT命令参数合法性检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.enReqType       = AT_MTA_SET_MODE_RPT_TYPE;
    stAtCmd.u.ucModeRptFlg  = AT_MODE_RESULT_CODE_NOT_REPORT_TYPE;

    if (0 != gastAtParaList[0].usParaLen)
    {
        stAtCmd.u.ucModeRptFlg  = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }

    /* 给MTA发送^mode设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

#if  (FEATURE_ON == FEATURE_LTE)
    ulResult = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulResult)
    {
        AT_ERR_LOG1("At_SetModePara:Get ModemID From ClientID fail,ClientID=%d", ucIndex);
        return AT_ERROR;
    }

    if (VOS_TRUE == AT_IsModemSupportRat(enModemId, TAF_PH_RAT_LTE))
    {
        PS_MEM_SET(&stL4AIndCfgReq, 0, sizeof(stL4AIndCfgReq));
        stL4AIndCfgReq.mode_bit_valid  = 1;
        stL4AIndCfgReq.mode_bit        = gastAtParaList[0].ulParaValue;

        /* 通知L此次mode的设置 */
        AT_SetLIndCfgReq(ucIndex, &stL4AIndCfgReq);
    }
#endif

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;

}
VOS_UINT32 At_SetSimstPara(TAF_UINT8 ucIndex)
{
    AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU     stAtCmd;
    VOS_UINT32                              ulResult;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.enReqType       = AT_MTA_SET_SIMST_RPT_TYPE;
    stAtCmd.u.ucSimstRptFlg = AT_SIMST_ENABLE_TYPE;

    if (0 != gastAtParaList[0].usParaLen)
    {
        stAtCmd.u.ucSimstRptFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }

    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_UNSOLICITED_RPT_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 At_SetHSPara(TAF_UINT8 ucIndex)
{
    TAF_UINT8 aucBuf[5];
    TAF_VOID* pPara;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[0].usParaLen)
    {
        PS_MEM_CPY(aucBuf, &gastAtParaList[0].ulParaValue, sizeof(TAF_UINT32));
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(0 != gastAtParaList[1].usParaLen)
    {
        aucBuf[4] = (TAF_UINT8)gastAtParaList[1].ulParaValue;
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    pPara = aucBuf;
    /* 执行命令操作，这些参数可以不用发送到MMA */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,
                                   TAF_PH_HS_PARA,pPara))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_HS_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_UpdateSvn(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           aucSvn[],
    VOS_UINT16                          usSvnLength
)
{
    TAF_SVN_DATA_STRU                   stSvn;


    PS_MEM_SET(&stSvn, 0x00, sizeof(stSvn));


    /* SVN 长度检查: 用户输入的SVN固定长度应为2位 */
    if (TAF_SVN_DATA_LENGTH != usSvnLength)
    {
        AT_NORM_LOG("AT_UpdateSvn: the length of SVN is error.");
        return AT_PHYNUM_LENGTH_ERR;
    }

    /* SVN 字符串检查1: 确认<SVN>为数字字符串, 并将ASCII码转换成BCD码 */
    if (AT_SUCCESS != At_AsciiNum2Num(stSvn.aucSvn, aucSvn, usSvnLength))
    {
        AT_NORM_LOG("AT_UpdateSvn: the number of SVN is error.");
        return AT_PHYNUM_NUMBER_ERR;
    }

    /* SVN 字符串检查2: 99是SVN的保留数值，返回AT_PHYNUM_NUMBER_ERR物理号错误
       参考协议: 3GPP 23003 6.3 Allocation principles SVN value 99 is reserved for future use. */
    if ((9 == stSvn.aucSvn[0]) && (9 == stSvn.aucSvn[1]))
    {
        AT_NORM_LOG("AT_UpdateSvn: the number of SVN is reserved.");
        return AT_PHYNUM_NUMBER_ERR;
    }

    /* 设置SVN的NV项激活标志为激活，此标志默认为未激活； */
    stSvn.ucActiveFlag = NV_ITEM_ACTIVE;

    /* 清空无效的末位字节 */
    stSvn.aucReserve[0] = 0;

    if (NV_OK !=  NV_WriteEx(enModemId, en_NV_Item_Imei_Svn, &stSvn, sizeof(stSvn)))
    {
         AT_WARN_LOG("AT_UpdateSvn: Fail to write NV.");
         return AT_ERROR;
    }

    return AT_OK;
}


VOS_UINT32 AT_CheckPhyNumPara(AT_PHYNUM_TYPE_ENUM_UINT32 *penSetType)
{
    VOS_UINT32                          ulTimes;
    VOS_UINT32                          ulRet;

    /* <type>参数检查: BALONG仅支持输入物理号为IMEI或SVN，
       否则，直接返回错误，< err_code > 错误码为1物理号不合法 */
    /* 全部格式化为大写字符 */
    if(AT_FAILURE == At_UpString(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen))
    {
        return AT_ERROR;
    }

    if (0 == VOS_StrCmp((VOS_CHAR *)gastAtParaList[0].aucPara, "IMEI"))
    {
        *penSetType = AT_PHYNUM_TYPE_IMEI;
    }
    else if (0 == VOS_StrCmp((VOS_CHAR *)gastAtParaList[0].aucPara, "SVN"))
    {
        *penSetType = AT_PHYNUM_TYPE_SVN;
    }
    else if (0 == VOS_StrCmp((VOS_CHAR *)gastAtParaList[0].aucPara, "MACWLAN"))
    {
        *penSetType = AT_PHYNUM_TYPE_MAC;
    }
    else
    {
        AT_NORM_LOG("AT_CheckPhyNumPara: the type of physical is error.");
        return AT_PHYNUM_TYPE_ERR;
    }

    /* < times >参数检查: 输入< times >范围为0-1，否则返回AT_ERROR */
    if (gastAtParaList[2].usParaLen != 0)
    {
        /* TIMES的取值范围是0－1 */
        ulRet = At_Auc2ul(gastAtParaList[2].aucPara,
                          gastAtParaList[2].usParaLen,
                          &ulTimes);
        if ((AT_SUCCESS != ulRet) || (ulTimes > 1))
        {
            AT_NORM_LOG("AT_CheckPhyNumPara: times error.");
            return AT_ERROR;
        }
    }

    return AT_OK;
}
VOS_UINT32  AT_SetPhyNumPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    AT_PHYNUM_TYPE_ENUM_UINT32          enSetType;
    VOS_BOOL                            bPhyNumIsNull;
    MODEM_ID_ENUM_UINT16                enModemId;

    enModemId = MODEM_ID_0;

    /* 参数个数有效性检查: 参数至少包括<type>,<number>两部分，
       最多可以再包含< times > ，参数个数错误，返回AT_ERROR */
    if ((2 != gucAtParaIndex) && (3 != gucAtParaIndex))
    {
        AT_NORM_LOG("AT_SetPhyNumPara: the number of parameter is error.");
        return AT_ERROR;
    }

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("AT_SetPhyNumPara: Get modem id fail.");
        return AT_ERROR;
    }

    ulRet = AT_CheckPhyNumPara(&enSetType);
    if (AT_OK != ulRet)
    {
        return ulRet;
    }

    /* 物理号不为空，且数据保护未解锁，直接返回错误，< err_code > 错误码为1物理号不合法。 */
    if (AT_OK != AT_PhyNumIsNull(enModemId, enSetType, &bPhyNumIsNull))
    {
        return AT_ERROR;
    }

    if ((VOS_FALSE == bPhyNumIsNull) && (VOS_TRUE == g_bAtDataLocked))
    {
        AT_NORM_LOG("AT_SetPhyNumPara: physical number is not null and data locked.");
        return AT_PHYNUM_TYPE_ERR;
    }

    /* 若当前用户设置的物理号为IMEI，则修改en_NV_Item_IMEI中的数据 */
    /* 若当前用户设置的物理号为SVN，则修改en_Nv_Item_Imei_Svn中的数据 */
    /* 若当前用户设置的物理号为MAC，则修改MAC地址对应的NV项(新增数据卡MAC地址的NV项)中的数据 */
    if (AT_PHYNUM_TYPE_IMEI == enSetType)
    {
        ulRet = AT_UpdateImei(enModemId, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
    }
    else if (AT_PHYNUM_TYPE_SVN == enSetType)
    {
        ulRet = AT_UpdateSvn(enModemId, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
    }
    else
    {
        /*更新MAC地址到NV*/
        ulRet = AT_UpdateMacPara(gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
    }

    return ulRet;
}
VOS_UINT32 AT_SetGTimerPara(VOS_UINT8 ucIndex)
{

    /* 参数个数错误，返回AT_DEVICE_OTHER_ERROR，上报CME ERROR:1 */
    if (1 != gucAtParaIndex)
    {
        AT_NORM_LOG("AT_SetGTimerPara: the number of parameter is error.");
        return AT_ERROR;
    }

    /* 数据保护未解锁，返回AT_DATA_UNLOCK_ERROR，上报CME ERROR:0 */
    if (g_bAtDataLocked != VOS_FALSE)
    {
        AT_NORM_LOG("AT_SetGTimerPara: data locked.");
        return AT_ERROR;
    }

    if (NV_OK !=  NV_WriteEx(MODEM_ID_0, en_NV_Item_GPRS_ActiveTimerLength,
                           &gastAtParaList[0].ulParaValue,
                           sizeof(gastAtParaList[0].ulParaValue)))
    {
         AT_WARN_LOG("AT_SetGTimerPara: Fail to write en_NV_Item_GPRS_ActiveTimerLength.");
         return AT_ERROR;
    }

    return AT_OK;
}
TAF_UINT32 At_SetQcdmgPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 该命令无输入参数 */
    if(0 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ucIndex所对应的AT通道不是PCUI/UART通道 */
    if ((AT_USB_COM_PORT_NO != gastAtClientTab[ucIndex].ucPortNo)
     && (AT_UART_PORT_NO != gastAtClientTab[ucIndex].ucPortNo))
    {
        return AT_ERROR;
    }

    /*检查是否已有处于DIAG模式的通道*/
    if(AT_SUCCESS == At_CheckDataState(AT_DIAG_DATA_MODE))
    {
        return AT_ERROR;
    }

    /*记录AT/DIAG通道所对应的索引号*/
    gucOmDiagIndex = ucIndex;

    /* 通知OAM切换至DIAG模式 */
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
    if (AT_USB_COM_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CPM_NotifyChangePort(AT_PCUI_PORT, CPM_DIAG_COMM);
    }
    else
    {
        CPM_NotifyChangePort(AT_UART_PORT, CPM_DIAG_COMM);
    }
#endif
    /*将通道模式设置为DIAG模式*/
    At_SetMode(gucOmDiagIndex, AT_DATA_MODE, AT_DIAG_DATA_MODE);

    return AT_OK;
}
TAF_UINT32 At_SetU2DiagPara(TAF_UINT8 ucIndex)
{
    AT_USB_ENUM_STATUS_STRU             stUsbEnumStatus;

    AT_PID_ENABLE_TYPE_STRU             stPidEnableType;


    PS_MEM_SET(&stUsbEnumStatus, 0x00, sizeof(stUsbEnumStatus));

    stPidEnableType.ulPidEnabled = VOS_FALSE;

    /* 读取PID使能NV项 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_PID_Enable_Type,
                        &stPidEnableType,
                        sizeof(AT_PID_ENABLE_TYPE_STRU)))
    {
        return AT_ERROR;
    }

    /* NV读取成功，检查PID是否使能，若使能，返回ERROR */
    if (VOS_FALSE != stPidEnableType.ulPidEnabled)
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    /* 参数有效性检查 */
    if (VOS_OK != DRV_U2DIAG_VALUE_CHECK(gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 填充stUsbEnumStatus结构体 */
    PS_MEM_SET(&stUsbEnumStatus, 0, sizeof(AT_USB_ENUM_STATUS_STRU));

    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_USB_Enum_Status, &stUsbEnumStatus, sizeof(AT_USB_ENUM_STATUS_STRU)))
    {
        if ( (gastAtParaList[0].ulParaValue == stUsbEnumStatus.ulValue)
            && (1 == stUsbEnumStatus.ulStatus))
        {
            AT_INFO_LOG("At_SetU2DiagPara():The content to write is same as NV's");
            return AT_OK;
        }
    }

    stUsbEnumStatus.ulStatus = 1;
    stUsbEnumStatus.ulValue  = gastAtParaList[0].ulParaValue;

    /* 写入NVIM */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_USB_Enum_Status,
                           &stUsbEnumStatus,
                           sizeof(AT_USB_ENUM_STATUS_STRU)))
    {
        return AT_ERROR;
    }

    return AT_OK;
}
VOS_BOOL At_FistScanSetPortPara(
    VOS_UINT16                          *pusSemiIndex,
    VOS_UINT8                           *pucInputString,
    VOS_UINT16                          usInputLen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;
    VOS_UINT8                           ucSemiCount;
    VOS_UINT8                           aAvailableChar[] = {'0','1', '2', '3', '4',
                                                            '5', '6', '7', 'A',
                                                            'B', 'D', 'E', 'F',
                                                            ',', ';'};

    ucSemiCount = 0;

    for (i = 0; i < usInputLen; i++)
    {
        for (j = 0; j < (sizeof(aAvailableChar)/sizeof(VOS_UINT8)); j++)
        {
            if (*(pucInputString + i) == aAvailableChar[j])
            {
                if (';' == *(pucInputString + i))
                {
                    ucSemiCount++;
                    *pusSemiIndex = (VOS_UINT16)i;

                    /* 有且只能有一个分号 */
                    if (ucSemiCount > 1)
                    {
                        AT_ERR_LOG("At_FistScanSetPortPara:The num of semi can only be one!");
                        return VOS_FALSE;
                    }
                }
                break;
            }
        }

        /* 不是合法字符 */
        if ((sizeof(aAvailableChar)/sizeof(VOS_UINT8)) == j)
        {
            AT_ERR_LOG("At_FistScanSetPortPara:Invalid character!");
            return VOS_FALSE;
        }
    }

    /* 有且只能有一个分号并且不是第一个字符也不是最后一个字符 */
    if (( 0 == *pusSemiIndex ) || ((usInputLen - 1) == *pusSemiIndex))
    {
        AT_ERR_LOG("At_FistScanSetPortPara:The position of semi isn't suitable!");
        return VOS_FALSE;
    }

    /* 第一个和最后一个字符为逗号 */
    if ((',' == *pucInputString) || (',' == pucInputString[usInputLen - 1]))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_BOOL At_SecScanSetPortFirstPara(
    VOS_UINT16                          usSemiIndex,
    VOS_UINT8                           *pucFirstParaCount,
    VOS_UINT8                           *pucInputString,
    VOS_UINT16                          usInputLen
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucCharCount;

    ucCharCount = 0;

    for (i = 0; i <= usSemiIndex; i++)
    {
        /* 逗号或者第一个参数的结尾 */
        if ((',' == *(pucInputString + i)) || (';' == *(pucInputString + i)))
        {
            /* 参数之间多个逗号的情况A1,,,,A2;1,2 返回ERROR */
            /* 必须是两个字符A1,A2,FF */
            if (2 != ucCharCount)
            {
                AT_ERR_LOG("At_SecScanSetPortFirstPara:Invalid first parameter!");
                return VOS_FALSE;
            }

            /* LINT ERROR: ucCharCount = 2, i >= 2 */
            if ((i >= 2)
                && !((('A' == pucInputString[i - 2]) && ('1' == pucInputString[i - 1]))
                    || (('A' == pucInputString[i - 2]) && ('2' == pucInputString[i - 1]))
                    || (('F' == pucInputString[i - 2]) && ('F' == pucInputString[i - 1]))))

            {
                AT_ERR_LOG("At_SecScanSetPortFirstPara:Invalid first parameter!");
                return VOS_FALSE;
            }

            (*pucFirstParaCount)++;

            ucCharCount = 0;
        }
        else
        {
            ucCharCount++;
        }

        if (ucCharCount > 2)
        {
            AT_ERR_LOG("At_SecScanSetPortFirstPara:Invalid character!");
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_BOOL At_SecScanSetPortSecPara(
    VOS_UINT16                          usSemiIndex,
    VOS_UINT8                           *pucSecParaCount,
    VOS_UINT8                           aucSecPara[AT_SETPORT_PARA_MAX_LEN][3],
    VOS_UINT8                           *pucInputString,
    VOS_UINT16                          usInputLen
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           ucCharCount;
    VOS_UINT8                           aucTmpInputString[AT_SETPORT_PARA_MAX_CHAR_LEN];
    VOS_UINT16                          usTmpInputStringLen;

    ucCharCount = 0;

    if (usInputLen > AT_SETPORT_PARA_MAX_CHAR_LEN)
    {
        return VOS_FALSE;
    }

    /*第二个参数结尾加上逗号便于以下参数检查*/
    usTmpInputStringLen = usInputLen - usSemiIndex;
    PS_MEM_CPY(aucTmpInputString, pucInputString + usSemiIndex + 1, (VOS_SIZE_T)(usTmpInputStringLen - 1));
    aucTmpInputString[usTmpInputStringLen - 1] = ',';

    for (i = 0; i < usTmpInputStringLen; i++)
    {
        if (',' == *(aucTmpInputString + i))
        {
            /*参数之间多个逗号的情况A1,A2;1,,,2返回eror*/
            if (0 == ucCharCount)
            {
                AT_ERR_LOG("At_SecScanSetPortSecPara:Invalid second parameter!");
                return VOS_FALSE;
            }

            /* 如果是两个字符必须为A1,A2或16 */
            if (2 == ucCharCount)
            {
                /* LINT ERROR: ucCharCount = 2, i >= 2 */
                if ((i >= 2)
                    && !((('A' == aucTmpInputString[i - 2]) && ('1' == aucTmpInputString[i - 1]))
                      || (('A' == aucTmpInputString[i - 2]) && ('2' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('6' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('0' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('1' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('2' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('3' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('4' == aucTmpInputString[i - 1]))
                      || (('1' == aucTmpInputString[i - 2]) && ('5' == aucTmpInputString[i - 1]))
                 ))
                {
                    AT_ERR_LOG("At_SecScanSetPortSecPara:Invalid second parameter!");
                    return VOS_FALSE;
                }
            }

            /* 保存合法的参数 */
            PS_MEM_CPY(aucSecPara[*pucSecParaCount],
                (aucTmpInputString + i) - ucCharCount, ucCharCount);

            (*pucSecParaCount)++;

            ucCharCount = 0;
        }
        else
        {
            ucCharCount++;
        }

        if (ucCharCount > 2)
        {
            AT_ERR_LOG("At_SecScanSetPortSecPara:Invalid second parameter!");
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}
VOS_BOOL At_SaveSetPortFirstPara(
    VOS_UINT8                           ucFirstParaCount,
    VOS_UINT8                           *pucFirstPort,
    VOS_UINT8                           *pucInputString
)
{
    /* 参数个数不符合要求 */
    if ((ucFirstParaCount != 1) && (ucFirstParaCount != 2))
    {
        return VOS_FALSE;
    }

    /* 只有一个参数: FF或者A1 */
    if (1 == ucFirstParaCount)
    {
        if (('F' == *pucInputString) && ('F' == *(pucInputString + 1)))
        {
            *pucFirstPort = 0xFF;
        }
        else if (('A' == *pucInputString) && ('1' == *(pucInputString + 1)))
        {
            *pucFirstPort = 0xA1;
        }
        else
        {
            AT_ERR_LOG("At_SaveSetPortFirstPara:Invalid first parameter!");
            return VOS_FALSE;
        }
    }
    /* 两个参数: A1,A2 */
    else
    {
        if (('A' == *pucInputString) && ('1' == pucInputString[1])
         && ('A' == pucInputString[3]) && ('2' == pucInputString[4]))
        {
            *pucFirstPort       = 0xA1;
            *(pucFirstPort + 1) = 0xA2;
        }
        else
        {
            AT_ERR_LOG("At_SaveSetPortFirstPara:Invalid first parameter!");
            return VOS_FALSE;
        }
    }
    return VOS_TRUE;
}
VOS_BOOL At_SaveSetPortSecPara(
    VOS_UINT8                           ucSecParaCount,
    VOS_UINT8                           *pucSecPort,
    VOS_UINT8                           aucSecPara[AT_SETPORT_PARA_MAX_LEN][3]
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;

    /* 参数个数不符合要求 */
    if ((0 == ucSecParaCount) || (ucSecParaCount > AT_SETPORT_PARA_MAX_LEN))
    {
        AT_ERR_LOG("At_SaveSetPortSecPara:Invalid second parameter!");
        return VOS_FALSE;
    }

    /* 参数不能重复 */
    for (i = 0; i < (VOS_UINT8)(ucSecParaCount - 1); i++)
    {
        for (j = i + 1; j < ucSecParaCount; j++)
        {
            if (!VOS_MemCmp(aucSecPara[i], aucSecPara[j], 3))
            {
                AT_ERR_LOG("At_SaveSetPortSecPara:Invalid second parameter!");
                return VOS_FALSE;
            }
        }
    }

    for (i = 0; i < ucSecParaCount; i++)
    {
        for (j = 0; j < AT_SETPORT_DEV_LEN; j++)
        {
            if (!VOS_MemCmp(aucSecPara[i], g_astSetPortParaMap[j].aucAtSetPara, 3))
            {
                pucSecPort[i] = g_astSetPortParaMap[j].ucDrvPara;
                break;
            }
        }

        /* 说明不是合法的设备形态，返回错误 */
        if (j >= AT_SETPORT_DEV_LEN)
        {
            AT_ERR_LOG("At_SaveSetPortSecPara:Invalid second parameter!");
            return VOS_FALSE;
        }
    }
    return VOS_TRUE;
}
VOS_UINT32 AT_CheckPortTypePara(
    VOS_UINT8                           ucFirstParaCount,
    VOS_UINT8                          *pucFirstPort,
    VOS_UINT8                           ucSecParaCount,
    VOS_UINT8                          *pucSecPort
)
{
    VOS_UINT32                          ulRet;

    /* 切换前端口有效性检查:
    端口配置第一位必须为A1(CDROM),否则返回ERROR；
    第二位如果有，仅允许有A2(SD),否则返回ERROR；
    如果是直接上报多端口状态，分号前就直接是FF,否则返回ERROR。
    */
    if (2 == ucFirstParaCount)
    {
        if ((AT_DEV_CDROM != *(pucFirstPort))
        || (AT_DEV_SD != *(pucFirstPort + 1)))
        {
            return VOS_FALSE;
        }

    }
    else if (1 == ucFirstParaCount)
    {
        if ((AT_DEV_CDROM != *(pucFirstPort))
        && (AT_DEV_NONE != *(pucFirstPort)))
        {
            return VOS_FALSE;
        }

    }
    else
    {
        return VOS_FALSE;
    }

    /*
    切换后端口有效性检查:
        1.端口形态中必须有2(PCUI),否则返回ERROR；
        2.切换后端口必须是测试命令中输出的支持的端口，否则返回ERROR；
        3.不能设置有重复的端口，否则返回ERROR；
        4.MASS 不能设置到其他端口的前面，否则返回ERROR
        5.端口个数不能超过规定的个数,否则返回ERROR；
    */
    ulRet = DRV_USB_PORT_TYPE_VALID_CHECK(pucSecPort, ucSecParaCount);
    if (VOS_OK != ulRet)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_BOOL At_IsAvailableSetPortPara(
    VOS_UINT8                           *pucInputString,
    VOS_UINT16                          usInputLen,
    VOS_UINT8                           *pucFirstPort,
    VOS_UINT8                           *pucSecPort
)
{
    /* 分号的位置 */
    VOS_UINT16                          usSemiIndex;
    VOS_UINT8                           ucFirstParaCount;
    VOS_UINT8                           ucSecParaCount;
    VOS_UINT8                           aucSecPara[AT_SETPORT_PARA_MAX_LEN][3];
    VOS_BOOL                            bScanResult;
    VOS_UINT32                          ulRet;

    usSemiIndex         = 0;
    ucFirstParaCount    = 0;
    ucSecParaCount      = 0;

    VOS_MemSet(aucSecPara, 0x00, AT_SETPORT_PARA_MAX_LEN * 3);

    /* 第一轮扫描:是否有非法字符 */
    bScanResult = At_FistScanSetPortPara(&usSemiIndex, pucInputString, usInputLen);

    if (VOS_TRUE != bScanResult)
    {
        AT_ERR_LOG("At_IsAvailableSetPortPara:First parameter is invalid!");
        return VOS_FALSE;
    }

    /* 第二轮扫描:每个参数必须是A1,A2,FF,A,B,D,E,1,2,3,4,5,6,7 */
    /* 第一个参数 */
    bScanResult = At_SecScanSetPortFirstPara(usSemiIndex,
                                        &ucFirstParaCount,
                                        pucInputString,
                                        usInputLen);

    if (VOS_TRUE != bScanResult)
    {
        AT_ERR_LOG("At_IsAvailableSetPortPara:First parameter is invalid!");
        return VOS_FALSE;
    }

    /*第二个参数*/
    bScanResult = At_SecScanSetPortSecPara(usSemiIndex,
                                        &ucSecParaCount,
                                        aucSecPara,
                                        pucInputString,
                                        usInputLen);

    if (VOS_TRUE != bScanResult)
    {
        AT_ERR_LOG("At_IsAvailableSetPortPara:Second parameter is invalid!");
        return VOS_FALSE;
    }

    /*保存第一个参数*/
    bScanResult = At_SaveSetPortFirstPara(ucFirstParaCount, pucFirstPort, pucInputString);

    if (VOS_TRUE != bScanResult)
    {
        AT_ERR_LOG("At_IsAvailableSetPortPara:Save first parameter fail!");
        return VOS_FALSE;
    }

    /*保存第二个参数*/
    bScanResult = At_SaveSetPortSecPara(ucSecParaCount, pucSecPort, aucSecPara);

    if (VOS_TRUE != bScanResult)
    {
        AT_ERR_LOG("At_IsAvailableSetPortPara:Save second parameter fail!");
        return VOS_FALSE;
    }

    ulRet = AT_CheckPortTypePara(ucFirstParaCount, pucFirstPort, ucSecParaCount, pucSecPort);

    return  ulRet;
}
VOS_UINT32 At_SetPort(VOS_UINT8 ucIndex)
{
    VOS_BOOL                            bValidPara;
    VOS_UINT8                           aucFirstPort[AT_SETPORT_PARA_MAX_LEN];
    VOS_UINT8                           aucSecPort[AT_SETPORT_PARA_MAX_LEN];
    AT_DYNAMIC_PID_TYPE_STRU            stDynamicPidType;
    AT_PID_ENABLE_TYPE_STRU             stPidEnableType;


    stPidEnableType.ulPidEnabled = VOS_FALSE;


    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    PS_MEM_SET(aucFirstPort, 0x00, AT_SETPORT_PARA_MAX_LEN);
    PS_MEM_SET(aucSecPort, 0x00, AT_SETPORT_PARA_MAX_LEN);
    PS_MEM_SET(&stDynamicPidType, 0x00, sizeof(AT_DYNAMIC_PID_TYPE_STRU));

    /* 读取PID使能NV项 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_PID_Enable_Type,
                         &stPidEnableType,
                         sizeof(AT_PID_ENABLE_TYPE_STRU)))
    {
        AT_ERR_LOG("At_SetPort:Read NV failed!");
        return AT_ERROR;
    }

    /* NV读取成功，检查PID是否使能，若不使能，返回ERROR */
    if (VOS_TRUE != stPidEnableType.ulPidEnabled)
    {
        AT_WARN_LOG("At_SetPort:PID is not enabled");
        return AT_ERROR;
    }

    /* 1.参数为空
       2.参数超过最大长度
       参数最大长度: 两个参数的长度加一个分号 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (gastAtParaList[0].usParaLen > ((AT_SETPORT_PARA_MAX_CHAR_LEN * 2) + 1)))
    {
        return AT_ERROR;
    }

    /* 该AT命令支持用户输入大小写，先将小写转成大写，再进行字符串的解析 */
    At_UpString(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    /* 参数解析 */
    bValidPara = At_IsAvailableSetPortPara(gastAtParaList[0].aucPara,
                                        gastAtParaList[0].usParaLen,
                                        aucFirstPort,
                                        aucSecPort);
    if (VOS_FALSE == bValidPara)
    {
        AT_WARN_LOG("At_SetPort:Parameter is invalid");
        return AT_ERROR;
    }

    /* 读NV失败，直接返回ERROR */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_Huawei_Dynamic_PID_Type,
                        &stDynamicPidType,
                        sizeof(AT_DYNAMIC_PID_TYPE_STRU)))
    {
        AT_ERR_LOG("At_SetPort:Read NV fail!");
        return AT_ERROR;
    }

    /* 读NV成功，判断该NV的使能状态，若不使能，直接返回ERROR */
    if (VOS_TRUE == stDynamicPidType.ulNvStatus)
    {
        /* 判断要写入内容是否与NV中内容相同，如果相同不再写操作，直接返回OK */
        if(!(VOS_MemCmp(stDynamicPidType.aucFirstPortStyle, aucFirstPort, AT_SETPORT_PARA_MAX_LEN)))
        {
            if(!(VOS_MemCmp(stDynamicPidType.aucRewindPortStyle, aucSecPort, AT_SETPORT_PARA_MAX_LEN)))
            {
                AT_INFO_LOG("At_SetPort:Same, not need write");
                return AT_OK;
            }
        }
    }
    else
    {
        return AT_ERROR;
    }

    /* 此处增加DIAG口密码保护    */
    if (AT_OK != AT_CheckSetPortRight(stDynamicPidType.aucRewindPortStyle,
                                      aucSecPort))
    {
        return AT_ERROR;
    }

    PS_MEM_CPY(stDynamicPidType.aucFirstPortStyle, aucFirstPort, AT_SETPORT_PARA_MAX_LEN);
    PS_MEM_CPY(stDynamicPidType.aucRewindPortStyle, aucSecPort, AT_SETPORT_PARA_MAX_LEN);

    /* 写入NV */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Huawei_Dynamic_PID_Type,
                        &stDynamicPidType,
                        sizeof(AT_DYNAMIC_PID_TYPE_STRU)))
    {
        AT_ERR_LOG("At_SetPort:Write NV fail");
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}


VOS_UINT32 AT_OpenSpecificPort(VOS_UINT8 ucPort)
{
    AT_DYNAMIC_PID_TYPE_STRU            stDynamicPidType;
    VOS_UINT32                          ulPortPos;
    VOS_UINT32                          ulPortNum;


    PS_MEM_SET(&stDynamicPidType, 0x00, sizeof(stDynamicPidType));

    ulPortPos = AT_DEV_NONE;


    /* 读NV项en_NV_Item_Huawei_Dynamic_PID_Type获取当前的端口状态 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_Huawei_Dynamic_PID_Type,
                        &stDynamicPidType,
                        sizeof(AT_DYNAMIC_PID_TYPE_STRU)))
    {
        AT_WARN_LOG("AT_OpenSpecificPort: Read NV fail!");
        return AT_ERROR;
    }

    /* 判断端口是否已经打开: 已经打开则直接返回AT_OK */
    if (VOS_TRUE == stDynamicPidType.ulNvStatus)
    {
        /* 查询NV项en_NV_Item_Huawei_Dynamic_PID_Type中是否已经存在该端口 */
        AT_GetSpecificPort(ucPort,
                           stDynamicPidType.aucRewindPortStyle,
                           &ulPortPos,
                           &ulPortNum);

        if (AT_DEV_NONE != ulPortPos)
        {
            return AT_OK;
        }

    }
    else
    {
        AT_WARN_LOG("AT_OpenSpecificPort: en_NV_Item_Huawei_Dynamic_PID_Type is inactive!");
        return AT_ERROR;
    }

    /* 当前不能再增加端口 */
    if (ulPortNum >= AT_SETPORT_PARA_MAX_LEN)
    {
        return AT_ERROR;
    }

    stDynamicPidType.aucRewindPortStyle[ulPortNum] = ucPort;

    /* 更新端口集合数据到NV项en_NV_Item_Huawei_Dynamic_PID_Type */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Huawei_Dynamic_PID_Type,
                        &stDynamicPidType,
                        sizeof(AT_DYNAMIC_PID_TYPE_STRU)))
    {
        AT_ERR_LOG("AT_OpenSpecificPort: Write NV fail");
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}
VOS_UINT32 AT_CloseSpecificPort(VOS_UINT8 ucPort)
{
    AT_DYNAMIC_PID_TYPE_STRU            stDynamicPidType;
    VOS_UINT32                          ulPortPos;
    VOS_UINT32                          ulPortNum;
    VOS_UINT32                          ulLoop;


    PS_MEM_SET(&stDynamicPidType, 0x00, sizeof(stDynamicPidType));

    ulPortPos = AT_DEV_NONE;


    /* 读NV项en_NV_Item_Huawei_Dynamic_PID_Type获取当前的端口状态 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_Huawei_Dynamic_PID_Type,
                          &stDynamicPidType,
                          sizeof(AT_DYNAMIC_PID_TYPE_STRU)))
    {
        AT_ERR_LOG("AT_CloseSpecificPort: Read NV fail!");
        return AT_ERROR;
    }

    /* 判断该端口是否已经关闭: 已经关闭则直接返回AT_OK */
    if (VOS_TRUE == stDynamicPidType.ulNvStatus)
    {
        /* 查询NV项en_NV_Item_Huawei_Dynamic_PID_Type中是否已经存在该端口 */
        AT_GetSpecificPort(ucPort,
                           stDynamicPidType.aucRewindPortStyle,
                           &ulPortPos,
                           &ulPortNum);

        if (AT_DEV_NONE == ulPortPos)
        {
            return AT_OK;
        }
    }
    else
    {
        return AT_ERROR;
    }

    /* 删除NV项中的指定端口 */
    stDynamicPidType.aucRewindPortStyle[ulPortPos] = 0;
    ulPortNum--;

    for (ulLoop = ulPortPos; ulLoop < ulPortNum; ulLoop++)
    {
        stDynamicPidType.aucRewindPortStyle[ulLoop] = stDynamicPidType.aucRewindPortStyle[ulLoop + 1UL];
    }

    stDynamicPidType.aucRewindPortStyle[ulPortNum] = 0;

    /* 端口异常数据保护: 切换后的设备形态中，第一个设备不能为MASS设备(0xa1,0xa2) */
    if (0 != ulPortNum)
    {
        if ((AT_DEV_CDROM == stDynamicPidType.aucRewindPortStyle[0])
         || (AT_DEV_SD == stDynamicPidType.aucRewindPortStyle[0]))
        {
            return AT_ERROR;
        }
    }

    /* 更新端口集合数据到NV项en_NV_Item_Huawei_Dynamic_PID_Type */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Huawei_Dynamic_PID_Type,
                          &stDynamicPidType,
                          sizeof(AT_DYNAMIC_PID_TYPE_STRU)))
    {
        AT_ERR_LOG("AT_CloseSpecificPort: Write NV fail");
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}

TAF_UINT32 At_SetPcscInfo(TAF_UINT8 ucIndex)
{
    AT_DYNAMIC_PID_TYPE_STRU            stDynamicPidType;
    AT_PID_ENABLE_TYPE_STRU             stPidEnableType;


    stPidEnableType.ulPidEnabled = VOS_FALSE;


    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    PS_MEM_SET(&stDynamicPidType, 0x00, sizeof(AT_DYNAMIC_PID_TYPE_STRU));

    /* 读取PID使能NV项 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_PID_Enable_Type,
                         &stPidEnableType,
                         sizeof(AT_PID_ENABLE_TYPE_STRU)))
    {
        AT_ERR_LOG("At_SetPcscInfo:Read NV31 failed!");
        return AT_ERROR;
    }

    /* NV读取成功，检查PID是否使能，若不使能，返回ERROR */
    if (VOS_TRUE != stPidEnableType.ulPidEnabled)
    {
        AT_WARN_LOG("At_SetPcscInfo:PID is not enabled!");
        return AT_ERROR;
    }

    /*  打开PCSC口*/
    if ( VOS_TRUE == gastAtParaList[0].ulParaValue )
    {
        return AT_OpenSpecificPort(AT_DEV_PCSC);
    }
    /* 关闭PCSC口 */
    else
    {
        return AT_CloseSpecificPort(AT_DEV_PCSC);
    }
}


VOS_UINT32 At_SetCellSearch(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    VOS_UINT8                           ucCellSrh;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    ucCellSrh = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 发送消息 ID_AT_MTA_WRR_CELLSRH_SET_REQ 给 AT AGENT 处理，该消息带参数(VOS_UINT8)gastAtParaList[0].ulParaValue */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   ID_AT_MTA_WRR_CELLSRH_SET_REQ,
                                   (VOS_VOID*)&ucCellSrh,
                                   sizeof(ucCellSrh),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_WAS_MNTN_SET_CELLSRH;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}



TAF_UINT32 At_SetGetportmodePara (TAF_UINT8 ucIndex)
{
    TAF_PH_PORT_STRU     stPortInfo;
    TAF_PH_FMR_ID_STRU   stFmrID;
    TAF_UINT16           usDataLen = 0;
    TAF_UINT32           ulRslt;
    TAF_UINT32           ulPortInfoLen;

     /* 参数检查 */
    if ((AT_CMD_OPT_SET_PARA_CMD == g_stATParseCmd.ucCmdOptType) && (0 == gastAtParaList[0].usParaLen))
    {
        return AT_ERROR;
    }

    PS_MEM_SET(&stPortInfo, 0, sizeof(TAF_PH_PORT_STRU));

    /* MMA_CardType((VOS_CHAR *)stPortInfo.ProductName.aucProductName); */
    VOS_StrNCpy((VOS_CHAR *)stPortInfo.ProductName.aucProductName,
                 PRODUCTION_CARD_TYPE,
                 sizeof(stPortInfo.ProductName.aucProductName));



    PS_MEM_SET(&stFmrID, 0x00, sizeof(TAF_PH_FMR_ID_STRU));

    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_FMRID, &stFmrID, sizeof(TAF_PH_FMR_ID_STRU)))
    {
        AT_WARN_LOG("At_SetMsIdInfo:WARNING:NVIM Read en_NV_Item_FMRID falied!");
        return AT_ERROR;
    }
    else
    {
        PS_MEM_CPY(stPortInfo.FmrId.aucMfrId, &stFmrID, sizeof(TAF_PH_FMR_ID_STRU));
    }

    ulPortInfoLen = TAF_MAX_PORT_INFO_LEN + 1;

    ulRslt = (VOS_UINT32)(DRV_GET_PORT_MODE((char *)stPortInfo.PortId.aucPortInfo, &ulPortInfoLen));
    if( VOS_OK != ulRslt )
    {
        AT_WARN_LOG("At_SetGetportmodePara:DRV_GET_PORT_MODE Info fail.");
        return AT_ERROR;
    }

    usDataLen =  (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr, "%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen, "%s: %s", "TYPE", stPortInfo.ProductName.aucProductName);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen, ": %s,",  stPortInfo.FmrId.aucMfrId);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen, "%s",  stPortInfo.PortId.aucPortInfo);

    gstAtSendData.usBufLen = usDataLen;
    return AT_OK;
}


VOS_UINT32   At_SetCvoicePara (VOS_UINT8 ucIndex)
{
    APP_VC_VOICE_MODE_ENUM_U16          usVoiceMode;


    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }


    /* 参数过多 */
    if(gucAtParaIndex != 1)
    {
        return AT_ERROR;
    }

    /* 目前只支持PC VOICE模式 */
    usVoiceMode = (APP_VC_VOICE_MODE_ENUM_U16)gastAtParaList[0].ulParaValue;

    if (APP_VC_VOICE_MODE_PCVOICE != usVoiceMode)
    {
        return AT_ERROR;
    }

    /* 返回值为AT_SUCCESS改为VOS_OK，对应起来  */
    if (VOS_OK == APP_VC_SetVoiceMode(gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      (VOS_UINT8)APP_VC_AppVcVoiceMode2VcPhyVoiceMode(usVoiceMode),
                                      I0_WUEPS_PID_VC))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CVOICE_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }



}
VOS_UINT32 At_SetDdsetexPara (VOS_UINT8 ucIndex)
{
    APP_VC_VOICE_PORT_ENUM_U8           ucVoicePort;


    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 目前只支持DIAG */
    ucVoicePort = (APP_VC_VOICE_PORT_ENUM_U8)gastAtParaList[0].ulParaValue;

    if (APP_VC_VOICE_PORT_DIAG != ucVoicePort)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (VOS_OK == APP_VC_SetVoicePort(gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      ucVoicePort))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DDSETEX_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }

}
TAF_UINT32 At_SetCmsrPara (TAF_UINT8 ucIndex)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((0 != gastAtParaList[0].ulParaValue)&&( 1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 参数设置 */
    pstSmsCtx->ucParaCmsr = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    return AT_OK;
}
TAF_UINT32 At_SetCmgiPara (TAF_UINT8 ucIndex)
{
    MN_MSG_STATUS_TYPE_ENUM_U8          enMsgStatus;
    MN_MSG_LIST_PARM_STRU               stListPara;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (gucAtParaIndex > 1)
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 设置<stat> */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }
    else
    {
        enMsgStatus = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }

    stListPara.bChangeFlag = VOS_FALSE;
    stListPara.enMemStore  = pstSmsCtx->stCpmsInfo.enMemReadorDelete;
    stListPara.enStatus    = enMsgStatus;
    PS_MEM_SET(stListPara.aucReserve1, 0x00, sizeof(stListPara.aucReserve1));

    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if (MN_ERR_NO_ERROR == MN_MSG_Delete_Test(gastAtClientTab[ucIndex].usClientId,
                                              gastAtClientTab[ucIndex].opId,
                                              &stListPara))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMGI_SET;
        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32 At_SetCmmtPara (TAF_UINT8 ucIndex)
{
    MN_MSG_MODIFY_STATUS_PARM_STRU               stModifyParm;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
          return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 参数检查 */
    if ((AT_CMD_OPT_SET_PARA_CMD == g_stATParseCmd.ucCmdOptType) && (0 == gastAtParaList[0].usParaLen))
    {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* 初始化 */
    PS_MEM_SET(&stModifyParm,0x00,sizeof(stModifyParm));

    stModifyParm.ulIndex = gastAtParaList[0].ulParaValue;

    /* 设置<stat> */
    if (0 == gastAtParaList[1].ulParaValue)
    {
        stModifyParm.enStatus = MN_MSG_STATUS_MT_NOT_READ;
    }
    else
    {
        stModifyParm.enStatus = MN_MSG_STATUS_MT_READ;
    }

    /* 设置读取存储器 */
    stModifyParm.enMemStore = pstSmsCtx->stCpmsInfo.enMemReadorDelete;

     /* 执行命令操作 */
    gastAtClientTab[ucIndex].opId = At_GetOpId();
    if (MN_ERR_NO_ERROR == MN_MSG_ModifyStatus(gastAtClientTab[ucIndex].usClientId,
                                               gastAtClientTab[ucIndex].opId,
                                               &stModifyParm))

    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMMT_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetUssdModePara(TAF_UINT8 ucIndex)
{
    USSD_TRANS_MODE_STRU                stUssdTranMode;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 输入参数取值检查, 必须为 0/1 */
    if ((1 != gastAtParaList[0].ulParaValue) && (0 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 更新全局变量 */
    pstSsCtx                        = AT_GetModemSsCtxAddrFromClientId(ucIndex);
    pstSsCtx->usUssdTransMode       = (VOS_UINT16)gastAtParaList[0].ulParaValue;

    stUssdTranMode.ucStatus         = VOS_TRUE;
    stUssdTranMode.ucUssdTransMode  = (VOS_UINT8)pstSsCtx->usUssdTransMode;

    /* 设置完拨号模式后，给C核发送消息 */
    if (TAF_SUCCESS != AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                               gastAtClientTab[ucIndex].opId,
                                               TAF_MSG_SET_USSDMODE_MSG,
                                               &stUssdTranMode,
                                               sizeof(USSD_TRANS_MODE_STRU),
                                               I0_WUEPS_PID_TAF))
    {
        return AT_ERROR;
    }

    return AT_OK;
}
TAF_UINT32 At_SetAdcTempPara(TAF_UINT8 ucIndex)
{
    SPY_TEMP_THRESHOLD_PARA_STRU stTempPara;
    VOS_UINT32                   ulChangeFlag = SPY_TEMP_THRESHOLD_PARA_UNCHANGE;
    VOS_UINT32                   ulRet;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 4)
    {
        return AT_TOO_MANY_PARA;
    }

    /*全设为无效值*/
    PS_MEM_SET(&stTempPara,0xFF,sizeof(stTempPara));


    if(0 != gastAtParaList[0].usParaLen)
    {
        stTempPara.ulIsEnable   = gastAtParaList[0].ulParaValue;
        ulChangeFlag = SPY_TEMP_THRESHOLD_PARA_CHANGE;
    }

    if(0 != gastAtParaList[1].usParaLen)
    {
        stTempPara.lCloseAdcThreshold = (VOS_INT)gastAtParaList[1].ulParaValue;
        ulChangeFlag = SPY_TEMP_THRESHOLD_PARA_CHANGE;
    }

    if(0 != gastAtParaList[2].usParaLen)
    {
        stTempPara.lAlarmAdcThreshold = (VOS_INT)gastAtParaList[2].ulParaValue;
        ulChangeFlag = SPY_TEMP_THRESHOLD_PARA_CHANGE;
    }

    if(0 != gastAtParaList[3].usParaLen)
    {
        stTempPara.lResumeAdcThreshold = (VOS_INT)gastAtParaList[3].ulParaValue;
        ulChangeFlag = SPY_TEMP_THRESHOLD_PARA_CHANGE;
    }

    if(SPY_TEMP_THRESHOLD_PARA_CHANGE == ulChangeFlag)
    {
        /*调用接口设置门限值*/
        /* 发消息到C核设置 热保护温度状态  */
        ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                       gastAtClientTab[ucIndex].opId,
                                       DRV_AGENT_ADC_SET_REQ,
                                       &stTempPara,
                                       sizeof(stTempPara),
                                       I0_WUEPS_PID_DRV_AGENT);
        if (TAF_SUCCESS != ulRet)
        {
            AT_WARN_LOG("At_SetAdcTempPara: AT_FillAndSndAppReqMsg fail.");
            return AT_ERROR;
        }

        /* 设置AT模块实体的状态为等待异步返回 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ADC_SET;
        return AT_WAIT_ASYNC_RETURN;

    }

    return AT_OK;
}
TAF_UINT32 At_SetCommDebugPara(TAF_UINT8 ucIndex)
{
    SPY_TEMP_THRESHOLD_PARA_STRU        stTempPara;
    TAF_AT_NVIM_COMMDEGBUG_CFG_STRU     stDebugFlag;
    VOS_UINT16                          usFlag;
    USIMM_MsgBlock                     *pstMsg = VOS_NULL_PTR;
#if ( VOS_WIN32 == VOS_OS_VER )
    VOS_UINT8                           aucFilelist[]={0x6f,0x43};
#endif

    stDebugFlag.ulCommDebugFlag = VOS_FALSE;

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /*全设为无效值*/
    PS_MEM_SET(&stTempPara,0xFF,sizeof(stTempPara));


    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch(gastAtParaList[0].ulParaValue)
    {
        case 0x00:
            /*去激活所有Debug功能*/
            if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_COMMDEGBUG_CFG, &(stDebugFlag.ulCommDebugFlag), sizeof(TAF_AT_NVIM_COMMDEGBUG_CFG_STRU)))
            {
                LogPrint("At_SetOamDebugPara:NV_ReadEx Debug Flag0 Fail\r\n");

                return AT_CME_OPERATION_NOT_ALLOWED;
            }

            if(0x00 != stDebugFlag.ulCommDebugFlag)
            {
                stDebugFlag.ulCommDebugFlag = 0x00;

                if(NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_COMMDEGBUG_CFG,&(stDebugFlag.ulCommDebugFlag),sizeof(TAF_AT_NVIM_COMMDEGBUG_CFG_STRU)))
                {
                    LogPrint("At_SetOamDebugPara:NV_WriteEx Debug Flag0 Fail\r\n");

                    return AT_CME_OPERATION_NOT_ALLOWED;
                }

            }

            break;
        case 0x01:
            /*使能DRX定位信息上报功能*/
            if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_COMMDEGBUG_CFG, &(stDebugFlag.ulCommDebugFlag), sizeof(TAF_AT_NVIM_COMMDEGBUG_CFG_STRU)))
            {
                LogPrint("At_SetOamDebugPara:NV_ReadEx Debug Flag Fail\r\n");

                return AT_CME_OPERATION_NOT_ALLOWED;
            }

            if(0x01 != (stDebugFlag.ulCommDebugFlag&0x01))
            {
                stDebugFlag.ulCommDebugFlag |= 0x01;

                if(NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_COMMDEGBUG_CFG,&(stDebugFlag.ulCommDebugFlag),sizeof(TAF_AT_NVIM_COMMDEGBUG_CFG_STRU)))
                {
                    LogPrint("At_SetOamDebugPara:NV_WriteEx Debug Flag Fail\r\n");

                    return AT_CME_OPERATION_NOT_ALLOWED;
                }

            }

            break;
#if 0
        case 0x02:
            /*调用USIMM初始化信息记录接口*/
            /* 接口打桩致LINT不过 */
            if(VOS_OK != DRV_USIMMSCI_RECORD_DATA_SAVE())
            {
                return AT_CME_MEMORY_FAILURE;
            }
            /*lint +e774 +e506*/
            break;
#endif
        case 0x03:
            /*使能USIMM初始化信息记录功能*/
            if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_COMMDEGBUG_CFG, &(stDebugFlag.ulCommDebugFlag), sizeof(TAF_AT_NVIM_COMMDEGBUG_CFG_STRU)))
            {
                LogPrint("At_SetOamDebugPara:NV_ReadEx Debug Flag2 Fail\r\n");

                return AT_CME_OPERATION_NOT_ALLOWED;
            }

            if(0x02 != (stDebugFlag.ulCommDebugFlag&0x02))
            {
                stDebugFlag.ulCommDebugFlag |= 0x02;

                if(NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_COMMDEGBUG_CFG,&(stDebugFlag.ulCommDebugFlag),sizeof(TAF_AT_NVIM_COMMDEGBUG_CFG_STRU)))
                {
                    LogPrint("At_SetOamDebugPara:NV_WriteEx Debug Flag2 Fail\r\n");

                    return AT_CME_OPERATION_NOT_ALLOWED;
                }

            }

            break;

        case 0x05:

            usFlag = 1;

            if(NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Usim_Debug_Mode_Set, &usFlag, sizeof(VOS_UINT16)))
            {
                LogPrint("At_SetOamDebugPara:NV_WriteEx Usim Debug Flag Fail\r\n");

                return AT_CME_OPERATION_NOT_ALLOWED;
            }

            break;

        case 0x06:

            usFlag = 0;

            if(NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Usim_Debug_Mode_Set, &usFlag, sizeof(VOS_UINT16)))
            {
                LogPrint("At_SetOamDebugPara:NV_WriteEx Usim Debug Flag Fail\r\n");

                return AT_CME_OPERATION_NOT_ALLOWED;
            }

            break;

        case 0x07:
            pstMsg = (USIMM_MsgBlock *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(USIMM_MsgBlock)-VOS_MSG_HEAD_LENGTH);

            if(VOS_NULL_PTR == pstMsg)
            {
                LogPrint("USIMM_InitCardStart:AllocMsg Failed.");

                return VOS_ERR;
            }

            pstMsg->ulReceiverPid      = WUEPS_PID_USIM;
            pstMsg->enMsgType          = USIMM_CMDTYPE_INITSTART;
            if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
            {
                LogPrint("USIMM_InitCardStart:sndmsg Failed.");
            }
            break;

        default:
            break;
    }

    return AT_OK;
}


TAF_UINT32 AT_SetDwinsPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulEnabled;
    WINS_CONFIG_STRU                    stWins;
    VOS_UINT32                          ulLength;

    ulLength = 0;
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置参数 */
    ulEnabled = gastAtParaList[0].ulParaValue;
    NV_GetLength(en_NV_Item_WINS_Config, &ulLength);
    /* 填充stWins结构体 */
    PS_MEM_SET(&stWins, 0, sizeof(WINS_CONFIG_STRU));

    if ( NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_WINS_Config, &stWins, ulLength) )
    {
        if ( (1 == stWins.ucStatus) && (ulEnabled == stWins.ucWins) )
        {
            AT_INFO_LOG("AT_SetDwinsPara():The content to write is same as NV's");

            /* 更新PPP的WINS设置 */
            PPP_UpdateWinsConfig((VOS_UINT8)gastAtParaList[0].ulParaValue);

            /* 更新APS的WINS设置 */
            TAF_PS_ConfigNbnsFunction(WUEPS_PID_AT,
                                      gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      ulEnabled);

            return AT_OK;
        }
    }

    stWins.ucStatus = 1;
    stWins.ucWins   = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 写入NVIM */
    NV_GetLength(en_NV_Item_WINS_Config, &ulLength);
    if ( NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_WINS_Config, &stWins, ulLength) )
    {
        return AT_ERROR;
    }
    else
    {
        /* 更新PPP的WINS设置 */
        PPP_UpdateWinsConfig((VOS_UINT8)gastAtParaList[0].ulParaValue);

        /* 更新APS的WINS设置 */
        TAF_PS_ConfigNbnsFunction(WUEPS_PID_AT,
                                  gastAtClientTab[ucIndex].usClientId,
                                  0,
                                  ulEnabled);

        return AT_OK;
    }
}


TAF_UINT32 At_SetYjcxPara (TAF_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_YJCX_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_YJCX_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
TAF_UINT32 At_SetAt2OmPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 该命令无输入参数 */
    if(0 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*ucIndex所对应的AT通道不是AT/OM通道*/
    if ( (AT_USB_COM_PORT_NO != gastAtClientTab[ucIndex].ucPortNo)
      && (AT_UART_PORT_NO    != gastAtClientTab[ucIndex].ucPortNo)
      && (AT_CTR_PORT_NO     != gastAtClientTab[ucIndex].ucPortNo)
      && (AT_HSUART_PORT_NO  != gastAtClientTab[ucIndex].ucPortNo))
    {
        return AT_ERROR;
    }

    /*检查是否已有处于OM模式的AT通道*/
    if(AT_SUCCESS == At_CheckDataState(AT_OM_DATA_MODE))
    {
        return AT_ERROR;
    }

    /*记录AT/OM通道所对应的索引号*/
    gucAtOmIndex = ucIndex;

    /* 通知OAM切换至OM模式 */
    if (AT_USB_COM_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CPM_NotifyChangePort(AT_PCUI_PORT, CPM_OM_COMM);
    }
    else if(AT_CTR_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        vos_printf("\r\nAt_SetAt2OmPara:Change The AT Ctrl to OM, index is %d", ucIndex);

        At_FormatResultData(AT_CLIENT_TAB_CTRL_INDEX, AT_OK); /*需要先回复OK*/

        VOS_TaskDelay(20);  /*保证返回数据发送完成*/

        /* 通知LMSP 关闭ctrl口*/

        if(VOS_OK != DMS_VcomCtrlClose())
        {
            vos_printf("\r\nAt_SetAt2OmPara:Close The AT Ctrl to OM Error");
        }
        /* 通知GU MSP 打开ctrl口*/
        GU_OamCtrlPortInit();

        /* 通知CPM ctrl口已经切换完成*/
        CPM_NotifyChangePort(CPM_CTRL_PORT, CPM_OM_COMM);
    }
    else if (AT_UART_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CPM_NotifyChangePort(AT_UART_PORT, CPM_OM_COMM);
    }

    else if (AT_HSUART_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CPM_NotifyChangePort(AT_HSUART_PORT, CPM_OM_COMM);
    }
    else
    {
        /* make pc_lint happy */
    }

    /*将通道模式设置为OM模式*/
    At_SetMode(gucAtOmIndex, AT_DATA_MODE, AT_OM_DATA_MODE);

    return AT_OK;
}
#else
TAF_UINT32 At_SetAt2OmPara(TAF_UINT8 ucIndex)
{
    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 该命令无输入参数 */
    if(0 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*ucIndex所对应的AT通道不是AT/OM通道*/
    if ( (AT_USB_COM_PORT_NO != gastAtClientTab[ucIndex].ucPortNo)
      && (AT_UART_PORT_NO    != gastAtClientTab[ucIndex].ucPortNo)
      && (AT_CTR_PORT_NO     != gastAtClientTab[ucIndex].ucPortNo)
      && (AT_HSUART_PORT_NO  != gastAtClientTab[ucIndex].ucPortNo))
    {
        return AT_ERROR;
    }

    /*检查是否已有处于OM模式的AT通道*/
    if(AT_SUCCESS == At_CheckDataState(AT_OM_DATA_MODE))
    {
        return AT_ERROR;
    }

    /*记录AT/OM通道所对应的索引号*/
    gucAtOmIndex = ucIndex;

    /* 通知OAM切换至OM模式 */
    if (AT_USB_COM_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CBTCPM_NotifyChangePort(AT_PCUI_PORT);
    }
    else if (AT_UART_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CBTCPM_NotifyChangePort(AT_UART_PORT);
    }

    else if (AT_HSUART_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
    {
        CBTCPM_NotifyChangePort(AT_HSUART_PORT);
    }
    else
    {
        /* make pc_lint happy */
    }

    /*将通道模式设置为OM模式*/
    At_SetMode(gucAtOmIndex, AT_DATA_MODE, AT_OM_DATA_MODE);

    return AT_OK;
}
#endif
TAF_UINT32 AT_ResumeOm2AtMode(TAF_VOID)
{
    /*检查是否已有处于OM模式的通道*/
    if(AT_SUCCESS != At_CheckDataState(AT_OM_DATA_MODE))
    {
        return VOS_ERR;
    }

    At_SetMode(gucAtOmIndex,AT_CMD_MODE,AT_NORMAL_MODE);   /* 返回命令模式 */
    return VOS_OK;
}

VOS_UINT32 AT_GetOperatorNameFromParam(
    VOS_UINT16                         *usOperNameLen,
    VOS_CHAR                           *pucOperName,
    VOS_UINT16                          usBufSize,
    VOS_UINT32                          ulFormatType
)
{
    if ((AT_COPS_LONG_ALPH_TYPE == ulFormatType)
     && (usBufSize > gastAtParaList[2].usParaLen)
     && (0 != gastAtParaList[2].usParaLen))
    {
        PS_MEM_CPY((VOS_CHAR*)pucOperName,(VOS_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
        pucOperName[gastAtParaList[2].usParaLen] = 0;
        *usOperNameLen = gastAtParaList[2].usParaLen;

        return AT_OK;
    }
    else if ((AT_COPS_SHORT_ALPH_TYPE == ulFormatType)
          && (usBufSize > gastAtParaList[2].usParaLen)
          && (0 != gastAtParaList[2].usParaLen))
    {
        PS_MEM_CPY((VOS_CHAR*)pucOperName,(VOS_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
        pucOperName[gastAtParaList[2].usParaLen] = 0;
        *usOperNameLen = gastAtParaList[2].usParaLen;

        return AT_OK;
    }
    else if ((usBufSize > gastAtParaList[2].usParaLen)
          && ((5 == gastAtParaList[2].usParaLen)
           || (6 == gastAtParaList[2].usParaLen)))
    {
        PS_MEM_CPY((VOS_CHAR*)pucOperName,(VOS_CHAR*)gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen);
        pucOperName[gastAtParaList[2].usParaLen] = 0;
        *usOperNameLen = gastAtParaList[2].usParaLen;

        return AT_OK;
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
}

VOS_UINT32 At_SetCpolPara(VOS_UINT8 ucIndex)
{
    TAF_PH_SET_PREFPLMN_STRU            stPrefPlmn;
    VOS_UINT32                          ulRst;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_SET(&stPrefPlmn,0,sizeof(stPrefPlmn));

    /* 参数过多 */
#if(FEATURE_ON == FEATURE_LTE)
    if ( gucAtParaIndex > 7 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#else
    if ( gucAtParaIndex > 6 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#endif

    if ( (2 == gucAtParaIndex)
      && (0 == gastAtParaList[0].usParaLen)
      && (0 != gastAtParaList[1].usParaLen))
    {
        /* 当前只存在<format>，如果当前对应的<oper>能够获取，则读取命令中<oper>
           的format需要改变 */
        pstNetCtx->ucCpolFormatType = (AT_COPS_FORMAT_TYPE)gastAtParaList[1].ulParaValue;
        return AT_OK;
    }

    if (pstNetCtx->enPrefPlmnType > MN_PH_PREF_PLMN_HPLMN)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*保存参数到结构中，发送给MMA进行处理*/
    stPrefPlmn.enPrefPlmnType           = pstNetCtx->enPrefPlmnType;
    stPrefPlmn.ucAtParaIndex            = gucAtParaIndex;
    stPrefPlmn.usIndexLen               = gastAtParaList[0].usParaLen;
    stPrefPlmn.usIndex                  = gastAtParaList[0].ulParaValue;
    stPrefPlmn.usFormatLen              = gastAtParaList[1].usParaLen;
    stPrefPlmn.usFormat                 = gastAtParaList[1].ulParaValue;
    stPrefPlmn.usGsmRatLen              = gastAtParaList[3].usParaLen;
    stPrefPlmn.usGsmRat                 = gastAtParaList[3].ulParaValue;
    stPrefPlmn.usGsmCompactRatLen       = gastAtParaList[4].usParaLen;
    stPrefPlmn.usGsmCompactRat          = gastAtParaList[4].ulParaValue;
    stPrefPlmn.usUtrnLen                = gastAtParaList[5].usParaLen;
    stPrefPlmn.usUtrn                   = gastAtParaList[5].ulParaValue;

#if(FEATURE_ON == FEATURE_LTE)
    /* LTE的接入技术以及长度 */
    stPrefPlmn.usEutrnLen                = gastAtParaList[6].usParaLen;
    stPrefPlmn.usEutrn                   = gastAtParaList[6].ulParaValue;
#endif

    if (0 < stPrefPlmn.usFormatLen)
    {
        pstNetCtx->ucCpolFormatType = (AT_COPS_FORMAT_TYPE)stPrefPlmn.usFormat;

        ulRst = AT_GetOperatorNameFromParam(&stPrefPlmn.usOperNameLen,
                                             stPrefPlmn.ucOperName,
                                             sizeof(stPrefPlmn.ucOperName),
                                             stPrefPlmn.usFormat);
        if ( AT_OK != ulRst )
        {
            return ulRst;
        }
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   TAF_MSG_MMA_SET_PREF_PLMN,
                                   (VOS_UINT8*)&stPrefPlmn, /*优先网络列表操作请求*/
                                   sizeof(TAF_PH_SET_PREFPLMN_STRU),
                                   I0_WUEPS_PID_MMA);

    if( TAF_SUCCESS == ulRst )
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPOL_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;

}
VOS_UINT32 At_SetCplsPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                           ulRst;
    MN_PH_PREF_PLMN_TYPE_ENUM_U8         ucPrefPlmnType;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        ucPrefPlmnType = (MN_PH_PREF_PLMN_TYPE_ENUM_U8)gastAtParaList[0].ulParaValue;
    }
    else
    {
        ucPrefPlmnType = MN_PH_PREF_PLMN_UPLMN;
    }

    if (ucPrefPlmnType > MN_PH_PREF_PLMN_HPLMN)
    {
        return AT_CME_OPERATION_NOT_SUPPORTED;
    }

    /* 发送消息 TAF_MSG_MMA_SET_PREF_PLMN_TYPE 给 MMA 处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   TAF_MSG_MMA_SET_PREF_PLMN_TYPE,
                                   (VOS_VOID*)&ucPrefPlmnType, /*要求设置的优先网络类型*/
                                   sizeof(VOS_UINT8),
                                   I0_WUEPS_PID_MMA);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MMA_SET_PREF_PLMN_TYPE;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;

}
TAF_UINT32 At_SetpidPara(TAF_UINT8 ucIndex)
{
    TAF_UINT32           ulRslt;
    TAF_UINT8            diagValue;

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 输入参数取值检查, 必须为 0/1 */
    if ((1 != gastAtParaList[0].ulParaValue) && (0 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    diagValue = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    ulRslt = (VOS_UINT32)(DRV_SET_PID(diagValue));
    if( VOS_OK != ulRslt )
    {
        AT_WARN_LOG("At_SetpidPara:Setpid fail.");
        return AT_ERROR;
    }

    return AT_OK;

  }
VOS_UINT32 AT_SetOpwordParaForApModem ( VOS_UINT8  ucIndex )
{
    VOS_UINT32                              ulResult;
    DRV_AGENT_OPWORD_SET_REQ_STRU           stOpwordSetReq;

    /* 局部变量初始化 */
    PS_MEM_SET(&stOpwordSetReq, 0x00, sizeof(DRV_AGENT_OPWORD_SET_REQ_STRU));

    /* 检查码流参数长度 */
    if (DRV_AGENT_PORT_PASSWORD_LEN != gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 密码的合法性检查，密码为16个“0”-“9”ASCII字符,密码由ASCII码转换为十进制数字 */
    ulResult = At_AsciiNum2Num(stOpwordSetReq.aucPortPassword,
                               gastAtParaList[0].aucPara,
                               DRV_AGENT_PORT_PASSWORD_LEN);
    if ( AT_FAILURE == ulResult)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 转换成功, 发送跨核消息到C核, 获取控制DIAG/SHELL口权限 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_OPWORD_SET_REQ,
                                      &stOpwordSetReq,
                                      sizeof(DRV_AGENT_OPWORD_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetOpwordParaForApModem: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_OPWORD_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetOpwordPara ( VOS_UINT8  ucIndex )
{
    VOS_UINT32                              ulCheckRlst = VOS_FALSE;
    VOS_UINT8                              *pucSystemAppConfig;
    VOS_UINT32                              ulResult;

    /* 读取NV项中当前产品形态 */
    pucSystemAppConfig = AT_GetSystemAppConfigAddr();

    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    /* 已经具有权限  直接返回OK */
    if (AT_E5_RIGHT_FLAG_YES == g_enATE5RightFlag)
    {
        return AT_OK;
    }

    /* 若当前设备的形态为AP-MODEM */
    if ( SYSTEM_APP_ANDROID == *pucSystemAppConfig)
    {
        ulResult = AT_SetOpwordParaForApModem(ucIndex);
        return ulResult;
    }

    /* 输入密码超长 */
    if ( gastAtParaList[0].usParaLen > AT_DISLOG_PWD_LEN )
    {
        return AT_ERROR;
    }

    /* 密码比较 */
    if (0 == VOS_StrLen((VOS_CHAR *)g_acATOpwordPwd))
    {
        /* NV中密码为空则输入任何密码都能获取权限 */
        ulCheckRlst = VOS_TRUE;
    }
    else
    {
        if (0 == VOS_StrCmp((VOS_CHAR*)g_acATOpwordPwd, (VOS_CHAR*)gastAtParaList[0].aucPara))
        {
            ulCheckRlst = VOS_TRUE;
        }
    }

    if ( VOS_FALSE == ulCheckRlst )
    {
        return AT_ERROR;
    }

    /* 密码比对成功  获取权限 */
    g_enATE5RightFlag = AT_E5_RIGHT_FLAG_YES;

    /* 按D25的做法  不保存权限标志 */

    return AT_OK;
}


VOS_UINT32 AT_SetCpwordPara ( VOS_UINT8  ucIndex )
{
    /*已经不具有权限*/
    if (AT_E5_RIGHT_FLAG_NO == g_enATE5RightFlag )
    {
        return AT_OK;
    }

    /* 释放权限 */
    g_enATE5RightFlag = AT_E5_RIGHT_FLAG_NO;

    /* 按D25的做法  不保存权限标志 */

    return AT_OK;
}
VOS_UINT32 AT_SetDislogPara ( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRet;
    AT_PID_ENABLE_TYPE_STRU             stPidEnableType;

#if (FEATURE_ON==FEATURE_LTE)
    VOS_UINT32                          ulRetDiag;
    VOS_UINT32                          ulRet3GDiag;
    VOS_UINT32                          ulRetGps;
#endif


    stPidEnableType.ulPidEnabled = VOS_FALSE;


    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen) || (gucAtParaIndex > 1))
    {
        return AT_ERROR;
    }

    /* 切换PCUI口到DIAG服务 */
    if ( AT_DISLOG_PCUI_TO_DIAG == gastAtParaList[0].ulParaValue )
    {
        /* ucIndex所对应的AT通道不是PCUI/UART通道 */
        if ((AT_USB_COM_PORT_NO != gastAtClientTab[ucIndex].ucPortNo)
         && (AT_UART_PORT_NO != gastAtClientTab[ucIndex].ucPortNo))
        {
            return AT_ERROR;
        }

        /*检查是否已有处于DIAG模式的通道*/
        if(AT_SUCCESS == At_CheckDataState(AT_DIAG_DATA_MODE))
        {
            return AT_ERROR;
        }

        /*记录AT/DIAG通道所对应的索引号*/
        gucOmDiagIndex = ucIndex;

        /* 通知OAM切换至DIAG模式 */
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
        if (AT_USB_COM_PORT_NO == gastAtClientTab[ucIndex].ucPortNo)
        {
            CPM_NotifyChangePort(AT_PCUI_PORT, CPM_DIAG_COMM);
        }
        else
        {
            CPM_NotifyChangePort(AT_UART_PORT, CPM_DIAG_COMM);
        }
#endif

        /*将通道模式设置为DIAG模式*/
        At_SetMode(gucOmDiagIndex, AT_DATA_MODE, AT_DIAG_DATA_MODE);

        return AT_OK;
    }

    /* 读取PID使能NV项 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_PID_Enable_Type,
                        &stPidEnableType,
                        sizeof(AT_PID_ENABLE_TYPE_STRU)))
    {
        AT_ERR_LOG("AT_SetDislogPara:Read NV failed!");
        return AT_ERROR;
    }

    /* NV读取成功，检查PID是否使能，若不使能，返回AT_OK */
    if (VOS_TRUE != stPidEnableType.ulPidEnabled)
    {
        AT_WARN_LOG("AT_SetDislogPara:PID is not enabled");
        return AT_OK;
    }


    /*  打开DIAG口*/
    if ( AT_DISLOG_DIAG_OPEN == gastAtParaList[0].ulParaValue )
    {
#if (FEATURE_ON==FEATURE_LTE)
        ulRet3GDiag   = AT_OpenSpecificPort(AT_DEV_DIAG);
        ulRetDiag     = AT_OpenSpecificPort(AT_DEV_4G_DIAG);
        ulRetGps      = AT_OpenSpecificPort(AT_DEV_4G_GPS);

        if((AT_OK == ulRetDiag) && (AT_OK == ulRetGps) && (AT_OK == ulRet3GDiag))
        {
            ulRet = AT_OK;
        }
        else
        {
            ulRet = AT_ERROR;
        }
#else
        ulRet = AT_OpenDiagPort();
#endif
        return ulRet;
    }

    /* 关闭DIAG口 */
    if ( AT_DISLOG_DIAG_CLOSE == gastAtParaList[0].ulParaValue )
    {
#if (FEATURE_ON == FEATURE_LTE)
        ulRet3GDiag = AT_CloseSpecificPort(AT_DEV_DIAG);
        ulRetDiag   = AT_CloseSpecificPort(AT_DEV_4G_DIAG);
        ulRetGps    = AT_CloseSpecificPort(AT_DEV_4G_GPS);

        if((AT_OK == ulRetDiag) && (AT_OK == ulRetGps) && (AT_OK == ulRet3GDiag))
        {
            ulRet = AT_OK;
        }
        else
        {
            ulRet = AT_ERROR;
        }

#else
        ulRet = AT_CloseDiagPort();
#endif
        return ulRet;
    }


    return AT_ERROR;

}
#if (FEATURE_ON == FEATURE_SECURITY_SHELL)
VOS_UINT32 AT_SetSpwordPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           i;
    DRV_AGENT_SPWORD_SET_REQ_STRU       stSpwordSetReq;

    /* 如果超过三次，则在系统重新启动前不再处理该命令，直接返回Error */
    if (g_stSpWordCtx.ucErrTimes >= AT_SHELL_PWD_VERIFY_MAX_TIMES)
    {
        AT_WARN_LOG("AT_SetSpwordPara:WARNING: Verify Max Times!");
        return AT_ERROR;
    }

    /* 一旦AT^SPWORD命令校验成功，系统不再对用户以后输入的AT^SPWORD命令进行处理，直接返回ERROR */
    if (VOS_TRUE == g_stSpWordCtx.ucShellPwdCheckFlag)
    {
        AT_WARN_LOG("AT_SetSpwordPara:WARNING: Not need Verified!");
        return AT_ERROR;
    }

    /* 有且仅有一个参数，参数长度为8 */
    if ((gucAtParaIndex > 1)
     || (gastAtParaList[0].usParaLen != AT_SHELL_PWD_LEN))
    {
        AT_WARN_LOG("AT_SetSpwordPara:WARNING: Parameter error!");
        return AT_ERROR;
    }

    /* 参数只能为字符0-9 */
    for (i = 0; i < AT_SHELL_PWD_LEN; i++)
    {
        if ((gastAtParaList[0].aucPara[i] < '0')
         || (gastAtParaList[0].aucPara[i] > '9'))
        {
            AT_WARN_LOG("AT_SetSpwordPara:WARNING: PWD NOT 0-9!");
            return AT_ERROR;
        }
    }

    /* 密码校验需要在C核实现 */
    VOS_MemCpy(stSpwordSetReq.acShellPwd, (VOS_CHAR *)gastAtParaList[0].aucPara, AT_SHELL_PWD_LEN);

    if (TAF_SUCCESS == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                              gastAtClientTab[ucIndex].opId,
                                              DRV_AGENT_SPWORD_SET_REQ,
                                              &stSpwordSetReq,
                                              sizeof(stSpwordSetReq),
                                              I0_WUEPS_PID_DRV_AGENT))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SPWORD_SET;             /*设置当前操作模式 */
        return AT_WAIT_ASYNC_RETURN;                                            /* 等待异步事件返回 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_SetShellPara ( VOS_UINT8 ucIndex )
{
    AT_SHELL_OPEN_FLAG_ENUM_U32         enTmpOpenFlag;

    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    /*  WIFI使用SHELL口 */
    if (AT_SHELL_WIFI_USE == gastAtParaList[0].ulParaValue)
    {
        /* 保存WIFI使用SHELL结果到NV项 */
        enTmpOpenFlag = AT_SHELL_OPEN_FLAG_WIFI;
        if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_AT_SHELL_OPEN_FLAG, &enTmpOpenFlag,
                              sizeof(enTmpOpenFlag)))
        {
            AT_WARN_LOG("AT_SetShellPara:WARNING:NV_WriteEx SHELL_OPEN_FLAG faild!");
            return AT_ERROR;
        }

        return AT_OK;
    }

    /*  打开SHELL口 */
    if (AT_SHELL_OPEN == gastAtParaList[0].ulParaValue )
    {
        /* 判断CK是否校验通过 */
        if (g_stSpWordCtx.ucShellPwdCheckFlag != VOS_TRUE)
        {
            AT_WARN_LOG("AT_SetShellPara:WARNING:NV_WriteEx SHELL_OPEN_FLAG faild!");

            return AT_ERROR;
        }

        /* 保存打开结果到NV项 */
        enTmpOpenFlag = AT_SHELL_OPEN_FLAG_OPEN;
        if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_AT_SHELL_OPEN_FLAG, &enTmpOpenFlag,
                              sizeof(enTmpOpenFlag)))
        {
            AT_WARN_LOG("AT_SetShellPara:WARNING:NV_WriteEx SHELL_OPEN_FLAG faild!");

            return AT_ERROR;
        }

        return AT_OK;
    }

    /*关闭SHELL口*/
    if ( AT_SHELL_CLOSE == gastAtParaList[0].ulParaValue )
    {
        /* 保存当前SHELL口的关闭状态到NV项 */
        enTmpOpenFlag = AT_SHELL_OPEN_FLAG_CLOSE;
        if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_AT_SHELL_OPEN_FLAG, &enTmpOpenFlag,
                              sizeof(enTmpOpenFlag)))
        {
            AT_WARN_LOG("AT_SetShellPara:WARNING:NV_WriteEx SHELL_OPEN_FLAG faild!");
            return AT_ERROR;
        }

        return AT_OK;
    }

    return AT_ERROR;
}
#endif

#if (FEATURE_ON==FEATURE_LTE)



VOS_UINT32 AT_SetRsrpCfgPara ( VOS_UINT8 ucIndex )
{

    NVIM_RSRP_CFG_STRU stRsrpCfg;
    VOS_UINT32 RsrpLevel =0;

    PS_MEM_SET(&stRsrpCfg, 0, sizeof(stRsrpCfg));

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen)
        || (0 == gastAtParaList[1].usParaLen)
        || (gucAtParaIndex != 2))
    {
        return AT_ERROR;
    }

    if (NV_OK != NVM_Read(EN_NV_ID_RSRP_CFG, &stRsrpCfg,
                          sizeof(stRsrpCfg)))
    {
        vos_printf("AT_SetRsrpCfgPara:WARNING:NV_ReadEx RSRP faild!\n");
        return AT_ERROR;
    }

    /* 根据输入值设置NV项 */
    RsrpLevel = gastAtParaList[0].ulParaValue;
    stRsrpCfg.ssLevel[RsrpLevel] = (VOS_INT16)(gastAtParaList[0].ulParaValue);
    stRsrpCfg.ssValue[RsrpLevel] = (VOS_INT16)(gastAtParaList[1].ulParaValue);
    if(g_enATE5RightFlag == AT_E5_RIGHT_FLAG_YES)
    {
        if(NV_OK != NVM_Write(EN_NV_ID_RSRP_CFG, &stRsrpCfg,
                          sizeof(stRsrpCfg)))
        {
            vos_printf("AT_SetRsrpCfgPara:WARNING:NV_WriteEx RSRP faild!\n");
            return AT_ERROR;
        }
    }
    else
    {
        return AT_ERROR; /* 如果权限未打开，返回ERROR */
    }

    return AT_OK;
}
#endif

#if (FEATURE_ON==FEATURE_LTE)

VOS_UINT32 AT_SetRscpCfgPara ( VOS_UINT8 ucIndex )
{
    NVIM_RSCP_CFG_STRU stRscpCfg;
    VOS_UINT32 RscpLevel =0;

    PS_MEM_SET(&stRscpCfg, 0, sizeof(stRscpCfg));

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen)
        || (0 == gastAtParaList[1].usParaLen)
        || (gucAtParaIndex != 2))
    {
            vos_printf("RSCP参数格式错误\n");
        return AT_ERROR;
    }

    if (NV_OK != NVM_Read(EN_NV_ID_RSCP_CFG, &stRscpCfg,
                          sizeof(stRscpCfg)))
    {
        vos_printf("RSCP read error!\n");
        return AT_ERROR;
    }

    /* 根据输入值设置NV项 */
    RscpLevel = gastAtParaList[0].ulParaValue;
    stRscpCfg.ssLevel[RscpLevel] = (VOS_INT16)(gastAtParaList[0].ulParaValue);
    stRscpCfg.ssValue[RscpLevel] = (VOS_INT16)(gastAtParaList[1].ulParaValue);
    if(g_enATE5RightFlag == AT_E5_RIGHT_FLAG_YES)
    {
        if (NV_OK != NVM_Write(EN_NV_ID_RSCP_CFG, &stRscpCfg,
                          sizeof(stRscpCfg)))
        {
            vos_printf("判断RSCP是否正确写入NV \n");
            return AT_ERROR;
        }
    }
    else
    {
        vos_printf("RSCP写入权限未开\n");
        return AT_ERROR; /* 如果权限未打开，返回ERROR */
    }

    return AT_OK;
}
#endif
#if (FEATURE_ON==FEATURE_LTE)

VOS_UINT32 AT_SetEcioCfgPara ( VOS_UINT8 ucIndex )
{
    NVIM_ECIO_CFG_STRU stEcioCfg;
    VOS_UINT32 EcioLevel =0;

    PS_MEM_SET(&stEcioCfg, 0, sizeof(stEcioCfg));

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen)
        || (0 == gastAtParaList[1].usParaLen)
        || (gucAtParaIndex != 2))
    {
            vos_printf("ECIO参数格式错误\n");
        return AT_ERROR;
    }

    if (NV_OK != NVM_Read(EN_NV_ID_ECIO_CFG, &stEcioCfg,
                          sizeof(stEcioCfg)))
    {
            vos_printf("ECIO read 错误\n");
        return AT_ERROR;
    }

    /* 根据输入值设置NV项 */
    EcioLevel = gastAtParaList[0].ulParaValue;
    stEcioCfg.ssLevel[EcioLevel] = (VOS_INT16)(gastAtParaList[0].ulParaValue);
    stEcioCfg.ssValue[EcioLevel] = (VOS_INT16)(gastAtParaList[1].ulParaValue);
    if(g_enATE5RightFlag == AT_E5_RIGHT_FLAG_YES)
    {
        if (NV_OK != NVM_Write(EN_NV_ID_ECIO_CFG, &stEcioCfg,
                          sizeof(stEcioCfg)))
        {
                vos_printf("ECIO写入错误\n");
            return AT_ERROR;
        }
    }
    else
    {
        vos_printf("ECIO写入权限未打开\n");
        return AT_ERROR; /* 如果权限未打开，返回ERROR */
    }

    return AT_OK;
}
#endif
#if (FEATURE_ON==FEATURE_LTE)

VOS_UINT32 AT_SetPdprofmodPara(VOS_UINT8 ucIndex)
{
    TAF_PDP_PROFILE_EXT_STRU    stPdpProfInfo;

    PS_MEM_SET(&stPdpProfInfo, 0, sizeof(TAF_PDP_PROFILE_EXT_STRU));

    /* 若无参数，则直接返回OK */
    if ( AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType )
    {
        return AT_OK;
    }

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 6 )
    {
        return AT_TOO_MANY_PARA;
    }

    stPdpProfInfo.ucCid                 = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stPdpProfInfo.ucDefined         = VOS_FALSE;
    }
    else
    {
        stPdpProfInfo.ucDefined         = VOS_TRUE;

        /* 设置<PDP_type> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            if ( gastAtParaList[1].ulParaValue < TAF_PDP_PPP )
            {
                stPdpProfInfo.bitOpPdpType   = VOS_TRUE;

                /* IP:1, IPV6:2, IPV4V6:3, PPP:4 */
                stPdpProfInfo.enPdpType  = (VOS_UINT8)(gastAtParaList[1].ulParaValue + 1);

#if ( FEATURE_OFF == FEATURE_IPV6 )
                if ( (TAF_PDP_IPV6   == stPdpProfInfo.enPdpType)
                  || (TAF_PDP_IPV4V6 == stPdpProfInfo.enPdpType) )
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
#endif
            }
        }
        /* 对比终端的结果，在<PDP_type>参数为""(两个引号)时,则保留原先的值，
           若g_TafCidTab[ucCid].ucUsed指示上下文被删除，则使用之前保留下来的值 */

        /* 设置<APN> */
        if ( gastAtParaList[2].usParaLen > TAF_MAX_APN_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if ( 0 != gastAtParaList[2].usParaLen )
        {
            /* 检查 */
            if ( VOS_OK != AT_CheckApnFormat(gastAtParaList[2].aucPara,
                                             gastAtParaList[2].usParaLen) )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            stPdpProfInfo.bitOpApn       = VOS_TRUE;
            PS_MEM_CPY((VOS_CHAR*)stPdpProfInfo.aucApn,
                       (VOS_CHAR*)gastAtParaList[2].aucPara,
                       gastAtParaList[2].usParaLen);
            stPdpProfInfo.aucApn[gastAtParaList[2].usParaLen] = 0;
        }

        /* 设置<username> */
        if ( gastAtParaList[3].usParaLen > TAF_MAX_AUTHDATA_USERNAME_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if(0 != gastAtParaList[3].usParaLen)
        {
            stPdpProfInfo.bitOpUsername = VOS_TRUE;

            PS_MEM_CPY((VOS_CHAR*)stPdpProfInfo.aucUserName,
                       (VOS_CHAR*)gastAtParaList[3].aucPara,
                       gastAtParaList[3].usParaLen);
            stPdpProfInfo.aucUserName[gastAtParaList[3].usParaLen] = 0;
        }

        /* 设置<passwd> */
        if ( gastAtParaList[4].usParaLen > TAF_MAX_AUTHDATA_PASSWORD_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if ( 0 != gastAtParaList[4].usParaLen )
        {
            stPdpProfInfo.bitOpPassword = VOS_TRUE;

            PS_MEM_CPY((VOS_CHAR*)stPdpProfInfo.aucPassWord,
                       (VOS_CHAR*)gastAtParaList[4].aucPara,
                       gastAtParaList[4].usParaLen);
            stPdpProfInfo.aucPassWord[gastAtParaList[4].usParaLen] = 0;
        }

        /* 设置<Auth_type> */
        if ( 0 != gastAtParaList[5].usParaLen )
        {
            stPdpProfInfo.bitOpAuthType = VOS_TRUE;
            stPdpProfInfo.ucAuthType        = (VOS_UINT8)gastAtParaList[5].ulParaValue;
        }

    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetPdpProfInfo( WUEPS_PID_AT,
                                          gastAtClientTab[ucIndex].usClientId,
                                          0, &stPdpProfInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PDPROFMOD_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
#endif



VOS_UINT32 AT_SetWifiGlobalMacPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           aucE5GwMacAddr[AT_MAC_ADDR_LEN+1]; /* MAC地址*/

    if (gucAtParaIndex != 1)
    {
        AT_NORM_LOG("AT_SetApMacPara: the number of parameters is error.");
        return AT_ERROR;
    }

    if (gastAtParaList[0].usParaLen > AT_MAC_ADDR_LEN )
    {
        return AT_ERROR;
    }

    PS_MEM_SET(aucE5GwMacAddr, 0x00, sizeof(aucE5GwMacAddr));

    /* 写MAC地址参数到NV */
    VOS_StrNCpy( (VOS_CHAR *)aucE5GwMacAddr, (VOS_CHAR *)gastAtParaList[0].aucPara,
                gastAtParaList[0].usParaLen );


    ulRet = NV_WriteEx(MODEM_ID_0, en_NV_Item_WIFI_MAC_ADDR, aucE5GwMacAddr, AT_MAC_ADDR_LEN);

    if (NV_OK != ulRet)
    {
        AT_WARN_LOG("AT_SetApMacPara: Fail to write nv.");
        return AT_ERROR;
    }

    return AT_OK;
}


VOS_UINT32 AT_SetDockPara (VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucClientIndex;
    VOS_UINT16                          usLength;

    usLength = 0;

    if (AT_APP_USER != gastAtClientTab[ucIndex].UserType )
    {
        return AT_FAILURE;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FAILURE;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_FAILURE;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_FAILURE;
    }

    VOS_MemSet(gastAtParaList[0].aucPara + gastAtParaList[0].usParaLen,
               0x00,
               (VOS_SIZE_T)(AT_PARA_MAX_LEN - gastAtParaList[0].usParaLen));

    for (ucClientIndex = 0 ; ucClientIndex < AT_MAX_CLIENT_NUM; ucClientIndex++)
    {
        if (AT_USBCOM_USER == gastAtClientTab[ucClientIndex].UserType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s%s%s",
                                        gaucAtCrLf,
                                        gastAtParaList[0].aucPara,
                                        gaucAtCrLf);
            At_SendResultData(ucClientIndex, pgucAtSndCodeAddr, usLength);
            break;
        }
    }

    return AT_SUCCESS;
}
VOS_UINT32 AT_SetCsqlvlPara( VOS_UINT8 ucIndex )
{
    /* 发消息到C核获取 信号格数 */
    if(TAF_SUCCESS != Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId, 0,
                                   TAF_PH_CSQLVL_VALUE_PARA, VOS_NULL_PTR))
    {
        AT_WARN_LOG("AT_SetCsqlvlPara: Taf_ParaQuery fail.");
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSQLVL_SET;
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32  AT_SetCsqlvlExtPara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_CSQLVLEXT_VALUE_PARA,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSQLVLEXT_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetApbatlvlPara( VOS_UINT8 ucIndex )
{
    BATT_STATE_T                        stBatLvl;
    VOS_UINT32                          ucChargeState;

    if (BSP_MODULE_UNSUPPORT == DRV_GET_BATTERY_SUPPORT() )
    {
        return AT_ERROR;
    }

    /*Arguments found, return error.*/
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        vos_printf("AT^APBATLVL arguments found, return error!!\r\n ");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 调用接口获取充电情况 */
    if ( VOS_OK != DRV_CHG_GET_BATTERY_STATE( &stBatLvl ) )
    {
        return AT_ERROR;
    }

    /* 区分充电和未充电状态 */
    if ( (stBatLvl.charging_state == CHARGING_UP)
      || (stBatLvl.charging_state == CHARGING_DOWN) )
    {
        ucChargeState = 1;
    }
    else if ( (stBatLvl.charging_state == NO_CHARGING_UP)
           || (stBatLvl.charging_state == NO_CHARGING_DOWN) )
    {
        ucChargeState = 0;
    }
    else
    {
        return AT_ERROR;
    }

    /* 过滤电池电量错误值 */
    if ( (stBatLvl.battery_level < BATT_LOW_POWER)
       ||(stBatLvl.battery_level >= BATT_LEVEL_MAX))
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR*)pgucAtSndCodeAddr,
                                                    (TAF_CHAR*)pgucAtSndCodeAddr,
                                                    "%s:%d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    ucChargeState,
                                                    stBatLvl.battery_level);

    return AT_OK;
}


VOS_UINT32 AT_SetOpenportPara( VOS_UINT8 ucIndex )
{
    /* 能在这里处理OPENPORT, 要么是已经有权限了再输入OPENPORT,直接返回OK
       要么是没开启密码保护, 也直接返回OK */
    return AT_OK;
}


VOS_UINT32  At_CheckNdisIpv4AddrPara(
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen)
{
    VOS_UINT32                          i;
    VOS_UINT32                          usTemp;
    VOS_UINT8                           ucDotNum;
    VOS_UINT8                           ucNNUM;

    usTemp      = 0;
    ucDotNum    = 0;
    ucNNUM      = 0;

    for (i = 0; i < usLen; i++)
    {
        if ('.' == pData[i])
        {
            if ((usTemp > 255) || (ucNNUM > 3) || (ucNNUM == 0))
            {
                return  AT_ERROR;
            }
            usTemp = 0;
            ucNNUM = 0;
            ucDotNum++;
        }
        else if ( (pData[i] >= '0') && (pData[i] <= '9') )
        {
            ucNNUM++;
            usTemp = (usTemp*10) + (pData[i]-'0');
        }
        else
        {
            return  AT_ERROR;
        }
    }

    if ((ucDotNum != 3) || (usTemp > 255) || (ucNNUM > 3) || (ucNNUM == 0))
    {
        return AT_ERROR;
    }

    return  AT_OK;
}


VOS_UINT32  At_NdisIpv4AddParaProc(
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen,
    VOS_UINT8                          *pDesData,
    VOS_UINT32                         *pulFlag
)
{
    if (0 == usLen)
    {
        AT_INFO_LOG("At_NdisIpv4AddParaProc: Address isn't present.");
        *pulFlag = VOS_FALSE;
        return AT_SUCCESS;
    }
    else
    {
         /*检查参数是否正确 */
        if(AT_ERROR == At_CheckNdisIpv4AddrPara(pData,usLen))
        {
            AT_WARN_LOG("At_NdisIpv4AddParaProc:the para is error.");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (usLen > AT_NDIS_ADDR_PARA_LEN)
        {
            AT_WARN_LOG("At_NdisIpv4AddParaProc: length of IP_Address is beyond 15.");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        PS_MEM_SET(pDesData, 0, (AT_NDIS_ADDR_PARA_LEN + 1));
        PS_MEM_CPY(pDesData, pData, usLen);
    }

    *pulFlag = VOS_TRUE;

    return AT_SUCCESS;
}
VOS_UINT32  At_SetDnsPrim(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    AT_DIAL_PARAM_STRU                 *pstAppDialPara;

    pstAppDialPara                      = AT_APP_GetDailParaAddr();

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    if (1 != gucAtParaIndex)
    {
        AT_WARN_LOG("At_SetDnsPrim: too many parameters.");
        return AT_ERROR;
    }

    if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
    {
        /* 解析IPV4<Primary_DNS>参数 */
        ulRst = At_NdisIpv4AddParaProc(gastAtParaList[0].aucPara,
                                       gastAtParaList[0].usParaLen,
                                       pstAppDialPara->aucPrimIPv4DNSAddr,
                                       &pstAppDialPara->ulPrimIPv4DNSValidFlag);
        if (AT_SUCCESS != ulRst)
        {
            pstAppDialPara->ulPrimIPv4DNSValidFlag  = VOS_FALSE;
            PS_MEM_SET(pstAppDialPara->aucPrimIPv4DNSAddr, 0x00, TAF_MAX_IPV4_ADDR_STR_LEN);
            AT_INFO_LOG("At_SetDnsPrim: IPV4 Primary_DNS isn't present.");
            return ulRst;
        }

        return AT_OK;
    }
    /* 解析IPV4<Primary_DNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[0].aucPara,
                                   gastAtParaList[0].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv4DNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv4DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        gstAtNdisAddParam.ulPrimIPv4DNSValidFlag = 0;
        PS_MEM_SET(gstAtNdisAddParam.aucPrimIPv4DNSAddr, 0, (AT_NDIS_ADDR_PARA_LEN + 1));
        AT_INFO_LOG("At_SetDnsPrim: IPV4 Primary_DNS isn't present.");
        return ulRst;
    }

    return AT_OK;
}


VOS_UINT32  At_SetDnsSnd(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    AT_DIAL_PARAM_STRU                 *pstAppDialPara;

    pstAppDialPara                      = AT_APP_GetDailParaAddr();

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    if (1 != gucAtParaIndex)
    {
        AT_WARN_LOG("At_SetDnsSnd: too many parameters.");
        return AT_ERROR;
    }

    if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
    {
        /* 解析IPV4<Secondary_DNS>参数 */
        ulRst = At_NdisIpv4AddParaProc(gastAtParaList[0].aucPara,
                                       gastAtParaList[0].usParaLen,
                                       pstAppDialPara->aucSndIPv4DNSAddr,
                                       &pstAppDialPara->ulSndIPv4DNSValidFlag);
        if (AT_SUCCESS != ulRst)
        {
            pstAppDialPara->ulSndIPv4DNSValidFlag   = VOS_FALSE;
            PS_MEM_SET(pstAppDialPara->aucSndIPv4DNSAddr, 0x00, TAF_MAX_IPV4_ADDR_STR_LEN);
            AT_INFO_LOG("At_SetDnsSnd: IPV4 Secondary_DNS isn't present.");
            return ulRst;
        }

        return AT_OK;
    }

    /* 解析IPV4<Secondary_DNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[0].aucPara,
                               gastAtParaList[0].usParaLen,
                               gstAtNdisAddParam.aucSndIPv4DNSAddr,
                              &gstAtNdisAddParam.ulSndIPv4DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        gstAtNdisAddParam.ulSndIPv4DNSValidFlag = 0;
        PS_MEM_SET(gstAtNdisAddParam.aucSndIPv4DNSAddr, 0, (AT_NDIS_ADDR_PARA_LEN + 1));
        AT_INFO_LOG("At_SetDnsSnd: IPV4 Secondary_DNS isn't present.");
        return ulRst;
    }

    return AT_OK;
}


#if (FEATURE_ON == FEATURE_IPV6)
VOS_UINT32 AT_CheckIpv6Capability(
    VOS_UINT8                           ucPdpType
)
{
    switch (AT_GetIpv6Capability())
    {
        case AT_IPV6_CAPABILITY_IPV4_ONLY:
            /* IPv4 only只能发起IPv4的PDP激活 */
            if (TAF_PDP_IPV4 != ucPdpType)
            {
                AT_ERR_LOG("AT_CheckIpv6Capablity: Only IPv4 is supported!");
                return VOS_ERR;
            }
            break;

        case AT_IPV6_CAPABILITY_IPV6_ONLY:
            /* IPv6 only只能发起IPv6的PDP激活 */
            if (TAF_PDP_IPV6 != ucPdpType)
            {
                AT_ERR_LOG("AT_CheckIpv6Capablity: Only IPv6 is supported!");
                return VOS_ERR;
            }
            break;

        default:
            break;
    }

    return VOS_OK;
}
#endif

#if (FEATURE_ON == FEATURE_IPV6)

VOS_UINT32 AT_CheckNdisIpv6AddrPara(
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          usTemp;
    VOS_UINT8                           ucDotNum;
    VOS_UINT8                           ucNNUM;

    usTemp                              = 0;
    ucDotNum                            = 0;
    ucNNUM                              = 0;

    for (i = 0; i < usLen; i++)
    {
        if (':' == pData[i])
        {
            if ((usTemp > 65535) || (ucNNUM > 5) || (ucNNUM == 0))
            {
                return  AT_ERROR;
            }

            usTemp = 0;
            ucNNUM = 0;
            ucDotNum++;
        }
        else if ( (pData[i] >= '0') && (pData[i] <= '9') )
        {
            ucNNUM++;
            usTemp = (usTemp*10) + (pData[i]-'0');
        }
        else
        {
            return  AT_ERROR;
        }
    }

    if ( ((ucDotNum != 7) && (ucDotNum != 2 ))
      || (usTemp > 65535)
      || (ucNNUM > 5)
      || (ucNNUM == 0))
    {
        return AT_ERROR;
    }

    return  AT_OK;
}


VOS_UINT32 AT_NdisIpv6AddParaProc(
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen,
    VOS_UINT8                          *pDesData,
    VOS_UINT32                         *pulFlag
)
{
    if (0 == usLen)
    {
        AT_INFO_LOG("AT_NdisIpv6AddParaProc: Address isn't present.");
        *pulFlag = VOS_FALSE;
        return AT_SUCCESS;
    }
    else
    {
         /*检查参数是否正确 */
        if(AT_ERROR == AT_CheckNdisIpv6AddrPara(pData,usLen))
        {
            AT_WARN_LOG("AT_NdisIpv6AddParaProc:the para is error.");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (usLen > AT_NDIS_IPV6_ADDR_PARA_LEN)
        {
            AT_WARN_LOG("AT_NdisIpv6AddParaProc: length of IP_Address is beyond 63.");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        PS_MEM_SET(pDesData, 0, (AT_NDIS_IPV6_ADDR_PARA_LEN + 1));
        PS_MEM_CPY(pDesData, pData, usLen);
    }

    *pulFlag = VOS_TRUE;

    return AT_SUCCESS;
}
VOS_UINT32 AT_SetNdisIpv6AddPara(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulRst;

    /* 解析<Primary_DNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[1].aucPara,
                                   gastAtParaList[1].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv6DNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv6DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisIpv6AddPara: Primary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Secondary_DNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[2].aucPara,
                                   gastAtParaList[2].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv6DNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv6DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisIpv6AddPara: Secondary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Primary_NBNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[3].aucPara,
                                   gastAtParaList[3].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv6WINNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv6WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisIpv6AddPara: Primary_NBNS isn't present.");
        return ulRst;
    }

    /* 解析<Secondary_NBNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[4].aucPara,
                                   gastAtParaList[4].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv6WINNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv6WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisIpv6AddPara: Secondary_NBNS isn't present.");
        return ulRst;
    }

    return AT_SUCCESS;
}
#endif


VOS_UINT32  AT_SetNdisIpv4v6AddPara(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulRst;

    /* 解析<IP_Address>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[1].aucPara,
                                   gastAtParaList[1].usParaLen,
                                   gstAtNdisAddParam.aucIPv4Addr,
                                   &gstAtNdisAddParam.ulIPv4ValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: IP_Address isn't present.");
        return ulRst;
    }


    /* 解析<Primary_DNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[2].aucPara,
                                   gastAtParaList[2].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv4DNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv4DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Primary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Secondary_DNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[3].aucPara,
                                   gastAtParaList[3].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv4DNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv4DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Secondary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Primary_NBNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[4].aucPara,
                                   gastAtParaList[4].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv4WINNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv4WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Primary_NBNS isn't present.");
        return ulRst;
    }

    /* 解析<Secondary_NBNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[5].aucPara,
                                   gastAtParaList[5].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv4WINNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv4WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Secondary_NBNS isn't present.");
        return ulRst;
    }

#if(FEATURE_ON == FEATURE_IPV6)
    /* 解析<Primary_DNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[6].aucPara,
                                   gastAtParaList[6].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv6DNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv6DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Primary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Secondary_DNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[7].aucPara,
                                   gastAtParaList[7].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv6DNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv6DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Secondary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Primary_NBNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[8].aucPara,
                                   gastAtParaList[8].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv6WINNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv6WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Primary_NBNS isn't present.");
        return ulRst;
    }

    /* 解析<Secondary_NBNS>参数 */
    ulRst = AT_NdisIpv6AddParaProc(gastAtParaList[9].aucPara,
                                   gastAtParaList[9].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv6WINNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv6WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("AT_SetNdisIpv4v6AddPara: Secondary_NBNS isn't present.");
        return ulRst;
    }
#endif

    return AT_SUCCESS;
}
VOS_UINT32  At_SetNdisAddPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    if (gucAtParaIndex > 6)
    {
        AT_WARN_LOG("At_SetNdisAddPara: too many parameters.");
        return AT_TOO_MANY_PARA;
    }

    PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));

    /* 解析<IP_Type>参数 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        AT_WARN_LOG("At_SetNdisAddPara: IP_Type isn't present.");
        return AT_CME_INCORRECT_PARAMETERS;
    }
    else
    {
        /*0:IPV4   1:IPV6，2:IPV4V6*/
        if (0 == gastAtParaList[0].ulParaValue)
        {
            gstAtNdisAddParam.enPdpType = TAF_PDP_IPV4;
        }
        else if (1 == gastAtParaList[0].ulParaValue)
        {
            gstAtNdisAddParam.enPdpType = TAF_PDP_IPV6;
        }
        else if (2 == gastAtParaList[0].ulParaValue )
        {
            gstAtNdisAddParam.enPdpType = TAF_PDP_IPV4V6;
        }
        else
        {
            AT_WARN_LOG("At_SetNdisAddPara: IP_Type isn't valid.");
            return AT_ERROR;
        }
    }

    gstAtNdisAddParam.ucPdpTypeValidFlag = VOS_TRUE;

    /* 解析<IP_Address>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[1].aucPara,
                                   gastAtParaList[1].usParaLen,
                                   gstAtNdisAddParam.aucIPv4Addr,
                                   &gstAtNdisAddParam.ulIPv4ValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisAddPara: IP_Address isn't present.");
        return ulRst;
    }


    /* 解析<Primary_DNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[2].aucPara,
                                   gastAtParaList[2].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv4DNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv4DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisAddPara: Primary_DNS isn't present.");
        return ulRst;
    }


    /* 解析<Secondary_DNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[3].aucPara,
                                   gastAtParaList[3].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv4DNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv4DNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_WARN_LOG("At_NdisConnProc: length of password is beyond 255.");
        return AT_CME_INCORRECT_PARAMETERS;
    }


    /* 解析<Primary_NBNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[4].aucPara,
                                   gastAtParaList[4].usParaLen,
                                   gstAtNdisAddParam.aucPrimIPv4WINNSAddr,
                                   &gstAtNdisAddParam.ulPrimIPv4WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisAddPara: Primary_NBNS isn't present.");
        return ulRst;
    }

    /* 解析<Secondary_NBNS>参数 */
    ulRst = At_NdisIpv4AddParaProc(gastAtParaList[5].aucPara,
                                   gastAtParaList[5].usParaLen,
                                   gstAtNdisAddParam.aucSndIPv4WINNSAddr,
                                   &gstAtNdisAddParam.ulSndIPv4WINNSValidFlag);
    if (AT_SUCCESS != ulRst)
    {
        PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));
        AT_INFO_LOG("At_SetNdisAddPara: Secondary_NBNS isn't present.");
        return ulRst;
    }

    return AT_OK;
}
VOS_UINT32 AT_NidsCheckDialParam(VOS_VOID)
{
    if (gucAtParaIndex > 6)
    {
        AT_NORM_LOG1("At_NdisDupProc:gucAtParaIndex=%d.\n",gucAtParaIndex);
        return AT_TOO_MANY_PARA;
    }

    /* 检查CID,CID必须输入*/
    if (0 == gastAtParaList[0].usParaLen)
    {
        AT_INFO_LOG("At_NdisDupProc:cid not ext");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* CID为1~31*/
    if ( (gastAtParaList[0].ulParaValue > TAF_MAX_CID)
      || (gastAtParaList[0].ulParaValue < 1) )
    {
        AT_NORM_LOG1("At_NdisDupProc:gastAtParaList[0].ulParaValue=%d\n",gastAtParaList[0].ulParaValue);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*拨号参数检查，该参数不能省略，且只能为0或1，1表示拨号、0表示断开拨号*/
    if (0 == gastAtParaList[1].usParaLen)
    {
        AT_INFO_LOG("At_NdisDupProc:dial unknow");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*AT^NDISDUP=1,1*/
    if ( (1 != gastAtParaList[1].ulParaValue)
      && (0 != gastAtParaList[1].ulParaValue) )
    {
        AT_INFO_LOG("At_NdisDupProc:dial unknow");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* APN */
    if ( gastAtParaList[2].usParaLen  > TAF_MAX_APN_LEN )
    {
        AT_WARN_LOG("AT_CheckApn: WARNING: APN len is error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 != gastAtParaList[2].usParaLen )
    {
        /* 检查 */
        if ( VOS_OK != AT_CheckApnFormat(gastAtParaList[2].aucPara,
                                         gastAtParaList[2].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

     /* User Name */
    if ( gastAtParaList[3].usParaLen > TAF_MAX_GW_AUTH_USERNAME_LEN )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

     /* Password */
    if ( gastAtParaList[4].usParaLen > TAF_MAX_GW_AUTH_PASSWORD_LEN )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}
VOS_UINT8 AT_NdisFindCidForDialDown(VOS_VOID)
{
    /* IPv4v6 */
    if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4v6State)
      || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4v6State) )
    {
        return g_stAtNdisDhcpPara.ucIpv4v6Cid;
    }
    else
    {
        /* IPv4 */
        if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4State) )
        {
            return g_stAtNdisDhcpPara.ucIpv4Cid;
        }

        /* IPv6 */
        if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv6State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv6State) )
        {
            return g_stAtNdisDhcpPara.ucIpv6Cid;
        }
    }

    return TAF_INVALID_CID;
}
VOS_UINT32 AT_GetNdisDialParam(
    TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo,
    VOS_UINT8                           ucIndex
)
{
    TAF_PDP_PRIM_CONTEXT_STRU                    stPdpCidInfo;
    VOS_UINT32                          ulRet;


    PS_MEM_SET(&stPdpCidInfo, 0x00, sizeof(stPdpCidInfo));


    pstDialParaInfo->ucCid      = (VOS_UINT8)gastAtParaList[0].ulParaValue;

#ifdef DMT
    ulRet = MN_APS_GetPdpCidPara(&stPdpCidInfo, pstDialParaInfo->ucCid);
#else
    ulRet = TAF_AGENT_GetPdpCidPara(&stPdpCidInfo, ucIndex, pstDialParaInfo->ucCid);
#endif
    if (VOS_OK != ulRet)
    {
        return AT_FAILURE;
    }
    /* 获取PDP类型 */
    if (VOS_TRUE == gstAtNdisAddParam.ucPdpTypeValidFlag)
    {
        pstDialParaInfo->enPdpType      = gstAtNdisAddParam.enPdpType;
    }
    else if ((TAF_PDP_IPV4   == stPdpCidInfo.stPdpAddr.enPdpType)
     || (TAF_PDP_IPV6   == stPdpCidInfo.stPdpAddr.enPdpType)
     || (TAF_PDP_IPV4V6 == stPdpCidInfo.stPdpAddr.enPdpType))
    {
#if (FEATURE_OFF == FEATURE_IPV6)
        if ((TAF_PDP_IPV6   == stPdpCidInfo.stPdpAddr.enPdpType)
        || (TAF_PDP_IPV4V6 == stPdpCidInfo.stPdpAddr.enPdpType))
        {
            return AT_FAILURE;
        }
#endif
        pstDialParaInfo->enPdpType      = stPdpCidInfo.stPdpAddr.enPdpType;
    }
    else
    {
        /* 默认使用IPV4 */
        pstDialParaInfo->enPdpType      = TAF_PDP_IPV4;
    }

#if(FEATURE_ON == FEATURE_IPV6)
    if (VOS_ERR == AT_CheckIpv6Capability(pstDialParaInfo->enPdpType))
    {
        AT_INFO_LOG("AT_NidsGetDialParam:PDP type is error");
        return AT_FAILURE;
    }
#endif

    if ( 0 != gastAtParaList[2].usParaLen )
    {
        pstDialParaInfo->bitOpApn       = VOS_TRUE;

        PS_MEM_CPY((VOS_CHAR*)pstDialParaInfo->aucApn,
                   (VOS_CHAR*)gastAtParaList[2].aucPara,
                   gastAtParaList[2].usParaLen);
        pstDialParaInfo->aucApn[gastAtParaList[2].usParaLen] = 0;
    }

    if ( 0 != gastAtParaList[3].usParaLen )
    {
        pstDialParaInfo->bitOpUserName  = VOS_TRUE;

        PS_MEM_CPY((VOS_CHAR*)pstDialParaInfo->aucUserName,
                   (VOS_CHAR*)gastAtParaList[3].aucPara,
                   gastAtParaList[3].usParaLen);
        pstDialParaInfo->aucUserName[gastAtParaList[3].usParaLen] = 0;
    }

    if ( 0 != gastAtParaList[4].usParaLen )
    {
        pstDialParaInfo->bitOpPassWord  = VOS_TRUE;

        PS_MEM_CPY((VOS_CHAR*)pstDialParaInfo->aucPassWord,
                   (VOS_CHAR*)gastAtParaList[4].aucPara,
                   gastAtParaList[4].usParaLen);
        pstDialParaInfo->aucPassWord[gastAtParaList[4].usParaLen] = 0;
    }

    /* Auth Type */
    if ( gastAtParaList[5].usParaLen > 0 )
    {
        pstDialParaInfo->bitOpAuthType  = VOS_TRUE;
        pstDialParaInfo->enAuthType     = AT_CtrlGetPDPAuthType(gastAtParaList[5].ulParaValue,
                                                                gastAtParaList[5].usParaLen);
    }
    else
    {
        /* 如果用户名和密码长度均不为0, 且鉴权类型未设置, 则默认使用CHAP类型 */
        if ( (0 != gastAtParaList[3].usParaLen)
          && (0 != gastAtParaList[4].usParaLen) )
        {
            pstDialParaInfo->bitOpAuthType = VOS_TRUE;
            pstDialParaInfo->enAuthType    = PPP_CHAP_AUTH_TYPE;
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_NdisDialDownProc(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulPdpFlag;

    ulPdpFlag = 0;

    /* 如果所有的PDP都处于IDLE, 返回OK; 如果有一个处于DEACTING也返回OK */
    if ( VOS_TRUE == AT_NdisCheckPdpIdleState() )
    {
        AT_WARN_LOG("AT_NdisDialDownProc: the state is already IDLE.");
        return AT_OK;
    }

    if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4v6State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4v6State) )
    {
        /* AT^NDISDUP的第一个参数和当前的cid不一致，直接返回ERROR */
        if (g_stAtNdisDhcpPara.ucIpv4v6Cid != gastAtParaList[0].ulParaValue)
        {
            return AT_ERROR;
        }

        ulPdpFlag = 1;

        if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                     AT_NDIS_GET_CLIENT_ID(),
                                     0,
                                     g_stAtNdisDhcpPara.ucIpv4v6Cid))
        {
            AT_WARN_LOG("AT_NdisDialDownProc():TAF_PS_CallEnd fail");
        }

        AT_NdisSetState(TAF_PDP_IPV4V6, AT_PDP_STATE_DEACTING);
    }
    else
    {
        if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4State) )
        {
            /* AT^NDISDUP的第一个参数和当前的cid不一致，直接返回ERROR */
            if (g_stAtNdisDhcpPara.ucIpv4Cid == gastAtParaList[0].ulParaValue)
            {
                ulPdpFlag = 1;
                if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                             AT_NDIS_GET_CLIENT_ID(),
                                             0,
                                             g_stAtNdisDhcpPara.ucIpv4Cid))
                {
                    AT_WARN_LOG("AT_NdisDialDownProc():TAF_PS_CallEnd fail");
                }
                AT_NdisSetState(TAF_PDP_IPV4, AT_PDP_STATE_DEACTING);

                if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv6State)
                  || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv6State) )
                {

                    if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                                 AT_NDIS_GET_CLIENT_ID(),
                                                 0,
                                                 g_stAtNdisDhcpPara.ucIpv6Cid))
                    {
                        AT_WARN_LOG("AT_NdisDialDownProc():TAF_PS_CallEnd fail");
                    }
                    AT_NdisSetState(TAF_PDP_IPV6, AT_PDP_STATE_DEACTING);
                }
            }
        }

        if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv6State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv6State) )
        {
            /* AT^NDISDUP的第一个参数和当前的cid不一致，直接返回ERROR */
            if (g_stAtNdisDhcpPara.ucIpv6Cid == gastAtParaList[0].ulParaValue)
            {
                ulPdpFlag = 1;
                if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                             AT_NDIS_GET_CLIENT_ID(),
                                             0,
                                             g_stAtNdisDhcpPara.ucIpv6Cid))
                {
                    AT_WARN_LOG("AT_NdisDialDownProc():TAF_PS_CallEnd fail");
                }
                AT_NdisSetState(TAF_PDP_IPV6, AT_PDP_STATE_DEACTING);

                if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4State)
                  || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4State) )
                {

                    if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                                 AT_NDIS_GET_CLIENT_ID(),
                                                 0,
                                                 g_stAtNdisDhcpPara.ucIpv4Cid))
                    {
                        AT_WARN_LOG("AT_NdisDialDownProc():TAF_PS_CallEnd fail");
                    }
                    AT_NdisSetState(TAF_PDP_IPV4, AT_PDP_STATE_DEACTING);
                }

            }
        }
    }

    if (0 == ulPdpFlag)
    {
        return AT_ERROR;
    }


    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NDISCONN_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 AT_NdisCheckActingStateForDial(VOS_VOID )
{
    if ( (AT_PDP_STATE_IDLE == g_stAtNdisDhcpPara.enIpv4State )
      && (AT_PDP_STATE_IDLE == g_stAtNdisDhcpPara.enIpv6State )
      && (AT_PDP_STATE_IDLE == g_stAtNdisDhcpPara.enIpv4v6State ))
    {
        /* 发起PDP激活请求, 返回OK */
        return AT_OK;
    }
    else if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4State )
           || (AT_PDP_STATE_ACTED == g_stAtNdisDhcpPara.enIpv4State )
           || (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv6State )
           || (AT_PDP_STATE_ACTED == g_stAtNdisDhcpPara.enIpv6State )
           || (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4v6State )
           || (AT_PDP_STATE_ACTED == g_stAtNdisDhcpPara.enIpv4v6State ))
    {
        /* PDP已激活或正在激活, 返回OK */
        return AT_SUCCESS;
    }
    else
    {
        /* PDP状态不满足拨号要求, 返回ERROR */
        return  AT_FAILURE;
    }
}
VOS_UINT32 AT_NdisDialUpProc(VOS_UINT8 ucIndex)
{
    TAF_PS_DIAL_PARA_STRU               stDialParaInfo;
    AT_PS_USER_INFO_STRU                stUsrInfo;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    PS_MEM_SET(gstAtNdisAddParam.aucUsername, 0x00, TAF_MAX_GW_AUTH_USERNAME_LEN);
    PS_MEM_SET(gstAtNdisAddParam.aucPassword, 0x00, TAF_MAX_GW_AUTH_PASSWORD_LEN);
    PS_MEM_SET(&stDialParaInfo, 0x00, sizeof(TAF_PS_DIAL_PARA_STRU));
    PS_MEM_SET(&stUsrInfo, 0x00, sizeof(AT_PS_USER_INFO_STRU));

    /* 解析用户信息 */
    AT_NDIS_ParseUsrInfo(ucIndex, &stUsrInfo);

    /*如果已经拨号或正在拨号，直接回复OK*/
    ulResult = AT_NdisCheckActingStateForDial();
    if (AT_SUCCESS == ulResult)
    {
        AT_INFO_LOG("AT_NdisDialUpProc: the state is already active or activing");
        return AT_OK;
    }

    /*断开拨号过程中不允许拨号*/
    if (AT_FAILURE == ulResult)
    {
        AT_INFO_LOG("AT_NdisDialUpProc: the state is error");
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_UNKNOWN);
        return AT_ERROR;
    }

    /* 获取拨号参数 */
    ulResult = AT_GetNdisDialParam(&stDialParaInfo, ucIndex);
    if ( AT_SUCCESS != ulResult )
    {
        AT_ERR_LOG("AT_NdisDialUpProc: the PDP TYPE is not fit.");
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_UNKNOWN);
        return AT_ERROR;
    }

    /* 保存拨号参数，用于IPV6回退 */
    gstAtNdisAddParam.usAuthType      = stDialParaInfo.enAuthType;
    gstAtNdisAddParam.ucCid           = stDialParaInfo.ucCid;
    gstAtNdisAddParam.enPdpType       = stDialParaInfo.enPdpType;

    /* 保存用户名 */
    gstAtNdisAddParam.usUsernameLen   = gastAtParaList[3].usParaLen;
    PS_MEM_CPY((VOS_CHAR*)gstAtNdisAddParam.aucUsername,
               (VOS_CHAR*)gastAtParaList[3].aucPara,
               gastAtParaList[3].usParaLen);

    /* 保存用户密码 */
    gstAtNdisAddParam.usPasswordLen   = gastAtParaList[4].usParaLen;
    PS_MEM_CPY((VOS_CHAR*)gstAtNdisAddParam.aucPassword,
               (VOS_CHAR*)gastAtParaList[4].aucPara,
               gastAtParaList[4].usParaLen);

    /* APN */
    gstAtNdisAddParam.ucAPNLen        = (VOS_UINT8)gastAtParaList[2].usParaLen;
    PS_MEM_CPY((VOS_CHAR*)gstAtNdisAddParam.aucAPN,
               (VOS_CHAR*)gastAtParaList[2].aucPara,
               gastAtParaList[2].usParaLen);

    /* 发起拨号 */
    if (VOS_OK == TAF_PS_CallOrig(WUEPS_PID_AT,
                                  AT_NDIS_GET_CLIENT_ID(),
                                  0,
                                  &stDialParaInfo))
    {
        /* 保存用户信息 */
        g_stAtNdisDhcpPara.stUsrInfo = stUsrInfo;

        if (TAF_PDP_IPV4 == stDialParaInfo.enPdpType)
        {
            g_stAtNdisDhcpPara.ucIpv4Cid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        }
        else if (TAF_PDP_IPV6 == stDialParaInfo.enPdpType)
        {
            g_stAtNdisDhcpPara.ucIpv6Cid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        }
        else
        {
            g_stAtNdisDhcpPara.ucIpv4v6Cid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        }

        g_enAtNdisActPdpType       = stDialParaInfo.enPdpType;
        g_enAtFirstNdisActPdpType  = stDialParaInfo.enPdpType;

        AT_NdisSetState(g_enAtNdisActPdpType, AT_PDP_STATE_ACTING);

        /* 设置当前pdpact操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NDISCONN_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
VOS_UINT32 At_NdisDupProc(VOS_UINT8 ucIndex)
{
    VOS_UINT32                  ulRet;

    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        AT_NORM_LOG1("At_NdisDupProc:g_stATParseCmd.ucCmdOptType=%d\n",g_stATParseCmd.ucCmdOptType);

        /* 记录PS域呼叫错误码 */
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_INVALID_PARAMETER);

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*参数检查*/
    ulRet = AT_NidsCheckDialParam();
    if ( AT_SUCCESS != ulRet)
    {
        AT_NORM_LOG1("At_NdisDupProc:ulRet=%d\n",ulRet);

        /* 记录PS域呼叫错误码 */
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_INVALID_PARAMETER);

        return ulRet;
    }

    /* 处理拨号，拨号或断开拨号 */
    if (0 == gastAtParaList[1].ulParaValue)
    {
        return AT_NdisDialDownProc(ucIndex);
    }
    else
    {
        /* 记录NDIS拨号参数 */
        AT_SaveNdisConnDailPara(gastAtParaList);

        return AT_NdisDialUpProc(ucIndex);
    }

}


VOS_UINT32 AT_AppCheckDialParam(VOS_VOID)
{
    VOS_UINT8                           aucIpv4Addr[TAF_IPV4_ADDR_LEN];

    if (gucAtParaIndex > 7)
    {
        AT_NORM_LOG1("AT_AppCheckDialParam:gucAtParaIndex=%d.\n",gucAtParaIndex);
        return AT_TOO_MANY_PARA;
    }

    /* 检查CID,CID必须输入*/
    if (0 == gastAtParaList[0].usParaLen)
    {
        AT_INFO_LOG("AT_AppCheckDialParam:cid not ext");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* CID为1~31*/
    if ( (gastAtParaList[0].ulParaValue > TAF_MAX_CID)
      || (gastAtParaList[0].ulParaValue < 1) )
    {
        AT_NORM_LOG1("AT_AppCheckDialParam:gastAtParaList[0].ulParaValue=%d\n",gastAtParaList[0].ulParaValue);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 拨号参数检查，该参数不能省略，且只能为0或1，1表示拨号、0表示断开拨号 */
    if (0 == gastAtParaList[1].usParaLen)
    {
        AT_INFO_LOG("AT_AppCheckDialParam:dial unknow");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^NDISDUP=1,1 */
    if ( (1 != gastAtParaList[1].ulParaValue)
      && (0 != gastAtParaList[1].ulParaValue) )
    {
        AT_INFO_LOG("AT_AppCheckDialParam:dial unknow");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* APN */
    if ( gastAtParaList[2].usParaLen  > TAF_MAX_APN_LEN )
    {
        AT_WARN_LOG("AT_CheckApn: WARNING: APN len is error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 != gastAtParaList[2].usParaLen )
    {
        /* 检查 */
        if ( VOS_OK != AT_CheckApnFormat(gastAtParaList[2].aucPara,
                                         gastAtParaList[2].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /* User Name */
    if ( gastAtParaList[3].usParaLen > TAF_MAX_GW_AUTH_USERNAME_LEN )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

     /* Password */
    if ( gastAtParaList[4].usParaLen > TAF_MAX_GW_AUTH_PASSWORD_LEN )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*IP地址不能大于 15*/
    if (gastAtParaList[6].usParaLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    else
    {
        if (gastAtParaList[6].usParaLen > 0)
        {
            if (VOS_OK != AT_Ipv4AddrAtoi((VOS_CHAR *)gastAtParaList[6].aucPara, aucIpv4Addr))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }
    }


    return AT_SUCCESS;
}


VOS_UINT32 AT_AppCheckPdpStateForDial(VOS_VOID )
{
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity;

    pstAppPdpEntity                     = AT_APP_GetPdpEntInfoAddr();

    if ( (AT_PDP_STATE_IDLE == pstAppPdpEntity->enIpv4State )
      && (AT_PDP_STATE_IDLE == pstAppPdpEntity->enIpv6State )
      && (AT_PDP_STATE_IDLE == pstAppPdpEntity->enIpv4v6State ))
    {
        /* 发起PDP激活请求, 返回OK */
        return AT_OK;
    }
    else if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4State)
           || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4State)
           || (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv6State)
           || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv6State)
           || (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4v6State)
           || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4v6State) )
    {
        /* PDP已激活或正在激活, 返回OK */
        return AT_SUCCESS;
    }
    else
    {
        /* PDP状态不满足拨号要求, 返回ERROR */
        return  AT_FAILURE;
    }
}
VOS_UINT8 AT_AppFindCidForDialDown(VOS_VOID)
{
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity;

    pstAppPdpEntity                     = AT_APP_GetPdpEntInfoAddr();

    /* IPv4v6 */
    if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4v6State)
      || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4v6State) )
    {
        return pstAppPdpEntity->ucIpv4v6Cid;
    }
    else
    {
        /* IPv4 */
        if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4State)
          || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4State) )
        {
            return pstAppPdpEntity->ucIpv4Cid;
        }

        /* IPv6 */
        if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv6State)
          || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv6State) )
        {
            return pstAppPdpEntity->ucIpv6Cid;
        }
    }

    return TAF_INVALID_CID;
}
VOS_VOID AT_AppReportPdpActiveState(
    VOS_UINT8                           ucIndex,
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity
)
{
    VOS_UINT16                          usLength;
    VOS_UINT8                           aucAtStrIpv4[] = "IPV4";
    VOS_UINT8                           aucAtStrIpv6[] = "IPV6";
    AT_IPV6_RA_INFO_STRU               *pstAppRaInfoAddr;

    pstAppRaInfoAddr = AT_APP_GetRaInfoAddr();

    usLength                            = 0;

    /* 如果拨号连接已存在, 上报当前的连接状态 */
    if (AT_PDP_STATE_ACTED == pstAppPdpEntity->enIpv4v6State)
    {
        AT_NORM_LOG("AT_AppReportPdpActiveState:NORMAL: The state of IPv4v6 is ACTED!");

        /* 先上报OK */
        At_FormatResultData(ucIndex, AT_OK);

        /* 再上报^DCONN */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                    "%s^DCONN:%d,\"%s\"%s",
                                    gaucAtCrLf,
                                    pstAppPdpEntity->ucIpv4v6Cid,
                                    aucAtStrIpv4,
                                    gaucAtCrLf);
        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        if (VOS_TRUE == pstAppRaInfoAddr->bitOpPrefixAddr)
        {
            usLength = 0;

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                    "%s^DCONN:%d,\"%s\"%s",
                                    gaucAtCrLf,
                                    pstAppPdpEntity->ucIpv4v6Cid,
                                    aucAtStrIpv6,
                                    gaucAtCrLf);

            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
        }
    }
    else if ( (AT_PDP_STATE_ACTED == pstAppPdpEntity->enIpv4State)
           && (AT_PDP_STATE_ACTED == pstAppPdpEntity->enIpv6State) )
    {
        AT_NORM_LOG("AT_AppDialUpProc:NORMAL: The state of IPv4 and IPv6 is ACTED!");

        /* 先上报OK */
        At_FormatResultData(ucIndex, AT_OK);

        /* 再上报^DCONN */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                    "%s^DCONN:%d,\"%s\"%s",
                                    gaucAtCrLf,
                                    pstAppPdpEntity->ucIpv4Cid,
                                    aucAtStrIpv4,
                                    gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        if (VOS_TRUE == pstAppRaInfoAddr->bitOpPrefixAddr)
        {
            usLength = 0;

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                    "%s^DCONN:%d,\"%s\"%s",
                                    gaucAtCrLf,
                                    pstAppPdpEntity->ucIpv6Cid,
                                    aucAtStrIpv6,
                                    gaucAtCrLf);

            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
        }
    }
    else if ( (AT_PDP_STATE_ACTED == pstAppPdpEntity->enIpv4State)
           && (AT_PDP_STATE_IDLE  == pstAppPdpEntity->enIpv6State) )
    {
        AT_NORM_LOG("AT_AppDialUpProc:NORMAL: The state of IPv4 is ACTED!");

        /* 先上报OK */
        At_FormatResultData(ucIndex, AT_OK);

        /* 再上报^DCONN */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                    "%s^DCONN:%d,\"%s\"%s",
                                    gaucAtCrLf,
                                    pstAppPdpEntity->ucIpv4Cid,
                                    aucAtStrIpv4,
                                    gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }
    else if ( (AT_PDP_STATE_IDLE  == pstAppPdpEntity->enIpv4State)
           && (AT_PDP_STATE_ACTED == pstAppPdpEntity->enIpv6State) )
    {
        AT_NORM_LOG("AT_AppDialUpProc:NORMAL: The state of IPv6 is ACTED!");

        /* 先上报OK */
        At_FormatResultData(ucIndex, AT_OK);

        if (VOS_TRUE == pstAppRaInfoAddr->bitOpPrefixAddr)
        {
            /* 再上报^DCONN */
            usLength = 0;

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                    "%s^DCONN:%d,\"%s\"%s",
                                    gaucAtCrLf,
                                    pstAppPdpEntity->ucIpv6Cid,
                                    aucAtStrIpv6,
                                    gaucAtCrLf);

            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
        }
    }
    else
    {
        /* 无拨号连接, 直接返回OK */
        AT_NORM_LOG("AT_AppDialUpProc:NORMAL: No PDP is ACTED!");

        /* 先上报OK */
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}
VOS_UINT32 AT_GetAppDialParam(
    TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo,
    VOS_UINT8                           ucIndex
)
{
    TAF_PDP_PRIM_CONTEXT_STRU                    stPdpCidInfo;
    VOS_UINT32                          ulRet;

    PS_MEM_SET(&stPdpCidInfo, 0x00, sizeof(TAF_PDP_PRIM_CONTEXT_STRU));
    PS_MEM_SET(pstDialParaInfo, 0x00, sizeof(TAF_PS_DIAL_PARA_STRU));

    /* 获取CID */
    pstDialParaInfo->ucCid            = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    ulRet = TAF_AGENT_GetPdpCidPara(&stPdpCidInfo, ucIndex, pstDialParaInfo->ucCid);
    if (VOS_OK != ulRet)
    {
        return AT_FAILURE;
    }

    if ((TAF_PDP_IPV4   == stPdpCidInfo.stPdpAddr.enPdpType)
     || (TAF_PDP_IPV6   == stPdpCidInfo.stPdpAddr.enPdpType)
     || (TAF_PDP_IPV4V6 == stPdpCidInfo.stPdpAddr.enPdpType))
    {
#if (FEATURE_OFF == FEATURE_IPV6)
        if ((TAF_PDP_IPV6   == stPdpCidInfo.stPdpAddr.enPdpType)
        || (TAF_PDP_IPV4V6 == stPdpCidInfo.stPdpAddr.enPdpType))
        {
            return AT_FAILURE;
        }
#endif
        pstDialParaInfo->enPdpType      = stPdpCidInfo.stPdpAddr.enPdpType;
    }
    else
    {
        /* 默认使用IPV4 */
        pstDialParaInfo->enPdpType      = TAF_PDP_IPV4;
    }

#if(FEATURE_ON == FEATURE_IPV6)
    if (VOS_ERR == AT_CheckIpv6Capability(pstDialParaInfo->enPdpType))
    {
        AT_INFO_LOG("AT_NidsGetDialParam:PDP type is error");
        return AT_FAILURE;
    }
#endif

    if ( 0 != gastAtParaList[2].usParaLen )
    {
        pstDialParaInfo->bitOpApn       = VOS_TRUE;

        PS_MEM_CPY((VOS_CHAR*)pstDialParaInfo->aucApn,
                   (VOS_CHAR*)gastAtParaList[2].aucPara,
                   gastAtParaList[2].usParaLen);
        pstDialParaInfo->aucApn[gastAtParaList[2].usParaLen] = 0;
    }

    if ( 0 != gastAtParaList[3].usParaLen )
    {
        pstDialParaInfo->bitOpUserName  = VOS_TRUE;

        PS_MEM_CPY((VOS_CHAR*)pstDialParaInfo->aucUserName,
                   (VOS_CHAR*)gastAtParaList[3].aucPara,
                   gastAtParaList[3].usParaLen);
        pstDialParaInfo->aucUserName[gastAtParaList[3].usParaLen] = 0;
    }

    if ( 0 != gastAtParaList[4].usParaLen )
    {
        pstDialParaInfo->bitOpPassWord  = VOS_TRUE;

        PS_MEM_CPY((VOS_CHAR*)pstDialParaInfo->aucPassWord,
                   (VOS_CHAR*)gastAtParaList[4].aucPara,
                   gastAtParaList[4].usParaLen);
        pstDialParaInfo->aucPassWord[gastAtParaList[4].usParaLen] = 0;
    }

    /* Auth Type */
    if ( gastAtParaList[5].usParaLen > 0 )
    {
        pstDialParaInfo->bitOpAuthType  = VOS_TRUE;
        pstDialParaInfo->enAuthType     = AT_CtrlGetPDPAuthType(gastAtParaList[5].ulParaValue,
                                                                gastAtParaList[5].usParaLen);
    }
    else
    {
        /* 如果用户名和密码长度均不为0, 且鉴权类型未设置, 则默认使用CHAP类型 */
        if ( (0 != gastAtParaList[3].usParaLen)
          && (0 != gastAtParaList[4].usParaLen) )
        {
            pstDialParaInfo->bitOpAuthType = VOS_TRUE;
            pstDialParaInfo->enAuthType    = PPP_CHAP_AUTH_TYPE;
        }
    }

    /* 设置<PDP_addr> */
    if ( gastAtParaList[6].usParaLen > 0 )
    {
        /* 把IP地址字符串转换成数字, IP地址格式已经统一在前面检查 */
        if ( VOS_OK == AT_Ipv4AddrAtoi((VOS_CHAR*)gastAtParaList[6].aucPara,
                                       (VOS_UINT8*)pstDialParaInfo->stPdpAddr.aucIpv4Addr) )
        {
            pstDialParaInfo->bitOpIpAddr         = VOS_TRUE;
            pstDialParaInfo->stPdpAddr.enPdpType = TAF_PDP_IPV4;
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_AppDialUpProc(VOS_UINT8 ucIndex)
{
    AT_DIAL_PARAM_STRU                 *pstAppDailPara;
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity;
    TAF_PS_DIAL_PARA_STRU               stDialParaInfo;
    AT_PS_USER_INFO_STRU                stUsrInfo;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    PS_MEM_SET(&stDialParaInfo, 0x00, sizeof(TAF_PS_DIAL_PARA_STRU));
    PS_MEM_SET(&stUsrInfo, 0x00, sizeof(AT_PS_USER_INFO_STRU));

    pstAppPdpEntity = AT_APP_GetPdpEntInfoAddr();
    pstAppDailPara  = AT_APP_GetDailParaAddr();

    PS_MEM_SET(pstAppDailPara->aucUsername, 0x00, TAF_MAX_GW_AUTH_USERNAME_LEN);
    PS_MEM_SET(pstAppDailPara->aucPassword, 0x00, TAF_MAX_GW_AUTH_PASSWORD_LEN);

    /* 解析用户信息 */
    AT_APP_ParseUsrInfo(ucIndex, &stUsrInfo);

    /* 如果已经拨号或正在拨号, 直接返回OK */
    ulResult = AT_AppCheckPdpStateForDial();
    if (AT_SUCCESS == ulResult)
    {
        AT_INFO_LOG("AT_AppDialUpProc: The state is already active or activing");
        AT_AppReportPdpActiveState(ucIndex, pstAppPdpEntity);
        return AT_SUCCESS;
    }

    /* 断开拨号过程中不允许拨号 */
    if (AT_FAILURE == ulResult)
    {
        AT_INFO_LOG("AT_AppDialUpProc: the state is error");
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_UNKNOWN);
        return AT_ERROR;
    }

    /* 获取拨号参数 */
    ulResult = AT_GetAppDialParam(&stDialParaInfo, ucIndex);
    if ( AT_SUCCESS != ulResult )
    {
        /* 记录PS域呼叫错误码 */
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_UNKNOWN);
        return AT_ERROR;
    }

    /* 保存拨号参数，用于IPV6回退 */
    pstAppDailPara->usAuthType      = stDialParaInfo.enAuthType;
    pstAppDailPara->ucCid           = stDialParaInfo.ucCid;
    pstAppDailPara->enPdpType       = stDialParaInfo.enPdpType;

    /* APN */
    pstAppDailPara->ucAPNLen        = (VOS_UINT8)gastAtParaList[2].usParaLen;
    PS_MEM_CPY((VOS_CHAR *)pstAppDailPara->aucAPN,
               (VOS_CHAR *)gastAtParaList[2].aucPara,
               gastAtParaList[2].usParaLen);

    /* 保存用户名 */
    pstAppDailPara->usUsernameLen   = gastAtParaList[3].usParaLen;
    PS_MEM_CPY((VOS_CHAR *)pstAppDailPara->aucUsername,
               (VOS_CHAR *)gastAtParaList[3].aucPara,
               gastAtParaList[3].usParaLen);

    /* 保存用户密码 */
    pstAppDailPara->usPasswordLen   = gastAtParaList[4].usParaLen;
    PS_MEM_CPY((VOS_CHAR *)pstAppDailPara->aucPassword,
               (VOS_CHAR *)gastAtParaList[4].aucPara,
               gastAtParaList[4].usParaLen);

    /* PDP ADDR */
    pstAppDailPara->ulIPv4ValidFlag = stDialParaInfo.bitOpIpAddr;
    PS_MEM_CPY((VOS_CHAR *)pstAppDailPara->aucIPv4Addr,
               (VOS_CHAR *)gastAtParaList[6].aucPara,
               gastAtParaList[6].usParaLen);

    /* 发起拨号 */
    if (VOS_OK == TAF_PS_CallOrig(WUEPS_PID_AT,
                                  AT_APP_GET_CLIENT_ID(),
                                  0,
                                  &stDialParaInfo))
    {
        /* 保存用户信息 */
        pstAppPdpEntity->stUsrInfo = stUsrInfo;

        if (TAF_PDP_IPV4 == stDialParaInfo.enPdpType)
        {
            pstAppPdpEntity->ucIpv4Cid   = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        }
        else if (TAF_PDP_IPV6 == stDialParaInfo.enPdpType)
        {
            pstAppPdpEntity->ucIpv6Cid   = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        }
        else
        {
            pstAppPdpEntity->ucIpv4v6Cid = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        }

        AT_APP_SetFirstActPdpType(stDialParaInfo.enPdpType);
        AT_APP_SetActPdpType(stDialParaInfo.enPdpType);
        AT_AppSetPdpState(stDialParaInfo.enPdpType, AT_PDP_STATE_ACTING);

        /* 设置当前pdpact操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NDISCONN_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
VOS_UINT32 AT_AppDialDownProc(
    VOS_UINT8                           ucIndex
)
{
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity;
    VOS_UINT32                          ulPdpFlag;

    ulPdpFlag = 0;

    pstAppPdpEntity                     = AT_APP_GetPdpEntInfoAddr();

    /* 如果所有的PDP都处于IDLE, 返回OK; 如果有一个处于DEACTING, 也返回OK */
    if (VOS_TRUE == AT_AppCheckPdpIdleState())
    {
        AT_WARN_LOG("AT_AppDialDownProc: The state is already IDLE.");
        return AT_OK;
    }

    if ((AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4v6State)
     || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4v6State) )
    {
         /* AT^NDISDUP的第一个参数和当前的cid不一致，直接返回ERROR */
        if (pstAppPdpEntity->ucIpv4v6Cid != gastAtParaList[0].ulParaValue)
        {
            return AT_ERROR;
        }

        ulPdpFlag = 1;

        if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                     AT_APP_GET_CLIENT_ID(),
                                     0,
                                     g_stAtAppPdpEntity.ucIpv4v6Cid))
        {
            AT_WARN_LOG("AT_AppDialDownProc():TAF_PS_CallEnd fail");
        }

        AT_AppSetPdpState(TAF_PDP_IPV4V6, AT_PDP_STATE_DEACTING);
    }
    else
    {
        if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4State)
          || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4State) )
        {
            /* AT^NDISDUP的第一个参数和当前的cid不一致，直接返回ERROR */
            if (pstAppPdpEntity->ucIpv4Cid == gastAtParaList[0].ulParaValue)
            {
                ulPdpFlag = 1;
                if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                             AT_APP_GET_CLIENT_ID(),
                                             0,
                                             pstAppPdpEntity->ucIpv4Cid))
                {
                    AT_WARN_LOG("AT_AppDialDownProc():TAF_PS_CallEnd fail");
                }

                AT_AppSetPdpState(TAF_PDP_IPV4, AT_PDP_STATE_DEACTING);

                if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv6State)
                  || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv6State) )
                {
                    if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                                 AT_APP_GET_CLIENT_ID(),
                                                 0,
                                                 pstAppPdpEntity->ucIpv6Cid))
                    {
                        AT_WARN_LOG("AT_AppDialDownProc():TAF_PS_CallEnd fail");
                    }

                    AT_AppSetPdpState(TAF_PDP_IPV6, AT_PDP_STATE_DEACTING);
                }
            }

        }

        if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv6State)
          || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv6State) )
        {
            /* AT^NDISDUP的第一个参数和当前的cid不一致，直接返回ERROR */
            if (pstAppPdpEntity->ucIpv6Cid == gastAtParaList[0].ulParaValue)
            {
                ulPdpFlag = 1;
                if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                             AT_APP_GET_CLIENT_ID(),
                                             0,
                                             pstAppPdpEntity->ucIpv6Cid))
                {
                    AT_WARN_LOG("AT_AppDialDownProc():TAF_PS_CallEnd fail");
                }

                AT_AppSetPdpState(TAF_PDP_IPV6, AT_PDP_STATE_DEACTING);

                if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4State)
                  || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4State) )
                {
                    if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                                 AT_APP_GET_CLIENT_ID(),
                                                 0,
                                                 pstAppPdpEntity->ucIpv4Cid))
                    {
                        AT_WARN_LOG("AT_AppDialDownProc():TAF_PS_CallEnd fail");
                    }

                    AT_AppSetPdpState(TAF_PDP_IPV4, AT_PDP_STATE_DEACTING);
                }

            }

        }

    }

    if (0 == ulPdpFlag)
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NDISCONN_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}



VOS_UINT32 AT_DialProc(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulResult;

    /* 参数为空 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        AT_NORM_LOG1("AT_DialProc:g_stATParseCmd.ucCmdOptType=%d\n",g_stATParseCmd.ucCmdOptType);

        /* 记录PS域呼叫错误码 */
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_INVALID_PARAMETER);

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数检查 */
    ulResult = AT_AppCheckDialParam();
    if (AT_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_DialProc:WARNING: Wrong parameter!");

        /* 记录PS域呼叫错误码 */
        AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_INVALID_PARAMETER);

        return ulResult;
    }

    /* 处理拨号，拨号或断开拨号 */
    if (0 == gastAtParaList[1].ulParaValue)
    {
        return AT_AppDialDownProc(ucIndex);
    }
    else
    {
        if (AT_HILINK_GATEWAY_MODE == g_enHiLinkMode)
        {
            /* 记录PS域呼叫错误码 */
            AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_UNKNOWN);

            return AT_ERROR;
        }

        return AT_AppDialUpProc(ucIndex);
    }
}


VOS_UINT32 AT_SetNdisdupPara (VOS_UINT8 ucIndex)
{
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* 注意不要被名字误导:
       AT^NDISUP AT命令不只是用于NDIS拨号。E5和闪电卡形态也使用该命令进行拨号。
       实现时需要根据下发AT命令的端口形态来调用不同的函数API进行拨号。 */

    /*可维可测: 接收到APP下发的NDIS拨号命令的slice值*/
    g_ulRcvAppNdisdupSlice = OM_GetSlice();

    if (AT_NDIS_USER == gastAtClientTab[ucIndex].UserType)
    {
        return At_NdisDupProc(ucIndex);
    }

    if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
    {
        /* 处理E5、闪电卡、E355等形态的拨号 */
        if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            return AT_DialProc(ucIndex);
        }
        /* 处理手机形态的拨号 */
        else
        {
            return AT_PS_ProcDialCmd(ucIndex);
        }
    }

    /* 处理AP_MODEM形态通过HISC通道下发的拨号 */
    if (VOS_TRUE == AT_CheckHsicUser(ucIndex))
    {
        return AT_PS_ProcDialCmd(ucIndex);
    }

    if (AT_USBCOM_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetPcuiPsCallFlag())
        {
            return AT_PS_ProcDialCmd(ucIndex);
        }

        if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            return AT_DialProc(ucIndex);
        }
        else
        {
            return At_NdisDupProc(ucIndex);
        }
    }


    if (AT_CTR_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetCtrlPsCallFlag())
        {
            return AT_PS_ProcDialCmd(ucIndex);
        }
    }

    /*在V2版本中，其它端口下输入该命令，直接返回OK*/
    return AT_OK;
}
TAF_UINT32 At_SetAuthdataPara (TAF_UINT8 ucIndex)
{
    TAF_AUTHDATA_EXT_STRU               stAuthDataInfo;

    PS_MEM_SET(&stAuthDataInfo, 0, sizeof(TAF_AUTHDATA_EXT_STRU));

    /* 若无参数，则直接返回OK */
    if ( AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType )
    {
        return AT_OK;
    }

    /* 参数检查 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 5 )
    {
        return AT_TOO_MANY_PARA;
    }

    stAuthDataInfo.ucCid                    = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 如果参数只有<CID>, 无需检查其它参数 */
    if ( 1 == gucAtParaIndex )
    {
        stAuthDataInfo.ucDefined            = VOS_FALSE;
    }
    else
    {
        stAuthDataInfo.ucDefined            = VOS_TRUE;

        /* 设置<Auth_type> */
        if ( 0 != gastAtParaList[1].usParaLen )
        {
            stAuthDataInfo.bitOpAuthType    = VOS_TRUE;
            stAuthDataInfo.enAuthType       = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }

        /* 设置<PLMN> */
        if ( gastAtParaList[2].usParaLen > TAF_MAX_AUTHDATA_PLMN_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if ( 0 != gastAtParaList[2].usParaLen )
        {
            stAuthDataInfo.bitOpPlmn        = VOS_TRUE;

            PS_MEM_CPY((VOS_CHAR*)stAuthDataInfo.aucPlmn,
                       (VOS_CHAR*)gastAtParaList[2].aucPara,
                       gastAtParaList[2].usParaLen);
            stAuthDataInfo.aucPlmn[gastAtParaList[2].usParaLen] = 0;
        }

        /* 设置<username> */
        if ( gastAtParaList[4].usParaLen > TAF_MAX_AUTHDATA_USERNAME_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if(0 != gastAtParaList[4].usParaLen)
        {
            stAuthDataInfo.bitOpUserName    = VOS_TRUE;

            PS_MEM_CPY((VOS_CHAR*)stAuthDataInfo.aucUserName,
                       (VOS_CHAR*)gastAtParaList[4].aucPara,
                       gastAtParaList[4].usParaLen);
            stAuthDataInfo.aucUserName[gastAtParaList[4].usParaLen] = 0;
        }

        /* 设置<passwd> */
        if ( gastAtParaList[3].usParaLen > TAF_MAX_AUTHDATA_PASSWORD_LEN )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if ( 0 != gastAtParaList[3].usParaLen )
        {
            stAuthDataInfo.bitOpPassWord    = VOS_TRUE;

            PS_MEM_CPY((VOS_CHAR*)stAuthDataInfo.aucPassWord,
                       (VOS_CHAR*)gastAtParaList[3].aucPara,
                       gastAtParaList[3].usParaLen);
            stAuthDataInfo.aucPassWord[gastAtParaList[3].usParaLen] = 0;
        }

    }

    /* 执行命令操作 */
    if ( VOS_OK != TAF_PS_SetAuthDataInfo(WUEPS_PID_AT,
                                          gastAtClientTab[ucIndex].usClientId,
                                          0,
                                          &stAuthDataInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_AUTHDATA_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 At_SetCrpnPara (TAF_UINT8 ucIndex)
{
    MN_MMA_CRPN_QRY_REQ_STRU            stMnMmaCrpnQry;

    /* 参数有效性检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        return AT_OK;
    }

    if (2 != gucAtParaIndex)
    {
        AT_WARN_LOG("At_SetCrpnPara: gucAtParaIndex != 2.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 若<name_index>或<name>参数为空，则返回失败 */
    if ((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 检查<name>参数长度有效性 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        if (gastAtParaList[1].usParaLen >= TAF_PH_OPER_NAME_LONG)
        {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }
    }
    else if(1 == gastAtParaList[0].ulParaValue)
    {
        if (gastAtParaList[1].usParaLen >= TAF_PH_OPER_NAME_SHORT)
        {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }
    }
    else
    {
        if ((gastAtParaList[1].usParaLen < 5) || (gastAtParaList[1].usParaLen > 6))
        {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }
    }


    PS_MEM_SET(&stMnMmaCrpnQry, 0, sizeof(MN_MMA_CRPN_QRY_REQ_STRU));

    /* 发送消息结构赋值 */
    stMnMmaCrpnQry.ucPlmnType    = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stMnMmaCrpnQry.usPlmnNameLen = gastAtParaList[1].usParaLen;
    PS_MEM_CPY(stMnMmaCrpnQry.aucPlmnName, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);

    /* 发送TAF_MSG_MMA_CRPN_HANDLE消息给MMA处理 */
    if (TAF_SUCCESS == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                        0,
                                        TAF_MSG_MMA_CRPN_HANDLE,
                                        &stMnMmaCrpnQry,
                                        sizeof(MN_MMA_CRPN_QRY_REQ_STRU),
                                        I0_WUEPS_PID_MMA))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CRPN_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
VOS_UINT32  AT_SetAlsPara( TAF_UINT8 ucIndex )
{
    VOS_UINT32                          ulRet;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 参数过多 */
    if ( gucAtParaIndex > 2 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*第一个参数不存在*/
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 != gastAtParaList[1].usParaLen )
    {
        ulRet = MN_CALL_SetAlsLineNo(ucIndex,
                                     (MN_CALL_ALS_LINE_NO_ENUM_U8)gastAtParaList[1].ulParaValue);
        if (VOS_OK != ulRet)
        {
            return AT_ERROR;
        }

        pstSsCtx->ucSalsType = (TAF_UINT8)gastAtParaList[0].ulParaValue;

        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APP_SET_ALS_REQ;

        return AT_WAIT_ASYNC_RETURN;
    }

    pstSsCtx->ucSalsType = (TAF_UINT8)gastAtParaList[0].ulParaValue;

    return AT_OK;
}


VOS_UINT32 At_CheckRxDivOrRxPriCmdPara(VOS_VOID)
{
    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_RX_DIV_OTHER_ERR;
    }
    /* 参数不全，直接返回失败 */
    if (   (0 == gastAtParaList[0].usParaLen)
        || (0 == gastAtParaList[1].usParaLen)
        || (18 < gastAtParaList[1].usParaLen))
    {
        return AT_CME_RX_DIV_OTHER_ERR;
    }

    /* 目前支持WCDMA，如果是其它值，直接返回失败 */
    if ('0' != gastAtParaList[0].aucPara[0])
    {
         return AT_CME_RX_DIV_OTHER_ERR;
    }

    return AT_OK;
}
VOS_UINT32 At_GetUserSetRxDivOrRxPriParaNum(
                VOS_UINT32      *pulSetDivLowBands,
                VOS_UINT32      *pulSetDivHighBands
                )
{
    VOS_UINT32                      ulResult1;
    VOS_UINT32                      ulResult2;
    VOS_UINT8                       *pucInputPara;
    VOS_UINT16                      usInputLen;

    ulResult1 = AT_OK;
    ulResult2 = AT_OK;

    *pulSetDivLowBands  = 0;
    *pulSetDivHighBands = 0;

    /* 设置接收分集参数可以有两种格式:带0x和不带0x.
       如果格式是0x时，当0x后面不带其它参数，认为参数错误 */
    pucInputPara = gastAtParaList[1].aucPara;
    usInputLen   = gastAtParaList[1].usParaLen;
    if (   ('0' == gastAtParaList[1].aucPara[0])
        && ('X' == gastAtParaList[1].aucPara[1]))
    {
        if (gastAtParaList[1].usParaLen < 3)
        {
            return AT_CME_RX_DIV_OTHER_ERR;
        }

        pucInputPara = &(gastAtParaList[1].aucPara[2]);
        usInputLen   = (VOS_UINT8)(gastAtParaList[1].usParaLen - 2);
    }

    if (usInputLen > 16)
    {
        return AT_CME_RX_DIV_OTHER_ERR;
    }

    /* AT在前面解析时，转换成了ASCII码，此时需要转换成数字 */
    if (usInputLen <= 8)
    {
        ulResult1 = At_String2Hex(pucInputPara,
                        usInputLen, pulSetDivLowBands);
    }
    else
    {
        ulResult1 = At_String2Hex(pucInputPara,
                            usInputLen-8,pulSetDivHighBands);
        ulResult2 = At_String2Hex(pucInputPara + (usInputLen - 8),
                            8,pulSetDivLowBands);
    }
    if ((AT_FAILURE == ulResult1) || (AT_FAILURE == ulResult2))
    {
        return AT_CME_RX_DIV_OTHER_ERR;
    }

    return AT_OK;
}
VOS_UINT32 At_CovertUserSetRxDivOrRxPriParaToMsInternal(
    VOS_UINT32 ulSetDivLowBands,
    VOS_UINT32 ulSetDivHighBands,
    VOS_UINT16 *pusSetDivBands
                )
{
    VOS_UINT32                      ulMsCapaDivLowBands;
    VOS_UINT32                      ulMsCapaDivHighBands;

    *pusSetDivBands      = 0;

    /*用户设置的接收分集格式如下:
        0x80000            GSM850
        0x300              GSM900
        0x80               DCS1800
        0x200000           PCS1900
        0x400000           WCDMA2100
        0x800000           WCDMA1900
        0x1000000          WCDMA1800
        0x2000000          WCDMA_AWS
        0x4000000          WCDMA850
        0x2000000000000    WCDMA900
        0x4000000000000    WCDMA1700
      而MS支持的接收分集格式如下:
        2100M/ bit1  1900M/bit2  1800M/bit3  1700M/bit4  1600M/bit5
        1500M/bit6   900M/bit7   850M/bit8   800M/bit9   450M/bit10
      需要把用户设置的接收分集转换成MS支持的格式
    */
    ulMsCapaDivLowBands  = TAF_PH_BAND_WCDMA_I_IMT_2100
                         | TAF_PH_BAND_WCDMA_II_PCS_1900
                         | TAF_PH_BAND_WCDMA_III_1800
                         | TAF_PH_BAND_WCDMA_V_850;
    ulMsCapaDivHighBands = TAF_PH_BAND_WCDMA_IX_1700
                         | TAF_PH_BAND_WCDMA_VIII_900;
    if (   (0 != (ulSetDivLowBands & (~ulMsCapaDivLowBands)))
        || (0 != (ulSetDivHighBands & (~ulMsCapaDivHighBands))))
    {
        return AT_CME_RX_DIV_NOT_SUPPORTED;
    }
    if (0 != (ulSetDivHighBands & TAF_PH_BAND_WCDMA_VIII_900))
    {
        *pusSetDivBands |= AT_MS_SUPPORT_RX_DIV_900;
    }
    if (0 != (ulSetDivHighBands & TAF_PH_BAND_WCDMA_IX_1700))
    {
        *pusSetDivBands |= AT_MS_SUPPORT_RX_DIV_IX_1700;
    }
    if (0 != (ulSetDivLowBands & TAF_PH_BAND_WCDMA_I_IMT_2100))
    {
        *pusSetDivBands |= AT_MS_SUPPORT_RX_DIV_2100;
    }
    if (0 != (ulSetDivLowBands & TAF_PH_BAND_WCDMA_II_PCS_1900))
    {
        *pusSetDivBands |= AT_MS_SUPPORT_RX_DIV_1900;
    }
    if (0 != (ulSetDivLowBands & TAF_PH_BAND_WCDMA_III_1800))
    {
        *pusSetDivBands |= AT_MS_SUPPORT_RX_DIV_1800;
    }
    if (0 != (ulSetDivLowBands & TAF_PH_BAND_WCDMA_V_850))
    {
        *pusSetDivBands |= AT_MS_SUPPORT_RX_DIV_850;
    }

    return AT_OK;
}


VOS_UINT32 At_SaveRxDivPara(
        VOS_UINT16                      usSetDivBands,
        VOS_UINT8                       ucRxDivCfg
        )
{
    TAF_AT_NVIM_RXDIV_CONFIG_STRU       stRxdivConfig;
    VOS_UINT32                          ulNvLength;

    PS_MEM_SET(&stRxdivConfig, 0x00, sizeof(TAF_AT_NVIM_RXDIV_CONFIG_STRU));
    ulNvLength = 0;

    stRxdivConfig.ucVaild = ucRxDivCfg;

    /*NV项en_NV_Item_ANTENNA_CONFIG在V3R2B060后改成en_NV_Item_W_RF_DIV_BAND，
      两个NV项完全一样。*/
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_W_RF_DIV_BAND, &usSetDivBands, sizeof(VOS_UINT16)))
    {
        PS_LOG(WUEPS_PID_AT, 0, PS_PRINT_ERROR,
                "NV_WriteEx en_NV_Item_W_RF_DIV_BAND fail!\n");
        return AT_CME_RX_DIV_OTHER_ERR;
    }

    NV_GetLength(en_NV_Item_RXDIV_CONFIG, &ulNvLength);
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_RXDIV_CONFIG, &stRxdivConfig, ulNvLength))
    {
        PS_LOG(WUEPS_PID_AT, 0, PS_PRINT_ERROR,
                "NV_WriteEx en_NV_RXDIV_CONFIG fail!\n");
        return AT_CME_RX_DIV_OTHER_ERR;
    }

    return AT_OK;
}
#if(FEATURE_LTE == FEATURE_ON)
VOS_UINT32 At_GetTmodeFromNV(VOS_VOID)
{
    VOS_UINT32                          ulTmodeRat;
    VOS_UINT32                          ulRet;
#if(VOS_OS_VER == VOS_WIN32)
    ulTmodeRat=0x40;
    ulRet = ERR_MSP_SUCCESS;
#else


    ulTmodeRat = 0;

    ulRet = NVM_Read(EN_NV_ID_TMODE_RAT, &ulTmodeRat, sizeof(ulTmodeRat));
    vos_printf("\n At_GetTmodeFromNV ulTmodeRat=0x%x \n",ulTmodeRat);
#endif
    if(ERR_MSP_SUCCESS != ulRet)
    {
        vos_printf("\n At_GetTmodeFromNV return failed !!!\n");
        ulTmodeRat = 0xc0;

        g_stTmodeRat.ucWCDMAIsSet   =      ulTmodeRat & 0x00000001;
        g_stTmodeRat.ucCDMAIsSet    = (ulTmodeRat>>1) & 0x00000001;
        g_stTmodeRat.ucTDSCDMAIsSet = (ulTmodeRat>>2) & 0x00000001;
        g_stTmodeRat.ucGSMIsSet     = (ulTmodeRat>>3) & 0x00000001;
        g_stTmodeRat.ucEDGEIsSet    = (ulTmodeRat>>4) & 0x00000001;
        g_stTmodeRat.ucAWSIsSet     = (ulTmodeRat>>5) & 0x00000001;
        g_stTmodeRat.ucFDDLTEIsSet  = (ulTmodeRat>>6) & 0x00000001;
        g_stTmodeRat.ucTDDLTEIsSet  = (ulTmodeRat>>7) & 0x00000001;
        g_stTmodeRat.ucWIFIIsSet    = (ulTmodeRat>>8) & 0x00000001;

        return ulRet;
    }
    else
    {
        if(ulTmodeRat == 0)
        {
            return ERR_MSP_NV_DATA_INVALID;
        }
        else
        {
            g_stTmodeRat.ucWCDMAIsSet   =      ulTmodeRat & 0x00000001;
            g_stTmodeRat.ucCDMAIsSet    = (ulTmodeRat>>1) & 0x00000001;
            g_stTmodeRat.ucTDSCDMAIsSet = (ulTmodeRat>>2) & 0x00000001;
            g_stTmodeRat.ucGSMIsSet     = (ulTmodeRat>>3) & 0x00000001;
            g_stTmodeRat.ucEDGEIsSet    = (ulTmodeRat>>4) & 0x00000001;
            g_stTmodeRat.ucAWSIsSet     = (ulTmodeRat>>5) & 0x00000001;
            g_stTmodeRat.ucFDDLTEIsSet  = (ulTmodeRat>>6) & 0x00000001;
            g_stTmodeRat.ucTDDLTEIsSet  = (ulTmodeRat>>7) & 0x00000001;
            g_stTmodeRat.ucWIFIIsSet    = (ulTmodeRat>>8) & 0x00000001;

        }
    }
    return ERR_MSP_SUCCESS;

}

VOS_BOOL at_TmodeIsLteSingle(VOS_VOID)
{
    VOS_UINT32 ulLte = 0;
    VOS_UINT32 ulGu = 0;

    ulLte = g_stTmodeRat.ucTDSCDMAIsSet+g_stTmodeRat.ucFDDLTEIsSet+g_stTmodeRat.ucTDDLTEIsSet;
    ulGu = g_stTmodeRat.ucWCDMAIsSet+g_stTmodeRat.ucCDMAIsSet+g_stTmodeRat.ucGSMIsSet
              +g_stTmodeRat.ucEDGEIsSet +g_stTmodeRat.ucAWSIsSet+g_stTmodeRat.ucWIFIIsSet;

    if( ( ulLte >= 1 ) && ( ulGu == 0 ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

VOS_BOOL at_TmodeIsGuSingle(VOS_VOID)
{
    VOS_UINT32 ulLte = 0;
    VOS_UINT32 ulGu = 0;

    ulLte = g_stTmodeRat.ucTDSCDMAIsSet+g_stTmodeRat.ucFDDLTEIsSet+g_stTmodeRat.ucTDDLTEIsSet;
    ulGu = g_stTmodeRat.ucWCDMAIsSet+g_stTmodeRat.ucCDMAIsSet+g_stTmodeRat.ucGSMIsSet
              +g_stTmodeRat.ucEDGEIsSet +g_stTmodeRat.ucAWSIsSet+g_stTmodeRat.ucWIFIIsSet;

    if( ( ulLte == 0 )&& ( ulGu >= 1 ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

VOS_BOOL at_TmodeIsMultiMode(VOS_VOID)
{
    VOS_UINT32 ulLte = 0;
    VOS_UINT32 ulGu = 0;

    ulLte = g_stTmodeRat.ucTDSCDMAIsSet+g_stTmodeRat.ucFDDLTEIsSet+g_stTmodeRat.ucTDDLTEIsSet;
    ulGu = g_stTmodeRat.ucWCDMAIsSet+g_stTmodeRat.ucCDMAIsSet+g_stTmodeRat.ucGSMIsSet
              +g_stTmodeRat.ucEDGEIsSet +g_stTmodeRat.ucAWSIsSet+g_stTmodeRat.ucWIFIIsSet;

    if( ( ulLte >= 1 )&& ( ulGu >= 1 ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


TAF_VOID AtTmodShowDebug(VOS_VOID)
{
    vos_printf("\n g_ulTmodeNum=0x%x \n",g_ulTmodeNum);
    vos_printf("\n g_ulGuTmodeCnfNum=0x%x \n",g_ulGuTmodeCnfNum);
}

#endif


TAF_UINT32  At_SetTModePara(TAF_UINT8 ucIndex )
{

    TAF_MMA_PHONE_MODE_PARA_STRU        stPhoneModePara;
    VOS_UINT8                           ucUpdateFlag;


    PS_MEM_SET(&stPhoneModePara, 0x00, sizeof(stPhoneModePara));


    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置TMODE */
    if (AT_TMODE_BUTT <= gastAtParaList[0].ulParaValue )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch (gastAtParaList[0].ulParaValue)
    {
        case AT_TMODE_NORMAL :

            stPhoneModePara.PhMode = TAF_PH_MODE_FULL;

            if (VOS_TRUE == TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPhoneModePara))
            {
                /* 设置当前操作类型 */
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_TMODE;
                PS_MEM_SET(&g_stAtDevCmdCtrl,0x00,sizeof(g_stAtDevCmdCtrl));
                g_stAtDevCmdCtrl.ucCurrentTMode = AT_TMODE_NORMAL;

                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        case AT_TMODE_FTM :

            if (AT_TMODE_UPGRADE == g_stAtDevCmdCtrl.ucCurrentTMode)
            {
                return AT_ERROR;
            }

            stPhoneModePara.PhMode = TAF_PH_MODE_FT;

            if (VOS_TRUE == TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPhoneModePara))
            {
                /* 设置当前操作类型 */
                g_stAtDevCmdCtrl.ucCurrentTMode = AT_TMODE_FTM;
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_TMODE;
                return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
            }
            else
            {
                return AT_ERROR;
            }
        case AT_TMODE_UPGRADE :

            if (AT_TMODE_FTM == g_stAtDevCmdCtrl.ucCurrentTMode)
            {
                return AT_ERROR;
            }
            /* 发消息到 C 核设置升级标志位，不需要再启定时器，消息类型为DRV_AGENT_TMODE_SET_REQ
               直接返回AT_OK，不需要等待设置结果 */
            ucUpdateFlag = VOS_TRUE;

            if (TAF_SUCCESS != AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_TMODE_SET_REQ,
                                   &ucUpdateFlag,
                                   sizeof(ucUpdateFlag),
                                   I0_WUEPS_PID_DRV_AGENT))
            {
                AT_ERR_LOG("AT_SetPstandbyPara: AT_FillAndSndAppReqMsg fail.");
            }

            g_stAtDevCmdCtrl.ucCurrentTMode = AT_TMODE_UPGRADE;
            return AT_OK;
        case AT_TMODE_RESET :

            g_stAtDevCmdCtrl.ucCurrentTMode = AT_TMODE_RESET;

            /* 单板重启 */
            ucUpdateFlag = VOS_FALSE;

            if (TAF_SUCCESS != AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_TMODE_SET_REQ,
                                   &ucUpdateFlag,
                                   sizeof(ucUpdateFlag),
                                   I0_WUEPS_PID_DRV_AGENT))
            {
                AT_ERR_LOG("AT_SetPstandbyPara: AT_FillAndSndAppReqMsg fail.");
            }

            return AT_OK;
        case AT_TMODE_POWEROFF:
                return AT_SetTmodeAutoPowerOff(ucIndex);

           /* 按北京要求模式4->11和模式1做相同的处理 */
        case AT_TMODE_SIGNALING :
               if (AT_TMODE_UPGRADE == g_stAtDevCmdCtrl.ucCurrentTMode)
               {
                   return AT_ERROR;
               }

           stPhoneModePara.PhMode = TAF_PH_MODE_FT;

           if (VOS_TRUE == TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPhoneModePara))
           {
               /* 设置当前操作类型 */
               g_stAtDevCmdCtrl.ucCurrentTMode = AT_TMODE_SIGNALING;
               gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_TMODE;
               return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
           }
           else
           {
               return AT_ERROR;
           }

           /* 按北京要求模式5->12和模式0做相同的处理 */
        case AT_TMODE_OFFLINE :

           stPhoneModePara.PhMode = TAF_PH_MODE_FULL;

           if (VOS_TRUE == TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPhoneModePara))
           {
               /* 设置当前操作类型 */
               gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_TMODE;
               PS_MEM_SET(&g_stAtDevCmdCtrl,0x00,sizeof(g_stAtDevCmdCtrl));
               g_stAtDevCmdCtrl.ucCurrentTMode = AT_TMODE_OFFLINE;

               return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
           }
           else
           {
               return AT_ERROR;
           }

#if((FEATURE_ON == FEATURE_LTE) || (FEATURE_ON == FEATURE_UE_MODE_TDS))
        case AT_TMODE_F_NONESIGNAL:
        case AT_TMODE_SYN_NONESIGNAL:
        case AT_TMODE_SET_SLAVE:
        case AT_TMODE_GU_BT:
        case AT_TMODE_TDS_FAST_CT:
        case AT_TMODE_TDS_BT:
        case AT_TMODE_COMM_CT:
            if(atSetTmodePara(ucIndex, gastAtParaList[0].ulParaValue) == AT_OK)
            {
                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_TMODE;
                g_stAtDevCmdCtrl.ucCurrentTMode        = (VOS_UINT8)gastAtParaList[0].ulParaValue;
                return AT_WAIT_ASYNC_RETURN;
            }
            else
            {
                return AT_ERROR;
            }
#endif

        default:
            break;
    }


    return AT_ERROR;
}



TAF_UINT8 At_GetDspLoadMode(VOS_UINT32 ulRatMode)
{
    if ((AT_RAT_MODE_WCDMA == ulRatMode)
     || (AT_RAT_MODE_AWS == ulRatMode))
    {
        return VOS_RATMODE_WCDMA;
    }
    else if ((AT_RAT_MODE_GSM == ulRatMode)
          || (AT_RAT_MODE_EDGE == ulRatMode))
    {
        return VOS_RATMODE_GSM;
    }
    else
    {
        return VOS_RATMODE_BUTT;
    }

}


VOS_UINT32  At_SendTxOnOffToGHPA(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSwitch
)
{
    AT_GHPA_RF_TX_CFG_REQ_STRU          *pstTxMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* 申请AT_GHPA_RF_TX_CFG_REQ_STRU消息 */
    ulLength = sizeof(AT_GHPA_RF_TX_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstTxMsg = (AT_GHPA_RF_TX_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstTxMsg)
    {
        AT_WARN_LOG("At_SendRfCfgReqToGHPA: alloc Tx msg fail!");
        return AT_FAILURE;
    }

    PS_MEM_SET(pstTxMsg,0x00,sizeof(AT_GHPA_RF_TX_CFG_REQ_STRU));

    /* 填写消息头 */
    pstTxMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstTxMsg->ulSenderPid     = WUEPS_PID_AT;
    pstTxMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstTxMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_GPHY);
    pstTxMsg->ulLength        = ulLength;

    /* Tx Cfg */

    /* 填写消息体 */
    pstTxMsg->usMsgID = ID_AT_GHPA_RF_TX_CFG_REQ;
    usMask            = G_RF_MASK_TX_AFC;

    /*AT^FDAC设置的FDAC值和AT^FWAVE设置的power值表示的含义相同，取后设置的值*/
    /* 发射控制方式：
        0：GMSK电压控制,此方式下usTxVpa要配置；
        1：功率控制,此方式下usTxPower要配置；
        2：8PSK PaVbias电压&DBB Atten&RF Atten控制，
        usPAVbias和usRfAtten,sDbbAtten三个参数都要配置；*/
    if ( VOS_TRUE == g_stAtDevCmdCtrl.bFdacFlag )
    {
        pstTxMsg->enCtrlMode = AT_GHPA_RF_CTRLMODE_TYPE_GMSK;
        pstTxMsg->usTxVpa = g_stAtDevCmdCtrl.usFDAC;  /* DAC设置 */
    }
    else
    {
        pstTxMsg->enCtrlMode = AT_GHPA_RF_CTRLMODE_TYPE_TXPOWER;
        pstTxMsg->usTxPower = g_stAtDevCmdCtrl.usPower;
    }

    /* Tx Cfg */
    pstTxMsg->usMask = usMask;              /* 按位标识配置类型 */
    pstTxMsg->usAFC =  0;                   /* AFC */
    pstTxMsg->usFreqNum =
                (VOS_UINT16)((g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand << 12) \
                            | g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn);
                                            /* (Band << 12) | Arfcn */
    pstTxMsg->usTxMode = g_stAtDevCmdCtrl.usTxMode;    /* 0:burst发送; 1:连续发送 */
    pstTxMsg->usModType = G_MOD_TYPE_GMSK;  /* 发射调制方式:0表示GMSK调制;1表示8PSK调制方式 */

    if (AT_DSP_RF_SWITCH_ON == ucSwitch)
    {
        pstTxMsg->usTxEnable = GDSP_CTRL_TX_ON;
    }
    else
    {
        pstTxMsg->usTxEnable = GDSP_CTRL_TX_OFF;
    }

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstTxMsg))
    {
        AT_WARN_LOG("At_SendTxOnOffToGHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}
VOS_UINT32  At_SendRxOnOffToGHPA(
    VOS_UINT8                           ucIndex,
    VOS_UINT32                          ulRxSwitch
)
{
    AT_GHPA_RF_RX_CFG_REQ_STRU          *pstRxMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* 申请AT_GHPA_RF_RX_CFG_REQ_STRU消息 */
    ulLength = sizeof(AT_GHPA_RF_RX_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstRxMsg = (AT_GHPA_RF_RX_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstRxMsg)
    {
        AT_WARN_LOG("At_SendRxOnOffToGHPA: alloc Rx msg fail!");
        return AT_FAILURE;
    }

    /* Rx Cfg */
    pstRxMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_GPHY);
    pstRxMsg->ulLength        = ulLength;
    pstRxMsg->usMsgID         = ID_AT_GHPA_RF_RX_CFG_REQ;
    pstRxMsg->usRsv           = 0;
    pstRxMsg->usRsv2          = 0;

    usMask = G_RF_MASK_RX_ARFCN | G_RF_MASK_RX_AGCMODE \
            | G_RF_MASK_RX_AGCGAIN | G_RF_MASK_RX_MODE;

    /* Reference MASK_CAL_RF_G_RX_* section */
    pstRxMsg->usMask = usMask;

    /* (Band << 12) | Arfcn */
    pstRxMsg->usFreqNum =
                    (VOS_UINT16)((g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand << 12) \
                                | g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn);

    /* 0:burst接收; 1:连续接收;*/
    pstRxMsg->usRxMode  = AT_GDSP_RX_MODE_BURST;

    /* 1:Fast AGC,0:Slow AGC */
    pstRxMsg->usAGCMode = AT_GDSP_RX_SLOW_AGC_MODE;

    /* AGC档位，共四档,取值为0-3*/
    pstRxMsg->usAgcGain = g_stAtDevCmdCtrl.ucLnaLevel;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstRxMsg))
    {
        AT_WARN_LOG("At_SendRxOnOffToGHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}
TAF_UINT32  At_SetFChanPara(TAF_UINT8 ucIndex )
{
    DRV_AGENT_FCHAN_SET_REQ_STRU         stFchanSetReq;

    /* 调用 LTE 模的接口分支 */
#if(FEATURE_LTE == FEATURE_ON)

    if ( (AT_RAT_MODE_FDD_LTE == gastAtParaList[0].ulParaValue)
       ||(AT_RAT_MODE_TDD_LTE == gastAtParaList[0].ulParaValue))
    {

        g_stAtDevCmdCtrl.ucDeviceRatMode = (AT_DEVICE_CMD_RAT_MODE_ENUM_UINT8)(gastAtParaList[0].ulParaValue);

        return atSetFCHANPara(ucIndex);
    }

#endif

#if(FEATURE_UE_MODE_TDS == FEATURE_ON)
    if(AT_RAT_MODE_TDSCDMA == gastAtParaList[0].ulParaValue)
    {
        g_stAtDevCmdCtrl.ucDeviceRatMode = (AT_DEVICE_CMD_RAT_MODE_ENUM_UINT8)(gastAtParaList[0].ulParaValue);
        return atSetFCHANPara(ucIndex);
    }
#endif


    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FCHAN_OTHER_ERR;
    }
        /* 参数不符合要求 */
    if (gucAtParaIndex != 3)
    {
        return AT_FCHAN_OTHER_ERR;
    }

    /* WIFI的第一个参数必须为8，第二个参数必须为15*/
    if (((AT_RAT_MODE_WIFI == gastAtParaList[0].ulParaValue) && (AT_BAND_WIFI != gastAtParaList[1].ulParaValue))
      ||((AT_BAND_WIFI == gastAtParaList[1].ulParaValue)&&(AT_RAT_MODE_WIFI != gastAtParaList[0].ulParaValue)))
    {
        return AT_CHANNEL_NOT_SET;
    }

    /* WIFI 分支 */
    if (BSP_MODULE_SUPPORT == DRV_GET_WIFI_SUPPORT())
    {
        if (AT_RAT_MODE_WIFI == gastAtParaList[0].ulParaValue)
        {
            /*WIFI未Enable直接返回失败*/
            if(VOS_FALSE == (VOS_UINT32)WIFI_GET_STATUS())
            {
                return AT_FCHAN_OTHER_ERR;
            }

            g_stAtDevCmdCtrl.ucDeviceRatMode = AT_RAT_MODE_WIFI;

            return AT_OK;
        }
    }

    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_DEVICE_MODE_ERROR;
    }

    /* 检查FCHAN 的接入模式是否支持*/
    if ((AT_RAT_MODE_GSM != gastAtParaList[0].ulParaValue)
     && (AT_RAT_MODE_EDGE != gastAtParaList[0].ulParaValue)
     && (AT_RAT_MODE_WCDMA != gastAtParaList[0].ulParaValue)
     && (AT_RAT_MODE_AWS != gastAtParaList[0].ulParaValue))
    {
        return AT_DEVICE_MODE_ERROR;
    }

    if (AT_BAND_BUTT <= gastAtParaList[1].ulParaValue)
    {
        return AT_FCHAN_BAND_NOT_MATCH;
    }

    PS_MEM_SET(&stFchanSetReq, 0, sizeof(DRV_AGENT_FCHAN_SET_REQ_STRU));

    stFchanSetReq.ucLoadDspMode       = At_GetDspLoadMode (gastAtParaList[0].ulParaValue);
    stFchanSetReq.ucCurrentDspMode    = At_GetDspLoadMode (g_stAtDevCmdCtrl.ucDeviceRatMode);  /*当前接入模式 */
    stFchanSetReq.bDspLoadFlag        = g_stAtDevCmdCtrl.bDspLoadFlag;
    stFchanSetReq.ucDeviceRatMode     = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stFchanSetReq.ucDeviceAtBand      = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    stFchanSetReq.usChannelNo         = (VOS_UINT16)gastAtParaList[2].ulParaValue;

    if (TAF_SUCCESS == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                              gastAtClientTab[ucIndex].opId,
                                              DRV_AGENT_FCHAN_SET_REQ,
                                              &stFchanSetReq,
                                              sizeof(stFchanSetReq),
                                              I0_WUEPS_PID_DRV_AGENT))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_FCHAN_SET;             /*设置当前操作模式 */
        return AT_WAIT_ASYNC_RETURN;                                           /* 等待异步事件返回 */
    }
    else
    {
        return AT_ERROR;
    }
}
TAF_UINT32  At_SendTxOnOffToHPA(TAF_UINT8 ucSwitch)
{
    AT_HPA_RF_CFG_REQ_STRU              *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* 申请AT_HPA_RF_CFG_REQ_STRU消息 */
    ulLength = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendTxOnOffToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    PS_MEM_SET(pstMsg,0x00,sizeof(AT_HPA_RF_CFG_REQ_STRU));

    /* 填写消息头 */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = DSP_PID_WPHY;
    pstMsg->ulLength        = ulLength;

    /* 填写消息体 */
    pstMsg->usMsgID = ID_AT_HPA_RF_CFG_REQ;
    /* Tx Cfg */
    usMask =  W_RF_MASK_AFC | W_RF_MASK_TX_ARFCN \
             | W_RF_MASK_TX_TXONOFF | W_RF_MASK_TX_PAMODE |W_RF_MASK_TX_POWDET;

    /*AT^FDAC设置的FDAC值和AT^FWAVE设置的power值表示的含义相同，取后设置的值*/
    if ( VOS_TRUE == g_stAtDevCmdCtrl.bFdacFlag )
    {
        usMask                      = usMask | W_RF_MASK_TX_AGC;
        pstMsg->stRfCfgPara.usTxAGC = g_stAtDevCmdCtrl.usFDAC;
    }
    else
    {
        usMask                       = usMask | W_RF_MASK_TX_POWER;
        pstMsg->stRfCfgPara.sTxPower = (VOS_INT16)g_stAtDevCmdCtrl.usPower;
    }

    /* Tx Cfg */
    pstMsg->stRfCfgPara.usMask      = usMask;                             /* 按位标识配置类型 */
    pstMsg->stRfCfgPara.usTxAFCInit = W_AFC_INIT_VALUE;              /* AFC */
    pstMsg->stRfCfgPara.usTxBand    = g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;    /* 1,2,3...,协议中的band编号,注意不是BandId */
    pstMsg->stRfCfgPara.usTxFreqNum = g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn; /* Arfcn */
    pstMsg->stRfCfgPara.usTxPAMode  = g_stAtDevCmdCtrl.ucPaLevel;

    if (AT_DSP_RF_SWITCH_ON == ucSwitch)
    {
        pstMsg->stRfCfgPara.usTxEnable = WDSP_CTRL_TX_ON;               /* 打开发射TX */
    }
    else
    {
        pstMsg->stRfCfgPara.usTxEnable = WDSP_CTRL_TX_OFF;               /* 关闭发射TX */
    }
    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendTxOnOffToHPA: Send msg fail!");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}


VOS_UINT32 At_SendRxOnOffToHPA(
    VOS_UINT32                          ulRxSwitch
)
{
    AT_HPA_RF_CFG_REQ_STRU              *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* 申请AT_HPA_RF_CFG_REQ_STRU消息 */
    ulLength = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendRxOnOffToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    PS_MEM_SET(pstMsg, 0x00, sizeof(AT_HPA_RF_CFG_REQ_STRU));

    /* 填写消息头 */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = DSP_PID_WPHY;
    pstMsg->ulLength        = ulLength;

    /* 填写消息体 */
    pstMsg->usMsgID = ID_AT_HPA_RF_CFG_REQ;

    usMask = W_RF_MASK_RX_ARFCN | W_RF_MASK_RX_ANTSEL \
             | W_RF_MASK_RX_RXONOFF;

    /* 按位标识配置类型 */
    pstMsg->stRfCfgPara.usMask      = usMask;

    /* 1,2,3...,协议中的band编号 */
    pstMsg->stRfCfgPara.usRxBand    = g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;
    pstMsg->stRfCfgPara.usRxFreqNum = g_stAtDevCmdCtrl.stDspBandArfcn.usDlArfcn;

    /* DSP频段Band1-Band9格式转换为
       W Rf support
    -------------------------------------------------------------------------------
            bit8       bit7      bit6     bit5    bit4     bit3      bit2     bit1
    -------------------------------------------------------------------------------
    WCDMA   900(VIII)  2600(VII) 800(VI)  850(V)  1700(IV) 1800(III) 1900(II) 2100(I) oct1
            spare      spare     spare    spare   spare    spare     spare   J1700(IX)oct2
    */

    if (AT_RX_DIV_ON == g_stAtDevCmdCtrl.ucPriOrDiv)
    {
        pstMsg->stRfCfgPara.usRxAntSel = ANT_TWO;
    }
    else
    {
        pstMsg->stRfCfgPara.usRxAntSel = ANT_ONE;
    }

    /* LNA模式控制 */
    pstMsg->stRfCfgPara.usRxLNAGainMode = g_stAtDevCmdCtrl.ucLnaLevel;

    if (AT_DSP_RF_SWITCH_ON == ulRxSwitch)
    {
        if (ANT_TWO == pstMsg->stRfCfgPara.usRxAntSel)
        {
            /* CPU控制打开射频芯片,天线1和2 */
            pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ALL_ANT_ON;
        }
        else
        {
            /* CPU控制打开射频芯片,天线1 */
            pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ANT1_ON;
        }
    }
    else
    {
        /* 关闭接收RX */
        pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_OFF;
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendRxOnOffToHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_SetFpaPara(VOS_UINT8 ucIndex)
{
    /* 调用LTE 模的接口分支 */
#if(FEATURE_LTE == FEATURE_ON)
    if ((AT_RAT_MODE_FDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode)
      ||(AT_RAT_MODE_TDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        return AT_CMD_NOT_SUPPORT;
    }
#endif
    if(AT_RAT_MODE_TDSCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        return AT_CMD_NOT_SUPPORT;
    }
    /* 命令状态检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FPA_OTHER_ERR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_FPA_OTHER_ERR;
    }

    /* 该命令需在非信令模式下使用 */
    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_DEVICE_MODE_ERROR;
    }

    /* 该命令需在设置非信令信道后使用,即^FCHAN成功执行后 */
    if (VOS_FALSE == g_stAtDevCmdCtrl.bDspLoadFlag)
    {
        return AT_CHANNEL_NOT_SET;
    }

    /* 把设置保存在本地变量 */
    g_stAtDevCmdCtrl.ucPaLevel = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    return AT_OK;

}


VOS_UINT32 At_SetFlnaPara(VOS_UINT8 ucIndex)
{

    /* 调用 LTE 模的接口分支 */
#if(FEATURE_LTE == FEATURE_ON)
    if ((AT_RAT_MODE_FDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode)
      ||(AT_RAT_MODE_TDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        return atSetFLNAPara(ucIndex);
    }
#endif

#if(FEATURE_UE_MODE_TDS == FEATURE_ON)
    if(AT_RAT_MODE_TDSCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        return atSetFLNAPara(ucIndex);
    }
#endif

    /* 命令状态检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FLNA_OTHER_ERR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_FLNA_OTHER_ERR;
    }

    /* 该命令需在非信令模式下使用 */
    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_DEVICE_MODE_ERROR;
    }

    /* 该命令需在设置非信令信道后使用 */
    if (VOS_FALSE == g_stAtDevCmdCtrl.bDspLoadFlag)
    {
        return AT_CHANNEL_NOT_SET;
    }

    /* 参数LNA等级取值范围检查 */
    if ((AT_RAT_MODE_WCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
     || (AT_RAT_MODE_AWS == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        /* WDSP LNA等级取值为0-2 */
        if (gastAtParaList[0].ulParaValue > DSP_LNA_HIGH_GAIN_MODE)
        {
            return AT_FLNA_OTHER_ERR;
        }
    }

    /* 把设置保存在本地变量 */
    g_stAtDevCmdCtrl.ucLnaLevel = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    return AT_OK;

}


TAF_UINT32  At_SetFTxonPara(TAF_UINT8 ucIndex )
{
    TAF_UINT8                           ucSwitch;

    /* 添加 LTE 模的接口分支 */
#if(FEATURE_LTE == FEATURE_ON)
    if ((AT_RAT_MODE_FDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode)
      ||(AT_RAT_MODE_TDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        return atSetFTXONPara(ucIndex);
    }
#endif

#if(FEATURE_UE_MODE_TDS == FEATURE_ON)
    if(AT_RAT_MODE_TDSCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        return atSetFTXONPara(ucIndex);
    }

#endif
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FTXON_OTHER_ERR;
    }
    /* 参数不符合要求 */
    if (1 != gucAtParaIndex)
    {
        return AT_FTXON_OTHER_ERR;
    }

    ucSwitch = (VOS_UINT8) gastAtParaList[0].ulParaValue;
    if (ucSwitch > AT_DSP_RF_SWITCH_ON)
    {
        return AT_FTXON_OTHER_ERR;
    }

    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_DEVICE_MODE_ERROR;
    }

    if (VOS_FALSE == g_stAtDevCmdCtrl.bDspLoadFlag)
    {
        return AT_CHANNEL_NOT_SET;
    }

    if ((AT_RAT_MODE_WCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
     || (AT_RAT_MODE_AWS == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        if (AT_FAILURE == At_SendTxOnOffToHPA(ucSwitch))
        {
            return AT_FTXON_SET_FAIL;
        }
    }
    else
    {
        if (AT_FAILURE == At_SendTxOnOffToGHPA(ucIndex, ucSwitch))
        {
            return AT_FTXON_SET_FAIL;
        }
    }
    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_FTXON;
    g_stAtDevCmdCtrl.ucIndex = ucIndex;

    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */

}
VOS_UINT32  At_SetFRxonPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                           ulSwitch;

     /* 调用 LTE 模的接口分支 */
#if(FEATURE_LTE == FEATURE_ON)
    if ((AT_RAT_MODE_FDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode)
      ||(AT_RAT_MODE_TDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        return atSetFRXONPara(ucIndex);
    }
#endif

#if(FEATURE_UE_MODE_TDS == FEATURE_ON)
    if(AT_RAT_MODE_TDSCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        return atSetFRXONPara(ucIndex);
    }
#endif


    /* 命令状态检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FRXON_OTHER_ERR;
    }

    /* 参数不符合要求 */
    if (gucAtParaIndex != 1)
    {
        return AT_FRXON_OTHER_ERR;
    }

    /* 该AT命令在AT^TMODE=1非信令模式下使用 */
    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_DEVICE_MODE_ERROR;
    }

    /* 该AT命令需要在AT^FCHAN设置非信令信道命令执行成功后使用 */
    if (VOS_FALSE == g_stAtDevCmdCtrl.bDspLoadFlag)
    {
        return AT_CHANNEL_NOT_SET;
    }

    ulSwitch = gastAtParaList[0].ulParaValue;

    /* 把开关接收机请求发给W物理层 */
    if ((AT_RAT_MODE_WCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
     || (AT_RAT_MODE_AWS   == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        if (AT_FAILURE == At_SendRxOnOffToHPA(ulSwitch))
        {
            return AT_FRXON_SET_FAIL;
        }
    }
    else
    {
        /* 把开关接收机请求发给G物理层 */
        if (AT_FAILURE == At_SendRxOnOffToGHPA(ucIndex, ulSwitch))
        {
            return AT_FRXON_SET_FAIL;
        }
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_FRXON;
    g_stAtDevCmdCtrl.ucIndex               = ucIndex;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;

}
TAF_UINT32  AT_SetFDac(TAF_UINT8 ucIndex )
{

    TAF_UINT16                           usDAC;

    /*调用 LTE 模的接口分支*/
#if(FEATURE_LTE == FEATURE_ON)
    if ((AT_RAT_MODE_FDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode)
      ||(AT_RAT_MODE_TDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        return AT_CMD_NOT_SUPPORT;
    }
#endif
    if(AT_RAT_MODE_TDSCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        return AT_CMD_NOT_SUPPORT;
    }
    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_FDAC_SET_FAIL;
    }
    /* 参数不符合要求 */
    if (gucAtParaIndex != 1)
    {
        return AT_FDAC_SET_FAIL;
    }
    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_DEVICE_MODE_ERROR;
    }

    if (VOS_FALSE == g_stAtDevCmdCtrl.bDspLoadFlag)
    {
        return AT_CHANNEL_NOT_SET;
    }

    usDAC = (VOS_UINT16)gastAtParaList[0].ulParaValue;

    if ((AT_RAT_MODE_WCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
     || (AT_RAT_MODE_AWS == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        if (usDAC > WDSP_MAX_TX_AGC)
        {
            return AT_FDAC_SET_FAIL;
        }
        else
        {
            g_stAtDevCmdCtrl.usFDAC = (VOS_UINT16)gastAtParaList[0].ulParaValue;
        }
    }
    else
    {
        if (usDAC > GDSP_MAX_TX_VPA)
        {
            return AT_FDAC_SET_FAIL;
        }
        else
        {
            g_stAtDevCmdCtrl.usFDAC = (VOS_UINT16)gastAtParaList[0].ulParaValue;
        }
    }

    /*AT^FDAC设置的FDAC值和AT^FWAVE设置的power值表示的含义相同，取后设置的值*/
    g_stAtDevCmdCtrl.bFdacFlag  = VOS_TRUE;
    g_stAtDevCmdCtrl.bPowerFlag = VOS_FALSE;

    return AT_OK;    /* 返回命令处理挂起状态 */

}


#if ( FEATURE_ON == FEATURE_LTE )

VOS_UINT32 At_SetSfm(VOS_UINT8 ucIndex)
{
    VOS_UINT32  ulCustomVersion;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    if(0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /* AT命令与NV中存储的信息相反 */
    if(0 == gastAtParaList[0].ulParaValue)
    {
        ulCustomVersion = 1;    /* 1 表示正式版本 */
    }
    else
    {
        ulCustomVersion = 0;    /* 0 表示烧片版本 */
    }

    /* 判断是否为烧片版本 */
    if ( VOS_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_SW_VERSION_FLAG, &ulCustomVersion, sizeof(ulCustomVersion)) )
    {
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}

#endif



TAF_UINT32   At_SetBsn(TAF_UINT8 ucIndex)
{
    TAF_PH_SERIAL_NUM_STRU stSerialNum;

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    /* 如果参数长度<imei>不等于16，直接返回错误 */
    if(TAF_SERIAL_NUM_LEN != gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }


    /*检查<imei>是否为数字字符串,不是则直接返回错误*/
    if(AT_FAILURE == At_CheckNumCharString( gastAtParaList[0].aucPara,
                                        gastAtParaList[0].usParaLen))
    {
        return AT_ERROR;
    }

    PS_MEM_CPY(stSerialNum.aucSerialNum, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);
    PS_MEM_SET(stSerialNum.aucSerialNum+TAF_SERIAL_NUM_LEN, 0, (VOS_SIZE_T)(4*sizeof(stSerialNum.aucSerialNum[0])));

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_ManufactureInfo,
                           stSerialNum.aucSerialNum,
                           TAF_SERIAL_NUM_NV_LEN))
    {
        return AT_ERROR;
    }
    else
    {
       return AT_OK;
    }

}


VOS_UINT32   At_SetQosPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                               ucQosPara;
    AT_TRAFFIC_CLASS_CUSTOMIZE_STRU         stATTrafficClass;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulNvLength;

    enModemId = MODEM_ID_0;
    ulNvLength = 0;

    /* 命令状态类型检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 数据保护未解除 */
    if (VOS_TRUE == g_bAtDataLocked)
    {
        return  AT_DATA_UNLOCK_ERROR;
    }

    /* 参数个数不为1 */
    if (gucAtParaIndex != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 参数长度不为1 */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    ucQosPara = gastAtParaList[0].aucPara[0];

    /* ucQosPara取值范围为'0'~'4' */
    if (('0' <= ucQosPara) && ('4' >= ucQosPara))
    {
        ucQosPara = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 将NV项en_NV_Item_TRAFFIC_CLASS_Type的ucStatus单元置为激活态1 */
    stATTrafficClass.ucStatus       = NV_ITEM_ACTIVE;

    /* 将数字参数值写入NV项的ucTrafficClass单元 */
    stATTrafficClass.ucTrafficClass = ucQosPara;

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("At_SetQosPara: Get modem id fail.");
        return AT_ERROR;
    }

    NV_GetLength(en_NV_Item_TRAFFIC_CLASS_Type, &ulNvLength);
    if (NV_OK != NV_WriteEx(enModemId,
                            en_NV_Item_TRAFFIC_CLASS_Type,
                            &stATTrafficClass,
                            ulNvLength))
    {
        AT_WARN_LOG("At_SetQosPara:WARNING:NVIM Write en_NV_Item_TRAFFIC_CLASS_Type failed!");
        return AT_DEVICE_OTHER_ERROR;
    }

    return  AT_OK;
}
VOS_UINT32   At_SetSDomainPara(VOS_UINT8 ucIndex)
{
    TAF_PH_SERVICE_DOMAIN               ucSrvDomain;
    NAS_NVIM_MS_CLASS_STRU              stMsClass;
    VOS_UINT32                          ulNvLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRet;

    enModemId = MODEM_ID_0;
    PS_MEM_SET(&stMsClass, 0x00, sizeof(NAS_NVIM_MS_CLASS_STRU));
    ulNvLength = 0;

    /* 命令状态类型检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 数据保护未解除 */
    if (VOS_TRUE == g_bAtDataLocked)
    {
        return AT_DATA_UNLOCK_ERROR;
    }

    /* 参数个数不为1 */
    if (gucAtParaIndex != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 参数长度不为1 */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    ucSrvDomain = gastAtParaList[0].aucPara[0];

    /* ucSrvDomain取值范围为'0'~'3' */
    if (('0' <= ucSrvDomain) && ('3' >= ucSrvDomain))
    {
        /*将字符参数转换为数字,范围为0~3*/
        ucSrvDomain = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 将数字参数转换为MsClass类型，即将3转换为0*/
    if (TAF_PH_SERVICE_ANY == ucSrvDomain)
    {
        ucSrvDomain = TAF_PH_SERVICE_CS;
    }

    stMsClass.ucMsClass = ucSrvDomain;
    /* 将转换后的参数值写入NV项en_NV_Item_MMA_MsClass的MsClass单元 */
    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("At_SetSDomainPara: Get modem id fail.");
        return AT_ERROR;
    }

    NV_GetLength(en_NV_Item_MMA_MsClass, &ulNvLength);
    if (NV_OK != NV_WriteEx(enModemId,
                            en_NV_Item_MMA_MsClass,
                            &stMsClass,
                            ulNvLength))
    {
       AT_WARN_LOG("At_SetSDomainPara:WARNING:NVIM Write en_NV_Item_MMA_MsClass failed!");
       return AT_DEVICE_OTHER_ERROR;
    }
    return  AT_OK;
}



VOS_UINT32 At_WriteDpaCatToNV(VOS_UINT8 ucDpaRate)
{
    AT_NVIM_UE_CAPA_STRU                stUECapa;
    AT_DPACAT_PARA_STRU                 astDhpaCategory[AT_DPACAT_CATEGORY_TYPE_BUTT] = {
                {PS_TRUE,   AT_HSDSCH_PHY_CATEGORY_6,  PS_FALSE, 0, PS_FALSE},                                                           /* 支持速率等级3.6M  */
                {PS_TRUE,   AT_HSDSCH_PHY_CATEGORY_8,  PS_FALSE, 0, PS_FALSE},                                                           /* 支持速率等级7.2M  */
                {PS_TRUE,   AT_HSDSCH_PHY_CATEGORY_11, PS_FALSE, 0, PS_FALSE},                                                           /* 支持速率等级1.8M  */
                {PS_TRUE,   AT_HSDSCH_PHY_CATEGORY_10, PS_FALSE, 0, PS_FALSE},                                                          /*  支持速率等级14.4M */
                {PS_TRUE,   AT_HSDSCH_PHY_CATEGORY_10, PS_TRUE,  AT_HSDSCH_PHY_CATEGORY_14, PS_FALSE}};           /*  支持速率等级21M */


    PS_MEM_SET(&stUECapa, 0x00, sizeof(stUECapa));


    if (ucDpaRate >= AT_DPACAT_CATEGORY_TYPE_BUTT)
    {
        AT_WARN_LOG("At_WriteDpaCatToNV: WARNING:Input DPA rate index invalid!");
        return VOS_ERR;
    }

    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_WAS_RadioAccess_Capa_New, &stUECapa, sizeof(AT_NVIM_UE_CAPA_STRU)))
    {
        AT_WARN_LOG("At_WriteDpaCatToNV: en_NV_Item_WAS_RadioAccess_Capa NV Read Fail!");
        return VOS_ERR;
    }

    /* 是否支持enHSDSCHSupport的标志                */
    stUECapa.enHSDSCHSupport        = astDhpaCategory[ucDpaRate].enHSDSCHSupport;

    /* 支持HS-DSCH物理层的类型标志                  */
    stUECapa.ucHSDSCHPhyCategory    = astDhpaCategory[ucDpaRate].ucHSDSCHPhyCategory;
    stUECapa.enMacEhsSupport        = astDhpaCategory[ucDpaRate].enMacEhsSupport;
    stUECapa.ucHSDSCHPhyCategoryExt = astDhpaCategory[ucDpaRate].ucHSDSCHPhyCategoryExt;

    /* 是否支持 Multi cell support,如果支持MultiCell,Ex2存在 */
    stUECapa.enMultiCellSupport     = astDhpaCategory[ucDpaRate].enMultiCellSupport;

    /* 将新的能力写到内部的全局变量，并更新到NV项中 */
    stUECapa.ulHspaStatus           = NV_ITEM_ACTIVE;                /* 此项成为激活项 */

    /* 已经设置好NV结构体中的对应值，将这些值写入NV */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_WAS_RadioAccess_Capa_New, &stUECapa, sizeof(AT_NVIM_UE_CAPA_STRU)))
    {
        AT_WARN_LOG("At_WriteDpaCatToNV: en_NV_Item_WAS_RadioAccess_Capa NV Write Fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32   At_SetDpaCat(VOS_UINT8 ucIndex)
{
    /*0     支持速率为3.6M
     *1     支持速率为7.2M
     *2     支持速率为1.8M
     *3     支持速率为14.4M
     *4     支持速率为21M
    */
    VOS_UINT8                           ucDpaRate;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数不为1 */
    if (gucAtParaIndex != 1)
    {
        return AT_ERROR;
    }

    /* 参数内容只能为1个 */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_ERROR;
    }

    ucDpaRate = gastAtParaList[0].aucPara[0];
    /* ucDpaRate取值范围为'0'~'4' */
    if (('0' <=ucDpaRate) && ('4' >= ucDpaRate))
    {
        ucDpaRate = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_ERROR;
    }

    if (VOS_TRUE == g_bAtDataLocked)
    {
        return  AT_ERROR;
    }

    /* 调用写NV接口函数: At_WriteDpaCatToNV,返回操作结果 */
    if (VOS_OK == At_WriteDpaCatToNV(ucDpaRate))
    {
        return  AT_OK;
    }
    else
    {
        AT_WARN_LOG("At_SetDpaCat:WARNING:WAS_MNTN_SetDpaCat failed!");
        return AT_ERROR;
    }

}
VOS_UINT32 AT_WriteRrcVerToNV(VOS_UINT8 ucSrcWcdmaRRC)
{
    AT_NVIM_UE_CAPA_STRU                stUECapa;


    /* 从NV项中读取en_NV_Item_WAS_RadioAccess_Capa值 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_WAS_RadioAccess_Capa_New,
                         &stUECapa,
                         sizeof(AT_NVIM_UE_CAPA_STRU)))
    {
        AT_WARN_LOG("AT_WriteRrcVerToNV():en_NV_Item_WAS_RadioAccess_Capa_New NV Read Fail!");
        return VOS_ERR;
    }

    switch(ucSrcWcdmaRRC)
    {
        case AT_RRC_VERSION_WCDMA:
            stUECapa.enEDCHSupport    = VOS_FALSE;
            stUECapa.enHSDSCHSupport  = VOS_FALSE;
            break;

        case AT_RRC_VERSION_DPA:
            stUECapa.enEDCHSupport    = VOS_FALSE;
            stUECapa.enHSDSCHSupport  = VOS_TRUE;
            break;

        case AT_RRC_VERSION_DPA_AND_UPA:
            stUECapa.enAsRelIndicator = AT_PTL_VER_ENUM_R6;
            stUECapa.enEDCHSupport    = VOS_TRUE;
            stUECapa.enHSDSCHSupport  = VOS_TRUE;
            break;

        case AT_RRC_VERSION_HSPA_PLUNS:
            stUECapa.enAsRelIndicator = AT_PTL_VER_ENUM_R7;
            stUECapa.enEDCHSupport    = VOS_TRUE;
            stUECapa.enHSDSCHSupport  = VOS_TRUE;
            break;

        default:
            break;
    }

    /* 将要设置的值写入Balong NV 结构体 */
    stUECapa.ulHspaStatus     = NV_ITEM_ACTIVE;


    /* 已经设置好NV结构体中的对应值，将这些值写入NV */
    if(NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_WAS_RadioAccess_Capa_New,
                         &stUECapa,
                         sizeof(AT_NVIM_UE_CAPA_STRU)))
    {
        AT_WARN_LOG("AT_WriteRrcVerToNV():en_NV_Item_WAS_RadioAccess_Capa_New NV Write Fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32   At_SetHspaSpt(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucRRCVer;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_DPAUPA_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex != 1)
    {
        return AT_DPAUPA_ERROR;
    }

    /* 参数内容只能为1个 */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_DPAUPA_ERROR;
    }

    ucRRCVer = gastAtParaList[0].aucPara[0];
    /* ucRRCVer取值范围为'0'~'2' */
    if (('0' <=ucRRCVer) && ('2' >= ucRRCVer))
    {
        ucRRCVer = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_DPAUPA_ERROR;
    }

    /* 检查是否解除数据保护,未解除时返回出错信息:ErrCode:0 */
    if (VOS_TRUE == g_bAtDataLocked)
    {
        return  AT_DATA_UNLOCK_ERROR;
    }

    /* 调用写NV接口函数: AT_WriteRrcVerToNV,返回操作结果 */
    if (VOS_OK == AT_WriteRrcVerToNV(ucRRCVer))
    {
        return  AT_OK;
    }
    else
    {
        AT_WARN_LOG("At_SetHspaSpt:WARNING:WAS_MNTN_SetHspaSpt failed!");
        return AT_DPAUPA_ERROR;
    }

}


VOS_UINT32  At_SetDataLock(VOS_UINT8 ucIndex )
{
    DRV_AGENT_DATALOCK_SET_REQ_STRU     stDatalockInfo;

    /* 设置命令无参数 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    /* <unlock_code>必须为长度为8的数字字符串 */
    if ( TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX != gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    if (AT_FAILURE == At_CheckNumString(gastAtParaList[0].aucPara,
                                        TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX))
    {
        return AT_ERROR;
    }

    if (VOS_TRUE == g_bAtDataLocked)
    {
        /* 复制用户解锁码 */
        stDatalockInfo.aucPwd[TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX] = '\0';
        PS_MEM_CPY(stDatalockInfo.aucPwd, gastAtParaList[0].aucPara, TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);

        /* 发消息到 C 核 获取 DATALOCK 校验信息 */
        if (TAF_SUCCESS == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                                  gastAtClientTab[ucIndex].opId,
                                                  DRV_AGENT_DATALOCK_SET_REQ,
                                                  &stDatalockInfo,
                                                  sizeof(stDatalockInfo),
                                                  I0_WUEPS_PID_DRV_AGENT))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DATALOCK_SET;           /*设置当前操作模式 */
            return AT_WAIT_ASYNC_RETURN;                                            /* 等待异步事件返回 */
        }
        else
        {
            return AT_ERROR;
        }
    }

    return AT_OK;

}

#if ( FEATURE_ON == FEATURE_LTE )
/* 生产NV恢复 */
VOS_UINT32  At_SetInfoRRS(VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数个数过多 */
    if (gucAtParaIndex != 0)
    {
        return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_INFORRS_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_INFORRS_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
#endif

VOS_UINT32  At_SetInfoRBU(VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数个数过多 */
    if (gucAtParaIndex != 0)
    {
        return AT_ERROR;
    }
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_INFORBU_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_INFORBU_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}

VOS_UINT32 At_SetGPIOPL(VOS_UINT8 ucIndex)
{
    DRV_AGENT_GPIOPL_SET_REQ_STRU       stGpioplSet;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    /* 如果参数长度GPIOPL大于20个字节,即:BCD数 > 20*2个，直接返回错误 */
    if (gastAtParaList[0].usParaLen > (AT_GPIOPL_MAX_LEN*2))
    {
        return AT_ERROR;
    }

    /*检查GPIO值是否为16进制字符串,不是则直接返回错误;是则转换为16进制数字*/
    if (AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[0].aucPara,
                                           &gastAtParaList[0].usParaLen))
    {
        return AT_ERROR;
    }

    PS_MEM_SET(stGpioplSet.aucGpiopl, 0, DRVAGENT_GPIOPL_MAX_LEN);

    PS_MEM_CPY(stGpioplSet.aucGpiopl, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    /*调用AT_FillAndSndAppReqMsg发消息到C核I0_WUEPS_PID_DRV_AGENT*/
    if (TAF_SUCCESS == AT_FillAndSndAppReqMsg( gastAtClientTab[ucIndex].usClientId,
                                              gastAtClientTab[ucIndex].opId,
                                              DRV_AGENT_GPIOPL_SET_REQ,
                                              &stGpioplSet,
                                              sizeof(stGpioplSet),
                                              I0_WUEPS_PID_DRV_AGENT))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_GPIOPL_SET;       /*设置当前操作模式 */
        return AT_WAIT_ASYNC_RETURN;                                      /* 等待异步事件返回 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32  At_ConvertSDErrToAtErrCode(VOS_UINT32 ulSDOprtErr)
{
    VOS_UINT32 ulAtSDErrCode;

    switch (ulSDOprtErr)
    {
        case AT_SD_ERR_CRAD_NOT_EXIST:
            ulAtSDErrCode = AT_SD_CARD_NOT_EXIST;
            break;

        case AT_SD_ERR_INIT_FAILED:
            ulAtSDErrCode = AT_SD_CARD_INIT_FAILED;
            break;

        case AT_SD_ERR_OPRT_NOT_ALLOWED:
            ulAtSDErrCode = AT_SD_CARD_OPRT_NOT_ALLOWED;
            break;

        case AT_SD_ERR_ADDR_ERR :
            ulAtSDErrCode = AT_SD_CARD_ADDR_ERR;
            break;

        default:
            ulAtSDErrCode = AT_SD_CARD_OTHER_ERR;
            break;

    }

    return ulAtSDErrCode;
}

VOS_BOOL At_IsArrayContentValueEquToPara(
    VOS_UINT8                           ucValue,
    VOS_UINT32                          ulArrLen,
    VOS_UINT8                           aucContent[]
 )
{
    VOS_UINT32                          i;

    for (i = 0; i < ulArrLen; i++)
    {
        if(ucValue != aucContent[i])
        {
            break;
        }
    }
    if (i != ulArrLen)
    {
        return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }

}


VOS_UINT32 At_SDOprtRead(VOS_UINT8 ucIndex)
{
    VOS_ULONG                           ulOprtErr;
    VOS_UINT32                          ulOprtRslt;
    VOS_UINT32                          ulAddr;
    VOS_UINT8                           *pucSdData;
    VOS_UINT16                          usLength;
    VOS_BOOL                            bSameContent;
    VOS_UINT8                           ucPrintData;

    if (2 != gucAtParaIndex)
    {
        return AT_SD_CARD_OTHER_ERR;
    }

    /* 申请用于保存指定地址开始的512个字节的内存*/
    pucSdData = (VOS_UINT8* )PS_MEM_ALLOC(WUEPS_PID_AT, AT_SD_DATA_UNIT_LEN);

    if (VOS_NULL_PTR == pucSdData)
    {
        AT_ERR_LOG("At_SD_Read: Fail to Alloc memory.");
        return AT_SD_CARD_OTHER_ERR;
    }

    ulAddr = gastAtParaList[1].ulParaValue;

    ulOprtRslt = DRV_SDMMC_AT_PROCESS(SD_MMC_OPRT_READ, ulAddr, 0, pucSdData, &ulOprtErr);

    ucPrintData = 0;

    if (VOS_OK == ulOprtRslt)
    {
        ulOprtErr = AT_DEVICE_ERROR_BEGIN;

        /* 指定地址开始的512个字节内容是否相同 */
        bSameContent = At_IsArrayContentValueEquToPara(pucSdData[0],AT_SD_DATA_UNIT_LEN,pucSdData);

        if (VOS_TRUE == bSameContent)
        {
            switch(pucSdData[0])
            {
                case 0x00:
                    ucPrintData = 0;
                    break;
                case 0x55:
                    ucPrintData = 1;
                    break;
                case 0xAA:
                    ucPrintData = 2;
                    break;
                case 0xFF:
                    ucPrintData = 3;
                    break;
                 default:
                    ulOprtErr = AT_SD_CARD_OTHER_ERR;
                    break;

            }
        }
        else
        {
            ulOprtErr = AT_SD_CARD_OTHER_ERR;
        }

        PS_MEM_FREE(WUEPS_PID_AT, pucSdData);

        if  (AT_DEVICE_ERROR_BEGIN == ulOprtErr)
        {
            /* 正常返回，打印对应的显示格式 */
            usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR*)pgucAtSndCodeAddr,
                                              "%s:%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              ucPrintData);

            gstAtSendData.usBufLen = usLength;
            return AT_OK;
        }
        else
        {
            /* 转换为对应错误码打印 */
            return At_ConvertSDErrToAtErrCode((VOS_UINT32)ulOprtErr);
        }

    }
    else
    {
        PS_MEM_FREE(WUEPS_PID_AT,pucSdData);
        return At_ConvertSDErrToAtErrCode((VOS_UINT32)ulOprtErr);
    }

}

VOS_UINT32 At_SDOprtWrtEraseFmt(TAF_UINT8 ucIndex)
{
    VOS_ULONG                           ulOprtErr;
    VOS_UINT32                          ulOprtRslt;
    VOS_UINT32                          ulAddr;
    VOS_UINT32                          ulData;

    switch (gastAtParaList[0].ulParaValue)
    {
        case AT_SD_OPRT_FORMAT:
            /* 对SD卡进行格式化操作 */
            ulOprtRslt = DRV_SDMMC_AT_PROCESS(SD_MMC_OPRT_FORMAT,
                                              0, 0, VOS_NULL_PTR, &ulOprtErr);
            break;

        case AT_SD_OPRT_ERASE:
            /* 只有一个参数时，擦除整张SD卡; 多个参数时，删除指定地址的SD卡内容  */
            if (1 == gucAtParaIndex)
            {
                ulOprtRslt = DRV_SDMMC_AT_PROCESS(SD_MMC_OPRT_ERASE_ALL,
                                                  0, 0, VOS_NULL_PTR, &ulOprtErr);
            }
            else
            {
                ulAddr     = gastAtParaList[1].ulParaValue;
                ulOprtRslt = DRV_SDMMC_AT_PROCESS(SD_MMC_OPRT_ERASE_SPEC_ADDR,
                                                  ulAddr, 0, VOS_NULL_PTR, &ulOprtErr);
            }
            break;

        case AT_SD_OPRT_WRITE:
            /* 对SD卡的指定地址进行写操作 */
            if (3 == gucAtParaIndex)
            {
                ulAddr     = gastAtParaList[1].ulParaValue;
                ulData     = gastAtParaList[2].ulParaValue;
                ulOprtRslt = DRV_SDMMC_AT_PROCESS(SD_MMC_OPRT_WRITE,
                                                  ulAddr, ulData, VOS_NULL_PTR, &ulOprtErr);
            }
            else
            {
                return AT_SD_CARD_OTHER_ERR;
            }
            break;

        default:
            return AT_SD_CARD_OPRT_NOT_ALLOWED;
    }

    if (VOS_OK != ulOprtRslt)
    {
       return At_ConvertSDErrToAtErrCode((VOS_UINT32)ulOprtErr);
    }

    return AT_OK;
}
VOS_UINT32  At_SetSD(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulCurrentOprtStaus;
    VOS_INT32                           lCurrentSDStaus;

    /* 设置命令无参数时，AT^SD为查询SD卡在位状态 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        lCurrentSDStaus = DRV_SDMMC_GET_STATUS();
        if (AT_SD_STATUS_EXIST == lCurrentSDStaus)
        {
            return AT_OK;
        }
        else
        {
            return AT_ERROR;
        }
    }

    /* 参数过多 */
    if (gucAtParaIndex > 3)
    {
        return AT_SD_CARD_OTHER_ERR;
    }

    /* SD卡操作为串行方式，查询当前是否有进行的操作，以确定是否可进行新的设置操作 */
    ulCurrentOprtStaus = DRV_SDMMC_GET_OPRT_STATUS();

    /* 上次的操作尚未结束 ，不进行新操作 */
    if (AT_SD_OPRT_RSLT_NOT_FINISH == ulCurrentOprtStaus)
    {
        return AT_SD_CARD_OTHER_ERR;
    }

    /*  当前非执行状态，可进行新的读、写、格式化操作 */
    if (AT_SD_OPRT_READ == gastAtParaList[0].ulParaValue)
    {
        return At_SDOprtRead(ucIndex);
    }
    else
    {
        return At_SDOprtWrtEraseFmt(ucIndex);
    }


}


 VOS_UINT32 At_CheckRxdivOrRxpriParaIfSupported(
     VOS_UINT16                         *pusSetBands,
     VOS_UINT16                         usBands
 )
 {
     VOS_UINT32                         ulResult;
     VOS_UINT32                         i;
     VOS_UINT32                         ulSetLowBands;
     VOS_UINT32                         ulSetHighBands;

     /* 把用户设置的接收分集或主集参数转换成数字 */
     ulResult = At_GetUserSetRxDivOrRxPriParaNum(&ulSetLowBands, &ulSetHighBands);
     if (AT_OK != ulResult)
     {
         return ulResult;
     }

     /* 如果设置成0X3FFFFFFF，认为是打开所有支持的分集或主集;
        否则，把用户设置的参数转成和驱动保存的RX分集或主集对应的格式 */
     if ((TAF_PH_BAND_ANY == ulSetLowBands) && (0 == ulSetHighBands))
     {
         *pusSetBands = usBands;
     }
     else
     {
         ulResult = At_CovertUserSetRxDivOrRxPriParaToMsInternal(ulSetLowBands,
                                           ulSetHighBands, pusSetBands);
         if (AT_OK != ulResult)
         {
             return ulResult;
         }
     }

     /* 底软支持的RX DIV格式如下:
          2100M/ bit1  1900M/bit2  1800M/bit3  1700M/bit4  1600M/bit5
          1500M/bit6   900M/bit7   850M/bit8   800M/bit9   450M/bit10
       如果打开的是底软不支持的分集或主集，返回错误 */
     for (i = 0; i < 16; i++)
     {
         if ((0 == ((usBands >> i) & 0x0001))
          && (0x0001 == ((*pusSetBands >> i) & 0x0001)))
         {
              return AT_CME_RX_DIV_NOT_SUPPORTED;
         }
     }

     return AT_OK;

 }


VOS_UINT32 At_SetRxDiv(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    AT_DRV_AGENT_RXDIV_SET_STRU         stRxdivSet;
    VOS_UINT32                          ulResult;

    ulResult = At_CheckRxDivOrRxPriCmdPara();
    if (AT_OK != ulResult)
    {
        return ulResult;
    }

    /* 把用户设置的接收分集或主集参数转换成数字 */
    ulResult = At_GetUserSetRxDivOrRxPriParaNum(&stRxdivSet.ulSetLowBands,
                                                &stRxdivSet.ulSetHighBands);
    if (AT_OK != ulResult)
    {
        return ulResult;
    }

    stRxdivSet.ucRxOnOff = g_stAtDevCmdCtrl.ucRxOnOff;
    stRxdivSet.usDspBand = g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;

    /* 发送消息DRV_AGENT_HARDWARE_QRY给AT AGENT处理，该消息无参数结构 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   DRV_AGENT_RXDIV_SET_REQ,
                                   (VOS_VOID*)&stRxdivSet,
                                   sizeof(stRxdivSet),
                                   I0_WUEPS_PID_DRV_AGENT);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DRV_AGENT_RXDIV_SET_REQ;
        return AT_WAIT_ASYNC_RETURN;
    }
    return AT_ERROR;
}


VOS_UINT32  AT_ProcUus1WithParm(
    VOS_UINT8                           ucIndex,
    MN_CALL_UUS1_PARAM_STRU            *pstUus1Group
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulMsgNum;
    VOS_UINT32                          i;
    VOS_UINT32                          ulRst;

    /* 去处<n>和<m>后,实际的消息个数 */
    ulMsgNum   = gucAtParaIndex;


    if(0 == gastAtParaList[2].usParaLen)
    {
        pstUus1Group->ulActNum = 0;
    }
    else
    {
        /* 设置message */
        for (i = 2 ; (i < ulMsgNum && i < (AT_MAX_PARA_NUMBER-1)); i++)
        {

            /* 先需要转换,由于该命令可设置多次参数,AT格式中仅对前4个参数进行转换,
            后面的参数未进行转换,因此需要将数组中的值进行所转换*/
            if (AT_SUCCESS != At_Auc2ul(gastAtParaList[i].aucPara,
                                        gastAtParaList[i].usParaLen,
                                       &gastAtParaList[i].ulParaValue))
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            pstUus1Group->stUus1Info[pstUus1Group->ulActNum].enMsgType = gastAtParaList[i].ulParaValue;
            i++;

            /*<UUIE>不存在,表明是去激活 */
            if ( 0 == gastAtParaList[i].usParaLen )
            {
                pstUus1Group->enSetType[pstUus1Group->ulActNum] = MN_CALL_SET_UUS1_DEACT;
            }
            else
            {
                pstUus1Group->enSetType[pstUus1Group->ulActNum] = MN_CALL_SET_UUS1_ACT;
                /* 需将ASCII转换为HEX */
                if (AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[i].aucPara,
                                                      &gastAtParaList[i].usParaLen))
                {
                    return AT_CME_INCORRECT_PARAMETERS;
                }

                PS_MEM_CPY(pstUus1Group->stUus1Info[pstUus1Group->ulActNum].aucUuie,
                           gastAtParaList[i].aucPara,
                           gastAtParaList[i].usParaLen);
            }

            ulRet = MN_CALL_CheckUus1ParmValid(pstUus1Group->enSetType[pstUus1Group->ulActNum],
                                              &(pstUus1Group->stUus1Info[pstUus1Group->ulActNum]));
            if ( MN_ERR_NO_ERROR != ulRet )
            {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            pstUus1Group->ulActNum++;

            /* 如果未携带UUIE,则默认表示结束不关心后面参数 */
            if (MN_CALL_SET_UUS1_DEACT == pstUus1Group->enSetType[pstUus1Group->ulActNum])
            {
                break;
            }
        }
    }
    /* 1.对要设置的各个Uus1Info进行设定，然后通过MN_CALL_APP_SET_UUSINFO消息带参数结构
         MN_CALL_UUS1_PARAM_STRU通知CS进行参数设置*/
    ulRst = MN_CALL_SendAppRequest(MN_CALL_APP_SET_UUSINFO_REQ,
                                   gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   0,
                                   (MN_CALL_APP_REQ_PARM_UNION*)pstUus1Group);
    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APP_SET_UUSINFO_REQ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}


VOS_UINT32 At_SetCuus1Para(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulRet;
    MN_CALL_UUS1_PARAM_STRU             stUus1Group;

    /*************************************************************
    27007中该命令格式,该命令可以不携带任何参数,如果不携带参数直接返回OK
    + CUUS1 =[<n>[,<m>[,<message>[,<UUIE> [,<message>[,<UUIE>[,...]]]]]]]
    ****************************************************************/

    /* 不携带参数直接返回OK */
    if ( 0 == gucAtParaIndex )
    {
        return AT_OK;
    }

    PS_MEM_SET(&stUus1Group,0x00,sizeof(stUus1Group));

    /* 设置<n> */
    if(0 != gastAtParaList[0].usParaLen)
    {
        stUus1Group.enCuus1IFlg = (MN_CALL_CUUS1_FLG_ENUM_U32)gastAtParaList[0].ulParaValue;
    }
    else
    {
        /* <n>不存在同时参数个数不为0返回ERROR */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<m> */
    if(0 != gastAtParaList[1].usParaLen)
    {
        stUus1Group.enCuus1UFlg = (MN_CALL_CUUS1_FLG_ENUM_U32)gastAtParaList[1].ulParaValue;
    }

    /* 设置message */
    /* 只带n和m参数时，也要给MN发消息 */
    ulRet = AT_ProcUus1WithParm(ucIndex, &stUus1Group);


    return ulRet;

}

VOS_UINT32  At_SetGlastErrPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;
    TAF_PS_CAUSE_ENUM_UINT32            enPsCause;

    usLength                            = 0;

    /* 检查拨号错误码是否使能*/
    if (PPP_DIAL_ERR_CODE_DISABLE == gucPppDialErrCodeRpt)
    {
       return AT_ERROR;
    }

    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

     /* 没有参数*/
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数值错误*/
    if (1 != gastAtParaList[0].ulParaValue)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    gulErrType = gastAtParaList[0].ulParaValue;

    /* 获取错误码*/
    enPsCause = AT_PS_GetPsCallErrCause(ucIndex);
    if (TAF_PS_CAUSE_SUCCESS == enPsCause)
    {
        return AT_ERROR;
    }

    /* 将错误码上报给后台*/
    usLength =  (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: %d,%d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       gulErrType,
                                       AT_Get3gppSmCauseByPsCause(enPsCause));

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_SetMDatePara(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulRet;
    AT_DATE_STRU                        stDate;
    TAF_AT_NVIM_MANUFACTURE_DATE_STRU   stManufactrueDate;

    if (gucAtParaIndex != 1)
    {
        AT_NORM_LOG("AT_SetMDatePara: the number of parameters is error.");
        return AT_ERROR;
    }

    /* 参数检查1:日期格式检查；*/
    ulRet = AT_CheckDateFormat(gastAtParaList[0].aucPara,
                               gastAtParaList[0].usParaLen);
    if (AT_OK != ulRet)
    {
        AT_NORM_LOG("AT_SetMDatePara: the format of parameters is error.");
        return AT_ERROR;
    }

    /* 参数检查2:获取生产日期并对日期作有效性检查；*/
    ulRet = AT_GetDate(gastAtParaList[0].aucPara,
                       gastAtParaList[0].usParaLen,
                       &stDate);
    if (AT_OK != ulRet)
    {
        AT_NORM_LOG("AT_SetMDatePara: Fail to get the date.");
        return AT_ERROR;
    }

    ulRet = AT_CheckDate(&stDate);
    if (AT_OK != ulRet)
    {
        AT_NORM_LOG("AT_SetMDatePara: the value of parameters is error.");
        return AT_ERROR;
    }

    /* 写日期参数到NV */
    PS_MEM_CPY(stManufactrueDate.aucMDate, gastAtParaList[0].aucPara, (AT_MDATE_STRING_LENGTH - 1));
    stManufactrueDate.aucMDate[AT_MDATE_STRING_LENGTH - 1] = '\0';
    ulRet = NV_WriteEx(MODEM_ID_0, en_NV_Item_AT_MANUFACTURE_DATE,
                     stManufactrueDate.aucMDate,
                     AT_MDATE_STRING_LENGTH);
    if (NV_OK != ulRet)
    {
        AT_WARN_LOG("AT_SetMDatePara: Fail to write nv.");
        return AT_ERROR;
    }

    return AT_OK;
}
VOS_UINT32 AT_SetFacInfoPara(
    VOS_UINT8                          ucIndex
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           *pucFacInfo = TAF_NULL_PTR;
    VOS_UINT32                          ulOffSet;

    if ((gucAtParaIndex != 2) || ((0 == gastAtParaList[1].usParaLen)))
    {
        AT_WARN_LOG("AT_SetFacInfoPara: the number of parameters is error.");
        return AT_ERROR;
    }

    /* 长度超过制造信息有效字段长度则参数检查失败 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        if (AT_FACINFO_INFO1_LENGTH != gastAtParaList[1].usParaLen)
        {
            AT_NORM_LOG("AT_SetFacInfoPara: Length of info1 is failure.");
            return AT_ERROR;
        }

        if (AT_FAILURE == At_CheckNumString(gastAtParaList[1].aucPara, AT_FACINFO_INFO1_LENGTH))
        {
            return AT_ERROR;
        }
    }
    else
    {
        if (AT_FACINFO_INFO2_LENGTH != gastAtParaList[1].usParaLen)
        {
            AT_NORM_LOG("AT_SetFacInfoPara: Length of info2 is failure.");
            return AT_ERROR;
        }

        if (AT_FAILURE == At_CheckNumString(gastAtParaList[1].aucPara, AT_FACINFO_INFO2_LENGTH))
        {
            return AT_ERROR;
        }
    }

    /* 获取NV中已经存储的制造信息 */
    pucFacInfo = (TAF_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_FACINFO_STRING_LENGTH);
    if (TAF_NULL_PTR == pucFacInfo)
    {
        AT_WARN_LOG("AT_SetFacInfoPara: fail to alloc memory.");
        return AT_ERROR;
    }

    ulRet = NV_ReadEx(MODEM_ID_0, en_NV_Item_AT_FACTORY_INFO,
                    pucFacInfo,
                    AT_FACINFO_STRING_LENGTH);
    if (NV_OK != ulRet)
    {
        AT_WARN_LOG("AT_SetFacInfoPara: fail to read NVIM . ");
        PS_MEM_FREE(WUEPS_PID_AT, pucFacInfo);
        return AT_ERROR;
    }

    /*
       组装待写入NV项en_NV_Item_AT_FACTORY_INFO的局部变量:
       根据用户输入参数<INDEX>刷新制造信息,
       <INDEX>为0则刷新前129字节，其中第129字节为结束符；
       <INDEX>为1则刷新后129字节，其中第129字节为结束符；
    */
    ulOffSet = (AT_FACINFO_INFO1_LENGTH + 1) * gastAtParaList[0].ulParaValue;
    if ((ulOffSet + gastAtParaList[1].usParaLen) > ((AT_FACINFO_INFO1_LENGTH + 1) + AT_FACINFO_INFO2_LENGTH))
    {
        AT_WARN_LOG("AT_SetFacInfoPara: offset is error. ");
        PS_MEM_FREE(WUEPS_PID_AT, pucFacInfo);
        return AT_ERROR;
    }

    *((pucFacInfo + ulOffSet) + gastAtParaList[1].usParaLen) = '\0';

    PS_MEM_CPY((pucFacInfo + ulOffSet),
                gastAtParaList[1].aucPara,
                gastAtParaList[1].usParaLen);

    /* 刷新用户修改后的制造信息到NV项en_NV_Item_AT_FACTORY_INFO */
    ulRet = NV_WriteEx(MODEM_ID_0, en_NV_Item_AT_FACTORY_INFO,
                     pucFacInfo,
                     AT_FACINFO_STRING_LENGTH);
    if (NV_OK != ulRet)
    {
        AT_WARN_LOG("AT_SetFacInfoPara: fail to write NVIM . ");
        PS_MEM_FREE(WUEPS_PID_AT, pucFacInfo);
        return AT_ERROR;
    }

    PS_MEM_FREE(WUEPS_PID_AT, pucFacInfo);
    return AT_OK;
}
VOS_UINT32 At_SetCallSrvPara(VOS_UINT8 ucIndex)
{
    NAS_NVIM_CUSTOMIZE_SERVICE_STRU              stCustSrv;

    /* 命令状态检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 参数个数过多 */
    if (gucAtParaIndex != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 检查是否解除数据保护,未解除时返回出错信息 */
    if (VOS_TRUE == g_bAtDataLocked)
    {
        return  AT_DATA_UNLOCK_ERROR;
    }

    stCustSrv.ulCustomizeService = gastAtParaList[0].ulParaValue;
    stCustSrv.ulStatus           = NV_ITEM_ACTIVE;          /*NV项设置为激活*/

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_CustomizeService, &stCustSrv, sizeof(stCustSrv)))
    {
        AT_WARN_LOG("At_SetCallSrvPara():en_NV_Item_CustomizeService NV Write Fail!");
        return AT_DEVICE_OTHER_ERROR;
    }

    return AT_OK;

}


VOS_UINT32 At_WriteWifiNVToDefault(
    AT_CUSTOMIZE_ITEM_DFLT_ENUM_UINT8   enCustomizeItem
)
{
    TAF_AT_MULTI_WIFI_SEC_STRU              stWifiKey;
    TAF_AT_MULTI_WIFI_SSID_STRU             stWifiSsid;

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return VOS_OK;
    }

    PS_MEM_SET(&stWifiKey, 0, sizeof(stWifiKey));
    PS_MEM_SET(&stWifiSsid, 0, sizeof(stWifiSsid));

    /* 读取WIFI KEY对应的NV项 */
    if (NV_OK!= NV_ReadEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_KEY, &stWifiKey, sizeof(TAF_AT_MULTI_WIFI_SEC_STRU)))
    {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:READ NV ERROR");
        return VOS_ERR;
    }

    /* 设置WIKEY为默认值 */
    PS_MEM_SET(stWifiKey.aucWifiWpapsk, 0x00, sizeof(stWifiKey.aucWifiWpapsk));

    /* 设置WIWEP为默认值 */
    PS_MEM_SET(stWifiKey.aucWifiWepKey1, 0x00, sizeof(stWifiKey.aucWifiWepKey1));
    PS_MEM_SET(stWifiKey.aucWifiWepKey2, 0x00, sizeof(stWifiKey.aucWifiWepKey2));
    PS_MEM_SET(stWifiKey.aucWifiWepKey3, 0x00, sizeof(stWifiKey.aucWifiWepKey3));
    PS_MEM_SET(stWifiKey.aucWifiWepKey4, 0x00, sizeof(stWifiKey.aucWifiWepKey4));

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_KEY, &stWifiKey, sizeof(TAF_AT_MULTI_WIFI_SEC_STRU)))
    {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:WRITE NV ERROR");
        return VOS_ERR;
    }

    /*读取SSID对应的NV项*/
    if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_STATUS_SSID, &stWifiSsid, sizeof(TAF_AT_MULTI_WIFI_SSID_STRU)))
    {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:READ NV ERROR");
        return VOS_ERR;
    }

    /* 设置SSID为默认值 */
    PS_MEM_SET(stWifiSsid.aucWifiSsid, 0x00, sizeof(stWifiSsid.aucWifiSsid));

    /*写入WIFI SSID对应的NV项*/
    if (VOS_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_STATUS_SSID, &stWifiSsid, sizeof(TAF_AT_MULTI_WIFI_SSID_STRU)))
    {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:WRITE NV ERROR");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 At_WriteSimLockNVToDefault(
    AT_CUSTOMIZE_ITEM_DFLT_ENUM_UINT8   enCustomizeItem
)
{
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU  stSimLockPlmnInfo;
    TAF_CUSTOM_CARDLOCK_STATUS_STRU     stCardLockStatus;
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU  stSimLockMaxTimes;
    VOS_UINT32                          i;

    VOS_UINT8                           aucRange[TAF_PH_SIMLOCK_PLMN_STR_LEN]
                                            = {AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                               AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL};


    /* 该定制项在可配置需求文档中默认值和单板自定义的默认值相同
       en_NV_Item_CustomizeSimLockPlmnInfo的ulStatus:默认未激活;
       mnc_num:默认为2; range_begin和rang_end默认全为0xAA */
    stSimLockPlmnInfo.ulStatus = NV_ITEM_DEACTIVE;
    for (i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++)
    {
        stSimLockPlmnInfo.astSimLockPlmnRange[i].ucMncNum = AT_SIM_LOCK_MNC_NUM_DEFAULT_VAL;
        PS_MEM_CPY(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin,
                   aucRange, sizeof(aucRange));
        PS_MEM_CPY(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeEnd,
                   aucRange, sizeof(aucRange));
    }

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_CustomizeSimLockPlmnInfo,
                          &stSimLockPlmnInfo, sizeof(stSimLockPlmnInfo)))
    {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():en_NV_Item_CustomizeSimLockPlmnInfo NV Write Fail!");
        return VOS_ERR;
    }

    /* 该定制项在可配置需求文档中默认值和单板自定义的默认值相同,
       en_NV_Item_CardlockStatus的ulStatus:默认为未激活; CardlockStatus:默认值为2;
       RemainUnlockTimes:默认值为0 */
    stCardLockStatus.ulStatus            = NV_ITEM_DEACTIVE;
    stCardLockStatus.enCardlockStatus    = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
    stCardLockStatus.ulRemainUnlockTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_CardlockStatus, &stCardLockStatus,
                          sizeof(stCardLockStatus)))

    {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():en_NV_Item_CardlockStatus NV Write Fail!");
        return VOS_ERR;
    }


    /*向C核发送消息备份simlockNV*/
    if (TAF_SUCCESS != AT_FillAndSndAppReqMsg(0,
                                              0,
                                              DRV_AGENT_SIMLOCK_NV_SET_REQ,
                                              &stCardLockStatus,
                                              sizeof(stCardLockStatus),
                                              I0_WUEPS_PID_DRV_AGENT))
    {

        AT_WARN_LOG("At_WriteSimLockNVToDefault():DRV_AGENT_SIMLOCK_NV_SET_REQ NV Write Fail!");
        return VOS_ERR;
    }


    /* 该定制项在可配置需求文档中默认值和单板自定义的默认值相同,
       en_NV_Item_CustomizeSimLockMaxTimes的ulStatus:默认为未激活；
       LockMaxTimes:默认最大解锁次数为10次 */
    stSimLockMaxTimes.ulStatus       = NV_ITEM_DEACTIVE;
    stSimLockMaxTimes.ulLockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_CustomizeSimLockMaxTimes, &stSimLockMaxTimes,
                          sizeof(stSimLockMaxTimes)))
    {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():en_NV_Item_CustomizeSimLockMaxTimes NV Write Fail!");
        return VOS_ERR;
    }

    return VOS_OK;

}



VOS_UINT32 At_WriteCustomizeSrvNVToDefault(
    AT_CUSTOMIZE_ITEM_DFLT_ENUM_UINT8   enCustomizeItem
)
{
    NAS_NVIM_CUSTOMIZE_SERVICE_STRU              stCustSrv;

    /* 该定制项在可配置需求文档中默认值与单板自定义的默认值相同,
       ulStatus:默认为未激活;ulCustomizeService:默认禁止普通语音业务 */
    stCustSrv.ulCustomizeService = VOS_FALSE;
    stCustSrv.ulStatus           = NV_ITEM_DEACTIVE;

    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_CustomizeService, &stCustSrv, sizeof(stCustSrv)))
    {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault():en_NV_Item_CustomizeService NV Write Fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 At_WriteGprsActTimerNVToDefault(
    AT_CUSTOMIZE_ITEM_DFLT_ENUM_UINT8   enCustomizeItem
)
{
    VOS_UINT32                          ulGprsActiveTimerLen;

    /* 可配置需求文档中默认值和单板自定义的默认值均为20秒 */
    ulGprsActiveTimerLen = AT_GPRS_ACT_TIMER_LEN_DEFAULT_VAL;
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_GPRS_ActiveTimerLength,
                          &ulGprsActiveTimerLen, sizeof(ulGprsActiveTimerLen)))
    {
        AT_WARN_LOG("At_WriteGprsActTimerNVToDefault():en_NV_Item_GPRS_ActiveTimerLength NV Write Fail!");
        return VOS_ERR;
    }

    return VOS_OK;

}
VOS_UINT32 At_SetCustomizeItemToDefaultVal(
    AT_CUSTOMIZE_ITEM_DFLT_ENUM_UINT8   enCustomizeItem
)
{

    /* 设置WIFI定制项对应的en_NV_Item_WIFI_KEY、
       en_NV_Item_WIFI_STATUS_SSID 2个NV项 */
    if (VOS_OK != At_WriteWifiNVToDefault(enCustomizeItem))
    {
        return VOS_ERR;
    }

    /* 设置SimLock定制项对应的en_NV_Item_CustomizeSimLockPlmnInfo、
       en_NV_Item_CardlockStatus、en_NV_Item_CustomizeSimLockMaxTimes 3个NV项 */
    if (VOS_OK != At_WriteSimLockNVToDefault(enCustomizeItem))
    {
        return VOS_ERR;
    }

    return VOS_OK;

}


VOS_UINT32 At_SetCsdfltPara(VOS_UINT8 ucIndex)
{
    /* 命令状态检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数个数过多 */
    if (gucAtParaIndex != 0)
    {
        return AT_ERROR;
    }

    /* 检查是否解除数据保护,未解除时返回出错信息 */
    if (VOS_TRUE == g_bAtDataLocked)
    {
        return  AT_ERROR;
    }

    /* 默认值为0 */
    if (VOS_OK != At_SetCustomizeItemToDefaultVal(AT_CUSTOMIZE_ITEM_DEFAULT_VALUE_FROM_CFGFILE))
    {
        return AT_ERROR;
    }

    /*  恢复出厂设置后通知应用保存wifi定制信息 */
    if (BSP_MODULE_SUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        AT_PhSendRestoreFactParmNoReset();
    }

    return AT_OK;

}
VOS_UINT32  At_GetParaCnt(
    VOS_UINT8                           *pucData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT16                          i;
    VOS_UINT32                          ulCnt;

    ulCnt = 0;
    for ( i = 0; i < usLen ; i++ )
    {
        if (',' == pucData[i])
        {
            ++ ulCnt;
        }
    }

    /*参数个数 = ','个数+1*/
    return (ulCnt + 1);
}
VOS_UINT32 At_AsciiNum2SimLockImsiStr (
    VOS_UINT8                           *pucDst,
    VOS_UINT8                           *pucSrc,
    VOS_UINT16                          usSrcLen
)
{
    VOS_UINT16                          usChkLen;
    VOS_UINT8                           ucTmp;
    VOS_UINT8                           ucBcdCode;

    /* 字符串长度最大为16 */
    if (usSrcLen > (TAF_PH_SIMLOCK_PLMN_STR_LEN * 2))
    {
        return AT_FAILURE;
    }

    for (usChkLen = 0; usChkLen < usSrcLen; usChkLen++)
    {
        /* the number is 0-9 */
        if ((pucSrc[usChkLen] >= 0x30) && (pucSrc[usChkLen] <= 0x39))
        {
            ucBcdCode = pucSrc[usChkLen] - 0x30;
        }
        else
        {
            return AT_FAILURE;
        }

        ucTmp = usChkLen % 2;   /* 判断高低位 */
        if (0 == ucTmp)
        {
            pucDst[usChkLen/2] = (VOS_UINT8)((ucBcdCode << 4) & 0xF0); /* 高位 */
        }
        else
        {
            pucDst[usChkLen/2] |= (VOS_UINT8)(ucBcdCode);   /* 低位 */
        }
    }

    if(1 == (usSrcLen % 2))
    {
        pucDst[usSrcLen/2] |= 0x0F; /* 低位 */
    }

    return AT_SUCCESS;
}


VOS_UINT32 At_CovertAtParaToSimlockPlmnInfo(
    VOS_UINT32                          ulParaCnt,
    AT_PARSE_PARA_TYPE_STRU             *pstParalist,
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU  *pstSimLockPlmnInfo
)
{
    VOS_UINT8                           ucMncNum;
    VOS_UINT8                           aucImsiStr[TAF_PH_SIMLOCK_PLMN_STR_LEN];
    VOS_UINT8                           *pucPlmnRange;
    VOS_UINT32                          i;
    VOS_UINT32                          ulPlmnParaIdx;
    VOS_UINT32                          ulImsiStrLen;

    ulPlmnParaIdx = 0;
    ucMncNum      = 2;


    PS_MEM_SET(aucImsiStr, 0x00, sizeof(aucImsiStr));

    /* Plmn号段信息，从第2个参数开始，每3个为一组，对应(MNClen,PlmnRangeBegin,PlmnRangeEnd) */
    for (i = 1; i < ulParaCnt; i++)
    {
        /* 当Plmninfo参数index是3n+1时,对应MNC的长度 */
        if (1 == (i % 3))
        {
            if (1 != pstParalist[i].usParaLen)
            {
                return AT_SIMLOCK_PLMN_MNC_LEN_ERR;
            }
            else if (('2' != pstParalist[i].aucPara[0])
                  && ('3' != pstParalist[i].aucPara[0]))
            {
                return AT_SIMLOCK_PLMN_MNC_LEN_ERR;
            }
            else
            {
                ucMncNum = pstParalist[i].aucPara[0] - '0';
                pstSimLockPlmnInfo->astSimLockPlmnRange[ulPlmnParaIdx].ucMncNum = ucMncNum;
            }

        }
        else if (2 == (i % 3))
        {
            /* 当Plmninfo参数index是3n+2 时,对应PlmnRangeBegin */
            if (AT_FAILURE == At_AsciiNum2SimLockImsiStr(aucImsiStr, pstParalist[i].aucPara,pstParalist[i].usParaLen))
            {
                return AT_ERROR;
            }
            else
            {
                ulImsiStrLen = (pstParalist[i].usParaLen + 1) / 2;
                pucPlmnRange = pstSimLockPlmnInfo->astSimLockPlmnRange[ulPlmnParaIdx].aucRangeBegin;
                PS_MEM_CPY(pucPlmnRange, aucImsiStr, (VOS_UINT16)ulImsiStrLen);
                PS_MEM_SET(pucPlmnRange + ulImsiStrLen, 0xFF,
                                (TAF_PH_SIMLOCK_PLMN_STR_LEN - ulImsiStrLen));
            }
        }
        else
        {
            /* 当Plmninfo参数index是3n+2 时,对应PlmnRangeEnd */
            if (AT_FAILURE == At_AsciiNum2SimLockImsiStr(aucImsiStr, pstParalist[i].aucPara,pstParalist[i].usParaLen))
            {
                return AT_ERROR;
            }
            else
            {
                ulImsiStrLen = (pstParalist[i].usParaLen + 1) / 2;
                pucPlmnRange = pstSimLockPlmnInfo->astSimLockPlmnRange[ulPlmnParaIdx].aucRangeEnd;
                PS_MEM_CPY(pucPlmnRange, aucImsiStr, (VOS_UINT16)ulImsiStrLen);
                PS_MEM_SET(pucPlmnRange + ulImsiStrLen, 0xFF,
                                (TAF_PH_SIMLOCK_PLMN_STR_LEN - ulImsiStrLen));
            }
            ++ ulPlmnParaIdx;
        }

    }

    return AT_OK;

}


VOS_UINT32 At_SetSimLockPlmnInfo(
    VOS_UINT8                           ucIndex,
    VOS_UINT32                          ulParaCnt,
    AT_PARSE_PARA_TYPE_STRU             *pstParalist
)
{
    VOS_UINT32                          ulRslt;
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU  stSimLockPlmnInfo;
    TAF_CUSTOM_CARDLOCK_STATUS_STRU     stCardLockStatus;
    VOS_UINT32                          i;
    VOS_UINT32                          ulPlmnInfoNum;
    /*TAF_CUSTOM_SIM_LOCK_PLMN_RANGE_STRU stDftPlmnInfoVal;*/

    if (VOS_TRUE == g_bAtDataLocked)
    {
        ulRslt = AT_DATA_UNLOCK_ERROR;
    }
    else if (ulParaCnt > ((TAF_MAX_SIM_LOCK_RANGE_NUM * 3) + 1))
    {
        ulRslt = AT_SIMLOCK_PLMN_NUM_ERR;
    }
    else if (((ulParaCnt - 1) % 3) != 0)
    {
        ulRslt = AT_ERROR;
    }
    else
    {
        ulRslt =  At_CovertAtParaToSimlockPlmnInfo(ulParaCnt, pstParalist, &stSimLockPlmnInfo);
    }

    if (AT_OK != ulRslt)
    {
        At_FormatResultData(ucIndex, ulRslt);
        return AT_ERROR;
    }
#if 0
    if (VOS_TRUE == DRV_SEC_CHECK())
    {
        At_FormatResultData(ucIndex, AT_OK);
        return AT_OK;
    }
#endif

    /* 获得PLMN 号段(MNClen,PlmnRangeBegin,PlmnRangeEnd)的个数 ，第2个参数开始是Plmn 信息 */
    ulPlmnInfoNum = (ulParaCnt - 1) / 3;

    /* 将剩余的PLMN号段设置为默认值 */
    for (i = ulPlmnInfoNum ; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++)
    {
        PS_MEM_SET(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin,
                                        AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                        TAF_PH_SIMLOCK_PLMN_STR_LEN);
        PS_MEM_SET(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeEnd,
                                        AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
                                        TAF_PH_SIMLOCK_PLMN_STR_LEN);
        stSimLockPlmnInfo.astSimLockPlmnRange[i].ucMncNum = AT_SIM_LOCK_MNC_NUM_DEFAULT_VAL;
    }

    stSimLockPlmnInfo.ulStatus = NV_ITEM_ACTIVE;


    PS_MEM_SET(&stCardLockStatus, 0x00, sizeof(stCardLockStatus));

    /* 获取当前解锁状态*/
    ulRslt = NV_ReadEx(MODEM_ID_0, en_NV_Item_CardlockStatus,
                      &stCardLockStatus,
                      sizeof(stCardLockStatus));

    if (NV_OK != ulRslt)
    {
        AT_WARN_LOG("At_SetSimLockPlmnInfo:read en_NV_Item_CardlockStatus Fail.");
        At_FormatResultData(ucIndex, AT_ERROR);
        return AT_ERROR;
    }

    /* 如果当前已经处于锁定状态，直接返回ERROR */
    if ((NV_ITEM_ACTIVE == stCardLockStatus.ulStatus)
     && (TAF_OPERATOR_UNLOCK_TIMES_MIN == stCardLockStatus.ulRemainUnlockTimes)
     && (TAF_OPERATOR_LOCK_LOCKED == stCardLockStatus.enCardlockStatus))
    {
        AT_WARN_LOG("At_SetSimLockPlmnInfo: is locked, operation is not allowed.");
        At_FormatResultData(ucIndex, AT_ERROR);
        return AT_ERROR;

    }

    /* 将en_NV_Item_CardlockStatus的状态置为激活，CardStatus 的内容设置为1,Remain Times保持不变 */
    stCardLockStatus.ulStatus            = NV_ITEM_ACTIVE;
    stCardLockStatus.enCardlockStatus    = TAF_OPERATOR_LOCK_NEED_UNLOCK_CODE;

    ulRslt = NV_WriteEx(MODEM_ID_0, en_NV_Item_CardlockStatus,
                        &stCardLockStatus,
                        sizeof(stCardLockStatus));
    if (NV_OK != ulRslt)
    {
        AT_WARN_LOG("At_SetSimLockPlmnInfo:write en_NV_Item_CardlockStatus Fail.");
        At_FormatResultData(ucIndex, AT_ERROR);
        return AT_ERROR;
    }


    /*向C核发送消息备份simlockNV*/
    if (TAF_SUCCESS != AT_FillAndSndAppReqMsg(0,
                                              0,
                                              DRV_AGENT_SIMLOCK_NV_SET_REQ,
                                              &stCardLockStatus,
                                              sizeof(stCardLockStatus),
                                              I0_WUEPS_PID_DRV_AGENT))
    {

        AT_WARN_LOG("At_SetSimLockPlmnInfo():DRV_AGENT_SIMLOCK_NV_SET_REQ NV Write Fail!");
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_ERR;
    }


    /* 将PLMN信息写入到NV项en_NV_Item_CustomizeSimLockPlmnInfo */

    ulRslt = NV_WriteEx(MODEM_ID_0, en_NV_Item_CustomizeSimLockPlmnInfo,
                        &stSimLockPlmnInfo,
                        sizeof(stSimLockPlmnInfo));
    if (NV_OK != ulRslt)
    {
        AT_WARN_LOG("At_SetSimLockPlmnInfo:write en_NV_Item_CustomizeSimLockPlmnInfo Fail.");
        At_FormatResultData(ucIndex, AT_ERROR);
        return AT_ERROR;
    }

    At_FormatResultData(ucIndex, AT_OK);
    return AT_OK;
}


VOS_UINT32 At_SetMaxLockTimes(VOS_UINT8 ucIndex)
{
    TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU          stSimLockMaxTimes;

    /* 设置参数为空 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 参数个数过多 */
    if (gucAtParaIndex != 1)
    {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 检查是否解除数据保护,未解除时返回出错信息 */
    if (VOS_TRUE == g_bAtDataLocked)
    {
        return  AT_DATA_UNLOCK_ERROR;
    }

    stSimLockMaxTimes.ulStatus       = NV_ITEM_ACTIVE;
    stSimLockMaxTimes.ulLockMaxTimes = gastAtParaList[0].ulParaValue;


    /* 向C核发送消息设置SIMLOCK的最大次数 */
    if (TAF_SUCCESS != AT_FillAndSndAppReqMsg(0,
                                              0,
                                              DRV_AGENT_MAX_LOCK_TIMES_SET_REQ,
                                              &stSimLockMaxTimes,
                                              sizeof(stSimLockMaxTimes),
                                              I0_WUEPS_PID_DRV_AGENT))
    {

        AT_WARN_LOG("At_SetMaxLockTimes():DRV_AGENT_SIMLOCK_NV_SET_REQ NV Write Fail!");
        return AT_DEVICE_OTHER_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MAXLCKTMS_SET;
    return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */


}


VOS_UINT32 AT_SetVertime ( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 发送消息DRV_AGENT_VERTIME_QRY_REQ给AT代理处理 */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_VERTIME_QRY_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if(AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VERSIONTIME_READ;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

#if ( VOS_WIN32 == VOS_OS_VER )

/* Modified by c00318887 for DPlmn扩容和优先接入HPLMN, 2015-5-18, begin */

VOS_UINT32 AT_ResetNplmn ( VOS_UINT8 ucIndex )
{
#ifdef DMT
    NAS_STUB_AT_ResetNplmn();
#endif
    return AT_OK;
}
/* Modified by c00318887 for DPlmn扩容和优先接入HPLMN, 2015-5-18, end */

VOS_UINT32 AT_SetNvimPara ( VOS_UINT8 ucIndex )
{
    NV_ID_ENUM_U16                      eNvimTempId;
    VOS_UINT32                          ulTempLen;
    VOS_UINT32                          ulRst;
    VOS_UINT8*                          pNvTemp;
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRet;

    enModemId = MODEM_ID_0;
    ulTempLen = 0;

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);
    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("At_SetCgsnPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* 参数不正确*/
    if (!(((0 == gastAtParaList[0].ulParaValue) && (2 == gucAtParaIndex))
       || ((1 == gastAtParaList[0].ulParaValue) && (3 == gucAtParaIndex))))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*第二个参数为NV项ID*/
    eNvimTempId = (NV_ID_ENUM_U16)gastAtParaList[1].ulParaValue;

    /*第一个参数为0表示读NV项值，为1表示写NV项*/
    if (0 == gastAtParaList[0].ulParaValue)
    {
        NV_GetLength(eNvimTempId, &ulTempLen);
        pNvTemp = (VOS_UINT8 *)PS_MEM_ALLOC(I0_WUEPS_PID_TAF, ulTempLen);

        if (VOS_NULL_PTR == pNvTemp)
        {
            AT_ERR_LOG("AT_SetNvimPara:ERROR:ALLOC MEMORY FAIL.");
            return AT_ERROR;
        }
        ulRst = AT_ReadValueFromNvim(eNvimTempId, pNvTemp, ulTempLen, enModemId);
        if (AT_OK == ulRst )
        {
            usLength = 0;
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"^NVSTUB:");
            usLength += (TAF_UINT16)At_HexString2AsciiNumPrint(AT_CMD_MAX_LEN,
                                                                 (TAF_INT8 *)pgucAtSndCodeAddr,
                                                                 pgucAtSndCodeAddr + usLength,
                                                                 pNvTemp,
                                                                 (VOS_UINT16)ulTempLen);

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
            ulRst = AT_SUCCESS;
        }

        /* 增加内存释放 */
        PS_MEM_FREE(I0_WUEPS_PID_TAF, pNvTemp);
        return ulRst;
    }
    else
    {
        if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[2].aucPara,
                                              &gastAtParaList[2].usParaLen))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (NV_OK !=  NV_WriteEx(enModemId, eNvimTempId, gastAtParaList[2].aucPara, gastAtParaList[2].usParaLen))
        {
             AT_WARN_LOG("AT_SetNvimPara():WARNING:Invoke NVIM Write function falied");
             return AT_ERROR;
        }

#ifdef DMT
        NAS_STUB_UpdateGlobalVar(eNvimTempId);
#endif
        return AT_OK;
    }
}
VOS_UINT32 AT_SetPidReinitPara ( VOS_UINT8 ucIndex )
{
#ifdef DMT
    VOS_UINT8                           ucValue;

    ucValue = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    NAS_STUB_PidReinit(ucValue);
#endif
    return AT_OK;
}

TAF_UINT32 AT_SetUSIMPara(TAF_UINT8 ucIndex)
{
    USIMM_STK_COMMAND_DETAILS_STRU      stCMDDetail;
    VOS_UINT8                           ucRecordNum;
    USIMM_SET_FILE_INFO_STRU            stSetFileInfo;
    USIMM_RESET_INFO_STRU               stRstInfo;

    ucRecordNum = 0;

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara, &gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (3 == gucAtParaIndex)
    {
        ucRecordNum = gastAtParaList[2].ulParaValue;
    }

    /* 当读到AT&T & DCM的定制需求更新0x4F36和0x4F34文件时，要设置enAppType */
    if ((USIMM_ATT_RAT_BALANCING_FID == gastAtParaList[0].ulParaValue)
     || (USIMM_ATT_ACTING_PLMN_FID == gastAtParaList[0].ulParaValue))
    {
        stSetFileInfo.enAppType     = USIMM_ATT_APP;
        stSetFileInfo.usEfId        = (VOS_UINT16)gastAtParaList[0].ulParaValue;
        stSetFileInfo.ucRecordNum   = ucRecordNum;
        stSetFileInfo.ulEfLen       = gastAtParaList[1].usParaLen;
        stSetFileInfo.pucEfContent  = gastAtParaList[1].aucPara;
        stSetFileInfo.ucRsv         = 0;

        USIMM_SetFileReq(WUEPS_PID_AT, 0, &stSetFileInfo);
    }
    else
    {
        stSetFileInfo.enAppType     = USIMM_UNLIMIT_APP;
        stSetFileInfo.usEfId        = (VOS_UINT16)gastAtParaList[0].ulParaValue;
        stSetFileInfo.ucRecordNum   = ucRecordNum;
        stSetFileInfo.ulEfLen       = gastAtParaList[1].usParaLen;
        stSetFileInfo.pucEfContent  = gastAtParaList[1].aucPara;
        stSetFileInfo.ucRsv         = 0;

        USIMM_SetFileReq(WUEPS_PID_AT, 0, &stSetFileInfo);
    }

    if (0x6F07 == gastAtParaList[0].ulParaValue)
    {
        stCMDDetail.ucCommandType = USIMM_RESET;
        VOS_MemSet(&stRstInfo, 0x0, sizeof(stRstInfo));

        USIMM_ResetReq(WUEPS_PID_AT, &stCMDDetail, &stRstInfo);
    }


#ifdef DMT
    NAS_STUB_ReadUsimPara();
#endif

    return AT_OK;
}
VOS_UINT32 AT_SetSTKParaStub(VOS_UINT8 ucIndex)
{
    USIMM_STK_COMMAND_DETAILS_STRU      stCMDDetail;
    STK_NAS_STEERING_OF_ROAMING_STRU   *pstSteerOfRoamMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    /* 申请内存  */
    pstSteerOfRoamMsg = (STK_NAS_STEERING_OF_ROAMING_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                                               sizeof(STK_NAS_STEERING_OF_ROAMING_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstSteerOfRoamMsg )
    {
        /* 内存申请失败 */
        AT_ERR_LOG("AT_SetSTKParaStub:ERROR: Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara, &gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 模拟USIM给MMC模块发送消息 */
    pstSteerOfRoamMsg->ulMsgName  = STK_NAS_STEERING_OF_ROAMING_IND;
    pstSteerOfRoamMsg->usOplmnListLen = 0;

    /* 填写相关参数 */
    pstSteerOfRoamMsg->ulSenderPid          = MAPS_STK_PID;
    pstSteerOfRoamMsg->ulReceiverPid        = AT_GetDestPid(ucIndex, I0_WUEPS_PID_MMC);
    pstSteerOfRoamMsg->ulLength             = sizeof(STK_NAS_STEERING_OF_ROAMING_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 调用VOS发送原语 */
    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstSteerOfRoamMsg);
    if ( VOS_OK != ulRslt )
    {
        AT_ERR_LOG("AT_SetSTKParaStub:ERROR:PS_SEND_MSG ");
        return VOS_ERR;
    }

    return AT_OK;
}
VOS_UINT32 AT_RefreshUSIMPara(VOS_UINT8 ucIndex)
{

    USIMM_STK_COMMAND_DETAILS_STRU      stCMDDetail;
    PS_USIM_REFRESH_IND_STRU           *pstMmcUsimRefreshMsg;
    PS_USIM_REFRESH_IND_STRU           *pstMmaUsimRefreshMsg;
    VOS_UINT32                          ulRslt;

    /* 申请内存  */
    pstMmcUsimRefreshMsg = (PS_USIM_REFRESH_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                                               sizeof(PS_USIM_REFRESH_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMmcUsimRefreshMsg )
    {
        /* 内存申请失败 */
        AT_ERR_LOG("AT_RefreshUSIMPara:ERROR: Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    /* 申请内存  */
    pstMmaUsimRefreshMsg = (PS_USIM_REFRESH_IND_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT,
                                               sizeof(PS_USIM_REFRESH_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMmaUsimRefreshMsg )
    {
        /* 内存申请失败 */
        AT_ERR_LOG("AT_RefreshUSIMPara:ERROR: Memory Alloc Error for pstMsg");
        PS_MEM_FREE(WUEPS_PID_AT, pstMmcUsimRefreshMsg);
        return VOS_ERR;
    }


    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara, &gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 模拟USIM给MMC模块发送消息 */
    pstMmcUsimRefreshMsg->ulMsgName            = PS_USIM_REFRESH_IND;
    pstMmcUsimRefreshMsg->astEfId[0].usFileId  = gastAtParaList[0].ulParaValue;
    pstMmcUsimRefreshMsg->usEfNum              = 1;
    pstMmcUsimRefreshMsg->enRefreshType        = USIMM_REFRESH_FILE_LIST;

    /* 填写相关参数 */
    pstMmcUsimRefreshMsg->ulSenderPid          = MAPS_PIH_PID;
    pstMmcUsimRefreshMsg->ulReceiverPid        = AT_GetDestPid(ucIndex, I0_WUEPS_PID_MMC);
    pstMmcUsimRefreshMsg->ulLength             = sizeof(PS_USIM_REFRESH_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 调用VOS发送原语 */
    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMmcUsimRefreshMsg);

    if ( VOS_OK != ulRslt )
    {
        AT_ERR_LOG("AT_RefreshUSIMPara:ERROR:PS_SEND_MSG ");
        return VOS_ERR;
    }

    /* 模拟USIM给MMA模块发送消息 */
    pstMmaUsimRefreshMsg->ulMsgName            = PS_USIM_REFRESH_IND;
    pstMmaUsimRefreshMsg->astEfId[0].usFileId  = gastAtParaList[0].ulParaValue;
    pstMmaUsimRefreshMsg->usEfNum              = 1;
    pstMmaUsimRefreshMsg->enRefreshType        = USIMM_REFRESH_FILE_LIST;

    /* 填写相关参数 */
    pstMmaUsimRefreshMsg->ulSenderPid          = MAPS_PIH_PID;
    pstMmaUsimRefreshMsg->ulReceiverPid        = AT_GetDestPid(ucIndex, I0_WUEPS_PID_MMA);
    pstMmaUsimRefreshMsg->ulLength             = sizeof(PS_USIM_REFRESH_IND_STRU) - VOS_MSG_HEAD_LENGTH;


    /* 调用VOS发送原语 */
    ulRslt = PS_SEND_MSG(WUEPS_PID_AT, pstMmaUsimRefreshMsg);

    if ( VOS_OK != ulRslt )
    {
        AT_ERR_LOG("AT_RefreshUSIMPara:ERROR:PS_SEND_MSG ");
        return VOS_ERR;
    }

    return AT_OK;
}
#endif


/* begin V7R1 PhaseI Modify */

VOS_UINT32 At_SetSystemInfoEx(TAF_UINT8 ucIndex)
{
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   TAF_PH_SYSINFO_EX_VALUE_PARA,
                                   TAF_NULL_PTR))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SYSINFO_READ;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
/* end V7R1 PhaseI Modify */


VOS_UINT32 AT_SetGodloadPara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRet;

    /* 参数检查 */
    if (0 != gucAtParaIndex)
    {
        AT_WARN_LOG("AT_SetGodloadPara: too many parameters.");
        return AT_ERROR;
    }

    /* 发消息到C核指示使单板进入下载模式 */
    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_GODLOAD_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("AT_SetApbatlvlPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_GODLOAD_SET;
    return AT_WAIT_ASYNC_RETURN;

}
VOS_UINT32 At_SetResetPara( VOS_UINT8 ucIndex  )
{
    VOS_UINT8                           ucUpdateFlag;
    VOS_UINT32                          ulRslt;

    if (0 != gucAtParaIndex)
    {
        AT_WARN_LOG("At_SetResetPara: too many parameters.");
        return AT_ERROR;
    }

    /* 先返回OK */
    At_FormatResultData(ucIndex,AT_OK);

    /* B31版本升级工具切换后，下发AT^RESET命令后来不及把OK回复给PC，需要增加延时，延时500ms */
    VOS_TaskDelay(500);

    /* 发送消息到C核，重启 */
    ucUpdateFlag = VOS_FALSE;

    ulRslt = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                    gastAtClientTab[ucIndex].opId,
                                    DRV_AGENT_TMODE_SET_REQ,
                                    &ucUpdateFlag,
                                    sizeof(ucUpdateFlag),
                                    I0_WUEPS_PID_DRV_AGENT);
    if (VOS_OK != ulRslt)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_SetNvBackUpPara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    if (0 != gucAtParaIndex)
    {
        AT_WARN_LOG("AT_SetNvBackUpPara: too many parameters.");
        return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_NVBACKUP_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NVBACKUP_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetNvManufactureExtPara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    if (0 != gucAtParaIndex)
    {
        AT_WARN_LOG("AT_SetNvManufactureExtPara: too many parameters.");
        return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_NVMANUFACTUREEXT_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NVMANUFACTUREEXT_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 At_SetNvRestorePara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                 ulRst;

    if (0 != gucAtParaIndex)
    {
        AT_WARN_LOG("At_SetNvRestorePara: too many parameters.");
        return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_NVRESTORE_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NVRESTORE_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}


VOS_UINT32  AT_SetNvRststtsPara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulRst;

    if (0 != gucAtParaIndex)
    {
        AT_WARN_LOG("AT_SetNvRststtsPara: too many parameters.");
        return AT_ERROR;
    }

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   DRV_AGENT_NVRSTSTTS_QRY_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NVRSTSTTS_READ;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetWiFiEnablePara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    ulRst = VOS_OK;

    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].ulParaValue )
    {
        /*关闭WIFI*/
        ulRst = (VOS_UINT32)WIFI_POWER_SHUTDOWN();
    }
    else if (1 == gastAtParaList[0].ulParaValue )
    {
        /* 产品线API未提供 */
        #if 0
            WiFiReset();
        #endif
    }
    else
    {
#if(FEATURE_ON == FEATURE_LTE)

        VOS_TaskDelay(5500);    /* 按底软要求需要5.5s秒延迟 */

#endif

        if ( VOS_OK != WIFI_POWER_SHUTDOWN())
        {
            return AT_ERROR;
        }

#if(FEATURE_ON == FEATURE_LTE)

        VOS_TaskDelay(1000);    /* 按底软要求需要1秒延迟 */

#endif


        /*测试模式打开WIFI*/
        ulRst = (VOS_UINT32)WIFI_POWER_START();
    }

    if (VOS_OK != ulRst)
    {
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetWiFiModePara(VOS_UINT8 ucIndex)
{
    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }

    WIFI_TEST_CMD("wl down");

    if (AT_WIFI_SUPPORT_MODE_B == gastAtParaList[0].ulParaValue)
    {
        WIFI_TEST_CMD("wl nmode 0");
        WIFI_TEST_CMD("wl gmode 0");
    }
    else if (AT_WIFI_SUPPORT_MODE_G == gastAtParaList[0].ulParaValue)
    {
        WIFI_TEST_CMD("wl nmode 0");
        WIFI_TEST_CMD("wl gmode 2");
    }
    else
    {
        WIFI_TEST_CMD("wl nmode 1");
        WIFI_TEST_CMD("wl gmode 1");
    }

    WIFI_TEST_CMD("wl up");

    /*保存全局变量里，已备查询*/
    g_ulWifiMode = gastAtParaList[0].ulParaValue;

    return AT_OK;
}
/*****************************************************************************
 函 数 名  : AT_SetWiFiBandPara
 功能描述  : 设置Wifi的带宽
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月26日
    作    者   : 王毛
    修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT32 AT_SetWiFiBandPara(VOS_UINT8 ucIndex)
{

    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /*目前Wifi只支持20M，40M返回ERROR*/
    if(AT_WIFI_BAND_20M != gastAtParaList[0].ulParaValue)
    {
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetWiFiFreqPara(VOS_UINT8 ucIndex)
{
    VOS_CHAR                            acCmd[200]={0};
    VOS_UINT32                          ulWifiFreq;
    VOS_UINT32                          i;
    VOS_UINT32                          aulChannel[] = {2412,2417,2422,2427,2432,
                                                        2437,2442,2447,2452,2457,
                                                        2462,2467,2472,2484};

    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /* 计算信道  */
    ulWifiFreq = 0;
    for (i=0; i<(sizeof(aulChannel)/sizeof(VOS_UINT32)); i++)
    {
        if (gastAtParaList[0].ulParaValue == aulChannel[i])
        {
            ulWifiFreq = (i+1);
            break;
        }
    }

    if (0 == ulWifiFreq)
    {
        AT_WARN_LOG("AT_SetWiFiFreqPara: freq ERROR");
        return AT_ERROR;
    }

    /* 调用底软接口 */
    WIFI_TEST_CMD("wl down");

    VOS_sprintf(acCmd, "wl channel %d", ulWifiFreq);
    WIFI_TEST_CMD(acCmd);

    WIFI_TEST_CMD("wl up");

    /* 保存全局变量里，已备查询 */
    g_ulWifiFreq = ulWifiFreq;

    return AT_OK;
}


VOS_UINT32 AT_TransferWifiRate(
    VOS_UINT32                          ulInRate,
    VOS_UINT32                         *pulOutRate
)
{
    VOS_UINT8                           ucIndex;
    /* WIFI n模式 AT^WIDATARATE设置的速率值和WL命令速率值的对应表 */
    VOS_UINT32                          aucAtWifiNRate_Table[AT_WIFI_N_RATE_NUM] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500};

    for (ucIndex = 0; ucIndex < AT_WIFI_N_RATE_NUM; ucIndex++)
    {
        if (aucAtWifiNRate_Table[ucIndex] == ulInRate)
        {
            *pulOutRate = ucIndex;
            break;
        }
    }

    if (ucIndex >= AT_WIFI_N_RATE_NUM)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 AT_SetWiFiRatePara(VOS_UINT8 ucIndex)
{
    VOS_CHAR                            acCmd[200]={0};
    VOS_UINT32                          ulWifiRate;

#if(FEATURE_ON == FEATURE_LTE)
#else
    VOS_UINT32                          ulNRate;
    VOS_UINT32                          ulRslt;
#endif
    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    ulWifiRate = gastAtParaList[0].ulParaValue/100;


    if (AT_WIFI_SUPPORT_MODE_N == g_ulWifiMode)         /*11 n*/
    {
#if(FEATURE_ON == FEATURE_LTE)
        VOS_sprintf(acCmd, "wl nrate -m %d", 7);
#else
        ulRslt = AT_TransferWifiRate(gastAtParaList[0].ulParaValue, &ulNRate);

        if (VOS_OK == ulRslt)
        {
            VOS_sprintf(acCmd, "wl nrate -m %d", ulNRate);
        }
        else
        {
            return AT_ERROR;
        }
#endif
    }
    else if (AT_WIFI_SUPPORT_MODE_G == g_ulWifiMode )    /*11 g*/
    {
#if(FEATURE_ON == FEATURE_LTE)
        VOS_sprintf(acCmd, "wl bg_rate %d", ulWifiRate);
#else
        VOS_sprintf(acCmd, "wl rate %d", ulWifiRate);
#endif
    }
    else if(AT_WIFI_SUPPORT_MODE_B == g_ulWifiMode)    /*11 b*/
    {
#if(FEATURE_ON == FEATURE_LTE)
        VOS_sprintf(acCmd, "wl bg_rate %d", ulWifiRate);
#else
        VOS_sprintf(acCmd, "wl rate %d", ulWifiRate);
#endif
    }
    else
    {
        AT_WARN_LOG("AT_SetWiFiRatePara: wifimode ERROR");
        return AT_ERROR;
    }

    WIFI_TEST_CMD(acCmd);


    /*保存全局变量里，已备查询*/
    g_ulWifiRate = gastAtParaList[0].ulParaValue;

    return AT_OK;
}

VOS_UINT32 AT_SetWiFiPowerPara(VOS_UINT8 ucIndex)
{
    VOS_CHAR                            acCmd[200]={0};
    VOS_INT32                           lWifiPower;

    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }

    g_lWifiPower = (VOS_INT32)gastAtParaList[0].ulParaValue;

    lWifiPower = g_lWifiPower/100;

    if (lWifiPower < AT_WIFI_POWER_MIN)
    {
        lWifiPower = AT_WIFI_POWER_MIN;
    }

    if (lWifiPower > AT_WIFI_POWER_MAX)
    {
        lWifiPower = AT_WIFI_POWER_MAX;
    }

    VOS_sprintf(acCmd, "wl txpwr1 -o -d %d", lWifiPower);

    /*调用底软接口*/
    WIFI_TEST_CMD(acCmd);


    return AT_OK;
}



VOS_UINT32 AT_SetWiFiTxPara(VOS_UINT8 ucIndex)
{
#if(FEATURE_ON == FEATURE_LTE)
    VOS_CHAR                            acCmd[200]={0};
#endif

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }
#if(FEATURE_ON == FEATURE_LTE)
    if(g_ulWifiMode == AT_WIFI_SUPPORT_MODE_CW)
    {
        if (AT_WIFI_TX_OFF == gastAtParaList[0].ulParaValue)
        {
            /* WIFI_TEST_CMD("wl down"); */
            WIFI_TEST_CMD("wl fqacurcy 0");
            /* WIFI_TEST_CMD("wl up"); */
        }
        else
        {
            WIFI_TEST_CMD("wl up");
            WIFI_TEST_CMD("wl band b");
            WIFI_TEST_CMD("wl out");
            WIFI_TEST_CMD("wl phy_txpwrctrl 0");

            if(0 == g_ulWifiRF)
            {
                WIFI_TEST_CMD("wl phy_txpwrindex 0 127");
            }
            else
            {
                WIFI_TEST_CMD("wl phy_txpwrindex 127 0");
            }

            WIFI_TEST_CMD("wl phy_txpwrctrl 1");

            VOS_sprintf(acCmd, "wl fqacurcy %d", g_ulWifiFreq);

            WIFI_TEST_CMD(acCmd);

        }
    }
    else
    {
#endif
    /* 参数为零关闭发射机，参数为1打开发射机 */
    if (AT_WIFI_TX_OFF == gastAtParaList[0].ulParaValue)
    {
         WIFI_TEST_CMD("wl pkteng_stop tx");
    }
    else
    {
        /* 调用底软接口 */
        WIFI_TEST_CMD("wl down");

        WIFI_TEST_CMD("wl mpc 0");

        WIFI_TEST_CMD("wl country ALL");

        WIFI_TEST_CMD("wl frameburst 1");

        WIFI_TEST_CMD("wl scansuppress 1");

        WIFI_TEST_CMD("wl up");

        WIFI_TEST_CMD("wl pkteng_start 00:11:22:33:44:55 tx 100 1500 0");

#if(FEATURE_ON == FEATURE_LTE)
        WIFI_TEST_CMD("wl phy_forcecal 1");
#endif
    }
#if(FEATURE_ON == FEATURE_LTE)
    }
#endif
    return AT_OK;
}

VOS_UINT32 AT_SetWiFiRxPara(VOS_UINT8 ucIndex)
{
    VOS_CHAR                            acCmd[200]={0};

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /* 参数过多*/
    if (gucAtParaIndex > 3)
    {
        return  AT_TOO_MANY_PARA;
    }

    /*参数为零关闭接收机，参数为1打开接收机*/
    if ( AT_WIFI_RX_OFF == gastAtParaList[0].ulParaValue)
    {
        /* 调用驱动关闭接收机 */
        WIFI_TEST_CMD("wl pkteng_stop rx");
    }
    else
    {
        WIFI_TEST_CMD("wl down");

        WIFI_TEST_CMD("wl mpc 0");

        WIFI_TEST_CMD("wl country ALL");

        WIFI_TEST_CMD("wl frameburst 1");

        WIFI_TEST_CMD("wl scansuppress 1");


        WIFI_TEST_CMD("wl up");

        VOS_sprintf(acCmd, "wl pkteng_start %c%c:%c%c:%c%c:%c%c:%c%c:%c%c rx async 0 0",
                    gastAtParaList[1].aucPara[0],gastAtParaList[1].aucPara[1],
                    gastAtParaList[1].aucPara[2],gastAtParaList[1].aucPara[3],
                    gastAtParaList[1].aucPara[4],gastAtParaList[1].aucPara[5],
                    gastAtParaList[1].aucPara[6],gastAtParaList[1].aucPara[7],
                    gastAtParaList[1].aucPara[8],gastAtParaList[1].aucPara[9],
                    gastAtParaList[1].aucPara[10],gastAtParaList[1].aucPara[11]);

        WIFI_TEST_CMD(acCmd);

        WIFI_GET_RX_PACKET_REPORT(&g_ulUcastWifiRxPkts,  &g_ulMcastWifiRxPkts);


    }

    return AT_OK;
}
VOS_UINT32 AT_SetWiFiPacketPara(VOS_UINT8 ucIndex)
{
    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }
    /* 参数过多*/
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }


    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    /* TODO: */
    /* 目前暂用该方式清零 */
    WIFI_GET_RX_PACKET_REPORT(&g_ulUcastWifiRxPkts,  &g_ulMcastWifiRxPkts);


    return AT_OK;
}


VOS_UINT32 AT_SetWiFiLogPara(VOS_UINT8 ucIndex)
{
    TAF_AT_MULTI_WIFI_SSID_STRU               stWifiSsid;
    TAF_AT_MULTI_WIFI_SEC_STRU                stWifiKey;

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /* 参数过多*/
    if (gucAtParaIndex > 2)
    {
        return  AT_TOO_MANY_PARA;
    }

    PS_MEM_SET(&stWifiKey, 0, sizeof(stWifiKey));
    PS_MEM_SET(&stWifiSsid, 0, sizeof(stWifiSsid));

    /*设置WIFI SSID*/
    if (0 == gastAtParaList[0].ulParaValue)
    {
        /*参数长度过长*/
        if (gastAtParaList[1].usParaLen >= AT_WIFI_SSID_LEN_MAX)
        {
            return  AT_CME_INCORRECT_PARAMETERS;
        }

        /*读取WIFI KEY对应的NV项*/
        if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_STATUS_SSID, &stWifiSsid, sizeof(TAF_AT_MULTI_WIFI_SSID_STRU)))
        {
            AT_WARN_LOG("AT_SetWiFiSsidPara:READ NV ERROR");
            return AT_ERROR;
        }
        else
        {
            PS_MEM_SET(stWifiSsid.aucWifiSsid[0], 0x00, AT_WIFI_SSID_LEN_MAX);

            PS_MEM_CPY(stWifiSsid.aucWifiSsid[0], gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);

            stWifiSsid.aucWifiSsid[0][gastAtParaList[1].usParaLen] = '\0';

            /*写入WIFI SSID对应的NV项*/
            if (VOS_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_STATUS_SSID, &stWifiSsid, sizeof(TAF_AT_MULTI_WIFI_SSID_STRU)))
            {
                AT_WARN_LOG("AT_SetWiFiSsidPara:WRITE NV ERROR");
                return AT_ERROR;
            }
        }
    }
    else
    {
        /*读取WIFI KEY对应的NV项*/
        if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_KEY, &stWifiKey, sizeof(TAF_AT_MULTI_WIFI_SEC_STRU)))
        {
            AT_WARN_LOG("AT_SetWiFiKeyPara:READ NV ERROR");
            return AT_ERROR;
        }
        else
        {

            /*参数长度过长*/
            if (gastAtParaList[1].usParaLen >= AT_WIFI_KEY_LEN_MAX)
            {
                return  AT_CME_INCORRECT_PARAMETERS;
            }

            /*根据index写入对应的KEY*/
            switch(stWifiKey.ucWifiWepKeyIndex[0])
            {
                case 0:
                    VOS_MemCpy(stWifiKey.aucWifiWepKey1[0], gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
                    stWifiKey.aucWifiWepKey1[0][gastAtParaList[1].usParaLen] = '\0';
                    break;

                case 1:
                    VOS_MemCpy(stWifiKey.aucWifiWepKey2[0], gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
                    stWifiKey.aucWifiWepKey2[0][gastAtParaList[1].usParaLen] = '\0';
                    break;

                case 2:
                    VOS_MemCpy(stWifiKey.aucWifiWepKey3[0], gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
                    stWifiKey.aucWifiWepKey3[0][gastAtParaList[1].usParaLen] = '\0';
                    break;

                case 3:
                    VOS_MemCpy(stWifiKey.aucWifiWepKey4[0], gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
                    stWifiKey.aucWifiWepKey4[0][gastAtParaList[1].usParaLen] = '\0';
                    break;

                default:
                    break;
            }

            if (VOS_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_KEY, &stWifiKey, sizeof(TAF_AT_MULTI_WIFI_SEC_STRU)))
            {
                AT_WARN_LOG("AT_SetWiFiKeyPara:WRITE NV ERROR");
                return AT_ERROR;
            }
        }
    }

    return AT_OK;
}


VOS_UINT32 AT_SetWiFiSsidPara(VOS_UINT8 ucIndex)
{
    TAF_AT_MULTI_WIFI_SSID_STRU         stWifiSsid;
    VOS_UINT8                           ucGroup;

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /* 参数过多*/
    if (gucAtParaIndex > 2)
    {
        return  AT_TOO_MANY_PARA;
    }

    /*参数长度过长*/
    if (gastAtParaList[1].usParaLen >= AT_WIFI_SSID_LEN_MAX)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    /* SSID最多4组 */
    if (gastAtParaList[0].ulParaValue >= AT_WIFI_MAX_SSID_NUM)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    if (VOS_TRUE == g_bAtDataLocked)
    {
        return AT_ERROR;
    }

    PS_MEM_SET(&stWifiSsid, 0, sizeof(stWifiSsid));

    ucGroup = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /*读取WIFI KEY对应的NV项*/
    if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_STATUS_SSID,&stWifiSsid, sizeof(TAF_AT_MULTI_WIFI_SSID_STRU)))
    {
        AT_WARN_LOG("AT_SetWiFiSsidPara:READ NV ERROR");
        return AT_ERROR;
    }
    else
    {
        PS_MEM_CPY(&(stWifiSsid.aucWifiSsid[ucGroup][0]), gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
        stWifiSsid.aucWifiSsid[ucGroup][gastAtParaList[1].usParaLen] = '\0';

        /*写入WIFI SSID对应的NV项*/
        if (VOS_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_STATUS_SSID,&stWifiSsid, sizeof(TAF_AT_MULTI_WIFI_SSID_STRU)))
        {
            AT_WARN_LOG("AT_SetWiFiSsidPara:WRITE NV ERROR");
            return AT_ERROR;
        }
    }

    return AT_OK;
}
VOS_UINT32 AT_SetWiFiKeyPara(VOS_UINT8 ucIndex)
{
    TAF_AT_MULTI_WIFI_SEC_STRU          stWifiKey;
    VOS_UINT8                           ucGroup;

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return  AT_TOO_MANY_PARA;
    }

    /* 参数长度过长 */
    if (gastAtParaList[1].usParaLen > AT_WIFI_WLWPAPSK_LEN)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    if (VOS_TRUE == g_bAtDataLocked)
    {
        return AT_ERROR;
    }

    /* 做多4组SSID */
    if (gastAtParaList[0].ulParaValue >= AT_WIFI_MAX_SSID_NUM)
    {
       return  AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stWifiKey, 0, sizeof(stWifiKey));

    ucGroup = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 读取WIFI KEY对应的NV项 */
    if (NV_OK!= NV_ReadEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_KEY,&stWifiKey, sizeof(TAF_AT_MULTI_WIFI_SEC_STRU)))
    {
        AT_WARN_LOG("AT_SetWiFiKeyPara:READ NV ERROR");
        return AT_ERROR;
    }
    else
    {
        /* 写入KEY */
        PS_MEM_CPY(&(stWifiKey.aucWifiWpapsk[ucGroup][0]), gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);
        stWifiKey.aucWifiWpapsk[ucGroup][gastAtParaList[1].usParaLen] = '\0';


        if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_MULTI_WIFI_KEY, &stWifiKey, sizeof(TAF_AT_MULTI_WIFI_SEC_STRU)))
        {
            AT_WARN_LOG("AT_SetWiFiKeyPara:WRITE NV ERROR");
            return AT_ERROR;
        }
    }


    return AT_OK;
}


VOS_VOID AT_PrintWifiChannelInfo(
    AT_WIFI_INFO_STRU                  *pstWifiInfo,
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT8                           ucLoopIndex;
    VOS_UINT16                          usLength;

    /* 初始化 */
    ucLoopIndex                         = 0;
    usLength                            = gstAtSendData.usBufLen;


    /* 单板支持802.11b制式, 输出802.11b制式支持的信道号 */
    if (VOS_TRUE == pstWifiInfo->bitOpbSupport)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:%s",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           "b");
        for (ucLoopIndex = pstWifiInfo->usbLowChannel; ucLoopIndex <= pstWifiInfo->usbHighChannel; ucLoopIndex++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",%d",
                                               ucLoopIndex);
        }
        if ( (VOS_TRUE == pstWifiInfo->bitOpgSupport)
          || (VOS_TRUE == pstWifiInfo->bitOpnSupport) )
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",gaucAtCrLf);
        }
    }

    /* 单板支持802.11g制式, 输出802.11g制式支持的信道号 */
    if (VOS_TRUE == pstWifiInfo->bitOpgSupport)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:%s",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           "g");
        for (ucLoopIndex = pstWifiInfo->usgLowChannel; ucLoopIndex <= pstWifiInfo->usgHighChannel; ucLoopIndex++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",%d",
                                               ucLoopIndex);
        }
        if (VOS_TRUE == pstWifiInfo->bitOpnSupport)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",gaucAtCrLf);
        }
    }

    /* 单板支持802.11n制式, 输出802.11n制式支持的信道号 */
    if (VOS_TRUE == pstWifiInfo->bitOpnSupport)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:%s",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           "n");
        for (ucLoopIndex = pstWifiInfo->usnLowChannel; ucLoopIndex <= pstWifiInfo->usnHighChannel; ucLoopIndex++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",%d",
                                               ucLoopIndex);
        }
    }

    gstAtSendData.usBufLen += usLength;
    return;
}


VOS_VOID AT_PrintWifibPowerInfo(
    AT_WIFI_INFO_STRU                  *pstWifiInfo,
    VOS_UINT8                           ucWifiMode,
    VOS_UINT8                           ucIndex
)
{

    /* 输出802.11b制式期望功率 */
    gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                       "%s:%s",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       "b");
    if ( (AT_WIFI_MODE_ONLY_PA == ucWifiMode)
      || (AT_WIFI_MODE_PA_NOPA == ucWifiMode) )
    {
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                           ",%d",
                                           pstWifiInfo->ausbPower[0]);
    }
    if ( (AT_WIFI_MODE_ONLY_NOPA == ucWifiMode)
      || (AT_WIFI_MODE_PA_NOPA == ucWifiMode) )
    {
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                           ",%d",
                                           pstWifiInfo->ausbPower[1]);
    }
}
VOS_VOID AT_PrintWifigPowerInfo(
    AT_WIFI_INFO_STRU                  *pstWifiInfo,
    VOS_UINT8                           ucWifiMode,
    VOS_UINT8                           ucIndex
)
{

    /* 输出802.11g制式期望功率 */
    gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                       "%s:%s",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       "g");
    if ( (AT_WIFI_MODE_ONLY_PA == ucWifiMode)
      || (AT_WIFI_MODE_PA_NOPA == ucWifiMode) )
    {
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                           ",%d",
                                           pstWifiInfo->ausgPower[0]);
    }
    if ( (AT_WIFI_MODE_ONLY_NOPA == ucWifiMode)
      || (AT_WIFI_MODE_PA_NOPA == ucWifiMode) )
    {
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                           ",%d",
                                           pstWifiInfo->ausgPower[1]);
    }
}
VOS_VOID AT_PrintWifinPowerInfo(
    AT_WIFI_INFO_STRU                  *pstWifiInfo,
    VOS_UINT8                           ucWifiMode,
    VOS_UINT8                           ucIndex
)
{
    /* 输出802.11n制式期望功率 */
    gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                       "%s:%s",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       "n");
    if ( (AT_WIFI_MODE_ONLY_PA == ucWifiMode)
      || (AT_WIFI_MODE_PA_NOPA == ucWifiMode) )
    {
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                           ",%d",
                                           pstWifiInfo->ausnPower[0]);
    }
    if ( (AT_WIFI_MODE_ONLY_NOPA == ucWifiMode)
      || (AT_WIFI_MODE_PA_NOPA == ucWifiMode) )
    {
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                           ",%d",
                                           pstWifiInfo->ausnPower[1]);
    }
}
VOS_VOID AT_PrintWifiPowerInfo(
    AT_WIFI_INFO_STRU                  *pstWifiInfo,
    VOS_UINT8                           ucIndex
)
{
    AT_WIFI_MODE_ENUM_UINT8             ucWifiMode;

    /* 初始化 */
    ucWifiMode                          = (VOS_UINT8)WIFI_GET_PA_MODE();

    /* 单板支持802.11b制式, 输出期望功率 */
    if (VOS_TRUE == pstWifiInfo->bitOpbSupport)
    {
        AT_PrintWifibPowerInfo(pstWifiInfo, ucWifiMode, ucIndex);
        if ( (VOS_TRUE == pstWifiInfo->bitOpgSupport)
          || (VOS_TRUE == pstWifiInfo->bitOpnSupport) )
        {
            gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                       "%s",gaucAtCrLf);
        }

    }

    /* 单板支持802.11g制式, 输出期望功率 */
    if (VOS_TRUE == pstWifiInfo->bitOpgSupport)
    {
        AT_PrintWifigPowerInfo(pstWifiInfo, ucWifiMode, ucIndex);
        if (VOS_TRUE == pstWifiInfo->bitOpnSupport)
        {
            gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                       "%s",gaucAtCrLf);
        }
    }

    /* 单板支持802.11n制式, 输出期望功率 */
    if (VOS_TRUE == pstWifiInfo->bitOpnSupport)
    {
        AT_PrintWifinPowerInfo(pstWifiInfo, ucWifiMode, ucIndex);
    }

    return;
}


VOS_UINT32 AT_SetWifiInfoPara(VOS_UINT8 ucIndex)
{
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT16                          usLen;
    const VOS_CHAR * cString = "1,2,3,4,5,6,7,8,9,10,11,12,13";

    usLen = 0;

    if(gucAtParaIndex > 2)
    {
        return AT_ERROR;
    }

    if(0 == gastAtParaList[0].ulParaValue)
    {
        /* 0,0 */
        if(0 == gastAtParaList[1].ulParaValue)
        {
            usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                        (TAF_CHAR *)pgucAtSndCodeAddr,
                        (TAF_CHAR *)pgucAtSndCodeAddr + usLen,
                        "^WIINFO:b,%s%s^WIINFO:g,%s%s^WIINFO:n,%s",
                        cString, gaucAtCrLf, cString, gaucAtCrLf, cString);
        }
        else    /* 0,1 */
        {
            usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                        (TAF_CHAR *)pgucAtSndCodeAddr,
                        (TAF_CHAR *)pgucAtSndCodeAddr + usLen,
                        "^WIINFO:n,%s",
                        cString);
        }
    }
    else
    {
        /* 1,0 */
        if(0 == gastAtParaList[1].ulParaValue)
        {
            usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                        (TAF_CHAR *)pgucAtSndCodeAddr,
                        (TAF_CHAR *)pgucAtSndCodeAddr + usLen,
                        "^WIINFO:b,130%s^WIINFO:g,80%s^WIINFO:n,80",
                        gaucAtCrLf, gaucAtCrLf);
        }
        else    /* 1,1 */
        {
            usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                        (TAF_CHAR *)pgucAtSndCodeAddr,
                        (TAF_CHAR *)pgucAtSndCodeAddr + usLen,
                        "^WIINFO:n,80");
        }
    }
    gstAtSendData.usBufLen = usLen;

    return AT_OK;
#else

    AT_WIFI_INFO_STRU                   stWifiInfo = {0};

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /* 初始化 */
    gstAtSendData.usBufLen              = 0;

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_ERROR;
    }

    /*读取WIFI INFO对应的NV项*/
    if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_WIFI_INFO, &stWifiInfo, sizeof(AT_WIFI_INFO_STRU)))
    {
        AT_WARN_LOG("AT_SetWifiInfoPara:READ NV ERROR!");
        return AT_ERROR;
    }

    /* 查询支持的信道号或期望功率 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        /* 输出各制式支持的信道号 */
        AT_PrintWifiChannelInfo(&stWifiInfo, ucIndex);
    }
    else
    {
        /* 输出各制式的期望功率 */
        AT_PrintWifiPowerInfo(&stWifiInfo, ucIndex);
    }

    return AT_OK;
#endif
}
VOS_UINT32 AT_SetWifiPaRangePara (VOS_UINT8 ucIndex)
{
    AT_WIFI_MODE_ENUM_UINT8             ucWifiMode;

    /* 初始化 */
    ucWifiMode                          = (VOS_UINT8)WIFI_GET_PA_MODE();

    if (BSP_MODULE_UNSUPPORT == DRV_GET_WIFI_SUPPORT() )
    {
        return AT_ERROR;
    }


    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    /*参数检查*/
    if ('h' == gastAtParaList[0].aucPara[0])
    {
        if (AT_WIFI_MODE_ONLY_NOPA == ucWifiMode)
        {
            return AT_ERROR;
        }

        if (VOS_OK != WIFI_SET_PA_MODE(AT_WIFI_MODE_ONLY_PA))
        {
            return AT_ERROR;
        }
    }
    else if ('l' == gastAtParaList[0].aucPara[0])
    {
        if (AT_WIFI_MODE_ONLY_PA == ucWifiMode)
        {
            return AT_ERROR;
        }

        if (VOS_OK != WIFI_SET_PA_MODE(AT_WIFI_MODE_ONLY_NOPA))
        {
            return AT_ERROR;
        }
    }
    else
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}
VOS_UINT32 AT_SetTmmiPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           aucFacInfo[AT_FACTORY_INFO_LEN];

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }


    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }
    VOS_MemSet(aucFacInfo, 0x00, AT_FACTORY_INFO_LEN);

    /* 写入en_NV_Item_Factory_Info，需偏移24个字节，长度4为四个字节，因此需要先读 */
    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_Factory_Info, aucFacInfo, AT_FACTORY_INFO_LEN);

    if (NV_OK != ulResult)
    {
        AT_ERR_LOG("AT_SetTmmiPara: NV Read Fail!");
        return AT_ERROR;
    }

    /* 成功 写入"ST P" */
    if (1 == gastAtParaList[0].ulParaValue)
    {
        VOS_MemCpy(&aucFacInfo[AT_MMI_TEST_FLAG_OFFSET], AT_MMI_TEST_FLAG_SUCC, AT_MMI_TEST_FLAG_LEN);
    }
    /* 失败 清空 */
    else
    {
        VOS_MemCpy(&aucFacInfo[AT_MMI_TEST_FLAG_OFFSET], "0000", AT_MMI_TEST_FLAG_LEN);
    }

    ulResult = NV_WriteEx(MODEM_ID_0, en_NV_Item_Factory_Info, aucFacInfo, AT_FACTORY_INFO_LEN);

    if (NV_OK != ulResult)
    {
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}
VOS_UINT32 AT_SetChrgEnablePara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;

    ulRet                               = DRV_GET_BATTERY_SUPPORT();

    if (BSP_MODULE_UNSUPPORT == ulRet)
    {
        return AT_ERROR;
    }


    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }


    /*参数长度过长*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    if((0 == gastAtParaList[0].ulParaValue)
    || (1 == gastAtParaList[0].ulParaValue))
    {

        /* 调用驱动接口使能或去使能充电 */
        DRV_CHG_STATE_SET(gastAtParaList[0].ulParaValue);
    }
    else
    {

        /*调用补电接口*/
        DRV_CHG_BATT_SPLY();
    }


    return AT_OK;
}
VOS_UINT32 AT_SetTestScreenPara(VOS_UINT8 ucIndex)
{
#if 0
    VOS_UINT32                          ulRst;

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return  AT_TOO_MANY_PARA;
    }

    /* 调用底软屏测试接口 */
    ulRst = (VOS_UINT32)DRV_SCREEN_TEST_CMD(gastAtParaList[0].ulParaValue,gastAtParaList[1].ulParaValue);

    if (VOS_OK != ulRst)
    {
        return AT_ERROR;
    }
#endif
    return AT_OK;
}


VOS_UINT32 AT_SetCdurPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucCallId;
    VOS_UINT32                          ulRet;

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return  AT_TOO_MANY_PARA;
    }

    if ((gucAtParaIndex != 1)
     || (0 == gastAtParaList[0].usParaLen)
     || (gastAtParaList[0].ulParaValue > AT_CALL_MAX_NUM))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucCallId = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 发消息到C核获取通话时长 */
    ulRet = MN_CALL_QryCdur(gastAtClientTab[ucIndex].usClientId,
                            gastAtClientTab[ucIndex].opId,
                            ucCallId);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("AT_SetCdurPara: MN_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CDUR_READ;
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 AT_SetWebPwdPara(VOS_UINT8 ucIndex)
{
    VOS_CHAR                                aucWebPwdTmp[AT_WEBUI_PWD_MAX + 1];
    TAF_AT_NVIM_WEB_ADMIN_PASSWORD_STRU     stWebPwd;

    PS_MEM_SET(&stWebPwd, 0x0, sizeof(stWebPwd));

    /* 参数检查 */
    if (gucAtParaIndex > 2)
    {
        return  AT_TOO_MANY_PARA;
    }

    /* 参数长度过长 */
    if (gastAtParaList[1].usParaLen > AT_WEBUI_PWD_MAX)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }
    /* 校验WEBUI PWD */
    if (AT_WEBUI_PWD_VERIFY == gastAtParaList[0].ulParaValue)
    {
        if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_WEB_ADMIN_PASSWORD,&stWebPwd, sizeof(stWebPwd)))
        {
            AT_WARN_LOG("AT_SetWebPwdPara:READ NV ERROR");
            return AT_ERROR;
        }

        PS_MEM_CPY(aucWebPwdTmp, &stWebPwd, sizeof(stWebPwd));

        /* 密码不匹配 */
        if (0 != VOS_StrCmp(aucWebPwdTmp,(VOS_CHAR*)gastAtParaList[1].aucPara))
        {
            return AT_ERROR;
        }
    }
    else
    {
        /* 读取WEBPWD对应的NV项 */
        if (VOS_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_WEB_ADMIN_PASSWORD, gastAtParaList[1].aucPara, AT_WEBUI_PWD_MAX))
        {
            AT_WARN_LOG("AT_SetWebPwdPara:WRITE NV ERROR");
            return AT_ERROR;
        }
    }

    return AT_OK;
}
VOS_UINT32  AT_SetSdloadPara(VOS_UINT8 ucIndex)
{

#if (FEATURE_ON == FEATURE_LTE)
    return AT_SetLteSdloadPara(ucIndex);
#else
    VOS_UINT32                          ulRet;

    /* 1. 发消息到C核指示使单板进入下载模式 */
    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_SDLOAD_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("AT_SetSdloadPara: MN_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 2. 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SDLOAD_SET;
    return AT_WAIT_ASYNC_RETURN;

#endif
}


VOS_UINT32 AT_SetProdNamePara(VOS_UINT8 ucIndex)
{
    TAF_AT_PRODUCT_ID_STRU              stProductId;
    VOS_UINT32                          ulProductIdLen;
    VOS_UINT32                          ulRet;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    PS_MEM_SET(&stProductId,0,sizeof(TAF_AT_PRODUCT_ID_STRU));

    /* 从NV50048中读取产品名称 */
    /* 读取NV项en_NV_Item_PRODUCT_ID*/
    ulRet = NV_ReadEx(MODEM_ID_0, en_NV_Item_PRODUCT_ID,
                    &stProductId,
                    sizeof(stProductId.ulNvStatus) + sizeof(stProductId.aucProductId));

    if (NV_OK != ulRet)
    {
        AT_WARN_LOG("AT_SetProdNamePara: Fail to read en_NV_Item_PRODUCT_ID");
        return AT_ERROR;
    }


    /*产品名称写到NV项中，长度最长AT_PRODUCT_NAME_MAX_NUM 30，超过截断*/
    ulProductIdLen
        = (gastAtParaList[0].usParaLen > AT_PRODUCT_NAME_MAX_NUM) ? AT_PRODUCT_NAME_MAX_NUM:gastAtParaList[0].usParaLen;

    PS_MEM_SET(stProductId.aucProductId, 0, sizeof(stProductId.aucProductId));
    PS_MEM_CPY(stProductId.aucProductId, gastAtParaList[0].aucPara, (VOS_UINT16)ulProductIdLen);

    stProductId.ulNvStatus = VOS_TRUE;

    /* 写入NV */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_PRODUCT_ID,
                        &stProductId,
                        sizeof(stProductId.ulNvStatus) + sizeof(stProductId.aucProductId)))
    {
        AT_ERR_LOG("AT_SetProdNamePara:Write NV fail");
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}


VOS_UINT32  At_SendContinuesWaveOnToHPA(
    VOS_UINT8                           ucCtrlType,
    VOS_UINT16                          usPower
)
{
    AT_HPA_RF_CFG_REQ_STRU              *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* 申请AT_HPA_RF_CFG_REQ_STRU消息 */
    ulLength = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendContinuesWaveOnToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    PS_MEM_SET(pstMsg,0x00,sizeof(AT_HPA_RF_CFG_REQ_STRU));

    /* 填写消息头 */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = DSP_PID_WPHY;
    pstMsg->ulLength        = ulLength;

    /* 填写消息体 */
    pstMsg->usMsgID = ID_AT_HPA_RF_CFG_REQ;

    /* Tx Cfg */
    usMask =  W_RF_MASK_TX_TXONOFF | W_RF_MASK_TX_POWER;
    pstMsg->stRfCfgPara.sTxPower = (VOS_INT16)usPower;

    /* 按位标识配置类型 */
    pstMsg->stRfCfgPara.usMask      = usMask;
    pstMsg->stRfCfgPara.usTxAFCInit = W_AFC_INIT_VALUE;

    /* 打开单音信号 */
    pstMsg->stRfCfgPara.usTxEnable = ucCtrlType;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendContinuesWaveOnToHPA: Send msg fail!");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}


VOS_UINT32 AT_SetFwavePara(VOS_UINT8 ucIndex)
{
    /* WIFI*/
    VOS_CHAR                        acCmd[200] = {0};

#if(FEATURE_LTE == FEATURE_ON)
    if ((AT_RAT_MODE_FDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode)
        || (AT_RAT_MODE_TDD_LTE == g_stAtDevCmdCtrl.ucDeviceRatMode))
    {
        return atSetFWAVEPara(ucIndex);
    }
#endif

#if(FEATURE_UE_MODE_TDS == FEATURE_ON)
    if (AT_RAT_MODE_TDSCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        return atSetFWAVEPara(ucIndex);
    }
#endif

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数个数不正确，必须包括波形类型和波形功率 */
    if (gucAtParaIndex != 2)
    {
        return AT_ERROR;
    }

    /* 目前波形类型只支持设置单音*/
    if (gastAtParaList[0].ulParaValue > 7)
    {
        return AT_ERROR;
    }

    /* 该命令需在非信令模式下使用 */
    if (AT_TMODE_FTM != g_stAtDevCmdCtrl.ucCurrentTMode)
    {
        return AT_ERROR;
    }

    /* WIFI*/
    if (AT_RAT_MODE_WIFI == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        /*WIFI未Enable直接返回失败*/
        if(VOS_FALSE == (VOS_UINT32)WIFI_GET_STATUS())
        {
            return AT_ERROR;
        }

        /*向WIFI发送单音波形信号请求*/
        VOS_sprintf(acCmd,"athtestcmd -ieth0 --tx sine --txpwr %d", gastAtParaList[1].ulParaValue/100);

        WIFI_TEST_CMD(acCmd);

        return AT_OK;
    }


    /* 把设置保存在本地变量
       AT^FDAC设置的FDAC值和AT^FWAVE设置的power值表示的含义相同，取后设置的值
       功率值以0.01为单位，传给DSP的值会除10，所以AT还需要将该值再除10*/
    g_stAtDevCmdCtrl.usPower    = (VOS_UINT16)(gastAtParaList[1].ulParaValue/10);
    g_stAtDevCmdCtrl.bPowerFlag = VOS_TRUE;
    g_stAtDevCmdCtrl.bFdacFlag  = VOS_FALSE;
    /* 记录下type信息，并转换为G物理层使用的TxMode，在向物理层发送ID_AT_GHPA_RF_TX_CFG_REQ时携带 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        g_stAtDevCmdCtrl.usTxMode = 1;
    }
    else
    {
        g_stAtDevCmdCtrl.usTxMode = 0;
    }


    /* WCDMA */
    if (AT_RAT_MODE_WCDMA == g_stAtDevCmdCtrl.ucDeviceRatMode)
    {
        /* 向WDSP发送开关单音信号的原语请求 */
        if (AT_FAILURE == At_SendContinuesWaveOnToHPA(WDSP_CTRL_TX_ONE_TONE, g_stAtDevCmdCtrl.usPower))
        {
            return AT_ERROR;
        }

        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SET_FWAVE;
        g_stAtDevCmdCtrl.ucIndex = ucIndex;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }

    return AT_OK;

}
VOS_UINT32 AT_SetApRptSrvUrlPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           aucApRptSrvUrl[AT_AP_XML_RPT_SRV_URL_LEN + 1];
    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (1 != gucAtParaIndex )
    {
        return AT_ERROR;
    }

    /* URL为空或者长度超过127*/
    if ((0 == gastAtParaList[0].usParaLen) || (gastAtParaList[0].usParaLen > AT_AP_XML_RPT_SRV_URL_LEN))
    {
        return AT_ERROR;
    }

    PS_MEM_SET(aucApRptSrvUrl, 0, (VOS_SIZE_T)(AT_AP_XML_RPT_SRV_URL_LEN + 1));
    PS_MEM_CPY(aucApRptSrvUrl, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    /* 写NV:en_NV_Item_AP_RPT_SRV_URL*/
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_AP_RPT_SRV_URL,
                        aucApRptSrvUrl,
                        AT_AP_XML_RPT_SRV_URL_LEN + 1))
    {
        AT_ERR_LOG("AT_SetApRptSrvUrlPara:Write NV fail");
        return AT_ERROR;
    }
    else
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    "^APRPTSRVURLNTY");

        return AT_OK;
    }
}


VOS_UINT32 AT_SetApXmlInfoTypePara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           aucApXmlInfoType[AT_AP_XML_RPT_INFO_TYPE_LEN + 1];

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (1 != gucAtParaIndex )
    {
        return AT_ERROR;
    }

    /* INFO TYPE为空或者长度超过127*/
    if ((0 == gastAtParaList[0].usParaLen) || (gastAtParaList[0].usParaLen > AT_AP_XML_RPT_INFO_TYPE_LEN))
    {
        return AT_ERROR;
    }

    PS_MEM_SET(aucApXmlInfoType, 0, (VOS_SIZE_T)(AT_AP_XML_RPT_INFO_TYPE_LEN + 1));
    PS_MEM_CPY(aucApXmlInfoType, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    /* 写NV:en_NV_Item_AP_XML_INFO_TYPE*/
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_AP_XML_INFO_TYPE,
                        aucApXmlInfoType,
                        AT_AP_XML_RPT_INFO_TYPE_LEN + 1))
    {
        AT_ERR_LOG("AT_SetApXmlInfoTypePara:Write NV fail");
        return AT_ERROR;
    }
    else
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    "^APXMLINFOTYPENTY");

        return AT_OK;
    }
}


VOS_UINT32 AT_SetApXmlRptFlagPara(VOS_UINT8 ucIndex)
{
    TAF_AT_NVIM_AP_XML_RPT_FLG_STRU     stApXmlRptFlg;
    VOS_UINT32                          ulNvLength;

    ulNvLength = 0;
    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (1 != gucAtParaIndex )
    {
        return AT_ERROR;
    }

    stApXmlRptFlg.ucApXmlRptFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 写NV:en_NV_Item_AP_XML_RPT_FLAG*/
    NV_GetLength(en_NV_Item_AP_XML_RPT_FLAG, &ulNvLength);
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_AP_XML_RPT_FLAG,
                        &stApXmlRptFlg,
                        ulNvLength))
    {
       AT_ERR_LOG("AT_SetApXmlRptFlagPara:Write NV fail");
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }
}



VOS_UINT32 AT_SetFastDormPara(VOS_UINT8 ucIndex)
{
    AT_RABM_FASTDORM_PARA_STRU          stFastDormPara;
    VOS_UINT32                          ulRslt;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第二个参数为空，则填写默认值 */
    if (0 == gastAtParaList[1].usParaLen)
    {
        /* 填写默认值5S */
        gastAtParaList[1].ulParaValue = AT_FASTDORM_DEFAULT_TIME_LEN;
    }

    stFastDormPara.enFastDormOperationType = gastAtParaList[0].ulParaValue;
    stFastDormPara.ulTimeLen = gastAtParaList[1].ulParaValue;

#if (FEATURE_ON == FEATURE_LTE)
    AT_SetLFastDormPara(ucIndex);
#endif

    /* 在AT融合版本上，新增的接口为直接发消息，因此这里直接发送消息给RABM */
    ulRslt = AT_SndSetFastDorm(gastAtClientTab[ucIndex].usClientId,
                               gastAtClientTab[ucIndex].opId,
                               &stFastDormPara);
    if (VOS_OK == ulRslt)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_FASTDORM_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
VOS_UINT32 AT_SetMemInfoPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;

    /* 参数检查 */
    if( (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
     || (gucAtParaIndex > 1) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发消息到C核获取 MEMINFO 信息, gastAtParaList[0].ulParaValue 中为查询的类型 */
    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_MEMINFO_QRY_REQ,
                                   &(gastAtParaList[0].ulParaValue),
                                   sizeof(gastAtParaList[0].ulParaValue),
                                   I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("AT_QryMemInfoPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MEMINFO_READ;
    return AT_WAIT_ASYNC_RETURN;

}

#if (VOS_WIN32 == VOS_OS_VER)
/*****************************************************************************
 函 数 名  : AT_SetRplmnStub
 功能描述  : 对RPLMN进行打桩以便GTR用例测试
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月18日
    作    者   : zhoujun 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  AT_SetRplmnStub( VOS_UINT8 ucIndex )
{
    TAF_PLMN_ID_STRU                    stPlmnId;
    VOS_UINT32                          ulLai;
    VOS_UINT32                          ulRac;


    /* 参数不正确,
    0:表示需要设置L的RPLMN
    1:表示需要设置GU的RPLMN
    */
    if ( (0 != gastAtParaList[0].ulParaValue)
      && (1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 需要设置PLMN ID的值 */
    if ( (5 != gastAtParaList[1].usParaLen)
       && (6 != gastAtParaList[1].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Mcc */
    if(AT_FAILURE == At_String2Hex(gastAtParaList[1].aucPara,3,&stPlmnId.Mcc))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    /* Mnc */
    if(AT_FAILURE == At_String2Hex(&gastAtParaList[1].aucPara[3],gastAtParaList[1].usParaLen-3,&stPlmnId.Mnc))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    stPlmnId.Mcc |= 0xFFFFF000U;
    stPlmnId.Mnc |= (0xFFFFFFFFU << ((gastAtParaList[1].usParaLen-3)*4));

    /* lai */
    ulLai = 0x0001;
    if (4 == gastAtParaList[2].usParaLen)
    {
        if(AT_FAILURE == At_String2Hex(gastAtParaList[2].aucPara, 4, &ulLai))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /*rac  */
    ulRac = 0x01;
    if (2 == gastAtParaList[3].usParaLen)
    {
        if(AT_FAILURE == At_String2Hex(gastAtParaList[3].aucPara, 2, &ulRac))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /* 需要设置L的RPLMN */
    if ( 0 == gastAtParaList[0].ulParaValue )
    {
        NAS_SetLteRplmnId(&stPlmnId, ulLai,ulRac);
    }
    else
    {
    }

    return AT_OK;
}


VOS_UINT32  AT_SetDamParaStub( VOS_UINT8 ucIndex )
{
    TAF_PLMN_ID_STRU                    stImsiPlmnList[16];
    VOS_UINT8                           ucImsiPlmnNum;
    TAF_PLMN_ID_STRU                    stDamPlmnList[16];
    VOS_UINT8                           ucDamPlmnNum;
    VOS_UINT8                           ucEnableLteTrigPlmnSearchFlag;
    VOS_UINT8                           ucAddDamPlmnInDisabledPlmnList;
    VOS_UINT8                           i;
    VOS_UINT8                           ucParaIndex;
    VOS_UINT32                          ulMcc;
    VOS_UINT32                          ulMnc;

    PS_MEM_SET(&stImsiPlmnList, 0, sizeof(stImsiPlmnList));
    PS_MEM_SET(&stDamPlmnList, 0, sizeof(stDamPlmnList));

    /* 第一个参数为enable lte是否需要搜网，0为不需要；1为需要；
       第二个参数为DAM网络disable lte后是否加入禁止接入技术网络列表中，0为不需要；1为需要；
       第三个参数为imsi plmn list个数，如果个数为2，第四个参数和第五个参数为 imsi plmn id。
       第六个参数为DAM plmn list 个数，如果个数为2，第七个参数和第8个参数为dam plmn id*/
    if(AT_FAILURE == At_String2Hex(gastAtParaList[0].aucPara,gastAtParaList[0].usParaLen,&gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( (0 != gastAtParaList[0].ulParaValue)
      && (1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucEnableLteTrigPlmnSearchFlag = gastAtParaList[0].ulParaValue;

    if(AT_FAILURE == At_String2Hex(gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen,&gastAtParaList[1].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( (0 != gastAtParaList[1].ulParaValue)
      && (1 != gastAtParaList[1].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucAddDamPlmnInDisabledPlmnList = gastAtParaList[1].ulParaValue;

    if(AT_FAILURE == At_String2Hex(gastAtParaList[2].aucPara,gastAtParaList[2].usParaLen,&gastAtParaList[2].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gastAtParaList[2].ulParaValue > 16)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucImsiPlmnNum = gastAtParaList[2].ulParaValue;
    for (i = 1; i <= ucImsiPlmnNum; i++)
    {
        /* Mcc */
        if(AT_FAILURE == At_String2Hex(gastAtParaList[2+i].aucPara,3,&ulMcc))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        stImsiPlmnList[i-1].Mcc = ulMcc;

        /* Mnc */
        if(AT_FAILURE == At_String2Hex(&gastAtParaList[2+i].aucPara[3],gastAtParaList[2+i].usParaLen-3,&ulMnc))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        stImsiPlmnList[i-1].Mnc = ulMnc;

        stImsiPlmnList[i-1].Mcc |= 0xFFFFF000U;
        stImsiPlmnList[i-1].Mnc |= (0xFFFFFFFFU << ((gastAtParaList[2+i].usParaLen-3)*4));
    }

    ucParaIndex = 2+ucImsiPlmnNum+1;
    if(AT_FAILURE == At_String2Hex(gastAtParaList[ucParaIndex].aucPara,gastAtParaList[ucParaIndex].usParaLen,&gastAtParaList[ucParaIndex].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucDamPlmnNum = gastAtParaList[ucParaIndex].ulParaValue;

    for (i = 1; i <= ucDamPlmnNum; i++)
    {
        /* Mcc */
        if(AT_FAILURE == At_String2Hex(gastAtParaList[ucParaIndex+i].aucPara,3,&stDamPlmnList[i-1].Mcc))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* Mnc */
        if(AT_FAILURE == At_String2Hex(&gastAtParaList[ucParaIndex+i].aucPara[3],gastAtParaList[ucParaIndex+i].usParaLen-3,&stDamPlmnList[i-1].Mnc))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        stDamPlmnList[i-1].Mcc |= 0xFFFFF000U;
        stDamPlmnList[i-1].Mnc |= (0xFFFFFFFFU << ((gastAtParaList[ucParaIndex+i].usParaLen-3)*4));
    }

    NAS_SetDamCfg(ucEnableLteTrigPlmnSearchFlag, ucAddDamPlmnInDisabledPlmnList,
        ucImsiPlmnNum, stImsiPlmnList, ucDamPlmnNum, stDamPlmnList);
    return AT_OK;
}



VOS_UINT32 AT_SetCsUnAvailPlmnStub( VOS_UINT8 ucIndex )
{
    TAF_PLMN_ID_STRU                    stPlmnId;


    /* 参数不正确,
    0:表示需要删除CS不可用网络
    1:表示需要新增CS不可用网络
    */
    if ( (0 != gastAtParaList[0].ulParaValue)
      && (1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 需要设置PLMN ID的值 */
    if ( (5 != gastAtParaList[1].usParaLen)
       && (6 != gastAtParaList[1].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Mcc */
    if(AT_FAILURE == At_String2Hex(gastAtParaList[1].aucPara,3,&stPlmnId.Mcc))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    /* Mnc */
    if(AT_FAILURE == At_String2Hex(&gastAtParaList[1].aucPara[3],gastAtParaList[1].usParaLen-3,&stPlmnId.Mnc))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    stPlmnId.Mcc |= 0xFFFFF000U;
    stPlmnId.Mnc |= (0xFFFFFFFFU << ((gastAtParaList[1].usParaLen-3)*4));

    /* 需要设置L的CS不可用网络 */
    if (VOS_OK == NAS_SetCsUnAvailPlmnId(gastAtParaList[0].ulParaValue, &stPlmnId))
    {
        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetForbRoamTaStub( VOS_UINT8 ucIndex )
{
    /* 参数不正确,
    0:表示需要删除禁止漫游TA
    1:表示需要新增禁止漫游TA
    */
    if ( (0 != gastAtParaList[0].ulParaValue)
      && (1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 需要设置L的禁止漫游TA */
    if (VOS_OK == NAS_SetForbRoamTa(gastAtParaList[0].ulParaValue))
    {
        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32  AT_SetTinTypeStub( VOS_UINT8 ucIndex )
{
    if (gucAtParaIndex > 1)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置TIN类型 */
    NAS_SetTinType(gastAtParaList[0].ulParaValue);

    return AT_OK;
}
VOS_UINT32  AT_SetPsRegisterContainDrxStub( VOS_UINT8 ucIndex )
{
    if (gucAtParaIndex > 1)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

    NAS_SetPsRegContainDrx(gastAtParaList[0].ulParaValue);

    return AT_OK;
}


VOS_UINT32  AT_SetPsBearIsrFlgStub( VOS_UINT8 ucIndex )
{
    if (gucAtParaIndex > 3)
    {
        return  AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_ON == FEATURE_LTE)

    /* 设置usapi 对应的pdp是在ISR激活前激活的还是在ISR激活后激活的 */
    NAS_SetPsBearerIsrFlg(gastAtParaList[0].ulParaValue,
                          gastAtParaList[1].ulParaValue,
                          gastAtParaList[2].ulParaValue);
#endif

    return AT_OK;
}
VOS_UINT32 AT_SetImsRatStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                                               ucLteImsSupportFlg;
    VOS_UINT8                                               ucLteEmsSupportFlg;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 2)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数 LTE IMS是否支持 */
    ucLteImsSupportFlg       = gastAtParaList[0].ulParaValue;

    /* 第二个参数 LTE EMS是否支持 */
    ucLteEmsSupportFlg    = gastAtParaList[1].ulParaValue;

    NAS_SetLteImsSupportFlag(ucLteImsSupportFlg, ucLteEmsSupportFlg);

    return AT_OK;

}



VOS_UINT32 AT_SetImsCapabilityStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                                               ucVoiceCallOnImsSupportFlg;
    VOS_UINT8                                               ucVideoCallOnImsSupportFlg;
    VOS_UINT8                                               ucSmsOnImsSupportFlg;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 3)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数表示voice call是否支持 0:不支持;1:支持 */
    ucVoiceCallOnImsSupportFlg      = gastAtParaList[0].ulParaValue;

    /* 第二个参数表示video call是否支持 0:不支持;1:支持 */
    ucVideoCallOnImsSupportFlg      = gastAtParaList[1].ulParaValue;

    /* 第三个参数表示sms是否支持 0:不支持;1:支持 */
    ucSmsOnImsSupportFlg            = gastAtParaList[2].ulParaValue;

    NAS_SetImsCapabilitySupportFlag(ucVoiceCallOnImsSupportFlg, ucVideoCallOnImsSupportFlg, ucSmsOnImsSupportFlg);

    return AT_OK;

}
VOS_UINT32 AT_SetDomainStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                                               ucVoiceDomain;
    VOS_UINT8                                               ucSmsDomain;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 2)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数 voice domain时，0:CS ONLY; 1:IMS PS ONLY; 2:CS PREFERRED; 3:IMS PS PREFERRED */
    ucVoiceDomain   = gastAtParaList[0].ulParaValue;

    /* 第二个参数 sms domain时，  0:NOT USE SMS OVER IP;    1:PREFERR TO USE SMS OVER IP */
    ucSmsDomain    = gastAtParaList[1].ulParaValue;

    NAS_SetPreferDomain(ucVoiceDomain, ucSmsDomain);

    return AT_OK;

}



VOS_UINT32 AT_SetCmdImsRoamingStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucImsRoamingFlag;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取漫游支持参数 */
    ucImsRoamingFlag   = gastAtParaList[0].ulParaValue;

    NAS_SetImsRoaming(ucImsRoamingFlag);

    return AT_OK;
}
VOS_UINT32 AT_SetCmdRedailStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucCallRedialImsToCs;
    VOS_UINT8                           ucSmsRedialImsToCs;
    VOS_UINT8                           ucSsRedialImsToCs;
    VOS_UINT8                           ucCallRedialCsToIms;
    VOS_UINT8                           ucSmsRedialCsToIms;
    VOS_UINT8                           ucSsRedialCsToIms;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 7)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* IMS到CS呼叫换域重拨配置 */
    ucCallRedialImsToCs = gastAtParaList[0].ulParaValue;

    /* IMS到CS短信换域重拨配置 */
    ucSmsRedialImsToCs  = gastAtParaList[1].ulParaValue;

    /* IMS到CS补充换域重拨配置 */
    ucSsRedialImsToCs   = gastAtParaList[2].ulParaValue;

    /* CS到IMS呼叫换域重拨配置 */
    ucCallRedialCsToIms = gastAtParaList[3].ulParaValue;

    /* CS到IMS短信换域重拨配置 */
    ucSmsRedialCsToIms  = gastAtParaList[4].ulParaValue;

    /* CS到IMS补充换域重拨配置 */
    ucSsRedialCsToIms   = gastAtParaList[5].ulParaValue;

    NAS_SetRedialCfg(ucCallRedialImsToCs, ucSmsRedialImsToCs, ucSsRedialImsToCs,
                     ucCallRedialCsToIms, ucSmsRedialCsToIms, ucSsRedialCsToIms);

    return AT_OK;
}



VOS_UINT32 AT_SetImsVoiceInterSysLauEnableStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucForceLauFlag;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取漫游支持参数 */
    ucForceLauFlag   = gastAtParaList[0].ulParaValue;

    NAS_SetImsVoiceInterSysLauEnable(ucForceLauFlag);

    return AT_OK;
}
VOS_UINT32 AT_SetImsVoiceMMEnableStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucImsVoiceMMEnableFlag;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取漫游支持参数 */
    ucImsVoiceMMEnableFlag   = gastAtParaList[0].ulParaValue;

    NAS_SetImsVoiceMMEnableFlg(ucImsVoiceMMEnableFlag);

    return AT_OK;
}
VOS_UINT32 AT_SetCmdImsUssdStub(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucImsUssdFlag;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取漫游支持参数 */
    ucImsUssdFlag   = gastAtParaList[0].ulParaValue;

    NAS_SetImsUssd(ucImsUssdFlag);

    return AT_OK;
}


/* Added by c00318887 for 移植T3402 , 2015-6-18, begin */
/*****************************************************************************
 函 数 名  : AT_SetT3402Stub
 功能描述  : 对T3402相关设置进行打桩以便DMT用例测试
 输入参数  : ucIndex - 索引
 输出参数  : 无
 返 回 值  : 执行结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月18日
    作    者   : c00318887
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  AT_SetT3402Stub( VOS_UINT8 ucIndex )
{
    VOS_UINT8                           i;
    VOS_UINT8                           ucParaIndex;

    /* 参数过多 */
    if ( gucAtParaIndex > 2 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 == gastAtParaList[1].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数为 disable lte start T3402 enable lte FLAG  */
    if(AT_FAILURE == At_String2Hex(gastAtParaList[0].aucPara,gastAtParaList[0].usParaLen,&gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( (0 != gastAtParaList[0].ulParaValue)
      && (1 != gastAtParaList[0].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_String2Hex(gastAtParaList[1].aucPara,gastAtParaList[1].usParaLen,&gastAtParaList[1].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( (0 != gastAtParaList[1].ulParaValue)
      && (1 != gastAtParaList[1].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    NAS_MML_SetDisableLteStartT3402EnableLteFlag(gastAtParaList[0].ulParaValue);

    NAS_MML_SetHighPrioRatBgEnableLteFlag(gastAtParaList[1].ulParaValue);

    return AT_OK;
}

/* Added by c00318887 for 移植T3402 , 2015-6-18, end */

#endif


VOS_UINT32 AT_SetDnsQueryPara(VOS_UINT8 ucIndex)
{

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 给APS模块发送执行命令操作消息 */
    if ( VOS_OK != TAF_PS_GetDynamicDnsInfo(WUEPS_PID_AT,
                                                gastAtClientTab[ucIndex].usClientId,
                                                gastAtClientTab[ucIndex].opId,
                                                (VOS_UINT8)gastAtParaList[0].ulParaValue))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DNSQUERY_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 AT_SetMemStatusPara(VOS_UINT8 ucIndex)
{
    MN_MSG_SET_MEMSTATUS_PARM_STRU      stMemStatus;

    /* 初始化 */
    PS_MEM_SET(&stMemStatus, 0x00, sizeof(MN_MSG_SET_MEMSTATUS_PARM_STRU));

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 1 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置参数 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        stMemStatus.enMemFlag = MN_MSG_MEM_FULL_SET;
    }
    else
    {
        stMemStatus.enMemFlag = MN_MSG_MEM_FULL_UNSET;
    }

    /* 调用MN消息API发送设置请求给MSG模块 */
    if ( MN_ERR_NO_ERROR != MN_MSG_SetMemStatus(gastAtClientTab[ucIndex].usClientId,
                                                gastAtClientTab[ucIndex].opId,
                                                &stMemStatus))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt  = AT_CMD_CSASM_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetApRptPortSelectPara(VOS_UINT8 ucIndex)
{
    AT_PORT_RPT_CFG_UNION               unRptCfg;
    AT_CLIENT_CONFIGURATION_STRU       *pstClientCfg;
    AT_CLIENT_CFG_MAP_TAB_STRU         *pstCfgMapTbl;
    VOS_UINT8                           i;

    unRptCfg.ulRptCfgBit64  = 0;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if ( gucAtParaIndex > 2 )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( 0 == gastAtParaList[0].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ( 0 == gastAtParaList[1].usParaLen )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取用户设置的参数 */
    /* AT的端口个数大于32个，所以用两个32bit的参数表示对应的端口是否允许主动上报 */
    AT_String2Hex(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen, &unRptCfg.aulRptCfgBit32[0]);
    AT_String2Hex(gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen, &unRptCfg.aulRptCfgBit32[1]);

    for (i = 0; i < AT_GET_CLIENT_CFG_TAB_LEN(); i++)
    {
        pstCfgMapTbl = AT_GetClientCfgMapTbl(i);
        pstClientCfg = AT_GetClientConfig(pstCfgMapTbl->enClientId);

        pstClientCfg->ucReportFlg = (unRptCfg.ulRptCfgBit64 & pstCfgMapTbl->ulRptCfgBit64) ?
                                    VOS_TRUE : VOS_FALSE;
    }

    return AT_OK;
}


VOS_UINT32 At_SetGpioPara (VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucGroup;
    VOS_UINT8                           ucPin;
    VOS_UINT8                           ucValue;

    /* 如果不支持HSIC特性，不支持GPIO切换 */
    if (BSP_MODULE_SUPPORT != DRV_GET_HSIC_SUPPORT())
    {
        return AT_ERROR;
    }

    /* para too many */
    if (gucAtParaIndex > 3)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucGroup = (VOS_UINT8)(gastAtParaList[0].ulParaValue / 100);
    ucPin   = (VOS_UINT8)(gastAtParaList[0].ulParaValue % 100);

    if (VOS_OK != DRV_GPIO_SET(ucGroup, ucPin, (VOS_UINT8)gastAtParaList[1].ulParaValue))
    {
        return AT_ERROR;
    }

    VOS_TaskDelay(gastAtParaList[2].ulParaValue);

    /* 完成GPIO设置后需要将电平反转过来 */
    ucValue = (gastAtParaList[1].ulParaValue == DRV_GPIO_HIGH)?DRV_GPIO_LOW:DRV_GPIO_HIGH;


    if (VOS_OK != DRV_GPIO_SET(ucGroup, ucPin, ucValue))
    {
        return AT_ERROR;
    }

    return AT_OK;
}
VOS_UINT32 At_SetUsbSwitchPara (VOS_UINT8 ucIndex)
{
    VOS_INT                          iResult;
    VOS_UINT8                        ucUsbStatus;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* para too many */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }


    ucUsbStatus = USB_SWITCH_OFF;

    if (VOS_OK != DRV_USB_PHY_SWITCH_GET(&ucUsbStatus))
    {
        return AT_ERROR;
    }

    /* 设置Modem的USB启动 */
    if(AT_USB_SWITCH_SET_VBUS_VALID == gastAtParaList[0].ulParaValue)
    {
        if (USB_SWITCH_ON != ucUsbStatus)
        {
            iResult = DRV_USB_PHY_SWITCH_SET(USB_SWITCH_ON);
        }
        else
        {
            return AT_OK;
        }
    }
    else if(AT_USB_SWITCH_SET_VBUS_INVALID == gastAtParaList[0].ulParaValue)
    {
        if (USB_SWITCH_OFF != ucUsbStatus)
        {
            iResult = DRV_USB_PHY_SWITCH_SET(USB_SWITCH_OFF);
        }
        else
        {
            return AT_OK;
        }
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (iResult == VOS_OK)
    {
        return At_PortSwitchSndMsg(ucUsbStatus);
    }
    else
    {
        return AT_ERROR;
    }
}


TAF_UINT32 At_PortSwitchSndMsg (TAF_UINT32 ulSwitchMode)
{
    OM_PORT_SWITCH_MSG_STRU             *pMsg;

    pMsg = (OM_PORT_SWITCH_MSG_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                                                   sizeof(OM_PORT_SWITCH_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        return AT_ERROR;
    }

    pMsg->ulReceiverPid = ACPU_PID_OM;
    pMsg->ulMsgName     = (OM_AT_MSG_ENUM_UINT32)AT_OM_PORT_SWITCH;
    pMsg->ulSwitchMode  = (OM_PORT_SWITCH_MODE_ENUM_UINT32)ulSwitchMode;

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        return AT_ERROR;
    }

    return AT_OK;
}



VOS_UINT32 AT_SetSARReduction(VOS_UINT8 ucIndex)
{
    VOS_UINT32                              ulResult;
	VOS_UINT16                              usUETestMode;

    VOS_UINT16                              usSarReduction;
    AT_MODEM_MT_INFO_CTX_STRU              *pstMtInfoCtx = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                    enModemId;
    VOS_UINT16                              usAntState;

	usUETestMode = 0;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        AT_ERR_LOG("AT_SetSARReduction: Cmd Opt Type is wrong.");
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 读取NV项判定是否为测试模式，该NV项只有一份 */
    ulResult = NV_ReadEx(MODEM_ID_0,
                         en_NV_Item_RF_SAR_BACKOFF_TESTMODE,
                         &usUETestMode,
                         sizeof(usUETestMode));

    if (NV_OK != ulResult)
    {
        AT_ERR_LOG("AT_SetSARReduction:Read NV fail");
        return AT_ERROR;
    }


    enModemId  = MODEM_ID_0;

    ulResult   = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulResult)
    {
        AT_ERR_LOG("AT_SetSARReduction:Get modem id fail");

        return AT_ERROR;
    }

    pstMtInfoCtx = AT_GetModemMtInfoCtxAddrFromModemId(enModemId);

    ulResult = TAF_AGENT_GetAntState(ucIndex, &usAntState);
    if (VOS_OK != ulResult)
    {
        AT_ERR_LOG("AT_QrySARReduction:Get modem id fail");
        return AT_ERROR;
    }

    if ((AT_ANT_CONDUCTION_MODE == usAntState) && (VOS_TRUE != usUETestMode))
    {
        AT_ERR_LOG("AT_SetSARReduction: Test mode wrong");
        return AT_ERROR;
    }

    usSarReduction = (VOS_UINT16)gastAtParaList[0].ulParaValue;
    pstMtInfoCtx->usSarReduction = usSarReduction;

    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      DRV_AGENT_SARREDUCTION_SET_REQ,
                                      &usSarReduction,
                                      sizeof(usSarReduction),
                                      I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS != ulResult)
    {
        AT_ERR_LOG("AT_SetSARReduction: Send Msg Fail");

        return AT_ERROR;
    }

    return AT_OK;
}
TAF_UINT32 At_SetScidPara(TAF_UINT8 ucIndex)
{
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 执行命令操作 */
    if(AT_SUCCESS == Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId,0,TAF_PH_ICC_ID,TAF_NULL_PTR))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ICCID_READ;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UCHAR ToBase64Char(const VOS_UCHAR code6bit)
{
    /* [ 0..25] => ['A'..'Z'] */
    if (code6bit<26)
    {
        return (VOS_UCHAR)(code6bit+'A');
    }

    /* [26..51] => ['a'..'z'] */
    else if (code6bit<52)
    {
        return (VOS_UCHAR)(code6bit+('a'-26));
    }

    /* [52..61] => ['0'..'9'] */
    else if (code6bit<62)
    {
        return (VOS_UCHAR)(code6bit-52+'0');
    }

    /* 62 => '+' */
    else if (code6bit==62)
    {
        return '+';
    }

    /* 63 => */
    else
    {
        return '/';
    }
}
VOS_VOID AtBase64Encode(const VOS_VOID* pdata,const VOS_UINT32 data_size,VOS_VOID* out_pcode)
{
    const VOS_UCHAR* input      =(const unsigned char*)pdata;
    const VOS_UCHAR* input_end  =&input[data_size];
    VOS_UCHAR*       output     =(unsigned char*)out_pcode;
    VOS_UINT32       bord_width = 0;

    for(; input+2 < input_end; input += 3, output += 4 )
    {
        output[0] = ToBase64Char(   input[0] >> 2 );
        output[1] = ToBase64Char( ((input[0] << 4) | (input[1] >> 4)) & 0x3F );
        output[2] = ToBase64Char( ((input[1] << 2) | (input[2] >> 6)) & 0x3F );
        output[3] = ToBase64Char(   input[2] & 0x3F);
    }

    if(input_end > input)
    {
        bord_width = (VOS_UINT32)(input_end-input);
        if ( bord_width == 1 )
        {
            output[0] = ToBase64Char(   input[0] >> 2 );
            output[1] = ToBase64Char(  (input[0] << 4) & 0x3F );
            output[2] = AT_BASE64_PADDING;
            output[3] = AT_BASE64_PADDING;
        }
        else if (bord_width==2)
        {
            output[0] = ToBase64Char(   input[0] >> 2 );
            output[1] = ToBase64Char( ((input[0] << 4) | (input[1] >> 4)) & 0x3F );
            output[2] = ToBase64Char(  (input[1] << 2) & 0x3F );
            output[3] = AT_BASE64_PADDING;
        }
        else
        {
        }
    }
}



unsigned int AtBase64Decode(const void* pcode,const unsigned int code_size,void* out_pdata,
                            const unsigned int data_size,unsigned int* out_pwrited_data_size)
{
    const unsigned char DECODE_DATA_MAX = 64-1;
    const unsigned char DECODE_PADDING  = DECODE_DATA_MAX+2;
    const unsigned char DECODE_ERROR    = DECODE_DATA_MAX+3;
    static unsigned char BASE64_DECODE[256];
    static VOS_BOOL initialized=VOS_FALSE;
    unsigned int code0;
    unsigned int code1;
    unsigned int code2;
    unsigned int code3;
    unsigned int bord_width;
    const unsigned char* input_last_fast_node;
    unsigned int output_size;
    unsigned char* output;
    const unsigned char* input;
    unsigned int code_node;
    unsigned int i;

    if(!initialized)
    {
        for(i=0;i<256;++i)
        {
            BASE64_DECODE[i]=DECODE_ERROR;
        }
        for(i='A';i<='Z';++i)
        {
            BASE64_DECODE[i]=(unsigned char)(i-'A');
        }
        for(i='a';i<='z';++i)
        {
            BASE64_DECODE[i]=(unsigned char)(i-'a'+26);
        }
        for(i='0';i<='9';++i)
        {
            BASE64_DECODE[i]=(unsigned char)(i-'0'+26*2);
        }
        BASE64_DECODE['+']=26*2+10;
        BASE64_DECODE['/']=26*2+10+1;
        BASE64_DECODE['=']=DECODE_PADDING;
        initialized=VOS_TRUE;
    }
    *out_pwrited_data_size=0;
    code_node=code_size/4;
    if ((code_node*4)!=code_size)
    {
        return b64Result_CODE_SIZE_ERROR;
    }
    else if (code_node==0)
    {
        return b64Result_OK;
    }
    else
    {
    }

    /* code_node>0 */
    input=(const unsigned char*)pcode;
    output=(unsigned char*)out_pdata;
    output_size=code_node*3;

    if (input[code_size-2]==AT_BASE64_PADDING)
    {
        if (input[code_size-1]!=AT_BASE64_PADDING)
        {
            return b64Result_CODE_ERROR;
        }

        output_size-=2;
    }
    else if (input[code_size-1]==AT_BASE64_PADDING)
    {
        --output_size;
    }
    else
    {
    }

    if (output_size>data_size)
    {
        return b64Result_DATA_SIZE_SMALLNESS;
    }
    input_last_fast_node=&input[output_size/3UL * 4UL];

    for(;input<input_last_fast_node;input+=4,output+=3)
    {
        code0=BASE64_DECODE[input[0]];
        code1=BASE64_DECODE[input[1]];
        code2=BASE64_DECODE[input[2]];
        code3=BASE64_DECODE[input[3]];
        if ( ((code0|code1)|(code2|code3)) <= DECODE_DATA_MAX )
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
            output[1]=(unsigned char)((code1<<4) + (code2>>2));
            output[2]=(unsigned char)((code2<<6) + code3);
        }
        else
        {
            return b64Result_CODE_ERROR;
        }
    }
    bord_width=output_size%3;
    if (bord_width==1)
    {
        code0=BASE64_DECODE[input[0]];
        code1=BASE64_DECODE[input[1]];
        if ((code0|code1) <= DECODE_DATA_MAX)
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
        }
        else
        {
            return b64Result_CODE_ERROR;
        }
    }
    else if (bord_width==2)
    {
        code0=BASE64_DECODE[input[0]];
        code1=BASE64_DECODE[input[1]];
        code2=BASE64_DECODE[input[2]];
        if ((code0|code1|code2) <= DECODE_DATA_MAX)
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
            output[1]=(unsigned char)((code1<<4) + (code2>>2));
        }
        else
        {
            return b64Result_CODE_ERROR;
        }
    }
    else
    {
    }
    *out_pwrited_data_size=output_size;
    return b64Result_OK;
}
VOS_UINT32 AT_RsfrSimLockProc(VOS_UINT8* pgcSrcData, VOS_UINT16* pusLen)
{
    TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU  stSimLockPlmnInfo;
    VOS_UINT32                          i;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucMncLen;
    VOS_UINT32                          ulPlmnIdx;
    VOS_UINT32                          ulRsltChkPlmnBegin;
    VOS_UINT32                          ulRsltChkPlmnEnd;
    VOS_UINT32                          ulTotalPlmnNum;
    VOS_BOOL                            abPlmnValidFlg[TAF_MAX_SIM_LOCK_RANGE_NUM] = {VOS_FALSE};
    VOS_UINT8                           aucAsciiNumBegin[(TAF_PH_SIMLOCK_PLMN_STR_LEN * 2) + 1];
    VOS_UINT8                           aucAsciiNumEnd[(TAF_PH_SIMLOCK_PLMN_STR_LEN * 2) + 1];


    PS_MEM_SET(&stSimLockPlmnInfo, 0x00, sizeof(stSimLockPlmnInfo));

    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_CustomizeSimLockPlmnInfo,
                       &stSimLockPlmnInfo,
                       sizeof(stSimLockPlmnInfo)))
    {
        return AT_ERROR;
    }

    ulTotalPlmnNum = 0;

    if (NV_ITEM_DEACTIVE == stSimLockPlmnInfo.ulStatus)
    {
        /* 状态非激活时，显示Plmn个数为0 */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData,
                                           (VOS_CHAR *)pgcSrcData, "%s%s:%d%s",
                                           gaucAtCrLf,"^SIMLOCK",ulTotalPlmnNum,gaucAtCrLf);

        *pusLen = usLength;
        return AT_OK;
    }

    /*  判断Plmn号段是否有效 */
    for ( i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++ )
    {
        ucMncLen = stSimLockPlmnInfo.astSimLockPlmnRange[i].ucMncNum;

        ulRsltChkPlmnBegin = At_SimlockPlmnNumToAscii(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin,
                                    TAF_PH_SIMLOCK_PLMN_STR_LEN,aucAsciiNumBegin);

        ulRsltChkPlmnEnd = At_SimlockPlmnNumToAscii(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin,
                                    TAF_PH_SIMLOCK_PLMN_STR_LEN,aucAsciiNumEnd);

        if ((AT_SUCCESS == ulRsltChkPlmnBegin)
         && (AT_SUCCESS == ulRsltChkPlmnEnd)
         && ((ucMncLen <= 3) && (ucMncLen >= 2)))
        {
            abPlmnValidFlg[i] = VOS_TRUE;
            ++ ulTotalPlmnNum;
        }
    }

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData,
                                       (VOS_CHAR *)pgcSrcData, "%s%s:%d%s",
                                       gaucAtCrLf,"^SIMLOCK",ulTotalPlmnNum,gaucAtCrLf);

    if (0 == ulTotalPlmnNum)
    {
        *pusLen = usLength;
        return AT_OK;
    }

    /* CustomInfoSetting 工具软件要求的格式 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData,
                                (VOS_CHAR *)pgcSrcData + usLength,
                                "^SIMLOCK:");

    ulPlmnIdx = 0;
    for ( i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++ )
    {
        if (VOS_FALSE == abPlmnValidFlg[i])
        {
            continue;
        }

        ++ ulPlmnIdx;

        At_SimlockPlmnNumToAscii(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeBegin,
                                    TAF_PH_SIMLOCK_PLMN_STR_LEN,aucAsciiNumBegin);

        At_SimlockPlmnNumToAscii(stSimLockPlmnInfo.astSimLockPlmnRange[i].aucRangeEnd,
                                    TAF_PH_SIMLOCK_PLMN_STR_LEN,aucAsciiNumEnd);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData,
                                    (VOS_CHAR *)pgcSrcData + usLength,
                                    "%d,%d,%s,%s%s",
                                    ulPlmnIdx,
                                    stSimLockPlmnInfo.astSimLockPlmnRange[i].ucMncNum,
                                    aucAsciiNumBegin,
                                    aucAsciiNumEnd,
                                    gaucAtCrLf);
    }

    *pusLen = usLength;

    return AT_OK;
}




VOS_UINT32 AT_SetRsfrPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    VOS_UINT16                          usLength = 0;
    VOS_CHAR                            cName[AT_RSFR_RSFW_NAME_LEN + 1]       = {0};
    VOS_CHAR                            cSubName[AT_RSFR_RSFW_SUBNAME_LEN + 1] = {0};
    VOS_UINT8                          *pcRsfrSrcItem;
    VOS_CHAR                           *pcRsfrDstItem;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数个数不为两个 */
    if (gucAtParaIndex != 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数为空 */
    if ((0 == gastAtParaList[0].usParaLen)
        || (gastAtParaList[0].usParaLen > AT_RSFR_RSFW_NAME_LEN))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_CPY(cName, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    At_UpString((VOS_UINT8*)cName, gastAtParaList[0].usParaLen);

    if (gastAtParaList[1].usParaLen > AT_RSFR_RSFW_SUBNAME_LEN)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_CPY(cSubName, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);

    if(ERR_MSP_SUCCESS == AT_STRCMP(cName, "VERSION"))
    {
        if(TAF_SUCCESS == AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                                gastAtClientTab[ucIndex].opId,
                                                DRV_AGENT_VERSION_QRY_REQ,
                                                VOS_NULL_PTR,
                                                0,
                                                I0_WUEPS_PID_DRV_AGENT))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_RSFR_VERSION_QRY;
            return AT_WAIT_ASYNC_RETURN;
        }
        else
        {
            return AT_ERROR;
        }
    }
    else if(ERR_MSP_SUCCESS == AT_STRCMP(cName, "SIMLOCK"))
    {
        pcRsfrSrcItem = (VOS_UINT8*)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
        if (VOS_NULL_PTR == pcRsfrSrcItem )
        {
            return AT_ERROR;
        }

        PS_MEM_SET(pcRsfrSrcItem, 0, AT_RSFR_RSFW_MAX_LEN);

        ulRet = AT_RsfrSimLockProc(pcRsfrSrcItem, &usLength);
        if(AT_OK != ulRet)
        {
            PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
            return ulRet;
        }

        pcRsfrDstItem = (VOS_CHAR*)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
        if (VOS_NULL_PTR == pcRsfrDstItem )
        {
            PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
            return AT_ERROR;
        }

        PS_MEM_SET(pcRsfrDstItem, 0, AT_RSFR_RSFW_MAX_LEN);

        /* 把从SIMLOCK命令中读出来的结果字符串作为编码源 */
        AtBase64Encode(pcRsfrSrcItem, usLength, pcRsfrDstItem);

        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                    "^RSFR:\"%s\",\"%s\",123,1,1,%s",
                                    cName, cSubName, gaucAtCrLf);

        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                    "\"%s\"", pcRsfrDstItem);

        PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
        PS_MEM_FREE(WUEPS_PID_AT, pcRsfrDstItem);

        return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }
}



VOS_VOID AT_SetRsfrVersionCnf(VOS_UINT8 ucIndex, VOS_UINT8* pucData, VOS_UINT32 ulLen)
{
    VOS_UINT16 usLength;
    VOS_CHAR* pcRsfrItem = NULL;

    pcRsfrItem = (VOS_CHAR*)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
    if (VOS_NULL_PTR == pcRsfrItem )
    {
        return ;
    }

    PS_MEM_SET(pcRsfrItem, 0, AT_RSFR_RSFW_MAX_LEN);

    /* 把从SIMLOCK命令中读出来的结果字符串作为编码源 */
    AtBase64Encode(pucData, ulLen, pcRsfrItem);

    usLength = 0;
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                "^RSFR:\"VERSION\",\"111\",125,1,1,%s",
                                gaucAtCrLf);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                "\"%s\"", pcRsfrItem);

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex, AT_OK);

    PS_MEM_FREE(WUEPS_PID_AT, pcRsfrItem);

    return ;
}
VOS_VOID AT_RsfwSimLockProc(VOS_UINT8 *pucData, VOS_UINT16 usLen)
{
    VOS_UINT16 i = 0;
    VOS_UINT32 usNum = 0;

    /* 补充AT两个字符 */
    pucData[0] = 'A';
    pucData[1] = 'T';
    pucData[10] = '=';      /* 把冒号改为等号 */

    if(usLen < 13)
    {
        return ;
    }

    if(('1' == pucData[11]) && (',' == pucData[12]))
    {

        /* 工具下发的字符串里MNC长度添的是0，需要改为2或3 */
        for(i = 11; i < usLen; i++)
        {
            if(',' == pucData[i])
            {
                usNum++;
                if(i+8 >= usLen)
                {
                    return ;
                }

                /* 通过判断PLMN的位数来确定是2还是3 */
                if((1 == (usNum%3)) && ('0' == pucData[i+1]) && (',' == pucData[i+2]))
                {
                    if(',' == pucData[i+8])
                    {
                        pucData[i+1] = '2';
                    }
                    else
                    {
                        pucData[i+1] = '3';
                    }
                }
            }
        }
    }

    return ;
}


VOS_UINT32 AT_SetRsfwPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32  ulRet;
    VOS_UINT32  ulLength = 0;
    VOS_CHAR    cName[AT_RSFR_RSFW_NAME_LEN + 1]   = {0};
    VOS_UINT8*  pucRsfw  = NULL;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数个数不为六个 */
    if (gucAtParaIndex != 6)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数为空 */
    if ((0 == gastAtParaList[0].usParaLen)
        || (gastAtParaList[0].usParaLen > AT_RSFR_RSFW_NAME_LEN))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_CPY(cName, gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen);

    At_UpString((VOS_UINT8*)cName, gastAtParaList[0].usParaLen);

    /* CustomInfoSetting 工具软件只检查参数<name>，其余参数不检查 */

    if(ERR_MSP_SUCCESS == AT_STRCMP(cName, "SIMLOCK"))
    {
        if(gastAtParaList[5].usParaLen > AT_PARA_MAX_LEN)
        {
            vos_printf("[AT_SetRsfwPara] : param 5 is too long : %d.\n", gastAtParaList[5].usParaLen);

            return AT_ERROR;
        }

        pucRsfw = (TAF_UINT8*)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
        if (VOS_NULL_PTR == pucRsfw )
        {
            return AT_ERROR;
        }

        PS_MEM_SET(pucRsfw, 0, AT_RSFR_RSFW_MAX_LEN);

        /* 前两个字符用来补充AT，然后送给^SIMLOCK命令的处理函数处理 */
        ulRet = AtBase64Decode(gastAtParaList[5].aucPara, gastAtParaList[5].usParaLen,
            &(pucRsfw[2]), (AT_RSFR_RSFW_MAX_LEN-2), &ulLength);
        if(b64Result_OK != ulRet)
        {
            PS_MEM_FREE(WUEPS_PID_AT, pucRsfw);
            return AT_ERROR;
        }

        /* 整理从CustomInfoSetting接收到的字符串 */
        AT_RsfwSimLockProc(pucRsfw, (VOS_UINT16)(ulLength + 2));

        ulRet = At_ProcSimLockPara(ucIndex, pucRsfw, (VOS_UINT16)(ulLength + 2));

        if(AT_SUCCESS != ulRet)
        {
            vos_printf("[AT_SetRsfwPara] : At_ProcSimLockPara failed %d.\n", ulRet);
        }

        PS_MEM_FREE(WUEPS_PID_AT, pucRsfw);

        /* AT_ERROR AT_OK在At_ProcSimLockPara中返回 */
        return AT_SUCCESS;
    }
#if (FEATURE_ON == FEATURE_VSIM)
    else if(VOS_OK == AT_STRCMP(cName, "VSIM"))
    {
        return At_DealRSFWVsim(ucIndex);
    }
#endif  /*(FEATURE_ON == FEATURE_VSIM)*/
    else
    {
        return AT_ERROR;
    }
}



VOS_UINT32 AT_SetHukPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    DRV_AGENT_HUK_SET_REQ_STRU          stHukSetReq;

    /* 局部变量初始化 */
    ulResult = VOS_NULL;
    usLength = gastAtParaList[0].usParaLen;
    PS_MEM_SET(&stHukSetReq, 0x00, sizeof(DRV_AGENT_HUK_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 检查码流参数长度 */
    if (AT_HUK_PARA_LEN != usLength)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 将字符串参数转换为码流 */
    ulResult = At_AsciiNum2HexString(gastAtParaList[0].aucPara, &usLength);
    if ( (AT_SUCCESS != ulResult) || (DRV_AGENT_HUK_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    PS_MEM_CPY(stHukSetReq.aucHUK, gastAtParaList[0].aucPara, DRV_AGENT_HUK_LEN);

    /* 转换成功, 发送跨核消息到C核, 设置HUK */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_HUK_SET_REQ,
                                      &stHukSetReq,
                                      sizeof(DRV_AGENT_HUK_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetHukPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_HUK_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_DecodeFacAuthPubkeyPara(AT_FACAUTHPUBKEY_SET_REQ_STRU *pstFacAuthPubKey)
{
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;

    /* 局部变量初始化 */
    ulResult = VOS_NULL;
    usLength = AT_FACAUTHPUBKEY_PARA_LEN;

    ulResult = At_AsciiNum2HexString(pstFacAuthPubKey->aucPubKey, &usLength);
    if ( (AT_SUCCESS != ulResult) || (DRV_AGENT_PUBKEY_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    usLength = AT_FACAUTHPUBKEY_SIGN_PARA_LEN;

    ulResult = At_AsciiNum2HexString(pstFacAuthPubKey->aucPubKeySign, &usLength);
    if ( (AT_SUCCESS != ulResult) || (DRV_AGENT_PUBKEY_SIGNATURE_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}
VOS_UINT32 AT_SetFacAuthPubkeyPara(
    VOS_UINT8                           ucIndex,
    AT_FACAUTHPUBKEY_SET_REQ_STRU      *pstFacAuthPubKey
)
{
    VOS_UINT32                              ulResult;
    DRV_AGENT_FACAUTHPUBKEY_SET_REQ_STRU    stFacAuthPubkeySetReq;

    /* 局部变量初始化 */
    PS_MEM_SET(&stFacAuthPubkeySetReq, 0x00, sizeof(DRV_AGENT_FACAUTHPUBKEY_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 将字符串参数转换为码流 */
    ulResult = AT_DecodeFacAuthPubkeyPara(pstFacAuthPubKey);
    if (AT_SUCCESS != ulResult)
    {
        return ulResult;
    }
    PS_MEM_CPY(stFacAuthPubkeySetReq.aucPubKey,
               pstFacAuthPubKey->aucPubKey,
               DRV_AGENT_PUBKEY_LEN);
    PS_MEM_CPY(stFacAuthPubkeySetReq.aucPubKeySign,
               pstFacAuthPubKey->aucPubKeySign,
               DRV_AGENT_PUBKEY_SIGNATURE_LEN);

    /* 转换成功, 发送跨核消息到C核, 设置产线公钥 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_FACAUTHPUBKEY_SET_REQ,
                                      &stFacAuthPubkeySetReq,
                                      sizeof(DRV_AGENT_FACAUTHPUBKEY_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetFacAuthPubkeyPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 由于FACAUTHPUBKEY特殊处理，需要手动启动定时器*/
    if (AT_SUCCESS != At_StartTimer(AT_SET_PARA_TIME, ucIndex))
    {
        AT_WARN_LOG("AT_SetFacAuthPubkeyPara: At_StartTimer fail.");
        return AT_ERROR;
    }

    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_FACAUTHPUBKEY_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetIdentifyStartPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送跨核消息到C核, 发起鉴权握手 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_IDENTIFYSTART_SET_REQ,
                                      VOS_NULL_PTR,
                                      0,
                                      I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetIdentifyStartPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_IDENTIFYSTART_SET;
    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 AT_SetIdentifyEndPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    DRV_AGENT_IDENTIFYEND_SET_REQ_STRU  stIdentifyEndSetReq;
    VOS_UINT16                          usLength;

    /* 局部变量初始化 */
    ulResult = VOS_NULL;
    usLength = gastAtParaList[0].usParaLen;
    PS_MEM_SET(&stIdentifyEndSetReq, 0x00, sizeof(DRV_AGENT_IDENTIFYEND_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数错误 */
    if (1 < gucAtParaIndex)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 检查码流参数长度 */
    if (AT_RSA_CIPHERTEXT_PARA_LEN != usLength)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 将字符串参数转换为码流 */
    ulResult = At_AsciiNum2HexString(gastAtParaList[0].aucPara, &usLength);
    if ( (AT_SUCCESS != ulResult) || (DRV_AGENT_RSA_CIPHERTEXT_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    PS_MEM_CPY(stIdentifyEndSetReq.aucRsaText,
               gastAtParaList[0].aucPara,
               DRV_AGENT_RSA_CIPHERTEXT_LEN);

    /* 发送跨核消息到C核, 完成产线鉴权 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_IDENTIFYEND_SET_REQ,
                                      &stIdentifyEndSetReq,
                                      sizeof(DRV_AGENT_IDENTIFYEND_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetIdentifyEndPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_IDENTIFYEND_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetSimlockDataWritePara(
    VOS_UINT8                           ucIndex,
    AT_SIMLOCKDATAWRITE_SET_REQ_STRU   *pstSimlockDataWrite
)
{
    VOS_UINT32                                  ulResult;
    VOS_UINT16                                  usLength;
    DRV_AGENT_SIMLOCKDATAWRITE_SET_REQ_STRU     stSimlockDataWriteSetReq;

    /* 局部变量初始化 */
    ulResult = VOS_NULL;
    usLength = AT_SIMLOCKDATAWRITE_PARA_LEN;
    PS_MEM_SET(&stSimlockDataWriteSetReq,
               0x00,
               sizeof(DRV_AGENT_SIMLOCKDATAWRITE_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 将字符串参数转换为码流 */
    ulResult = At_AsciiNum2HexString(pstSimlockDataWrite->aucCategoryData, &usLength);
    if ( (AT_SUCCESS != ulResult)
      || (DRV_AGENT_SIMLOCKDATAWRITE_STRU_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    PS_MEM_CPY(stSimlockDataWriteSetReq.aucCategoryData,
               pstSimlockDataWrite->aucCategoryData,
               DRV_AGENT_SIMLOCKDATAWRITE_STRU_LEN);

    /* 转换成功, 发送跨核消息到C核, 写入锁网锁卡数据 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_SIMLOCKDATAWRITE_SET_REQ,
                                      &stSimlockDataWriteSetReq,
                                      sizeof(DRV_AGENT_SIMLOCKDATAWRITE_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetSimlockDataWritePara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 由于SIMLOCKDATAWRITE特殊处理，需要手动启动定时器*/
    if (AT_SUCCESS != At_StartTimer(AT_SET_PARA_TIME, ucIndex))
    {
        AT_WARN_LOG("AT_SetSimlockDataWritePara: At_StartTimer fail.");
        return AT_ERROR;
    }

    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SIMLOCKDATAWRITE_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_CheckPhonePhyNumPara(DRV_AGENT_PH_PHYNUM_TYPE_ENUM_UINT8 *penSetType)
{
    /* <type>参数检查: 支持设置物理号IMEI或SN，否则, 直接返回物理号类型错误 */
    /* 全部格式化为大写字符 */
    if (AT_FAILURE == At_UpString(gastAtParaList[0].aucPara, gastAtParaList[0].usParaLen))
    {
        return AT_PERSONALIZATION_PH_PHYNUM_TYPE_ERROR;
    }

    if (0 == VOS_StrCmp((VOS_CHAR *)gastAtParaList[0].aucPara, "IMEI"))
    {
        *penSetType = DRV_AGENT_PH_PHYNUM_IMEI;
    }
    else if (0 == VOS_StrCmp((VOS_CHAR *)gastAtParaList[0].aucPara, "SN"))
    {
        *penSetType = DRV_AGENT_PH_PHYNUM_SN;
    }
    else
    {
        AT_NORM_LOG("AT_CheckPhonePhyNumPara: the type of physical is error.");
        return AT_PERSONALIZATION_PH_PHYNUM_TYPE_ERROR;
    }

    /* 参数长度检查 */
    if (AT_RSA_CIPHERTEXT_PARA_LEN != gastAtParaList[1].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}
VOS_UINT32 AT_SetPhonePhynumPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulCheckParaRst;
    DRV_AGENT_PHONEPHYNUM_SET_REQ_STRU  stPhonePhynumSetReq;
    VOS_UINT16                          usLength;

    /* 局部变量初始化 */
    ulResult        = VOS_NULL;
    ulCheckParaRst  = VOS_NULL;
    usLength        = gastAtParaList[1].usParaLen;
    PS_MEM_SET(&stPhonePhynumSetReq, 0x00, sizeof(DRV_AGENT_PHONEPHYNUM_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (2 < gucAtParaIndex)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 参数个数错误 */
    if (2 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulCheckParaRst = AT_CheckPhonePhyNumPara(&(stPhonePhynumSetReq.enPhynumType));
    if (AT_SUCCESS != ulCheckParaRst)
    {
        return ulCheckParaRst;
    }

    /* 将字符串参数转换为码流 */
    ulResult = At_AsciiNum2HexString(gastAtParaList[1].aucPara, &usLength);
    if ( (AT_SUCCESS != ulResult) || (DRV_AGENT_RSA_CIPHERTEXT_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    PS_MEM_CPY(stPhonePhynumSetReq.aucPhynumValue,
               gastAtParaList[1].aucPara,
               DRV_AGENT_RSA_CIPHERTEXT_LEN);

    /* 发送跨核消息到C核, 完成产线鉴权 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_PHONEPHYNUM_SET_REQ,
                                      &stPhonePhynumSetReq,
                                      sizeof(DRV_AGENT_PHONEPHYNUM_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetPhonePhynumPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PHONEPHYNUM_SET;
    return AT_WAIT_ASYNC_RETURN;

}



VOS_UINT32 AT_SetPortCtrlTmpPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                              ulResult;
    DRV_AGENT_PORTCTRLTMP_SET_REQ_STRU      stPortCtrlTmpSetReq;
    VOS_UINT8                               aucPara[AT_PORTCTRLTMP_PARA_LEN];

    /* 局部变量初始化 */
    PS_MEM_SET(&stPortCtrlTmpSetReq, 0x00, sizeof(DRV_AGENT_PORTCTRLTMP_SET_REQ_STRU));
    PS_MEM_SET(aucPara, 0x00, AT_PORTCTRLTMP_PARA_LEN);

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

     /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 检查码流参数长度 */
    if (AT_PORTCTRLTMP_PARA_LEN != gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 密码的合法性检查，密码为16个“0”-“9”ASCII字符,密码由ASCII码转换为十进制数字 */
    ulResult = At_AsciiNum2Num(aucPara, gastAtParaList[0].aucPara, DRV_AGENT_PORT_PASSWORD_LEN);
    if ( AT_FAILURE == ulResult)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数检查成功, 发送跨核消息到C核 */
    PS_MEM_CPY(stPortCtrlTmpSetReq.aucPortPassword, aucPara, DRV_AGENT_PORT_PASSWORD_LEN);
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_PORTCTRLTMP_SET_REQ,
                                      &stPortCtrlTmpSetReq,
                                      sizeof(DRV_AGENT_PORTCTRLTMP_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetPortCtrlTmpPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PORTCTRLTMP_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetPortAttribSetPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;
    DRV_AGENT_PORTATTRIBSET_SET_REQ_STRU    stPortAttribSetReq;

    /* 局部变量初始化 */
    PS_MEM_SET(&stPortAttribSetReq, 0x00, sizeof(DRV_AGENT_PORTATTRIBSET_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

     /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 检查码流参数长度 */
    if ( (AT_PORTATTRIBSET_PARA_ONOFF_LEN != gastAtParaList[0].usParaLen)
      || (AT_RSA_CIPHERTEXT_PARA_LEN      != gastAtParaList[1].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置查询端口锁状态 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        stPortAttribSetReq.enPortStatus = DRV_AGENT_PORT_STATUS_OFF;
    }
    else
    {
        stPortAttribSetReq.enPortStatus = DRV_AGENT_PORT_STATUS_ON;
    }

    /* 将字符串参数转换为码流 */
    usLength = gastAtParaList[1].usParaLen;
    ulResult = At_AsciiNum2HexString(gastAtParaList[1].aucPara, &usLength);
    if ( (TAF_SUCCESS != ulResult) || (DRV_AGENT_RSA_CIPHERTEXT_LEN != usLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_CPY(stPortAttribSetReq.aucPortPassword,
               gastAtParaList[1].aucPara,
               DRV_AGENT_RSA_CIPHERTEXT_LEN);
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_PORTATTRIBSET_SET_REQ,
                                      &stPortAttribSetReq,
                                      sizeof(DRV_AGENT_PORTATTRIBSET_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetPortAttribSetPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PORTATTRIBSET_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetApSimStPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                              ulResult;
    DRV_AGENT_AP_SIMST_SET_REQ_STRU         stApSimStSetReq;

    /* 局部变量初始化 */
    PS_MEM_SET(&stApSimStSetReq, 0x00, sizeof(DRV_AGENT_AP_SIMST_SET_REQ_STRU));

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_ERROR;
    }

     /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /* 参数设置为1时，目前不支持，直接返回OK */
    if (1 == gastAtParaList[0].ulParaValue)
    {
        stApSimStSetReq.ulUsimState = DRV_AGENT_USIM_OPERATE_ACT;
    }
    else
    {
        stApSimStSetReq.ulUsimState = DRV_AGENT_USIM_OPERATE_DEACT;
    }

    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      DRV_AGENT_AP_SIMST_SET_REQ,
                                      &stApSimStSetReq,
                                      sizeof(DRV_AGENT_AP_SIMST_SET_REQ_STRU),
                                      I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetApSimStPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APSIMST_SET;

    return AT_WAIT_ASYNC_RETURN;

}


VOS_VOID AT_ConvertMccToNasType(
    VOS_UINT32                          ulMcc,
    VOS_UINT32                         *pulNasMcc
)
{
    VOS_UINT32                          aulTmp[3];
    VOS_UINT32                          i;

    *pulNasMcc  = 0;

    for (i = 0; i < 3 ; i++ )
    {
        aulTmp[i]       = ulMcc & 0x0f;
        ulMcc         >>=  4;
    }

    *pulNasMcc  = aulTmp[2]
             |((VOS_UINT32)aulTmp[1] << 8)
             |((VOS_UINT32)aulTmp[0] << 16);

}


VOS_UINT32 AT_SetNvmEccNumPara(
    VOS_UINT8                           ucIndex
)
{
    MN_CALL_APP_CUSTOM_ECC_NUM_REQ_STRU stEccNumReq;
    VOS_UINT32                          ulRst;
    VOS_UINT32                          ulTemp;
    VOS_UINT32                          ulMcc;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        AT_ERR_LOG("AT_SetNvmEccNumSetPara: Cmd Opt Type is wrong.");
        return AT_ERROR;
    }

    /* 参数过多 */
    if ( (gucAtParaIndex < 6) || (gucAtParaIndex > 7) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数合法性检查, index > total认为设置无效 */
    if (gastAtParaList[0].ulParaValue > gastAtParaList[1].ulParaValue)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 消息初始化 */
    VOS_MemSet(&stEccNumReq, 0, sizeof(MN_CALL_APP_CUSTOM_ECC_NUM_REQ_STRU));

    /* 将ECC NUM转换为BCD码 */
    if (MN_ERR_NO_ERROR != AT_AsciiNumberToBcd((TAF_CHAR*)&gastAtParaList[2].aucPara[0],
                                                stEccNumReq.stEccNum.aucBcdNum,
                                                &stEccNumReq.stEccNum.ucNumLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulTemp  = 0;
    ulTemp |= (gastAtParaList[5].ulParaValue % 10);
    ulTemp |= (((gastAtParaList[5].ulParaValue /10) % 10) << 4);
    ulTemp |= (((gastAtParaList[5].ulParaValue /100) % 10) << 8);

    /* 将MCC转化为NAS结构 */
    AT_ConvertMccToNasType(ulTemp, &ulMcc);

    stEccNumReq.ucIndex         = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stEccNumReq.ucTotal         = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    stEccNumReq.ucCategory      = (VOS_UINT8)gastAtParaList[3].ulParaValue;
    stEccNumReq.ucSimPresent    = (VOS_UINT8)gastAtParaList[4].ulParaValue;
    stEccNumReq.ulMcc           = ulMcc;

    if (gucAtParaIndex == 7)
    {
        stEccNumReq.ucAbnormalServiceFlg    = (VOS_UINT8)gastAtParaList[6].ulParaValue;
    }
    else
    {
        /* 默认有卡非正常服务状态时紧急呼号码标示默认值为FALSE */
        stEccNumReq.ucAbnormalServiceFlg    = VOS_FALSE;
    }

    /* 发送消息 */
    ulRst = MN_CALL_SendAppRequest(MN_CALL_APP_CUSTOM_ECC_NUM_REQ,
                                   gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   0,
                                  (MN_CALL_APP_REQ_PARM_UNION*)&stEccNumReq);

    if (MN_ERR_NO_ERROR != ulRst)
    {
        AT_WARN_LOG("AT_SetNvmEccNumPara: MN_CALL_SendAppRequest fail.");
        return AT_ERROR;
    }

    /* 设置结束直接返回OK */
    return AT_OK;
}
VOS_UINT32 AT_SetCposPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_AGPS_CTX_STRU             *pstAgpsCtx = VOS_NULL_PTR;

    pstAgpsCtx = AT_GetModemAgpsCtxAddrFromClientId(ucIndex);

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 为准备输入的XML码流申请内存 */
    pstAgpsCtx->stXml.pcXmlTextHead = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_XML_MAX_LEN);
    if (VOS_NULL_PTR == pstAgpsCtx->stXml.pcXmlTextHead)
    {
        AT_ERR_LOG("AT_SetCposPara: Memory malloc failed!");
        return AT_ERROR;
    }
    pstAgpsCtx->stXml.pcXmlTextCur  = pstAgpsCtx->stXml.pcXmlTextHead;

    /* 切换为AGPS XML码流输入模式 */
    At_SetMode(ucIndex, AT_CMD_MODE, AT_XML_MODE);

    /* 返回提示XML码流输入状态 */
    return AT_WAIT_XML_INPUT;
}
VOS_UINT32 AT_SetCposrPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_AGPS_CTX_STRU             *pstAgpsCtx = VOS_NULL_PTR;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    pstAgpsCtx = AT_GetModemAgpsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    pstAgpsCtx->enCposrReport = (AT_CPOSR_FLG_ENUM_UNIT8)gastAtParaList[0].ulParaValue;

    return AT_OK;
}
VOS_UINT32 AT_SetXcposrPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_AGPS_CTX_STRU             *pstAgpsCtx = VOS_NULL_PTR;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    pstAgpsCtx = AT_GetModemAgpsCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if( gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 保存模式 */
    pstAgpsCtx->enXcposrReport = (AT_XCPOSR_FLG_ENUM_UNIT8)gastAtParaList[0].ulParaValue;
    return AT_OK;
}


VOS_UINT32 AT_SetCgpsClockPara(VOS_UINT8 ucIndex)
{
    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if( gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_ON == FEATURE_AGPS)
    if (1 == gastAtParaList[0].ulParaValue)
    {
        if (VOS_OK != set_gps_ref_clk_enable(VOS_TRUE))
        {
            AT_ERR_LOG("AT_SetCgpsClockPara: set_gps_ref_clk_enable failed!");
            return AT_ERROR;
        }
    }
    else if (0 == gastAtParaList[0].ulParaValue)
    {
        if (VOS_OK != set_gps_ref_clk_enable(VOS_FALSE))
        {
            AT_ERR_LOG("AT_SetCgpsClockPara: set_gps_ref_clk_enable failed!");
            return AT_ERROR;
        }
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
#else
    return AT_OK;
#endif
}
VOS_UINT32 AT_SetGpsInfoPara(VOS_UINT8 ucIndex)
{
    return AT_OK;
}


#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
VOS_UINT32 AT_SetLogCfgPara(VOS_UINT8 ucIndex)
{
    NV_SOCP_SDLOG_CFG_STRU              stSocpCfg;
    VOS_UINT32                          ulResult;

    /* 当输入^logcfg=1时，后边三个参数不填，会当成0处理，但SOCP无法分配流水线为0K的缓存，导致单板会不停复位 */
    if (((1 == gucAtParaIndex) && (0 == gastAtParaList[0].ulParaValue)) || (4 == gucAtParaIndex))
    {
        /* 参数合法，继续执行 */
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stSocpCfg.ulSocpDelayWriteFlg   = gastAtParaList[0].ulParaValue;
    stSocpCfg.ulGuSocpLevel         = gastAtParaList[1].ulParaValue;
    stSocpCfg.ulLSocpLevel          = gastAtParaList[2].ulParaValue;
    stSocpCfg.ulTimeOutValue        = gastAtParaList[3].ulParaValue;

    BSP_SOCP_RefreshSDLogCfg(stSocpCfg.ulTimeOutValue);

    ulResult = NV_WriteEx(MODEM_ID_0, en_NV_ID_SOCP_SDLOG_CFG, (VOS_VOID *)(&stSocpCfg), sizeof(NV_SOCP_SDLOG_CFG_STRU));

    if (VOS_OK != ulResult)
    {
        return AT_ERROR;
    }

    return AT_OK;
}
#else
VOS_UINT32 AT_SetLogCfgPara(VOS_UINT8 ucIndex)
{
    NV_SOCP_SDLOG_CFG_STRU              stSocpCfg;
    VOS_UINT32                          ulResult;

    /* 当输入^logcfg=1时，后边三个参数不填，会当成0处理，但SOCP无法分配流水线为0K的缓存，导致单板会不停复位 */
    if (((1 == gucAtParaIndex) && (0 == gastAtParaList[0].ulParaValue))
        || ((4 == gucAtParaIndex)&& (1 == gastAtParaList[0].ulParaValue)))
    {
        /* 参数合法，继续执行 */
    }
    else
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stSocpCfg.ulSocpDelayWriteFlg   = gastAtParaList[0].ulParaValue;
    stSocpCfg.ulIndSocpLevel        = gastAtParaList[1].ulParaValue;
    stSocpCfg.ulCfgSocpLevel        = gastAtParaList[2].ulParaValue;

    if (0 == stSocpCfg.ulSocpDelayWriteFlg)
    {
        /* 假如延迟写入关闭,默认为10ms */
        stSocpCfg.ulTimeOutValue    = 10;
    }
    else
    {
        stSocpCfg.ulTimeOutValue    = gastAtParaList[3].ulParaValue;
    }

    BSP_SOCP_RefreshSDLogCfg(stSocpCfg.ulTimeOutValue);

    ulResult = NV_WriteEx(MODEM_ID_0, en_NV_ID_SOCP_SDLOG_CFG, (VOS_VOID *)(&stSocpCfg), sizeof(NV_SOCP_SDLOG_CFG_STRU));

    if (VOS_OK != ulResult)
    {
        return AT_ERROR;
    }

    return AT_OK;
}
#endif
VOS_UINT32 AT_SetLogSavePara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* SOCP的接口在V9R1上由OAM提供，使用其原型 */
    if (BSP_OK != BSP_SOCP_EncDstBufFlush())
    {
        return AT_ERROR;
    }

    return AT_OK;
}


VOS_UINT32 AT_SetApSecPara(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucCmdPara,
    VOS_UINT16                          usParaLen )
{
    VOS_UINT32                          ulResult;
    AT_MTA_APSEC_REQ_STRU               stApSecSetReq;

    /* 初始化 */
    PS_MEM_SET(&stApSecSetReq, 0x00, sizeof(AT_MTA_APSEC_REQ_STRU));

    /* 将字符串参数转换为码流 */
    ulResult = At_AsciiNum2HexString(pucCmdPara, &usParaLen);
    if (AT_SUCCESS != ulResult)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stApSecSetReq.ulSPLen = (VOS_UINT32)usParaLen;
    PS_MEM_CPY(stApSecSetReq.aucSecPacket, pucCmdPara, stApSecSetReq.ulSPLen);

    /* 发送跨核消息ID_AT_MTA_APSEC_SET_REQ到C核 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      ID_AT_MTA_APSEC_SET_REQ,
                                      &stApSecSetReq,
                                      sizeof(AT_MTA_APSEC_REQ_STRU),
                                      I0_UEPS_PID_MTA);
    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetApSecPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APSEC_SET;

    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetCISAPara(
    VOS_UINT8                           ucIndex,
    VOS_UINT32                          ulLengthValue,
    VOS_UINT8                          *pucCommand,
    VOS_UINT16                          usCommandLength)
{
    SI_PIH_ISDB_ACCESS_COMMAND_STRU     stCommand;
    VOS_UINT16                          usCommandHexLen;

    /* 初始化 */
    PS_MEM_SET(&stCommand, 0x00, sizeof(stCommand));

    /* 参数检查 */
    if ( (ulLengthValue > (SI_APDU_MAX_LEN * 2))
       ||(ulLengthValue != usCommandLength) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <command>是ASCII码，做输入参数到HEX的转换 */
    usCommandHexLen =   usCommandLength;
    if (AT_FAILURE == At_AsciiNum2HexString(pucCommand, &usCommandHexLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<length> */
    stCommand.ulLen = usCommandHexLen;

    /* 设置<command> */
    PS_MEM_CPY((TAF_VOID *)stCommand.aucCommand, (TAF_VOID *)pucCommand, (VOS_UINT16)stCommand.ulLen);

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PIH_IsdbAccessReq(gastAtClientTab[ucIndex].usClientId, 0, &stCommand))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CISA_SET;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        AT_WARN_LOG("AT_SetCISAPara: SI_PIH_IsdbAccessReq fail.");

        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetCmutPara(VOS_UINT8 ucIndex)
{
    APP_VC_MUTE_STATUS_ENUM_UINT8        enMuteStatus;

    /* 无参数 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多或者错误 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取命令参数, 提取要设置的静音状态 */
    if (0 == gastAtParaList[0].ulParaValue)
    {
        enMuteStatus = APP_VC_MUTE_STATUS_UNMUTED;
    }
    else
    {
        enMuteStatus = APP_VC_MUTE_STATUS_MUTED;
    }

    /* 执行命令操作 */
    if (VOS_OK != APP_VC_SetMuteStatus(gastAtClientTab[ucIndex].usClientId,
                                       0,
                                       enMuteStatus))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMUT_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32  AT_SetSimlockUnlockPara( VOS_UINT8 ucIndex )
{
    VOS_UINT32                          ulResult;
    AT_MTA_SIMLOCKUNLOCK_REQ_STRU       stSimlockUnlockSetReq;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (2 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 解析锁网锁卡解锁参数 */
    if ( (0 == gastAtParaList[0].usParaLen)
      || (AT_SIMLOCKUNLOCK_PWD_PARA_LEN != gastAtParaList[1].usParaLen)
      || (gastAtParaList[0].ulParaValue > AT_MTA_PERS_CATEGORY_SERVICE_PROVIDER) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stSimlockUnlockSetReq, 0, sizeof(AT_MTA_SIMLOCKUNLOCK_REQ_STRU));
    stSimlockUnlockSetReq.enCategory = (AT_MTA_PERS_CATEGORY_ENUM_UINT8)gastAtParaList[0].ulParaValue;

    /* 密码的合法性检查，密码为16个“0”-“9”ASCII字符,密码由ASCII码转换为十进制数字 */
    ulResult = At_AsciiNum2Num(stSimlockUnlockSetReq.aucPassword,
                               gastAtParaList[1].aucPara,
                               gastAtParaList[1].usParaLen);
    if ( AT_SUCCESS != ulResult)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送跨核消息到C核, 解锁锁网锁卡 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      ID_AT_MTA_SIMLOCKUNLOCK_SET_REQ,
                                      &stSimlockUnlockSetReq,
                                      sizeof(AT_MTA_SIMLOCKUNLOCK_REQ_STRU),
                                      I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetSimlockUnlockPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SIMLOCKUNLOCK_SET;
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_FillApdsPara(
    VOS_UINT8                           ucIndex,
    MN_CALL_ORIG_PARAM_STRU            *pstOrigParam
)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 默认情况是使用CCA设置的默认值 */
    pstOrigParam->enCallMode  = pstSsCtx->enCModType;
    pstOrigParam->stDataCfg   = pstSsCtx->stCbstDataCfg;
    pstOrigParam->enClirCfg   = pstSsCtx->ucClirType;
    pstOrigParam->stCugCfg    = pstSsCtx->stCcugCfg;

    /* 设置<dial_string> */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return VOS_ERR;
    }
    else
    {
        if (VOS_OK !=  AT_FillCalledNumPara(gastAtParaList[0].aucPara,
                                            gastAtParaList[0].usParaLen,
                                            &pstOrigParam->stDialNumber))
        {
            return VOS_ERR;
        }
    }

    /* 检查被叫号码子地址的长度 */
    if (gastAtParaList[1].usParaLen > MN_CALL_MAX_SUBADDR_INFO_LEN)
    {
        return VOS_ERR;
    }

    /* 设置<sub_string> */
    if (0 != gastAtParaList[1].usParaLen)
    {
        pstOrigParam->stSubaddr.IsExist       = VOS_TRUE;

        /* 偏移量 = 子地址编码的长度 + Octet3的长度*/
        pstOrigParam->stSubaddr.LastOctOffset = (VOS_UINT8)gastAtParaList[1].usParaLen + sizeof(pstOrigParam->stSubaddr.Octet3);
        pstOrigParam->stSubaddr.Octet3        = (0x80 | (MN_CALL_SUBADDR_NSAP << 4));
        PS_MEM_CPY(pstOrigParam->stSubaddr.SubAddrInfo,
                   gastAtParaList[1].aucPara,
                   gastAtParaList[1].usParaLen);
    }

    /* 设置<I><G><call_type><voice_domain>参数 */
    if (0 != gastAtParaList[2].usParaLen)
    {
        if (0 == gastAtParaList[2].ulParaValue)
        {
            pstOrigParam->enClirCfg = AT_CLIR_INVOKE;
        }
        else if (1 == gastAtParaList[2].ulParaValue)
        {
            pstOrigParam->enClirCfg = AT_CLIR_SUPPRESS;
        }
        else
        {
            return VOS_ERR;
        }
    }

    if (0 != gastAtParaList[3].usParaLen)
    {
        pstOrigParam->stCugCfg.bEnable    = gastAtParaList[3].ulParaValue;
    }

    if (0 != gastAtParaList[4].usParaLen)
    {
        pstOrigParam->enCallType          = (VOS_UINT8)gastAtParaList[4].ulParaValue;
    }

    if (0 != gastAtParaList[5].usParaLen)
    {
        pstOrigParam->enVoiceDomain       = (VOS_UINT8)gastAtParaList[5].ulParaValue;
    }

    return VOS_OK;
}


VOS_UINT32 AT_SetApdsPara(VOS_UINT8 ucIndex)
{
    MN_CALL_ORIG_PARAM_STRU             stOrigParam;
    VOS_UINT32                          ulRelt;

    /* 初始化 */
    PS_MEM_SET(&stOrigParam, 0x00, sizeof(MN_CALL_ORIG_PARAM_STRU));

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

     /* 参数过多 */
    if (gucAtParaIndex > 6)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 检查并填写输入的参数 */
    ulRelt = AT_FillApdsPara(ucIndex, &stOrigParam);
    if (VOS_OK != ulRelt)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送消息原语 */
    if (AT_SUCCESS == MN_CALL_Orig(gastAtClientTab[ucIndex].usClientId,
                                  0,
                                  &gastAtClientTab[ucIndex].AtCallInfo.CallId,
                                  &stOrigParam))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APDS_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_UNKNOWN);
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetClprPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送跨核消息AT_CALL_CLPR_GET_REQ到C核,  */
    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_CLPR_GET_REQ,
                                      gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      (MN_CALL_ID_T)gastAtParaList[0].ulParaValue,
                                      VOS_NULL_PTR);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetClprPara: MN_CALL_SendAppRequest fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CLPR_SET;

    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 AT_SetCLteRoamAllowPara(VOS_UINT8 ucIndex)
{
    NAS_MMC_NVIM_LTE_INTERNATIONAL_ROAM_CFG_STRU  stNvimLteRoamAllowedFlg;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (1 != gucAtParaIndex)
    {
        return AT_ERROR;
    }

    stNvimLteRoamAllowedFlg.ucLteRoamAllowedFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stNvimLteRoamAllowedFlg.aucReserve[0]       = 0x00;

    /* 写NV:en_NV_Item_Lte_Internation_Roam_Config,
       该NV的结构为 NAS_MMC_NVIM_LTE_INTERNATIONAL_ROAM_CFG_STRU, 只写第1个字节 */
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_Lte_Internation_Roam_Config,
                          &(stNvimLteRoamAllowedFlg),
                          sizeof(VOS_UINT16)))
    {
        AT_ERR_LOG("AT_SetCLteRoamAllowPara:Write NV fail");
        return AT_ERROR;
    }
    else
    {
        return AT_OK;
    }

}



VOS_UINT32 At_SetWlthresholdcfgPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                                              ulResult;
    AT_MTA_RESEL_OFFSET_CFG_SET_NTF_STRU                    stReselOffsetCfgNtf;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stReselOffsetCfgNtf, 0, sizeof(stReselOffsetCfgNtf));

    stReselOffsetCfgNtf.ucOffsetFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 发送消息AT_MTA_RESEL_OFFSET_CFG_SET_REQ_STRU*/
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      ID_AT_MTA_RESEL_OFFSET_CFG_SET_NTF,
                                      &stReselOffsetCfgNtf,
                                      sizeof(stReselOffsetCfgNtf),
                                      I0_UEPS_PID_MTA);

#if  (FEATURE_ON == FEATURE_LTE)
    /*L使用参数*/
    At_SetLWThresholdCfgPara(ucIndex);
#endif

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("At_SetWlthresholdcfgPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    return AT_OK;

}



VOS_UINT32 At_SetSwverPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;

    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 发消息到C核获取充电状态和电池电量 */
    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   DRV_AGENT_SWVER_SET_REQ,
                                   VOS_NULL_PTR,
                                   0,
                                   I0_WUEPS_PID_DRV_AGENT);
    if (TAF_SUCCESS != ulRet)
    {
        AT_WARN_LOG("At_SetSwverPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SWVER_SET;
    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 AT_SetCbgPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    APP_VC_MSG_ENUM_U16                 usMsgName;

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取下发VC的消息头名称 */
    if ( 0 == gastAtParaList[0].ulParaValue )
    {
        usMsgName = APP_VC_MSG_SET_FOREGROUND_REQ;
    }
    else
    {
        usMsgName = APP_VC_MSG_SET_BACKGROUND_REQ;
    }

    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      usMsgName,
                                      VOS_NULL_PTR,
                                      0,
                                      I0_WUEPS_PID_VC);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetCbgPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CBG_SET;

    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SndMtaBodySarPara(
    VOS_UINT8                           ucIndex,
    MTA_BODY_SAR_STATE_ENUM_UINT16      enBodySarState,
    MTA_BODY_SAR_PARA_STRU             *pstBodySarPara)
{
    VOS_UINT32                          ulResult;
    AT_MTA_BODY_SAR_SET_REQ_STRU        stBodySarSetReq;

    PS_MEM_SET(&stBodySarSetReq, 0, sizeof(stBodySarSetReq));

    /* 填充消息结构体 */
    stBodySarSetReq.enState = enBodySarState;
    PS_MEM_CPY(&stBodySarSetReq.stBodySARPara, pstBodySarPara, sizeof(MTA_BODY_SAR_PARA_STRU));

    /* 发送消息AT_MTA_BODY_SAR_SET_REQ_STRU */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      ID_AT_MTA_BODY_SAR_SET_REQ,
                                      &stBodySarSetReq,
                                      sizeof(AT_MTA_BODY_SAR_SET_REQ_STRU),
                                      I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SndMtaBodySarPara: AT_FillAndSndAppReqMsg fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_SetBodySarOnPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    MTA_BODY_SAR_PARA_STRU              stBodySarPara;

    PS_MEM_SET(&stBodySarPara, 0, sizeof(stBodySarPara));

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 判断BODYSAR当前状态 */
    if (g_enAtBodySarState == gastAtParaList[0].ulParaValue)
    {
        return AT_OK;
    }

    /* 读取BODYSAR NV项 */
    ulResult = NV_ReadEx(MODEM_ID_0,
                         en_NV_Item_BODY_SAR_PARA,
                         &stBodySarPara,
                         sizeof(MTA_BODY_SAR_PARA_STRU));

    if (NV_OK != ulResult)
    {
        AT_ERR_LOG("AT_SetBodySarOnPara: Read NV fail");
        return AT_ERROR;
    }

    /* 发送消息给MTA */
    ulResult = AT_SndMtaBodySarPara(ucIndex,
                                    (MTA_BODY_SAR_STATE_ENUM_UINT16)gastAtParaList[0].ulParaValue,
                                    &stBodySarPara);
    if (VOS_OK != ulResult)
    {
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_BODYSARON_SET;
    return AT_WAIT_ASYNC_RETURN;
}
VOS_UINT32 AT_CheckBodySarWcdmaPara(
    AT_BODYSARWCDMA_SET_PARA_STRU      *pstBodySarWcdmaPara)
{
    AT_NV_WG_RF_MAIN_BAND_STRU          stWGBand;
    VOS_UINT8                           ucLoop;
    VOS_UINT32                          ulLen;

    PS_MEM_SET(&stWGBand, 0, sizeof(stWGBand));

    /* 参数个数检查 */
    if ( (gucAtParaIndex < 1)
      || (gucAtParaIndex > AT_BODYSARWCDMA_MAX_PARA_NUM) )
    {
        AT_WARN_LOG1("AT_CheckBodySarWcdmaPara: Para Num Incorrect!", gucAtParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 当参数个数为1时，补充默认值 */
    if (1 == gucAtParaIndex)
    {
        ulLen = VOS_StrLen(AT_RF_BAND_ANY_STR);
        PS_MEM_CPY(gastAtParaList[1].aucPara,
                   AT_RF_BAND_ANY_STR,
                   ulLen);
        gastAtParaList[1].aucPara[ulLen]    = '\0';
        gastAtParaList[1].usParaLen         = (VOS_UINT16)ulLen;
        gucAtParaIndex                      = 2;
    }

    /* 参数个数必须为偶数 */
    if (0 != (gucAtParaIndex % 2))
    {
        AT_WARN_LOG1("AT_CheckBodySarWcdmaPara: Para Num is not Even!", gucAtParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取WCDMA Band能力值 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_WG_RF_MAIN_BAND,
                           &stWGBand,
                           sizeof(stWGBand)))
    {
        AT_ERR_LOG("AT_CheckBodySarWcdmaPara: Read NV fail!");
        return AT_ERROR;
    }

    /* 循环检查Band频段是否支持，并填充参数结构体 */
    pstBodySarWcdmaPara->ucParaNum = 0;
    for (ucLoop = 0; ucLoop < (gucAtParaIndex / 2); ucLoop++)
    {
        /* 将用户输入的频段参数由16进制字符串转换为数字 */
        if (AT_SUCCESS != At_String2Hex(gastAtParaList[ucLoop * 2 + 1].aucPara,
                                        gastAtParaList[ucLoop * 2 + 1].usParaLen,
                                        &gastAtParaList[ucLoop * 2 + 1].ulParaValue))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 如果用户输入的是任意Band频段，则替换为当前支持的Band频段值 */
        if (AT_RF_BAND_ANY == gastAtParaList[ucLoop * 2 + 1].ulParaValue)
        {
            gastAtParaList[ucLoop * 2 + 1].ulParaValue = stWGBand.unWcdmaBand.ulBand;
        }

        /* 检查用户输入的频段当前是否支持，如果不支持返回参数错误 */
        if (0 != (gastAtParaList[ucLoop * 2 + 1].ulParaValue & (~stWGBand.unWcdmaBand.ulBand)))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pstBodySarWcdmaPara->asPower[pstBodySarWcdmaPara->ucParaNum]    = (VOS_INT16)gastAtParaList[ucLoop * 2].ulParaValue;
        pstBodySarWcdmaPara->aulBand[pstBodySarWcdmaPara->ucParaNum]    = gastAtParaList[ucLoop * 2 + 1].ulParaValue;
        pstBodySarWcdmaPara->ucParaNum++;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_FillBodySarWcdmaPara(
    AT_BODYSARWCDMA_SET_PARA_STRU      *pstBodySarWcdmaPara,
    MTA_BODY_SAR_PARA_STRU             *pstBodySarPara)
{
    VOS_UINT8                           ucLoop1;
    MTA_WCDMA_BAND_ENUM_UINT16          enLoop2;
    VOS_UINT8                           ucLoop3;

    /* 遍历命令所有的参数 */
    for (ucLoop1 = 0; ucLoop1 < pstBodySarWcdmaPara->ucParaNum; ucLoop1++)
    {
        /* 按照WCDMA Band来填充参数 */
        for (enLoop2 = MTA_WCDMA_I_2100; enLoop2 < MTA_WCDMA_BAND_BUTT; enLoop2++)
        {
            if (0 == (pstBodySarWcdmaPara->aulBand[ucLoop1] & (0x00000001 << (enLoop2 - 1))))
            {
                continue;
            }

            for (ucLoop3 = 0; ucLoop3 < pstBodySarPara->usWBandNum; ucLoop3++)
            {
                if (enLoop2 == pstBodySarPara->astWBandPara[ucLoop3].enBand)
                {
                    break;
                }
            }

            if (ucLoop3 == pstBodySarPara->usWBandNum)
            {
                /* 若超出W Band最大支持个数，则回复失败 */
                if (MTA_BODY_SAR_WBAND_MAX_NUM <= pstBodySarPara->usWBandNum)
                {
                    AT_ERR_LOG("AT_FillBodySarWcdmaPara: Too Many W Band!");
                    return VOS_ERR;
                }

                pstBodySarPara->usWBandNum++;
            }

            pstBodySarPara->astWBandPara[ucLoop3].enBand = enLoop2;
            pstBodySarPara->astWBandPara[ucLoop3].sPower = pstBodySarWcdmaPara->asPower[ucLoop1];
        }
    }

    return VOS_OK;
}
VOS_UINT32 AT_SetBodySarWcdmaPara(VOS_UINT8 ucIndex)
{
    MTA_BODY_SAR_PARA_STRU              stBodySarPara;
    AT_BODYSARWCDMA_SET_PARA_STRU       stBodySarWcdmaPara;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stBodySarPara, 0, sizeof(stBodySarPara));
    PS_MEM_SET(&stBodySarWcdmaPara, 0, sizeof(stBodySarWcdmaPara));

    /* 检查参数有效性 */
    ulResult = AT_CheckBodySarWcdmaPara(&stBodySarWcdmaPara);
    if (AT_SUCCESS != ulResult)
    {
        return ulResult;
    }

    /* 从NV项中读取Body SAR功率门限值 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_BODY_SAR_PARA,
                           &stBodySarPara,
                           sizeof(stBodySarPara)))
    {
        AT_ERR_LOG("AT_SetBodysarWcdmaPara: Read NV fail!");
        return AT_ERROR;
    }

    /* 将WCDMA的频段参数填充到Body SAR参数结构体中 */
    if (VOS_OK != AT_FillBodySarWcdmaPara(&stBodySarWcdmaPara, &stBodySarPara))
    {
        return AT_ERROR;
    }

    /* 将Body SAR功率门限值保存到NV */
    if (NV_OK != NV_WriteEx(MODEM_ID_0,
                            en_NV_Item_BODY_SAR_PARA,
                            &(stBodySarPara),
                            sizeof(stBodySarPara)))
    {
        AT_ERR_LOG("AT_SetBodysarWcdmaPara: Write NV fail!");
        return AT_ERROR;
    }

    /* 如果当前开启了Body SAR功能，则通知物理层新的Body SAR参数 */
    if (MTA_BODY_SAR_ON == g_enAtBodySarState)
    {
        if (VOS_OK != AT_SndMtaBodySarPara(ucIndex,
                                           g_enAtBodySarState,
                                           &stBodySarPara))
        {
            AT_WARN_LOG("AT_SetBodySarWcdmaPara: AT_SndMtaBodySarPara fail!");
        }
    }

    return AT_OK;
}
VOS_UINT32 AT_GetGsmBandCapa(VOS_UINT32 *pulGBand)
{
    AT_NV_WG_RF_MAIN_BAND_STRU          stWGBand;

    PS_MEM_SET(&stWGBand, 0, sizeof(stWGBand));
    *pulGBand = 0;

    /* 从NV项中读取单板支持的频段 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_WG_RF_MAIN_BAND,
                           &stWGBand,
                           sizeof(stWGBand)))
    {
        AT_ERR_LOG("AT_GetGsmBandCapa: Read NV fail!");
        return VOS_ERR;
    }

    if (VOS_TRUE == stWGBand.unGsmBand.BitBand.BandGsm850)
    {
        *pulGBand |= MTA_BODY_SAR_GBAND_GPRS_850_MASK;
        *pulGBand |= MTA_BODY_SAR_GBAND_EDGE_850_MASK;
    }

    if ( (VOS_TRUE == stWGBand.unGsmBand.BitBand.BandGsmP900)
      || (VOS_TRUE == stWGBand.unGsmBand.BitBand.BandGsmR900)
      || (VOS_TRUE == stWGBand.unGsmBand.BitBand.BandGsmE900) )
    {
        *pulGBand |= MTA_BODY_SAR_GBAND_GPRS_900_MASK;
        *pulGBand |= MTA_BODY_SAR_GBAND_EDGE_900_MASK;
    }

    if (VOS_TRUE == stWGBand.unGsmBand.BitBand.BandGsm1800)
    {
        *pulGBand |= MTA_BODY_SAR_GBAND_GPRS_1800_MASK;
        *pulGBand |= MTA_BODY_SAR_GBAND_EDGE_1800_MASK;
    }

    if (VOS_TRUE == stWGBand.unGsmBand.BitBand.BandGsm1900)
    {
        *pulGBand |= MTA_BODY_SAR_GBAND_GPRS_1900_MASK;
        *pulGBand |= MTA_BODY_SAR_GBAND_EDGE_1900_MASK;
    }

    return VOS_OK;
}
VOS_UINT32 AT_CheckBodySarGsmPara(
    AT_BODYSARGSM_SET_PARA_STRU        *pstBodySarGsmPara)
{
    VOS_UINT32                          ulGBand;
    VOS_UINT8                           ucLoop;
    VOS_UINT32                          ulLen;

    ulGBand         = 0;

    /* 参数个数检查 */
    if ( (gucAtParaIndex < 1)
      || (gucAtParaIndex > AT_BODYSARGSM_MAX_PARA_NUM) )
    {
        AT_WARN_LOG1("AT_CheckBodySarGsmPara: Para Num Incorrect!", gucAtParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 当参数个数为1时，补充默认值 */
    if (1 == gucAtParaIndex)
    {
        ulLen = VOS_StrLen(AT_RF_BAND_ANY_STR);
        PS_MEM_CPY(gastAtParaList[1].aucPara,
                   AT_RF_BAND_ANY_STR,
                   ulLen);
        gastAtParaList[1].aucPara[ulLen]    = '\0';
        gastAtParaList[1].usParaLen         = (VOS_UINT16)ulLen;
        gucAtParaIndex                      = 2;
    }

    /* 参数个数必须为偶数 */
    if (0 != (gucAtParaIndex % 2))
    {
        AT_WARN_LOG1("AT_CheckBodySarGsmPara: Para Num is not Even!", gucAtParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取GSM Band能力值 */
    if (VOS_OK != AT_GetGsmBandCapa(&ulGBand))
    {
        return AT_ERROR;
    }

    /* 循环检查Band频段是否支持，并填充参数结构体 */
    pstBodySarGsmPara->ucParaNum = 0;
    for (ucLoop = 0; ucLoop < (gucAtParaIndex / 2); ucLoop++)
    {
        /* 将用户输入的频段参数由16进制字符串转换为数字 */
        if (AT_SUCCESS != At_String2Hex(gastAtParaList[ucLoop * 2 + 1].aucPara,
                                        gastAtParaList[ucLoop * 2 + 1].usParaLen,
                                        &gastAtParaList[ucLoop * 2 + 1].ulParaValue))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 如果用户输入的是任意Band频段，则替换为当前支持的Band频段值 */
        if (AT_RF_BAND_ANY == gastAtParaList[ucLoop * 2 + 1].ulParaValue)
        {
            gastAtParaList[ucLoop * 2 + 1].ulParaValue = ulGBand;
        }

        /* 检查用户输入的频段当前是否支持，如果不支持返回参数错误 */
        if (0 != (gastAtParaList[ucLoop * 2 + 1].ulParaValue & (~ulGBand)))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pstBodySarGsmPara->asPower[pstBodySarGsmPara->ucParaNum]    = (VOS_INT16)gastAtParaList[ucLoop * 2].ulParaValue;
        pstBodySarGsmPara->aulBand[pstBodySarGsmPara->ucParaNum]    = gastAtParaList[ucLoop * 2 + 1].ulParaValue;
        pstBodySarGsmPara->ucParaNum++;
    }

    return AT_SUCCESS;
}


VOS_VOID AT_FillBodySarGsmPara(
    AT_BODYSARGSM_SET_PARA_STRU        *pstBodySarGsmPara,
    MTA_BODY_SAR_PARA_STRU             *pstBodySarPara)
{
    VOS_UINT8                           ucLoop;

    /* 遍历命令所有的参数 */
    for (ucLoop = 0; ucLoop < pstBodySarGsmPara->ucParaNum; ucLoop++)
    {
        if (0 != (MTA_BODY_SAR_GBAND_GPRS_850_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_GPRS_850_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_850].sGPRSPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_EDGE_850_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_EDGE_850_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_850].sEDGEPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_GPRS_900_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_GPRS_900_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_900].sGPRSPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_EDGE_900_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_EDGE_900_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_900].sEDGEPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_GPRS_1800_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_GPRS_1800_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_1800].sGPRSPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_EDGE_1800_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_EDGE_1800_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_1800].sEDGEPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_GPRS_1900_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_GPRS_1900_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_1900].sGPRSPower = pstBodySarGsmPara->asPower[ucLoop];
        }

        if (0 != (MTA_BODY_SAR_GBAND_EDGE_1900_MASK & pstBodySarGsmPara->aulBand[ucLoop]))
        {
            pstBodySarPara->ulGBandMask |= MTA_BODY_SAR_GBAND_EDGE_1900_MASK;
            pstBodySarPara->astGBandPara[MTA_GSM_1900].sEDGEPower = pstBodySarGsmPara->asPower[ucLoop];
        }
    }

    return;
}
VOS_UINT32 AT_SetBodySarGsmPara(VOS_UINT8 ucIndex)
{
    MTA_BODY_SAR_PARA_STRU              stBodySarPara;
    AT_BODYSARGSM_SET_PARA_STRU         stBodySarGsmPara;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stBodySarPara, 0, sizeof(stBodySarPara));
    PS_MEM_SET(&stBodySarGsmPara, 0, sizeof(stBodySarGsmPara));

    /* 检查参数有效性 */
    ulResult = AT_CheckBodySarGsmPara(&stBodySarGsmPara);
    if (AT_SUCCESS != ulResult)
    {
        return ulResult;
    }

    /* 从NV项中读取Body SAR功率门限值 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_BODY_SAR_PARA,
                           &stBodySarPara,
                           sizeof(stBodySarPara)))
    {
        AT_ERR_LOG("AT_SetBodySarGsmPara: Read NV fail!");
        return AT_ERROR;
    }

    /* 将GSM的频段参数填充到Body SAR参数结构体中 */
    AT_FillBodySarGsmPara(&stBodySarGsmPara, &stBodySarPara);

    /* 将Body SAR功率门限值保存到NV */
    if (NV_OK != NV_WriteEx(MODEM_ID_0,
                            en_NV_Item_BODY_SAR_PARA,
                            &(stBodySarPara),
                            sizeof(stBodySarPara)))
    {
        AT_ERR_LOG("AT_SetBodySarGsmPara: Write NV fail!");
        return AT_ERROR;
    }

    /* 如果当前开启了Body SAR功能，则通知物理层新的Body SAR参数 */
    if (MTA_BODY_SAR_ON == g_enAtBodySarState)
    {
        if (VOS_OK != AT_SndMtaBodySarPara(ucIndex,
                                           g_enAtBodySarState,
                                           &stBodySarPara))
        {
            AT_WARN_LOG("AT_SetBodySarGsmPara: AT_SndMtaBodySarPara fail!");
        }
    }

    return AT_OK;
}
VOS_UINT32 AT_SetCopnPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRet;
    TAF_MMA_COPN_INFO_QUERY_REQ_STRU    stCopnInfo;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /*
    AT向MMA请求运营商信息:
    因为核间消息限制，不能一次获取所有运营商信息，这里定义为一次获取50条运营商信息
    第一条请求消息，从索引0开始要求连续的50条运营商信息
    */
    stCopnInfo.usFromIndex  = 0;
    stCopnInfo.usPlmnNum    = TAF_MMA_COPN_PLMN_MAX_NUM;

    /* 向MMA发消息请求运营商信息 */
    ulRet = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   TAF_MSG_MMA_COPN_INFO_QUERY_REQ,
                                   &stCopnInfo,
                                   sizeof(stCopnInfo),
                                   I0_WUEPS_PID_MMA);
    if (TAF_SUCCESS != ulRet)
    {
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_COPN_QRY;

    return AT_WAIT_ASYNC_RETURN;

}


VOS_UINT32 AT_SetNCellMonitorPara(VOS_UINT8 ucIndex)
{
    AT_MTA_NCELL_MONITOR_SET_REQ_STRU                       stAtCmd;
    VOS_UINT32                                              ulResult;

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stAtCmd, 0x00, sizeof(stAtCmd));
    stAtCmd.ucSwitch    = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 给MTA发送^NCELLMONITOR设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   0,
                                   ID_AT_MTA_NCELL_MONITOR_SET_REQ,
                                   &stAtCmd,
                                   sizeof(AT_MTA_NCELL_MONITOR_SET_REQ_STRU),
                                   I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_NCELL_MONITOR_SET;

    return AT_WAIT_ASYNC_RETURN;

}
VOS_UINT32 At_SetSimInsertPara(VOS_UINT8 ucIndex)
{
    TAF_MMA_SIM_INSERT_IND_STRU         stSimInsertState;
    VOS_UINT32                          ulResult;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gastAtParaList[0].ulParaValue > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */

    stSimInsertState.enSimInsertState = (TAF_SIM_INSERT_STATE_ENUM_UINT32)gastAtParaList[0].ulParaValue;

    ulResult = MN_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      TAF_MSG_MMA_SIMINSERT_IND,
                                      &stSimInsertState,
                                      sizeof(TAF_MMA_SIM_INSERT_IND_STRU),
                                      I0_WUEPS_PID_MMA);
    if (AT_SUCCESS == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_SIMINSERT_SET;
        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        return AT_ERROR;
    }

}


VOS_UINT32 At_SetModemLoopPara(VOS_UINT8 ucIndex)
{

    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucModemLoop;

    /* 参数检查 */
    if ( AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucModemLoop = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    ulResult = APP_VC_SetModemLoop(gastAtClientTab[ucIndex].usClientId,
                                   gastAtClientTab[ucIndex].opId,
                                   ucModemLoop);
    if (TAF_SUCCESS == ulResult)
    {
        return AT_OK;    /* 此命令不需要等待C核回复，直接返回OK */
    }
    else
    {
        return AT_ERROR;
    }

}
VOS_UINT32 At_SetReleaseRrc(VOS_UINT8 ucIndex)
{
    VOS_UINT8                              *pucSystemAppConfig;

    /* 读取NV项中当前产品形态 */
    pucSystemAppConfig = AT_GetSystemAppConfigAddr();

#ifndef DMT
    /* 非ANDROID系统不支持 */
    if ( SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        return AT_CMD_NOT_SUPPORT;
    }
#endif

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 下发请求 */
    if (VOS_OK == At_SndReleaseRrcReq(gastAtClientTab[ucIndex].usClientId,
                                          gastAtClientTab[ucIndex].opId))
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_RELEASE_RRC_SET;

        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_SetRefclkfreqPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    AT_MTA_REFCLKFREQ_SET_REQ_STRU      stRefClockReq;

    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* 参数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_ERROR;
    }

    /* 初始化 */
    PS_MEM_SET(&stRefClockReq, 0, sizeof(stRefClockReq));

    stRefClockReq.enRptFlg = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 发送跨核消息到C核, 设置GPS参考时钟状态是否主动上报 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      ID_AT_MTA_REFCLKFREQ_SET_REQ,
                                      &stRefClockReq,
                                      sizeof(stRefClockReq),
                                      I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetRefclkfreqPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_REFCLKFREQ_SET;

    return AT_WAIT_ASYNC_RETURN;
}


/* Added by d0212987 for OM output log , 2013-6-4, begin */

VOS_UINT32 At_SetPullomlogrrcPara(VOS_UINT8 ucIndex)
{
    OM_AcpuLogShowToFile(VOS_TRUE);

    return AT_OK;
}
/* Added by d0212987 for OM output log, 2013-6-4, end */

/* Added by d0212987 for read die id and chip id, 2013-6-4, begin */

VOS_UINT32 At_SetDieSNPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           aucDieSN[AT_DIE_SN_LEN];
    VOS_INT32                           i;
    VOS_UINT16                          usLength;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 获取DIE SN */
    if (VOS_OK != DRV_GET_DIEID(aucDieSN, AT_DIE_SN_LEN))
    {
        return AT_ERROR;
    }

    /* 最高位,高4 BIT置0 */
    aucDieSN[AT_DIE_SN_LEN -1] = (aucDieSN[AT_DIE_SN_LEN -1] & 0x0F);

    /* 格式化输出查询结果 */
    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      "%s: 0x",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    for (i = (AT_DIE_SN_LEN-1); i>=0; i--)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%02x",
                                      aucDieSN[i]);
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 At_SetChipSNPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           aucChipSN[AT_CHIP_SN_LEN];
    VOS_INT32                           i;
    VOS_UINT16                          usLength;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 获取CHIP SN */
    if (VOS_OK != DRV_GET_CHIPID(aucChipSN, AT_CHIP_SN_LEN))
    {
        return AT_ERROR;
    }

    /* 格式化输出查询结果 */
    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      "%s: 0x",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    for (i = (AT_CHIP_SN_LEN-1); i>=0; i--)
    {

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%02x",
                                      aucChipSN[i]);
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}
/* Added by d0212987 for read die id and chip id, 2013-6-4, end */

VOS_UINT32 At_SetHandleDect(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    VOS_UINT8                           ucHandleType;
    VOS_UINT8                          *pucSystemAppConfig;

    /* 读取NV项中当前产品形态 */
    pucSystemAppConfig = AT_GetSystemAppConfigAddr();

#ifndef DMT
    /* 该命令用来配置phone和pad形态cp侧手持位置，非ANDROID系统不支持 */
    if ( SYSTEM_APP_ANDROID != *pucSystemAppConfig )
    {
        return AT_CMD_NOT_SUPPORT;
    }
#endif

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /* handletype 取值错误 */
    if (gastAtParaList[0].ulParaValue > AT_MTA_HANDLEDECT_MAX_TYPE)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucHandleType = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 发送消息 ID_AT_MTA_HANDLEDECT_SET_REQ 给MTA处理，该消息带参数(VOS_UINT8)gastAtParaList[0].ulParaValue */
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   ID_AT_MTA_HANDLEDECT_SET_REQ,
                                   (VOS_VOID*)&ucHandleType,
                                   sizeof(ucHandleType),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_HANDLEDECT_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

#if (FEATURE_ON == FEATURE_IMS)
TAF_UINT32 AT_SetCiregPara(TAF_UINT8 ucIndex)
{

    AT_IMSA_CIREG_SET_REQ_STRU          stCireg;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stCireg, 0x00, sizeof(AT_IMSA_CIREG_SET_REQ_STRU));

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        stCireg.enCireg = gastAtParaList[0].ulParaValue;

    }
    else
    {
        /* 如果参数为空，默认设置为不主动上报 */
        stCireg.enCireg = AT_IMSA_IMS_REG_STATE_DISABLE_REPORT;
    }


    /* 给IMSA发送+CIREG设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     ID_AT_IMSA_CIREG_SET_REQ,
                                     &stCireg.enCireg,
                                     sizeof(AT_IMSA_IMS_REG_STATE_REPORT_ENUM_UINT32),
                                     PS_PID_IMSA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CIREG_SET;

    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetCirepPara(TAF_UINT8 ucIndex)
{

    AT_IMSA_CIREP_SET_REQ_STRU          stCirep;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stCirep, 0x00, sizeof(AT_IMSA_CIREP_SET_REQ_STRU));

    /* 参数检查 */
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if(gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数不为空 */
    if(0 != gastAtParaList[0].usParaLen)
    {
        stCirep.enReport = gastAtParaList[0].ulParaValue;

    }
    else
    {
        /* 如果参数为空，默认设置为不主动上报 */
        stCirep.enReport = AT_IMSA_CIREP_REPORT_DISENABLE;
    }


    /* 给IMSA发送+CIREG设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     ID_AT_IMSA_CIREP_SET_REQ,
                                     &stCirep.enReport,
                                     sizeof(AT_IMSA_CIREP_REPORT_ENUM_UINT32),
                                     PS_PID_IMSA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CIREP_SET;

    return AT_WAIT_ASYNC_RETURN;
}
TAF_UINT32 AT_SetVolteimpuPara(TAF_UINT8 ucIndex)
{
    AT_IMSA_VOLTEIMPU_QRY_REQ_STRU      stImpu;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stImpu, 0x00, sizeof(AT_IMSA_VOLTEIMPU_QRY_REQ_STRU));

    /* 参数检查 */
    if(AT_CMD_OPT_SET_CMD_NO_PARA != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 给IMSA发送^VOLTEIMPU设置请求 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                     0,
                                     ID_AT_IMSA_VOLTEIMPU_QRY_REQ,
                                     stImpu.aucContent,
                                     sizeof(stImpu.aucContent),
                                     PS_PID_IMSA);

    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_IMPU_SET;

    return AT_WAIT_ASYNC_RETURN;
}


TAF_UINT32 AT_SetCacmimsPara(TAF_UINT8 ucIndex)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulCallNum;
    MN_CALL_ORIG_PARAM_STRU             stOrigParam;
    TAF_CALL_ECONF_DIAL_REQ_STRU        stDialReq;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx    = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    PS_MEM_SET(&stOrigParam, 0x00, sizeof(MN_CALL_ORIG_PARAM_STRU));
    PS_MEM_SET(&stDialReq,   0x00, sizeof(TAF_CALL_ECONF_DIAL_REQ_STRU));

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数不对 */
    /* 参数的个数应该是total_callnum+2 */
    if ((gastAtParaList[0].ulParaValue + 2) != gucAtParaIndex)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 普通多方通话邀请 */
    if (0 == gastAtParaList[1].ulParaValue)
    {
        /* 普通多方通话只能邀请一个用户 */
        if (gastAtParaList[0].ulParaValue != 1)
        {
            AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 设置<dial_string> */
        if (VOS_OK !=  AT_FillCalledNumPara(gastAtParaList[2].aucPara,
                                            gastAtParaList[2].usParaLen,
                                            &stOrigParam.stDialNumber))
        {
            AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
            return AT_CME_INCORRECT_PARAMETERS;
        }

        stOrigParam.enCallType              = MN_CALL_TYPE_VOICE;
        stOrigParam.enCallMode              = pstSsCtx->enCModType;
        stOrigParam.stDataCfg               = pstSsCtx->stCbstDataCfg;
        stOrigParam.ucImsInvitePtptFlag     = VOS_TRUE;

        stOrigParam.enVoiceDomain           = TAF_CALL_VOICE_DOMAIN_AUTO;

        if(VOS_OK == MN_CALL_Orig(gastAtClientTab[ucIndex].usClientId,
                                  0,
                                  &gastAtClientTab[ucIndex].AtCallInfo.CallId,
                                  &stOrigParam))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CACMIMS_SET;
            return AT_WAIT_ASYNC_RETURN;
        }
        else
        {
            AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_UNKNOWN);
            return AT_ERROR;
        }
    }
    /* 增强型多方通话邀请 */
    else
    {
        ulCallNum = gastAtParaList[0].ulParaValue;

        /* 默认情况是使用CCA设置的默认值 */
        stDialReq.enCallMode  = pstSsCtx->enCModType;
        stDialReq.stDataCfg   = pstSsCtx->stCbstDataCfg;
        stDialReq.enClirCfg   = pstSsCtx->ucClirType;
        stDialReq.stCugCfg    = pstSsCtx->stCcugCfg;

        for (i = 0; i < ulCallNum; i++)
        {
            if (0 == gastAtParaList[2 + i].usParaLen)
            {
                AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
                return AT_CME_INCORRECT_PARAMETERS;
            }

            if (VOS_OK != AT_FillCalledNumPara(gastAtParaList[2 + i].aucPara,
                                               gastAtParaList[2 + i].usParaLen,
                                               &stDialReq.stEconfCalllist.astDialNumber[i]))
            {
                AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }

        stDialReq.stEconfCalllist.ulCallNum = ulCallNum;

        stDialReq.enCallType            = MN_CALL_TYPE_VOICE;
        stDialReq.enVoiceDomain         = TAF_CALL_VOICE_DOMAIN_IMS;
        stDialReq.ucImsInvitePtptFlag   = VOS_TRUE;

        ulResult = MN_CALL_SendAppRequest(TAF_CALL_APP_ECONF_DIAL_REQ,
                                          gastAtClientTab[ucIndex].usClientId,
                                          gastAtClientTab[ucIndex].opId,
                                          0,
                                          (MN_CALL_APP_REQ_PARM_UNION *)&stDialReq);
        if (VOS_OK == ulResult)
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CACMIMS_SET;
            return AT_WAIT_ASYNC_RETURN;
        }
        else
        {
            AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_UNKNOWN);
            return AT_ERROR;
        }

    }
}
TAF_UINT32 AT_SetCcwaiPara(TAF_UINT8 ucIndex)
{
    AT_IMSA_CCWAI_SET_REQ_STRU *    stCcwaiReq;
    VOS_UINT32                      ulResult;

    /* 参数过多 */
    if(gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if((0 == gastAtParaList[0].usParaLen) || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stCcwaiReq = (AT_IMSA_CCWAI_SET_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(AT_IMSA_CCWAI_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if(stCcwaiReq == VOS_NULL)
    {
        return AT_ERROR;
    }

    /* 填充消息结构体 */
    stCcwaiReq->ulSenderPid     = WUEPS_PID_AT;
    stCcwaiReq->ulReceiverPid   = PS_PID_IMSA;
    stCcwaiReq->ulLength        = sizeof(AT_IMSA_CCWAI_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    stCcwaiReq->ulMsgId         = ID_AT_IMSA_CCWAI_SET_REQ;
    stCcwaiReq->usClientId      = gastAtClientTab[ucIndex].usClientId;
    stCcwaiReq->ucOpId          = 0;
    stCcwaiReq->enMode          = (AT_IMSA_CCWAI_MODE_ENUM_UINT8)gastAtParaList[0].ulParaValue;
    stCcwaiReq->ulSrvClass      = gastAtParaList[1].ulParaValue;

    /* 给IMSA发送^CCWAI设置请求 */
    ulResult = VOS_SendMsg(WUEPS_PID_AT, stCcwaiReq);
    if (TAF_SUCCESS != ulResult)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CCWAI_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/*****************************************************************************
 函 数 名  : AT_SetUiccAuthPara
 功能描述  : ^UICCAUTH=<auth_mode>,<rand>,<auth>
 输入参数  : ucIndex - 端口索引
 输出参数  : 无
 返 回 值  : AT_XXX  - ATC返回码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月18日
    作    者   : zhuli
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SetUiccAuthPara(VOS_UINT8 ucIndex)
{
    SI_PIH_UICCAUTH_STRU        stAuthData;

    if ((gucAtParaIndex != 3)
        ||(0 == gastAtParaList[1].usParaLen)
        ||(0 == gastAtParaList[2].usParaLen))
    {
        AT_ERR_LOG("AT_SetUiccAuthPara: para check fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara,
                                            &gastAtParaList[1].usParaLen))
    {
        AT_ERR_LOG("AT_SetUiccAuthPara: At_AsciiNum2HexString para1 fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[2].aucPara,
                                            &gastAtParaList[2].usParaLen))
    {
        AT_ERR_LOG("AT_SetUiccAuthPara: At_AsciiNum2HexString para2 fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((gastAtParaList[1].usParaLen > SI_AUTH_DATA_MAX)
        ||(gastAtParaList[2].usParaLen > SI_AUTH_DATA_MAX))
    {
        AT_ERR_LOG("AT_SetUiccAuthPara: String Len fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (SI_PIH_ATUICCAUTH_USIMAKA == gastAtParaList[0].ulParaValue)
    {
        stAuthData.enAuthType   = SI_PIH_UICCAUTH_AKA;
        stAuthData.enAppType    = SI_PIH_UICCAPP_USIM;
    }
    else if (SI_PIH_ATUICCAUTH_ISIMAKA == gastAtParaList[0].ulParaValue)
    {
        stAuthData.enAuthType   = SI_PIH_UICCAUTH_AKA;
        stAuthData.enAppType    = SI_PIH_UICCAPP_ISIM;
    }
    else if (SI_PIH_ATUICCAUTH_USIMGBA == gastAtParaList[0].ulParaValue)
    {
        stAuthData.enAuthType   = SI_PIH_UICCAUTH_GBA;
        stAuthData.enAppType    = SI_PIH_UICCAPP_USIM;
    }
    else if (SI_PIH_ATUICCAUTH_ISIMGBA == gastAtParaList[0].ulParaValue)
    {
        stAuthData.enAuthType   = SI_PIH_UICCAUTH_GBA;
        stAuthData.enAppType    = SI_PIH_UICCAPP_ISIM;
    }
    else
    {
        AT_ERR_LOG("AT_SetUiccAuthPara: Para 1 value fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    stAuthData.uAuthData.stAka.ulRandLen = gastAtParaList[1].usParaLen;

    stAuthData.uAuthData.stAka.ulAuthLen = gastAtParaList[2].usParaLen;

    /* GBA和AKA鉴权数据结构是一样的 */
    VOS_MemCpy(stAuthData.uAuthData.stAka.aucRand, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);

    VOS_MemCpy(stAuthData.uAuthData.stAka.aucAuth, gastAtParaList[2].aucPara, gastAtParaList[2].usParaLen);

    if(AT_SUCCESS == SI_PIH_UiccAuthReq(gastAtClientTab[ucIndex].usClientId,
                                        0,
                                        &stAuthData))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_UICCAUTH_SET;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
       AT_ERR_LOG("AT_SetUiccAuthPara: SI_PIH_UiccAuthReq fail.");

       return AT_ERROR;
    }
}

/*****************************************************************************
 函 数 名  : AT_SetURSMPara
 功能描述  : ^UICCAUTH=<app_type>,<command>,<fileid>,<reocrd_num>[,<data>,<pathid>]]
 输入参数  : ucIndex - 端口索引
 输出参数  : 无
 返 回 值  : AT_XXX  - ATC返回码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月18日
    作    者   : zhuli
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SetURSMPara(VOS_UINT8 ucIndex)
{
    SI_PIH_ACCESSFILE_STRU              stCommand;
    VOS_UINT16                          usFileID;

    /* 参数过多 */
    if ((gucAtParaIndex > 6)
        ||(gucAtParaIndex < 4))
    {
        AT_ERR_LOG("AT_SetURSMPara: too many para");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((SI_PIH_ACCESS_WRITE == gastAtParaList[1].ulParaValue)
        &&(0 == gastAtParaList[4].usParaLen))
    {
        AT_ERR_LOG("AT_SetURSMPara: Write para wrong");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((SI_PIH_ACCESS_READ == gastAtParaList[1].ulParaValue)
        &&(0 != gastAtParaList[4].usParaLen))
    {
        AT_ERR_LOG("AT_SetURSMPara: Read para wrong");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取文件ID前两位 */
    usFileID   = (gastAtParaList[2].ulParaValue >> 8) & (0x00FF);

    /* 输入的文件ID必须是EF文件，前两位不可以是3F/5F/7F */
    if ((MFLAB == usFileID)
       || (DFUNDERMFLAB == usFileID)
       || (DFUNDERDFLAB == usFileID))
    {
        AT_ERR_LOG("AT_SetURSMPara: File Id error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 != gastAtParaList[4].usParaLen)
    {
        if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[4].aucPara,
                                                &gastAtParaList[4].usParaLen))
        {
            AT_ERR_LOG("AT_SetURSMPara: At_AsciiNum2HexString para4 fail.");

            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if (0 != gastAtParaList[5].usParaLen)
    {
        if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[5].aucPara,
                                                &gastAtParaList[5].usParaLen))
        {
            AT_ERR_LOG("AT_SetURSMPara: At_AsciiNum2HexString para5 fail.");

            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if ((USIMM_T0_APDU_MAX_LEN < gastAtParaList[4].usParaLen)
        ||(USIMM_MAX_PATH_LEN < gastAtParaList[5].usParaLen)
        ||(SI_PIH_UICCAPP_BUTT <= gastAtParaList[0].ulParaValue))
    {
        AT_ERR_LOG("AT_SetURSMPara: AT para 4 or 5 is fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    stCommand.usEfId        = (VOS_UINT16)gastAtParaList[2].ulParaValue;
    stCommand.enAppType     = gastAtParaList[0].ulParaValue;
    stCommand.enCmdType     = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    stCommand.ucRecordNum   = (VOS_UINT8)gastAtParaList[3].ulParaValue;
    stCommand.ulDataLen     = gastAtParaList[4].usParaLen;
    stCommand.usPathLen     = gastAtParaList[5].usParaLen;

    VOS_MemCpy(stCommand.aucCommand, gastAtParaList[4].aucPara, gastAtParaList[4].usParaLen);
    VOS_MemCpy(stCommand.ausPath, gastAtParaList[5].aucPara, gastAtParaList[5].usParaLen);

    /* 执行命令操作 */
    if(AT_SUCCESS == SI_PIH_AccessUICCFileReq(gastAtClientTab[ucIndex].usClientId,
                                                0,
                                                &stCommand))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CURSM_SET;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        AT_ERR_LOG("AT_SetURSMPara: SI_PIH_AccessUICCFileReq error.");

        return AT_ERROR;
    }
}

/*****************************************************************************
 函 数 名  : AT_SetKsNafAuthPara
 功能描述  : ^KSNAFAUTH=<app_type>,<naf_id>,<impi>
 输入参数  : ucIndex - 端口索引
 输出参数  : 无
 返 回 值  : AT_XXX  - ATC返回码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年1月18日
    作    者   : zhuli
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SetKsNafAuthPara(VOS_UINT8 ucIndex)
{
    SI_PIH_UICCAUTH_STRU        stAuthData;

    if ((gucAtParaIndex != 3)
        ||(0 == gastAtParaList[1].usParaLen)
        ||(0 == gastAtParaList[2].usParaLen))
    {
        AT_ERR_LOG("AT_SetKsNafAuthPara: check para fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[1].aucPara, &gastAtParaList[1].usParaLen))
    {
        AT_ERR_LOG("AT_SetKsNafAuthPara: At_AsciiNum2HexString para1 fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(AT_FAILURE == At_AsciiNum2HexString(gastAtParaList[2].aucPara, &gastAtParaList[2].usParaLen))
    {
        AT_ERR_LOG("AT_SetKsNafAuthPara: At_AsciiNum2HexString para2 fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((gastAtParaList[1].usParaLen > SI_AUTH_DATA_MAX)
        ||(gastAtParaList[2].usParaLen > SI_AUTH_DATA_MAX))
    {
        AT_ERR_LOG("AT_SetKsNafAuthPara: Str Len check fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (SI_PIH_ATKSNAFAUTH_USIM == gastAtParaList[0].ulParaValue)
    {
        stAuthData.enAuthType   = SI_PIH_UICCAUTH_NAF;
        stAuthData.enAppType    = SI_PIH_UICCAPP_USIM;
    }
    else if (SI_PIH_ATKSNAFAUTH_ISIM == gastAtParaList[0].ulParaValue)
    {
        stAuthData.enAuthType   = SI_PIH_UICCAUTH_NAF;
        stAuthData.enAppType    = SI_PIH_UICCAPP_ISIM;
    }
    else
    {
        AT_ERR_LOG("AT_SetKsNafAuthPara: Auth type fail.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    stAuthData.uAuthData.stKsNAF.ulNAFIDLen = gastAtParaList[1].usParaLen;

    stAuthData.uAuthData.stKsNAF.ulIMPILen  = gastAtParaList[2].usParaLen;

    VOS_MemCpy(stAuthData.uAuthData.stKsNAF.aucNAFID, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);

    VOS_MemCpy(stAuthData.uAuthData.stKsNAF.aucIMPI, gastAtParaList[2].aucPara, gastAtParaList[2].usParaLen);

    if(AT_SUCCESS == SI_PIH_UiccAuthReq(gastAtClientTab[ucIndex].usClientId, 0, &stAuthData))
    {
        /* 设置当前操作类型 */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_KSNAFAUTH_SET;

        return AT_WAIT_ASYNC_RETURN;    /* 返回命令处理挂起状态 */
    }
    else
    {
        AT_ERR_LOG("AT_SetKsNafAuthPara: SI_PIH_UiccAuthReq fail.");

        return AT_ERROR;
    }
}
#endif


VOS_UINT32 AT_SetEOPlmnParaCheck(VOS_VOID)
{

    /*参数有效性检查*/
    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数不为TAF_AT_EOPLMN_PARA_NUM，返回AT_CME_INCORRECT_PARAMETERS*/
    if ( TAF_AT_EOPLMN_PARA_NUM != gucAtParaIndex )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第1个参数检查,version长度不能大于TAF_MAX_USER_CFG_OPLMN_VERSION_LEN */
    if (TAF_MAX_USER_CFG_OPLMN_VERSION_LEN <= gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第2个参数检查，组Index值不能大于5 */
    if (TAF_MAX_USER_CFG_OPLMN_GROUP_INDEX < gastAtParaList[1].ulParaValue)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第3个参数检查，pduLen值不能大于500 */
    if (TAF_AT_PLMN_WITH_RAT_LEN * TAF_MAX_GROUP_CFG_OPLMN_NUM < gastAtParaList[2].ulParaValue)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第3个参数检查，pduLen值必须为TAF_AT_PLMN_WITH_RAT_LEN的整数倍 */
    if (0 != (gastAtParaList[2].ulParaValue % TAF_AT_PLMN_WITH_RAT_LEN))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 最大只允许设置256个，因此最后的第6组(index=5)最大允许用户设置6个OPLMN */
    if ((gastAtParaList[2].ulParaValue > 6 * TAF_AT_PLMN_WITH_RAT_LEN)
     && (TAF_MAX_USER_CFG_OPLMN_GROUP_INDEX == gastAtParaList[1].ulParaValue))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第4个参数检查，PDU的实际长度必须与指定的pdulen相同 */
    if (gastAtParaList[3].usParaLen != gastAtParaList[2].ulParaValue)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第4个参数检查，PDU数据必须可以完成16进制字符串转换 */
    if (AT_SUCCESS != At_AsciiNum2HexString(gastAtParaList[3].aucPara, &gastAtParaList[3].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}
VOS_UINT32 AT_SetUserCfgOPlmnPara(VOS_UINT8 ucIndex)
{
    TAF_MMA_SET_EOPLMN_LIST_STRU        stEOPlmnSetPara;
    VOS_UINT32                          ulRst;

    PS_MEM_SET(&stEOPlmnSetPara, 0, sizeof(TAF_MMA_SET_EOPLMN_LIST_STRU));


    /* 参数个数和合法性检查,不合法直接返回失败 */
    ulRst = AT_SetEOPlmnParaCheck();
    if (AT_SUCCESS != ulRst)
    {
        return ulRst;
    }

    /* 将设置的AT参数封装成TAF_MMA_SET_EOPLMN_LIST_STRU的形式 */
    PS_MEM_CPY(stEOPlmnSetPara.aucVersion,
               gastAtParaList[0].aucPara,
               TAF_MAX_USER_CFG_OPLMN_VERSION_LEN);

    PS_MEM_CPY(stEOPlmnSetPara.aucOPlmnWithRat,
               gastAtParaList[3].aucPara,
               gastAtParaList[3].usParaLen);

    stEOPlmnSetPara.ucIndex      = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    stEOPlmnSetPara.ucOPlmnCount = (VOS_UINT8)(gastAtParaList[2].ulParaValue / TAF_AT_PLMN_WITH_RAT_LEN);

    /* 与MMA进行交互不成功返回失败，交互成功返回命令处理挂起状态 */
    ulRst = Taf_SetEOPlmnHandle(gastAtClientTab[ucIndex].usClientId, 0, &stEOPlmnSetPara);
    if (AT_SUCCESS == ulRst)
    {
        /* 指示当前用户的命令操作类型为设置命令 */
        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;
        gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_EOPLMN_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}



VOS_UINT32 AT_SetEcidPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulResult;
    AT_MTA_ECID_SET_REQ_STRU            stEcidSetReq;

    /* 参数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 初始化 */
    PS_MEM_SET(&stEcidSetReq, 0, sizeof(stEcidSetReq));

    stEcidSetReq.ulVersion = gastAtParaList[0].ulParaValue;

    /* 发送跨核消息到C核, 设置GPS参考时钟状态是否主动上报 */
    ulResult = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      ID_AT_MTA_ECID_SET_REQ,
                                      &stEcidSetReq,
                                      sizeof(stEcidSetReq),
                                      I0_UEPS_PID_MTA);

    if (TAF_SUCCESS != ulResult)
    {
        AT_WARN_LOG("AT_SetEcidPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECID_SET;

    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_ON == FEATURE_AT_HSUART)

VOS_VOID AT_UpdateUartCfgNV(VOS_VOID)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;
    TAF_NV_UART_CFG_STRU                stUartNvCfg;

    /* 初始化 */
    PS_MEM_SET(&stUartNvCfg, 0, sizeof(TAF_NV_UART_CFG_STRU));

    if (NV_OK != NV_ReadEx(MODEM_ID_0,
                           en_NV_Item_UART_CFG,
                           &stUartNvCfg,
                           sizeof(TAF_NV_UART_CFG_STRU)))
    {
        AT_ERR_LOG("AT_UpdateUartCfgNV: Read NV fail!");
        AT_HSUART_DBG_NV_READ_ERR(1);
        return;
    }

    /* 获取UART参数 */
    pstUartCtx                   = AT_GetUartCtxAddr();
    stUartNvCfg.ulBaudRate       = pstUartCtx->stPhyConfig.enBaudRate;
    stUartNvCfg.stFrame.ucFormat = pstUartCtx->stPhyConfig.stFrame.enFormat;
    stUartNvCfg.stFrame.ucParity = pstUartCtx->stPhyConfig.stFrame.enParity;

    /* 更新UART参数NV */
    if (NV_OK != NV_WriteEx(MODEM_ID_0,
                            en_NV_Item_UART_CFG,
                            &stUartNvCfg,
                            sizeof(TAF_NV_UART_CFG_STRU)))
    {
        AT_ERR_LOG("AT_UpdateUartCfgNV: Write NV fail!");
        AT_HSUART_DBG_NV_WRITE_ERR(1);
    }

    return ;
}
VOS_UINT32 AT_SetIprPara(VOS_UINT8 ucIndex)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulBaudRate;

    pstUartCtx = AT_GetUartCtxAddr();

    /* 只支持UART端口下发 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return AT_OK;
    }

    /* 参数检查 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        ulBaudRate = AT_UART_DEFAULT_BAUDRATE;
    }
    else
    {
        if (0 == gastAtParaList[0].usParaLen)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        ulBaudRate = gastAtParaList[0].ulParaValue;
    }

    /* 波特率有效性检查 */
    if (VOS_TRUE != AT_HSUART_IsBaudRateValid(ulBaudRate))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 保存波特率参数至上下文 */
    pstUartCtx->stPhyConfig.enBaudRate = ulBaudRate;

    /* 更新UART参数到NV */
    AT_UpdateUartCfgNV();

    /* 先上报OK */
    At_FormatResultData(ucIndex, AT_OK);

    /* 调用DRV函数设置串口的波特率 */
    if (VOS_OK != DRV_UDI_IOCTL(g_alAtUdiHandle[ucIndex], UART_IOCTL_SET_BAUD, (VOS_VOID *)&ulBaudRate))
    {
        AT_ERR_LOG("AT_SetIprPara, ERROR, Set Baud failed!");
        AT_HSUART_DBG_IOCTL_SET_BAUD_FAIL_NUM(1);
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}
VOS_INT32 AT_ConfigUartCharFrameParam(
    VOS_UINT8                           ucIndex,
    AT_UART_FORMAT_ENUM_UINT8           enFormat,
    AT_UART_PARITY_ENUM_UINT8           enParity
)
{
    AT_UART_FORMAT_PARAM_STRU          *pstFormatPara = VOS_NULL_PTR;
    VOS_UINT32                          ulUdiDataLenth;
    VOS_UINT32                          ulUdiStpLenth;
    VOS_UINT32                          ulUdiParity;
    VOS_UINT32                          ulResult;

    /* 参数初始化 */
    ulUdiDataLenth = WLEN_8_BITS;
    ulUdiStpLenth  = STP2_OFF;
    ulUdiParity    = PARITY_NO_CHECK;

    /* 将设置的帧格式和校验方法转换为DRV接口格式 */
    pstFormatPara = AT_HSUART_GetFormatParam(enFormat);
    if (VOS_NULL_PTR == pstFormatPara)
    {
       return VOS_ERROR;
    }

    ulResult = AT_HSUART_GetUdiValueByDataLen(pstFormatPara->enDataBitLength, &ulUdiDataLenth);
    if (VOS_OK != ulResult)
    {
        return VOS_ERROR;
    }

    ulResult = AT_HSUART_GetUdiValueByStopLen(pstFormatPara->enStopBitLength, &ulUdiStpLenth);
    if (VOS_OK != ulResult)
    {
        return VOS_ERROR;
    }

    if (AT_UART_PARITY_LEN_1_BIT == pstFormatPara->enParityBitLength)
    {
        ulResult = AT_HSUART_GetUdiValueByParity(enParity, &ulUdiParity);
        if (VOS_OK != ulResult)
        {
            return VOS_ERROR;
        }
    }
    else
    {
        ulUdiParity = PARITY_NO_CHECK;
    }

    /* 调用DRV函数设置串口数据位长度 */
    if (VOS_OK != DRV_UDI_IOCTL(g_alAtUdiHandle[ucIndex], UART_IOCTL_SET_WLEN, (VOS_VOID *)&ulUdiDataLenth))
    {
        AT_ERR_LOG("AT_ConfigUartCharFrameParam, ERROR, Set WLEN failed!");
        AT_HSUART_DBG_IOCTL_SET_WLEN_FAIL_NUM(1);
        return VOS_ERROR;
    }

    /* 调用DRV函数设置串口停止位长度 */
    if (VOS_OK != DRV_UDI_IOCTL(g_alAtUdiHandle[ucIndex], UART_IOCTL_SET_STP2, (VOS_VOID *)&ulUdiStpLenth))
    {
        AT_ERR_LOG("AT_ConfigUartCharFrameParam, ERROR, Set STP2 failed!");
        AT_HSUART_DBG_IOCTL_SET_STP_FAIL_NUM(1);
        return VOS_ERROR;
    }

    /* 调用DRV函数设置串口校验位 */
    if (VOS_OK != DRV_UDI_IOCTL(g_alAtUdiHandle[ucIndex], UART_IOCTL_SET_EPS, (VOS_VOID *)&ulUdiParity))
    {
        AT_ERR_LOG("AT_ConfigUartCharFrameParam, ERROR, Set Parity failed!");
        AT_HSUART_DBG_IOCTL_SET_PARITY_FAIL_NUM(1);
        return VOS_ERROR;
    }

    return VOS_OK;
}
VOS_UINT32 AT_SetIcfPara(VOS_UINT8 ucIndex)
{

    AT_UART_CTX_STRU                   *pstUartCtx      = VOS_NULL_PTR;
    AT_UART_FORMAT_ENUM_UINT8           enFormat;
    AT_UART_PARITY_ENUM_UINT8           enParity;

    pstUartCtx = AT_GetUartCtxAddr();

    /* 只支持UART端口下发 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return AT_OK;
    }

    /* 参数检查 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数有效性检查 */
    /* 参数为空，使用等效值(3,3),即:8个数据位1个停止位无校验位 */
    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        enFormat = AT_UART_DEFAULT_FORMAT;
        enParity = AT_UART_DEFAULT_PARITY;
    }
    else
    {
        /* AT_CMD_OPT_SET_PARA_CMD */
        /* AT+ICF= */
        if (0 == gucAtParaIndex)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* AT+ICF=, */
        if (0 == gastAtParaList[0].usParaLen)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        enFormat = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        if (gucAtParaIndex > 1)
        {
            if (0 != gastAtParaList[1].usParaLen)
            {
                enParity = (VOS_UINT8)gastAtParaList[1].ulParaValue;
            }
            else
            {
                /* AT+ICF=<format>, */
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }
        else
        {
            /* AT+ICF=<format> */
            enParity = AT_UART_DEFAULT_PARITY;
        }
    }

    /* 保存帧格式参数至上下文 */
    pstUartCtx->stPhyConfig.stFrame.enFormat = enFormat;
    pstUartCtx->stPhyConfig.stFrame.enParity = enParity;

    /* 更新UART参数到NV */
    AT_UpdateUartCfgNV();

    /* 先上报OK */
    At_FormatResultData(ucIndex, AT_OK);

    if (VOS_OK != AT_ConfigUartCharFrameParam(ucIndex, enFormat, enParity))
    {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_SetIfcPara(VOS_UINT8 ucIndex)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulRtsFlag;
    VOS_UINT32                          ulCtsFlag;

    ulRtsFlag   = VOS_FALSE;
    ulCtsFlag   = VOS_FALSE;
    pstUartCtx  = AT_GetUartCtxAddr();

    /* 命令参数个数检查 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT+IFC等效于AT+IFC=0,0
     * 注: 该处理为产品线要求, IUT-T-V.250中规定AT+IFC等效于AT+IFC=2,2
     */
    if (AT_CMD_OPT_SET_CMD_NO_PARA == g_stATParseCmd.ucCmdOptType)
    {
        pstUartCtx->stFlowCtrl.enDceByDte = AT_UART_DEFAULT_FC_DCE_BY_DTE;
        pstUartCtx->stFlowCtrl.enDteByDce = AT_UART_DEFAULT_FC_DTE_BY_DCE;
        AT_HSUART_ConfigFlowCtrl(ucIndex, VOS_FALSE, VOS_FALSE);
        return AT_OK;
    }

    /* AT_CMD_OPT_SET_PARA_CMD */
    /* AT+IFC= */
    if (0 == gucAtParaIndex)
    {
        /* AT+IFC= */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT+IFC=, */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (gucAtParaIndex > 1)
    {
        if (0 != gastAtParaList[1].usParaLen)
        {
            /* AT+IFC=<DCE_by_DTE>,<DTE_by_DCE> */
            pstUartCtx->stFlowCtrl.enDteByDce = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }
        else
        {
            /* AT+IFC=<DCE_by_DTE>, */
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        /* AT+IFC=<DCE_by_DTE> */
        pstUartCtx->stFlowCtrl.enDteByDce = AT_UART_DEFAULT_FC_DTE_BY_DCE;
    }

    pstUartCtx->stFlowCtrl.enDceByDte = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    if (AT_UART_FC_DCE_BY_DTE_RTS == pstUartCtx->stFlowCtrl.enDceByDte)
    {
        ulRtsFlag = VOS_TRUE;
    }

    if (AT_UART_FC_DTE_BY_DCE_CTS == pstUartCtx->stFlowCtrl.enDteByDce)
    {
        ulCtsFlag = VOS_TRUE;
    }

    AT_HSUART_ConfigFlowCtrl(ucIndex, ulRtsFlag, ulCtsFlag);

    return AT_OK;
}
VOS_UINT32 AT_SetOPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRslt;
    AT_DATA_MODE_TYPE                   ucDataMode;

    /* 获取当前数据模式 */
    ucDataMode = gastAtClientTab[ucIndex].DataMode;

    /* 如果当前通道模式为ONLINE-COMMAND模式, 直接恢复当前数据模式 */
    if (AT_ONLINE_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        At_SetMode(ucIndex, AT_DATA_MODE, ucDataMode);
        ulRslt = AT_CONNECT;
    }
    else
    {
        ulRslt = AT_NO_CARRIER;
    }

    return ulRslt;
}


VOS_UINT32 AT_SetSwitchUart(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulmode;

    if(AT_CLIENT_ID_HSUART != ucIndex)
    {
        vos_printf("AT_SetSwitchUart: 1\r\n");
        return AT_ERROR;
    }

    /* 判断CK是否校验通过 */
    if (VOS_TRUE != g_stSpWordCtx.ucShellPwdCheckFlag)
    {
        vos_printf("AT_SetSwitchUart: 2\r\n");
        AT_WARN_LOG("AT_SetSwitchUart:WARNING: datalock check faild!");
        return AT_ERROR;
    }

     /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        vos_printf("AT_SetSwitchUart: 3\r\n");
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        vos_printf("AT_SetSwitchUart: 4\r\n");
        return AT_ERROR;
    }

    /* 切换到 A shell*/
    if (AT_UART_MODE_ASHELL == gastAtParaList[0].ulParaValue)
    {
        vos_printf("AT_SetSwitchUart: 5\r\n");
        ulmode = UART_A_SHELL;
    }
    else if(AT_UART_MODE_CSHELL == gastAtParaList[0].ulParaValue)
    {
        vos_printf("AT_SetSwitchUart: 6\r\n");
        ulmode = UART_C_SHELL;
    }
    else
    {
        vos_printf("AT_SetSwitchUart: 7\r\n");
        AT_WARN_LOG("AT_SetSwitchUart: unknown value!");
        return AT_ERROR;
    }

    if (VOS_OK != DRV_UDI_IOCTL(g_alAtUdiHandle[ucIndex], UART_IOCTL_SET_AC_SHELL, &ulmode))
    {
        vos_printf("AT_SetSwitchUart: 8\r\n");
        AT_WARN_LOG("AT_SetSwitchUart:DRV_UDI_IOCTL faild!");
        return AT_ERROR;
    }

    AT_INFO_LOG("AT_SetSwitchUart: success and leave!!");
    vos_printf("AT_SetSwitchUart: success and leave!!");

    return AT_OK;

}
#endif

VOS_UINT32 At_SetAntSwitchPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    DRV_AGENT_ANTSWITCH_SET_STRU        stAntSwitchSetPara;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if(gastAtParaList[0].ulParaValue > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stAntSwitchSetPara.ulState = gastAtParaList[0].ulParaValue;
    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   DRV_AGENT_ANTSWITCH_SET_REQ,
                                   (VOS_VOID*)(&stAntSwitchSetPara),
                                   sizeof(stAntSwitchSetPara),
                                   I0_WUEPS_PID_DRV_AGENT);

    if (TAF_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ANTSWITCH_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
VOS_UINT32 AT_SetRATCombinePara(VOS_UINT8 ucIndex)
{
    TAF_NV_LC_CTRL_PARA_STRU            stCtrlData;
    MODEM_ID_ENUM_UINT16                enModemId;

    /* 参数检查 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    PS_MEM_SET(&stCtrlData, 0x0, sizeof(stCtrlData));

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_SetRATCombinePara: Get modem id fail.");

        return AT_ERROR;
    }

    if (NV_OK != NV_ReadEx(enModemId, en_NV_Item_LC_Ctrl_PARA, &stCtrlData, sizeof(TAF_NV_LC_CTRL_PARA_STRU)))
    {
        AT_ERR_LOG("AT_SetRATCombinePara: Get en_NV_Item_LC_Ctrl_PARA fail.");

        return AT_ERROR;
    }

    if (VOS_TRUE != stCtrlData.ucLCEnableFlg)
    {
        return AT_CME_OPERATION_NOT_ALLOWED;
    }

    if (0 == gastAtParaList[0].ulParaValue)
    {
        stCtrlData.enRatCombined = TAF_NVIM_LC_RAT_COMBINED_CL;
    }
    else
    {
        stCtrlData.enRatCombined = TAF_NVIM_LC_RAT_COMBINED_GUL;
    }

    if (NV_OK != NV_WriteEx(enModemId, en_NV_Item_LC_Ctrl_PARA, &stCtrlData, sizeof(TAF_NV_LC_CTRL_PARA_STRU)))
    {
        return AT_ERROR;
    }

    return AT_OK;
}


#if (FEATURE_ON == FEATURE_IMS)
VOS_UINT32 AT_SetCallModifyInitPara(VOS_UINT8 ucIndex)
{
    MN_CALL_MODIFY_REQ_STRU             stModifyReq;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stModifyReq, 0x00, sizeof(MN_CALL_MODIFY_REQ_STRU));

    /* 参数检查 */
    if(3 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen)
     || (0 == gastAtParaList[2].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stModifyReq.enCurrCallType      = (MN_CALL_TYPE_ENUM_U8)gastAtParaList[1].ulParaValue;
    stModifyReq.enExpectCallType    = (MN_CALL_TYPE_ENUM_U8)gastAtParaList[2].ulParaValue;

    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_CALL_MODIFY_REQ,
                                      gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      (MN_CALL_ID_T)gastAtParaList[0].ulParaValue,
                                      (MN_CALL_APP_REQ_PARM_UNION *)&stModifyReq);
    if (VOS_OK == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CALL_MODIFY_INIT_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
VOS_UINT32 AT_SetCallModifyAnsPara(VOS_UINT8 ucIndex)
{
    MN_CALL_MODIFY_REQ_STRU             stModifyReq;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stModifyReq, 0x00, sizeof(MN_CALL_MODIFY_REQ_STRU));

    /* 参数检查 */
    if(3 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen)
     || (0 == gastAtParaList[2].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    stModifyReq.enCurrCallType      = (MN_CALL_TYPE_ENUM_U8)gastAtParaList[1].ulParaValue;
    stModifyReq.enExpectCallType    = (MN_CALL_TYPE_ENUM_U8)gastAtParaList[2].ulParaValue;

    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_CALL_ANSWER_REMOTE_MODIFY_REQ,
                                      gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      (MN_CALL_ID_T)gastAtParaList[0].ulParaValue,
                                      (MN_CALL_APP_REQ_PARM_UNION *)&stModifyReq);
    if (VOS_OK == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CALL_MODIFY_ANS_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
VOS_UINT32 AT_FillEconfDialPara(
    VOS_UINT8                           ucIndex,
    TAF_CALL_ECONF_DIAL_REQ_STRU       *pstDialParam
)
{
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulCallNum;
    VOS_UINT32                          i;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* 默认情况是使用CCA设置的默认值 */
    pstDialParam->enCallMode  = pstSsCtx->enCModType;
    pstDialParam->stDataCfg   = pstSsCtx->stCbstDataCfg;
    pstDialParam->enClirCfg   = pstSsCtx->ucClirType;
    pstDialParam->stCugCfg    = pstSsCtx->stCcugCfg;

    /* 第二个参数为会议中心号码，目前不关注 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[2].usParaLen)
     || (0 == gastAtParaList[3].usParaLen)
     || (0 == gastAtParaList[4].usParaLen)
     || (0 == gastAtParaList[5].usParaLen))
    {
        return VOS_ERR;
    }
    else
    {
        /* 从第7个参数开始为与会者号码，第一个参数指明与会者人数 */
        ulCallNum = gastAtParaList[0].ulParaValue;

        for (i = 0; i < ulCallNum; i++)
        {
            if (0 == gastAtParaList[6 + i].usParaLen)
            {
                return VOS_ERR;
            }

            if (VOS_OK !=  AT_FillCalledNumPara(gastAtParaList[6 + i].aucPara,
                                                gastAtParaList[6 + i].usParaLen,
                                                &pstDialParam->stEconfCalllist.astDialNumber[i]))
            {
                return VOS_ERR;
            }
        }

        pstDialParam->stEconfCalllist.ulCallNum = ulCallNum;
    }

    /* 设置<I>参数 */
    if (0 == gastAtParaList[2].ulParaValue)
    {
        pstDialParam->enClirCfg = AT_CLIR_INVOKE;
    }
    else if (1 == gastAtParaList[2].ulParaValue)
    {
        pstDialParam->enClirCfg = AT_CLIR_SUPPRESS;
    }
    else
    {
        return VOS_ERR;
    }

    pstDialParam->enCallType            = (MN_CALL_TYPE_ENUM_U8)gastAtParaList[3].ulParaValue;
    pstDialParam->enVoiceDomain         = (TAF_CALL_VOICE_DOMAIN_ENUM_UINT8)gastAtParaList[4].ulParaValue;
    pstDialParam->ucImsInvitePtptFlag   = VOS_FALSE;

    return VOS_OK;

}


VOS_UINT32 AT_SetEconfDialPara(VOS_UINT8 ucIndex)
{
    TAF_CALL_ECONF_DIAL_REQ_STRU        stDialReq;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stDialReq, 0x00, sizeof(TAF_CALL_ECONF_DIAL_REQ_STRU));

    /* 参数检查 */
    if ((gucAtParaIndex < AT_CMD_ECONF_DIAL_MIN_PARA_NUM)
     || (gucAtParaIndex > AT_CMD_ECONF_DIAL_MAX_PARA_NUM))
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数不对 */
    /* 参数的个数应该是total_callnum+6 */
    if ((gastAtParaList[0].ulParaValue + 6) != gucAtParaIndex)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 检查并填写输入的参数 */
    ulResult = AT_FillEconfDialPara(ucIndex, &stDialReq);
    if (VOS_OK != ulResult)
    {
        AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_INVALID_PARAMETER);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulResult = MN_CALL_SendAppRequest(TAF_CALL_APP_ECONF_DIAL_REQ,
                                      gastAtClientTab[ucIndex].usClientId,
                                      gastAtClientTab[ucIndex].opId,
                                      0,
                                      (MN_CALL_APP_REQ_PARM_UNION *)&stDialReq);
    if (VOS_OK == ulResult)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_ECONF_DIAL_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

/* Added by zwx247453 for VOLTE SWITCH, 2015-02-02, Begin */
/*****************************************************************************
 函 数 名  : AT_SetImsSwitchPara
 功能描述  : 设置IMS 打开和关闭
             AT^IMSSWITCH=<value>
 输入参数  : ucIndex - 端口索引
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年02月02日
    作    者   : zwx247453
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 AT_SetImsSwitchPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    TAF_MMA_IMS_SWITCH_SET_ENUM_UINT8   enImsSwitch;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    enImsSwitch = (TAF_MMA_IMS_SWITCH_SET_ENUM_UINT8)gastAtParaList[0].ulParaValue;

    ulRst = TAF_MMA_SetImsSwitchReq(WUEPS_PID_AT,
                                    gastAtClientTab[ucIndex].usClientId,
                                    0,
                                    enImsSwitch);
    if (VOS_TRUE != ulRst)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_IMS_SWITCH_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/*****************************************************************************
 函 数 名  : AT_InputValueTransToVoiceDomain
 功能描述  : 将AT输入的参数值转换为与modem侧对应的voice domain值
 输入参数  : ulValue
 输出参数  : penVoiceDomain
 返 回 值  : VOS_TRUE
             VOS_FALSE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年02月04日
    作    者   : zwx247453
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 AT_InputValueTransToVoiceDomain(
    VOS_UINT32                          ulValue,
    TAF_MMA_VOICE_DOMAIN_ENUM_UINT32   *penVoiceDomain
)
{
    VOS_UINT32                          ulRst;

    ulRst = VOS_TRUE;

    switch (ulValue)
    {
        case AT_VOICE_DOMAIN_TYPE_CS_ONLY:
            *penVoiceDomain = TAF_MMA_VOICE_DOMAIN_CS_ONLY;
            break;

        case AT_VOICE_DOMAIN_TYPE_CS_PREFERRED:
            *penVoiceDomain = TAF_MMA_VOICE_DOMAIN_CS_PREFERRED;
            break;

        case AT_VOICE_DOMAIN_TYPE_IMS_PS_PREFERRED:
            *penVoiceDomain = TAF_MMA_VOICE_DOMAIN_IMS_PS_PREFERRED;
            break;

       case AT_VOICE_DOMAIN_TYPE_IMS_PS_ONLY:
            *penVoiceDomain = TAF_MMA_VOICE_DOMAIN_IMS_PS_ONLY;
            break;

        default:
            *penVoiceDomain = TAF_MMA_VOICE_DOMAIN_BUTT;
            ulRst = VOS_FALSE;
            break;
    }

    return ulRst;
}

/*****************************************************************************
 函 数 名  : AT_SetCevdpPara
 功能描述  : 设置语音优选域
             AT+CEVDP=[<setting>] (1:cs only 2:cs prefer 3:ps prefer 4:ps only)
 输入参数  : ucIndex - 端口索引
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年02月02日
    作    者   : zwx247453
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 AT_SetCevdpPara(VOS_UINT8 ucIndex)
{
    TAF_MMA_VOICE_DOMAIN_ENUM_UINT32    enVoiceDomain;
    VOS_UINT32                          ulRst;

    ulRst         = VOS_FALSE;
    enVoiceDomain = TAF_MMA_VOICE_DOMAIN_BUTT;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数检查 */
    if (1 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空，协议没有明确规定默认值，此处默认设置为CS ONLY */
    if (0 == gastAtParaList[0].usParaLen)
    {
        enVoiceDomain = TAF_MMA_VOICE_DOMAIN_CS_ONLY;
    }
    else
    {
        if (VOS_TRUE != AT_InputValueTransToVoiceDomain(gastAtParaList[0].ulParaValue, &enVoiceDomain))
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    ulRst = TAF_MMA_SetVoiceDomainReq(WUEPS_PID_AT,
                                      gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      enVoiceDomain);
    if (VOS_TRUE != ulRst)
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VOICE_DOMAIN_SET;

    return AT_WAIT_ASYNC_RETURN;
}
/* Added by zwx247453 for VOLTE SWITCH, 2015-02-02, End */

#endif


VOS_UINT32 AT_SetCclkPara(VOS_UINT8 ucIndex)
{
    return AT_CME_OPERATION_NOT_SUPPORTED;
}



VOS_UINT32 At_SetFemCtrl(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;
    AT_MTA_SET_FEMCTRL_REQ_STRU         stFemctrlReq;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 4)
    {
        return AT_TOO_MANY_PARA;
    }


    stFemctrlReq.ulCtrlType     = gastAtParaList[0].ulParaValue;
    stFemctrlReq.ulPara1        = gastAtParaList[1].ulParaValue;
    stFemctrlReq.ulPara2        = gastAtParaList[2].ulParaValue;
    stFemctrlReq.ulPara3        = gastAtParaList[3].ulParaValue;

    ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   ID_AT_MTA_SET_FEMCTRL_REQ,
                                   (VOS_VOID*)&stFemctrlReq,
                                   sizeof(stFemctrlReq),
                                   I0_UEPS_PID_MTA);

    if (AT_SUCCESS == ulRst)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_FEMCTRL_SET;
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}

#if (FEATURE_ON == FEATURE_HUAWEI_VP)

VOS_UINT32 AT_SetVoicePreferPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32      ulVoicePreferAct;
    VOS_UINT32      ulRslt;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数为空 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ulVoicePreferAct = gastAtParaList[0].ulParaValue;

    ulRslt = AT_SndSetVoicePrefer(gastAtClientTab[ucIndex].usClientId,
                               gastAtClientTab[ucIndex].opId,
                               ulVoicePreferAct);
    if (VOS_OK == ulRslt)
    {
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VOICEPREFER_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
#endif

/*Added by z00306637 for RATRFSWITCH, 2015-01-04, begin*/
/*****************************************************************************
 函 数 名  : At_SetRatRfSwitch
 功能描述  : 设置RF Profile Id
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年01月04日
    作    者   : z00306637
    修改内容   : 新增函数
*****************************************************************************/
VOS_UINT32 At_SetRatRfSwitch(VOS_UINT8 ucIndex)
{
    NAS_NV_TRI_MODE_ENABLE_STRU                     stTriModeEnableStru;
    NAS_NV_TRI_MODE_FEM_PROFILE_ID_STRU             stTriModeFemProfileIdStru;

    PS_MEM_SET(&stTriModeEnableStru, 0x0, sizeof(stTriModeEnableStru));
    PS_MEM_SET(&stTriModeFemProfileIdStru, 0x0, sizeof(stTriModeFemProfileIdStru));

    if(AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((0 == gastAtParaList[0].usParaLen)
     || (1 != gucAtParaIndex))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*查询是否支持全网通功能*/
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_TRI_MODE_ENABLE, &stTriModeEnableStru, sizeof(stTriModeEnableStru)))
    {
        AT_WARN_LOG("At_SetRatRfSwitch:read en_NV_Item_TRI_MODE_ENABLE failed");
        return AT_ERROR;
    }

    /*不支持，返回*/
    if(0 == stTriModeEnableStru.usEnable)
    {
        AT_WARN_LOG("At_SetRatRfSwitch:uhwEnable is not enabled");
        return AT_CME_OPERATION_NOT_ALLOWED;
    }

    stTriModeFemProfileIdStru.ulProfileId=gastAtParaList[0].ulParaValue;

    /* 配置Profile Id*/
    if (NV_OK != NV_WriteEx(MODEM_ID_0, en_NV_Item_TRI_MODE_FEM_PROFILE_ID, &stTriModeFemProfileIdStru, sizeof(stTriModeFemProfileIdStru)))
    {
        AT_WARN_LOG("At_SetRatRfSwitch: MODEM_ID_0 write en_NV_Item_TRI_MODE_FEM_PROFILE_ID failed");
        return AT_ERROR;
    }

    return AT_OK;
}

/*Added by z00306637 for RATRFSWITCH, 2015-01-04, end*/

#ifdef  __cplusplus
#if  __cplusplus
  }
  #endif
#endif

