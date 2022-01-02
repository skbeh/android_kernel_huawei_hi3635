/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : AtEventReport.c
  Author       : ---
  Version      : V100R001
  Date         : 2005-04-19
  Description  : 该C文件给出了---模块的实现
  Function List:


  History      :
   1. Date:2005-04-19
      Author: ---
      Modification:Create

************************************************************************/


/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "ATCmdProc.h"
#include "siappstk.h"
#include "PppInterface.h"
#include "AtDataProc.h"
#include "AtEventReport.h"
#include "TafDrvAgent.h"
#include "AtOamInterface.h"
#if( FEATURE_ON == FEATURE_CSD )
#include "AtCsdInterface.h"
#endif

#include "AtInputProc.h"
#include "FcInterface.h"
#include "AtCmdMsgProc.h"

#if(FEATURE_ON == FEATURE_LTE)
#include "gen_msg.h"
#include "at_lte_common.h"
#endif

#include "TafAppMma.h"

#include "AppVcApi.h"
#include "TafAppRabm.h"
#include "AtCmdSimProc.h"

#include  "product_config.h"

#include "TafStdlib.h"

#include "AtMsgPrint.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/
/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_AT_EVENTREPORT_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量定义
*****************************************************************************/
const AT_SMS_ERROR_CODE_MAP_STRU        g_astAtSmsErrorCodeMap[] =
{
    {TAF_MSG_ERROR_RP_CAUSE_UNASSIGNED_UNALLOCATED_NUMBER,                              AT_CMS_UNASSIGNED_UNALLOCATED_NUMBER},
    {TAF_MSG_ERROR_RP_CAUSE_OPERATOR_DETERMINED_BARRING,                                AT_CMS_OPERATOR_DETERMINED_BARRING},
    {TAF_MSG_ERROR_RP_CAUSE_CALL_BARRED,                                                AT_CMS_CALL_BARRED},
    {TAF_MSG_ERROR_RP_CAUSE_SHORT_MESSAGE_TRANSFER_REJECTED,                            AT_CMS_SHORT_MESSAGE_TRANSFER_REJECTED},
    {TAF_MSG_ERROR_RP_CAUSE_DESTINATION_OUT_OF_ORDER,                                   AT_CMS_DESTINATION_OUT_OF_SERVICE},
    {TAF_MSG_ERROR_RP_CAUSE_UNIDENTIFIED_SUBSCRIBER,                                    AT_CMS_UNIDENTIFIED_SUBSCRIBER},
    {TAF_MSG_ERROR_RP_CAUSE_FACILITY_REJECTED,                                          AT_CMS_FACILITY_REJECTED},
    {TAF_MSG_ERROR_RP_CAUSE_UNKNOWN_SUBSCRIBER,                                         AT_CMS_UNKNOWN_SUBSCRIBER},
    {TAF_MSG_ERROR_RP_CAUSE_NETWORK_OUT_OF_ORDER,                                       AT_CMS_NETWORK_OUT_OF_ORDER},
    {TAF_MSG_ERROR_RP_CAUSE_TEMPORARY_FAILURE,                                          AT_CMS_TEMPORARY_FAILURE},
    {TAF_MSG_ERROR_RP_CAUSE_CONGESTION,                                                 AT_CMS_CONGESTION},
    {TAF_MSG_ERROR_RP_CAUSE_RESOURCES_UNAVAILABLE_UNSPECIFIED,                          AT_CMS_RESOURCES_UNAVAILABLE_UNSPECIFIED},
    {TAF_MSG_ERROR_RP_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED,                          AT_CMS_REQUESTED_FACILITY_NOT_SUBSCRIBED},
    {TAF_MSG_ERROR_RP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED,                         AT_CMS_REQUESTED_FACILITY_NOT_IMPLEMENTED},
    {TAF_MSG_ERROR_RP_CAUSE_INVALID_SHORT_MESSAGE_TRANSFER_REFERENCE_VALUE,             AT_CMS_INVALID_SHORT_MESSAGE_TRANSFER_REFERENCE_VALUE},
    {TAF_MSG_ERROR_RP_CAUSE_INVALID_MANDATORY_INFORMATION,                              AT_CMS_INVALID_MANDATORY_INFORMATION},
    {TAF_MSG_ERROR_RP_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED,               AT_CMS_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED},
    {TAF_MSG_ERROR_RP_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE,   AT_CMS_MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE},
    {TAF_MSG_ERROR_RP_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED,        AT_CMS_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED},
    {TAF_MSG_ERROR_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,                                 AT_CMS_PROTOCOL_ERROR_UNSPECIFIED},
    {TAF_MSG_ERROR_RP_CAUSE_INTERWORKING_UNSPECIFIED,                                   AT_CMS_INTERWORKING_UNSPECIFIED},
    {TAF_MSG_ERROR_TP_FCS_TELEMATIC_INTERWORKING_NOT_SUPPORTED,                         AT_CMS_TELEMATIC_INTERWORKING_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_SHORT_MESSAGE_TYPE_0_NOT_SUPPORTED,                           AT_CMS_SHORT_MESSAGE_TYPE_0_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_CANNOT_REPLACE_SHORT_MESSAGE,                                 AT_CMS_CANNOT_REPLACE_SHORT_MESSAGE},
    {TAF_MSG_ERROR_TP_FCS_UNSPECIFIED_TPPID_ERROR,                                      AT_CMS_UNSPECIFIED_TPPID_ERROR},
    {TAF_MSG_ERROR_TP_FCS_DATA_CODING_SCHEME_ALPHABET_NOT_SUPPORTED,                    AT_CMS_DATA_CODING_SCHEME_ALPHABET_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_MESSAGE_CLASS_NOT_SUPPORTED,                                  AT_CMS_MESSAGE_CLASS_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_UNSPECIFIED_TPDCS_ERROR,                                      AT_CMS_UNSPECIFIED_TPDCS_ERROR},
    {TAF_MSG_ERROR_TP_FCS_COMMAND_CANNOT_BE_ACTIONED,                                   AT_CMS_COMMAND_CANNOT_BE_ACTIONED},
    {TAF_MSG_ERROR_TP_FCS_COMMAND_UNSUPPORTED,                                          AT_CMS_COMMAND_UNSUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_UNSPECIFIED_TPCOMMAND_ERROR,                                  AT_CMS_UNSPECIFIED_TPCOMMAND_ERROR},
    {TAF_MSG_ERROR_TP_FCS_TPDU_NOT_SUPPORTED,                                           AT_CMS_TPDU_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_SC_BUSY,                                                      AT_CMS_SC_BUSY},
    {TAF_MSG_ERROR_TP_FCS_NO_SC_SUBSCRIPTION,                                           AT_CMS_NO_SC_SUBSCRIPTION},
    {TAF_MSG_ERROR_TP_FCS_SC_SYSTEM_FAILURE,                                            AT_CMS_SC_SYSTEM_FAILURE},
    {TAF_MSG_ERROR_TP_FCS_INVALID_SME_ADDRESS,                                          AT_CMS_INVALID_SME_ADDRESS},
    {TAF_MSG_ERROR_TP_FCS_DESTINATION_SME_BARRED,                                       AT_CMS_DESTINATION_SME_BARRED},
    {TAF_MSG_ERROR_TP_FCS_SM_REJECTEDDUPLICATE_SM,                                      AT_CMS_SM_REJECTEDDUPLICATE_SM},
    {TAF_MSG_ERROR_TP_FCS_TPVPF_NOT_SUPPORTED,                                          AT_CMS_TPVPF_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_TPVP_NOT_SUPPORTED,                                           AT_CMS_TPVP_NOT_SUPPORTED},
    {TAF_MSG_ERROR_TP_FCS_SIM_SMS_STORAGE_FULL,                                         AT_CMS_SIM_SMS_STORAGE_FULL},
    {TAF_MSG_ERROR_TP_FCS_NO_SMS_STORAGE_CAPABILITY_IN_SIM,                             AT_CMS_NO_SMS_STORAGE_CAPABILITY_IN_SIM},
    {TAF_MSG_ERROR_TP_FCS_ERROR_IN_MS,                                                  AT_CMS_ERROR_IN_MS},
    {TAF_MSG_ERROR_TP_FCS_MEMORY_CAPACITY_EXCEEDED,                                     AT_CMS_MEMORY_CAPACITY_EXCEEDED},
    {TAF_MSG_ERROR_TP_FCS_SIM_APPLICATION_TOOLKIT_BUSY,                                 AT_CMS_SIM_APPLICATION_TOOLKIT_BUSY},
    {TAF_MSG_ERROR_TP_FCS_SIM_DATA_DOWNLOAD_ERROR,                                      AT_CMS_SIM_DATA_DOWNLOAD_ERROR},
    {TAF_MSG_ERROR_TP_FCS_UNSPECIFIED_ERROR_CAUSE,                                      AT_CMS_UNSPECIFIED_ERROR_CAUSE},
    {TAF_MSG_ERROR_STATE_NOT_COMPATIBLE,                                                AT_CMS_ME_FAILURE},
    {TAF_MSG_ERROR_NO_SERVICE,                                                          AT_CMS_NO_NETWORK_SERVICE},
    {TAF_MSG_ERROR_TC1M_TIMEOUT,                                                        AT_CMS_NETWORK_TIMEOUT},
    {TAF_MSG_ERROR_TR1M_TIMEOUT,                                                        AT_CMS_NETWORK_TIMEOUT},
    {TAF_MSG_ERROR_TR2M_TIMEOUT,                                                        AT_CMS_NO_CNMA_ACKNOWLEDGEMENT_EXPECTED},
};

const AT_PS_EVT_FUNC_TBL_STRU           g_astAtPsEvtFuncTbl[] =
{
    /* PS CALL */
    {ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF,
        AT_RcvTafPsCallEvtPdpActivateCnf},
    {ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ,
        AT_RcvTafPsCallEvtPdpActivateRej},
    {ID_EVT_TAF_PS_CALL_PDP_MANAGE_IND,
        AT_RcvTafPsCallEvtPdpManageInd},
    {ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_IND,
        AT_RcvTafPsCallEvtPdpActivateInd},
    {ID_EVT_TAF_PS_CALL_PDP_MODIFY_CNF,
        AT_RcvTafPsCallEvtPdpModifyCnf},
    {ID_EVT_TAF_PS_CALL_PDP_MODIFY_REJ,
        AT_RcvTafPsCallEvtPdpModifyRej},
    {ID_EVT_TAF_PS_CALL_PDP_MODIFY_IND,
        AT_RcvTafPsCallEvtPdpModifiedInd},
    {ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF,
        AT_RcvTafPsCallEvtPdpDeactivateCnf},
    {ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_IND,
        AT_RcvTafPsCallEvtPdpDeactivatedInd},

    {ID_EVT_TAF_PS_CALL_ORIG_CNF,
        AT_RcvTafPsCallEvtCallOrigCnf},
    {ID_EVT_TAF_PS_CALL_END_CNF,
        AT_RcvTafPsCallEvtCallEndCnf},
    {ID_EVT_TAF_PS_CALL_MODIFY_CNF,
        AT_RcvTafPsCallEvtCallModifyCnf},
    {ID_EVT_TAF_PS_CALL_ANSWER_CNF,
        AT_RcvTafPsCallEvtCallAnswerCnf},
    {ID_EVT_TAF_PS_CALL_HANGUP_CNF,
        AT_RcvTafPsCallEvtCallHangupCnf},

    /* D */
    {ID_EVT_TAF_PS_GET_D_GPRS_ACTIVE_TYPE_CNF,
        AT_RcvTafPsEvtGetGprsActiveTypeCnf},

    /* PPP */
    {ID_EVT_TAF_PS_PPP_DIAL_ORIG_CNF,
        AT_RcvTafPsEvtPppDialOrigCnf},

    /* +CGDCONT */
    {ID_EVT_TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtSetPrimPdpContextInfoCnf},
    {ID_EVT_TAF_PS_GET_PRIM_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtGetPrimPdpContextInfoCnf},

    /* +CGDSCONT */
    {ID_EVT_TAF_PS_SET_SEC_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtSetSecPdpContextInfoCnf},
    {ID_EVT_TAF_PS_GET_SEC_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtGetSecPdpContextInfoCnf},

    /* +CGTFT */
    {ID_EVT_TAF_PS_SET_TFT_INFO_CNF,
        AT_RcvTafPsEvtSetTftInfoCnf},
    {ID_EVT_TAF_PS_GET_TFT_INFO_CNF,
        AT_RcvTafPsEvtGetTftInfoCnf},

    /* +CGEQREQ */
    {ID_EVT_TAF_PS_SET_UMTS_QOS_INFO_CNF,
        AT_RcvTafPsEvtSetUmtsQosInfoCnf},
    {ID_EVT_TAF_PS_GET_UMTS_QOS_INFO_CNF,
        AT_RcvTafPsEvtGetUmtsQosInfoCnf},

    /* +CGEQMIN */
    {ID_EVT_TAF_PS_SET_UMTS_QOS_MIN_INFO_CNF,
        AT_RcvTafPsEvtSetUmtsQosMinInfoCnf},
    {ID_EVT_TAF_PS_GET_UMTS_QOS_MIN_INFO_CNF,
        AT_RcvTafPsEvtGetUmtsQosMinInfoCnf},

    /* +CGEQNEG */
    {ID_EVT_TAF_PS_GET_DYNAMIC_UMTS_QOS_INFO_CNF,
        AT_RcvTafPsEvtGetDynamicUmtsQosInfoCnf},

    /* +CGACT */
    {ID_EVT_TAF_PS_SET_PDP_CONTEXT_STATE_CNF,
        AT_RcvTafPsEvtSetPdpStateCnf},
    {ID_EVT_TAF_PS_GET_PDP_CONTEXT_STATE_CNF,
        AT_RcvTafPsEvtGetPdpStateCnf},

    /* +CGPADDR */
    {ID_EVT_TAF_PS_GET_PDP_IP_ADDR_INFO_CNF,
        AT_RcvTafPsEvtGetPdpIpAddrInfoCnf},
    {ID_EVT_TAF_PS_GET_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtGetPdpContextInfoCnf},

    /* +CGAUTO */
    {ID_EVT_TAF_PS_SET_ANSWER_MODE_INFO_CNF,
        AT_RcvTafPsEvtSetAnsModeInfoCnf},
    {ID_EVT_TAF_PS_GET_ANSWER_MODE_INFO_CNF,
        AT_RcvTafPsEvtGetAnsModeInfoCnf},

    /* +CGCONTRDP */
    {ID_EVT_TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtGetDynamicPrimPdpContextInfoCnf},
    /* +CGSCONTRDP */
    {ID_EVT_TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF,
        AT_RcvTafPsEvtGetDynamicSecPdpContextInfoCnf},

    /* +CGTFTRDP */
    {ID_EVT_TAF_PS_GET_DYNAMIC_TFT_INFO_CNF,
        AT_RcvTafPsEvtGetDynamicTftInfoCnf},

    /* +CGEQOS */
    {ID_EVT_TAF_PS_SET_EPS_QOS_INFO_CNF,
        AT_RcvTafPsEvtSetEpsQosInfoCnf},
    {ID_EVT_TAF_PS_GET_EPS_QOS_INFO_CNF,
        AT_RcvTafPsEvtGetEpsQosInfoCnf},

    /* +CGEQOSRDP */
    {ID_EVT_TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF,
        AT_RcvTafPsEvtGetDynamicEpsQosInfoCnf},

    /* ^CDQF/^DSFLOWQRY */
    {ID_EVT_TAF_PS_GET_DSFLOW_INFO_CNF,
        AT_RcvTafPsEvtGetDsFlowInfoCnf},

    /* ^CDCF/^DSFLOWCLR */
    {ID_EVT_TAF_PS_CLEAR_DSFLOW_CNF,
        AT_RcvTafPsEvtClearDsFlowInfoCnf},

    /* ^CDSF/^DSFLOWRPT/^FLOWRPTCTRL */
    {ID_EVT_TAF_PS_CONFIG_DSFLOW_RPT_CNF,
        AT_RcvTafPsEvtConfigDsFlowRptCnf},

    /* ^DSFLOWRPT */
    {ID_EVT_TAF_PS_REPORT_DSFLOW_IND,
        AT_RcvTafPsEvtReportDsFlowInd},

    /* ^CGAUTH */
    {ID_EVT_TAF_PS_SET_PDP_AUTH_INFO_CNF,
        AT_RcvTafPsEvtSetPdpAuthInfoCnf},
    {ID_EVT_TAF_PS_GET_PDP_AUTH_INFO_CNF,
        AT_RcvTafPsEvtGetPdpAuthInfoCnf},

    /* ^CGDNS */
    {ID_EVT_TAF_PS_SET_PDP_DNS_INFO_CNF,
        AT_RcvTafPsEvtSetPdpDnsInfoCnf},
    {ID_EVT_TAF_PS_GET_PDP_DNS_INFO_CNF,
        AT_RcvTafPsEvtGetPdpDnsInfoCnf},

    /* ^AUTHDATA */
    {ID_EVT_TAF_PS_SET_AUTHDATA_INFO_CNF,
        AT_RcvTafPsEvtSetAuthDataInfoCnf},
    {ID_EVT_TAF_PS_GET_AUTHDATA_INFO_CNF,
        AT_RcvTafPsEvtGetAuthDataInfoCnf},

#if (FEATURE_ON == FEATURE_IPV6)
    {ID_EVT_TAF_PS_CALL_PDP_IPV6_INFO_IND,
        AT_RcvTafPsEvtReportRaInfo},
#endif

    {ID_EVT_TAF_PS_CALL_PDP_DISCONNECT_IND,
        AT_RcvTafPsEvtPdpDisconnectInd},
    {ID_EVT_TAF_PS_GET_NEGOTIATION_DNS_CNF,
        AT_RcvTafPsEvtGetDynamicDnsInfoCnf},

#if(FEATURE_ON == FEATURE_LTE)
    {ID_EVT_TAF_PS_LTECS_INFO_CNF,
        atReadLtecsCnfProc},
    {ID_EVT_TAF_PS_CEMODE_INFO_CNF,
        atReadCemodeCnfProc},
     {ID_EVT_TAF_PS_SET_PDP_PROF_INFO_CNF,
        AT_RcvTafPsEvtSetPdprofInfoCnf},
    {ID_EVT_TAF_PS_GET_CID_SDF_CNF,
        AT_RcvTafPsEvtGetCidSdfInfoCnf},
#endif

    {ID_EVT_TAF_PS_SET_APDSFLOW_RPT_CFG_CNF,
        AT_RcvTafPsEvtSetApDsFlowRptCfgCnf},
    {ID_EVT_TAF_PS_GET_APDSFLOW_RPT_CFG_CNF,
        AT_RcvTafPsEvtGetApDsFlowRptCfgCnf},
    {ID_EVT_TAF_PS_APDSFLOW_REPORT_IND,
        AT_RcvTafPsEvtApDsFlowReportInd},

    {ID_EVT_TAF_PS_SET_DSFLOW_NV_WRITE_CFG_CNF,
        AT_RcvTafPsEvtSetDsFlowNvWriteCfgCnf},
    {ID_EVT_TAF_PS_GET_DSFLOW_NV_WRITE_CFG_CNF,
        AT_RcvTafPsEvtGetDsFlowNvWriteCfgCnf},
};

/* 主动上报命令与控制Bit位对应表 */
/* 命令对应顺序为Bit0~Bit63 */
AT_RPT_CMD_INDEX_ENUM_UINT8             g_aenAtCurcRptCmdTable[] =
{
    AT_RPT_CMD_MODE,        AT_RPT_CMD_RSSI,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_SRVST,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_SIMST,       AT_RPT_CMD_TIME,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_ANLEVEL,     AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_SMMEMFULL,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_CTZV,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_DSFLOWRPT,   AT_RPT_CMD_BUTT,
    AT_RPT_CMD_ORIG,        AT_RPT_CMD_CONF,        AT_RPT_CMD_CONN,        AT_RPT_CMD_CEND,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_STIN,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_CERSSI,      AT_RPT_CMD_LWCLASH,     AT_RPT_CMD_XLEMA,       AT_RPT_CMD_ACINFO,
    AT_RPT_CMD_PLMN,        AT_RPT_CMD_CALLSTATE,   AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT
};



AT_RPT_CMD_INDEX_ENUM_UINT8             g_aenAtUnsolicitedRptCmdTable[] =
{
    AT_RPT_CMD_MODE,        AT_RPT_CMD_RSSI,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_SRVST,
    AT_RPT_CMD_CREG,        AT_RPT_CMD_SIMST,       AT_RPT_CMD_TIME,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_ANLEVEL,     AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_CTZV,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_DSFLOWRPT,   AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_CUSD,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_CSSI,
    AT_RPT_CMD_CSSU,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_CERSSI,      AT_RPT_CMD_LWURC,       AT_RPT_CMD_BUTT,        AT_RPT_CMD_CUUS1U,
    AT_RPT_CMD_CUUS1I,      AT_RPT_CMD_CGREG,       AT_RPT_CMD_CEREG,       AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,
    AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT,        AT_RPT_CMD_BUTT
};

AT_CME_CALL_ERR_CODE_MAP_STRU           g_astAtCmeCallErrCodeMapTbl[] =
{
    { AT_CME_INCORRECT_PARAMETERS,      TAF_CS_CAUSE_INVALID_PARAMETER              },
    { AT_CME_SIM_FAILURE,               TAF_CS_CAUSE_SIM_NOT_EXIST                  },
    { AT_CME_SIM_PIN_REQUIRED,          TAF_CS_CAUSE_SIM_PIN_NEED                   },
    { AT_CME_UNKNOWN,                   TAF_CS_CAUSE_NO_CALL_ID                     },
    { AT_CME_OPERATION_NOT_ALLOWED,     TAF_CS_CAUSE_NOT_ALLOW                      },
    { AT_CME_INCORRECT_PARAMETERS,      TAF_CS_CAUSE_STATE_ERROR                    },
    { AT_CME_FDN_FAILED,                            TAF_CS_CAUSE_FDN_CHECK_FAILURE              },
    { AT_CME_CALL_CONTROL_BEYOND_CAPABILITY,        TAF_CS_CAUSE_CALL_CTRL_BEYOND_CAPABILITY    },
    { AT_CME_CALL_CONTROL_FAILED,                   TAF_CS_CAUSE_CALL_CTRL_TIMEOUT              },
    { AT_CME_CALL_CONTROL_FAILED,                   TAF_CS_CAUSE_CALL_CTRL_NOT_ALLOWED          },
    { AT_CME_CALL_CONTROL_FAILED,                   TAF_CS_CAUSE_CALL_CTRL_INVALID_PARAMETER    },
    { AT_CME_UNKNOWN,                   TAF_CS_CAUSE_UNKNOWN                        }
};

AT_CMS_SMS_ERR_CODE_MAP_STRU           g_astAtCmsSmsErrCodeMapTbl[] =
{
    { AT_CMS_U_SIM_BUSY,                            MN_ERR_CLASS_SMS_UPDATE_USIM},
    { AT_CMS_U_SIM_NOT_INSERTED,                    MN_ERR_CLASS_SMS_NOUSIM},
    { AT_CMS_MEMORY_FULL,                           MN_ERR_CLASS_SMS_STORAGE_FULL},
    { AT_CMS_U_SIM_PIN_REQUIRED,                    MN_ERR_CLASS_SMS_NEED_PIN1},
    { AT_CMS_U_SIM_PUK_REQUIRED,                    MN_ERR_CLASS_SMS_NEED_PUK1},
    { AT_CMS_U_SIM_FAILURE,                         MN_ERR_CLASS_SMS_UNAVAILABLE},
    { AT_CMS_OPERATION_NOT_ALLOWED,                 MN_ERR_CLASS_SMS_FEATURE_INAVAILABLE },
    { AT_CMS_SMSC_ADDRESS_UNKNOWN,                  MN_ERR_CLASS_SMS_INVALID_SCADDR},
    { AT_CMS_INVALID_PDU_MODE_PARAMETER,            MN_ERR_CLASS_SMS_MSGLEN_OVERFLOW},
    { AT_CMS_FDN_DEST_ADDR_FAILED,                  MN_ERR_CLASS_FDN_CHECK_DN_FAILURE},
    { AT_CMS_FDN_SERVICE_CENTER_ADDR_FAILED,        MN_ERR_CLASS_FDN_CHECK_SC_FAILURE},
    { AT_CMS_MO_SMS_CONTROL_FAILED,                 MN_ERR_CLASS_SMS_MO_CTRL_ACTION_NOT_ALLOWED},
    { AT_CMS_MO_SMS_CONTROL_FAILED,                 MN_ERR_CLASS_SMS_MO_CTRL_USIM_PARA_ERROR},
    { AT_CMS_MEMORY_FAILURE,                        MN_ERR_NOMEM}

};




AT_SMS_RSP_PROC_FUN g_aAtSmsMsgProcTable[MN_MSG_EVT_MAX] = {
    /*MN_MSG_EVT_SUBMIT_RPT*/           At_SendSmRspProc,
    /*MN_MSG_EVT_MSG_SENT*/             At_SetCnmaRspProc,
    /*MN_MSG_EVT_MSG_STORED*/           At_SmsRspNop,
    /*MN_MSG_EVT_DELIVER*/              At_SmsDeliverProc,
    /*MN_MSG_EVT_DELIVER_ERR*/          At_SmsDeliverErrProc,
    /*MN_MSG_EVT_SM_STORAGE_LIST*/      At_SmsStorageListProc,                  /*区分主动上报和响应消息的处理*/
    /*MN_MSG_EVT_STORAGE_FULL*/         At_SmsRspNop,
    /*MN_MSG_EVT_STORAGE_EXCEED*/       At_SmsStorageExceedProc,
    /*MN_MSG_EVT_READ*/                 At_ReadRspProc,
    /*MN_MSG_EVT_LIST*/                 At_ListRspProc,
    /*MN_MSG_EVT_WRITE*/                At_WriteSmRspProc,
    /*MN_MSG_EVT_DELETE*/               At_DeleteRspProc,
    /*MN_MSG_EVT_DELETE_TEST*/          At_DeleteTestRspProc,
    /*MN_MSG_EVT_MODIFY_STATUS*/        At_SmsModSmStatusRspProc,
    /*MN_MSG_EVT_WRITE_SRV_PARM*/       At_SetCscaCsmpRspProc,
    /*MN_MSG_EVT_READ_SRV_PARM*/        AT_QryCscaRspProc,
    /*MN_MSG_EVT_SRV_PARM_CHANGED*/     At_SmsSrvParmChangeProc,
    /*MN_MSG_EVT_DELETE_SRV_PARM*/      At_SmsRspNop,
    /*MN_MSG_EVT_READ_STARPT*/          At_SmsRspNop,
    /*MN_MSG_EVT_DELETE_STARPT*/        At_SmsRspNop,
    /*MN_MSG_EVT_SET_MEMSTATUS*/        AT_SetMemStatusRspProc,
    /*MN_MSG_EVT_MEMSTATUS_CHANGED*/    At_SmsRspNop,
    /*MN_MSG_EVT_MATCH_MO_STARPT_INFO*/ At_SmsRspNop,
    /*MN_MSG_EVT_SET_RCVMSG_PATH*/      At_SetRcvPathRspProc,
    /*MN_MSG_EVT_GET_RCVMSG_PATH*/      At_SmsRspNop,
    /*MN_MSG_EVT_RCVMSG_PATH_CHANGED*/  At_SmsRcvMsgPathChangeProc,
    /*MN_MSG_EVT_INIT_SMSP_RESULT*/     At_SmsInitSmspResultProc,
    /*MN_MSG_EVT_INIT_RESULT*/          At_SmsInitResultProc,
    /*MN_MSG_EVT_SET_LINK_CTRL_PARAM*/  At_SetCmmsRspProc,
    /*MN_MSG_EVT_GET_LINK_CTRL_PARAM*/  At_GetCmmsRspProc,
    /*MN_MSG_EVT_STUB_RESULT*/          At_SmsStubRspProc,
#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))
    /*MN_MSG_EVT_DELIVER_CBM*/          At_SmsDeliverCbmProc,
    /*MN_MSG_EVT_GET_CBTYPE*/           At_GetCbActiveMidsRspProc,
    /*MN_MSG_EVT_ADD_CBMIDS*/           AT_ChangeCbMidsRsp,
    /*MN_MSG_EVT_DELETE_CBMIDS*/        AT_ChangeCbMidsRsp,
    /*MN_MSG_EVT_DELETE_ALL_CBMIDS*/    AT_ChangeCbMidsRsp,

#if (FEATURE_ON == FEATURE_ETWS)
    /*MN_MSG_EVT_DELIVER_ETWS_PRIM_NOTIFY*/  At_ProcDeliverEtwsPrimNotify,
#else
    /*MN_MSG_EVT_DELIVER_ETWS_PRIM_NOTIFY*/  At_SmsRspNop,
#endif

#else
    /*MN_MSG_EVT_DELIVER_CBM*/          At_SmsRspNop,
    /*MN_MSG_EVT_GET_CBTYPE*/           At_SmsRspNop,
    /*MN_MSG_EVT_ADD_CBMIDS*/           At_SmsRspNop,
    /*MN_MSG_EVT_DELETE_CBMIDS*/        At_SmsRspNop,
    /*MN_MSG_EVT_DELETE_ALL_CBMIDS*/    At_SmsRspNop
    /*MN_MSG_EVT_DELIVER_ETWS_PRIM_NOTIFY*/   At_SmsRspNop,
#endif
};

/*结构的最后一项不做处理，仅表示结尾*/
AT_QUERY_TYPE_FUNC_STRU     g_aAtQryTypeProcFuncTbl[] =
{
    {TAF_PH_NETWORKNAME_PARA,          At_QryParaRspCopsProc},
    {TAF_PH_MFR_ID_PARA,               At_QryParaRspCgmiProc},
    {TAF_PH_MODEL_ID_PARA,             At_QryParaRspCgmmProc},
    {TAF_PH_REVISION_ID_PARA,          At_QryParaRspCgmrProc},
    {TAF_PH_QUICK_START_PARA,          At_QryParaRspCqstProc},
    {TAF_PH_AUTO_ATTACH_PARA,          At_QryParaRspCaattProc},
    {TAF_PH_SYSINFO_VALUE_PARA,        At_QryParaRspSysinfoProc},
    {TAF_PH_IMSI_ID_PARA,              At_QryParaRspCimiProc},
    {TAF_PH_MS_CLASS_PARA,             At_QryParaRspCgclassProc},
    {TAF_PH_CS_REG_STATE_PARA,         At_QryParaRspCregProc},
    {TAF_PH_PS_REG_STATE_PARA,         At_QryParaRspCgregProc},
#if(FEATURE_ON == FEATURE_LTE)

    {TAF_PH_EPS_REG_STATE_PARA,        AT_QryParaRspCeregProc},


    /* 删除At_QryParaRspCellRoamProc */
#endif
    {TAF_PH_MODE_GET_PARA,             At_QryParaRspCfunProc},
    {TAF_PH_ACCESS_MODE_PARA,          At_QryParaRspCpamProc},
    {TAF_PH_RSSI_VALUE_PARA,           At_QryParaRspCsqProc},
    {TAF_PH_BATTERY_POWER_PARA,        At_QryParaRspCbcProc},
    {TAF_PH_ICC_ID,                    At_QryParaRspIccidProc},
    {TAF_PH_PNN_PARA,                  At_QryParaRspPnnProc},
    {TAF_PH_CPNN_PARA,                 At_QryParaRspCPnnProc},
    {TAF_PH_OPL_PARA,                  At_QryParaRspOplProc},
    {TAF_PH_FPLMN_PARA,                At_QryParaRspCfplmnProc},
    {TAF_PH_HS_PARA,                   At_QryParaRspHsProc},
    {TAF_PH_PNN_RANGE_PARA,            At_QryRspUsimRangeProc},
    {TAF_PH_OPL_RANGE_PARA,            At_QryRspUsimRangeProc},
    {TAF_PH_CSQLVLEXT_VALUE_PARA,      AT_QryParaRspCsqlvlExtProc},
    {TAF_PH_CSQLVL_VALUE_PARA,         AT_QryParaRspCsqlvlProc},
    /* begin V7R1 PhaseI Modify */
    {TAF_PH_SYSINFO_EX_VALUE_PARA,     AT_QryParaRspSysinfoExProc},
    /* end V7R1 PhaseI Modify */

    {TAF_PH_REGISTER_TIME_VALUE_PARA,  AT_QryParaRegisterTimeProc},
    {TAF_PH_ANQUERY_VALUE_PARA,        AT_QryParaAnQueryProc},
    {TAF_PH_HOMEPLMN_VALUE_PARA,       AT_QryParaHomePlmnProc},
    {TAF_PH_CSNR_VALUE_PARA,           AT_QryParaCsnrProc},
    {TAF_PH_SPN_VALUE_PARA,            AT_QryParaSpnProc},


    {TAF_MM_PLMN_INFO_QRY_PARA,        At_QryMmPlmnInfoRspProc},

    {TAF_PH_PLMN_QRY_PARA,             At_RcvPlmnQryCnf},

    {TAF_PH_USER_SRV_STATE_QRY_PARA,   AT_RcvUserSrvStateQryCnf},

    {TAF_TELE_PARA_BUTT,               TAF_NULL_PTR}
};

#if (VOS_WIN32 == VOS_OS_VER)
LOCAL TAF_UINT8                         gaucAtStin[] = "^STIN:";
LOCAL TAF_UINT8                         gaucAtStmn[] = "^STMN:";
LOCAL TAF_UINT8                         gaucAtStgi[] = "^STGI:";
LOCAL TAF_UINT8                         gaucAtStsf[] = "^STSF:";
LOCAL TAF_UINT8                         gaucAtCsin[] = "^CSIN:";
LOCAL TAF_UINT8                         gaucAtCstr[] = "^CSTR:";
LOCAL TAF_UINT8                         gaucAtCsen[] = "^CSEN:";
LOCAL TAF_UINT8                         gaucAtCsmn[] = "^CSMN:";
LOCAL TAF_UINT8                         gaucAtCcin[] = "^CCIN:";
#else
TAF_UINT8                               gaucAtStin[] = "^STIN:";
TAF_UINT8                               gaucAtStmn[] = "^STMN:";
TAF_UINT8                               gaucAtStgi[] = "^STGI:";
TAF_UINT8                               gaucAtStsf[] = "^STSF:";
TAF_UINT8                               gaucAtCsin[] = "^CSIN:";
TAF_UINT8                               gaucAtCstr[] = "^CSTR:";
TAF_UINT8                               gaucAtCsen[] = "^CSEN:";
TAF_UINT8                               gaucAtCsmn[] = "^CSMN:";
TAF_UINT8                               gaucAtCcin[] = "^CCIN:";
#endif

static AT_CALL_CUUSU_MSG_STRU g_stCuusuMsgType[] =
{
    {MN_CALL_UUS1_MSG_SETUP             ,   AT_CUUSU_MSG_SETUP              },
    {MN_CALL_UUS1_MSG_DISCONNECT        ,   AT_CUUSU_MSG_DISCONNECT         },
    {MN_CALL_UUS1_MSG_RELEASE_COMPLETE  ,   AT_CUUSU_MSG_RELEASE_COMPLETE   }
};

static AT_CALL_CUUSI_MSG_STRU g_stCuusiMsgType[] =
{
    {MN_CALL_UUS1_MSG_ALERT             ,   AT_CUUSI_MSG_ALERT              },
    {MN_CALL_UUS1_MSG_PROGRESS          ,   AT_CUUSI_MSG_PROGRESS           },
    {MN_CALL_UUS1_MSG_CONNECT           ,   AT_CUUSI_MSG_CONNECT            },
    {MN_CALL_UUS1_MSG_RELEASE           ,   AT_CUUSI_MSG_RELEASE            }
};

/* begin V7R1 PhaseI Modify */
static AT_PH_SYS_MODE_TBL_STRU g_astSysModeTbl[] =
{
    {MN_PH_SYS_MODE_EX_NONE_RAT     ,"NO SERVICE"},
    {MN_PH_SYS_MODE_EX_GSM_RAT      ,"GSM"},
    {MN_PH_SYS_MODE_EX_CDMA_RAT     ,"CDMA"},
    {MN_PH_SYS_MODE_EX_WCDMA_RAT    ,"WCDMA"},
    {MN_PH_SYS_MODE_EX_TDCDMA_RAT   ,"TD-SCDMA"},
    {MN_PH_SYS_MODE_EX_WIMAX_RAT    ,"WIMAX"},
    {MN_PH_SYS_MODE_EX_LTE_RAT      ,"LTE"}
};

AT_PH_SUB_SYS_MODE_TBL_STRU g_astSubSysModeTbl[] =
{
    {MN_PH_SUB_SYS_MODE_EX_NONE_RAT         ,"NO SERVICE"},
    {MN_PH_SUB_SYS_MODE_EX_GSM_RAT          ,"GSM"},
    {MN_PH_SUB_SYS_MODE_EX_GPRS_RAT         ,"GPRS"},
    {MN_PH_SUB_SYS_MODE_EX_EDGE_RAT         ,"EDGE"},
    {MN_PH_SUB_SYS_MODE_EX_WCDMA_RAT        ,"WCDMA"},
    {MN_PH_SUB_SYS_MODE_EX_HSDPA_RAT        ,"HSDPA"},
    {MN_PH_SUB_SYS_MODE_EX_HSUPA_RAT        ,"HSUPA"},
    {MN_PH_SUB_SYS_MODE_EX_HSPA_RAT         ,"HSPA"},
    {MN_PH_SUB_SYS_MODE_EX_HSPA_PLUS_RAT    ,"HSPA+"},
    {MN_PH_SUB_SYS_MODE_EX_DCHSPA_PLUS_RAT  ,"DC-HSPA+"},
    {MN_PH_SUB_SYS_MODE_EX_TDCDMA_RAT       ,"TD-SCDMA"},
    {MN_PH_SUB_SYS_MODE_EX_TD_HSDPA_RAT     ,"HSDPA"},
    {MN_PH_SUB_SYS_MODE_EX_TD_HSUPA_RAT     ,"HSUPA"},
    {MN_PH_SUB_SYS_MODE_EX_TD_HSPA_RAT      ,"HSPA"},
    {MN_PH_SUB_SYS_MODE_EX_TD_HSPA_PLUS_RAT ,"HSPA+"},

    {MN_PH_SUB_SYS_MODE_EX_LTE_RAT          ,"LTE"}
};
/* end V7R1 PhaseI Modify */

#if(FEATURE_ON == FEATURE_LTE)
VOS_UINT32  g_ulGuTmodeCnf  = 0;
VOS_UINT32  g_ulLteTmodeCnf = 0;
 #endif

/* +CLCK命令参数CLASS与Service Type Code对应扩展表 */
AT_CLCK_CLASS_SERVICE_TBL_STRU          g_astClckClassServiceExtTbl[] = {
    {AT_CLCK_PARA_CLASS_VOICE,                      TAF_SS_TELE_SERVICE,        TAF_ALL_SPEECH_TRANSMISSION_SERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE,                      TAF_SS_TELE_SERVICE,        TAF_TELEPHONY_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE,                      TAF_SS_TELE_SERVICE,        TAF_EMERGENCY_CALLS_TSCODE},
    {AT_CLCK_PARA_CLASS_DATA,                       TAF_SS_BEARER_SERVICE,      TAF_ALL_BEARERSERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA,                       TAF_SS_TELE_SERVICE,        TAF_ALL_DATA_TELESERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_FAX,                        TAF_SS_TELE_SERVICE,        TAF_ALL_FACSIMILE_TRANSMISSION_SERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_FAX,                        TAF_SS_TELE_SERVICE,        TAF_FACSIMILE_GROUP3_AND_ALTER_SPEECH_TSCODE},
    {AT_CLCK_PARA_CLASS_FAX,                        TAF_SS_TELE_SERVICE,        TAF_AUTOMATIC_FACSIMILE_GROUP3_TSCODE},
    {AT_CLCK_PARA_CLASS_FAX,                        TAF_SS_TELE_SERVICE,        TAF_FACSIMILE_GROUP4_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE_DATA_FAX,             TAF_SS_TELE_SERVICE,        TAF_ALL_TELESERVICES_EXEPTSMS_TSCODE},
    {AT_CLCK_PARA_CLASS_SMS,                        TAF_SS_TELE_SERVICE,        TAF_ALL_SMS_SERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_SMS,                        TAF_SS_TELE_SERVICE,        TAF_SMS_MT_PP_TSCODE},
    {AT_CLCK_PARA_CLASS_SMS,                        TAF_SS_TELE_SERVICE,        TAF_SMS_MO_PP_TSCODE},
    {AT_CLCK_PARA_CLASS_VOICE_DATA_FAX_SMS,         TAF_SS_TELE_SERVICE,        TAF_ALL_TELESERVICES_TSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_ALL_DATA_CIRCUIT_SYNCHRONOUS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_ALL_DATACDS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_DATACDS_1200BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_DATACDS_2400BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_DATACDS_4800BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_DATACDS_9600BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_ALL_ALTERNATE_SPEECH_DATACDS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC,                  TAF_SS_BEARER_SERVICE,      TAF_ALL_SPEECH_FOLLOWED_BY_DATACDS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_ALL_DATA_CIRCUIT_ASYNCHRONOUS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_ALL_DATACDA_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_DATACDA_300BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_DATACDA_1200BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_DATACDA_1200_75BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_DATACDA_2400BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_DATACDA_4800BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_DATACDA_9600BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_ALL_ALTERNATE_SPEECH_DATACDA_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC,                 TAF_SS_BEARER_SERVICE,      TAF_ALL_SPEECH_FOLLOWED_BY_DATACDA_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PKT,                   TAF_SS_BEARER_SERVICE,      TAF_ALL_DATAPDS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PKT,                   TAF_SS_BEARER_SERVICE,      TAF_DATAPDS_2400BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PKT,                   TAF_SS_BEARER_SERVICE,      TAF_DATAPDS_4800BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PKT,                   TAF_SS_BEARER_SERVICE,      TAF_DATAPDS_9600BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC_PKT,              TAF_SS_BEARER_SERVICE,      TAF_ALL_SYNCHRONOUS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_ALL_PADACCESSCA_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_PADACCESSCA_300BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_PADACCESSCA_1200BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_PADACCESSCA_1200_75BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_PADACCESSCA_2400BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_PADACCESSCA_4800BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_PAD,                   TAF_SS_BEARER_SERVICE,      TAF_PADACCESSCA_9600BPS_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_ASYNC_PAD,             TAF_SS_BEARER_SERVICE,      TAF_ALL_ASYNCHRONOUS_SERVICES_BSCODE},
    {AT_CLCK_PARA_CLASS_DATA_SYNC_ASYNC_PKT_PKT,    TAF_SS_BEARER_SERVICE,      TAF_ALL_BEARERSERVICES_BSCODE},
};

/*****************************************************************************
   5 函数、变量声明
*****************************************************************************/

extern TAF_UINT8                               gucSTKCmdQualify ;

#if(FEATURE_ON == FEATURE_LTE)
extern   VOS_UINT32 g_ulGuOnly;
extern   VOS_UINT32 g_ulGuTmodeCnfNum;
extern   VOS_UINT32 g_ulLteIsSend2Dsp;
extern   VOS_UINT32 g_ulTmodeLteMode;
 #endif

/*****************************************************************************
   6 函数实现
*****************************************************************************/


VOS_UINT32 AT_CheckRptCmdStatus(
    VOS_UINT8                          *pucRptCfg,
    AT_CMD_RPT_CTRL_TYPE_ENUM_UINT8     enRptCtrlType,
    AT_RPT_CMD_INDEX_ENUM_UINT8         enRptCmdIndex
)
{
    AT_RPT_CMD_INDEX_ENUM_UINT8        *pulRptCmdTblPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulRptCmdTblSize;
    VOS_UINT8                           ucTableIndex;
    VOS_UINT32                          ulOffset;
    VOS_UINT8                           ucBit;

    /* 主动上报命令索引错误，默认主动上报 */
    if (enRptCmdIndex >= AT_RPT_CMD_BUTT)
    {
        return VOS_TRUE;
    }

    /* 主动上报受控类型填写错误，默认主动上报 */
    if (AT_CMD_RPT_CTRL_BUTT == enRptCtrlType)
    {
        return VOS_TRUE;
    }

    if (AT_CMD_RPT_CTRL_BY_CURC == enRptCtrlType)
    {
        pulRptCmdTblPtr = AT_GET_CURC_RPT_CTRL_STATUS_MAP_TBL_PTR();
        ulRptCmdTblSize = AT_GET_CURC_RPT_CTRL_STATUS_MAP_TBL_SIZE();
    }
    else
    {
        pulRptCmdTblPtr = AT_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_PTR();
        ulRptCmdTblSize = AT_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_SIZE();
    }

    for (ucTableIndex = 0; ucTableIndex < ulRptCmdTblSize; ucTableIndex++)
    {
        if (enRptCmdIndex == pulRptCmdTblPtr[ucTableIndex])
        {
            break;
        }
    }

    /* 与全局变量中的Bit位对比 */
    if (ulRptCmdTblSize != ucTableIndex)
    {
        /* 由于用户设置的字节序与Bit映射表序相反, 首先反转Bit位 */
        ulOffset        = AT_CURC_RPT_CFG_MAX_SIZE - (ucTableIndex / 8) - 1;
        ucBit           = (VOS_UINT8)(ucTableIndex % 8);

        return (VOS_UINT32)((pucRptCfg[ulOffset] >> ucBit) & 0x1);
    }

    return VOS_TRUE;
}


VOS_UINT32 At_ChgMnErrCodeToAt(
    VOS_UINT8                           ucIndex,
    VOS_UINT32                          ulMnErrorCode
)
{
    VOS_UINT32                          ulRtn;
    AT_CMS_SMS_ERR_CODE_MAP_STRU       *pstSmsErrMapTblPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulSmsErrMapTblSize;
    VOS_UINT32                          ulCnt;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    pstSmsErrMapTblPtr = AT_GET_CMS_SMS_ERR_CODE_MAP_TBL_PTR();
    ulSmsErrMapTblSize = AT_GET_CMS_SMS_ERR_CODE_MAP_TBL_SIZE();

    ulRtn = AT_CMS_UNKNOWN_ERROR;

    for (ulCnt = 0; ulCnt < ulSmsErrMapTblSize; ulCnt++)
    {
        if (pstSmsErrMapTblPtr[ulCnt].ulSmsCause == ulMnErrorCode)
        {
            ulRtn =  pstSmsErrMapTblPtr[ulCnt].ulCmsCode;

            if ((AT_CMGF_MSG_FORMAT_TEXT == pstSmsCtx->enCmgfMsgFormat)
             && (AT_CMS_INVALID_PDU_MODE_PARAMETER == ulRtn))
            {
                ulRtn = AT_CMS_INVALID_TEXT_MODE_PARAMETER;
            }

            break;
        }
    }

    return ulRtn;
}


TAF_UINT32 At_ChgTafErrorCode(TAF_UINT8 ucIndex, TAF_ERROR_CODE_ENUM_UINT32 enTafErrorCode)
{
    TAF_UINT32 ulRtn = 0;

    switch(enTafErrorCode)
    {
    case TAF_ERR_GET_CSQLVL_FAIL:
    case TAF_ERR_USIM_SVR_OPLMN_LIST_INAVAILABLE:
        ulRtn = AT_ERROR;
        break;

    case TAF_ERR_TIME_OUT:                  /* 超时错误 */
        ulRtn = AT_CME_NETWORK_TIMEOUT;
        break;

    case TAF_ERR_USIM_SIM_CARD_NOTEXIST:    /* SIM卡不存在 */
        ulRtn = AT_CME_SIM_NOT_INSERTED;
        break;
    case TAF_ERR_NEED_PIN1:                 /* 需要PIN码 */
        ulRtn = AT_CME_SIM_PIN_REQUIRED;
        break;

    case TAF_ERR_NEED_PUK1:                 /* 需要PUK码 */
        ulRtn = AT_CME_SIM_PUK_REQUIRED;
        break;

    case TAF_ERR_SIM_FAIL:
    case TAF_ERR_PB_STORAGE_OP_FAIL:
        ulRtn = AT_CME_SIM_FAILURE;
        break;

    case TAF_ERR_UNSPECIFIED_ERROR:         /* 未知错误 */
        ulRtn = AT_CME_UNKNOWN;
        break;

    case TAF_ERR_PARA_ERROR:                /* 参数错误 */
        ulRtn = AT_CME_INCORRECT_PARAMETERS;
        break;

    case TAF_ERR_SS_NEGATIVE_PASSWORD_CHECK:
        ulRtn = AT_CME_INCORRECT_PASSWORD;
        break;

    case TAF_ERR_SIM_BUSY:
        ulRtn = AT_CME_SIM_BUSY;
        break;
    case TAF_ERR_SIM_LOCK:
        ulRtn = AT_CME_PH_SIM_PIN_REQUIRED;
        break;
    case TAF_ERR_SIM_INCORRECT_PASSWORD:
        ulRtn = AT_CME_INCORRECT_PASSWORD;
        break;
    case TAF_ERR_PB_NOT_FOUND:
        ulRtn = AT_CME_NOT_FOUND;
        break;
    case TAF_ERR_PB_DIAL_STRING_TOO_LONG:
        ulRtn = AT_CME_DIAL_STRING_TOO_LONG;
        break;
    case TAF_ERR_PB_STORAGE_FULL:
        ulRtn = AT_CME_MEMORY_FULL;
        break;
    case TAF_ERR_PB_WRONG_INDEX:
        ulRtn = AT_CME_INVALID_INDEX;
        break;
    case TAF_ERR_CMD_TYPE_ERROR:
        ulRtn = AT_CME_OPERATION_NOT_ALLOWED;
        break;

    case TAF_ERR_FILE_NOT_EXIST:
        ulRtn = AT_CME_FILE_NOT_EXISTS;
        break;

    case TAF_ERR_NO_NETWORK_SERVICE:
        ulRtn = AT_CME_NO_NETWORK_SERVICE;
        break;
    case TAF_ERR_AT_ERROR:
        ulRtn = AT_ERROR;
        break;
    case TAF_ERR_CME_OPT_NOT_SUPPORTED:
        ulRtn = AT_CME_OPERATION_NOT_SUPPORTED;
        break;

    case TAF_ERR_NET_SEL_MENU_DISABLE:
        ulRtn = AT_CME_NET_SEL_MENU_DISABLE;
        break;

    case TAF_ERR_SYSCFG_CS_SERV_EXIST:
        ulRtn = AT_CME_CS_SERV_EXIST;
        break;
    case TAF_ERR_NEED_PUK2:
        ulRtn = AT_CME_SIM_PUK2_REQUIRED;
        break;
    case TAF_ERR_BUSY_ON_USSD:
    case TAF_ERR_BUSY_ON_SS:
        ulRtn = AT_CME_OPERATION_NOT_SUPPORTED;
        break;
    case TAF_ERR_SS_NET_TIMEOUT:
        ulRtn = AT_CME_NETWORK_TIMEOUT;
        break;
    case TAF_ERR_NO_SUCH_ELEMENT:
        ulRtn = AT_CME_NO_SUCH_ELEMENT;
        break;
    case TAF_ERR_MISSING_RESOURCE:
        ulRtn = AT_CME_MISSING_RESOURCE;
        break;
    /* Added by zwx247453 for VOLTE SWITCH, 2015-02-02, Begin */
    case TAF_ERR_IMS_NOT_SUPPORT:
        ulRtn = AT_CME_IMS_NOT_SUPPORT;
        break;
    case TAF_ERR_IMS_SERVICE_EXIST:
        ulRtn = AT_CME_IMS_SERVICE_EXIST;
        break;
    case TAF_ERR_IMS_VOICE_DOMAIN_PS_ONLY:
        ulRtn = AT_CME_IMS_VOICE_DOMAIN_PS_ONLY;
        break;
    case TAF_ERR_IMS_STACK_TIMEOUT:
        ulRtn = AT_CME_IMS_STACK_TIMEOUT;
        break;
    /* Added by zwx247453 for VOLTE SWITCH, 2015-02-02, end */

    default:
        if (VOS_NULL_PTR == g_stParseContext[ucIndex].pstCmdElement)
        {
            ulRtn = AT_CME_UNKNOWN;
        }
        else if(g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex > AT_CMD_SMS_BEGAIN)
        {
            ulRtn = AT_CMS_UNKNOWN_ERROR;
        }
        else
        {
            ulRtn = AT_CME_UNKNOWN;
        }
        break;
    }

    return ulRtn;
}


TAF_UINT32 At_SsClass2Print(TAF_UINT8 ucClass)
{
    TAF_UINT32 ulRtn = 0;

    switch(ucClass)
    {
    case TAF_ALL_SPEECH_TRANSMISSION_SERVICES_TSCODE:
        ulRtn = 1;
        break;

    case TAF_ALL_DATA_TELESERVICES_TSCODE:
        ulRtn = 2;
        break;

    case TAF_ALL_FACSIMILE_TRANSMISSION_SERVICES_TSCODE:
        ulRtn = 4;
        break;

    case TAF_ALL_SMS_SERVICES_TSCODE:
        ulRtn = 8;
        break;

    case TAF_ALL_DATA_CIRCUIT_SYNCHRONOUS_BSCODE:
    case TAF_ALL_DATACDS_SERVICES_BSCODE:
        ulRtn = 16;
        break;

    case TAF_ALL_DATA_CIRCUIT_ASYNCHRONOUS_BSCODE:
        ulRtn = 32;
        break;

    default:
        break;
    }

    return ulRtn;
}
/*****************************************************************************
 Prototype      : At_CcClass2Print
 Description    : 把CCA返回的CLASS以字符串方式输出，注意，不完整
 Input          : ucClass --- CCA的CLASS
 Output         : ---
 Return Value   : ulRtn输出结果
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_UINT32 At_CcClass2Print(MN_CALL_TYPE_ENUM_U8 enCallType,TAF_UINT8 *pDst)
{
    TAF_UINT16 usLength = 0;

    switch(enCallType)
    {
    case MN_CALL_TYPE_VOICE:
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst,"VOICE");
        break;

    case MN_CALL_TYPE_FAX:
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst,"FAX");
        break;

    case MN_CALL_TYPE_VIDEO:
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst,"SYNC");
        break;

    case MN_CALL_TYPE_CS_DATA:
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst,"DATA");
        break;

    default:
        AT_WARN_LOG("At_CcClass2Print CallType ERROR");
        break;
    }

    return usLength;
}

/* PC工程中AT从A核移到C核, At_sprintf有重复定义,故在此处添加条件编译宏 */
/*lint -e960 Note -- Violates MISRA 2004 Required Rule 12.5, non-primary expression used with logical operator*/
#if  !defined(__PS_WIN32_RECUR__) && !defined(DMT) && !defined(NAS_STUB)
/*lint +e960*/

TAF_UINT32 At_HexAlpha2AsciiString(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst,TAF_UINT8 *pucSrc,TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usLen = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8 *pWrite = pucDst;
    TAF_UINT8 *pRead = pucSrc;
    TAF_UINT8  ucHigh = 0;
    TAF_UINT8  ucLow = 0;

    if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + (2 * usSrcLen)) >= MaxLength)
    {
        AT_ERR_LOG("At_HexAlpha2AsciiString too long");
        return 0;
    }

    if(0 != usSrcLen)
    {
        /* 扫完整个字串 */
        while( usChkLen++ < usSrcLen )
        {
            ucHigh = 0x0F & (*pRead >> 4);
            ucLow = 0x0F & *pRead;

            usLen += 2;    /* 记录长度 */

            if(0x09 >= ucHigh)   /* 0-9 */
            {
                *pWrite++ = ucHigh + 0x30;
            }
            else if(0x0A <= ucHigh)    /* A-F */
            {
                *pWrite++ = ucHigh + 0x37;
            }
            else
            {

            }

            if(0x09 >= ucLow)   /* 0-9 */
            {
                *pWrite++ = ucLow + 0x30;
            }
            else if(0x0A <= ucLow)    /* A-F */
            {
                *pWrite++ = ucLow + 0x37;
            }
            else
            {

            }

            /* 下一个字符 */
            pRead++;
        }

    }
    return usLen;
}
#endif
TAF_UINT16 At_UnicodeFormatPrint(const TAF_UINT8 *pSrc, TAF_UINT8 *pDest, TAF_UINT32 Dcs)
{
    TAF_UINT8 i, j;
    TAF_UINT16 len;
    TAF_UINT16 base, tmp;

    base = 0;

    if(SI_PB_ALPHATAG_TYPE_UCS2_81 == Dcs)                     /* name decode by 81 */
    {
        base = (TAF_UINT16)((pSrc[1]<<0x07)&0x7F80);           /* get the basepoint value */

        j = 0x02;                                              /* name content offset */
    }
    else if(SI_PB_ALPHATAG_TYPE_UCS2_82 == Dcs)                /* name decode by 82 */
    {
        base = (TAF_UINT16)((pSrc[1]<<0x08)&0xFF00) + pSrc[2];  /* get the basepoint value */

        j = 0x03;                                               /* name content offset */
    }
    else                                                        /* name decode error */
    {
        return 0;
    }

    len = 2 * pSrc[0];                                          /* get the length of name */

    if((len == 0x00)||(len == 0xFF))                            /* length error */
    {
        return 0;
    }

    for( i = 0; i < pSrc[0] ; i++)                              /* decode action begin */
    {
        if((pSrc[j+i]&0x80) == 0x00)
        {
            pDest[i*2] = 0x00;

            pDest[(i*2)+1] = pSrc[j+i];
        }
        else
        {
            tmp = base + (pSrc[j+i]&0x7F);

            pDest[i*2] = (TAF_UINT8)((tmp&0xFF00)>>0x08);

            pDest[(i*2)+1] = (TAF_UINT8)(tmp&0x00FF);
        }
    }

    return len;                                                    /* return the Byte number of the name */
}
VOS_UINT32 AT_Hex2AsciiStrLowHalfFirst(
    VOS_UINT32                          ulMaxLength,
    VOS_INT8                            *pcHeadaddr,
    VOS_UINT8                           *pucDst,
    VOS_UINT8                           *pucSrc,
    VOS_UINT16                          usSrcLen
)
{
    VOS_UINT16                          usLen;
    VOS_UINT16                          usChkLen;
    VOS_UINT8                           *pcWrite;
    VOS_UINT8                           *pcRead;
    VOS_UINT8                           ucHigh;
    VOS_UINT8                           ucLow;

    usLen           = 0;
    usChkLen        = 0;
    pcWrite         = pucDst;
    pcRead          = pucSrc;


    if (((VOS_UINT32)(pucDst - (VOS_UINT8 *)pcHeadaddr) + (2 * usSrcLen)) >= ulMaxLength)
    {
        AT_ERR_LOG("AT_Hex2AsciiStrLowHalfFirst too long");
        return 0;
    }

    if (0 != usSrcLen)
    {
        /* 扫完整个字串 */
        while ( usChkLen++ < usSrcLen )
        {
            ucHigh = 0x0F & (*pcRead >> 4);
            ucLow  = 0x0F & *pcRead;

            usLen += 2;    /* 记录长度 */

            /* 先转换低半字节 */
            if (0x09 >= ucLow)   /* 0-9 */
            {
                *pcWrite++ = ucLow + 0x30;
            }
            else if (0x0A <= ucLow)    /* A-F */
            {
                *pcWrite++ = ucLow + 0x37;
            }
            else
            {

            }

            /* 再转换高半字节 */
            if (0x09 >= ucHigh)   /* 0-9 */
            {
                *pcWrite++ = ucHigh + 0x30;
            }
            else if (0x0A <= ucHigh)    /* A-F */
            {
                *pcWrite++ = ucHigh + 0x37;
            }
            else
            {

            }

            /* 下一个字符 */
            pcRead++;
        }

    }

    return usLen;
}


/*****************************************************************************
 Prototype      : At_ReadNumTypePara
 Description    : 读取ASCII类型的号码
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
TAF_UINT32 At_ReadNumTypePara(TAF_UINT8 *pucDst,TAF_UINT8 *pucSrc)
{
    TAF_UINT16 usLength = 0;

    if(AT_CSCS_UCS2_CODE == gucAtCscsType)       /* +CSCS:UCS2 */
    {
        TAF_UINT16 usSrcLen = (TAF_UINT16)VOS_StrLen((TAF_CHAR *)pucSrc);

        usLength += (TAF_UINT16)At_Ascii2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pucDst + usLength,pucSrc,usSrcLen);
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pucDst + usLength,"%s",pucSrc);
    }
    return usLength;
}
#if( FEATURE_ON == FEATURE_CSD )
VOS_UINT32 AT_SendCsdCallStateInd(
    VOS_UINT8                           ucIndex,
    AT_CSD_CALL_TYPE_STATE_ENUM_UINT16  enCallState
)
{
    AT_CSD_CALL_STATE_TYPE_IND_STRU     *pstMsg;

    /* 申请AT_CSD_CALL_STATE_TYPE_IND_STRU消息 */
    pstMsg = (AT_CSD_CALL_STATE_TYPE_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                            WUEPS_PID_AT,
                            sizeof(AT_CSD_CALL_STATE_TYPE_IND_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("AT_SendCsdCallStateInd: alloc msg fail!");
        return VOS_ERR;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR*)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               sizeof(AT_CSD_CALL_STATE_TYPE_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = ACPU_PID_CSD;
    pstMsg->enMsgId         = ID_AT_CSD_CALL_STATE_IND;

    /* 填写消息体 */
    pstMsg->enCallState     = enCallState;
    pstMsg->ucIndex         = ucIndex;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("AT_SendCsdCallStateInd: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
#endif

VOS_BOOL  AT_IsFindVedioModemStatus(
    VOS_UINT8                           ucIndex,
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    AT_DCE_MSC_STRU                     stMscStru;
    TAF_UINT32                          ulDelayaCnt;

#if (FEATURE_ON == FEATURE_AT_HSIC)
    if ((AT_CLIENT_TAB_MODEM_INDEX != ucIndex)
     && (AT_CLIENT_TAB_HSIC_MODEM_INDEX != ucIndex))
#else
    if (AT_CLIENT_TAB_MODEM_INDEX != ucIndex)
#endif
    {
        return VOS_FALSE;
    }

    /* 在AT_CSD_DATA_MODE模式下，返回命令模式，DCD信号拉低，此时可以再次处理PC侧来的AT命令 */
    if (( AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
     && (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
     && (AT_CSD_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
     && (MN_CALL_TYPE_VIDEO == enCallType))
    {

        /* 返回命令模式 */
        At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);

#if( FEATURE_ON == FEATURE_CSD )
        /* 去注册Modem端口VIDEO PHONE流控点 */
        AT_DeRegModemVideoPhoneFCPoint(ucIndex);

        AT_SendCsdCallStateInd(ucIndex, AT_CSD_CALL_STATE_OFF);
#endif

        PS_MEM_SET(&stMscStru, 0, sizeof(stMscStru));

        /* 拉低DCD信号 */
        stMscStru.OP_Dcd = 1;
        stMscStru.ucDcd = 0;
        stMscStru.OP_Dsr = 1;
        stMscStru.ucDsr = 1;
        AT_SetModemStatus(ucIndex, &stMscStru);

        /*EVENT-UE Down DCD*/
        AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DCE_DOWN_DCD,
                         VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);

        /* 为了保证拉高的管教信号在PC侧先处理，增加延时的处理  */
        ulDelayaCnt = 1500000;
        while( ulDelayaCnt-- )
        {
            ;
        }

        /* 拉高DCD信号 */
        stMscStru.OP_Dcd = 1;
        stMscStru.ucDcd = 1;
        stMscStru.OP_Dsr = 1;
        stMscStru.ucDsr = 1;
        AT_SetModemStatus(ucIndex, &stMscStru);

        return VOS_TRUE;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);
#endif

    return VOS_FALSE;
}
VOS_VOID  AT_CsRspEvtReleasedProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    VOS_BOOL                            bRet;
    VOS_UINT32                          ulTimerName;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;


    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucIndex);

    g_ucDtrDownFlag = VOS_FALSE;


#if (FEATURE_ON == FEATURE_AT_HSUART)
    /* 停止RING TE */
    AT_VoiceStopRingTe(pstCallInfo->callId);
#endif

    /* 记录cause值 */
    AT_SetCsCallErrCause(ucIndex, pstCallInfo->enCause);

    if ((AT_CMD_CHUP_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
      ||(AT_CMD_H_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
      ||(AT_CMD_CHLD_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
      ||(AT_CMD_CTFR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        if (VOS_TRUE == pstCcCtx->stS0TimeInfo.bTimerStart)
        {
            ulTimerName = pstCcCtx->stS0TimeInfo.ulTimerName;

            AT_StopRelTimer(ulTimerName, &(pstCcCtx->stS0TimeInfo.s0Timer));
            pstCcCtx->stS0TimeInfo.bTimerStart = VOS_FALSE;
            pstCcCtx->stS0TimeInfo.ulTimerName = 0;
        }

        AT_IsFindVedioModemStatus(ucIndex,pstCallInfo->enCallType);

        AT_ReportCendResult(ucIndex, pstCallInfo);

        return;
    }
    else
    {
        /*
        需要增加来电类型，传真，数据，可视电话，语音呼叫
        */
        if (TAF_CS_CAUSE_SUCCESS != pstCallInfo->enCause) /* 记录cause值 */
        {
            gastAtClientTab[ucIndex].ulCause = pstCallInfo->enCause;
        }

        if (VOS_TRUE == pstCcCtx->stS0TimeInfo.bTimerStart)
        {
            ulTimerName = pstCcCtx->stS0TimeInfo.ulTimerName;

            AT_StopRelTimer(ulTimerName, &(pstCcCtx->stS0TimeInfo.s0Timer));
            pstCcCtx->stS0TimeInfo.bTimerStart = VOS_FALSE;
            pstCcCtx->stS0TimeInfo.ulTimerName = 0;
        }

        /* 上报CEND，CS域的可视电话不需要上报^CEND */
        if ((MN_CALL_TYPE_VOICE == pstCallInfo->enCallType)
         || (MN_CALL_TYPE_EMERGENCY == pstCallInfo->enCallType)
         || (AT_EVT_IS_PS_VIDEO_CALL(pstCallInfo->enCallType, pstCallInfo->enVoiceDomain)))
        {
            AT_ReportCendResult(ucIndex, pstCallInfo);

            return;
        }

        ulResult = AT_NO_CARRIER;

        if (AT_EVT_IS_VIDEO_CALL(pstCallInfo->enCallType))
        {
            if (TAF_CS_CAUSE_CC_NW_USER_ALERTING_NO_ANSWER == pstCallInfo->enCause)
            {
                ulResult = AT_NO_ANSWER;
            }

            if (TAF_CS_CAUSE_CC_NW_USER_BUSY == pstCallInfo->enCause)
            {
                ulResult = AT_BUSY;
            }
        }

        /* AT命令触发的话，需要清除相应的状态变量 */
        if (AT_EVT_REL_IS_NEED_CLR_TIMER_STATUS_CMD(gastAtClientTab[ucIndex].CmdCurrentOpt))
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
        }


        bRet = AT_IsFindVedioModemStatus(ucIndex,pstCallInfo->enCallType);
        if ( VOS_TRUE == bRet )
        {
            return ;
        }
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}



VOS_VOID  AT_CsRspEvtConnectProc(
    VOS_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    TAF_UINT8                           aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1];
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_CsRspEvtConnectProc: Get modem id fail.");
        return;
    }

    pstSsCtx = AT_GetModemSsCtxAddrFromModemId(enModemId);

#if (FEATURE_ON == FEATURE_AT_HSUART)
    /* 停止RING TE */
    AT_VoiceStopRingTe(pstCallInfo->callId);
#endif

    /* CS呼叫成功, 清除CS域错误码 */
    AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_SUCCESS);

    /* 需要判断来电类型，如VOICE或者DATA */
    if(MN_CALL_DIR_MO == pstCallInfo->enCallDir)
    {
        if(AT_COLP_ENABLE_TYPE == pstSsCtx->ucColpType)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s+COLP: ",gaucAtCrLf);
            if(0 != pstCallInfo->stConnectNumber.ucNumLen)
            {
                AT_BcdNumberToAscii(pstCallInfo->stConnectNumber.aucBcdNum,
                                    pstCallInfo->stConnectNumber.ucNumLen,
                                    (VOS_CHAR *)aucAsciiNum);
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"%s\",%d,\"\",,\"\"",aucAsciiNum,(pstCallInfo->stConnectNumber.enNumType | AT_NUMBER_TYPE_EXT));
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"\",,\"\",,\"\"");
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
            usLength = 0;
        }
    }

    /* Video下，通过At_FormatResultData来上报CONNECT */
    if (AT_EVT_IS_VIDEO_CALL(pstCallInfo->enCallType))
    {
        gastAtClientTab[ucIndex].ucCsRabId = pstCallInfo->ucRabId;
        ulResult = AT_CONNECT;

        /* 如果是PCUI口发起的拨打可视电话的操作，则不迁到数据态，只有MODEM口发起的VP操作才需迁到数据态  */
        if (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
        {
#if( FEATURE_ON == FEATURE_CSD )
            /* 注册Modem端口VIDEO PHONE的流控点 */
            AT_RegModemVideoPhoneFCPoint(ucIndex, FC_ID_MODEM);

            AT_SendCsdCallStateInd(ucIndex, AT_CSD_CALL_STATE_ON);
#endif

            At_SetMode(ucIndex, AT_DATA_MODE, AT_CSD_DATA_MODE);   /* 开始数传 */
        }
    }
    else
    {
        if (VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CONN))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"^CONN:%d",pstCallInfo->callId);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",pstCallInfo->enCallType);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
        }
        return;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


VOS_VOID  AT_ProcCsRspEvtOrig(
    TAF_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    TAF_UINT16                          usLength;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_CsRspEvtOrigProc: Get modem id fail.");
        return;
    }

    pstCcCtx = AT_GetModemCcCtxAddrFromModemId(enModemId);

    /* 可视电话里面，这里不能上报^ORIG ，因此只有普通语音和紧急呼叫的情况下，才上报^ORIG */
    if ((VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_ORIG))
     && ((MN_CALL_TYPE_VOICE == pstCallInfo->enCallType)
      || (MN_CALL_TYPE_EMERGENCY == pstCallInfo->enCallType)
      || (AT_EVT_IS_PS_VIDEO_CALL(pstCallInfo->enCallType, pstCallInfo->enVoiceDomain))))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"^ORIG:%d",pstCallInfo->callId);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",pstCallInfo->enCallType);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }

    /* 发起呼叫后收到回复的OK后，将当前是否存在呼叫标志置为TRUE */
    pstCcCtx->ulCurIsExistCallFlag = VOS_TRUE;

    return;
}

VOS_VOID At_TransferNotifyCodeEnum(
    MN_CALL_SS_NOTIFY_CODE_ENUM_U8      enCode,
    VOS_UINT8                          *pucCode
)
{
    switch (enCode)
    {
        case MN_CALL_SS_NTFY_FORWORDED_CALL:
            *pucCode = 0;
            break;

        case MN_CALL_SS_NTFY_MT_CUG_INFO:
            *pucCode = 1;
            break;

        case MN_CALL_SS_NTFY_ON_HOLD:
            *pucCode = 2;
            break;

        case MN_CALL_SS_NTFY_RETRIEVED:
            *pucCode = 3;
            break;

        case MN_CALL_SS_NTFY_ENTER_MPTY:
            *pucCode = 4;
            break;

        case MN_CALL_SS_NTFY_DEFLECTED_CALL:
            *pucCode = 9;
            break;

        default:
            *pucCode = 0xFF;
            break;
    }
}


VOS_VOID At_ProcCsRspEvtCssuNotifiy(
    VOS_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo,
    VOS_UINT16                         *pusLength
)
{
    VOS_UINT8                           ucCode;
    VOS_UINT8                           aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1] = {0};
    ucCode   = 0;

    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
        return;
    }

    /* +CSSU: <code2>[,<index>[,<number>,<type>[,<subaddr>,<satype>]]] */
    if (((VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CSSU))
      && (MN_CALL_SS_NTFY_BE_DEFLECTED < pstCallInfo->stSsNotify.enCode))
     && ((AT_CMD_D_CS_VOICE_CALL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_D_CS_DATA_CALL_SET  != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_APDS_SET            != gastAtClientTab[ucIndex].CmdCurrentOpt)))
    {
        At_TransferNotifyCodeEnum(pstCallInfo->stSsNotify.enCode, &ucCode);

#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s^CSSU: ",
                                                 gaucAtCrLf);
        }
        else
#endif
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s+CSSU: ",
                                                 gaucAtCrLf);
        }

        /* <code2> */
        if (0xFF != ucCode)
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%d", ucCode);
        }

        /* ,<index> */
        if (MN_CALL_SS_NTFY_MT_CUG_INFO == pstCallInfo->stSsNotify.enCode)
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 ",%d", pstCallInfo->stSsNotify.ulCugIndex);
        }

        if (0 != pstCallInfo->stCallNumber.ucNumLen)
        {
            AT_BcdNumberToAscii(pstCallInfo->stCallNumber.aucBcdNum,
                                pstCallInfo->stCallNumber.ucNumLen,
                                (VOS_CHAR *)aucAsciiNum);

            /* ,<number> */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 ",\"%s\"", aucAsciiNum);

            /* ,<type> */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 ",%d", (pstCallInfo->stCallNumber.enNumType | AT_NUMBER_TYPE_EXT));
        }

        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s", gaucAtCrLf);
    }
    return;
}
VOS_VOID At_ProcCsRspEvtCssiNotifiy(
    VOS_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo,
    VOS_UINT16                         *pusLength
)
{

    /* +CSSI: <code1>[,<index>] */
    if ((VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CSSI))
     && (MN_CALL_SS_NTFY_BE_DEFLECTED >= pstCallInfo->stSsNotify.enCode))
    {
#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s^CSSI: ",
                                                 gaucAtCrLf);
        }
        else
#endif
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s+CSSI: ",
                                                 gaucAtCrLf);
        }

        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%d", pstCallInfo->stSsNotify.enCode);

        if (MN_CALL_SS_NTFY_MO_CUG_INFO == pstCallInfo->stSsNotify.enCode)
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 ",%d", pstCallInfo->stSsNotify.ulCugIndex);
        }

        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s", gaucAtCrLf);
    }

    return;
}
VOS_VOID  AT_ProcCsRspEvtSsNotify(
    VOS_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength  = 0;


    At_ProcCsRspEvtCssiNotifiy(ucIndex, pstCallInfo, &usLength);

    At_ProcCsRspEvtCssuNotifiy(ucIndex, pstCallInfo, &usLength);
    if ( (AT_BROADCAST_CLIENT_INDEX_MODEM_0 != ucIndex)
        && (AT_BROADCAST_CLIENT_INDEX_MODEM_1 != ucIndex) )
    {
        enModemId = MODEM_ID_0;

        /* 获取client id对应的Modem Id */
        ulRslt = AT_GetModemIdFromClient(gastAtClientTab[ucIndex].usClientId, &enModemId);

        if (VOS_OK != ulRslt)
        {
            AT_WARN_LOG("AT_ProcCsRspEvtSsNotify: WARNING:MODEM ID NOT FOUND!");
            return;
        }

        /* CCALLSTATE需要广播上报，根据MODEM ID设置广播上报的Index */
        if (MODEM_ID_0 == enModemId)
        {
            ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
        }
        else
        {
            ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_1;
        }
    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}
VOS_VOID  AT_ProcCsRspEvtCallProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    TAF_UINT16                          usLength;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_CsRspEvtCallProcProc: Get modem id fail.");
        return;
    }

    /* CS可视电话里面，这里不能上报^CONF ，因此只有普通语音和紧急呼叫的情况下，才上报^CONF */
    if ((VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CONF))
     && ((MN_CALL_TYPE_VOICE == pstCallInfo->enCallType)
      || (MN_CALL_TYPE_EMERGENCY == pstCallInfo->enCallType)
      || (AT_EVT_IS_PS_VIDEO_CALL(pstCallInfo->enCallType, pstCallInfo->enVoiceDomain))))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"^CONF:%d",pstCallInfo->callId);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }

    return;
}
TAF_VOID At_CsRspProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;

    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucIndex);

    switch( enEvent )            /* 根据事件类型 */
    {



    case MN_CALL_EVT_CONNECT:
        AT_CsRspEvtConnectProc(ucIndex, enEvent, pstCallInfo);
        return;

    case MN_CALL_EVT_ORIG:
        AT_ProcCsRspEvtOrig(ucIndex, pstCallInfo);
        return;

    case MN_CALL_EVT_CALL_PROC:
        AT_ProcCsRspEvtCallProc(ucIndex, pstCallInfo);
        return;


    case MN_CALL_EVT_SS_CMD_RSLT:
        if (AT_CMD_CURRENT_OPT_BUTT == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            return;
        }

        if(MN_CALL_SS_RES_SUCCESS == pstCallInfo->enSsResult)
        {
            ulResult = AT_OK;
        }
        else
        {
            ulResult = AT_ERROR;
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;

    case MN_CALL_EVT_SS_NOTIFY:
    /*case MN_CALL_EVT_ALERTING:alerting 应该不会出现在这个场景下吧*/
        AT_ProcCsRspEvtSsNotify(ucIndex, pstCallInfo);
        return;

    case MN_CALL_EVT_RELEASED:
        AT_CsRspEvtReleasedProc(ucIndex, enEvent, pstCallInfo);
        return;

    case MN_CALL_EVT_UUS1_INFO:
        AT_CsUus1InfoEvtIndProc(ucIndex,enEvent,pstCallInfo);
        break;
    case MN_CALL_EVT_GET_CDUR_CNF:
        AT_RcvCdurQryRsp(ucIndex,enEvent,pstCallInfo);
        return;

    case MN_CALL_EVT_ALL_RELEASED:

        /* 收到所有呼叫都RELEASED后，将当前是否存在呼叫标志置为FALSE */
        pstCcCtx->ulCurIsExistCallFlag = VOS_FALSE;
        break;

    default:
        AT_WARN_LOG("At_CsRspProc CallEvent ERROR");
        return;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}

TAF_UINT8 At_GetSsCode(
    MN_CALL_SS_NOTIFY_CODE_ENUM_U8      enCode,
    MN_CALL_STATE_ENUM_U8               enCallState
)
{
    switch(enCode)
    {
        case MN_CALL_SS_NTFY_FORWORDED_CALL:
            return 0;

        case MN_CALL_SS_NTFY_MT_CUG_INFO:
            return 1;

        case MN_CALL_SS_NTFY_ON_HOLD:
            return 2;

        case MN_CALL_SS_NTFY_RETRIEVED:
            return 3;

        case MN_CALL_SS_NTFY_ENTER_MPTY:
            return 4;

        case MN_CALL_SS_NTFY_DEFLECTED_CALL:
            return 9;

        case MN_CALL_SS_NTFY_EXPLICIT_CALL_TRANSFER:
            if ( MN_CALL_S_ALERTING == enCallState )
            {
                return 7;
            }
            return 8;

        case MN_CALL_SS_NTFY_CCBS_BE_RECALLED:
            return 0x16;

        default:
            return 0xFF;
    }
}

TAF_VOID AT_CsSsNotifyEvtIndProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    TAF_UINT16 usLength;
    TAF_UINT8  ucTmp;
    TAF_UINT8  aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1];

    usLength = 0;
    /* +CSSI: <code1>[,<index>] */
    if((VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CSSI))
     && ((MN_CALL_SS_NTFY_BE_DEFLECTED >= pstCallInfo->stSsNotify.enCode)
     || (MN_CALL_SS_NTFY_CCBS_RECALL == pstCallInfo->stSsNotify.enCode)) )
    {
#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s^CSSI: ",
                                               gaucAtCrLf);
        }
        else
#endif
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s+CSSI: ",
                                               gaucAtCrLf);
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%d",
                                            pstCallInfo->stSsNotify.enCode);

        if( MN_CALL_SS_NTFY_MO_CUG_INFO == pstCallInfo->stSsNotify.enCode )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",%d",
                                                pstCallInfo->stSsNotify.ulCugIndex);
        }

        if ( (MN_CALL_SS_NTFY_CCBS_RECALL == pstCallInfo->stSsNotify.enCode)
          && (MN_CALL_OPTION_EXIST == pstCallInfo->stCcbsFeature.OP_CcbsIndex))
        {
            usLength += (TAF_UINT16)At_CcClass2Print(pstCallInfo->enCallType,
                                                     pgucAtSndCodeAddr + usLength);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s",
                                                gaucAtCrLf);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",%d",
                                                pstCallInfo->stCcbsFeature.CcbsIndex);
            if ( MN_CALL_OPTION_EXIST == pstCallInfo->stCcbsFeature.OP_BSubscriberNum )
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    ",%s",
                                                    pstCallInfo->stCcbsFeature.aucBSubscriberNum);
            }
            if ( MN_CALL_OPTION_EXIST == pstCallInfo->stCcbsFeature.OP_NumType )
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    ",%s",
                                                    pstCallInfo->stCcbsFeature.NumType);
            }
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s",
                                            gaucAtCrLf);
    }
    /* +CSSU: <code2>[,<index>[,<number>,<type>[,<subaddr>,<satype>]]] */
    else if( (VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CSSU))
     && ( MN_CALL_SS_NTFY_BE_DEFLECTED < pstCallInfo->stSsNotify.enCode ) )
    {
        ucTmp = At_GetSsCode(pstCallInfo->stSsNotify.enCode,pstCallInfo->enCallState);

#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s^CSSU: ",
                                            gaucAtCrLf);
        }
        else
#endif
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s+CSSU: ",
                                            gaucAtCrLf);
        }

        if( 0xFF != ucTmp )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%d",
                                                ucTmp);
        }

        if( MN_CALL_SS_NTFY_MT_CUG_INFO == pstCallInfo->stSsNotify.enCode )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",%d",
                                                pstCallInfo->stSsNotify.ulCugIndex);
        }

        /* <number>, */
        if( 0 != pstCallInfo->stCallNumber.ucNumLen )
        {
            AT_BcdNumberToAscii(pstCallInfo->stCallNumber.aucBcdNum,
                                pstCallInfo->stCallNumber.ucNumLen,
                                (VOS_CHAR *)aucAsciiNum);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",\"%s\"",
                                                aucAsciiNum);
            /* <type>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",%d",
                                                (pstCallInfo->stCallNumber.enNumType | AT_NUMBER_TYPE_EXT));
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s",
                                            gaucAtCrLf);
    }
    else
    {
        ;
    }
    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
    return;
}

TAF_VOID At_CsIncomingEvtOfIncomeStateIndProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    TAF_UINT16                          usLength;
    TAF_UINT8                           ucCLIValid;
    TAF_UINT32                          ulTimerName;
    TAF_UINT8                           aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1];
    AT_DCE_MSC_STRU                     stMscStru;
    TAF_UINT16                          usLoop;
    TAF_UINT32                          ulDelayaCnt;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucIndex);
    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    usLength = 0;
    usLoop = 0;
    ulDelayaCnt = 50000;

    if( AT_CRC_ENABLE_TYPE == pstSsCtx->ucCrcType )         /* 命令与协议不符 */
    {
        /* +CRC -- +CRING: <type> */

#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%sIRING%s",
                                                gaucAtCrLf,
                                                gaucAtCrLf);
        }
        else
#endif
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s+CRING: ",
                                                gaucAtCrLf);

            usLength += (TAF_UINT16)At_CcClass2Print(pstCallInfo->enCallType,
                                                     pgucAtSndCodeAddr + usLength);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s",
                                                gaucAtCrLf);
        }
    }
    else
    {
        if (AT_EVT_IS_CS_VIDEO_CALL(pstCallInfo->enCallType, pstCallInfo->enVoiceDomain))
        {
            /* 由于此时还是广播上报，需要找到对应的MODEM端口,进行管脚信号的
               设置 */
            for(usLoop = 0; usLoop < AT_MAX_CLIENT_NUM; usLoop++)
            {
                if (AT_MODEM_USER == gastAtClientTab[usLoop].UserType)
                {
                    ucIndex = (VOS_UINT8)usLoop;
                    break;
                }
            }

            /* 如果是可视电话，如果当前没有MODEM端口，则直接返回 */
            if (AT_MAX_CLIENT_NUM == usLoop)
            {
                return;
            }

            /* 拉高DSR和RI的管脚信号,用于触发来电指示( 根据抓取后台软件的
               USB管脚 ) */
            PS_MEM_SET(&stMscStru,0x00,sizeof(AT_DCE_MSC_STRU));
            stMscStru.OP_Dsr = 1;
            stMscStru.ucDsr  = 1;
            stMscStru.OP_Ri = 1;
            stMscStru.ucRi  = 1;
            stMscStru.OP_Dcd = 1;
            stMscStru.ucDcd  = 1;
            AT_SetModemStatus((VOS_UINT8)usLoop,&stMscStru);

            /* 为了保证拉高的管教信号在PC侧先处理，增加延时的处理  */
            ulDelayaCnt = 50000;
            while( ulDelayaCnt-- )
            {
                ;
            }

            /*EVENT-UE UP DCD*/
            AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DCE_UP_DCD,
                            VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
        }

#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%sIRING%s",
                                                gaucAtCrLf,
                                                gaucAtCrLf);
        }
        else
#endif
        {
            /* +CRC -- RING */
            if( AT_V_ENTIRE_TYPE == gucAtVType )
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    "%sRING%s",
                                                    gaucAtCrLf,
                                                    gaucAtCrLf);
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    "2\r");
            }

        }

    }

    if( AT_CLIP_ENABLE_TYPE == pstSsCtx->ucClipType )
    {
        /*
        +CLIP: <number>,<type>
        其它部分[,<subaddr>,<satype>[,[<alpha>][,<CLI validity>]]]不用上报
        */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s+CLIP: ",
                                            gaucAtCrLf);

        if( MN_CALL_NO_CLI_PAYPHONE == pstCallInfo->enNoCliCause )
        {
            ucCLIValid = MN_CALL_NO_CLI_INTERACT;
        }
        else
        {
            ucCLIValid = pstCallInfo->enNoCliCause;
        }

        if( 0 != pstCallInfo->stCallNumber.ucNumLen )
        {
            AT_BcdNumberToAscii(pstCallInfo->stCallNumber.aucBcdNum,
                                pstCallInfo->stCallNumber.ucNumLen,
                                (VOS_CHAR *)aucAsciiNum);

           /* 来电号码类型为国际号码，需要在号码前加+,否则会回拨失败 */
           if (MN_MSG_TON_INTERNATIONAL == ((pstCallInfo->stCallNumber.enNumType >> 4) & 0x07))
           {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "\"+%s\",%d,\"\",,\"\",%d",
                                                    aucAsciiNum,
                                                    (pstCallInfo->stCallNumber.enNumType | AT_NUMBER_TYPE_EXT),
                                                    ucCLIValid);
           }
           else
           {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    "\"%s\",%d,\"\",,\"\",%d",
                                                    aucAsciiNum,
                                                    (pstCallInfo->stCallNumber.enNumType | AT_NUMBER_TYPE_EXT),
                                                    ucCLIValid);
           }
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "\"\",,\"\",,\"\",%d",
                                                ucCLIValid);
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s",
                                            gaucAtCrLf);
    }

    if ( AT_SALS_ENABLE_TYPE == pstSsCtx->ucSalsType )
    {
        /*上报是线路1还是线路2的来电*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s^ALS: ",
                                           gaucAtCrLf);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d",
                                           pstCallInfo->enAlsLineNo);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s",
                                           gaucAtCrLf);
    }

    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);

    /* 只有呼叫类型为voice时才支持自动接听功能，其他场景暂时不支持自动接听 */
    if ((MN_CALL_TYPE_VOICE  == pstCallInfo->enCallType)
     && (0 != pstCcCtx->stS0TimeInfo.ucS0TimerLen))
    {
        /* 如果自动接听功能没启动，收到RING事件后启动 */
        if (TAF_TRUE != pstCcCtx->stS0TimeInfo.bTimerStart)
        {
            ulTimerName = AT_S0_TIMER;
            ulTimerName |= AT_INTERNAL_PROCESS_TYPE;
            ulTimerName |= (ucIndex<<12);

            AT_StartRelTimer(&(pstCcCtx->stS0TimeInfo.s0Timer),
                              (pstCcCtx->stS0TimeInfo.ucS0TimerLen)*1000,
                              ulTimerName,
                              pstCallInfo->callId,
                              VOS_RELTIMER_NOLOOP);
            pstCcCtx->stS0TimeInfo.bTimerStart = TAF_TRUE;
            pstCcCtx->stS0TimeInfo.ulTimerName = ulTimerName;
        }
    }


    if (AT_EVT_IS_CS_VIDEO_CALL(pstCallInfo->enCallType, pstCallInfo->enVoiceDomain))
    {
        /* 为了保证呼叫相关信息(如RING，来电号码)在PC侧先处理，增加延时的处理  */
        ulDelayaCnt = 50000;
        while( ulDelayaCnt-- )
        {
            ;
        }

        /* 上报RING之后，拉低RI的管脚信号，仍然拉高DSR的管脚信号,
           ( 根据抓取后台软件的USB管脚信号交互 ) */
        PS_MEM_SET(&stMscStru,0x00,sizeof(AT_DCE_MSC_STRU));
        stMscStru.OP_Ri = 1;
        stMscStru.ucRi  = 0;
        stMscStru.OP_Dcd = 1;
        stMscStru.ucDcd  = 1;
        stMscStru.OP_Dsr = 1;
        stMscStru.ucDsr  = 1;
        AT_SetModemStatus((VOS_UINT8)usLoop,&stMscStru);
    }

    return;

}
TAF_VOID At_CsIncomingEvtOfWaitStateIndProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    TAF_UINT16 usLength;
    TAF_UINT8  aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1];
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    usLength = 0;
    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    if( AT_CCWA_ENABLE_TYPE == pstSsCtx->ucCcwaType )         /* 命令与协议不符 */
    {
#if (FEATURE_ON == FEATURE_IMS)
        if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s^CCWA: ",
                                                gaucAtCrLf);
        }
        else
#endif
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s+CCWA: ",
                                                gaucAtCrLf);
        }

        if( 0 != pstCallInfo->stCallNumber.ucNumLen )
        {
            AT_BcdNumberToAscii(pstCallInfo->stCallNumber.aucBcdNum,
                                pstCallInfo->stCallNumber.ucNumLen,
                                (VOS_CHAR *)aucAsciiNum);

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "\"%s\",%d",
                                                aucAsciiNum,
                                                (pstCallInfo->stCallNumber.enNumType | AT_NUMBER_TYPE_EXT));
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",");
        }
        if(MN_CALL_TYPE_VOICE == pstCallInfo->enCallType)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",1");
        }
        else if( MN_CALL_TYPE_VIDEO == pstCallInfo->enCallType )
        {
            if (TAF_CALL_VOICE_DOMAIN_IMS == pstCallInfo->enVoiceDomain)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    ",2");
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                    ",32");
            }
        }
        else if( MN_CALL_TYPE_FAX == pstCallInfo->enCallType )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",4");
        }
        else if( MN_CALL_TYPE_CS_DATA == pstCallInfo->enCallType )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                ",2");
        }
        else
        {

        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s",
                                            gaucAtCrLf);

        if ( AT_SALS_ENABLE_TYPE == pstSsCtx->ucSalsType)
        {
            /*上报是线路1还是线路2的来电*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s^ALS: ",
                                              gaucAtCrLf);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d",
                                               pstCallInfo->enAlsLineNo);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);
        }
    }

    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
    return;
}


TAF_VOID At_CsIncomingEvtIndProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    g_ucDtrDownFlag = VOS_FALSE;

    /*
    需要增加来电类型，传真，数据，可视电话，语音呼叫
    */
    /*
    +CRC -- +CRING: <type> || RING
    +CLIP: <number>,<type>[,<subaddr>,<satype>[,[<alpha>][,<CLI validity>]]]
    */
    if ( MN_CALL_S_INCOMING == pstCallInfo->enCallState )
    {
        At_CsIncomingEvtOfIncomeStateIndProc(ucIndex,enEvent,pstCallInfo);
    }
    else if ( MN_CALL_S_WAITING == pstCallInfo->enCallState )
    {
        At_CsIncomingEvtOfWaitStateIndProc(ucIndex,enEvent,pstCallInfo);
    }
    else
    {
        return;
    }

#if (FEATURE_ON == FEATURE_AT_HSUART)
    if ( (MN_CALL_TYPE_VOICE == pstCallInfo->enCallType)
      || (MN_CALL_TYPE_EMERGENCY == pstCallInfo->enCallType) )
    {
        /* 通过UART端口的RING脚输出波形通知TE */
        AT_VoiceStartRingTe(pstCallInfo->callId);
    }
#endif

    return;
}

/*****************************************************************************
 函 数 名  : AT_ConCallMsgTypeToCuusiMsgType
 功能描述  : 将CALL定义的消息类型转换为AT协议规定的消息类型
 输入参数  : enMsgType          :CALL定义的消息类型
 输出参数  : penCuusiMsgType    :CUUSI定义的消息类型
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年7月29日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_ConCallMsgTypeToCuusiMsgType(
    MN_CALL_UUS1_MSG_TYPE_ENUM_U32      enMsgType,
    AT_CUUSI_MSG_TYPE_ENUM_U32          *penCuusiMsgType

)
{
    VOS_UINT32                          i;

    for ( i = 0 ; i < sizeof(g_stCuusiMsgType)/sizeof(AT_CALL_CUUSI_MSG_STRU) ; i++ )
    {
        if ( enMsgType == g_stCuusiMsgType[i].enCallMsgType)
        {
            *penCuusiMsgType = g_stCuusiMsgType[i].enCuusiMsgType;
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


/*****************************************************************************
 函 数 名  : AT_ConCallMsgTypeToCuusuMsgType
 功能描述  : 将CALL定义的消息类型转换为AT协议规定的消息类型
 输入参数  : enMsgType          :CALL定义的消息类型
 输出参数  : penCuus1UMsgType   :CUUSU定义的消息类型
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年7月29日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_ConCallMsgTypeToCuusuMsgType(
    MN_CALL_UUS1_MSG_TYPE_ENUM_U32      enMsgType,
    AT_CUUSU_MSG_TYPE_ENUM_U32          *penCuusuMsgType

)
{
    VOS_UINT32                          i;

    for ( i = 0 ; i < sizeof(g_stCuusuMsgType)/sizeof(AT_CALL_CUUSU_MSG_STRU) ; i++ )
    {
        if ( enMsgType == g_stCuusuMsgType[i].enCallMsgType)
        {
            *penCuusuMsgType = g_stCuusuMsgType[i].enCuusuMsgType;
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_VOID AT_CsUus1InfoEvtIndProc(
    VOS_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                   *pstCallInfo
)
{
    VOS_UINT32                          ulMsgType;
    AT_CUUSI_MSG_TYPE_ENUM_U32          enCuusiMsgType;
    AT_CUUSU_MSG_TYPE_ENUM_U32          enCuusuMsgType;
    VOS_UINT32                          ulRet;
    MN_CALL_DIR_ENUM_U8                 enCallDir;
    VOS_UINT16                          usLength;

    enCallDir = pstCallInfo->enCallDir;
    ulMsgType = AT_CUUSI_MSG_ANY;

    ulRet = AT_ConCallMsgTypeToCuusiMsgType(pstCallInfo->stUusInfo.enMsgType,&enCuusiMsgType);
    if ( VOS_OK == ulRet )
    {
        ulMsgType = enCuusiMsgType;
        enCallDir = MN_CALL_DIR_MO;
    }
    else
    {
        ulRet = AT_ConCallMsgTypeToCuusuMsgType(pstCallInfo->stUusInfo.enMsgType,&enCuusuMsgType);
        if ( VOS_OK == ulRet )
        {
            ulMsgType = enCuusuMsgType;
            enCallDir = MN_CALL_DIR_MT;
        }
    }

    if ( VOS_OK != ulRet)
    {
        if ( MN_CALL_DIR_MO == pstCallInfo->enCallDir)
        {
            ulMsgType = AT_CUUSI_MSG_ANY;
        }
        else
        {
            ulMsgType = AT_CUUSU_MSG_ANY;
        }
    }

    usLength = 0;

    if ( MN_CALL_DIR_MO == enCallDir )
    {
        /* 未激活则不进行任何处理,不能上报 */
        if ( VOS_FALSE == AT_CheckRptCmdStatus(pstCallInfo->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CUUS1I))
        {
            return;
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "%s+CUUS1I:",
                                   gaucAtCrLf);
    }
    else
    {

        /* 未激活则不进行任何处理,不能上报 */
        if ( VOS_FALSE == AT_CheckRptCmdStatus(pstCallInfo->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CUUS1U) )
        {
            return;
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "%s+CUUS1U:",
                                   gaucAtCrLf);
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                    "%d,",
                                    ulMsgType);

    usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                   (TAF_INT8 *)pgucAtSndCodeAddr,
                                   (TAF_UINT8 *)pgucAtSndCodeAddr + usLength,
                                   pstCallInfo->stUusInfo.aucUuie,
                                   (pstCallInfo->stUusInfo.aucUuie[MN_CALL_LEN_POS] + MN_CALL_UUIE_HEADER_LEN));

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);

}


VOS_VOID At_ProcSetClccResult(
    VOS_UINT8                           ucNumOfCalls,
    MN_CALL_INFO_QRY_CNF_STRU          *pstCallInfos,
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT8                           ucTmp;
    AT_CLCC_MODE_ENUM_U8                enClccMode;
    VOS_UINT8                           aucAsciiNum[MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1];
    VOS_UINT16                          usLength;

    VOS_UINT8                          ucNumberType;

    ucNumberType = AT_NUMBER_TYPE_UNKOWN;

    usLength = 0;

    if ( (0 != ucNumOfCalls)
        && ( ucNumOfCalls <=  AT_CALL_MAX_NUM))
    {
        for (ucTmp = 0; ucTmp < ucNumOfCalls; ucTmp++)
        {
            /* <CR><LF> */
            if(0 != ucTmp)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s",
                                                   gaucAtCrLf);
            }

            AT_MapCallTypeModeToClccMode(pstCallInfos->astCallInfos[ucTmp].enCallType, &enClccMode);

            /* +CLCC:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr+ usLength,
                                               "%s: %d,%d,%d,%d,%d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName, /* +CLCC:  */
                                               pstCallInfos->astCallInfos[ucTmp].callId,             /* <id1>, */
                                               pstCallInfos->astCallInfos[ucTmp].enCallDir,          /* <dir>, */
                                               pstCallInfos->astCallInfos[ucTmp].enCallState,        /* <stat>, */
                                               enClccMode,                                          /* <mode>, */
                                               pstCallInfos->astCallInfos[ucTmp].enMptyState         /* <mpty>, */
                                               );

            if (MN_CALL_DIR_MO == pstCallInfos->astCallInfos[ucTmp].enCallDir)
            {
                if (0 != pstCallInfos->astCallInfos[ucTmp].stConnectNumber.ucNumLen)
                {
                    /* <number>, */
                    AT_BcdNumberToAscii(pstCallInfos->astCallInfos[ucTmp].stConnectNumber.aucBcdNum,
                                        pstCallInfos->astCallInfos[ucTmp].stConnectNumber.ucNumLen,
                                        (TAF_CHAR*)aucAsciiNum);

                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"%s\",%d,\"\",",
                                                       aucAsciiNum,
                                                       (pstCallInfos->astCallInfos[ucTmp].stConnectNumber.enNumType | AT_NUMBER_TYPE_EXT));
                }
                else if (0 != pstCallInfos->astCallInfos[ucTmp].stCalledNumber.ucNumLen)
                {
                    /* <number>, */
                    AT_BcdNumberToAscii(pstCallInfos->astCallInfos[ucTmp].stCalledNumber.aucBcdNum,
                                        pstCallInfos->astCallInfos[ucTmp].stCalledNumber.ucNumLen,
                                        (TAF_CHAR*)aucAsciiNum);

                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"%s\",%d,\"\",",
                                                       aucAsciiNum,
                                                       (pstCallInfos->astCallInfos[ucTmp].stCalledNumber.enNumType | AT_NUMBER_TYPE_EXT));


                }
                else
                {
                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"\",%d",ucNumberType);
                }
            }
            else
            {
                if (0 != pstCallInfos->astCallInfos[ucTmp].stCallNumber.ucNumLen)
                {
                    /* <number>, */
                    AT_BcdNumberToAscii(pstCallInfos->astCallInfos[ucTmp].stCallNumber.aucBcdNum,
                                        pstCallInfos->astCallInfos[ucTmp].stCallNumber.ucNumLen,
                                        (VOS_CHAR *)aucAsciiNum);

                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"%s\",%d,\"\",",
                                                       aucAsciiNum,
                                                       (pstCallInfos->astCallInfos[ucTmp].stCallNumber.enNumType | AT_NUMBER_TYPE_EXT));
                }
                else
                {
                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"\",%d",ucNumberType);

                }
            }
        }
    }

    gstAtSendData.usBufLen = usLength;

}

#if (FEATURE_ON == FEATURE_IMS)

VOS_VOID At_ProcQryClccResult(
    VOS_UINT8                           ucNumOfCalls,
    MN_CALL_INFO_QRY_CNF_STRU          *pstCallInfos,
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT8                           ucTmp;
    VOS_UINT8                           ucNumberType;
    VOS_UINT8                           aucAsciiNum[MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1];
    VOS_UINT16                          usLength;
    AT_CLCC_MODE_ENUM_U8                enClccMode;

    ucNumberType    = AT_NUMBER_TYPE_UNKOWN;
    usLength        = 0;

    if ((0 != ucNumOfCalls)
     && ( ucNumOfCalls <=  AT_CALL_MAX_NUM))
    {
        for (ucTmp = 0; ucTmp < ucNumOfCalls; ucTmp++)
        {
            /* <CR><LF> */
            if(0 != ucTmp)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s",
                                                   gaucAtCrLf);
            }

            AT_MapCallTypeModeToClccMode(pstCallInfos->astCallInfos[ucTmp].enCallType, &enClccMode);

            /* ^CLCC:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr+ usLength,
                                               "%s: %d,%d,%d,%d,%d,%d,%d,%d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName, /* ^CLCC:  */
                                               pstCallInfos->astCallInfos[ucTmp].callId,            /* <id1>, */
                                               pstCallInfos->astCallInfos[ucTmp].enCallDir,         /* <dir>, */
                                               pstCallInfos->astCallInfos[ucTmp].enCallState,       /* <stat>, */
                                               enClccMode,                                          /* <mode>, */
                                               pstCallInfos->astCallInfos[ucTmp].enMptyState,       /* <mpty>, */
                                               pstCallInfos->astCallInfos[ucTmp].enVoiceDomain,     /* <voice_domain> */
                                               pstCallInfos->astCallInfos[ucTmp].enCallType,        /* <call_type> */
                                               pstCallInfos->astCallInfos[ucTmp].ucEConferenceFlag  /* <isEConference> */
                                               );

            if (MN_CALL_DIR_MO == pstCallInfos->astCallInfos[ucTmp].enCallDir)
            {
                if (0 != pstCallInfos->astCallInfos[ucTmp].stConnectNumber.ucNumLen)
                {
                    /* <number>, */
                    AT_BcdNumberToAscii(pstCallInfos->astCallInfos[ucTmp].stConnectNumber.aucBcdNum,
                                        pstCallInfos->astCallInfos[ucTmp].stConnectNumber.ucNumLen,
                                        (TAF_CHAR*)aucAsciiNum);

                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"%s\",%d,\"\",",
                                                       aucAsciiNum,
                                                       (pstCallInfos->astCallInfos[ucTmp].stConnectNumber.enNumType | AT_NUMBER_TYPE_EXT));
                }
                else if (0 != pstCallInfos->astCallInfos[ucTmp].stCalledNumber.ucNumLen)
                {
                    /* <number>, */
                    AT_BcdNumberToAscii(pstCallInfos->astCallInfos[ucTmp].stCalledNumber.aucBcdNum,
                                        pstCallInfos->astCallInfos[ucTmp].stCalledNumber.ucNumLen,
                                        (TAF_CHAR*)aucAsciiNum);

                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"%s\",%d,\"\",",
                                                       aucAsciiNum,
                                                       (pstCallInfos->astCallInfos[ucTmp].stCalledNumber.enNumType | AT_NUMBER_TYPE_EXT));


                }
                else
                {
                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"\",%d",ucNumberType);
                }
            }
            else
            {
                if (0 != pstCallInfos->astCallInfos[ucTmp].stCallNumber.ucNumLen)
                {
                    /* <number>, */
                    AT_BcdNumberToAscii(pstCallInfos->astCallInfos[ucTmp].stCallNumber.aucBcdNum,
                                        pstCallInfos->astCallInfos[ucTmp].stCallNumber.ucNumLen,
                                        (VOS_CHAR *)aucAsciiNum);

                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"%s\",%d,\"\",",
                                                       aucAsciiNum,
                                                       (pstCallInfos->astCallInfos[ucTmp].stCallNumber.enNumType | AT_NUMBER_TYPE_EXT));
                }
                else
                {
                    /* <type>,不报<alpha>,<priority> */
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",\"\",%d",ucNumberType);

                }
            }
        }
    }

    gstAtSendData.usBufLen = usLength;

}
VOS_UINT32 At_ProcQryClccEconfResult(
    TAF_CALL_ECONF_INFO_QRY_CNF_STRU   *pstCallInfos,
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucTmp;
    VOS_CHAR                            aucAsciiNum[MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1];

    usLength        = 0;
    PS_MEM_SET(aucAsciiNum, 0x0, sizeof(aucAsciiNum));

     /* ^CLCCECONF: Maximum-user-count, n_address */
    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      "%s: %d,%d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,  /* ^CLCCECONF:  */
                                      pstCallInfos->ucNumOfMaxCalls,                        /* Maximum-user-count */
                                      pstCallInfos->ucNumOfCalls);

    if (0 != pstCallInfos->ucNumOfCalls)
    {
        /* n_address */
        for (ucTmp = 0; ucTmp < pstCallInfos->ucNumOfCalls; ucTmp++)
        {
            /* 转换电话号码 */
            if (0 != pstCallInfos->astCallInfo[ucTmp].stCallNumber.ucNumLen)
            {
                /* <number>, */
                AT_BcdNumberToAscii(pstCallInfos->astCallInfo[ucTmp].stCallNumber.aucBcdNum,
                                    pstCallInfos->astCallInfo[ucTmp].stCallNumber.ucNumLen,
                                    aucAsciiNum);
            }

            /* entity, Display-text,Status */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"%s\",\"%s\",%d",
                                               aucAsciiNum,                                         /* 电话号码 */
                                               pstCallInfos->astCallInfo[ucTmp].aucDisplaytext,     /* display-text */
                                               pstCallInfos->astCallInfo[ucTmp].enCallState);       /* Call State */

        }
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;

}
#endif


VOS_VOID At_CsAllCallInfoEvtCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT8                           ucNumOfCalls;
    MN_CALL_INFO_QRY_CNF_STRU          *pstCallInfos;
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucTmp;
    AT_CPAS_STATUS_ENUM_U8              enCpas;
    VOS_UINT32                          ulRet;

    /* 初始化 */
    ucIndex  = 0;

    /* 获取当前所有不为IDLE态的呼叫信息 */
    pstCallInfos = (MN_CALL_INFO_QRY_CNF_STRU *)pstData->aucContent;
    ucNumOfCalls = pstCallInfos->ucNumOfCalls;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCallInfos->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_CsAllCallInfoEvtCnfProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_CsAllCallInfoEvtCnfProc: AT_BROADCAST_INDEX.");
        return;
    }

    /* 格式化命令返回 */
    if (AT_CMD_CLCC_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* CLCC命令的结果回复 */
        At_ProcSetClccResult(ucNumOfCalls, pstCallInfos, ucIndex);

        ulRet = AT_OK;
    }
#if (FEATURE_ON == FEATURE_IMS)
    else if (AT_CMD_CLCC_QRY == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* ^CLCC?命令的结果回复 */
        At_ProcQryClccResult(ucNumOfCalls, pstCallInfos, ucIndex);

        ulRet = AT_OK;
    }
#endif
    else if (AT_CMD_CPAS_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* CPAS命令的结果回复 */
        if (ucNumOfCalls > AT_CALL_MAX_NUM)
        {
            At_FormatResultData(ucIndex, AT_CME_UNKNOWN);
            return;
        }

        if (0 == ucNumOfCalls)
        {
            enCpas = AT_CPAS_STATUS_READY;
        }
        else
        {
            enCpas = AT_CPAS_STATUS_CALL_IN_PROGRESS;
            for (ucTmp = 0; ucTmp < ucNumOfCalls; ucTmp++)
            {
                if (MN_CALL_S_INCOMING == pstCallInfos->astCallInfos[ucTmp].enCallState)
                {
                    enCpas = AT_CPAS_STATUS_RING;
                    break;
                }
            }
        }

        gstAtSendData.usBufLen  = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        enCpas);

        ulRet = AT_OK;

    }
    else
    {
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}


VOS_VOID AT_ProcCsCallConnectInd(
    VOS_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usLength;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_ProcCsCallConnectInd: Get modem id fail.");
        return;
    }

#if (FEATURE_ON == FEATURE_AT_HSUART)
    /* 停止RING TE */
    AT_VoiceStopRingTe(pstCallInfo->callId);
#endif

    /* CS呼叫成功, 清除CS域错误码 */
    AT_SetCsCallErrCause(ucIndex, TAF_CS_CAUSE_SUCCESS);

    if (VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CONN))
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                            "%s^CONN:%d,%d%s",
                                            gaucAtCrLf,
                                            pstCallInfo->callId,
                                            pstCallInfo->enCallType,
                                            gaucAtCrLf);
        At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
    }

    return;
}

TAF_VOID At_CsIndProc(
    TAF_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    AT_DCE_MSC_STRU                     stMscStru;
    TAF_UINT16                          usLoop;
    VOS_UINT32                          ulTimerName;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;


    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucIndex);

    switch( enEvent )            /* 根据事件类型 */
    {
        case MN_CALL_EVT_ALL_RELEASED:

            /* 收到所有呼叫都RELEASED后，将当前是否存在呼叫标志置为FALSE */
            pstCcCtx->ulCurIsExistCallFlag = VOS_FALSE;
            break;
        case MN_CALL_EVT_INCOMING:
            /*处理来电上报事件*/
            At_CsIncomingEvtIndProc(ucIndex,enEvent,pstCallInfo);

            /* 有来电时，将当前是否存在呼叫标志置为TRUE */
            pstCcCtx->ulCurIsExistCallFlag = VOS_TRUE;

            break;

        case MN_CALL_EVT_RELEASED:

            /* 记录cause值 */
            AT_SetCsCallErrCause(ucIndex, pstCallInfo->enCause);

            /* 如果是可视电话，因为在INCOMING的时候拉高了DCD的管脚信号，现在拉低DCD的管脚信号 */
            if (AT_EVT_IS_CS_VIDEO_CALL(pstCallInfo->enCallType, pstCallInfo->enVoiceDomain))
            {

                /* 由于此时还是广播上报，需要找到对应的MODEM端口,进行管脚信号的
                   设置 */
                for(usLoop = 0; usLoop < AT_MAX_CLIENT_NUM; usLoop++)
                {
                    if (AT_MODEM_USER == gastAtClientTab[usLoop].UserType)
                    {
                        ucIndex = (VOS_UINT8)usLoop;
                        break;
                    }
                }

                /* 如果是可视电话，如果当前没有MODEM端口，则直接返回 */
                if (AT_MAX_CLIENT_NUM == usLoop)
                {
                    return;
                }


                PS_MEM_SET(&stMscStru, 0, sizeof(stMscStru));
                /* 拉低DCD信号 */
                stMscStru.OP_Dcd = 1;
                stMscStru.ucDcd = 0;
                stMscStru.OP_Dsr = 1;
                stMscStru.ucDsr = 1;
                AT_SetModemStatus(ucIndex,&stMscStru);

                /*EVENT-UE Down DCD*/
                AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DCE_DOWN_DCD,
                                VOS_NULL_PTR, NAS_OM_EVENT_NO_PARA);
                return;
            }

            if (VOS_TRUE == pstCcCtx->stS0TimeInfo.bTimerStart)
            {
                ulTimerName = pstCcCtx->stS0TimeInfo.ulTimerName;

                AT_StopRelTimer(ulTimerName, &(pstCcCtx->stS0TimeInfo.s0Timer));
                pstCcCtx->stS0TimeInfo.bTimerStart = VOS_FALSE;
                pstCcCtx->stS0TimeInfo.ulTimerName = 0;
            }

#if (FEATURE_ON == FEATURE_AT_HSUART)
            /* 停止RING TE */
            AT_VoiceStopRingTe(pstCallInfo->callId);
#endif

            AT_ReportCendResult(ucIndex, pstCallInfo);

            break;


        case MN_CALL_EVT_CONNECT:
            AT_ProcCsCallConnectInd(ucIndex, pstCallInfo);
            break;

        case MN_CALL_EVT_SS_NOTIFY:
            AT_CsSsNotifyEvtIndProc(ucIndex,enEvent,pstCallInfo);
            break;

        case MN_CALL_EVT_UUS1_INFO:
            AT_CsUus1InfoEvtIndProc(ucIndex,enEvent,pstCallInfo);
            break;

        default:
            AT_LOG1("At_CsIndProc CallEvent ERROR", enEvent);
            break;
    }
}
TAF_VOID At_CsEventProc(MN_AT_IND_EVT_STRU *pstData,TAF_UINT16 usLen)
{
    TAF_UINT8                           ucIndex             = 0;
    MN_CALL_INFO_STRU                  *pstCallInfo;
    MN_CALL_EVENT_ENUM_U32              enEvent;
    TAF_UINT32                          ulEventLen;

    AT_LOG1("At_CsMsgProc pEvent->ClientId",pstData->clientId);
    AT_LOG1("At_CsMsgProc usMsgName",pstData->usMsgName);

    ulEventLen = sizeof(MN_CALL_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);
    pstCallInfo = (MN_CALL_INFO_STRU *)&pstData->aucContent[ulEventLen];

    if(AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_CsEventProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        if(AT_FAILURE == At_ClientIdToUserId(pstCallInfo->clientId, &ucIndex))
        {
            AT_NORM_LOG1("At_CsMsgProc: Not AT Client Id, clientid:", pstCallInfo->clientId);
            return;
        }

        if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
        {
            At_CsIndProc(ucIndex, enEvent, pstCallInfo);
        }
        else
        {
            AT_LOG1("At_CsMsgProc ucIndex",ucIndex);
            AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

            At_CsRspProc(ucIndex, enEvent, pstCallInfo);

        }
    }
    else
    {
        AT_LOG1("At_CsMsgProc ucIndex",ucIndex);
        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

        At_CsRspProc(ucIndex, enEvent, pstCallInfo);
    }

}


VOS_VOID At_QryAlsCnf(MN_AT_IND_EVT_STRU *pstData)
{
    MN_CALL_QRY_ALS_CNF_STRU           *pstAlsCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLen;
    AT_MODEM_SS_CTX_STRU               *pstModemSsCtx = VOS_NULL_PTR;

    pstAlsCnf = (MN_CALL_QRY_ALS_CNF_STRU *)pstData->aucContent;

    if (AT_SUCCESS != At_ClientIdToUserId(pstAlsCnf->ClientId, &ucIndex))
    {
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_QryAlsCnf : AT_BROADCAST_INDEX.");
        return;
    }

    usLen                  = 0;
    ulResult               = AT_ERROR;
    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    pstModemSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    if (TAF_ERR_NO_ERROR == pstAlsCnf->ulRet )
    {
        usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        "%s: %d",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        pstModemSsCtx->ucSalsType);

        usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR*)pgucAtSndCodeAddr + usLen,
                                        ",%d",
                                        pstAlsCnf->enAlsLine);

        gstAtSendData.usBufLen = usLen;

        ulResult = AT_OK;
    }

    else
    {
        /* 当不支持ALS特性时，由call上报错误时间，AT返回error，AT不区分是否支持该特性 */
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex,ulResult);

}
VOS_VOID At_QryUus1Cnf(MN_AT_IND_EVT_STRU *pstData)
{
    TAF_PH_QRY_UUS1_INFO_CNF_STRU*      pstUus1Cnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          i;
    VOS_UINT16                          usLen;
    VOS_UINT32                          ulUus1IFlg;
    VOS_UINT32                          ulUus1UFlg;

    pstUus1Cnf = (TAF_PH_QRY_UUS1_INFO_CNF_STRU *)pstData->aucContent;

    if (AT_SUCCESS != At_ClientIdToUserId(pstUus1Cnf->ClientId, &ucIndex))
    {
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_QryUus1Cnf : AT_BROADCAST_INDEX.");
        return;
    }

    usLen                  = 0;
    ulResult               = AT_ERROR;
    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 变量初始化为打开主动上报 */
    ulUus1IFlg  = VOS_TRUE;
    ulUus1UFlg  = VOS_TRUE;

    /* UUS1I是否打开 */
    if (MN_CALL_CUUS1_DISABLE == pstUus1Cnf->aenSetType[0])
    {
        ulUus1IFlg  = VOS_FALSE;
    }

    /* UUS1U是否打开 */
    if (MN_CALL_CUUS1_DISABLE == pstUus1Cnf->aenSetType[1])
    {
        ulUus1UFlg  = VOS_FALSE;
    }

    if (TAF_ERR_NO_ERROR == pstUus1Cnf->ulRet )
    {
        usLen +=  (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                         (VOS_CHAR*)pgucAtSndCodeAddr,
                                         "%s",
                                         gaucAtCrLf);

        usLen +=  (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                         (VOS_CHAR*)pgucAtSndCodeAddr + usLen,
                                         "%s:",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)(pgucAtSndCodeAddr + usLen),
                                        "%d,%d",
                                        ulUus1IFlg,
                                        ulUus1UFlg);

        for ( i = 0 ; i < pstUus1Cnf->ulActNum ; i++ )
        {
            usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            (VOS_CHAR *)(pgucAtSndCodeAddr + usLen),
                                            ",%d,",
                                            pstUus1Cnf->stUus1Info[i].enMsgType);

            usLen += (VOS_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                         (VOS_INT8 *)pgucAtSndCodeAddr,
                                                         (VOS_UINT8 *)pgucAtSndCodeAddr+usLen,
                                                         pstUus1Cnf->stUus1Info[i].aucUuie,
                                                         pstUus1Cnf->stUus1Info[i].aucUuie[MN_CALL_LEN_POS] + MN_CALL_UUIE_HEADER_LEN);

        }

        gstAtSendData.usBufLen = usLen;

        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex,ulResult);

}
VOS_VOID At_SetAlsCnf(MN_AT_IND_EVT_STRU *pstData)
{
    MN_CALL_SET_ALS_CNF_STRU      *pstUAlsCnf;
    VOS_UINT8                      ucIndex;
    VOS_UINT32                     ulResult;

    pstUAlsCnf = (MN_CALL_SET_ALS_CNF_STRU *)pstData->aucContent;

    if (AT_FAILURE == At_ClientIdToUserId(pstUAlsCnf->ClientId, &ucIndex))
    {
        AT_WARN_LOG("At_SetAlsCnf: AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_SetAlsCnf : AT_BROADCAST_INDEX.");
        return;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if( TAF_ERR_NO_ERROR == pstUAlsCnf->ulRet )
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_CME_UNKNOWN;
    }

    At_FormatResultData(ucIndex,ulResult);
}

VOS_VOID At_SetUus1Cnf(MN_AT_IND_EVT_STRU *pstData)
{
    TAF_PH_SET_UUS1_INFO_CNF_STRU* pstUus1Cnf;
    VOS_UINT8                      ucIndex;
    VOS_UINT32                     ulResult;

    pstUus1Cnf = (TAF_PH_SET_UUS1_INFO_CNF_STRU *)pstData->aucContent;

    if (AT_FAILURE == At_ClientIdToUserId(pstUus1Cnf->ClientId, &ucIndex))
    {
        AT_WARN_LOG("At_SetUus1Cnf: AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_SetUus1Cnf : AT_BROADCAST_INDEX.");
        return;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstUus1Cnf->ulRet)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_CME_INCORRECT_PARAMETERS;
    }

    At_FormatResultData(ucIndex,ulResult);

}


TAF_VOID At_CsMsgProc(MN_AT_IND_EVT_STRU *pstData,TAF_UINT16 usLen)
{
    MN_CALL_EVENT_ENUM_U32              enEvent;
    TAF_UINT32                          ulEventLen;

    ulEventLen = sizeof(MN_CALL_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);

    /* 由于原来回复消息的结构MN_CALL_INFO_STRU不是联合结构，所以该结构很大，
       不适合核间通信，所以新修改的消息采用其他结构,*/
    switch( enEvent )
    {
        case MN_CALL_EVT_CLCC_INFO:
            /* 处理MN Call发来的消息:回复查询当前所有呼叫信息的消息 */
            At_CsAllCallInfoEvtCnfProc(pstData,usLen);
            break;
        case MN_CALL_EVT_SET_ALS_CNF:
            At_SetAlsCnf(pstData);
            break;
        case MN_CALL_EVT_SET_UUS1_INFO_CNF:
            At_SetUus1Cnf(pstData);
            break;
        case MN_CALL_EVT_QRY_UUS1_INFO_CNF:
            At_QryUus1Cnf(pstData);
            break;
        case MN_CALL_EVT_QRY_ALS_CNF:
            At_QryAlsCnf(pstData);
            break;

        case MN_CALL_EVT_ECC_NUM_IND:
            At_RcvMnCallEccNumIndProc(pstData,usLen);
            break;

        case MN_CALL_EVT_CLPR_SET_CNF:
            At_SetClprCnf(pstData);
            break;

        case MN_CALL_EVT_SET_CSSN_CNF:
            At_RcvMnCallSetCssnCnf(pstData);
            break;

        case MN_CALL_EVT_CHANNEL_INFO_IND:
            AT_RcvMnCallChannelInfoInd(pstData->aucContent);
            break;

        case MN_CALL_EVT_XLEMA_CNF:
            At_RcvXlemaQryCnf(pstData, usLen);
            break;

        case MN_CALL_EVT_START_DTMF_CNF:
            AT_RcvTafCallStartDtmfCnf(pstData);
            break;

        case MN_CALL_EVT_STOP_DTMF_CNF:
            AT_RcvTafCallStopDtmfCnf(pstData);
            break;

        case MN_CALL_EVT_START_DTMF_RSLT:
        case MN_CALL_EVT_STOP_DTMF_RSLT:
            /* AT模块暂时不处理DTMF正式响应 */
            break;
        case MN_CALL_EVT_CALL_ORIG_CNF:
            At_RcvTafCallOrigCnf(pstData, usLen);
            break;

        case MN_CALL_EVT_SUPS_CMD_CNF:
            At_RcvTafCallSupsCmdCnf(pstData, usLen);
            break;

#if (FEATURE_ON == FEATURE_IMS)
        case MN_CALL_EVT_CALL_MODIFY_CNF:
            At_RcvTafCallModifyCnf(pstData, usLen);
            break;
        case MN_CALL_EVT_CALL_ANSWER_REMOTE_MODIFY_CNF:
            At_RcvTafCallAnswerRemoteModifyCnf(pstData, usLen);
            break;
        case MN_CALL_EVT_CALL_MODIFY_STATUS_IND:
            At_RcvTafCallModifyStatusInd(pstData, usLen);
            break;

        case TAF_CALL_EVT_CLCCECONF_INFO:
            AT_RcvTafGetEconfInfoCnf(pstData,usLen);
            break;

        case TAF_CALL_EVT_ECONF_DIAL_CNF:
            AT_RcvTafEconfDialCnf(pstData,usLen);
            break;

        case TAF_CALL_EVT_ECONF_NOTIFY_IND:
            AT_RcvTafEconfNotifyInd(pstData,usLen);
            break;
#endif

        default:
            At_CsEventProc(pstData,usLen);
            AT_CSCallStateReportProc(pstData);
            break;
    }
}


TAF_UINT32 At_CcfcQryReport (
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT8                           ucIndex
)
{
    TAF_UINT8  ucTmp    = 0;
    TAF_UINT16 usLength = 0;

    /*
    +CCFC: <status>,<class1>[,<number>,<type>[,<subaddr>,<satype>[,<time>]]]
    */
    if(1 == pEvent->OP_SsStatus)
    {
        /* +CCFC: <status>,<class1> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                    (TAF_CHAR *)pgucAtSndCodeAddr,
                    (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                    "%s: ",
                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                    (TAF_CHAR *)pgucAtSndCodeAddr,
                    (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                    "%d,%d",
                    (TAF_SS_ACTIVE_STATUS_MASK & pEvent->SsStatus),
                    AT_CC_CALSS_TYPE_INVALID);

        return usLength;
    }

    if (1 == pEvent->OP_FwdFeaturelist)
    {
        for(ucTmp = 0; ucTmp < pEvent->FwdFeaturelist.ucCnt; ucTmp++)
        {
            if(0 != ucTmp)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                            (TAF_CHAR *)pgucAtSndCodeAddr,
                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                            "%s",
                            gaucAtCrLf);
            }

            /* +CCFC:  */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                        (TAF_CHAR *)pgucAtSndCodeAddr,
                        (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                        "%s: ",
                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            /* <status> */
            if(1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_SsStatus)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                            (TAF_CHAR *)pgucAtSndCodeAddr,
                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                            "%d",
                            (TAF_SS_ACTIVE_STATUS_MASK
                            & (pEvent->FwdFeaturelist.astFwdFtr[ucTmp].SsStatus)));
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                            (TAF_CHAR *)pgucAtSndCodeAddr,
                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                            "0");
            }

            /* <class1> */
            if(1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_BsService)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                            (TAF_CHAR *)pgucAtSndCodeAddr,
                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                            ",%d",
                            At_GetClckClassFromBsCode(&(pEvent->FwdFeaturelist.astFwdFtr[ucTmp].BsService)));
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                            (TAF_CHAR *)pgucAtSndCodeAddr,
                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                            ",%d",
                            AT_CC_CALSS_TYPE_INVALID);
            }

            /* <number> */
            if(1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_FwdToNum)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                            (TAF_CHAR *)pgucAtSndCodeAddr,
                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                            ",\"%s\"",
                            pEvent->FwdFeaturelist.astFwdFtr[ucTmp].aucFwdToNum);

                /* <type> */
                if(1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_NumType)
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                ",%d",
                                pEvent->FwdFeaturelist.astFwdFtr[ucTmp].NumType);
                }
                else
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                ",%d",
                                At_GetCodeType(pEvent->FwdFeaturelist.astFwdFtr[ucTmp].aucFwdToNum[0]));
                }

                /* <subaddr> */
                if (1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_FwdToSubAddr)
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                ",\"%s\"",
                                pEvent->FwdFeaturelist.astFwdFtr[ucTmp].aucFwdToSubAddr);

                    /* <satype> */
                    if(1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_SubAddrType)
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                    (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                    ",%d",
                                    pEvent->FwdFeaturelist.astFwdFtr[ucTmp].SubAddrType);
                    }
                    else
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                    (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                    ",%d",
                                    At_GetCodeType(pEvent->FwdFeaturelist.astFwdFtr[ucTmp].aucFwdToSubAddr[0]));
                    }
                }
                else
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                ",,");
                }

                /* <time> */
                if(1 == pEvent->FwdFeaturelist.astFwdFtr[ucTmp].OP_NoRepCondTime)
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                ",%d",
                                pEvent->FwdFeaturelist.astFwdFtr[ucTmp].NoRepCondTime);
                }
                else
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                ",");
                }
            }


        }

    }

    return usLength;
}
TAF_VOID At_SsIndProc(TAF_UINT8  ucIndex,TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent)
{
    TAF_UINT16                          usLength = 0;
    TAF_UINT8                           ucTmp    = 0;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    VOS_UINT8                          *pucSystemAppConfig = VOS_NULL_PTR;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    switch(pEvent->SsEvent)             /* 其它事件 */
    {
        case TAF_SS_EVT_USS_NOTIFY_IND:                     /* 通知用户不用进一步操作 */
        case TAF_SS_EVT_USS_REQ_IND:                        /* 通知用户进一步操作 */
        case TAF_SS_EVT_USS_RELEASE_COMPLETE_IND:           /* 通知用户网络释放 */
        case TAF_SS_EVT_PROCESS_USS_REQ_CNF:
            /* <m> */
            if(TAF_SS_EVT_USS_NOTIFY_IND == pEvent->SsEvent)
            {
                ucTmp = 0;
            }
            else if(TAF_SS_EVT_USS_REQ_IND == pEvent->SsEvent)
            {
                ucTmp = 1;
            }
            else
            {

                if (SYSTEM_APP_ANDROID == *pucSystemAppConfig)
                {
                    ucTmp = 2;
                }
                else
                {
                    ucTmp = 0;
                }
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s+CUSD: ",gaucAtCrLf);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",ucTmp);
            /* <str> */
            if((1 == pEvent->OP_UssdString) && (1 == pEvent->OP_DataCodingScheme))
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                                  (TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"");

                switch(pstSsCtx->usUssdTransMode)
                {
                    case AT_USSD_TRAN_MODE:
                        usLength += (TAF_UINT16)At_HexString2AsciiNumPrint(AT_CMD_MAX_LEN,
                                                                           (TAF_INT8 *)pgucAtSndCodeAddr,
                                                                           pgucAtSndCodeAddr + usLength,
                                                                           pEvent->UssdString.aucUssdStr,
                                                                           pEvent->UssdString.usCnt);
                        break;

                    case AT_USSD_NON_TRAN_MODE:
                        if(TAF_SS_UCS2 == pEvent->DataCodingScheme)  /* DATA:IRA */
                        {
                            usLength += (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN,
                                                                            (TAF_INT8 *)pgucAtSndCodeAddr,
                                                                            pgucAtSndCodeAddr + usLength,
                                                                            pEvent->UssdString.aucUssdStr,
                                                                            pEvent->UssdString.usCnt);
                        }
                        else                                    /* DATA:UCS2 */
                        {
                            if(AT_CSCS_UCS2_CODE == gucAtCscsType)       /* +CSCS:UCS2 */
                            {
                                usLength += (TAF_UINT16)At_Ascii2UnicodePrint(AT_CMD_MAX_LEN,
                                                                              (TAF_INT8 *)pgucAtSndCodeAddr,
                                                                              pgucAtSndCodeAddr + usLength,
                                                                              pEvent->UssdString.aucUssdStr,
                                                                              pEvent->UssdString.usCnt);
                            }
                            else
                            {
                                PS_MEM_CPY((TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           pEvent->UssdString.aucUssdStr,
                                           pEvent->UssdString.usCnt);
                                usLength += pEvent->UssdString.usCnt;
                            }
                        }
                        break;

                    default:
                        break;
                }
                /* <dcs> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "\",%d",pEvent->DataCodingScheme);
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr +
                                               usLength,"%s",gaucAtCrLf);
            At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
            return;



        default:
            return;
    }

}



TAF_UINT8 At_GetClckClassFromBsCode(TAF_SS_BASIC_SERVICE_STRU *pstBs)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulItemsNum;

    ulItemsNum = sizeof(g_astClckClassServiceExtTbl) / sizeof(AT_CLCK_CLASS_SERVICE_TBL_STRU);

    /* 查表获取服务类型及服务码对应的Class */
    for (ulLoop = 0; ulLoop < ulItemsNum; ulLoop++)
    {
        if ( (g_astClckClassServiceExtTbl[ulLoop].enServiceType == pstBs->BsType)
          && (g_astClckClassServiceExtTbl[ulLoop].enServiceCode == pstBs->BsServiceCode) )
        {
            return g_astClckClassServiceExtTbl[ulLoop].enClass;
        }
    }

    return AT_UNKNOWN_CLCK_CLASS;
}
TAF_UINT32 At_SsRspCusdProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent
)
{
    TAF_UINT32                          ulResult;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if(TAF_SS_EVT_ERROR == pEvent->SsEvent)
    {
        /* 本地发生错误: 清除+CUSD状态 */
        ulResult          = At_ChgTafErrorCode(ucIndex,pEvent->ErrorCode);       /* 发生错误 */
    }
    else
    {
        /* 先报OK再发网络字符串 */
        ulResult          = AT_OK;
    }

    At_FormatResultData(ucIndex,ulResult);

    return ulResult;
}
TAF_VOID At_SsRspInterrogateCnfClipProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    TAF_UINT8                           ucTmp    = 0;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* +CLIP: <n>,<m> */
    if(1 == pEvent->OP_SsStatus)    /* 查到状态 */
    {
        ucTmp = (TAF_SS_ACTIVE_STATUS_MASK & pEvent->SsStatus);
    }
    else    /* 没有查到状态 */
    {
        ucTmp = 2;
    }

    *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                         "%s: %d,%d",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                         pstSsCtx->ucClipType,
                                         ucTmp);

    *pulResult = AT_OK;
}
TAF_VOID At_SsRspInterrogateCnfColpProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    TAF_UINT8                           ucTmp    = 0;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    if(1 == pEvent->OP_SsStatus)    /* 查到状态 */
    {
        ucTmp = (TAF_SS_ACTIVE_STATUS_MASK & pEvent->SsStatus);
    }
    else    /* 没有查到状态 */
    {
        ucTmp = 2;
    }

    *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                         "%s: %d,%d",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                         pstSsCtx->ucColpType,
                                         ucTmp);

    *pulResult = AT_OK;

}


TAF_VOID At_SsRspInterrogateCnfClirProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    TAF_UINT8                           ucTmp    = 0;
    TAF_SS_CLI_RESTRICION_OPTION        ucClirTmp ;
    TAF_UINT8                           ucCliSsStatus;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    if (1 == pEvent->OP_GenericServiceInfo) /* 查到状态 */
    {
        ucCliSsStatus = TAF_SS_ACTIVE_STATUS_MASK &pEvent->GenericServiceInfo.SsStatus;
        if (ucCliSsStatus)
        {
            if ( 1 == pEvent->GenericServiceInfo.OP_CliStrictOp)
            {
               ucClirTmp = pEvent->GenericServiceInfo.CliRestrictionOp;
               if (TAF_SS_CLI_PERMANENT == ucClirTmp)
               {
                 ucTmp = 1;
               }
               else if (TAF_SS_CLI_TMP_DEFAULT_RESTRICTED == ucClirTmp)
               {
                 ucTmp = 3;
               }
               else if (TAF_SS_CLI_TMP_DEFAULT_ALLOWED == ucClirTmp)
               {
                 ucTmp = 4;
               }
               else
               {
                 ucTmp = 2;
               }
            }
            else
            {
               ucTmp = 2;
            }
        }
        else
        {
            ucTmp = 0;
        }
    }
    else if (1 == pEvent->OP_SsStatus)
    {
        ucTmp = 0;
    }
    else /* 没有查到状态 */
    {
        ucTmp = 2;
    }

    *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                         "%s: %d,%d",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                         pstSsCtx->ucClirType,
                                         ucTmp);

    *pulResult = AT_OK;

}


TAF_VOID At_SsRspInterrogateCnfClckProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    TAF_UINT8                           ucTmp    = 0;
    TAF_UINT32                          i;
    VOS_UINT32                          ulCustomizeFlag;

    /* +CLCK: <status>,<class1> */
    if(1 == pEvent->OP_Error)       /* 需要首先判断错误码 */
    {
        *pulResult = At_ChgTafErrorCode(ucIndex,pEvent->ErrorCode);       /* 发生错误 */
        return;
    }

    if(1 == pEvent->OP_SsStatus)    /* 查到状态 */
    {
        ucTmp = (TAF_SS_ACTIVE_STATUS_MASK & pEvent->SsStatus);
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s: %d,%d",
                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                             ucTmp,
                                             AT_CLCK_PARA_CLASS_ALL);
    }
    else if(1 == pEvent->OP_BsServGroupList)
    {
        for (i=0; i<pEvent->BsServGroupList.ucCnt; i++)
        {
            if (i != 0)
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s",gaucAtCrLf);
            }

            /* 此处用ucTmp保存class，而不是status参数 */
            ucTmp = At_GetClckClassFromBsCode(&pEvent->BsServGroupList.astBsService[i]);
            if (ucTmp != AT_UNKNOWN_CLCK_CLASS)
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: %d,%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName, 1, ucTmp);
            }
            else
            {
                AT_WARN_LOG("+CLCK - Unknown class.");
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName, 1);
                return;
            }
        }
    }
    else    /* 没有查到状态 */
    {
        ucTmp = 0;
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,ucTmp);
    }

    /* 输出网络IE SS-STATUS值给用户 */
    ulCustomizeFlag = AT_GetSsCustomizePara(AT_SS_CUSTOMIZE_CLCK_QUERY);
    if ((VOS_TRUE == pEvent->OP_SsStatus)
     && (VOS_TRUE == ulCustomizeFlag))
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             ",%d",
                                             pEvent->SsStatus);
    }

    *pulResult = AT_OK;

}
TAF_VOID At_SsRspInterrogateCnfCcwaProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    TAF_UINT8                           ucTmp    = 0;
    TAF_UINT32                          i;
    VOS_UINT32                          ulCustomizeFlag;

    /* +CCWA: <status>,<class1> */
    if(1 == pEvent->OP_Error)       /* 需要首先判断错误码 */
    {
        *pulResult = At_ChgTafErrorCode(ucIndex,pEvent->ErrorCode);       /* 发生错误 */
        return;
    }

    if (1 == pEvent->OP_SsStatus)
    {
        /* 状态为激活 */
        ucTmp = (TAF_SS_ACTIVE_STATUS_MASK & pEvent->SsStatus);
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s: %d,%d",
                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                             ucTmp,
                                             AT_CLCK_PARA_CLASS_ALL);
    }
    else if(1 == pEvent->OP_BsServGroupList)
    {
        for (i=0; i<pEvent->BsServGroupList.ucCnt; i++)
        {
            if (i != 0)
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s",gaucAtCrLf);
            }

            /* 此处用ucTmp保存class，而不是status参数 */
            ucTmp = At_GetClckClassFromBsCode(&pEvent->BsServGroupList.astBsService[i]);
            if (ucTmp != AT_UNKNOWN_CLCK_CLASS)
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: %d,%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName, 1, ucTmp);
            }
            else
            {
                AT_WARN_LOG("+CCWA - Unknown class.");
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName, 1);
                return;
            }
        }
    }
    else    /* 状态为未激活 */
    {
        ucTmp = 0;
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,ucTmp);
    }

    /* 输出网络IE SS-STATUS值给用户 */
    ulCustomizeFlag = AT_GetSsCustomizePara(AT_SS_CUSTOMIZE_CCWA_QUERY);
    if ((VOS_TRUE == pEvent->OP_SsStatus)
     && (VOS_TRUE == ulCustomizeFlag))
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             ",%d",
                                             pEvent->SsStatus);
    }

    *pulResult = AT_OK;

}


TAF_VOID At_SsRspInterrogateCcbsCnfProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    VOS_UINT32                          i = 0;

    if (1 == pEvent->OP_GenericServiceInfo)
    {
        if (TAF_SS_PROVISIONED_STATUS_MASK & pEvent->GenericServiceInfo.SsStatus)
        {
            if (1 == pEvent->GenericServiceInfo.OP_CcbsFeatureList)
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Queue of Ccbs requests is: ");
                for (i = 0; i < pEvent->GenericServiceInfo.CcbsFeatureList.ucCnt; i++)
                {
                    if (VOS_TRUE == pEvent->GenericServiceInfo.CcbsFeatureList.astCcBsFeature[i].OP_CcbsIndex)
                    {
                        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s",gaucAtCrLf);
                        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                      "Index:%d",pEvent->GenericServiceInfo.CcbsFeatureList.astCcBsFeature[i].CcbsIndex);
                    }
                }
            }
            else
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Queue of Ccbs is empty");
            }
        }
        else
        {
            *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CCBS not provisioned");
        }
    }
    else if (1 == pEvent->OP_SsStatus)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CCBS not provisioned");
    }
    else
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Unknown");
    }
    *pulResult = AT_OK;

}


TAF_VOID At_SsRspInterrogateCnfCmmiProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
    if(1 == pEvent->OP_Error)       /* 需要首先判断错误码 */
    {
        *pulResult = At_ChgTafErrorCode(ucIndex,pEvent->ErrorCode);       /* 发生错误 */
        return;
    }

    if (AT_CMD_CMMI_QUERY_CLIP == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        if ((1 == pEvent->OP_SsStatus) &&
            (TAF_SS_PROVISIONED_STATUS_MASK & pEvent->SsStatus))
        {
            *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIP provisioned");
        }
        else if (0 == pEvent->OP_SsStatus)
        {
            *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Unknown");
        }
        else
        {
            *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIP not provisioned");
        }

        *pulResult = AT_OK;
    }
    else if (AT_CMD_CMMI_QUERY_CLIR == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        if (1 == pEvent->OP_GenericServiceInfo)
        {
            if (TAF_SS_PROVISIONED_STATUS_MASK & pEvent->GenericServiceInfo.SsStatus)
            {
                if (1 == pEvent->GenericServiceInfo.OP_CliStrictOp)
                {
                    switch (pEvent->GenericServiceInfo.CliRestrictionOp)
                    {
                    case TAF_SS_CLI_PERMANENT:
                        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIR provisioned in permanent mode");
                        break;

                    case TAF_SS_CLI_TMP_DEFAULT_RESTRICTED:
                        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIR temporary mode presentation restricted");
                        break;

                    case TAF_SS_CLI_TMP_DEFAULT_ALLOWED:
                        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIR temporary mode presentation allowed");
                        break;

                    default:
                        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Unknown");
                        break;
                    }
                }
                else
                {
                    *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Unknown");
                }
            }
            else
            {
                *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIR not provisioned");
            }
        }
        else if (1 == pEvent->OP_SsStatus)
        {
            *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"CLIR not provisioned");
        }
        else
        {
            *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"Unknown");
        }
        *pulResult = AT_OK;
    }
    else if ( AT_CMD_SS_INTERROGATE_CCBS == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        At_SsRspInterrogateCcbsCnfProc(ucIndex,pEvent,pulResult,pusLength);
    }
    else
    {
        *pulResult = AT_ERROR;
    }

}

/*****************************************************************************
 Prototype      : At_SsRspInterrogateCnfProc
 Description    : SS中查询结果上报处理
 Input          : pEvent --- 事件内容
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
 1.日    期   : 2008年8月21日
   作    者   : --------
   修改内容  : 问题单号:AT2D04989
*****************************************************************************/
TAF_VOID At_SsRspInterrogateCnfProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT32                          *pulResult,
    TAF_UINT16                          *pusLength
)
{
   /* TAF_UINT8                           ucTmp    = 0;*/

    switch(g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
    {
    case AT_CMD_CLIP:
        At_SsRspInterrogateCnfClipProc(ucIndex, pEvent, pulResult, pusLength);
        break;

    case AT_CMD_COLP:
        At_SsRspInterrogateCnfColpProc(ucIndex, pEvent, pulResult, pusLength);
        break;

    case AT_CMD_CLIR:
        At_SsRspInterrogateCnfClirProc(ucIndex, pEvent, pulResult, pusLength);
        break;

    case AT_CMD_CLCK:
        At_SsRspInterrogateCnfClckProc(ucIndex, pEvent, pulResult, pusLength);
        break;

    case AT_CMD_CCWA:
        At_SsRspInterrogateCnfCcwaProc(ucIndex, pEvent, pulResult, pusLength);
        break;

    case AT_CMD_CCFC:
        /* +CCFC: <status>,<class1>[,<number>,<type>[,<subaddr>,<satype>[,<time>]]] */
        if(1 == pEvent->OP_Error)       /* 需要首先判断错误码 */
        {
            *pulResult = At_ChgTafErrorCode(ucIndex,pEvent->ErrorCode);       /* 发生错误 */
            break;
        }

        *pusLength = (TAF_UINT16)At_CcfcQryReport(pEvent,ucIndex);
        *pulResult = AT_OK;
        break;

    case AT_CMD_CMMI:
        At_SsRspInterrogateCnfCmmiProc(ucIndex, pEvent, pulResult, pusLength);
        break;

    default:
        break;
    }
}



TAF_VOID At_SsRspUssdProc(
    TAF_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent,
    TAF_UINT16                          *pusLength
)
{
    TAF_UINT8                           ucTmp    = 0;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;

    VOS_UINT8                          *pucSystemAppConfig = VOS_NULL_PTR;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstSsCtx = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    /* <m> */
    if(TAF_SS_EVT_USS_NOTIFY_IND == pEvent->SsEvent)
    {
        ucTmp = 0;
    }
    else if(TAF_SS_EVT_USS_REQ_IND == pEvent->SsEvent)
    {
        ucTmp = 1;
    }
    else
    {

        if (SYSTEM_APP_ANDROID == *pucSystemAppConfig)
        {
            ucTmp = 2;
        }
        else if((0 == pEvent->OP_UssdString) && (0 == pEvent->OP_USSData))
        {
            ucTmp = 2;
        }
        else
        {
            ucTmp = 0;
        }

    }


    /* +CUSD: <m>[,<str>,<dcs>] */
    *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"+CUSD: %d",ucTmp);

    /* <str> */
    if((0 == pEvent->OP_Error) && ((1 == pEvent->OP_UssdString) || (1 == pEvent->OP_USSData)) && (1 == pEvent->OP_DataCodingScheme))
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,",\"");

        switch(pstSsCtx->usUssdTransMode)
        {
            case AT_USSD_TRAN_MODE:
                *pusLength += (TAF_UINT16)At_HexString2AsciiNumPrint(AT_CMD_MAX_LEN,
                                                                     (TAF_INT8 *)pgucAtSndCodeAddr,
                                                                     pgucAtSndCodeAddr + *pusLength,
                                                                     pEvent->UssdString.aucUssdStr,
                                                                     pEvent->UssdString.usCnt);
                break;

            case AT_USSD_NON_TRAN_MODE:
                if(TAF_SS_UCS2 == pEvent->DataCodingScheme)     /* DATA:IRA */
                {
                     *pusLength += (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pgucAtSndCodeAddr + *pusLength,pEvent->UssdString.aucUssdStr,pEvent->UssdString.usCnt);
                }
                else                                            /* DATA:UCS2 */
                {
                   if(AT_CSCS_UCS2_CODE == gucAtCscsType)       /* +CSCS:UCS2 */
                   {
                        *pusLength += (TAF_UINT16)At_Ascii2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pgucAtSndCodeAddr + *pusLength,pEvent->UssdString.aucUssdStr,pEvent->UssdString.usCnt);
                   }
                   else
                   {
                        PS_MEM_CPY((TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,pEvent->UssdString.aucUssdStr,pEvent->UssdString.usCnt);
                        *pusLength += pEvent->UssdString.usCnt;
                   }
                }
                break;

            default:
                break;
        }

        /* <dcs> */
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"\",%d",pEvent->DataCodingScheme);
    }
}
VOS_UINT32 AT_GetSsEventErrorCode(
    VOS_UINT8                           ucIndex,
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pEvent)
{
    if (VOS_TRUE == pEvent->OP_SsStatus)
    {
        if ( 0 == (TAF_SS_PROVISIONED_STATUS_MASK & pEvent->SsStatus) )
        {
            /* 返回业务未签约对应的错误码 */
            return AT_CME_SERVICE_NOT_PROVISIONED;
        }
    }

    return At_ChgTafErrorCode(ucIndex, pEvent->ErrorCode);
}
TAF_VOID At_SsRspProc(TAF_UINT8  ucIndex,TAF_SS_CALL_INDEPENDENT_EVENT_STRU  *pEvent)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    /* CLIP CCWA CCFC CLCK CUSD CPWD */
    if (AT_CMD_CUSD_REQ == gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        ulResult = At_SsRspCusdProc(ucIndex, pEvent);
        return;
    }

    if(TAF_SS_EVT_ERROR == pEvent->SsEvent) /* 如果是ERROR事件，则直接判断错误码 */
    {
        if (TAF_ERR_USSD_NET_TIMEOUT == pEvent->ErrorCode)
        {

            usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               "%s+CUSD: %d%s",
                                               gaucAtCrLf,
                                               AT_CUSD_M_NETWORK_TIMEOUT,
                                               gaucAtCrLf);

            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

            return;
        }

        if (AT_CMD_CURRENT_OPT_BUTT == gastAtClientTab[ucIndex].CmdCurrentOpt )
        {
            return;
        }

        /* 从SS Event中获取用于AT返回的错误码 */
        ulResult = AT_GetSsEventErrorCode(ucIndex, pEvent);

        AT_STOP_TIMER_CMD_READY(ucIndex);
    }
    else
    {
        switch(pEvent->SsEvent)             /* 其它事件 */
        {
        case TAF_SS_EVT_INTERROGATESS_CNF:          /* 查询结果上报 */
            At_SsRspInterrogateCnfProc(ucIndex, pEvent, &ulResult, &usLength);
            AT_STOP_TIMER_CMD_READY(ucIndex);
            break;

        case TAF_SS_EVT_ERASESS_CNF:
        case TAF_SS_EVT_REGISTERSS_CNF:
        case TAF_SS_EVT_ACTIVATESS_CNF:
        case TAF_SS_EVT_DEACTIVATESS_CNF:
        case TAF_SS_EVT_REG_PASSWORD_CNF:
        case TAF_SS_EVT_ERASE_CC_ENTRY_CNF:
            if(0 == pEvent->OP_Error)
            {
                ulResult = AT_OK;
            }
            else
            {
                ulResult = At_ChgTafErrorCode(ucIndex,pEvent->ErrorCode);       /* 发生错误 */
            }
            AT_STOP_TIMER_CMD_READY(ucIndex);
            break;

        case TAF_SS_EVT_USS_NOTIFY_IND:                     /* 通知用户不用进一步操作 */
        case TAF_SS_EVT_USS_REQ_IND:                        /* 通知用户进一步操作 */
        case TAF_SS_EVT_PROCESS_USS_REQ_CNF:                /* 通知用户网络释放 */
        case TAF_SS_EVT_USS_RELEASE_COMPLETE_IND:           /* 通知用户网络释放 */
            At_SsRspUssdProc(ucIndex, pEvent, &usLength);
            break;

        /* Delete TAF_SS_EVT_GET_PASSWORD_IND分支 */
        default:
            return;
        }
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}
TAF_VOID At_SsMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    TAF_SS_CALL_INDEPENDENT_EVENT_STRU *pEvent = TAF_NULL_PTR;
    TAF_UINT8 ucIndex = 0;

    pEvent = (TAF_SS_CALL_INDEPENDENT_EVENT_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_SS_CALL_INDEPENDENT_EVENT_STRU));
    if (TAF_NULL_PTR == pEvent)
    {
        AT_WARN_LOG("At_SsMsgProc Mem Alloc FAILURE");
        return;
    }

    PS_MEM_CPY(pEvent,pData,usLen);

    AT_LOG1("At_SsMsgProc pEvent->ClientId",pEvent->ClientId);
    AT_LOG1("At_SsMsgProc pEvent->SsEvent",pEvent->SsEvent);
    AT_LOG1("At_SsMsgProc pEvent->OP_Error",pEvent->OP_Error);
    AT_LOG1("At_SsMsgProc pEvent->ErrorCode",pEvent->ErrorCode);
    AT_LOG1("At_SsMsgProc pEvent->SsCode",pEvent->SsCode);
    AT_LOG1("At_SsMsgProc pEvent->Cause",pEvent->Cause);

    if(AT_FAILURE == At_ClientIdToUserId(pEvent->ClientId, &ucIndex))
    {
        AT_WARN_LOG("At_SsMsgProc At_ClientIdToUserId FAILURE");
        PS_MEM_FREE(WUEPS_PID_AT, pEvent);
        return;
    }

    if(AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        At_SsIndProc(ucIndex, pEvent);
    }
    else
    {
        AT_LOG1("At_SsMsgProc ucIndex",ucIndex);
        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

        At_SsRspProc(ucIndex,pEvent);
    }

    PS_MEM_FREE(WUEPS_PID_AT, pEvent);

}


TAF_UINT32 At_PhReadCreg(TAF_PH_REG_STATE_STRU  *pPara,TAF_UINT8 *pDst)
{
    TAF_UINT16 usLength = 0;

    if(pPara->CellId.ucCellNum > 0)
    {
        /* lac */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst + usLength,",\"%X%X%X%X\"",
                0x000f & (pPara->usLac >> 12),
                0x000f & (pPara->usLac >> 8),
                0x000f & (pPara->usLac >> 4),
                0x000f & (pPara->usLac >> 0));

        /* ci */
        if (CREG_CGREG_CI_RPT_FOUR_BYTE == gucCiRptByte)
        {
            /* VDF需求: CREG/CGREG的<CI>域以4字节方式上报 */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst + usLength,",\"%X%X%X%X%X%X%X%X\"",
                    0x000f & (pPara->CellId.aulCellId[0] >> 28),
                    0x000f & (pPara->CellId.aulCellId[0] >> 24),
                    0x000f & (pPara->CellId.aulCellId[0] >> 20),
                    0x000f & (pPara->CellId.aulCellId[0] >> 16),
                    0x000f & (pPara->CellId.aulCellId[0] >> 12),
                    0x000f & (pPara->CellId.aulCellId[0] >> 8),
                    0x000f & (pPara->CellId.aulCellId[0] >> 4),
                    0x000f & (pPara->CellId.aulCellId[0] >> 0));
        }
        else
        {
            /* <CI>域以2字节方式上报 */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst + usLength,",\"%X%X%X%X\"",
                    0x000f & (pPara->CellId.aulCellId[0] >> 12),
                    0x000f & (pPara->CellId.aulCellId[0] >> 8),
                    0x000f & (pPara->CellId.aulCellId[0] >> 4),
                    0x000f & (pPara->CellId.aulCellId[0] >> 0));
        }

        if((VOS_TRUE == g_usReportCregActParaFlg) && (TAF_PH_ACCESS_TECH_BUTT > pPara->ucAct))
        {
            /* rat */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pDst + usLength,",%d",pPara->ucAct);
        }
    }

    return usLength;
}

/*****************************************************************************
 函 数 名  : AT_PhSendPinReady
 功能描述  : E5形态时，pin解锁后广播上报应用
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年9月7日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_PhSendPinReady( VOS_VOID )
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
                                      "^CPINNTY:READY");
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)i,pgucAtSndCodeAddr,usLength);
}

/*****************************************************************************
 函 数 名  : AT_PhSendNeedPuk
 功能描述  : 当前pin解锁失败后，E5产品形态需要通知应用
 输入参数  : ucIndex :当前通道index
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年9月7日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_PhSendNeedPuk( VOS_VOID )
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
                                      "^CPINNTY:SIM PUK");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)i,pgucAtSndCodeAddr,usLength);
}


/*****************************************************************************
 函 数 名  : AT_PhSendSimLocked
 功能描述  : 通知当前SIM Locked
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年9月8日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_PhSendSimLocked( VOS_VOID )
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
                                      "^CARDLOCKNTY:SIM LOCKED");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)i,pgucAtSndCodeAddr,usLength);
}
VOS_VOID  AT_PhSendRoaming( VOS_UINT8 ucTmpRoamStatus )
{
    VOS_UINT32                          i;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucRoamStatus;

    ucRoamStatus = ucTmpRoamStatus;

    for ( i=0 ; i<AT_MAX_CLIENT_NUM; i++ )
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
                                     "%s^APROAMRPT:%d%s",
                                     gaucAtCrLf,
                                     ucRoamStatus,
                                     gaucAtCrLf);

    At_SendResultData((VOS_UINT8)i, pgucAtSndCodeAddr, usLength);

}
VOS_VOID AT_GetOnlyGURatOrder(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder
)
{
    TAF_PH_RAT_ORDER_STRU               stRatOrder;
    VOS_UINT32                          i;
    VOS_UINT8                           ucIndex;

    PS_MEM_CPY(&stRatOrder, pstRatOrder, sizeof(stRatOrder));
    ucIndex = 0;

    /* 获取GU模信息 */
    for (i = 0; i < stRatOrder.ucRatOrderNum; i++)
    {
        if ( (TAF_PH_RAT_WCDMA == stRatOrder.aenRatOrder[i])
          || (TAF_PH_RAT_GSM   == stRatOrder.aenRatOrder[i]) )
        {
            pstRatOrder->aenRatOrder[ucIndex] = stRatOrder.aenRatOrder[i];
            ucIndex++;
        }
    }

    pstRatOrder->ucRatOrderNum        = ucIndex;
    pstRatOrder->aenRatOrder[ucIndex] = TAF_PH_RAT_BUTT;

    return;
}



VOS_VOID AT_ReportSysCfgQryCmdResult(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg,
    VOS_UINT8                           ucIndex,
    VOS_UINT16                         *pusLength
)
{
    AT_SYSCFG_RAT_TYPE_ENUM_UINT8       enAccessMode;
    AT_SYSCFG_RAT_PRIO_ENUM_UINT8       enAcqorder;

    /* 从当前接入优先级中提取GU模接入优先级的信息 */
    AT_GetOnlyGURatOrder(&pstSysCfg->stRatOrder);

    enAcqorder   = pstSysCfg->enUserPrio;

    /* 把上报的TAF_PH_RAT_ORDER_STRU结构转换为mode和acqorder*/
    switch (pstSysCfg->stRatOrder.aenRatOrder[0])
    {
        case TAF_PH_RAT_GSM:
            if (VOS_TRUE == AT_IsSupportWMode(&pstSysCfg->stRatOrder))
            {
                enAccessMode = AT_SYSCFG_RAT_AUTO;
            }
            else
            {
                enAccessMode = AT_SYSCFG_RAT_GSM;
            }
            break;

        case TAF_PH_RAT_WCDMA:
            if (VOS_TRUE == AT_IsSupportGMode(&pstSysCfg->stRatOrder))
            {
                enAccessMode = AT_SYSCFG_RAT_AUTO;
            }
            else
            {
                enAccessMode = AT_SYSCFG_RAT_WCDMA;
            }
            break;

        default:
            /* 只支持L的情况，syscfg查询与标杆上报一致 */
            enAccessMode    = AT_SYSCFG_RAT_AUTO;

            enAcqorder      = AT_SYSCFG_RAT_PRIO_AUTO;
            break;
    }

    /* 按syscfg查询格式上报^SYSCFG:<mode>,<acqorder>,<band>,<roam>,<srvdomain>*/
    *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                       "%s:",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if ( 0 == pstSysCfg->stGuBand.ulBandHigh)
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                           "%d,%d,%X,%d,%d",
                                           enAccessMode,
                                           enAcqorder,
                                           pstSysCfg->stGuBand.ulBandLow,
                                           pstSysCfg->ucRoam,
                                           pstSysCfg->ucSrvDomain);
    }
    else
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                           "%d,%d,%X%08X,%d,%d",
                                           enAccessMode,
                                           enAcqorder,
                                           pstSysCfg->stGuBand.ulBandHigh,
                                           pstSysCfg->stGuBand.ulBandLow,
                                           pstSysCfg->ucRoam,
                                           pstSysCfg->ucSrvDomain);
    }
    gstAtSendData.usBufLen = *pusLength;

    return;
}


VOS_VOID AT_ConvertSysCfgRatOrderToStr(
    TAF_PH_RAT_ORDER_STRU              *pstRatOrder,
    VOS_UINT8                          *pucAcqOrder
)
{
    VOS_UINT8                          i;
    VOS_UINT8                          *pucAcqOrderBegin = VOS_NULL_PTR;

    pucAcqOrderBegin = pucAcqOrder;

    for (i = 0; i < pstRatOrder->ucRatOrderNum; i++)
    {
        if (TAF_PH_RAT_WCDMA == pstRatOrder->aenRatOrder[i])
        {
             VOS_StrCpy((VOS_CHAR *)pucAcqOrder, "02");
             pucAcqOrder += AT_SYSCFGEX_RAT_MODE_STR_LEN;
        }
        else if (TAF_PH_RAT_GSM == pstRatOrder->aenRatOrder[i])
        {
             VOS_StrCpy((VOS_CHAR *)pucAcqOrder, "01");
             pucAcqOrder += AT_SYSCFGEX_RAT_MODE_STR_LEN;
        }
#if (FEATURE_ON == FEATURE_LTE)
        else if (TAF_PH_RAT_LTE == pstRatOrder->aenRatOrder[i])
        {
             VOS_StrCpy((VOS_CHAR *)pucAcqOrder, "03");
             pucAcqOrder += AT_SYSCFGEX_RAT_MODE_STR_LEN;
        }
#endif
        else
        {
        }
    }

    *pucAcqOrder = '\0';

#if (FEATURE_ON == FEATURE_LTE)
    if ((0 == VOS_StrCmp((VOS_CHAR *)pucAcqOrderBegin, "030201"))
     && (TAF_PH_MAX_GUL_RAT_NUM == pstRatOrder->ucRatOrderNum))
    {
        /* 接入技术的个数为3且接入优先顺序为L->W->G,acqorder上报00 */
        pucAcqOrder = pucAcqOrderBegin;
        VOS_StrCpy((VOS_CHAR *)pucAcqOrder, "00");
        pucAcqOrder += AT_SYSCFGEX_RAT_MODE_STR_LEN;
        *pucAcqOrder = '\0';
    }
#else
    if ((0 == VOS_StrCmp((VOS_CHAR *)pucAcqOrderBegin, "0201"))
     && (TAF_PH_MAX_GU_RAT_NUM == pstRatOrder->ucRatOrderNum))
    {
        /* 接入技术的个数为2且接入优先顺序为W->G,acqorder上报00 */
        pucAcqOrder = pucAcqOrderBegin;
        VOS_StrCpy((VOS_CHAR *)pucAcqOrder, "00");
        pucAcqOrder += AT_SYSCFGEX_RAT_MODE_STR_LEN;
        *pucAcqOrder = '\0';
    }
#endif

    return;
}

VOS_VOID AT_ReportSysCfgExQryCmdResult(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfg,
    VOS_UINT8                           ucIndex,
    VOS_UINT16                         *pusLength
)
{
    VOS_UINT8                            aucAcqorder[7];
    VOS_UINT8                           *pucAcqOrder = VOS_NULL_PTR;

    pucAcqOrder = aucAcqorder;

    /* 把上报的TAF_PH_RAT_ORDER_STRU结构转换为acqorder字符串*/
    AT_ConvertSysCfgRatOrderToStr(&pstSysCfg->stRatOrder, pucAcqOrder);

    /* 按syscfgex查询格式上报^SYSCFGEX: <acqorder>,<band>,<roam>,<srvdomain>,<lteband> */
    *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                       "%s:",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (0 == pstSysCfg->stGuBand.ulBandHigh)
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                           "\"%s\",%X,%d,%d",
                                           pucAcqOrder,
                                           pstSysCfg->stGuBand.ulBandLow,
                                           pstSysCfg->ucRoam,
                                           pstSysCfg->ucSrvDomain);
    }
    else
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                           "\"%s\",%X%08X,%d,%d",
                                           pucAcqOrder,
                                           pstSysCfg->stGuBand.ulBandHigh,
                                           pstSysCfg->stGuBand.ulBandLow,
                                           pstSysCfg->ucRoam,
                                           pstSysCfg->ucSrvDomain);
    }

    if (0 == pstSysCfg->stLBand.ulBandHigh)
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                           ",%X",
                                           pstSysCfg->stLBand.ulBandLow);
    }
    else
    {
         *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                           ",%X%08X",
                                           pstSysCfg->stLBand.ulBandHigh,
                                           pstSysCfg->stLBand.ulBandLow);
    }

    gstAtSendData.usBufLen = *pusLength;

    return;
}




#if(FEATURE_ON == FEATURE_LTE)

VOS_VOID AT_ReportCeregResult(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent,
    VOS_UINT16                         *pusLength
)
{
    VOS_UINT32                          ulRst;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    enModemId = MODEM_ID_0;

    ulRst = AT_GetModemIdFromClient(ucIndex, &enModemId);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG1("AT_ReportCeregResult:Get ModemID From ClientID fail,ClientID=%d", ucIndex);
        return;
    }

    /* 当前平台是否支持LTE*/
    if (VOS_TRUE != AT_IsModemSupportRat(enModemId, TAF_PH_RAT_LTE))
    {
        return;
    }

    pstNetCtx = AT_GetModemNetCtxAddrFromModemId(enModemId);

    if ((AT_CEREG_RESULT_CODE_BREVITE_TYPE == pstNetCtx->ucCeregType)
     && (VOS_TRUE == pstEvent->CurRegSta.OP_PsRegState))
    {
        /* +CEREG: <stat> */
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s%s%d%s",
                                             gaucAtCrLf,
                                             gastAtStringTab[AT_STRING_CEREG].pucText,
                                             pstEvent->CurRegSta.ucPsRegState,
                                             gaucAtCrLf);
    }
    else if ((AT_CEREG_RESULT_CODE_ENTIRE_TYPE == pstNetCtx->ucCeregType)
          && (VOS_TRUE == pstEvent->CurRegSta.OP_PsRegState))

    {

        if ((TAF_PH_REG_REGISTERED_HOME_NETWORK == pstEvent->CurRegSta.ucPsRegState)
         || (TAF_PH_REG_REGISTERED_ROAM == pstEvent->CurRegSta.ucPsRegState))
        {
             /* +CEREG: <stat>[,<lac>,<ci>,[rat]] */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s%s%d", gaucAtCrLf,
                                                 gastAtStringTab[AT_STRING_CEREG].pucText,
                                                 pstEvent->CurRegSta.ucPsRegState);

            /* 与标杆做成一致，GU下只上报+CGREG: <stat> */
            if (TAF_PH_INFO_LTE_RAT == pstEvent->RatType)
            {
                *pusLength += (VOS_UINT16)At_PhReadCreg(&(pstEvent->CurRegSta),pgucAtSndCodeAddr + *pusLength);
            }

            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s", gaucAtCrLf);
        }
        else
        {
            /* +CEREG: <stat> */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                                 (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                 "%s%s%d%s", gaucAtCrLf,
                                                 gastAtStringTab[AT_STRING_CEREG].pucText,
                                                 pstEvent->CurRegSta.ucPsRegState, gaucAtCrLf);
        }
    }
    else
    {

    }

    return;

}
#endif
VOS_VOID AT_RcvMmInfoInd(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    TAF_UINT16                          usLength;

    usLength = 0;

    if (VOS_TRUE == pEvent->OP_MmInfo)
    {
        if ((NAS_MM_INFO_IE_UTLTZ == (pEvent->stMmInfo.ucIeFlg & NAS_MM_INFO_IE_UTLTZ))
         || (NAS_MM_INFO_IE_LTZ == (pEvent->stMmInfo.ucIeFlg & NAS_MM_INFO_IE_LTZ)))
        {
            /* 存在时间信息 */
            usLength += (TAF_UINT16)At_PrintMmTimeInfo(ucIndex,
                                                       pEvent,
                                                       (pgucAtSndCodeAddr + usLength));
        }

        At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
    }

    return;
}
VOS_VOID AT_ReportCgregResult(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent,
    VOS_UINT16                         *pusLength
)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);


    if ((AT_CGREG_RESULT_CODE_BREVITE_TYPE == pstNetCtx->ucCgregType)
     && (VOS_TRUE == pstEvent->CurRegSta.OP_PsRegState))
    {
        /* +CGREG: <stat> */
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s%s%d%s",
                                             gaucAtCrLf,
                                             gastAtStringTab[AT_STRING_CGREG].pucText,
                                             pstEvent->CurRegSta.ucPsRegState,
                                             gaucAtCrLf);
    }
    else if ((AT_CGREG_RESULT_CODE_ENTIRE_TYPE == pstNetCtx->ucCgregType)
          && (VOS_TRUE == pstEvent->CurRegSta.OP_PsRegState))
    {

        if ((TAF_PH_REG_REGISTERED_HOME_NETWORK == pstEvent->CurRegSta.ucPsRegState)
         || (TAF_PH_REG_REGISTERED_ROAM == pstEvent->CurRegSta.ucPsRegState))
        {
          /* +CGREG: <stat>[,<lac>,<ci>,[rat]] */
         *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                              "%s%s%d",
                                              gaucAtCrLf,
                                              gastAtStringTab[AT_STRING_CGREG].pucText,
                                              pstEvent->CurRegSta.ucPsRegState);

         *pusLength += (VOS_UINT16)At_PhReadCreg(&(pstEvent->CurRegSta),pgucAtSndCodeAddr + *pusLength);

         *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                              "%s",gaucAtCrLf);
        }
        else
        {
            /* +CGREG: <stat> */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                  "%s%s%d%s",
                                                  gaucAtCrLf,
                                                  gastAtStringTab[AT_STRING_CGREG].pucText,
                                                  pstEvent->CurRegSta.ucPsRegState,
                                                  gaucAtCrLf);
        }
    }
    else
    {

    }

    return;
}
VOS_VOID AT_ReportCregResult(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent,
    VOS_UINT16                         *pusLength
)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);


    if ((AT_CREG_RESULT_CODE_BREVITE_TYPE == pstNetCtx->ucCregType)
     && (VOS_TRUE == pstEvent->CurRegSta.OP_CsRegState))
    {
        /* +CREG: <stat> */
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                             "%s%s%d%s",
                                             gaucAtCrLf,
                                             gastAtStringTab[AT_STRING_CREG].pucText,
                                             pstEvent->CurRegSta.RegState,
                                             gaucAtCrLf);
    }
    else if ((AT_CREG_RESULT_CODE_ENTIRE_TYPE == pstNetCtx->ucCregType)
          && (VOS_TRUE == pstEvent->CurRegSta.OP_CsRegState))
    {
        if ((TAF_PH_REG_REGISTERED_HOME_NETWORK == pstEvent->CurRegSta.RegState)
        || (TAF_PH_REG_REGISTERED_ROAM == pstEvent->CurRegSta.RegState))
        {
            /* +CREG: <stat>[,<lac>,<ci>,[rat]] */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                  "%s%s%d",
                                                  gaucAtCrLf,
                                                  gastAtStringTab[AT_STRING_CREG].pucText,
                                                  pstEvent->CurRegSta.RegState);

            *pusLength += (VOS_UINT16)At_PhReadCreg(&(pstEvent->CurRegSta),
                                                     (pgucAtSndCodeAddr + *pusLength));

            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                                  "%s",gaucAtCrLf);
        }
        else
        {
            /* +CREG: <stat> */
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + *pusLength,
                                              "%s%s%d%s",
                                              gaucAtCrLf,
                                              gastAtStringTab[AT_STRING_CREG].pucText,
                                              pstEvent->CurRegSta.RegState,
                                              gaucAtCrLf);
        }
    }
    else
    {
    }

    return;
}
VOS_VOID AT_ProcSystemInfoInd(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent
)
{
    VOS_UINT16                          usLength;

    usLength  = 0;

    if (pstEvent->OP_CurRegSta)
    {
        AT_ReportCregResult(ucIndex, pstEvent, &usLength);

        AT_ReportCgregResult(ucIndex, pstEvent, &usLength);

#if(FEATURE_ON == FEATURE_LTE)
        /* 通过NV判断当前是否支持LTE */
        AT_ReportCeregResult(ucIndex, pstEvent, &usLength);
#endif
        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }

    return;
}
VOS_VOID AT_ProcModeChangeInd(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_ProcModeChangeInd: Get modem id fail.");
        return;
    }


    /* checkstatus和判断全局变量是否上报的处理都放到C核来做 */
    if (VOS_TRUE == pstEvent->OP_Mode)
    {

        /* ^Mode命令处理，需要将L模显示为W模式,与SYSINFO保持一致 */
        if (TAF_PH_INFO_LTE_RAT == pstEvent->RatType)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s%s%d,%d%s",
                                               gaucAtCrLf,
                                               gastAtStringTab[AT_STRING_MODE].pucText,
                                               TAF_PH_INFO_WCDMA_RAT,
                                               TAF_SYS_SUBMODE_WCDMA,
                                               gaucAtCrLf);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s%s%d,%d%s",
                                               gaucAtCrLf,
                                               gastAtStringTab[AT_STRING_MODE].pucText,
                                               pstEvent->RatType,
                                               pstEvent->ucSysSubMode,
                                               gaucAtCrLf);
        }

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }

    /* 此处全局变量维护不再准确，需要删除，用时从C核获取 */

    return;
}
VOS_VOID AT_ProcUsimInfoInd(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_ProcUsimInfoInd: Get modem id fail.");
        return;
    }



    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s^SIMST:%d,%d%s",
                                       gaucAtCrLf,
                                       pstEvent->SimStatus,
                                       pstEvent->MeLockStatus,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}
VOS_VOID At_RcvMmaPsInitResultIndProc(
    TAF_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength       = 0;

    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_RcvMmaPsInitResultIndProc: Get modem id fail.");
        return;
    }

    if (VOS_FALSE == pEvent->OP_PsInitRslt)
    {
        AT_ERR_LOG("At_RcvMmaPsInitResultIndProc: invalid msg.");
        return;
    }

    /* 只有modem初始化成功才调用底软接口操作 */
    if (VOS_TRUE == pEvent->ulPsInitRslt)
    {
        /* 拉GPIO管脚通知AP侧MODEM已经OK */
        DRV_OS_STATUS_SWITCH(VOS_TRUE);

        AT_SetModemState(enModemId);
    }
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s^PSINIT: %d%s",
                                       gaucAtCrLf,
                                       pEvent->ulPsInitRslt,
                                       gaucAtCrLf);

    At_SendResultData((VOS_UINT8)ucIndex, pgucAtSndCodeAddr, usLength);
}



VOS_VOID AT_ProcServiceStatusInd(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_ProcServiceStatusInd: Get modem id fail.");
        return;
    }

    if (1 == pstEvent->OP_Srvst)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_SRVST].pucText,
                                           pstEvent->ServiceStatus,
                                           gaucAtCrLf);
    }

    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);

    return;
}


TAF_VOID At_PhIndProc(TAF_UINT8 ucIndex, TAF_PHONE_EVENT_INFO_STRU *pEvent)
{
    switch(pEvent->PhoneEvent)
    {
        case TAF_PH_EVT_SYSTEM_INFO_IND:
            AT_ProcSystemInfoInd(ucIndex, pEvent);
            return;

        case TAF_PH_EVT_MODE_CHANGE_IND:
            AT_ProcModeChangeInd(ucIndex, pEvent);
            return;

        case TAF_PH_EVT_USIM_INFO_IND:
            AT_ProcUsimInfoInd(ucIndex, pEvent);
            return;

        case TAF_MMA_EVT_PS_INIT_RESULT_IND:
            At_RcvMmaPsInitResultIndProc(ucIndex, pEvent);
            return;

        case TAF_PH_EVT_SERVICE_STATUS_IND:
            AT_ProcServiceStatusInd(ucIndex, pEvent);
            return;

        case TAF_PH_EVT_RSSI_CHANGE_IND:
            AT_RcvMmaRssiChangeInd(ucIndex, pEvent);
            return;

        case TAF_PH_EVT_OPER_MODE_IND:
            AT_NORM_LOG("At_PhIndProc TAF_PH_EVT_OPER_MODE_IND Do nothing");
            return;

        case TAF_PH_EVT_MM_INFO_IND:
            AT_RcvMmInfoInd(ucIndex, pEvent);
            return;

        case MN_PH_EVT_SIMLOCKED_IND:
            AT_PhSendSimLocked();
            break;

        case MN_PH_EVT_ROAMING_IND:
            AT_PhSendRoaming( pEvent->ucRoamStatus );
            break;

        case TAF_PH_EVT_PLMN_CHANGE_IND:
            At_RcvMmaPlmnChangeIndProc(ucIndex, pEvent);
            break;

        case TAF_PH_EVT_NSM_STATUS_IND:
            AT_RcvMmaNsmStatusInd(ucIndex, pEvent);
            break;

        case TAF_MMA_EVT_AC_INFO_CHANGE_IND:
            AT_RcvMmaAcInfoChangeInd(ucIndex, pEvent);
            break;

        default:
            AT_WARN_LOG("At_PhIndProc Other PhoneEvent");
            return;
    }
}
VOS_VOID AT_PhnEvtTestSyscfg(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    if (0 == pEvent->OP_SysCfgBandGroup)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex,AT_ERROR);
        return;
    }

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /* SYSCFG */
    if (AT_CMD_SYSCFG_TEST == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        if (AT_ROAM_FEATURE_OFF == pstNetCtx->ucRoamFeature)
        {
            gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            "%s: (2,13,14,16),(0-3),%s,(0-2),(0-4)",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            pEvent->strSysCfgBandGroup);
        }
        else
        {
            gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            "%s: (2,13,14,16),(0-3),%s,(0-3),(0-4)",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            pEvent->strSysCfgBandGroup);
        }

    }
    /* SYSCFGEX */
    else
    {
        if (AT_ROAM_FEATURE_OFF == pstNetCtx->ucRoamFeature)
        {
            gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            "%s: (\"00\",\"01\",\"02\",\"03\",\"030201\",\"030102\",\"020301\",\"020103\",\"010203\",\"010302\",\"0302\",\"0301\",\"0201\",\"0203\",\"0102\",\"0103\",\"99\"),%s,(0-2),(0-4),((7fffffffffffffff,\"All bands\"))",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            pEvent->strSysCfgBandGroup);
        }
        else
        {
            gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            "%s: (\"00\",\"01\",\"02\",\"03\",\"030201\",\"030102\",\"020301\",\"020103\",\"010203\",\"010302\",\"0302\",\"0301\",\"0201\",\"0203\",\"0102\",\"0103\",\"99\"),%s,(0-2),(0-4),((7fffffffffffffff,\"All bands\"))",
                                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                             pEvent->strSysCfgBandGroup);
        }
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    At_FormatResultData(ucIndex,AT_OK);

    return;

}

VOS_VOID AT_PhnEvtTestCpol(
    TAF_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    VOS_UINT32       ulResult;

    ulResult               = AT_ERROR;
    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (1 == pEvent->OP_PrefPlmnList)
    {
        if ( 0 == pEvent->ulPrefPlmnNum )
        {
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
        }
        else
        {
            gstAtSendData.usBufLen = (TAF_UINT16)VOS_sprintf((VOS_CHAR*)pgucAtSndCodeAddr,"%s:(1-%d),(0-2)",
                                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                             pEvent->ulPrefPlmnNum);

            ulResult = AT_OK;
        }
    }

    At_FormatResultData(ucIndex,ulResult);

}

VOS_VOID AT_PhnEvtSetPrefPlmnType(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /*当前正在等待异步消息的正是 TAF_MSG_MMA_SET_PREF_PLMN_TYPE */
    if (AT_CMD_MMA_SET_PREF_PLMN_TYPE == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        gstAtSendData.usBufLen    = 0;
        pstNetCtx->enPrefPlmnType = pEvent->ucPrefPlmnType;

        AT_STOP_TIMER_CMD_READY(ucIndex);

        At_FormatResultData(ucIndex,AT_OK);
    }

    return;
}
VOS_VOID AT_PhnEvtSetMtPowerDown(
    VOS_UINT8 ucIndex,
    TAF_PHONE_EVENT_INFO_STRU  *pEvent
)
{
    VOS_UINT32       ulResult = AT_ERROR;

    gstAtSendData.usBufLen = 0;

    /*当前正在等待异步消息的正是TAF_MSG_MMA_MT_POWER_DOWN*/
    if (AT_CMD_MMA_MT_POWER_DOWN == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        if (TAF_ERR_NO_ERROR == pEvent->OP_PhoneError)
        {
            ulResult = AT_OK;
        }
    }

    At_FormatResultData(ucIndex,ulResult);
}
VOS_VOID AT_PhnEvtGetDomainAttachState(
    VOS_UINT8 ucIndex,
    TAF_PHONE_EVENT_INFO_STRU  *pEvent
)
{
    /*当前正在等待异步消息的不是AT_MSG_MMA_GET_CURRENT_ATTACH_STATUS*/
    if (AT_CMD_MMA_GET_CURRENT_ATTACH_STATUS != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return;
    }

    /* 参数错误或上报的事件错误 */
    if ((VOS_NULL_PTR == pEvent)
     || (1 != pEvent->OP_DomainAttachState))
    {
        return;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_PH_PS_CS_OPERATE == pEvent->stDomainAttachState.ucDomain)
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_UINT32)pEvent->stDomainAttachState.ucPsState,
                                                        (VOS_UINT32)pEvent->stDomainAttachState.ucCsState);

        At_FormatResultData(ucIndex,AT_OK);
    }
    else if (TAF_PH_PS_OPERATE == pEvent->stDomainAttachState.ucDomain)
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_UINT32)pEvent->stDomainAttachState.ucPsState);

        At_FormatResultData(ucIndex,AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex,AT_ERROR);
    }

    return;
}



AT_RRETURN_CODE_ENUM_UINT32 AT_PhnEvtPlmnList(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent
)
{
    AT_RRETURN_CODE_ENUM_UINT32         enResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucTmp;
    TAF_PH_OPERATOR_NAME_FORMAT_OP_STRU stAtOperName;

    /* 变量初始化 */
    usLength = 0;
    enResult = AT_ERROR;

    /* 错误时，把Taf上报的错误码转换成AT的错误码 */
    if ( VOS_TRUE == pstEvent->OP_PhoneError )  /* 错误 */
    {
        enResult = (AT_RRETURN_CODE_ENUM_UINT32)At_ChgTafErrorCode(ucIndex, pstEvent->PhoneError);
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s: ",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        for ( ucTmp = 0; ucTmp < pstEvent->PlmnList.ucPlmnNum; ucTmp++ )
        {
            PS_MEM_SET(&stAtOperName, 0x00, sizeof(stAtOperName));

            /* 除第一项外，其它以前要加逗号 */
            if ( 0 != ucTmp )
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  ",");
            }
            /* 显示上报的列表搜结果 */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "(%d",
                                               pstEvent->PlmnList.PlmnInfo[ucTmp].PlmnStatus);

            stAtOperName.OperName.PlmnId.Mcc = pstEvent->PlmnList.Plmn[ucTmp].Mcc;
            stAtOperName.OperName.PlmnId.Mnc = pstEvent->PlmnList.Plmn[ucTmp].Mnc;
            stAtOperName.ListOp              = TAF_PH_OPER_NAME_PLMN2ALPHANUMERICNAME;

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"\"");

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"\"");

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"%X%X%X",
                                               (0x0f00 & pstEvent->PlmnList.Plmn[ucTmp].Mcc) >> 8,
                                               (0x00f0 & pstEvent->PlmnList.Plmn[ucTmp].Mcc) >> 4,
                                               (0x000f & pstEvent->PlmnList.Plmn[ucTmp].Mcc));

            if ( 0x0F != ((0x0f00 & pstEvent->PlmnList.Plmn[ucTmp].Mnc) >> 8) )
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%X",
                                                   (0x0f00 & pstEvent->PlmnList.Plmn[ucTmp].Mnc) >> 8);

            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%X%X\"",
                                               (0x00f0 & pstEvent->PlmnList.Plmn[ucTmp].Mnc) >> 4,
                                               (0x000f & pstEvent->PlmnList.Plmn[ucTmp].Mnc));

            /* 接入技术为GSM时,显示数字0 */
            if ( TAF_PH_RA_GSM == pstEvent->PlmnList.PlmnInfo[ucTmp].RaMode ) /* GSM */
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  ",0");
            }
            /* 接入技术为WCDMA时,显示数字2 */
            else if ( TAF_PH_RA_WCDMA == pstEvent->PlmnList.PlmnInfo[ucTmp].RaMode ) /* CDMA */
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  ",2");
            }
            /* 接入技术为LTE时,显示数字7 */
            else if ( TAF_PH_RA_LTE == pstEvent->PlmnList.PlmnInfo[ucTmp].RaMode )   /* LTE */
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  ",7");
            }
            else
            {

            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ")");
        }

        /* 列表个数为0时，显示各个参数的取值范围 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",,(0,1,2,3,4),(0,1,2)");
        /* 返回AT_OK */
        enResult = AT_OK;
    }

    return enResult;
}
VOS_VOID    At_QryCpinRspProc(
    VOS_UINT8       ucIndex,
    TAF_PH_PIN_TYPE ucPinType,
    VOS_UINT16     *pusLength
)
{
    if(TAF_SIM_PIN == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"SIM PIN");
    }
    else if(TAF_SIM_PUK == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"SIM PUK");
    }
    else if(TAF_PHNET_PIN == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"PH-NET PIN");
    }
    else if(TAF_PHNET_PUK == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"PH-NET PUK");
    }
    else if(TAF_PHNETSUB_PIN == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"PH-NETSUB PIN");
    }
    else if(TAF_PHNETSUB_PUK == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"PH-NETSUB PUK");
    }
    else if(TAF_PHSP_PIN == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"PH-SP PIN");
    }
    else if(TAF_PHSP_PUK == ucPinType)
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"PH-SP PUK");
    }
    else
    {
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        *pusLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + *pusLength,"READY");
    }

    return;
}

#if(FEATURE_ON == FEATURE_LTE)

TAF_UINT32 AT_ProcOperModeWhenLteOn(VOS_UINT8 ucIndex)
{
    return atSetTmodePara(ucIndex, g_stAtDevCmdCtrl.ucCurrentTMode);
}
#endif

TAF_VOID At_PhRspProc(TAF_UINT8 ucIndex,TAF_PHONE_EVENT_INFO_STRU  *pEvent)
{
    TAF_UINT32                          ulResult;
    TAF_UINT16                          usLength;
    TAF_UINT8                           ucTmp;
    VOS_BOOL                            bNeedRptPinReady;
    VOS_BOOL                            bNeedRptNeedPuk;
    TAF_UINT32                          ulRst;
    MODEM_ID_ENUM_UINT16                enModemId;
#if(FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                           ucSptLteFlag;
    VOS_UINT8                           ucSptUtralTDDFlag;
#endif

    enModemId           = MODEM_ID_0;
    bNeedRptPinReady    = VOS_FALSE;
    bNeedRptNeedPuk     = VOS_FALSE;
    ulResult            = AT_FAILURE;
    usLength            = 0;
    ucTmp               = 0;

    ulRst = AT_GetModemIdFromClient(ucIndex, &enModemId);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG1("At_PhRspProc:Get ModemID From ClientID fail,ClientID=%d", ucIndex);
        return;
    }

    switch(pEvent->PhoneEvent)
    {
    case TAF_PH_EVT_ERR:
        ulResult = At_ChgTafErrorCode(ucIndex,pEvent->PhoneError);       /* 发生错误 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;

    case TAF_PH_EVT_PLMN_LIST_CNF:


        /* 处理列表搜结果上报事件 */
        ulResult = (TAF_UINT32)AT_PhnEvtPlmnList(ucIndex, pEvent);

        AT_STOP_TIMER_CMD_READY(ucIndex);

        break;

    case TAF_PH_EVT_PLMN_LIST_REJ:
        ulResult = AT_CME_OPERATION_NOT_ALLOWED;
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;

    case TAF_PH_EVT_PLMN_SEL_CNF:
    case TAF_PH_EVT_PLMN_RESEL_CNF:
        if(1 == pEvent->OP_PhoneError)  /* 错误 */
        {
            ulResult = At_ChgTafErrorCode(ucIndex,pEvent->PhoneError);       /* 发生错误 */
        }
        else
        {
            ulResult = AT_OK;
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;

    case TAF_PH_EVT_ATTACH_CNF:
    case TAF_PH_EVT_PS_ATTACH_CNF:
    case TAF_PH_EVT_CS_ATTACH_CNF:
    case TAF_PH_EVT_DETACH_CNF:
    case TAF_PH_EVT_PS_DETACH_CNF:
    case TAF_PH_EVT_CS_DETACH_CNF:
        if(1 == pEvent->OP_PhoneError)  /* 错误 */
        {
            gastAtClientTab[ucIndex].ulCause = pEvent->PhoneError;
            ulResult = AT_ERROR;        /* 按照协议，只报告错误，不报原因 */
        }
        else
        {
            ulResult = AT_OK;
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    /* 处理域注册状态上报消息, */
    case TAF_PH_EVT_GET_CURRENT_ATTACH_STATUS_CNF:
        AT_PhnEvtGetDomainAttachState(ucIndex,pEvent);
        return;
    /*处理下电回复消息*/
    case TAF_PH_EVT_MT_POWER_DOWN_CNF:
        AT_PhnEvtSetMtPowerDown(ucIndex,pEvent);
        return;
    case TAF_PH_EVT_SET_PREF_PLMN_TYPE_CNF:
        AT_PhnEvtSetPrefPlmnType(ucIndex,pEvent);
        return;
    case TAF_PH_EVT_TEST_PREF_PLMN_CNF:
        /*1.对+CPOL命令的测试结果进行输出。*/
        AT_PhnEvtTestCpol(ucIndex,pEvent);
        return;
    case TAF_PH_EVT_TEST_SYSCFG_CNF:
        /*1.对^syscfg命令的测试结果进行输出。*/
        AT_PhnEvtTestSyscfg(ucIndex,pEvent);
        return;
    case TAF_PH_EVT_SET_PREF_PLMN_CNF:
        /* 1.AT+CPLS 设置命令回复消息处理，
        从TAF_PHONE_ERROR结构中获取结果，格式化上报的数据，并向端口打印 */
        break;

    case TAF_PH_EVT_OP_PIN_CNF:
        if(1 == pEvent->OP_PhoneError)  /* MT本地错误 */
        {
            ulResult = At_ChgTafErrorCode(ucIndex,pEvent->PhoneError);       /* 发生错误 */
            if ( ( AT_CME_SIM_PUK_REQUIRED == ulResult )
              && (TAF_PIN_VERIFY == pEvent->PinCnf.CmdType))
            {
                bNeedRptNeedPuk = VOS_TRUE;
            }
        }
        else
        {
            if(TAF_PH_OP_PIN_OK == pEvent->PinCnf.OpPinResult)   /* USIMM返回错误 */
            {
                switch(pEvent->PinCnf.CmdType)
                {
                case TAF_PIN_QUERY:
                    /* AT+CLCK */
                    if(AT_CMD_CLCK == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                        if(TAF_PH_USIMM_ENABLE == pEvent->PinCnf.QueryResult.UsimmEnableFlg)
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"1");
                        }
                        else
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"0");
                        }
                        ulResult = AT_OK;
                    }

                    /* AT^CPIN */
                    else if(AT_CMD_CPIN_2 == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
                    {
                        if(TAF_SIM_PIN == pEvent->PinCnf.PinType)
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"SIM PIN,%d,%d,%d,%d,%d",\
                                                            pEvent->PinCnf.RemainTime.ucPin1RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPuk1RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPin1RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPuk2RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPin2RemainTime);
                        }
                        else if(TAF_SIM_PUK == pEvent->PinCnf.PinType)
                        {

                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"SIM PUK,%d,%d,%d,%d,%d",\
                                                                pEvent->PinCnf.RemainTime.ucPuk1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPuk1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPin1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPuk2RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPin2RemainTime);
                        }
                        else
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"READY,,%d,%d,%d,%d",\
                                                                pEvent->PinCnf.RemainTime.ucPuk1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPin1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPuk2RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPin2RemainTime);

                        }
                    }
                    /*AT+CPIN*/
                    else
                    {
                        At_QryCpinRspProc(ucIndex, pEvent->PinCnf.PinType, &usLength);
                    }

                    ulResult = AT_OK;
                    break;

                case TAF_PIN2_QUERY:
                    /* AT^CPIN2 */
                    if(AT_CMD_CPIN2 == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
                    {

                        if(TAF_SIM_PIN2 == pEvent->PinCnf.PinType)
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"SIM PIN2,%d,%d,%d,%d,%d",\
                                                            pEvent->PinCnf.RemainTime.ucPin2RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPuk1RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPin1RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPuk2RemainTime,\
                                                            pEvent->PinCnf.RemainTime.ucPin2RemainTime);
                        }
                        else if(TAF_SIM_PUK2 == pEvent->PinCnf.PinType)
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"SIM PUK2,%d,%d,%d,%d,%d",\
                                                                pEvent->PinCnf.RemainTime.ucPuk2RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPuk1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPin1RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPuk2RemainTime,\
                                                                pEvent->PinCnf.RemainTime.ucPin2RemainTime);
                        }
                        else
                        {
                            ulResult = AT_CME_SIM_FAILURE;
                            break;
                        }
                    }
                    else
                    {
                            ulResult = AT_ERROR;
                            break;
                    }
                    ulResult = AT_OK;
                    break;
                case TAF_PIN_VERIFY:
                case TAF_PIN_UNBLOCK:
                    if(TAF_SIM_NON == pEvent->PinCnf.PinType)
                    {
                        ulResult = AT_ERROR;
                    }
                    else
                    {
                        ulResult = AT_OK;

                        bNeedRptPinReady = VOS_TRUE;
                    }
                    break;

                case TAF_PIN_CHANGE:
                case TAF_PIN_DISABLE:
                case TAF_PIN_ENABLE:
                    ulResult = AT_OK;
                    break;

                default:
                    return;
                }
            }
            else
            {
                switch(pEvent->PinCnf.OpPinResult)
                {
                    case TAF_PH_OP_PIN_NEED_PIN1:
                        ulResult = AT_CME_SIM_PIN_REQUIRED;
                        break;

                    case TAF_PH_OP_PIN_NEED_PUK1:
                        bNeedRptNeedPuk = VOS_TRUE;
                        ulResult = AT_CME_SIM_PUK_REQUIRED;
                        break;

                    case TAF_PH_OP_PIN_NEED_PIN2:
                        ulResult = AT_CME_SIM_PIN2_REQUIRED;
                        break;

                    case TAF_PH_OP_PIN_NEED_PUK2:
                        ulResult = AT_CME_SIM_PUK2_REQUIRED;
                        break;
                    case TAF_PH_OP_PIN_INCORRECT_PASSWORD:
                        ulResult = AT_CME_INCORRECT_PASSWORD;
                        break;
                    case TAF_PH_OP_PIN_OPERATION_NOT_ALLOW:
                        ulResult = AT_CME_OPERATION_NOT_ALLOWED;
                        break;
                    case TAF_PH_OP_PIN_SIM_FAIL:
                        ulResult = AT_CME_SIM_FAILURE;
                        break;
                    default:
                        ulResult = AT_CME_UNKNOWN;
                        break;
                }
            }
        }

        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;

    case TAF_PH_EVT_OPER_MODE_CNF:
        if(1 == pEvent->OP_PhoneError)  /* MT本地错误 */
        {
            ulResult = At_ChgTafErrorCode(ucIndex,pEvent->PhoneError);       /* 发生错误 */
        }
        else if( TAF_PH_CMD_QUERY == pEvent->OperMode.CmdType)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",pEvent->OperMode.PhMode);
            ulResult = AT_OK;
        }
        else
        {
            ulResult = AT_OK;
        }

        if (TAF_PH_MODE_FULL == pEvent->OperMode.PhMode)
        {
            IMM_MntnAcpuCheckPoolLeak();
        }

        /* V7R2 ^PSTANDBY命令复用关机处理流程 */
#ifdef FEATURE_UPGRADE_TL
        if(AT_CMD_PSTANDBY_SET == (AT_LTE_CMD_CURRENT_OPT_ENUM)gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            return;
        }
#endif

#if((FEATURE_ON == FEATURE_LTE) || (FEATURE_ON == FEATURE_UE_MODE_TDS))
        /* 如果GU处理结果正确，则发送到TL测并等待结果 */
        if (ulResult == AT_OK)
        {
            ucSptLteFlag = AT_IsModemSupportRat(enModemId, TAF_PH_RAT_LTE);
            ucSptUtralTDDFlag = AT_IsModemSupportUtralTDDRat(enModemId);

            if ((VOS_TRUE == ucSptLteFlag)
             || (VOS_TRUE == ucSptUtralTDDFlag))
            {
                if ((AT_CMD_TMODE_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
                 || (AT_CMD_SET_TMODE == gastAtClientTab[ucIndex].CmdCurrentOpt))
                {
                    AT_ProcOperModeWhenLteOn(ucIndex);
                    return;
                }
            }
        }
#endif

        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;

    case TAF_PH_EVT_SYS_CFG_CNF:
        if(1 == pEvent->OP_PhoneError)  /* MT本地错误 */
        {
            ulResult = At_ChgTafErrorCode(ucIndex,pEvent->PhoneError);       /* 发生错误 */
        }
        else if(AT_CMD_SYSCFG_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_ReportSysCfgQryCmdResult(&(pEvent->stSysCfg), ucIndex, &usLength);
            ulResult = AT_OK;
        }
        else if (AT_CMD_SYSCFGEX_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_ReportSysCfgExQryCmdResult(&(pEvent->stSysCfg), ucIndex, &usLength);
            ulResult = AT_OK;
        }
        else
        {
            ulResult = AT_OK;
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);

        break;

    case TAF_PH_EVT_USIM_RESPONSE:
        /* +CSIM:  */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        if(1 == pEvent->OP_UsimAccessData)
        {
            /* <length>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"",pEvent->UsimAccessData.ucLen * 2);
            /* <command>, */
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,pEvent->UsimAccessData.aucResponse,pEvent->UsimAccessData.ucLen);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);
        ulResult = AT_OK;
        break;

    case TAF_PH_EVT_RESTRICTED_ACCESS_CNF:
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        if(1 == pEvent->OP_UsimRestrictAccess)
        {
            /* <sw1, sw2>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,%d",pEvent->RestrictedAccess.ucSW1, pEvent->RestrictedAccess.ucSW2);

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"");

            if(0 != pEvent->RestrictedAccess.ucLen)
            {
                /* <response> */
                usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,pEvent->RestrictedAccess.aucContent, pEvent->RestrictedAccess.ucLen);
            }

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);
        ulResult = AT_OK;
        break;

    case TAF_PH_EVT_OP_PINREMAIN_CNF:
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%d,%d,%d,%d ",pEvent->PinRemainCnf.ucPIN1Remain,pEvent->PinRemainCnf.ucPUK1Remain,pEvent->PinRemainCnf.ucPIN2Remain,pEvent->PinRemainCnf.ucPUK2Remain);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        ulResult = AT_OK;
        break;

    case TAF_PH_EVT_ME_PERSONALISATION_CNF:
        if (TAF_PH_ME_PERSONALISATION_OK != pEvent->MePersonalisation.OpRslt)
        {
            if (TAF_PH_ME_PERSONALISATION_NO_SIM == pEvent->MePersonalisation.OpRslt)
            {
                ulResult = At_ChgTafErrorCode(ucIndex, TAF_ERR_CMD_TYPE_ERROR);
            }
            else if (TAF_PH_ME_PERSONALISATION_OP_NOT_ALLOW == pEvent->MePersonalisation.OpRslt)
            {
                ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            }
            else if ( TAF_PH_ME_PERSONALISATION_WRONG_PWD == pEvent->MePersonalisation.OpRslt)
            {
                ulResult = AT_CME_INCORRECT_PASSWORD;
            }
            else
            {
                ulResult = AT_ERROR;
            }
        }
        else
        {
            switch(pEvent->MePersonalisation.CmdType)
            {
                case TAF_ME_PERSONALISATION_ACTIVE:
                case TAF_ME_PERSONALISATION_DEACTIVE:
                case TAF_ME_PERSONALISATION_SET:
                case TAF_ME_PERSONALISATION_PWD_CHANGE:
                case TAF_ME_PERSONALISATION_VERIFY:
                    ulResult = AT_OK;
                    break;

                case TAF_ME_PERSONALISATION_QUERY:
                    if ( AT_CMD_CARD_LOCK_READ == gastAtClientTab[ucIndex].CmdCurrentOpt )
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,",pEvent->MePersonalisation.unReportContent.OperatorLockInfo.OperatorLockStatus);
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,",pEvent->MePersonalisation.unReportContent.OperatorLockInfo.RemainTimes);
                        if( (pEvent->MePersonalisation.unReportContent.OperatorLockInfo.OperatorLen < TAF_PH_ME_LOCK_OPER_LEN_MIN)
                            ||(pEvent->MePersonalisation.unReportContent.OperatorLockInfo.OperatorLen > TAF_PH_ME_LOCK_OPER_LEN_MAX))
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"0");
                        }
                        else
                        {
                            for (ucTmp = 0;ucTmp< pEvent->MePersonalisation.unReportContent.OperatorLockInfo.OperatorLen;ucTmp++)
                            {
                                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",pEvent->MePersonalisation.unReportContent.OperatorLockInfo.Operator[ucTmp]);
                            }
                        }
                        ulResult = AT_OK;
                    }
                    else
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                        if(TAF_ME_PERSONALISATION_ACTIVE_STATUS == pEvent->MePersonalisation.ActiveStatus)
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"1");
                        }
                        else
                        {
                            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"0");
                        }
                        ulResult = AT_OK;
                    }
                    break;

                case TAF_ME_PERSONALISATION_RETRIEVE:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                    for (ucTmp = 0; ucTmp < pEvent->MePersonalisation.unReportContent.SimPersionalisationStr.DataLen; ucTmp++)
                    {
                        *(pgucAtSndCodeAddr + usLength + ucTmp) = pEvent->MePersonalisation.unReportContent.SimPersionalisationStr.aucSimPersonalisationStr[ucTmp] + 0x30;
                    }
                    usLength += ucTmp;
                    break;

                default:
                    ulResult = AT_ERROR;
                    break;
            }
        }
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    case TAF_PH_EVT_SETUP_SYSTEM_INFO_RSP:
        AT_NORM_LOG("At_PhRspProc EVT SETUP SYSTEM INFO RSP,Do nothing.");
        return;

    case TAF_PH_EVT_PLMN_LIST_ABORT_CNF:

        /* 容错处理, 当前不在列表搜ABORT过程中则不上报ABORT.
           如AT的ABORT保护定时器已超时, 之后再收到MMA的ABORT_CNF则不上报ABORT */
        if ( AT_CMD_COPS_ABORT_PLMN_LIST != gastAtClientTab[ucIndex].CmdCurrentOpt )
        {
            AT_WARN_LOG("At_PhRspProc  NOT ABORT PLMN LIST. ");
            return;
        }

        ulResult = AT_ABORT;
        AT_STOP_TIMER_CMD_READY(ucIndex);

        break;

    default:
        AT_WARN_LOG("At_PhRspProc Other PhoneEvent");
        return;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

    if ( VOS_TRUE == bNeedRptPinReady )
    {
        AT_PhSendPinReady();
    }

    if ( VOS_TRUE == bNeedRptNeedPuk )
    {
        AT_PhSendNeedPuk();
    }
}
TAF_VOID At_PhEventProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    TAF_PHONE_EVENT_INFO_STRU *pEvent;
    TAF_UINT8 ucIndex = 0;

    pEvent = (TAF_PHONE_EVENT_INFO_STRU *)pData;

    AT_LOG1("At_PhMsgProc pEvent->ClientId",pEvent->ClientId);
    AT_LOG1("At_PhMsgProc PhoneEvent",pEvent->PhoneEvent);
    AT_LOG1("At_PhMsgProc PhoneError",pEvent->PhoneError);

    if(AT_FAILURE == At_ClientIdToUserId(pEvent->ClientId, &ucIndex))
    {
        AT_WARN_LOG("At_PhRspProc At_ClientIdToUserId FAILURE");
        return;
    }

    if(AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        At_PhIndProc(ucIndex, pEvent);
    }
    else
    {
        AT_LOG1("At_PhMsgProc ucIndex",ucIndex);
        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

        At_PhRspProc(ucIndex,pEvent);
    }
}

TAF_VOID At_PhPlmnListProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    TAF_PHONE_EVENT_PLMN_LIST_STRU         *pEvent;
    VOS_UINT16                              usLength;
    VOS_UINT8                               ucTmp;
    VOS_UINT8                               ucIndex;
    TAF_PHONE_EVENT_INFO_STRU          *pstOrigenEvent;

    usLength    = 0;
    pEvent      = (TAF_PHONE_EVENT_PLMN_LIST_STRU*)pData;

    if(AT_FAILURE == At_ClientIdToUserId(pEvent->ClientId,&ucIndex))
    {
        AT_WARN_LOG("At_PhPlmnListProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_PhPlmnListProc : AT_BROADCAST_INDEX.");
        return;
    }

    /* 容错处理, 如当前正处于列表搜ABORT过程中则不上报列表搜结果 */
    if ( AT_CMD_COPS_TEST != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_PhPlmnListProc, TAF_PH_EVT_PLMN_LIST_CNF when Abort Plmn List or timeout. ");
        return;
    }

    pstOrigenEvent = (TAF_PHONE_EVENT_INFO_STRU *)pData;

    /* 如果是失败事件,直接上报ERROR */
    if (1 == pstOrigenEvent->OP_PhoneError)
    {
        At_PhRspProc(ucIndex, pstOrigenEvent);

        return;
    }

    usLength    = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    for(ucTmp = 0; ucTmp < pEvent->ulPlmnNum; ucTmp++)
    {
        if(0 != ucTmp)  /* 除第一项外，其它以前要加逗号 */
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",");
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"(%d",pEvent->astPlmnInfo[ucTmp].PlmnStatus);

        if (( '\0' == pEvent->astPlmnName[ucTmp].aucOperatorNameLong[0] )
         || ( '\0' == pEvent->astPlmnName[ucTmp].aucOperatorNameShort[0] ))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"\"");
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"\"");
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"",pEvent->astPlmnName[ucTmp].aucOperatorNameLong);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"",pEvent->astPlmnName[ucTmp].aucOperatorNameShort);
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%X%X%X",
            (0x0f00 & pEvent->astPlmnName[ucTmp].PlmnId.Mcc) >> 8,
            (0x00f0 & pEvent->astPlmnName[ucTmp].PlmnId.Mcc) >> 4,
            (0x000f & pEvent->astPlmnName[ucTmp].PlmnId.Mcc)
            );

        if(0x0F != ((0x0f00 & pEvent->astPlmnName[ucTmp].PlmnId.Mnc) >> 8))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%X",
            (0x0f00 & pEvent->astPlmnName[ucTmp].PlmnId.Mnc) >> 8
            );

        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%X%X\"",
            (0x00f0 & pEvent->astPlmnName[ucTmp].PlmnId.Mnc) >> 4,
            (0x000f & pEvent->astPlmnName[ucTmp].PlmnId.Mnc)
            );
        if(TAF_PH_RA_GSM == pEvent->astPlmnInfo[ucTmp].RaMode)  /* GSM */
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",0");
        }
        else if(TAF_PH_RA_WCDMA == pEvent->astPlmnInfo[ucTmp].RaMode)     /* CDMA */
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",2");
        }
        else if(TAF_PH_RA_LTE == pEvent->astPlmnInfo[ucTmp].RaMode)   /* LTE */
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",7");
        }
        else
        {

        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,")");
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",,(0,1,2,3,4),(0,1,2)");

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,AT_OK);

}


TAF_VOID At_PhMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    TAF_PHONE_EVENT         ucPhoneEvent;

    ucPhoneEvent = *(TAF_PHONE_EVENT*)pData;

    if (TAF_PH_EVT_PLMN_LIST_CNF == ucPhoneEvent)
    {
        At_PhPlmnListProc(pData,usLen);
    }
    else if (TAF_MMA_EVT_REG_REJ_INFO_IND == ucPhoneEvent)
    {
        AT_PhRegRejInfoProc(pData,usLen);
    }

    else if (TAF_MMA_EVT_PLMN_SELECTION_INFO_IND == ucPhoneEvent)
    {
        AT_PhPlmnSelectInfoProc(pData, usLen);
    }
    else if (TAF_MMA_EVT_USER_CFG_OPLMN_QRY_CNF == ucPhoneEvent)
    {
        AT_PhEOPlmnQueryCnfProc(pData);
    }
    else if (TAF_MMA_EVT_USER_CFG_OPLMN_SET_CNF == ucPhoneEvent)
    {
        AT_PhEOPlmnSetCnfProc(pData);
    }
    else if ( TAF_MMA_EVT_NET_SCAN_CNF == ucPhoneEvent )
    {
        AT_PhNetScanCnfProc(pData, usLen);
    }
    else
    {
        At_PhEventProc(pData,usLen);
    }
}


TAF_UINT32 At_Unicode2UnicodePrint(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usLen    = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucHigh1  = 0;
    TAF_UINT8  ucHigh2  = 0;
    TAF_UINT8  ucLow1   = 0;
    TAF_UINT8  ucLow2   = 0;
    TAF_UINT8 *pWrite   = pucDst;
    TAF_UINT8 *pRead    = pucSrc;

    if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + (2 * usSrcLen)) >= MaxLength)
    {
        AT_ERR_LOG("At_Unicode2UnicodePrint too long");
        return 0;
    }

    /* 扫完整个字串 */
    while( usChkLen < usSrcLen )
    {
        /* 第一个字节 */
        ucHigh1 = 0x0F & (*pRead >> 4);
        ucHigh2 = 0x0F & *pRead;

        if(0x09 >= ucHigh1)   /* 0-9 */
        {
            *pWrite++ = ucHigh1 + 0x30;
        }
        else if(0x0A <= ucHigh1)    /* A-F */
        {
            *pWrite++ = ucHigh1 + 0x37;
        }
        else
        {

        }

        if(0x09 >= ucHigh2)   /* 0-9 */
        {
            *pWrite++ = ucHigh2 + 0x30;
        }
        else if(0x0A <= ucHigh2)    /* A-F */
        {
            *pWrite++ = ucHigh2 + 0x37;
        }
        else
        {

        }

        /* 下一个字符 */
        usChkLen++;
        pRead++;

        /* 第二个字节 */
        ucLow1 = 0x0F & (*pRead >> 4);
        ucLow2 = 0x0F & *pRead;


        if(0x09 >= ucLow1)   /* 0-9 */
        {
            *pWrite++ = ucLow1 + 0x30;
        }
        else if(0x0A <= ucLow1)    /* A-F */
        {
            *pWrite++ = ucLow1 + 0x37;
        }
        else
        {

        }

        if(0x09 >= ucLow2)   /* 0-9 */
        {
            *pWrite++ = ucLow2 + 0x30;
        }
        else if(0x0A <= ucLow2)    /* A-F */
        {
            *pWrite++ = ucLow2 + 0x37;
        }
        else
        {

        }

        /* 下一个字符 */
        usChkLen++;
        pRead++;

        usLen += 4;    /* 记录长度 */
    }

    return usLen;
}

TAF_UINT32 At_HexString2AsciiNumPrint(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usLen    = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucHigh1  = 0;
    TAF_UINT8  ucHigh2  = 0;
    TAF_UINT8 *pWrite   = pucDst;
    TAF_UINT8 *pRead    = pucSrc;

    if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + (2 * usSrcLen)) >= MaxLength)
    {
        AT_ERR_LOG("At_Unicode2UnicodePrint too long");
        return 0;
    }

    /* 扫完整个字串 */
    while( usChkLen < usSrcLen )
    {
        /* 第一个字节 */
        ucHigh1 = 0x0F & (*pRead >> 4);
        ucHigh2 = 0x0F & *pRead;

        if(0x09 >= ucHigh1)   /* 0-9 */
        {
            *pWrite++ = ucHigh1 + 0x30;
        }
        else if(0x0A <= ucHigh1)    /* A-F */
        {
            *pWrite++ = ucHigh1 + 0x37;
        }
        else
        {

        }

        if(0x09 >= ucHigh2)   /* 0-9 */
        {
            *pWrite++ = ucHigh2 + 0x30;
        }
        else if(0x0A <= ucHigh2)    /* A-F */
        {
            *pWrite++ = ucHigh2 + 0x37;
        }
        else
        {

        }

        /* 下一个字符 */
        usChkLen++;
        pRead++;
        usLen += 2;    /* 记录长度 */
    }

    return usLen;
}



TAF_UINT32 At_Ascii2UnicodePrint(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usLen = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8 *pWrite   = pucDst;
    TAF_UINT8 *pRead    = pucSrc;
    TAF_UINT8  ucHigh = 0;
    TAF_UINT8  ucLow = 0;

    if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + (4 * usSrcLen)) >= MaxLength)
    {
        AT_ERR_LOG("At_Ascii2UnicodePrint too long");
        return 0;
    }

    /* 扫完整个字串 */
    while( usChkLen++ < usSrcLen )
    {
        *pWrite++ = '0';
        *pWrite++ = '0';
        ucHigh = 0x0F & (*pRead >> 4);
        ucLow = 0x0F & *pRead;

        usLen += 4;    /* 记录长度 */

        if(0x09 >= ucHigh)   /* 0-9 */
        {
            *pWrite++ = ucHigh + 0x30;
        }
        else if(0x0A <= ucHigh)    /* A-F */
        {
            *pWrite++ = ucHigh + 0x37;
        }
        else
        {

        }

        if(0x09 >= ucLow)   /* 0-9 */
        {
            *pWrite++ = ucLow + 0x30;
        }
        else if(0x0A <= ucLow)    /* A-F */
        {
            *pWrite++ = ucLow + 0x37;
        }
        else
        {

        }

        /* 下一个字符 */
        pRead++;
    }

    return usLen;
}
TAF_UINT16 At_PrintReportData(
    TAF_UINT32                          MaxLength,
    TAF_INT8                            *headaddr,
    MN_MSG_MSG_CODING_ENUM_U8           enMsgCoding,
    TAF_UINT8                           *pucDst,
    TAF_UINT8                           *pucSrc,
    TAF_UINT16                          usSrcLen
)
{
    TAF_UINT16 usLength = 0;
    TAF_UINT32 ulPrintStrLen;


    ulPrintStrLen = 0;


    if(MN_MSG_MSG_CODING_UCS2 == enMsgCoding)  /* DATA:IRA */
    {
        usLength += (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pucDst + usLength,pucSrc,usSrcLen);
    }
    else if (MN_MSG_MSG_CODING_8_BIT == enMsgCoding)                                   /* DATA:8BIT */
    {
        usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                        (TAF_INT8 *)pgucAtSndCodeAddr,
                                                        pucDst + usLength,
                                                        pucSrc,
                                                        usSrcLen);
    }
    else/* DATA:UCS2 */
    {
        if(AT_CSCS_UCS2_CODE == gucAtCscsType)       /* +CSCS:UCS2 */
        {
            usLength += (TAF_UINT16)At_Ascii2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pucDst + usLength,pucSrc,usSrcLen);
        }
        else
        {
            if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + usSrcLen) >= MaxLength)
            {
                AT_ERR_LOG("At_PrintReportData too long");
                return 0;
            }

            if ((AT_CSCS_IRA_CODE == gucAtCscsType)
             && (MN_MSG_MSG_CODING_7_BIT == enMsgCoding))
            {
                TAF_STD_ConvertDefAlphaToAscii(pucSrc, usSrcLen, (pucDst + usLength), &ulPrintStrLen);
                usLength += (TAF_UINT16)ulPrintStrLen;
            }
            else
            {
                PS_MEM_CPY((pucDst + usLength),pucSrc,usSrcLen);
                usLength += usSrcLen;
            }
        }
    }

    return usLength;
}
TAF_UINT32 At_MsgPduInd(
    MN_MSG_BCD_ADDR_STRU                *pstScAddr,
    MN_MSG_RAW_TS_DATA_STRU             *pstPdu,
    TAF_UINT8                           *pucDst
)
{
    TAF_UINT16                          usLength            = 0;

    /* <alpha> 不报 */

    /* <length> */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)(pucDst + usLength),
                                       ",%d",
                                       pstPdu->ulLen);

    /* <data> 有可能得到是UCS2，需仔细处理*/
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pucDst + usLength,
                                           "%s",
                                           gaucAtCrLf);

    /*SCA*/
    if (0 == pstScAddr->ucBcdLen)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pucDst + usLength),
                                           "00");
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pucDst + usLength),
                                           "%X%X%X%X",
                                           (((pstScAddr->ucBcdLen + 1) & 0xf0) >> 4),
                                           ((pstScAddr->ucBcdLen + 1) & 0x0f),
                                           ((pstScAddr->addrType & 0xf0) >> 4),
                                           (pstScAddr->addrType & 0x0f));

        usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                        (TAF_INT8 *)pgucAtSndCodeAddr,
                                                        pucDst + usLength,
                                                        pstScAddr->aucBcdNum,
                                                        (TAF_UINT16)pstScAddr->ucBcdLen);
    }

    usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                    (TAF_INT8 *)pgucAtSndCodeAddr,
                                                    pucDst + usLength,
                                                    pstPdu->aucData,
                                                    (TAF_UINT16)pstPdu->ulLen);

    return usLength;
}


VOS_UINT32 At_StaRptPduInd(
    MN_MSG_BCD_ADDR_STRU                *pstScAddr,
    MN_MSG_RAW_TS_DATA_STRU             *pstPdu,
    VOS_UINT8                           *pucDst
)
{
    VOS_UINT16                          usLength            = 0;

    /* <length> */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)(pucDst + usLength),
                                       "%d",
                                       pstPdu->ulLen);

    /* <data> 有可能得到是UCS2，需仔细处理*/
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pucDst + usLength,
                                       "%s",
                                       gaucAtCrLf);

    /*SCA*/
    if (0 == pstScAddr->ucBcdLen)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)(pucDst + usLength),
                                           "00");
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)(pucDst + usLength),
                                           "%X%X%X%X",
                                           (((pstScAddr->ucBcdLen + 1) & 0xf0) >> 4),
                                           ((pstScAddr->ucBcdLen + 1) & 0x0f),
                                           ((pstScAddr->addrType & 0xf0) >> 4),
                                           (pstScAddr->addrType & 0x0f));

        usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                        (TAF_INT8 *)pgucAtSndCodeAddr,
                                                        pucDst + usLength,
                                                        pstScAddr->aucBcdNum,
                                                        (TAF_UINT16)pstScAddr->ucBcdLen);
    }

    usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                    (TAF_INT8 *)pgucAtSndCodeAddr,
                                                    pucDst + usLength,
                                                    pstPdu->aucData,
                                                    (TAF_UINT16)pstPdu->ulLen);

    return usLength;
}
VOS_UINT32  AT_IsClientBlock(VOS_VOID)
{
    VOS_UINT32                          ulAtStatus;
    VOS_UINT32                          ulAtMode;
    AT_PORT_BUFF_CFG_ENUM_UINT8         enSmsBuffCfg;

    enSmsBuffCfg    = AT_GetPortBuffCfg();
    if (AT_PORT_BUFF_DISABLE == enSmsBuffCfg)
    {
        return VOS_FALSE;
    }

    ulAtStatus  = AT_IsAnyParseClientPend();
    ulAtMode    = AT_IsAllClientDataMode();

    /* 若当前有一个通道处于 pend状态，则需要缓存短信 */
    if (VOS_TRUE == ulAtStatus)
    {
        return VOS_TRUE;
    }

    /* 若当前所有通道都处于data模式，则缓存短信 */
    if (VOS_TRUE == ulAtMode)
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;

}




TAF_VOID  At_BufferMsgInTa(
    VOS_UINT8                           ucIndex,
    MN_MSG_EVENT_ENUM_U32               enEvent,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    MN_MSG_EVENT_INFO_STRU              *pstEventInfo;
    TAF_UINT8                           *pucUsed;
    AT_MODEM_SMS_CTX_STRU               *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    {
        pstSmsCtx->stSmtBuffer.ucIndex = pstSmsCtx->stSmtBuffer.ucIndex % AT_BUFFER_SMT_EVENT_MAX;
        pucUsed = &(pstSmsCtx->stSmtBuffer.aucUsed[pstSmsCtx->stSmtBuffer.ucIndex]);
        pstEventInfo = &(pstSmsCtx->stSmtBuffer.astEvent[pstSmsCtx->stSmtBuffer.ucIndex]);
        pstSmsCtx->stSmtBuffer.ucIndex++;
    }

    if (AT_MSG_BUFFER_FREE == *pucUsed)
    {
        *pucUsed = AT_MSG_BUFFER_USED;
    }

    PS_MEM_CPY(pstEventInfo, pstEvent, sizeof(MN_MSG_EVENT_INFO_STRU));
    return;
}


VOS_UINT16 AT_PrintSmsLength(
    MN_MSG_MSG_CODING_ENUM_U8           enMsgCoding,
    VOS_UINT32                          ulLength,
    VOS_UINT8                          *pDst
)
{
    VOS_UINT16                          usLength;
    VOS_UINT16                          usSmContentLength;

    /* UCS2编码显示字节长度应该是UCS2字符个数，不是BYTE数，所以需要字节数除以2 */
    if (MN_MSG_MSG_CODING_UCS2 == enMsgCoding)
    {
        usSmContentLength = (VOS_UINT16)ulLength >> 1;
    }
    else
    {
        usSmContentLength = (VOS_UINT16)ulLength;
    }

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pDst,
                                       ",%d",
                                       usSmContentLength);

    return usLength;
}
TAF_VOID At_ForwardMsgToTeInCmt(
    VOS_UINT8                            ucIndex,
    TAF_UINT16                          *pusSendLength,
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16                          usLength = *pusSendLength;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* +CMT */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "+CMT: ");

    if (AT_CMGF_MSG_FORMAT_TEXT == pstSmsCtx->enCmgfMsgFormat)
    {
        /* +CMT: <oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data> */
        /* <oa> */
        usLength += (TAF_UINT16)At_PrintAsciiAddr(&pstTsDataInfo->u.stDeliver.stOrigAddr,
                                                  (pgucAtSndCodeAddr + usLength));
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           ",");

        /* <alpha> 不报 */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           ",");

        /* <scts> */
        usLength += (TAF_UINT16)At_SmsPrintScts(&pstTsDataInfo->u.stDeliver.stTimeStamp,
                                                (pgucAtSndCodeAddr + usLength));
        if (AT_CSDH_SHOW_TYPE == pstSmsCtx->ucCsdhType)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");
            /* <tooa> */
            usLength += (TAF_UINT16)At_PrintAddrType(&pstTsDataInfo->u.stDeliver.stOrigAddr,
                                                     (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

            /*<fo>*/
            usLength += (TAF_UINT16)At_PrintMsgFo(pstTsDataInfo, (pgucAtSndCodeAddr + usLength));

            /* <pid> */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",%d",
                                               pstTsDataInfo->u.stDeliver.enPid);

            /* <dcs> */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",%d,",
                                               pstTsDataInfo->u.stDeliver.stDcs.ucRawDcsData);

            /* <sca> */
            usLength += (TAF_UINT16)At_PrintBcdAddr(&pstEvent->u.stDeliverInfo.stRcvMsgInfo.stScAddr,
                                                    (pgucAtSndCodeAddr + usLength));

            /* <tosca> */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",%d",
                                               pstEvent->u.stDeliverInfo.stRcvMsgInfo.stScAddr.addrType);

            /* <length> */
            usLength += AT_PrintSmsLength(pstTsDataInfo->u.stDeliver.stDcs.enMsgCoding,
                                          pstTsDataInfo->u.stDeliver.stUserData.ulLen,
                                          (pgucAtSndCodeAddr + usLength));
        }

        /* <data> 有可能得到是UCS2，需仔细处理*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s",
                                           gaucAtCrLf);

        usLength += (TAF_UINT16)At_PrintReportData(AT_CMD_MAX_LEN,
                                                   (TAF_INT8 *)pgucAtSndCodeAddr,
                                                   pstTsDataInfo->u.stDeliver.stDcs.enMsgCoding,
                                                   (pgucAtSndCodeAddr + usLength),
                                                   pstTsDataInfo->u.stDeliver.stUserData.aucOrgData,
                                                   (TAF_UINT16)pstTsDataInfo->u.stDeliver.stUserData.ulLen);
    }
    else
    {
        /* +CMT: [<alpha>],<length><CR><LF><pdu> */
        usLength += (TAF_UINT16)At_MsgPduInd(&pstEvent->u.stDeliverInfo.stRcvMsgInfo.stScAddr,
                                             &pstEvent->u.stDeliverInfo.stRcvMsgInfo.stTsRawData,
                                             (pgucAtSndCodeAddr + usLength));
    }
    *pusSendLength = usLength;
}
TAF_VOID  AT_ForwardDeliverMsgToTe(
    MN_MSG_EVENT_INFO_STRU              *pstEvent,
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo
)
{
    TAF_BOOL                            bCmtiInd;
    TAF_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_ForwardDeliverMsgToTe: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);


    AT_LOG1("AT_ForwardDeliverMsgToTe: current mt is", pstSmsCtx->stCnmiType.CnmiMtType);

    if (AT_CNMI_MT_NO_SEND_TYPE == pstSmsCtx->stCnmiType.CnmiMtType)
    {
        return;
    }

    bCmtiInd = TAF_FALSE;
    if ((MN_MSG_RCVMSG_ACT_STORE == pstEvent->u.stDeliverInfo.enRcvSmAct)
     && (MN_MSG_MEM_STORE_NONE != pstEvent->u.stDeliverInfo.enMemStore))
    {
        if ((AT_CNMI_MT_CMTI_TYPE == pstSmsCtx->stCnmiType.CnmiMtType)
         || (AT_CNMI_MT_CLASS3_TYPE == pstSmsCtx->stCnmiType.CnmiMtType))
        {
            bCmtiInd = TAF_TRUE;
        }

        if (MN_MSG_MSG_CLASS_2 == pstTsDataInfo->u.stDeliver.stDcs.enMsgClass)
        {
            bCmtiInd = TAF_TRUE;
        }
    }

    usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       "%s",
                                       gaucAtCrLf);

    /*根据MT设置和接收到事件的CLASS类型得到最终的事件上报格式:
    协议要求MT为3时CLASS类型获取实际MT类型, 该情况下终端不上报事件与协议不一致*/
    if (TAF_TRUE == bCmtiInd)
    {
        /* +CMTI: <mem>,<index> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "+CMTI: %s,%d",
                                           At_GetStrContent(At_GetSmsArea(pstEvent->u.stDeliverInfo.enMemStore)),
                                           pstEvent->u.stDeliverInfo.ulInex);
    }
    else
    {
        /*CMT的方式上报*/
        At_ForwardMsgToTeInCmt(ucIndex, &usLength,pstTsDataInfo,pstEvent);
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "%s",
                                   gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}


TAF_VOID  AT_ForwardStatusReportMsgToTe(
    MN_MSG_EVENT_INFO_STRU              *pstEvent,
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo
)
{
    TAF_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_ForwardStatusReportMsgToTe: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    AT_LOG1("AT_ForwardStatusReportMsgToTe: current ds is ", pstSmsCtx->stCnmiType.CnmiDsType);

    if (AT_CNMI_DS_NO_SEND_TYPE == pstSmsCtx->stCnmiType.CnmiDsType)
    {
        return;
    }

    usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       "%s",
                                       gaucAtCrLf);

    if ((MN_MSG_RCVMSG_ACT_STORE == pstEvent->u.stDeliverInfo.enRcvSmAct)
     && (MN_MSG_MEM_STORE_NONE != pstEvent->u.stDeliverInfo.enMemStore))
    {
        /* +CDSI: <mem>,<index> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "+CDSI: %s,%d",
                                           At_GetStrContent(At_GetSmsArea(pstEvent->u.stDeliverInfo.enMemStore)),
                                           pstEvent->u.stDeliverInfo.ulInex);
    }
    else
    {
        /* +CDS */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "+CDS: ");
        if (AT_CMGF_MSG_FORMAT_TEXT == pstSmsCtx->enCmgfMsgFormat)
        {
            /* +CDS: <fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st> */
            /*<fo>*/
            usLength += (TAF_UINT16)At_PrintMsgFo(pstTsDataInfo, (pgucAtSndCodeAddr + usLength));

            /*<mr>*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",%d,",
                                               pstTsDataInfo->u.stStaRpt.ucMr);

            /*<ra>*/
            usLength += (TAF_UINT16)At_PrintAsciiAddr(&pstTsDataInfo->u.stStaRpt.stRecipientAddr,
                                                      (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

            /*<tora>*/
            usLength += (TAF_UINT16)At_PrintAddrType(&pstTsDataInfo->u.stStaRpt.stRecipientAddr,
                                                     (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

            /* <scts> */
            usLength += (TAF_UINT16)At_SmsPrintScts(&pstTsDataInfo->u.stStaRpt.stTimeStamp,
                                                    (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

             /* <dt> */
             usLength += (TAF_UINT16)At_SmsPrintScts(&pstTsDataInfo->u.stStaRpt.stDischargeTime,
                                                     (pgucAtSndCodeAddr + usLength));

             /*<st>*/
             usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                ",%d",
                                                pstTsDataInfo->u.stStaRpt.enStatus);
        }
        else
        {
            /* +CDS: <length><CR><LF><pdu> */
            usLength += (VOS_UINT16)At_StaRptPduInd(&pstEvent->u.stDeliverInfo.stRcvMsgInfo.stScAddr,
                                                 &pstEvent->u.stDeliverInfo.stRcvMsgInfo.stTsRawData,
                                                 (pgucAtSndCodeAddr + usLength));
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}


TAF_VOID  AT_ForwardPppMsgToTe(
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet;
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo;

    pstTsDataInfo = At_GetMsgMem();

    ulRet = MN_MSG_Decode(&pstEvent->u.stDeliverInfo.stRcvMsgInfo.stTsRawData, pstTsDataInfo);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return;
    }

    if (MN_MSG_TPDU_DELIVER == pstTsDataInfo->enTpduType)
    {
        AT_ForwardDeliverMsgToTe(pstEvent, pstTsDataInfo);
    }
    else if (MN_MSG_TPDU_STARPT == pstTsDataInfo->enTpduType)
    {
        AT_ForwardStatusReportMsgToTe(pstEvent, pstTsDataInfo);
    }
    else
    {
        AT_WARN_LOG("AT_ForwardPppMsgToTe: invalid tpdu type.");
    }

    return;

}

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))

VOS_VOID AT_ForwardCbMsgToTe(
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet;
    MN_MSG_CBPAGE_STRU                  stCbmPageInfo;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_ForwardCbMsgToTe: WARNING:AT INDEX NOT FOUND!");
        return;
    }


    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    AT_LOG1("AT_ForwardCbMsgToTe: current bm is ", pstSmsCtx->stCnmiType.CnmiBmType);


    /*根据BM设置和接收到事件的CLASS类型得到最终的事件上报格式:
    不支持协议要求BM为3时,CBM的上报*/

    ulRet = MN_MSG_DecodeCbmPage(&(pstEvent->u.stCbsDeliverInfo.stCbRawData), &stCbmPageInfo);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return;
    }

    /*+CBM: <sn>,<mid>,<dcs>,<page>,<pages><CR><LF><data> (text mode enabled)*/
    /*+CBM: <length><CR><LF><pdu> (PDU mode enabled); or*/
    usLength  = 0;
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "+CBM: ");

    if (AT_CMGF_MSG_FORMAT_TEXT == pstSmsCtx->enCmgfMsgFormat)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%d,",
                                           stCbmPageInfo.stSn.usRawSnData);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%d,",
                                           stCbmPageInfo.usMid);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%d,",
                                           stCbmPageInfo.stDcs.ucRawDcsData);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%d,",
                                           stCbmPageInfo.ucPageIndex);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%d",
                                           stCbmPageInfo.ucPageNum);

        /* <data> 有可能得到是UCS2，需仔细处理*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s",
                                           gaucAtCrLf);

        usLength += (TAF_UINT16)At_PrintReportData(AT_CMD_MAX_LEN,
                                                   (TAF_INT8 *)pgucAtSndCodeAddr,
                                                   stCbmPageInfo.stDcs.enMsgCoding,
                                                   (pgucAtSndCodeAddr + usLength),
                                                   stCbmPageInfo.stContent.aucContent,
                                                   (TAF_UINT16)stCbmPageInfo.stContent.ulLen);
    }
    else
    {
        /*+CBM: <length><CR><LF><pdu> (PDU mode enabled); or*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%d",
                                           pstEvent->u.stCbsDeliverInfo.stCbRawData.ulLen);

        /* <data> 有可能得到是UCS2，需仔细处理*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s",
                                           gaucAtCrLf);

        usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                        (TAF_INT8 *)pgucAtSndCodeAddr,
                                                        (pgucAtSndCodeAddr + usLength),
                                                        pstEvent->u.stCbsDeliverInfo.stCbRawData.aucData,
                                                        (TAF_UINT16)pstEvent->u.stCbsDeliverInfo.stCbRawData.ulLen);
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

    /* 与标杆对比,此处还需多一个空行 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);


    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
}

#endif

TAF_VOID At_ForwardMsgToTe(
    MN_MSG_EVENT_ENUM_U32               enEvent,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{

    AT_LOG1("At_ForwardMsgToTe: current Event is ", enEvent);

    switch (enEvent)
    {
    case MN_MSG_EVT_DELIVER:
        AT_ForwardPppMsgToTe(pstEvent);
        break;

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))
    case MN_MSG_EVT_DELIVER_CBM:
        AT_ForwardCbMsgToTe(pstEvent);
        break;
#endif
    default:
        AT_WARN_LOG("At_SendSmtInd: invalid tpdu type.");
        break;
    }
    return;
}


TAF_VOID At_HandleSmtBuffer(
    VOS_UINT8                           ucIndex,
    AT_CNMI_BFR_TYPE                    ucBfrType
)
{
    TAF_UINT8                           ucLoop;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if (AT_CNMI_BFR_SEND_TYPE == ucBfrType)
    {
        for (ucLoop = 0; ucLoop < AT_BUFFER_SMT_EVENT_MAX; ucLoop ++)
        {
            if (AT_MSG_BUFFER_USED == pstSmsCtx->stSmtBuffer.aucUsed[ucLoop])
            {
                At_ForwardMsgToTe(MN_MSG_EVT_DELIVER, &(pstSmsCtx->stSmtBuffer.astEvent[ucLoop]));
            }
        }

    }

    PS_MEM_SET(&(pstSmsCtx->stSmtBuffer), 0x00, sizeof(pstSmsCtx->stSmtBuffer));

    return;
}


VOS_VOID AT_FlushSmsIndication(VOS_VOID)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;
    AT_PORT_BUFF_CFG_STRU              *pstPortBuffCfg = VOS_NULL_PTR;
    VOS_UINT16                          i;
    VOS_UINT32                          ulClientId;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           aucModemFlag[MODEM_ID_BUTT];

    pstPortBuffCfg = AT_GetPortBuffCfgInfo();
    PS_MEM_SET(aucModemFlag, 0x0, sizeof(aucModemFlag));

    if (pstPortBuffCfg->ucNum > AT_MAX_CLIENT_NUM)
    {
        pstPortBuffCfg->ucNum = AT_MAX_CLIENT_NUM;
    }

    /* 根据clientId查找需要flush 短信modem id */
    for (i = 0; i < pstPortBuffCfg->ucNum; i++)
    {
        ulClientId = pstPortBuffCfg->ulUsedClientID[i];
        ulRslt = AT_GetModemIdFromClient((VOS_UINT8)ulClientId, &enModemId);
        if (VOS_OK != ulRslt)
        {
            AT_ERR_LOG("AT_FlushSmsIndication: Get modem id fail");
            continue;
        }

        aucModemFlag[enModemId] = VOS_TRUE;
    }

    /* flush SMS */
    for (i = 0; i < MODEM_ID_BUTT; i++)
    {
        if (VOS_TRUE == aucModemFlag[i])
        {
            pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(i);

            for (i = 0; i < AT_BUFFER_SMT_EVENT_MAX; i ++)
            {
                if (AT_MSG_BUFFER_USED == pstSmsCtx->stSmtBuffer.aucUsed[i])
                {
                    At_ForwardMsgToTe(MN_MSG_EVT_DELIVER, &(pstSmsCtx->stSmtBuffer.astEvent[i]));
                }
            }

            PS_MEM_SET(&(pstSmsCtx->stSmtBuffer), 0x00, sizeof(pstSmsCtx->stSmtBuffer));
        }
    }

    return;
}


/*****************************************************************************
 函 数 名  : At_SmsModSmStatusRspProc
 功能描述  : 接收到CMMT命令的响应事件的处理:
 输入参数  : ucIndex - 用户索引
             pEvent  - 事件内容
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月4日
    作    者   : fuyingjun
    修改内容   : 新生成函数

*****************************************************************************/
TAF_VOID At_SmsModSmStatusRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    VOS_UINT32                          ulRet;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    if (TAF_TRUE != pstEvent->u.stModifyInfo.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stDeleteInfo.ulFailCause);
    }
    else
    {
        ulRet = AT_OK;
    }

    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulRet);
    return;
}


TAF_VOID At_SmsInitResultProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pEvent
)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    pstSmsCtx->stCpmsInfo.stUsimStorage.ulTotalRec = pEvent->u.stInitResultInfo.ulTotalSmRec;
    pstSmsCtx->stCpmsInfo.stUsimStorage.ulUsedRec = pEvent->u.stInitResultInfo.ulUsedSmRec;

    return;
}


VOS_VOID At_SmsDeliverErrProc(
    VOS_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if ((VOS_TRUE == pstSmsCtx->ucLocalStoreFlg)
     && (TAF_MSG_ERROR_TR2M_TIMEOUT == pstEvent->u.stDeliverErrInfo.enErrorCode))
    {
        pstSmsCtx->stCnmiType.CnmiMtType            = AT_CNMI_MT_NO_SEND_TYPE;
        pstSmsCtx->stCnmiType.CnmiDsType            = AT_CNMI_DS_NO_SEND_TYPE;
    }

    /* 短信接收流程因为写操作失败不会上报事件给AT，且该事件有ERROR LOG记录不需要上报给应用处理 */

    return;
}
VOS_VOID At_SmsInitSmspResultProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    PS_MEM_CPY(&(pstSmsCtx->stCscaCsmpInfo.stParmInUsim),
               &pstEvent->u.stInitSmspResultInfo.astSrvParm[0],
               sizeof(pstSmsCtx->stCscaCsmpInfo.stParmInUsim));
    PS_MEM_CPY(&(pstSmsCtx->stCpmsInfo.stRcvPath),
           &pstEvent->u.stInitSmspResultInfo.stRcvMsgPath,
           sizeof(pstSmsCtx->stCpmsInfo.stRcvPath));

    g_enClass0Tailor = pstEvent->u.stInitSmspResultInfo.enClass0Tailor;

    return;
}


VOS_VOID At_SmsSrvParmChangeProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    PS_MEM_CPY(&(pstSmsCtx->stCscaCsmpInfo.stParmInUsim),
               &pstEvent->u.stSrvParmChangeInfo.astSrvParm[0],
               sizeof(pstSmsCtx->stCscaCsmpInfo.stParmInUsim));

    return;
}


VOS_VOID At_SmsRcvMsgPathChangeProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    pstSmsCtx->stCpmsInfo.stRcvPath.enRcvSmAct = pstEvent->u.stRcvMsgPathInfo.enRcvSmAct;
    pstSmsCtx->stCpmsInfo.stRcvPath.enSmMemStore = pstEvent->u.stRcvMsgPathInfo.enSmMemStore;
    pstSmsCtx->stCpmsInfo.stRcvPath.enRcvStaRptAct = pstEvent->u.stRcvMsgPathInfo.enRcvStaRptAct;
    pstSmsCtx->stCpmsInfo.stRcvPath.enStaRptMemStore = pstEvent->u.stRcvMsgPathInfo.enStaRptMemStore;

    return;
}


VOS_VOID At_SmsStorageListProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    MN_MSG_STORAGE_LIST_EVT_INFO_STRU  *pstStorageListInfo;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    pstStorageListInfo = &pstEvent->u.stStorageListInfo;
    if (MN_MSG_MEM_STORE_SIM == pstStorageListInfo->enMemStroe)
    {
        PS_MEM_CPY(&(pstSmsCtx->stCpmsInfo.stUsimStorage),
                   &pstEvent->u.stStorageListInfo,
                   sizeof(pstSmsCtx->stCpmsInfo.stUsimStorage));
    }
    else
    {
        PS_MEM_CPY(&(pstSmsCtx->stCpmsInfo.stNvimStorage),
                   &pstEvent->u.stStorageListInfo,
                   sizeof(pstSmsCtx->stCpmsInfo.stNvimStorage));
    }

    if (!AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {

        /* 接收到NV的短信容量上报，修改NV的短信容量等待标志已接收到NV的短信容量 */
        if (MN_MSG_MEM_STORE_NV == pstStorageListInfo->enMemStroe)
        {
            gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus = TAF_FALSE;
        }

        /* 接收到SIM的短信容量上报，修改SIM的短信容量等待标志已接收到SIM的短信容量 */
        if (MN_MSG_MEM_STORE_SIM == pstStorageListInfo->enMemStroe)
        {
            gastAtClientTab[ucIndex].AtSmsData.bWaitForUsimStorageStatus = TAF_FALSE;
        }

        /* CPMS的设置操作需要等待所有容量信息和设置响应消息后完成 */
        if (AT_CMD_CPMS_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            if ((TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForCpmsSetRsp)
             && (TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus)
             && (TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForUsimStorageStatus))
            {
                AT_STOP_TIMER_CMD_READY(ucIndex);
                At_PrintSetCpmsRsp(ucIndex);
            }
        }

        /* CPMS的读取操作需要等待所有容量信息后完成 */
        if (AT_CMD_CPMS_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            if ((TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus)
             && (TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForUsimStorageStatus))
            {
                AT_STOP_TIMER_CMD_READY(ucIndex);
                At_PrintGetCpmsRsp(ucIndex);
            }
        }
    }

    return;
}


VOS_VOID At_SmsStorageExceedProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16 usLength = 0;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    /* 初始化 */
    enModemId       = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_SmsStorageExceedProc: Get modem id fail.");
        return;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%s",
                                       gaucAtCrLf);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "^SMMEMFULL: ");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR*)(pgucAtSndCodeAddr + usLength),
                                       "%s",
                                       At_GetStrContent(At_GetSmsArea(pstEvent->u.stStorageStateInfo.enMemStroe)));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%s",
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}
VOS_VOID At_SmsDeliverProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16                          usLength            = 0;
    TAF_UINT32                          ulRet               = AT_OK;
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo;
    VOS_UINT8                           ucUserId;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    ucUserId = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过ClientId获取ucUserId */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->clientId, &ucUserId) )
    {
        AT_WARN_LOG("AT_SmsDeliverProc: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucUserId);

    /* 当前短信类型为CLass0且短信定制为
    1:H3G与意大利TIM Class 0短信需求相同，不对短信进行存储，要求将CLASS 0
    短信直接采用+CMT进行主动上报。不受CNMI以及CPMS设置的影响，如果后台已经
    打开，则后台对CLASS 0短信进行显示。
    CLass0的短信此时不考虑MT,MODE的参数
    */

    pstTsDataInfo = At_GetMsgMem();
    ulRet = MN_MSG_Decode(&pstEvent->u.stDeliverInfo.stRcvMsgInfo.stTsRawData, pstTsDataInfo);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        return;
    }

    AT_StubSaveAutoReplyData(ucUserId, pstEvent, pstTsDataInfo);

#if (FEATURE_ON == FEATURE_AT_HSUART)
    AT_SmsStartRingTe(VOS_TRUE);
#endif

    if ( (MN_MSG_TPDU_DELIVER == pstTsDataInfo->enTpduType)
      && (MN_MSG_MSG_CLASS_0 == pstTsDataInfo->u.stDeliver.stDcs.enMsgClass)
      && (MN_MSG_CLASS0_DEF != g_enClass0Tailor))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

        if ((MN_MSG_CLASS0_TIM == g_enClass0Tailor)
         || (MN_MSG_CLASS0_VIVO == g_enClass0Tailor))
        {
            /*+CMT格式上报 */
            At_ForwardMsgToTeInCmt(ucUserId, &usLength,pstTsDataInfo,pstEvent);
        }
        else
        {
            /* +CMTI: <mem>,<index> */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "+CMTI: %s,%d",
                                               At_GetStrContent(At_GetSmsArea(pstEvent->u.stDeliverInfo.enMemStore)),
                                               pstEvent->u.stDeliverInfo.ulInex);
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s",
                                           gaucAtCrLf);

        At_SendResultData(ucUserId, pgucAtSndCodeAddr, usLength);
        return;
    }

    if (AT_CNMI_MODE_SEND_OR_DISCARD_TYPE == pstSmsCtx->stCnmiType.CnmiModeType)
    {
        At_ForwardMsgToTe(MN_MSG_EVT_DELIVER, pstEvent);
        return;
    }

    /* 当模式为0时缓存 */
    if (AT_CNMI_MODE_BUFFER_TYPE == pstSmsCtx->stCnmiType.CnmiModeType)
    {
        At_BufferMsgInTa(ucIndex, MN_MSG_EVT_DELIVER, pstEvent);
        return;
    }

    /* 当模式为2时缓存 */
    if (AT_CNMI_MODE_SEND_OR_BUFFER_TYPE == pstSmsCtx->stCnmiType.CnmiModeType)
    {
        /* 判断是否具备缓存的条件 */
        if (VOS_TRUE == AT_IsClientBlock())
        {
            At_BufferMsgInTa(ucIndex, MN_MSG_EVT_DELIVER, pstEvent);
        }
        else
        {
            At_ForwardMsgToTe(MN_MSG_EVT_DELIVER, pstEvent);
        }
        return;
    }

    /*目前不支持 AT_CNMI_MODE_EMBED_AND_SEND_TYPE*/

    return;
}
TAF_VOID At_SetRcvPathRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if (TAF_TRUE != pstEvent->u.stRcvMsgPathInfo.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stSrvParmInfo.ulFailCause);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    if (AT_CMD_CPMS_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /*保存临时数据到内存和NVIM*/
        pstSmsCtx->stCpmsInfo.stRcvPath.enSmMemStore = pstEvent->u.stRcvMsgPathInfo.enSmMemStore;
        pstSmsCtx->stCpmsInfo.stRcvPath.enStaRptMemStore = pstEvent->u.stRcvMsgPathInfo.enStaRptMemStore;
        pstSmsCtx->stCpmsInfo.enMemReadorDelete = pstSmsCtx->stCpmsInfo.enTmpMemReadorDelete;
        pstSmsCtx->stCpmsInfo.enMemSendorWrite = pstSmsCtx->stCpmsInfo.enTmpMemSendorWrite;

        gastAtClientTab[ucIndex].AtSmsData.bWaitForCpmsSetRsp = TAF_FALSE;

        /* CPMS的设置操作需要等待所有容量信息和设置响应消息后完成 */
        if ((TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForCpmsSetRsp)
         && (TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForNvStorageStatus)
         && (TAF_FALSE == gastAtClientTab[ucIndex].AtSmsData.bWaitForUsimStorageStatus))
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_PrintSetCpmsRsp(ucIndex);
        }

    }
    else if (AT_CMD_CSMS_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 执行命令操作 */
        pstSmsCtx->enCsmsMsgVersion                      = pstEvent->u.stRcvMsgPathInfo.enSmsServVersion;
        pstSmsCtx->stCpmsInfo.stRcvPath.enSmsServVersion = pstSmsCtx->enCsmsMsgVersion;

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)(pgucAtSndCodeAddr + gstAtSendData.usBufLen),
                                                        "%s: ",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        At_PrintCsmsInfo(ucIndex);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_OK);
    }
    else/*AT_CMD_CNMI_SET*/
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_OK);

        pstSmsCtx->stCnmiType.CnmiBfrType = pstSmsCtx->stCnmiType.CnmiTmpBfrType;
        pstSmsCtx->stCnmiType.CnmiDsType = pstSmsCtx->stCnmiType.CnmiTmpDsType;
        pstSmsCtx->stCnmiType.CnmiBmType = pstSmsCtx->stCnmiType.CnmiTmpBmType;
        pstSmsCtx->stCnmiType.CnmiMtType = pstSmsCtx->stCnmiType.CnmiTmpMtType;
        pstSmsCtx->stCnmiType.CnmiModeType = pstSmsCtx->stCnmiType.CnmiTmpModeType;
        pstSmsCtx->stCpmsInfo.stRcvPath.enRcvSmAct = pstEvent->u.stRcvMsgPathInfo.enRcvSmAct;
        pstSmsCtx->stCpmsInfo.stRcvPath.enRcvStaRptAct = pstEvent->u.stRcvMsgPathInfo.enRcvStaRptAct;

        if (0 != pstSmsCtx->stCnmiType.CnmiModeType)
        {
            At_HandleSmtBuffer(ucIndex, pstSmsCtx->stCnmiType.CnmiBfrType);
        }

    }
    return;
}


TAF_VOID At_SetCscaCsmpRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if (TAF_TRUE != pstEvent->u.stSrvParmInfo.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stSrvParmInfo.ulFailCause);
    }
    else
    {
        if (AT_CMD_CSMP_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            PS_MEM_CPY(&(pstSmsCtx->stCscaCsmpInfo.stVp), &(pstSmsCtx->stCscaCsmpInfo.stTmpVp), sizeof(pstSmsCtx->stCscaCsmpInfo.stVp));
            pstSmsCtx->stCscaCsmpInfo.ucFo = pstSmsCtx->stCscaCsmpInfo.ucTmpFo;
            pstSmsCtx->stCscaCsmpInfo.bFoUsed = TAF_TRUE;
        }
        PS_MEM_CPY(&(pstSmsCtx->stCscaCsmpInfo.stParmInUsim),
                   &pstEvent->u.stSrvParmInfo.stSrvParm,
                   sizeof(pstSmsCtx->stCscaCsmpInfo.stParmInUsim));
        ulRet = AT_OK;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);
    return;
}


TAF_VOID  At_DeleteRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet;
    MN_MSG_DELETE_PARAM_STRU            stDelete;


    PS_MEM_SET(&stDelete, 0x00, sizeof(stDelete));


    if ((AT_CMD_CMGD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CBMGD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        return;
    }

    stDelete.enMemStore = pstEvent->u.stDeleteInfo.enMemStore;
    stDelete.ulIndex = pstEvent->u.stDeleteInfo.ulIndex;
    if (TAF_TRUE != pstEvent->u.stDeleteInfo.bSuccess)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stDeleteInfo.ulFailCause);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    if (MN_MSG_DELETE_SINGLE == pstEvent->u.stDeleteInfo.enDeleteType)
    {
        gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes ^= AT_MSG_DELETE_SINGLE;
    }

    if (MN_MSG_DELETE_ALL == pstEvent->u.stDeleteInfo.enDeleteType)
    {
        gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes ^= AT_MSG_DELETE_ALL;
    }

    if (MN_MSG_DELETE_READ == pstEvent->u.stDeleteInfo.enDeleteType)
    {
        gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes ^= AT_MSG_DELETE_READ;
    }

    if (MN_MSG_DELETE_SENT == pstEvent->u.stDeleteInfo.enDeleteType)
    {
        gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes ^= AT_MSG_DELETE_SENT;
    }

    if (MN_MSG_DELETE_NOT_SENT == pstEvent->u.stDeleteInfo.enDeleteType)
    {
        gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes ^= AT_MSG_DELETE_UNSENT;
    }

    if (0 == gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_MsgDeleteCmdProc(ucIndex,
                            gastAtClientTab[ucIndex].opId,
                            stDelete,
                            gastAtClientTab[ucIndex].AtSmsData.ucMsgDeleteTypes);
    }
    return;
}


VOS_VOID AT_QryCscaRspProc(
    VOS_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU             *pstEvent
)
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* AT模块在等待CSCA查询命令的结果事件上报 */
    if (AT_CMD_CSCA_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (VOS_TRUE == pstEvent->u.stSrvParmInfo.bSuccess)
    {
        /* 更新短信中心号码到AT模块，解决MSG模块初始化完成事件上报时AT模块未启动问题 */
        PS_MEM_CPY(&(pstSmsCtx->stCscaCsmpInfo.stParmInUsim.stScAddr),
                   &pstEvent->u.stSrvParmInfo.stSrvParm.stScAddr,
                   sizeof(pstSmsCtx->stCscaCsmpInfo.stParmInUsim.stScAddr));

        /* 设置错误码为AT_OK           构造结构为+CSCA: <sca>,<toda>格式的短信 */
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /*短信中心号码存在指示为存在且短信中心号码长度不为0*/
        if ((0 == (pstEvent->u.stSrvParmInfo.stSrvParm.ucParmInd & MN_MSG_SRV_PARM_MASK_SC_ADDR))
         && (0 != pstEvent->u.stSrvParmInfo.stSrvParm.stScAddr.ucBcdLen))
        {
            /*将SCA地址由BCD码转换为ASCII码*/
            usLength += At_PrintBcdAddr(&pstEvent->u.stSrvParmInfo.stSrvParm.stScAddr,
                                        (pgucAtSndCodeAddr + usLength));

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",%d",
                                               pstEvent->u.stSrvParmInfo.stSrvParm.stScAddr.addrType);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "\"\",128");
        }

        gstAtSendData.usBufLen = usLength;
        ulRet                  = AT_OK;
    }
    else
    {
        /* 根据pstEvent->u.stSrvParmInfo.ulFailCause调用At_ChgMnErrCodeToAt转换出AT模块的错误码 */
        gstAtSendData.usBufLen = 0;
        ulRet                  = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stSrvParmInfo.ulFailCause);

    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);

    return;
}
VOS_VOID At_SmsStubRspProc(
    VOS_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU             *pstEvent
)
{
    VOS_UINT32                          ulRet;

    /* AT模块在等待CMSTUB命令的结果事件上报 */
    if (AT_CMD_CMSTUB_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (MN_ERR_NO_ERROR == pstEvent->u.stResult.ulErrorCode)
    {
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_CMS_UNKNOWN_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulRet);
    return;
}


VOS_UINT32 AT_GetBitMap(
    VOS_UINT32                         *pulBitMap,
    VOS_UINT32                          ulIndex
)
{
    VOS_UINT8                           ucX;
    VOS_UINT32                          ulY;
    VOS_UINT32                          ulMask;

    ulY = ulIndex/32;
    ucX = (VOS_UINT8)ulIndex%32;
    ulMask = (1 << ucX);
    if (0 != (pulBitMap[ulY] & ulMask))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_VOID AT_SmsListIndex(
    VOS_UINT16                          usLength,
    MN_MSG_DELETE_TEST_EVT_INFO_STRU   *pstPara,
    VOS_UINT16                         *pusPrintOffSet
)
{
    TAF_UINT32                          ulLoop;
    TAF_UINT32                          ulMsgNum;

    ulMsgNum  = 0;

    for (ulLoop = 0; ulLoop < pstPara->ulSmCapacity; ulLoop++)
    {
        if (TAF_TRUE == AT_GetBitMap(pstPara->aulValidLocMap, ulLoop))
        {
            ulMsgNum++;
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,", ulLoop);
        }

    }

    /* 删除最后一个"," */
    if (0 != ulMsgNum)
    {
        usLength -= 1;
    }

    *pusPrintOffSet = usLength;

    return;
}
TAF_VOID  At_DeleteTestRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16                          usLength;
    MN_MSG_DELETE_TEST_EVT_INFO_STRU   *pstPara;
    VOS_UINT32                          ulLoop;
    VOS_BOOL                            bMsgExist;

    pstPara = (MN_MSG_DELETE_TEST_EVT_INFO_STRU *)&pstEvent->u.stDeleteTestInfo;

    if (AT_CMD_CMGD_TEST == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          "%s: (",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        AT_SmsListIndex(usLength, pstPara, &usLength);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "),(0-4)");
    }
    else
    {
        /* 判断是否有短信索引列表输出: 无短信需要输出直接返回OK */
        bMsgExist = VOS_FALSE;

        for (ulLoop = 0; ulLoop < MN_MSG_CMGD_PARA_MAX_LEN; ulLoop++)
        {
            if (0 != pstPara->aulValidLocMap[ulLoop])
            {
                bMsgExist = VOS_TRUE;
                break;
            }
        }

        if (VOS_TRUE == bMsgExist)
        {
            usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              "%s: ",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            AT_SmsListIndex(usLength, pstPara, &usLength);
        }
        else
        {
            usLength = 0;
        }
    }

    gstAtSendData.usBufLen = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    At_FormatResultData(ucIndex,AT_OK);

    return;
}


TAF_VOID At_ReadRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16                          usLength            = 0;
    TAF_UINT32                          ulRet               = AT_OK;
    MN_MSG_TS_DATA_INFO_STRU           *pstTsDataInfo;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (TAF_TRUE != pstEvent->u.stReadInfo.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stReadInfo.ulFailCause);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_SmsPrintState(pstSmsCtx->enCmgfMsgFormat,
                                             pstEvent->u.stReadInfo.enStatus,
                                             (pgucAtSndCodeAddr + usLength));

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       ",");

    pstTsDataInfo = At_GetMsgMem();
    ulRet = MN_MSG_Decode(&pstEvent->u.stReadInfo.stMsgInfo.stTsRawData, pstTsDataInfo);
    if (MN_ERR_NO_ERROR != ulRet)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, ulRet);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    if (AT_CMGF_MSG_FORMAT_PDU == pstSmsCtx->enCmgfMsgFormat)/*PDU*/
    {
        /* +CMGR: <stat>,[<alpha>],<length><CR><LF><pdu> */
        usLength += (TAF_UINT16)At_MsgPduInd(&pstEvent->u.stReadInfo.stMsgInfo.stScAddr,
                                             &pstEvent->u.stReadInfo.stMsgInfo.stTsRawData,
                                             (pgucAtSndCodeAddr + usLength));

        gstAtSendData.usBufLen = usLength;
        At_FormatResultData(ucIndex, AT_OK);
        return;
    }

    switch (pstEvent->u.stReadInfo.stMsgInfo.stTsRawData.enTpduType)
    {
        case MN_MSG_TPDU_DELIVER:
            /* +CMGR: <stat>,<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>, <sca>,<tosca>,<length>]<CR><LF><data>*/
            /* <oa> */
            usLength += (TAF_UINT16)At_PrintAsciiAddr(&pstTsDataInfo->u.stDeliver.stOrigAddr,
                                                      (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");
            /* <alpha> 不报 */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

            /* <scts> */
            usLength += (TAF_UINT16)At_SmsPrintScts(&pstTsDataInfo->u.stDeliver.stTimeStamp,
                                                    (pgucAtSndCodeAddr + usLength));

            if (AT_CSDH_SHOW_TYPE == pstSmsCtx->ucCsdhType)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",");
                /* <tooa> */
                usLength += (TAF_UINT16)At_PrintAddrType(&pstTsDataInfo->u.stDeliver.stOrigAddr,
                                                         (pgucAtSndCodeAddr + usLength));
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",");

                /*<fo>*/
                usLength += (TAF_UINT16)At_PrintMsgFo(pstTsDataInfo, (pgucAtSndCodeAddr + usLength));

                /* <pid> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d",
                                                   pstTsDataInfo->u.stDeliver.enPid);

                /* <dcs> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d,",
                                                   pstTsDataInfo->u.stDeliver.stDcs.ucRawDcsData);

                /* <sca> */
                usLength += (TAF_UINT16)At_PrintBcdAddr(&pstEvent->u.stReadInfo.stMsgInfo.stScAddr,
                                                       (pgucAtSndCodeAddr + usLength));

                /* <tosca> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (TAF_CHAR *)pgucAtSndCodeAddr,
                                                  (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                  ",%d",
                                                  pstEvent->u.stReadInfo.stMsgInfo.stScAddr.addrType);

                /* <length> */
                usLength += AT_PrintSmsLength(pstTsDataInfo->u.stDeliver.stDcs.enMsgCoding,
                                              pstTsDataInfo->u.stDeliver.stUserData.ulLen,
                                              (pgucAtSndCodeAddr + usLength));

            }
            /* <data> 有可能得到是UCS2，需仔细处理*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               "%s",
                                               gaucAtCrLf);

            usLength += (TAF_UINT16)At_PrintReportData(AT_CMD_MAX_LEN,
                                                      (TAF_INT8 *)pgucAtSndCodeAddr,
                                                       pstTsDataInfo->u.stDeliver.stDcs.enMsgCoding,
                                                       (pgucAtSndCodeAddr + usLength),
                                                       pstTsDataInfo->u.stDeliver.stUserData.aucOrgData,
                                                       (TAF_UINT16)pstTsDataInfo->u.stDeliver.stUserData.ulLen);

            break;
        case MN_MSG_TPDU_SUBMIT:
            /*+CMGR: <stat>,<da>,[<alpha>][,<toda>,<fo>,<pid>,<dcs>,[<vp>], <sca>,<tosca>,<length>]<CR><LF><data>*/
            /* <da> */
            usLength += (TAF_UINT16)At_PrintAsciiAddr(&pstTsDataInfo->u.stSubmit.stDestAddr,
                                                      (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");
            /* <alpha> 不报 */

            if (AT_CSDH_SHOW_TYPE == pstSmsCtx->ucCsdhType)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",");

                /* <toda> */
                usLength += (TAF_UINT16)At_PrintAddrType(&pstTsDataInfo->u.stSubmit.stDestAddr,
                                                         (pgucAtSndCodeAddr + usLength));
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",");

                /*<fo>*/
                usLength += (TAF_UINT16)At_PrintMsgFo(pstTsDataInfo, (pgucAtSndCodeAddr + usLength));

                /* <pid> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d",
                                                   pstTsDataInfo->u.stSubmit.enPid);
                /* <dcs> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d,",
                                                   pstTsDataInfo->u.stSubmit.stDcs.ucRawDcsData);
                /* <vp>,还需要仔细处理 */
                usLength += At_MsgPrintVp(&pstTsDataInfo->u.stSubmit.stValidPeriod,
                                          (pgucAtSndCodeAddr + usLength));
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",");

                /* <sca> */
                usLength += At_PrintBcdAddr(&pstEvent->u.stReadInfo.stMsgInfo.stScAddr,
                                            (pgucAtSndCodeAddr + usLength));

                /* <tosca> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d",
                                                   pstEvent->u.stReadInfo.stMsgInfo.stScAddr.addrType);

                /* <length> */
                usLength += AT_PrintSmsLength(pstTsDataInfo->u.stSubmit.stDcs.enMsgCoding,
                                              pstTsDataInfo->u.stSubmit.stUserData.ulLen,
                                              (pgucAtSndCodeAddr + usLength));
            }

            /* <data> 有可能得到是UCS2，需仔细处理*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               "%s",
                                               gaucAtCrLf);

            usLength += (TAF_UINT16)At_PrintReportData(AT_CMD_MAX_LEN,
                                                       (TAF_INT8 *)pgucAtSndCodeAddr,
                                                       pstTsDataInfo->u.stSubmit.stDcs.enMsgCoding,
                                                       (pgucAtSndCodeAddr + usLength),
                                                       pstTsDataInfo->u.stSubmit.stUserData.aucOrgData,
                                                       (TAF_UINT16)pstTsDataInfo->u.stSubmit.stUserData.ulLen);

            break;
        case MN_MSG_TPDU_COMMAND:
            /*+CMGR: <stat>,<fo>,<ct>[,<pid>,[<mn>],[<da>],[<toda>],<length><CR><LF><cdata>]*/
            /*<fo>*/
            usLength += (TAF_UINT16)At_PrintMsgFo(pstTsDataInfo, (pgucAtSndCodeAddr + usLength));
            /* <ct> */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",%d",
                                               pstTsDataInfo->u.stCommand.enCmdType);

            if (AT_CSDH_SHOW_TYPE == pstSmsCtx->ucCsdhType)
            {
                /* <pid> */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d",
                                                   pstTsDataInfo->u.stCommand.enPid);

                /* <mn>,还需要仔细处理 */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d,",
                                                   pstTsDataInfo->u.stCommand.ucMsgNumber);

                /* <da> */
                usLength += (TAF_UINT16)At_PrintAsciiAddr(&pstTsDataInfo->u.stCommand.stDestAddr,
                                                         (pgucAtSndCodeAddr + usLength));
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",");

                /* <toda> */
                usLength += (TAF_UINT16)At_PrintAddrType(&pstTsDataInfo->u.stCommand.stDestAddr,
                                                         (pgucAtSndCodeAddr + usLength));

                /* <length>为0 */
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   ",%d",
                                                   pstTsDataInfo->u.stCommand.ucCommandDataLen);

                /* <data> 有可能得到是UCS2，需仔细处理*/
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                   "%s",
                                                   gaucAtCrLf);

                usLength += (TAF_UINT16)At_PrintReportData(AT_CMD_MAX_LEN,
                                                           (TAF_INT8 *)pgucAtSndCodeAddr,
                                                           MN_MSG_MSG_CODING_8_BIT,
                                                           (pgucAtSndCodeAddr + usLength),
                                                           pstTsDataInfo->u.stCommand.aucCmdData,
                                                           pstTsDataInfo->u.stCommand.ucCommandDataLen);

            }
            break;
        case MN_MSG_TPDU_STARPT:
            /*
            +CMGR: <stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>
            */
            /*<fo>*/
            usLength += (TAF_UINT16)At_PrintMsgFo(pstTsDataInfo, (pgucAtSndCodeAddr + usLength));

            /*<mr>*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",%d,",
                                               pstTsDataInfo->u.stStaRpt.ucMr);

            /*<ra>*/
            usLength += (TAF_UINT16)At_PrintAsciiAddr(&pstTsDataInfo->u.stStaRpt.stRecipientAddr,
                                                      (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

            /*<tora>*/
            usLength += (TAF_UINT16)At_PrintAddrType(&pstTsDataInfo->u.stStaRpt.stRecipientAddr,
                                                     (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

            /* <scts> */
            usLength += (TAF_UINT16)At_SmsPrintScts(&pstTsDataInfo->u.stStaRpt.stTimeStamp,
                                                    (pgucAtSndCodeAddr + usLength));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               ",");

             /* <dt> */
             usLength += (TAF_UINT16)At_SmsPrintScts(&pstTsDataInfo->u.stStaRpt.stDischargeTime,
                                                     (pgucAtSndCodeAddr + usLength));

             /*<st>*/
             usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                                ",%d",
                                                pstTsDataInfo->u.stStaRpt.enStatus);
            break;
        default:
            break;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);
    return;
}


TAF_VOID  At_ListRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16                          usLength;
    TAF_UINT32                          ulRet = AT_OK;
    MN_MSG_TS_DATA_INFO_STRU            *pstTsDataInfo;
    TAF_UINT32                          ulLoop;

    MN_MSG_LIST_PARM_STRU               stListParm;

    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if (TAF_TRUE != pstEvent->u.stListInfo.bSuccess)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stListInfo.ulFailCause);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    usLength = 0;
    if (VOS_TRUE == pstEvent->u.stListInfo.bFirstListEvt)
    {
        if (AT_V_ENTIRE_TYPE == gucAtVType)
        {
            usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           "%s",
                                           gaucAtCrLf);
        }
    }
    pstTsDataInfo = At_GetMsgMem();

    for (ulLoop = 0; ulLoop < pstEvent->u.stListInfo.ulReportNum; ulLoop++)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s: %d,",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstEvent->u.stListInfo.astSmInfo[ulLoop].ulIndex);

        usLength += (TAF_UINT16)At_SmsPrintState(pstSmsCtx->enCmgfMsgFormat,
                                                 pstEvent->u.stListInfo.astSmInfo[ulLoop].enStatus,
                                                 (pgucAtSndCodeAddr + usLength));
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           ",");
        /**/
        ulRet = MN_MSG_Decode(&pstEvent->u.stListInfo.astSmInfo[ulLoop].stMsgInfo.stTsRawData, pstTsDataInfo);
        if (MN_ERR_NO_ERROR != ulRet)
        {
            ulRet = At_ChgMnErrCodeToAt(ucIndex, ulRet);
            At_FormatResultData(ucIndex, ulRet);
            return;
        }

        if (AT_CMGF_MSG_FORMAT_PDU == pstSmsCtx->enCmgfMsgFormat)/*PDU*/
        {
            /*
            +CMGL: <index>,<stat>,[<alpha>],<length><CR><LF><pdu>
            [<CR><LF>+CMGL:<index>,<stat>,[<alpha>],<length><CR><LF><pdu>
            [...]]
            */
            usLength += (TAF_UINT16)At_MsgPduInd(&pstEvent->u.stListInfo.astSmInfo[ulLoop].stMsgInfo.stScAddr,/*??*/
                                                 &pstEvent->u.stListInfo.astSmInfo[ulLoop].stMsgInfo.stTsRawData,
                                                 (pgucAtSndCodeAddr + usLength));
        }
        else
        {
            usLength += (TAF_UINT16)At_PrintListMsg(ucIndex, pstEvent, pstTsDataInfo, (pgucAtSndCodeAddr + usLength));
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        usLength = 0;
    }

    if (TAF_TRUE == pstEvent->u.stListInfo.bLastListEvt)
    {
        gstAtSendData.usBufLen = 0;
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {

        /* 初始化 */
        PS_MEM_CPY( &stListParm, &(pstEvent->u.stListInfo.stReceivedListPara), sizeof(stListParm) );

        /* 通知SMS还需要继续显示剩下的短信 */
        stListParm.ucIsFirstTimeReq = VOS_FALSE;

        /* 执行命令操作 */
        if (MN_ERR_NO_ERROR != MN_MSG_List( gastAtClientTab[ucIndex].usClientId,
                                            gastAtClientTab[ucIndex].opId,
                                            &stListParm) )
        {
            gstAtSendData.usBufLen = 0;
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex, AT_ERROR);
            return;
        }

    }

    return;
}

/*****************************************************************************
 函 数 名  : At_WriteSmRspProc
 功能描述  : 接收到CMGW命令的响应事件的处理:
 输入参数  : ucIndex    - 用户索引
             pstEvent   - CMGW命令的响应事件
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月15日
    作    者   : fuyingjun
    修改内容   : 新生成函数

*****************************************************************************/
TAF_VOID At_WriteSmRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet               = AT_OK;
    TAF_UINT16                          usLength            = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    if (TAF_TRUE != pstEvent->u.stWriteInfo.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stWriteInfo.ulFailCause);
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d",
                                           pstEvent->u.stWriteInfo.ulIndex);

    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulRet);
    return;
}

/*****************************************************************************
 函 数 名  : At_SetCnmaRspProc
 功能描述  : 接收到CNMA命令的响应事件的处理:
 输入参数  : ucIndex    - 用户索引
             pstEvent   - CNMA命令的响应事件
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年3月15日
    作    者   : fuyingjun
    修改内容   : 新生成函数

*****************************************************************************/
TAF_VOID At_SetCnmaRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    if ((AT_CMD_CNMA_TEXT_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
     || (AT_CMD_CNMA_PDU_SET == gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_OK);
    }
    return;
}



VOS_UINT32 AT_GetSmsRpReportCause(TAF_MSG_ERROR_ENUM_UINT32 enMsgCause)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMapLength;

    /* 27005 3.2.5 0...127 3GPP TS 24.011 [6] clause E.2 values */
    /* 27005 3.2.5 128...255 3GPP TS 23.040 [3] clause 9.2.3.22 values.  */
    ulMapLength = sizeof(g_astAtSmsErrorCodeMap) / sizeof(g_astAtSmsErrorCodeMap[0]);

    for (i = 0; i < ulMapLength; i++)
    {
        if (g_astAtSmsErrorCodeMap[i].enMsgErrorCode == enMsgCause)
        {
            return g_astAtSmsErrorCodeMap[i].enAtErrorCode;
        }
    }

    return AT_CMS_UNKNOWN_ERROR;
}


TAF_VOID At_SendSmRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet               = AT_OK;
    TAF_UINT16                          usLength            = 0;

    AT_INFO_LOG("At_SendSmRspProc: step into function.");

    /* 状态不匹配: 当前没有等待发送结果的AT命令，丢弃该结果事件上报 */
    if ((AT_CMD_CMGS_TEXT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CMGS_PDU_SET  != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CMGC_TEXT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CMGC_PDU_SET  != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CMSS_SET      != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CMST_SET      != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        return;
    }

    gstAtSendData.usBufLen = 0;

    if (TAF_MSG_ERROR_NO_ERROR != pstEvent->u.stSubmitRptInfo.enErrorCode)
    {
        AT_NORM_LOG("At_SendSmRspProc: pstEvent->u.stSubmitRptInfo.enRptStatus is not ok.");

        ulRet = AT_GetSmsRpReportCause(pstEvent->u.stSubmitRptInfo.enErrorCode);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    if (gastAtClientTab[ucIndex].AtSmsData.ucMsgSentSmNum < 1)
    {
        AT_WARN_LOG("At_SendSmRspProc: the number of sent message is zero.");
        return;
    }
    gastAtClientTab[ucIndex].AtSmsData.ucMsgSentSmNum--;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d",
                                       pstEvent->u.stSubmitRptInfo.ucMr);

    if (0 == gastAtClientTab[ucIndex].AtSmsData.ucMsgSentSmNum)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = usLength;
        At_FormatResultData(ucIndex, ulRet);
    }
    else
    {
        At_MsgResultCodeFormat(ucIndex, usLength);
    }
    return;
}

#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))
VOS_VOID At_SmsDeliverCbmProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if ((AT_CNMI_MODE_SEND_OR_DISCARD_TYPE == pstSmsCtx->stCnmiType.CnmiModeType)
     || (AT_CNMI_MODE_SEND_OR_BUFFER_TYPE == pstSmsCtx->stCnmiType.CnmiModeType))
    {
        At_ForwardMsgToTe(MN_MSG_EVT_DELIVER_CBM, pstEvent);
        return;
    }

    /* 目前CBS消息不缓存 */
    #if 0
    if ((AT_CNMI_MODE_BUFFER_TYPE == AT_GetModemSmsCtxAddrFromClientId(ucIndex)->stCnmiType.CnmiModeType)
     || (AT_CNMI_MODE_SEND_OR_BUFFER_TYPE == AT_GetModemSmsCtxAddrFromClientId(ucIndex)->stCnmiType.CnmiModeType))
    {
        At_BufferMsgInTa(MN_MSG_EVT_DELIVER_CBM, pstEvent);
        return;
    }
    #endif

    if (AT_CNMI_MODE_EMBED_AND_SEND_TYPE == pstSmsCtx->stCnmiType.CnmiModeType)
    {
        /*目前不支持*/
    }

    return;
}



VOS_UINT32  AT_CbPrintRange(
    VOS_UINT16                          usLength,
    TAF_CBA_CBMI_RANGE_LIST_STRU       *pstCbMidr
)
{
    TAF_UINT32                          ulLoop;
    TAF_UINT16                          usAddLen;

    usAddLen = usLength;
    for (ulLoop = 0; ulLoop < pstCbMidr->usCbmirNum; ulLoop++)
    {
        if ( pstCbMidr->astCbmiRangeInfo[ulLoop].usMsgIdFrom
            == pstCbMidr->astCbmiRangeInfo[ulLoop].usMsgIdTo)
        {

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d",
                                               pstCbMidr->astCbmiRangeInfo[ulLoop].usMsgIdFrom);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d-%d",
                                           pstCbMidr->astCbmiRangeInfo[ulLoop].usMsgIdFrom,
                                           pstCbMidr->astCbmiRangeInfo[ulLoop].usMsgIdTo);

        }

        if (ulLoop != (pstCbMidr->usCbmirNum - 1))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",");
        }
    }

    usAddLen = usLength - usAddLen;

    return usAddLen ;
}


VOS_VOID At_GetCbActiveMidsRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT16                          usLength;
    TAF_UINT16                          usAddLength;
    VOS_UINT32                          ulRet;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    /* 停止定时器 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_TRUE != pstEvent->u.stCbsCbMids.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stCbsCbMids.ulFailCause);
        At_FormatResultData(ucIndex, ulRet);
        return;
    }

    usLength = 0;

    /* 保持的永远是激活列表,所以固定填写0 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s:0,",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "\"");

    /* 输出消息的MID */
    usAddLength = (VOS_UINT16)AT_CbPrintRange(usLength,&(pstEvent->u.stCbsCbMids.stCbMidr));

    usLength += usAddLength;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "\",\"");

    /* 输出的语言的MID */
    usAddLength = (VOS_UINT16)AT_CbPrintRange(usLength,&(pstSmsCtx->stCbsDcssInfo));

    usLength += usAddLength;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "\"");


    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);
    return;
}



VOS_VOID AT_ChangeCbMidsRsp(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    TAF_UINT32                          ulRet;

    if (TAF_TRUE != pstEvent->u.stCbsChangeInfo.bSuccess)
    {
        ulRet = At_ChgMnErrCodeToAt(ucIndex, pstEvent->u.stCbsChangeInfo.ulFailCause);
    }
    else
    {
        ulRet = AT_OK;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

}


#if (FEATURE_ON == FEATURE_ETWS)

VOS_VOID  At_ProcDeliverEtwsPrimNotify(
    VOS_UINT8                                               ucIndex,
    MN_MSG_EVENT_INFO_STRU                                 *pstEvent
)
{
    TAF_CBA_ETWS_PRIM_NTF_EVT_INFO_STRU                    *pstPrimNtf;
    VOS_UINT16                                              usLength;

    pstPrimNtf = &pstEvent->u.stEtwsPrimNtf;

    /* ^ETWSPN: <plmn id>,<warning type>,<msg id>,<sn>,<auth> [,<warning security information>] */
    /* 示例: ^ETWSPN: "46000",0180,4352,3000,1 */

    usLength   = 0;
    usLength  += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%s^ETWSPN: ",
                                       gaucAtCrLf);

    /* <plmn id>
       ulMcc，ulMnc的说明及示例：
       ulMcc的低8位    （即bit0--bit7），对应 MCC digit 1;
       ulMcc的次低8位  （即bit8--bit15），对应 MCC digit 2;
       ulMcc的次次低8位（即bit16--bit23），对应 MCC digit 3;

       ulMnc的低8位    （即bit0--bit7），对应 MNC digit 1;
       ulMnc的次低8位  （即bit8--bit15），对应 MNC digit 2;
       ulMnc的次次低8位（即bit16--bit23），对应 MNC digit 3;
    */
    if ( 0x0F0000 == (pstPrimNtf->stPlmn.ulMnc&0xFF0000) )
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "\"%d%d%d%d%d\",",
                                           (pstPrimNtf->stPlmn.ulMcc&0xFF),
                                           (pstPrimNtf->stPlmn.ulMcc&0xFF00)>>8,
                                           (pstPrimNtf->stPlmn.ulMcc&0xFF0000)>>16,
                                           (pstPrimNtf->stPlmn.ulMnc&0xFF),
                                           (pstPrimNtf->stPlmn.ulMnc&0xFF00)>>8);
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "\"%d%d%d%d%d%d\",",
                                           (pstPrimNtf->stPlmn.ulMcc&0xFF),
                                           (pstPrimNtf->stPlmn.ulMcc&0xFF00)>>8,
                                           (pstPrimNtf->stPlmn.ulMcc&0xFF0000)>>16,
                                           (pstPrimNtf->stPlmn.ulMnc&0xFF),
                                           (pstPrimNtf->stPlmn.ulMnc&0xFF00)>>8,
                                           (pstPrimNtf->stPlmn.ulMnc&0xFF0000)>>16);
    }

    /* <warning type> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%04X,",
                                       pstPrimNtf->usWarnType);
    /* <msg id> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%04X,",
                                       pstPrimNtf->usMsgId);
    /* <sn> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%04X,",
                                       pstPrimNtf->usSN);

    /* <auth> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                       "%d%s",
                                       pstPrimNtf->enAuthRslt,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

}
#endif  /* (FEATURE_ON == FEATURE_ETWS) */


#endif
TAF_VOID At_SetCmmsRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_RRETURN_CODE_ENUM_UINT32         ulResult = AT_CMS_UNKNOWN_ERROR;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MN_ERR_NO_ERROR == pstEvent->u.stLinkCtrlInfo.ulErrorCode)
    {
        ulResult = AT_OK;
    }

    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex,ulResult);
    return;
}


TAF_VOID At_GetCmmsRspProc(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_RRETURN_CODE_ENUM_UINT32          ulResult = AT_CMS_UNKNOWN_ERROR;
    MN_MSG_LINK_CTRL_EVT_INFO_STRU      *pstLinkCtrlInfo;                     /*event report:MN_MSG_EVT_SET_COMM_PARAM*/

    gstAtSendData.usBufLen = 0;
    pstLinkCtrlInfo = &pstEvent->u.stLinkCtrlInfo;
    if (MN_ERR_NO_ERROR == pstLinkCtrlInfo->ulErrorCode)
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstLinkCtrlInfo->enLinkCtrl);
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);
    return;
}
TAF_VOID At_SmsRspNop(
    TAF_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU              *pstEvent
)
{
    AT_INFO_LOG("At_SmsRspNop: no operation need for the event type ");
    return;
}


TAF_VOID At_SmsMsgProc(MN_AT_IND_EVT_STRU *pstData,TAF_UINT16 usLen)
{
    MN_MSG_EVENT_INFO_STRU              *pstEvent;
    MN_MSG_EVENT_ENUM_U32               enEvent;
    TAF_UINT8                           ucIndex;
    TAF_UINT32                          ulEventLen;


    enEvent = MN_MSG_EVT_MAX;


    AT_INFO_LOG("At_SmsMsgProc: Step into function.");
    AT_LOG1("At_SmsMsgProc: pstData->clientId,", pstData->clientId);

    ulEventLen = sizeof(MN_MSG_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);
    pstEvent = (MN_MSG_EVENT_INFO_STRU *)&pstData->aucContent[ulEventLen];

    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_SmsMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (!AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        #ifndef __PS_WIN32_RECUR__
        if (pstEvent->opId != gastAtClientTab[ucIndex].opId)
        {
            AT_LOG1("At_SmsMsgProc: pstEvent->opId,", pstEvent->opId);
            AT_LOG1("At_SmsMsgProc: gastAtClientTab[ucIndex].opId,", gastAtClientTab[ucIndex].opId);
            AT_NORM_LOG("At_SmsMsgProc: invalid operation id.");
            return;
        }
        #endif

        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);
    }

    if (enEvent >= MN_MSG_EVT_MAX)
    {
        AT_WARN_LOG("At_SmsRspProc: invalid event type.");
        return;
    }

    AT_LOG1("At_SmsMsgProc enEvent", enEvent);
    g_aAtSmsMsgProcTable[enEvent](ucIndex, pstEvent);
    return;
}


VOS_UINT32 At_ProcVcGetVolumeEvent(
    VOS_UINT8                           ucIndex,
    APP_VC_EVENT_INFO_STRU             *pstVcEvt
)
{
    VOS_UINT8                           aucIntraVolume[] = {AT_CMD_CLVL_LEV_0,AT_CMD_CLVL_LEV_1,
                                                            AT_CMD_CLVL_LEV_2,AT_CMD_CLVL_LEV_3,
                                                            AT_CMD_CLVL_LEV_4,AT_CMD_CLVL_LEV_5};
    VOS_UINT8                           ucVolumnLvl;
    VOS_UINT32                          i;

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("APP_VC_AppQryVolumeProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CLVL_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_TRUE == pstVcEvt->bSuccess)
    {
        /* 格式化AT+CLVL命令返回 */
        gstAtSendData.usBufLen = 0;

        ucVolumnLvl = 0;
        for (i = 0; i < 6; i++)
        {
            if (aucIntraVolume[i] == pstVcEvt->usVolume)
            {
                ucVolumnLvl = (VOS_UINT8)i;
                break;
            }
        }

        gstAtSendData.usBufLen =
            (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s: %d",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                   ucVolumnLvl);

        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}
VOS_UINT32 At_ProcVcSetMuteStatusEvent(
    VOS_UINT8                           ucIndex,
    APP_VC_EVENT_INFO_STRU             *pstVcEvtInfo
)
{
    VOS_UINT32                          ulRslt;

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_ProcVcSetMuteStatusEvent : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CMUT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    if (VOS_TRUE != pstVcEvtInfo->bSuccess)
    {
        ulRslt = AT_ERROR;
    }
    else
    {
        ulRslt = AT_OK;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}
VOS_UINT32 At_ProcVcGetMuteStatusEvent(
    VOS_UINT8                           ucIndex,
    APP_VC_EVENT_INFO_STRU             *pstVcEvtInfo
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usLength = 0;

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_ProcVcSetMuteStatusEvent : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CMUT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    if (VOS_TRUE == pstVcEvtInfo->bSuccess)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: %d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstVcEvtInfo->enMuteStatus);

        ulRslt = AT_OK;

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        ulRslt = AT_ERROR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}
VOS_VOID At_VcEventProc(
    VOS_UINT8                           ucIndex,
    APP_VC_EVENT_INFO_STRU              *pstVcEvt,
    APP_VC_EVENT_ENUM_U32               enEvent
)
{
    TAF_UINT32                          ulRet;
    switch (enEvent)
    {
        case APP_VC_EVT_SET_VOICE_MODE:
        case APP_VC_EVT_SET_VOLUME:
            if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
            {
                AT_WARN_LOG("At_VcEventProc : AT_BROADCAST_INDEX.");
                return;
            }

            if (TAF_TRUE != pstVcEvt->bSuccess)
            {
                ulRet = AT_ERROR;
            }
            else
            {
                ulRet = AT_OK;
            }

#if (FEATURE_ON == FEATURE_MULTI_MODEM)
            /* 当前AT是否在等待^VMSET命令返回 */
            if (AT_CMD_VMSET_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
            {

                gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
                return;
            }
#endif

            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex, ulRet);
            return;

        case APP_VC_EVT_GET_VOLUME:
            At_ProcVcGetVolumeEvent(ucIndex, pstVcEvt);
            return;

        case APP_VC_EVT_PARM_CHANGED:
            return;

        case APP_VC_EVT_SET_MUTE_STATUS:
            At_ProcVcSetMuteStatusEvent(ucIndex, pstVcEvt);
            return;

        case APP_VC_EVT_GET_MUTE_STATUS:
            At_ProcVcGetMuteStatusEvent(ucIndex, pstVcEvt);
            return;

        default:
            return;
    }

}
TAF_VOID At_VcMsgProc(MN_AT_IND_EVT_STRU *pstData,TAF_UINT16 usLen)
{
    APP_VC_EVENT_INFO_STRU              *pstEvent;
    APP_VC_EVENT_ENUM_U32               enEvent;
    TAF_UINT8                           ucIndex;
    TAF_UINT32                          ulEventLen;


    enEvent = APP_VC_EVT_BUTT;


    AT_INFO_LOG("At_VcMsgProc: Step into function.");
    AT_LOG1("At_VcMsgProc: pstData->clientId,", pstData->clientId);

    ulEventLen = sizeof(APP_VC_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);
    pstEvent = (APP_VC_EVENT_INFO_STRU *)&pstData->aucContent[ulEventLen];

    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_VcMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (!AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_LOG1("At_VcMsgProc: ucIndex", ucIndex);
        if (ucIndex >= AT_MAX_CLIENT_NUM)
        {
            AT_WARN_LOG("At_VcMsgProc: invalid CLIENT ID or index.");
            return;
        }

        if (pstEvent->opId != gastAtClientTab[ucIndex].opId)
        {
            AT_LOG1("At_VcMsgProc: pstEvent->opId,", pstEvent->opId);
            AT_LOG1("At_VcMsgProc: gastAtClientTab[ucIndex].opId,", gastAtClientTab[ucIndex].opId);
            AT_NORM_LOG("At_VcMsgProc: invalid operation id.");
            return;
        }

        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);
    }

    if (enEvent >= APP_VC_EVT_BUTT)
    {
        AT_WARN_LOG("At_SmsRspProc: invalid event type.");
        return;
    }

    AT_LOG1("At_VcMsgProc enEvent", enEvent);
    At_VcEventProc(ucIndex,pstEvent,enEvent);


}


TAF_VOID At_SetParaRspProc( TAF_UINT8 ucIndex,
                                      TAF_UINT8 OpId,
                                      TAF_PARA_SET_RESULT Result,
                                      TAF_PARA_TYPE ParaType,
                                      TAF_VOID *pPara)
{
    AT_RRETURN_CODE_ENUM_UINT32         ulResult = AT_FAILURE;
    TAF_UINT16 usLength = 0;

    /* 如果是PS域的复合命令 */
    if(gastAtClientTab[ucIndex].usAsyRtnNum > 0)
    {
        gastAtClientTab[ucIndex].usAsyRtnNum--;         /* 命令个数减1 */
        if(TAF_PARA_OK == Result)
        {
            if(0 != gastAtClientTab[ucIndex].usAsyRtnNum)
            {
                return;                                 /* 如果OK并且还有其它命令 */
            }
        }
        else
        {
            gastAtClientTab[ucIndex].usAsyRtnNum = 0;   /* 如果ERROR则不再上报其它命令结果 */
        }
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    switch(Result)
    {
    case TAF_PARA_OK:
        ulResult = AT_OK;
        break;

    case TAF_PARA_SIM_IS_BUSY:
        if(g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex > AT_CMD_SMS_BEGAIN)
        {
            ulResult = AT_CMS_U_SIM_BUSY;
        }
        else
        {
            ulResult = AT_CME_SIM_BUSY;
        }
        break;

    default:
        if(g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex > AT_CMD_SMS_BEGAIN)
        {
            ulResult = AT_CMS_UNKNOWN_ERROR;
        }
        else
        {
            ulResult = AT_CME_UNKNOWN;
        }
        break;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}

TAF_VOID At_SetMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    TAF_UINT16 ClientId = 0;
    TAF_UINT8 OpId = 0;
    TAF_PARA_SET_RESULT Result = 0;
    TAF_PARA_TYPE ParaType = 0;
    TAF_VOID *pPara = TAF_NULL_PTR;
    TAF_UINT16 usAddr = 0;
    TAF_UINT16 usParaLen = 0;
    TAF_UINT8 ucIndex  = 0;

    PS_MEM_CPY(&ClientId,pData,sizeof(ClientId));
    usAddr += sizeof(ClientId);

    PS_MEM_CPY(&OpId,pData+usAddr,sizeof(OpId));
    usAddr += sizeof(OpId);

    PS_MEM_CPY(&Result,pData+usAddr,sizeof(Result));
    usAddr += sizeof(Result);

    PS_MEM_CPY(&ParaType,pData+usAddr,sizeof(ParaType));
    usAddr += sizeof(ParaType);

    PS_MEM_CPY(&usParaLen,pData+usAddr,sizeof(usParaLen));
    usAddr += sizeof(usParaLen);

    if(0 != usParaLen)
    {
        pPara = pData+usAddr;
    }

    AT_LOG1("At_SetMsgProc ClientId",ClientId);
    AT_LOG1("At_SetMsgProc Result",Result);
    AT_LOG1("At_SetMsgProc ParaType",ParaType);

    if(AT_BUTT_CLIENT_ID == ClientId)
    {
        AT_WARN_LOG("At_SetMsgProc Error ucIndex");
        return;
    }
    else
    {
        if(AT_FAILURE == At_ClientIdToUserId(ClientId,&ucIndex))
        {
            AT_WARN_LOG("At_SetMsgProc At_ClientIdToUserId FAILURE");
            return;
        }

        if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
        {
            AT_WARN_LOG("At_SetMsgProc : AT_BROADCAST_INDEX.");
            return;
        }

        AT_LOG1("At_SetMsgProc ucIndex",ucIndex);
        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

        At_SetParaRspProc(ucIndex,OpId,Result,ParaType,pPara);
    }
}



#if 0

VOS_UINT32 AT_GetLCStartTime(VOS_VOID)
{
    VOS_UINT32                          ulSlice;
    VOS_UINT32                          ulTotalSec;

    ulSlice = OM_GetSlice();

    /*由于Slice值由全F递减，首先需要进行调整*/
    ulSlice = 0xffffffffU - ulSlice;

    /*Slice值每隔一秒增加32768，通过以下计算转换成10ms的tick值
    而先右移7位，再乘以100，是为了防止数据过大而溢出*/
    ulSlice >>= 7;
    ulSlice *= 100;
    ulSlice >>= 8;

    /* 1tick相当于10ms，将tick转换为秒 */
    ulTotalSec = ulSlice /100;
    return ulTotalSec;

}
#endif
VOS_UINT16 AT_GetOperNameLengthForCops(
    TAF_CHAR                            *pstr,
    TAF_UINT8                           ucMaxLen
)
{
    VOS_UINT16                          usRsltLen;
    TAF_UINT8                           i;

    usRsltLen = 0;

    for (i = 0; i < ucMaxLen; i++)
    {
        if ('\0' != pstr[i])
        {
            usRsltLen = i+1;
        }
    }

    return usRsltLen;

}

VOS_VOID At_QryParaRspCopsProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT16                          usNameLength = 0;
    TAF_PH_NETWORKNAME_STRU             stCops;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_CPY(&stCops, pPara, sizeof(TAF_PH_NETWORKNAME_STRU));

    /* A32D07158
     * +COPS: <mode>[,<format>,<oper>[,<AcT>]], get the PLMN selection mode from msg sent by MMA
     */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: %d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       stCops.PlmnSelMode);

    if ((0 == stCops.Name.PlmnId.Mcc)
     && (0 == stCops.Name.PlmnId.Mnc))
    {
        /* 无效 PLMNId 只显示 sel mode */
        ulResult = AT_OK;
        gstAtSendData.usBufLen = usLength;
        At_FormatResultData(ucIndex,ulResult);
        return;
    }

    /* <format> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%d",
                                       pstNetCtx->ucCopsFormatType);

    /* <oper> */
    switch (pstNetCtx->ucCopsFormatType)
    {
        /* 长名字，字符串类型 */
        case AT_COPS_LONG_ALPH_TYPE:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"");

            /*针对显示SPN中存在0x00有效字符的情况,获取其实际长度,At_sprintf以0x00结尾，不可用，使用PS_MEM_CPY代替*/
            usNameLength = AT_GetOperNameLengthForCops(stCops.Name.aucOperatorNameLong, TAF_PH_OPER_NAME_LONG);

            PS_MEM_CPY(pgucAtSndCodeAddr + usLength, stCops.Name.aucOperatorNameLong, usNameLength);

            usLength = usLength + usNameLength;
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "\"");
            break;

       /* 短名字，字符串类型 */
        case AT_COPS_SHORT_ALPH_TYPE:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"");

            /*针对显示SPN中存在0x00有效字符的情况,获取其实际长度,At_sprintf以0x00结尾，不可用，使用PS_MEM_CPY代替*/
            usNameLength = AT_GetOperNameLengthForCops(stCops.Name.aucOperatorNameShort, TAF_PH_OPER_NAME_SHORT);

            PS_MEM_CPY(pgucAtSndCodeAddr + usLength, stCops.Name.aucOperatorNameShort, usNameLength);

            usLength = usLength + usNameLength;
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "\"");
            break;

        /* BCD码的MCC、MNC，需要转换成字符串 */
        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"%X%X%X",
                                               (0x0f00 & stCops.Name.PlmnId.Mcc) >> 8,
                                               (0x00f0 & stCops.Name.PlmnId.Mcc) >> 4,
                                               (0x000f & stCops.Name.PlmnId.Mcc));

            if( 0x0F != ((0x0f00 & stCops.Name.PlmnId.Mnc) >> 8))
            {
                usLength +=(VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  "%X",
                                                  (0x0f00 & stCops.Name.PlmnId.Mnc) >> 8);
            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%X%X\"",
                                               (0x00f0 & stCops.Name.PlmnId.Mnc) >> 4,
                                               (0x000f & stCops.Name.PlmnId.Mnc));
            break;
    }

    /* <AcT> */
    if(TAF_PH_RA_GSM == stCops.RaMode)  /* GSM */
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",0");
    }
    else if(TAF_PH_RA_WCDMA == stCops.RaMode)   /* CDMA */
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",2");
    }
    else if(TAF_PH_RA_LTE == stCops.RaMode)   /* CDMA */
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",7");
    }
    else
    {

    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulResult);

}

/*****************************************************************************
 Prototype      : At_QryParaRspCgmiProc
 Description    : 参数查询结果Cgmi的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCgmiProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_FMR_ID_STRU                  stCgmi;

    PS_MEM_CPY(&stCgmi, pPara, sizeof(TAF_PH_FMR_ID_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stCgmi.aucMfrId);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

#if(FEATURE_ON == FEATURE_LTE)
TAF_VOID At_QryParaRspCellRoamProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_CELLROAM_STRU                  stCellRoam;

    PS_MEM_CPY(&stCellRoam, pPara, sizeof(TAF_PH_CELLROAM_STRU));

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
        "%s:%d,%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                 stCellRoam.RoamMode,
                 stCellRoam.RaMode);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}
#endif


/*****************************************************************************
 Prototype      : At_QryParaRspCgmmProc
 Description    : 参数查询结果Cgmm的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCgmmProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_MODEL_ID_STRU                stCgmm;

    PS_MEM_CPY(&stCgmm, pPara, sizeof(TAF_PH_MODEL_ID_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stCgmm.aucModelId);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

/*****************************************************************************
 Prototype      : At_QryParaRspCgmrProc
 Description    : 参数查询结果Cgmr的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCgmrProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_REVISION_ID_STRU             stCgmr;

    PS_MEM_CPY(&stCgmr, pPara, sizeof(TAF_PH_REVISION_ID_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stCgmr.aucRevisionId);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

/*****************************************************************************
 Prototype      : At_QryParaRspCqstProc
 Description    : 参数查询结果Cqst的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCqstProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s:%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,*((VOS_UINT8 *)pPara));

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}

/*****************************************************************************
 Prototype      : At_QryParaRspCaattProc
 Description    : 参数查询结果Caatt的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCaattProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s:%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,*((VOS_UINT8 *)pPara));

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


TAF_VOID At_QryParaRspSysinfoProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    TAF_PH_SYSINFO_STRU                 stSysInfo;
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    ulResult                            = AT_FAILURE;
    usLength                            = 0;

    PS_MEM_CPY(&stSysInfo, pPara, sizeof(TAF_PH_SYSINFO_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,stSysInfo.ucSrvStatus);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSrvDomain);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucRoamStatus);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSysMode);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSimStatus);

    if ( SYSTEM_APP_WEBUI == *pucSystemAppConfig)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSimLockStatus);
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",");
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSysSubMode);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


VOS_VOID At_QryMmPlmnInfoRspProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    TAF_MMA_MM_INFO_PLMN_NAME_STRU     *pstPlmnName = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           i;

    /* 变量初始化 */
    pstPlmnName = (TAF_MMA_MM_INFO_PLMN_NAME_STRU *)pPara;
    ulResult    = AT_ERROR;

    /* 转换LongName及ShortName */
    if ( pstPlmnName->ucLongNameLen <= TAF_PH_OPER_NAME_LONG
      && pstPlmnName->ucShortNameLen <= TAF_PH_OPER_NAME_SHORT )
    {

        /* ^MMPLMNINFO:<long name>,<short name> */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s:",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        for (i = 0; i < pstPlmnName->ucLongNameLen; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%02X",
                                               pstPlmnName->aucLongName[i]);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr + usLength, ",");

        for (i = 0; i < pstPlmnName->ucShortNameLen; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%02X",
                                               pstPlmnName->aucShortName[i]);
        }

        ulResult = AT_OK;
        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
    }

    At_FormatResultData(ucIndex,ulResult);

    return;
}

/*****************************************************************************
 Prototype      : At_QryParaRspCimiProc
 Description    : 参数查询结果Cimi的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCimiProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_IMSI_STRU                    stCimi;

    PS_MEM_CPY(&stCimi, pPara, sizeof(TAF_PH_IMSI_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stCimi.aucImsi);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

/*****************************************************************************
 Prototype      : At_QryParaRspCgclassProc
 Description    : 参数查询结果Cgclass的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCgclassProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_MS_CLASS_TYPE                stCgclass;

    PS_MEM_CPY(&stCgclass, pPara, sizeof(TAF_PH_MS_CLASS_TYPE));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    if(TAF_PH_MS_CLASS_A == stCgclass)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"A\"");
    }
    else if(TAF_PH_MS_CLASS_B == stCgclass)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"B\"");
    }
    else if(TAF_PH_MS_CLASS_CG == stCgclass)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"CG\"");
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"CC\"");
    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


VOS_VOID At_QryParaRspCregProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;

    TAF_PH_REG_STATE_STRU               stCreg;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_CPY(&stCreg, pPara, sizeof(TAF_PH_REG_STATE_STRU));

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: %d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       (VOS_UINT32)pstNetCtx->ucCregType);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%d",
                                       stCreg.RegState);

    if ((AT_CREG_RESULT_CODE_ENTIRE_TYPE == pstNetCtx->ucCregType)
     && ((TAF_PH_REG_REGISTERED_HOME_NETWORK == stCreg.RegState)
      || (TAF_PH_REG_REGISTERED_ROAM == stCreg.RegState)))
    {
        usLength += (VOS_UINT16)At_PhReadCreg(&stCreg, pgucAtSndCodeAddr + usLength);
    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


VOS_VOID At_QryParaRspCgregProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                            *pPara
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;

    TAF_PH_REG_STATE_STRU               stCgreg;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_CPY(&stCgreg, pPara, sizeof(TAF_PH_REG_STATE_STRU));


    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: %d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       (VOS_UINT32)pstNetCtx->ucCgregType);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%d",
                                       stCgreg.ucPsRegState);

    if ((AT_CGREG_RESULT_CODE_ENTIRE_TYPE == pstNetCtx->ucCgregType)
     && ((TAF_PH_REG_REGISTERED_HOME_NETWORK == stCgreg.ucPsRegState)
      || (TAF_PH_REG_REGISTERED_ROAM == stCgreg.ucPsRegState)))
    {
        usLength += (VOS_UINT16)At_PhReadCreg(&stCgreg, pgucAtSndCodeAddr + usLength);
    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

#if(FEATURE_ON == FEATURE_LTE)

VOS_VOID AT_QryParaRspCeregProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucOpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;

    TAF_PH_REG_STATE_STRU               stCereg;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    PS_MEM_CPY(&stCereg, pPara, sizeof(TAF_PH_REG_STATE_STRU));

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       (VOS_UINT32)pstNetCtx->ucCeregType);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,",%d",
                                       stCereg.ucPsRegState);

    /* 与标杆做成一致，GU下查询只上报stat */
    if ((AT_CEREG_RESULT_CODE_ENTIRE_TYPE == pstNetCtx->ucCeregType)
     && (TAF_PH_ACCESS_TECH_E_UTRAN == stCereg.ucAct)
     && ((TAF_PH_REG_REGISTERED_HOME_NETWORK == stCereg.ucPsRegState)
      || (TAF_PH_REG_REGISTERED_ROAM == stCereg.ucPsRegState)))
    {

        usLength += (VOS_UINT16)At_PhReadCreg(&stCereg, pgucAtSndCodeAddr + usLength);

    }

    ulResult               = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulResult);

}

#endif

/*****************************************************************************
 Prototype      : At_QryParaRspCfunProc
 Description    : 参数查询结果Cfun的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCfunProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_MODE                         Cfun;

    PS_MEM_CPY(&Cfun, pPara, sizeof(TAF_PH_MODE));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,Cfun);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}




TAF_VOID At_QryParaRspCpamProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_ACCESS_MODE_QUERY_PARA_STRU     stCpam;

    PS_MEM_CPY(&stCpam, pPara, sizeof(TAF_ACCESS_MODE_QUERY_PARA_STRU));

    /* 如果查询的接入模式为无效值，则返回ERROR，例如当前LTE only不支持GU */
    if (stCpam.AccessMode >= MN_MMA_CPAM_RAT_TYPE_BUTT)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d,%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,stCpam.AccessMode,stCpam.PlmnPrio);

        ulResult = AT_OK;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

/*****************************************************************************
 Prototype      : At_QryParaRspCsqProc
 Description    : 参数查询结果Csq的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCsqProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_RSSI_STRU                    stCsq;

    PS_MEM_CPY(&stCsq, pPara, sizeof(TAF_PH_RSSI_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    if(stCsq.ucRssiNum > 0)
    {
        /* 增加范围限制 */
        if((stCsq.aRssi[0].ucRssiValue >= 31) && (stCsq.aRssi[0].ucRssiValue < 99))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",31);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",stCsq.aRssi[0].ucRssiValue);
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stCsq.aRssi[0].ucChannalQual);
    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


/*****************************************************************************
 Prototype      : At_QryParaRspCbcProc
 Description    : 参数查询结果Cbc的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspCbcProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    TAF_PH_BATTERYPOWER_STRU            stCbc;

    PS_MEM_CPY(&stCbc, pPara, sizeof(TAF_PH_BATTERYPOWER_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,stCbc.BatteryPowerStatus);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stCbc.BatteryRemains);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


TAF_VOID At_QryParaRspIccidProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult;
    TAF_UINT16                          usLength;
    TAF_PH_ICC_ID_STRU                  stIccId;

    if (AT_CMD_ICCID_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return;
    }

    usLength = 0;
    PS_MEM_CPY(&stIccId, pPara, sizeof(TAF_PH_ICC_ID_STRU));

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (VOS_UINT16)AT_Hex2AsciiStrLowHalfFirst(AT_CMD_MAX_LEN,
                                                        (VOS_INT8 *)pgucAtSndCodeAddr,
                                                        (VOS_UINT8 *)pgucAtSndCodeAddr + usLength,
                                                        stIccId.aucIccId,
                                                        stIccId.ucLen);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulResult);

    return;
}
TAF_VOID At_QryRspUsimRangeProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_PH_QRY_USIM_RANGE_INFO_STRU     *pstUsimRangeInfo;
    TAF_UINT16                          usLength = 0;
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT8                           ucSimValue;
    VOS_BOOL                            bUsimInfoPrinted = VOS_FALSE;

    pstUsimRangeInfo = (TAF_PH_QRY_USIM_RANGE_INFO_STRU*)pPara;
    if((pstUsimRangeInfo->stUsimInfo.bFileExist == VOS_TRUE)
    && (TAF_PH_ICC_USIM == pstUsimRangeInfo->stUsimInfo.Icctype))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        ucSimValue = 1;
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d,(1,%d),%d",
                                           ucSimValue,
                                           pstUsimRangeInfo->stUsimInfo.ulTotalRecNum,
                                           pstUsimRangeInfo->stUsimInfo.ulRecordLen);
        bUsimInfoPrinted = VOS_TRUE;
    }
    if ((pstUsimRangeInfo->stSimInfo.bFileExist == VOS_TRUE)
     && (TAF_PH_ICC_SIM == pstUsimRangeInfo->stSimInfo.Icctype))
    {
        if (VOS_TRUE == bUsimInfoPrinted)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        ucSimValue = 0;
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d,(1,%d),%d",
                                           ucSimValue,
                                           pstUsimRangeInfo->stSimInfo.ulTotalRecNum,
                                           pstUsimRangeInfo->stSimInfo.ulRecordLen);

    }
    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

TAF_VOID At_QryParaRspPnnProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT16                          usLength = 0;

    TAF_PH_USIM_PNN_CNF_STRU            *pstPNN;
    TAF_UINT8                           FullNameLen;
    TAF_UINT8                           ShortNameLen;
    VOS_BOOL                            bShortNamePrinted;
    TAF_UINT32                          RecordLen;
    TAF_UINT8                           *pStrCurrentPos;
    TAF_UINT32                          ulPrintCnt;
    TAF_UINT32                          i;
    TAF_UINT32                          ulRet;
    TAF_PH_QRY_USIM_INFO_STRU           stUsimInfo;

    PS_MEM_SET(&stUsimInfo, 0x00, sizeof(stUsimInfo));

    pstPNN     = (TAF_PH_USIM_PNN_CNF_STRU*)pPara;
    RecordLen  = pstPNN->RecordLen;

    /* 查询PNN记录数和记录长度 */
    for (i = 0 ; i < pstPNN->TotalRecordNum; i++)
    {
        FullNameLen = 0;
        ShortNameLen = 0;
        ulPrintCnt = 0;
        bShortNamePrinted = VOS_FALSE;
        pStrCurrentPos = pstPNN->PNNRecord[i].PNN;

        if (FULL_NAME_IEI != pStrCurrentPos[0])
        {
            continue;
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        FullNameLen = pStrCurrentPos[1];

        /*打印长名*/

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
        usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,pStrCurrentPos,FullNameLen+2);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");

        ulPrintCnt += (FullNameLen + 2);
        pStrCurrentPos = (pstPNN->PNNRecord[i].PNN) + ulPrintCnt;

        if (0 < (RecordLen - ulPrintCnt ))
        {
            ShortNameLen = pStrCurrentPos[1];
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",");
        if (SHORT_NAME_IEI == pStrCurrentPos[0])
        {
            bShortNamePrinted = VOS_TRUE;
            /*打印短名*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength, pStrCurrentPos, ShortNameLen+2);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");

            ulPrintCnt += (ShortNameLen+2);

            pStrCurrentPos = pstPNN->PNNRecord[i].PNN + ulPrintCnt;
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"\"");
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",");


        if (( VOS_TRUE == bShortNamePrinted ) && (0 < (RecordLen - ulPrintCnt )))
        {
            /*PNN的其它信息*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength, pStrCurrentPos, (TAF_UINT16)(RecordLen - ulPrintCnt));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"\"");
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
        usLength = 0;

    }

    /* 如果本次上报的PNN数目与要求的相同，则认为C核仍有PNN没有上报，要继续发送请求进行查询 */
    if (TAF_MMA_PNN_INFO_MAX_NUM == pstPNN->TotalRecordNum)
    {
        stUsimInfo.ulRecNum                     = 0;
        stUsimInfo.enEfId                       = TAF_PH_PNN_FILE;
        stUsimInfo.Icctype                      = pstPNN->Icctype;
        stUsimInfo.stPnnQryIndex.usPnnNum       = TAF_MMA_PNN_INFO_MAX_NUM;
        stUsimInfo.stPnnQryIndex.usPnnCurrIndex = pstPNN->usPnnCurrIndex + TAF_MMA_PNN_INFO_MAX_NUM;

        ulRet = MN_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                       0,
                                       TAF_MSG_MMA_USIM_INFO,
                                       &stUsimInfo,
                                       sizeof(TAF_PH_QRY_USIM_INFO_STRU),
                                       I0_WUEPS_PID_MMA);

        if (TAF_SUCCESS != ulRet)
        {
            /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_ERROR);
        }
    }
    else
    {
        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_OK);
    }

}
TAF_VOID At_QryParaRspCPnnProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    const TAF_UINT8                     ucCodingSchemeMask = 0x70;
    const TAF_UINT8                     ucAddCIMask = 0x08;
    TAF_UINT8                           ucCodingScheme = 0;
    TAF_PH_USIM_PNN_CNF_STRU            *pstPNN;
    TAF_UINT8                           FullNameLen;
    TAF_UINT8                           ShortNameLen;
    TAF_UINT32                          RecordLen;
    TAF_UINT8                           * pStrCurrentPos;
    TAF_UINT32                          ulPrintCnt;

    pstPNN     = (TAF_PH_USIM_PNN_CNF_STRU*)pPara;
    RecordLen  = pstPNN->RecordLen;

    if (0 != pstPNN->TotalRecordNum)
    {
        FullNameLen = 0;
        ShortNameLen = 0;
        ulPrintCnt = 0;
        pStrCurrentPos = pstPNN->PNNRecord[0].PNN;

        if (FULL_NAME_IEI == pStrCurrentPos[0])
        {

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            FullNameLen = pStrCurrentPos[1];

            /*打印长名*/
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,pStrCurrentPos+3,FullNameLen-1);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");

            ucCodingScheme = (pStrCurrentPos[2] & ucCodingSchemeMask);

            if ( 0 != ucCodingScheme )
            {
                ucCodingScheme = 1;
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d,%d",
                                                ucCodingScheme,(pStrCurrentPos[2] & ucAddCIMask)>>3 );

            ulPrintCnt += (FullNameLen + 2);
            pStrCurrentPos = (pstPNN->PNNRecord[0].PNN) + ulPrintCnt;

            if (0 < (RecordLen - ulPrintCnt ))
            {
                ShortNameLen = pStrCurrentPos[1];
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",");

            if (SHORT_NAME_IEI == pStrCurrentPos[0])
            {
                /*打印短名*/
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
                usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength, pStrCurrentPos+3, ShortNameLen-1);
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");

                ucCodingScheme = (pStrCurrentPos[2] & ucCodingSchemeMask);

                if ( 0 != ucCodingScheme )
                {
                    ucCodingScheme = 1;
                }

                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d,%d",
                                    ucCodingScheme,(pStrCurrentPos[2] & ucAddCIMask)>>3 );
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"\"");

                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",0,0");

            }

        }

    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}


TAF_UINT8 At_IsOplRecPrintable(TAF_PH_USIM_OPL_RECORD * pstOplRec)
{
    TAF_UINT32 i;
    if ( 0xFF == pstOplRec->PNNIndex)
    {
        return VOS_FALSE;
    }

    for ( i=0; i < pstOplRec->PlmnLen; i++)
    {
        if ( 0xA <= pstOplRec->PLMN[i] )
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;

}

/*****************************************************************************
 Prototype      : At_QryParaRspOplProc
 Description    : 参数查询结果Opl的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspOplProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    TAF_UINT32                          i;
    TAF_UINT32                          j;
    TAF_PH_USIM_OPL_CNF_STRU            *pstOPL;
    TAF_UINT32                          ucRecCntPrinted = 0;

    pstOPL     = (TAF_PH_USIM_OPL_CNF_STRU*)pPara;

    /* 查询PNN记录数和记录长度 */
    for(i = 0 ; i <  pstOPL->TotalRecordNum; i++)
    {
        if ( VOS_FALSE == At_IsOplRecPrintable(pstOPL->OPLRecord+i))
        {
            continue;
        }
        if(0 != ucRecCntPrinted)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        for(j = 0 ; j < pstOPL->OPLRecord[i].PlmnLen; j++)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%X",pstOPL->OPLRecord[i].PLMN[j]);
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",0x%X-0x%X,%d",pstOPL->OPLRecord[i].LACLow, pstOPL->OPLRecord[i].LACHigh, pstOPL->OPLRecord[i].PNNIndex);

        ++ ucRecCntPrinted;
    }


    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}
TAF_VOID At_QryParaRspCfplmnProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    TAF_USER_PLMN_LIST_STRU             *pstUserPlmnList;
    TAF_UINT32                          i;

    pstUserPlmnList = (TAF_USER_PLMN_LIST_STRU*) pPara;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,pstUserPlmnList->usPlmnNum);
    for ( i = 0 ; i < pstUserPlmnList->usPlmnNum; i++ )
    {

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%X%X%X",
                     (0x0f00 & pstUserPlmnList->Plmn[i].Mcc) >> 8,
                     (0x00f0 & pstUserPlmnList->Plmn[i].Mcc) >> 4,
                     (0x000f & pstUserPlmnList->Plmn[i].Mcc)
                     );

        if( 0x0f00 == (0x0f00 & pstUserPlmnList->Plmn[i].Mnc))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%X%X\"",
                     (0x00f0 & pstUserPlmnList->Plmn[i].Mnc) >> 4,
                     (0x000f & pstUserPlmnList->Plmn[i].Mnc)
                     );
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%X%X%X\"",
                     (0x0f00 & pstUserPlmnList->Plmn[i].Mnc) >> 8,
                     (0x00f0 & pstUserPlmnList->Plmn[i].Mnc) >> 4,
                     (0x000f & pstUserPlmnList->Plmn[i].Mnc)
                     );
        }

    }

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}

/*****************************************************************************
 Prototype      : At_QryParaRspHsProc
 Description    : 参数查询结果Hs的上报处理
 Input          : usClientId --- 用户ID
                  OpId       --- 操作ID
                  QueryType  --- 查询类型
                  pPara      --- 结果
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_QryParaRspHsProc(
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                            *pPara
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;
    VOS_UINT8                           aucBuf[AT_HS_PARA_MAX_LENGTH];

    PS_MEM_CPY(aucBuf, pPara, sizeof(aucBuf));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,*(TAF_UINT32*)aucBuf);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",aucBuf[4]);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",aucBuf[5]);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",aucBuf[6]);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",aucBuf[7]);

    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}




VOS_UINT32 AT_QryParaRspPrefPlmnProc(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    TAF_PLMN_NAME_LIST_STRU            *pstAvailPlmnInfo;
    VOS_UINT32                          i;
    AT_COPS_FORMAT_TYPE                 CurrFormat;
    TAF_MMC_USIM_RAT                    usPlmnRat;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulValidPlmnNum;
    TAF_MMA_CPOL_INFO_QUERY_REQ_STRU    stCpolInfo;
    VOS_UINT8                           ucIndex;
    TAF_MMA_CPOL_INFO_QUERY_CNF_STRU   *pstCpolQryCnf = VOS_NULL_PTR;

    pstCpolQryCnf    = (TAF_MMA_CPOL_INFO_QUERY_CNF_STRU *)pMsg;

    pstAvailPlmnInfo = (TAF_PLMN_NAME_LIST_STRU*)&pstCpolQryCnf->stPlmnName;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCpolQryCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_QryParaRspPrefPlmnProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_QryParaRspPrefPlmnProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /*判断当前操作类型是否为AT_CMD_CPOL_READ */
    if (AT_CMD_CPOL_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    ulValidPlmnNum = 0;

    for(i = 0; i < pstAvailPlmnInfo->ulPlmnNum; i++)
    {
        if (VOS_FALSE == AT_PH_IsPlmnValid(&(pstAvailPlmnInfo->astPlmnName[i].PlmnId)))
        {
            continue;
        }

        ulValidPlmnNum++;

        if ((0 == pstCpolQryCnf->ulValidPlmnNum)
         && (1 == ulValidPlmnNum))
        {
            /* 参考V.250协议5.7.2: 根据V命令的设置给信息字段的头尾增加回车换行符 */
            if (AT_V_ENTIRE_TYPE == gucAtVType)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s",
                                                   gaucAtCrLf);
            }
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);
        }

        /* +CPOL:   */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* <index> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d",
                                           ((i + pstCpolQryCnf->ulFromIndex) + 1));

        CurrFormat = pstNetCtx->ucCpolFormatType;

        if (('\0' == pstAvailPlmnInfo->astPlmnName[i].aucOperatorNameLong[0])
         && (AT_COPS_LONG_ALPH_TYPE == CurrFormat))
        {
            CurrFormat = AT_COPS_NUMERIC_TYPE;
        }

        if (('\0' == pstAvailPlmnInfo->astPlmnName[i].aucOperatorNameShort[0])
         && (AT_COPS_SHORT_ALPH_TYPE == CurrFormat))
        {
            CurrFormat = AT_COPS_NUMERIC_TYPE;
        }

        /* <format> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           CurrFormat);

        /* <oper1> */
        if (AT_COPS_LONG_ALPH_TYPE == CurrFormat)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"%s\"",
                                               pstAvailPlmnInfo->astPlmnName[i].aucOperatorNameLong);
        }
        else if (AT_COPS_SHORT_ALPH_TYPE == CurrFormat)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"%s\"",
                                               pstAvailPlmnInfo->astPlmnName[i].aucOperatorNameShort);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",\"%X%X%X",
                                               (0x0f00 & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mcc) >> 8,
                                               (0x00f0 & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mcc) >> 4,
                                               (0x000f & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mcc));

            if (0x0F != ((0x0f00 & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mnc) >> 8))
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%X",
                                                   (0x0f00 & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mnc) >> 8);

            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%X%X\"",
                                               (0x00f0 & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mnc) >> 4,
                                               (0x000f & pstAvailPlmnInfo->astPlmnName[i].PlmnId.Mnc));
        }

        if ((MN_PH_PREF_PLMN_UPLMN == pstNetCtx->enPrefPlmnType)
         && (VOS_TRUE == pstAvailPlmnInfo->bPlmnSelFlg))
        {
            continue;
        }


        /* <GSM_AcT1> */
        /* <GSM_Compact_AcT1> */
        /* <UTRAN_AcT1> */
        usPlmnRat = pstAvailPlmnInfo->ausPlmnRat[i];

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,",%d,%d,%d,%d",
                                           (usPlmnRat & TAF_MMC_USIM_GSM_RAT) >> 7,
                                           (usPlmnRat & TAF_MMC_USIM_GSM_COMPACT_RAT) >> 6,
                                           (usPlmnRat & TAF_MMC_USIM_UTRN_RAT) >> 15,
                                           (usPlmnRat & TAF_MMC_USIM_E_UTRN_RAT) >> 14);

    }

    /*
    AT模块根据MMA上报的运营商个数来确认运营商信息是否收集完成: 小于请求的运营商
    个数则认为运营商信息已经收集完成，原因如下所述:
    AT模块每次请求37条运营商信息，起始位置为已经输出的最后一个运营商的下一条运营商信息索引
    如果待输出的运营商信息总数不足37条，则按实际的输出，
    如果运营商数为37的倍数，则AT会再发一次运营商信息请求，MMA回复的运营商信息总数为0
    */
    if (pstAvailPlmnInfo->ulPlmnNum < MAX_PLMN_NAME_LIST)
    {
        /* 参考V.250协议5.7.2: 根据V命令的设置给信息字段的头尾增加回车换行符 */
        if ((AT_V_ENTIRE_TYPE == gucAtVType)
         && (0 != (pstCpolQryCnf->ulValidPlmnNum + ulValidPlmnNum)))
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);

        }

        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        stCpolInfo.enPrefPLMNType   = pstNetCtx->enPrefPlmnType;
        stCpolInfo.ulFromIndex      = (pstCpolQryCnf->ulFromIndex + pstAvailPlmnInfo->ulPlmnNum);
        stCpolInfo.ulPlmnNum        = MAX_PLMN_NAME_LIST;
        stCpolInfo.ulValidPlmnNum   = (pstCpolQryCnf->ulValidPlmnNum + ulValidPlmnNum);
        ulResult                    = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                                             gastAtClientTab[ucIndex].opId,
                                                             TAF_MSG_MMA_CPOL_INFO_QUERY_REQ,
                                                             &stCpolInfo,
                                                             sizeof(stCpolInfo),
                                                             I0_WUEPS_PID_MMA);
        if (TAF_SUCCESS != ulResult)
        {
            /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_ERROR);
        }
    }

    return VOS_OK;

}
VOS_VOID AT_QryParaRspCsqlvlExtProc (
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_VOID                           *pPara
)
{
    MN_PH_CSQLVLEXT_PARA_STRU           stCsqLvlExtPara;

    PS_MEM_CPY(&stCsqLvlExtPara, pPara, sizeof(stCsqLvlExtPara));

    /* 根据MMA上报的查询结果显示CsqlvlExt到串口；*/
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR*)pgucAtSndCodeAddr,
                                                    (VOS_CHAR*)pgucAtSndCodeAddr,
                                                    "%s: %d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    stCsqLvlExtPara.enRssilv,
                                                    stCsqLvlExtPara.enBer);

    At_FormatResultData(ucIndex, AT_OK);
}
VOS_VOID AT_QryParaRspCsqlvlProc (
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    MN_PH_CSQLVL_PARA_STRU              stCsqLvlPara;

    PS_MEM_CPY(&stCsqLvlPara, pPara, sizeof(stCsqLvlPara));

    /* 根据MMA上报的查询结果显示Csqlvl到串口；*/
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR*)pgucAtSndCodeAddr,
                                                    (VOS_CHAR*)pgucAtSndCodeAddr,
                                                    "%s: %d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    stCsqLvlPara.usLevel,
                                                    stCsqLvlPara.usRscp);

    At_FormatResultData(ucIndex, AT_OK);

    return;
}
VOS_VOID AT_RcvCdurQryRsp(
    VOS_UINT8                           ucIndex,
    MN_CALL_EVENT_ENUM_U32              enEvent,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    VOS_UINT32                          ulResult;

    /* AT模块在等待CDUR查询命令的结果事件上报 */
    if (AT_CMD_CDUR_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return;
    }

    ulResult = AT_OK;

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_CS_CAUSE_SUCCESS == pstCallInfo->enCause)
    {
        /* 输出查询结果: 构造结构为^CDUR: <CurCallTime>格式 */
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:%d,%d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstCallInfo->callId,
                                                         pstCallInfo->ulCurCallTime);

        ulResult = AT_OK;

    }
    else
    {
        ulResult = AT_ConvertCallError(pstCallInfo->enCause);

        /* 调用At_FormatResultData输出结果 */
        gstAtSendData.usBufLen = 0;
    }

    At_FormatResultData(ucIndex, ulResult);

    return;
}
VOS_UINT32 AT_RcvDrvAgentTseLrfSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_TSELRF_SET_CNF_STRU      *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_TSELRF_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待TSELRF设置命令的结果事件上报 */
    if (AT_CMD_TSELRF_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_TSELRF_SET_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK */
        ulRet                            = AT_OK;
        g_stAtDevCmdCtrl.bDspLoadFlag    = VOS_TRUE;
        g_stAtDevCmdCtrl.ucDeviceRatMode = pstEvent->ucDeviceRatMode;
        g_stAtDevCmdCtrl.usFDAC          = 0;

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                            = AT_ERROR;
    }

    /* 4. 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentHkAdcGetRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_HKADC_GET_CNF_STRU      *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_HKADC_GET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待HKADC电压查询命令的结果事件上报 */
    if (AT_CMD_TBAT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_HKADC_GET_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:1,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstEvent->TbatHkadc);

        ulRet                            = AT_OK;

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                            = AT_ERROR;
    }

    /* 4. 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentAppdmverQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_APPDMVER_QRY_CNF_STRU    *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_APPDMVER_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAppdmverQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAppdmverQryRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待APPDMVER查询命令的结果事件上报 */
    if (AT_CMD_APPDMVER_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_APPDMVER_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK           构造结构为^APPDMVER:<pdmver>格式 */
        ulRet                  = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s:%s",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstEvent->acPdmver);

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 4. 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentDloadverQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_DLOADVER_QRY_CNF_STRU    *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_DLOADVER_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentDloadverQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentDloadverQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待APPDMVER查询命令的结果事件上报 */
    if (AT_CMD_DLOADVER_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_DLOADVER_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK           构造结构为<dloadver>格式 */
        ulRet                  = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s",
                                                        pstEvent->aucVersionInfo);
    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentAuthVerQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_AUTHVER_QRY_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_AUTHVER_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAuthVerQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAuthVerQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待AUTHVER查询命令的结果事件上报 */
    if (AT_CMD_AUTHVER_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_AUTHVER_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK           构造结构为<CR><LF>^ AUTHVER: <value> <CR><LF>
             <CR><LF>OK<CR><LF>格式 */
        ulRet                  = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s:%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstEvent->ulSimLockVersion);

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentFlashInfoQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;
    DRV_AGENT_FLASHINFO_QRY_CNF_STRU   *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_FLASHINFO_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentFlashInfoQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentFlashInfoQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待^FLASHINFO查询命令的结果事件上报 */
    if (AT_CMD_FLASHINFO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_FLASHINFO_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK
               构造结构为<CR><LF>~~~~~~FLASH INFO~~~~~~:<CR><LF>
                <CR><LF>MMC BLOCK COUNT:<blockcount>,
                     PAGE SIZE:<pagesize>,
                     PAGE COUNT PER BLOCK:<blocksize><CR><LF>
                <CR><LF>OK<CR><LF>格式 */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s%s",
                                           "~~~~~~FLASH INFO~~~~~~:",
                                            gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "MMC BLOCK COUNT:%d, PAGE SIZE:%d, PAGE COUNT PER BLOCK:%d",
                                            pstEvent->stFlashInfo.ulBlockCount,
                                            pstEvent->stFlashInfo.ulPageSize,
                                            pstEvent->stFlashInfo.ulPgCntPerBlk);

        ulRet     = AT_OK;

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        usLength  = 0;
        ulRet     = AT_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentDloadInfoQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_DLOADINFO_QRY_CNF_STRU   *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_DLOADINFO_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentDloadInfoQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentDloadInfoQryRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待DLOADINFO查询命令的结果事件上报 */
    if (AT_CMD_DLOADINFO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_DLOADINFO_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK
               构造结构为^DLOADINFO:<CR><LF>
             <CR><LF>swver:<software version><CR><LF>
             <CR><LF>isover:<iso version><CR><LF>
             <CR><LF>product name:<product name><CR><LF>
             <CR><LF>product name:<WebUiVer><CR><LF>
             <CR><LF>dload type: <dload type><CR><LF>
             <CR><LF>OK<CR><LF>格式 */
        ulRet                  = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        "%s",
                                                        pstEvent->aucDlodInfo);

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentHwnatQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_HWNATQRY_QRY_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_HWNATQRY_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHwnatQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHwnatQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待HWNAT查询命令的结果事件上报 */
    if (AT_CMD_HWNATQRY_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_HWNATQRY_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK
           构造结构为^HWNATQRY: <cur_mode> 格式 */
        ulRet                  = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:%d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstEvent->ulNetMode);
    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentAuthorityVerQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                           ulRet;
    VOS_UINT8                            ucIndex;
    DRV_AGENT_AUTHORITYVER_QRY_CNF_STRU *pstEvent;
    DRV_AGENT_MSG_STRU                  *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_AUTHORITYVER_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAuthorityVerQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAuthorityVerQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待AUTHORITYVER查询命令的结果事件上报 */
    if (AT_CMD_AUTHORITYVER_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 :  */
    if (DRV_AGENT_AUTHORITYVER_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK 格式为<CR><LF><Authority Version><CR><LF>
             <CR><LF>OK<CR><LF> */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s",
                                                        pstEvent->aucAuthority);

        ulRet = AT_OK;
    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        gstAtSendData.usBufLen = 0;
        ulRet                  = AT_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentAuthorityIdQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_AUTHORITYID_QRY_CNF_STRU *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_AUTHORITYID_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAuthorityIdQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentAuthorityIdQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待AUTHORITYID查询命令的结果事件上报 */
    if (AT_CMD_AUTHORITYID_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 :  */
    if (DRV_AGENT_AUTHORITYID_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK 格式为<CR><LF><Authority ID>, <Authority Type><CR><LF>
             <CR><LF>OK<CR><LF> */
        gstAtSendData.usBufLen  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s",
                                                        pstEvent->aucAuthorityId);

        ulRet                   = AT_OK;
    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                   = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentGodloadSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_GODLOAD_SET_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_GODLOAD_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentGodloadSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentGodloadSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待GODLOAD查询命令的结果事件上报 */
    if (AT_CMD_GODLOAD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_GODLOAD_SET_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK */
        ulRet = AT_OK;
    }
    else
    {
        /* 设置失败返回ERROR字符串 */
        ulRet = AT_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentPfverQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_PFVER_QRY_CNF_STRU       *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_PFVER_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentPfverQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentPfverQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待PFVER查询命令的结果事件上报 */
    if (AT_CMD_PFVER_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_PFVER_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK           构造结构为<CR><LF>^PFVER: <PfVer>,<VerTime> <CR><LF>
             <CR><LF>OK<CR><LF>格式 */
        ulRet                  = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s:\"%s %s\"",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstEvent->stPfverInfo.aucPfVer,
                                          pstEvent->stPfverInfo.acVerTime);

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentSdloadSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_SDLOAD_SET_CNF_STRU      *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_SDLOAD_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSdloadSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSdloadSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待SDLOAD查询命令的结果事件上报 */
    if (AT_CMD_SDLOAD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_SDLOAD_SET_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK */
        ulRet = AT_OK;
    }
    else
    {
        /* 设置失败返回ERROR字符串 */
        ulRet = AT_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentCpuloadQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_CPULOAD_QRY_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_CPULOAD_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentCpuloadQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentCpuloadQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待CPULOAD查询命令的结果事件上报 */
    if (AT_CMD_CPULOAD_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_CPULOAD_QRY_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstEvent->ulCurACpuLoad,
                                                        pstEvent->ulCurCCpuLoad);
        ulRet = AT_OK;
    }
    else
    {
        /* 设置失败返回ERROR字符串 */
        gstAtSendData.usBufLen = 0;
        ulRet                  = AT_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentMfreelocksizeQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                                  ulRet;
    VOS_UINT8                                   ucIndex;
    DRV_AGENT_MFREELOCKSIZE_QRY_CNF_STRU       *pstEvent;
    DRV_AGENT_MSG_STRU                         *pstRcvMsg;
    VOS_UINT32                                  ulACoreMemfreeSize;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_MFREELOCKSIZE_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulACoreMemfreeSize     = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentMfreelocksizeQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentMfreelocksizeQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待MFREELOCKSIZE查询命令的结果事件上报 */
    if (AT_CMD_MFREELOCKSIZE_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_MFREELOCKSIZE_QRY_NO_ERROR == pstEvent->enResult)
    {

        /* 获取A核的剩余系统内存 */
        ulACoreMemfreeSize = FREE_MEM_SIZE_GET();


        /* 由于底软返回的是KB，转为字节 */
        ulACoreMemfreeSize *= AT_KB_TO_BYTES_NUM;

        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s:%d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstEvent->lMaxFreeLockSize,
                                                        ulACoreMemfreeSize);


        /* 设置错误码为AT_OK */
        ulRet = AT_OK;
    }
    else
    {
        /* 设置失败返回ERROR字符串 */
        gstAtSendData.usBufLen = 0;
        ulRet                  = AT_ERROR;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentImsiChgQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    DRV_AGENT_IMSICHG_QRY_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_IMSICHG_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentImsiChgQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentImsiChgQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待IMSICHG查询命令的结果事件上报 */
    if (AT_CMD_IMSICHG_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR*)pgucAtSndCodeAddr,
                                                "^IMSICHG: %d,%d",
                                                pstEvent->usDualIMSIEnable,
                                                pstEvent->ulCurImsiSign);

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentInfoRbuSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    DRV_AGENT_INFORBU_SET_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_INFORBU_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentInfoRbuSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentInfoRbuSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待INFORBU设置命令的结果事件上报 */
    if (AT_CMD_INFORBU_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    if (NV_OK == pstEvent->ulRslt)
    {
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}

#if ( FEATURE_ON == FEATURE_LTE )
VOS_UINT32 AT_RcvDrvAgentInfoRrsSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    DRV_AGENT_INFORRS_SET_CNF_STRU     *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_INFORRS_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        return VOS_ERR;
    }

    /* AT模块在等待INFORRU设置命令的结果事件上报 */
    if (AT_CMD_INFORRS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置操作结果 :  */
    if (NV_OK == pstEvent->ulRslt)
    {
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}
#endif
VOS_UINT32 AT_RcvDrvAgentCpnnQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    DRV_AGENT_CPNN_QRY_CNF_STRU        *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_CPNN_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待CPNN查询命令的结果事件上报 */
    if (AT_CMD_CPNN_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ( VOS_TRUE == pstEvent->bNormalSrvStatus )
    {
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentCpnnTestRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    DRV_AGENT_CPNN_TEST_CNF_STRU       *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_CPNN_TEST_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnTestRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnTestRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待CPNN测试命令的结果事件上报 */
    if (AT_CMD_CPNN_TEST != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ( ( PS_USIM_SERVICE_AVAILIABLE == pstEvent->ulOplExistFlg )
      && ( PS_USIM_SERVICE_AVAILIABLE == pstEvent->ulPnnExistFlg )
      && ( VOS_TRUE == pstEvent->bNormalSrvStatus ) )
    {
        gstAtSendData.usBufLen = (TAF_UINT16)VOS_sprintf((TAF_CHAR*)pgucAtSndCodeAddr,
                                                         "%s:(0,1)",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentNvBackupSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    DRV_AGENT_NVBACKUP_SET_CNF_STRU    *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_NVBACKUP_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNvBackupSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNvBackupSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待NVBACKUP测试命令的结果事件上报 */
    if (AT_CMD_NVBACKUP_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (NV_OK == pstEvent->ulRslt)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%d",
                                                        pstEvent->ulRslt);
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentMemInfoQryRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pstRcvMsg            = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_MEMINFO_QRY_RSP_STRU     *pstCCpuMemInfoCnfMsg = VOS_NULL_PTR;
    AT_PID_MEM_INFO_PARA_STRU          *pstPidMemInfo        = VOS_NULL_PTR;
    VOS_UINT32                          ulACpuMemBufSize;
    VOS_UINT32                          ulACpuPidTotal;
    VOS_UINT16                          usAtLength;
    VOS_UINT32                          i;

    /* 初始化 */
    pstRcvMsg            = (DRV_AGENT_MSG_STRU *)pMsg;
    pstCCpuMemInfoCnfMsg = (DRV_AGENT_MEMINFO_QRY_RSP_STRU *)pstRcvMsg->aucContent;

    if ( VOS_NULL_PTR == pstRcvMsg )
    {
        AT_ERR_LOG("AT_RcvDrvAgentMemInfoQryRsp: Null Ptr!");
        return VOS_ERR;
    }

    /* 指向CCPU的每个PID的内存信息 */
    pstPidMemInfo       = (AT_PID_MEM_INFO_PARA_STRU *)pstCCpuMemInfoCnfMsg->aucData;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCCpuMemInfoCnfMsg->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_ERR_LOG("AT_RcvDrvAgentMemInfoQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* Client Id 为广播也返回ERROR */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_ERR_LOG("AT_RcvDrvAgentMemInfoQryRsp: AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* AT模块在等待MEMINFO查询命令的结果事件上报 */
    if (AT_CMD_MEMINFO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("AT_RcvDrvAgentMemInfoQryRsp: CmdCurrentOpt Error!");
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);


    /* 对返回结果中的参数进行检查 */
    if ( (VOS_OK != pstCCpuMemInfoCnfMsg->ulResult)
      || ( (AT_MEMQUERY_TTF != pstCCpuMemInfoCnfMsg->ulMemQryType)
        && (AT_MEMQUERY_VOS != pstCCpuMemInfoCnfMsg->ulMemQryType) ) )
    {

        /* 调用At_FormatResultData返回ERROR字符串 */
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }

    /* 先打印命令名称 */
    usAtLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        "%s:%s",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        gaucAtCrLf);




    usAtLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                         (VOS_CHAR *)pgucAtSndCodeAddr + usAtLength,
                                         "C CPU Pid:%d%s",
                                         pstCCpuMemInfoCnfMsg->ulPidNum,
                                         gaucAtCrLf);

    /* 依次打印C CPU每个PID的内存信息 */
    for (i = 0; i < pstCCpuMemInfoCnfMsg->ulPidNum; i++)
    {
        usAtLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + usAtLength,
                                             "%d,%d,%d%s",
                                             pstPidMemInfo[i].ulPid,
                                             pstPidMemInfo[i].ulMsgPeakSize,
                                             pstPidMemInfo[i].ulMemPeakSize,
                                             gaucAtCrLf);
    }

    /* 如果是查询VOS内存，则获取并打印A CPU的VOS内存使用情况.
       由于暂无A CPU的TTF内存查询接口，TTF类型查询只打印C CPU的TTF内存使用情况 */
    if (AT_MEMQUERY_VOS == pstCCpuMemInfoCnfMsg->ulMemQryType)
    {
        ulACpuMemBufSize = AT_PID_MEM_INFO_LEN * sizeof(AT_PID_MEM_INFO_PARA_STRU);

        /* 分配内存以查询A CPU的VOS内存使用信息 */
        pstPidMemInfo = (AT_PID_MEM_INFO_PARA_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, ulACpuMemBufSize);
        if (VOS_NULL_PTR != pstPidMemInfo)
        {
            PS_MEM_SET(pstPidMemInfo, 0x00, ulACpuMemBufSize);


            ulACpuPidTotal = 0;

            if (VOS_ERR != VOS_AnalyzePidMemory(pstPidMemInfo, ulACpuMemBufSize, &ulACpuPidTotal))
            {

                /* 依次打印 A CPU的每个PID的内存使用情况 */
                usAtLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                                     (VOS_CHAR *)pgucAtSndCodeAddr + usAtLength,
                                                     "A CPU Pid:%d%s",
                                                     ulACpuPidTotal,
                                                     gaucAtCrLf);

                for (i = 0; i < ulACpuPidTotal; i++)
                {
                    usAtLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr + usAtLength,
                                                         "%d,%d,%d%s",
                                                         pstPidMemInfo[i].ulPid,
                                                         pstPidMemInfo[i].ulMsgPeakSize,
                                                         pstPidMemInfo[i].ulMemPeakSize,
                                                         gaucAtCrLf);
                }

            }

            /* 释放内存 */
            PS_MEM_FREE(WUEPS_PID_AT, pstPidMemInfo);
        }
    }

    gstAtSendData.usBufLen = usAtLength;

    /* 调用At_FormatResultData返回OK字符串 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}

TAF_VOID At_QryParaRspProc  (
    TAF_UINT8                           ucIndex,
    TAF_UINT8                           OpId,
    TAF_PARA_TYPE                       QueryType,
    TAF_UINT16                          usErrorCode,
    TAF_VOID                            *pPara
)
{

    TAF_UINT32 ulResult = AT_FAILURE;
    TAF_UINT16 usLength = 0;
    /*TAF_UINT8  ucTmp = 0; */

    TAF_UINT8                 i;

    if(0 != usErrorCode)  /* 错误 */
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        ulResult = At_ChgTafErrorCode(ucIndex,usErrorCode);       /* 发生错误 */
        gstAtSendData.usBufLen = usLength;
        At_FormatResultData(ucIndex,ulResult);
        return;
    }

    if(TAF_NULL_PTR == pPara)   /* 如果查询出错 */
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        At_FormatResultData(ucIndex,AT_CME_UNKNOWN);

        return;
    }

    for (i = 0; i != TAF_TELE_PARA_BUTT; i++ )
    {
        if (QueryType == g_aAtQryTypeProcFuncTbl[i].QueryType)
        {
            if (QueryType != TAF_PH_ICC_ID)
            {
                AT_STOP_TIMER_CMD_READY(ucIndex);
            }

            g_aAtQryTypeProcFuncTbl[i].AtQryParaProcFunc(ucIndex,OpId,pPara);

            return;
        }
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    AT_WARN_LOG("At_QryParaRspProc QueryType FAILURE");
    return;
}

TAF_VOID At_QryMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
   TAF_UINT16                          usClientId = 0;
    TAF_UINT8                           OpId = 0;
    TAF_PARA_TYPE                       QueryType = 0;
    TAF_UINT16                          usErrorCode = 0;
    TAF_VOID                           *pPara = TAF_NULL_PTR;
    TAF_UINT16                          usAddr = 0;
    TAF_UINT16                          usParaLen = 0;
    TAF_UINT8                           ucIndex  = 0;

    PS_MEM_CPY(&usClientId,pData,sizeof(usClientId));
    usAddr += sizeof(usClientId);

    PS_MEM_CPY(&OpId,pData+usAddr,sizeof(OpId));
    usAddr += sizeof(OpId);

    PS_MEM_CPY(&QueryType,pData+usAddr,sizeof(QueryType));
    usAddr += sizeof(QueryType);

    PS_MEM_CPY(&usErrorCode,pData+usAddr,sizeof(usErrorCode));
    usAddr += sizeof(usErrorCode);

    PS_MEM_CPY(&usParaLen,pData+usAddr,sizeof(usParaLen));
    usAddr += sizeof(usParaLen);

    if(0 != usParaLen)
    {
        pPara = pData+usAddr;
    }

    AT_LOG1("At_QryMsgProc ClientId",usClientId);
    AT_LOG1("At_QryMsgProc QueryType",QueryType);
    AT_LOG1("At_QryMagProc usErrorCode", usErrorCode);
    if(AT_BUTT_CLIENT_ID == usClientId)
    {
        AT_WARN_LOG("At_QryMsgProc Error ucIndex");
        return;
    }
    else
    {
        if(AT_FAILURE == At_ClientIdToUserId(usClientId,&ucIndex))
        {
            AT_WARN_LOG("At_QryMsgProc At_ClientIdToUserId FAILURE");
            return;
        }

        if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
        {
            AT_WARN_LOG("At_QryMsgProc: AT_BROADCAST_INDEX.");
            return;
        }

        AT_LOG1("At_QryMsgProc ucIndex",ucIndex);
        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

        At_QryParaRspProc(ucIndex,OpId,QueryType,usErrorCode,pPara);
    }
}


TAF_VOID At_PIHIndProc(TAF_UINT8 ucIndex, SI_PIH_EVENT_INFO_STRU *pEvent)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength  = 0;
    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_PIHIndProc: Get modem id fail.");
        return;
    }

    switch(pEvent->EventType)
    {
        case SI_PIH_EVENT_HVRDH_IND:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s^HVRDH: %d%s",
                                               gaucAtCrLf,
                                               pEvent->PIHEvent.HvrdhInd.ulReDhFlag,
                                               gaucAtCrLf);
            break;

        case SI_PIH_EVENT_TEETIMEOUT_IND:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s^TEETIMEOUT: %d%s",
                                               gaucAtCrLf,
                                               pEvent->PIHEvent.TEETimeOut.ulData,
                                               gaucAtCrLf);
            break;

        default:
            AT_WARN_LOG("At_PIHIndProc: Abnormal EventType.");
            return;
    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}


TAF_VOID At_PIHRspProc(TAF_UINT8 ucIndex, SI_PIH_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT32 ulResult = AT_FAILURE;
    TAF_UINT16 usLength = 0;

    if(TAF_ERR_NO_ERROR != pEvent->PIHError)  /* 错误 */
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        ulResult = At_ChgTafErrorCode(ucIndex, (TAF_UINT16)(pEvent->PIHError));       /* 发生错误 */

        gstAtSendData.usBufLen = usLength;

        At_FormatResultData(ucIndex,ulResult);

        return;
    }

    switch(pEvent->EventType)
    {
        /* BDN/FDN对应相同的处理 */
        case SI_PIH_EVENT_FDN_CNF:
        case SI_PIH_EVENT_BDN_CNF:
            /* 如果是状态查询命令 */
            if(SI_PIH_FDN_BDN_QUERY == pEvent->PIHEvent.FDNCnf.FdnCmd)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",pEvent->PIHEvent.FDNCnf.FdnState);
            }
            break;

        case SI_PIH_EVENT_GENERIC_ACCESS_CNF:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            /* <length>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"",(pEvent->PIHEvent.GAccessCnf.Len+2)*2);
            if(pEvent->PIHEvent.GAccessCnf.Len != 0)
            {
                /* <command>, */
                usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,pEvent->PIHEvent.GAccessCnf.Command,pEvent->PIHEvent.GAccessCnf.Len);
            }
            /*SW1*/
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,&pEvent->PIHEvent.GAccessCnf.SW1,sizeof(TAF_UINT8));
            /*SW1*/
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,&pEvent->PIHEvent.GAccessCnf.SW2,sizeof(TAF_UINT8));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
            break;

        /* ^CISA命令的回复 */
        case SI_PIH_EVENT_ISDB_ACCESS_CNF:

            /* 判断当前操作类型是否为AT_CMD_CISA_SET */
            if (AT_CMD_CISA_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
            {
                AT_WARN_LOG("At_PIHRspProc: NOT CURRENT CMD OPTION!");
                return;
            }

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ", g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            /* <length>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"", (pEvent->PIHEvent.IsdbAccessCnf.usLen + 2) * 2);
            if(pEvent->PIHEvent.IsdbAccessCnf.usLen != 0)
            {
                /* <command>, */
                usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, pEvent->PIHEvent.IsdbAccessCnf.aucCommand, pEvent->PIHEvent.IsdbAccessCnf.usLen);
            }

            /*SW1*/
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, &pEvent->PIHEvent.IsdbAccessCnf.ucSW1, sizeof(TAF_UINT8));

            /*SW2*/
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, &pEvent->PIHEvent.IsdbAccessCnf.ucSW2, sizeof(TAF_UINT8));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength, "\"");

            break;

        case SI_PIH_EVENT_CCHO_SET_CNF:

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            /* <sessionid>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%u", pEvent->PIHEvent.ulSessionID);

            break;

        /*直接返回结果*/
        case SI_PIH_EVENT_CCHC_SET_CNF:
        case SI_PIH_EVENT_SCICFG_SET_CNF:
        case SI_PIH_EVENT_HVSST_SET_CNF:    /*直接输出结果*/
            break;

        case SI_PIH_EVENT_CGLA_SET_CNF:

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ", g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            /* <length>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"", (pEvent->PIHEvent.stCglaCmdCnf.usLen + 2) * 2);
            if(pEvent->PIHEvent.stCglaCmdCnf.usLen != 0)
            {
                /* <command>, */
                usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, pEvent->PIHEvent.stCglaCmdCnf.aucCommand, pEvent->PIHEvent.stCglaCmdCnf.usLen);
            }

            /*SW1*/
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, &pEvent->PIHEvent.stCglaCmdCnf.ucSW1, sizeof(TAF_UINT8));

            /*SW2*/
            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, &pEvent->PIHEvent.stCglaCmdCnf.ucSW2, sizeof(TAF_UINT8));
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength, "\"");

            break;

        case SI_PIH_EVENT_CARD_ATR_QRY_CNF:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength, "%s:\"", g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            usLength += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)pgucAtSndCodeAddr, (TAF_UINT8 *)pgucAtSndCodeAddr + usLength, pEvent->PIHEvent.stATRQryCnf.aucCommand, (VOS_UINT16)pEvent->PIHEvent.stATRQryCnf.ulLen);

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength, "\"");

            break;

        case SI_PIH_EVENT_SCICFG_QUERY_CNF:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s: %d, %d",
                                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                pEvent->PIHEvent.SciCfgCnf.enCard0Slot,
                                                pEvent->PIHEvent.SciCfgCnf.enCard1Slot);
            break;

        case SI_PIH_EVENT_HVSST_QUERY_CNF:
            usLength += At_HvsstQueryCnf(pEvent);
            break;

#if (FEATURE_VSIM == FEATURE_ON)
        case SI_PIH_EVENT_HVSDH_SET_CNF:    /*直接输出结果*/
        case SI_PIH_EVENT_HVTEE_SET_CNF:
        case SI_PIH_EVENT_FILE_WRITE_CNF:
            break;

        case SI_PIH_EVENT_HVSDH_QRY_CNF:
            usLength += AT_HvsDHQueryCnf(pEvent);
            break;

        case SI_PIH_EVENT_HVSCONT_QUERY_CNF:
            usLength += At_HvsContQueryCnf(pEvent);

            break;

        case SI_PIH_EVENT_HVCHECKCARD_CNF:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                    "%s: %d",
                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                    pEvent->PIHEvent.HvCheckCardCnf.enData);
            break;
#endif

#if (FEATURE_ON == FEATURE_IMS)
        case SI_PIH_EVENT_UICCAUTH_CNF:
            usLength += AT_UiccAuthCnf(ucIndex, pEvent);
            break;

        case SI_PIH_EVENT_URSM_CNF:
            usLength += AT_UiccAccessFileCnf(ucIndex, pEvent);
            break;
#endif

        case SI_PIH_EVENT_CARDTYPE_QUERY_CNF:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                    "%s: %d, %d, %d",
                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                    pEvent->PIHEvent.CardTypeCnf.ucMode,
                    pEvent->PIHEvent.CardTypeCnf.ucHasCModule,
                    pEvent->PIHEvent.CardTypeCnf.ucHasGModule);
            break;

        default:
            return;
    }

    ulResult = AT_OK;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex,ulResult);

    return;
}


TAF_VOID At_PbIndMsgProc(SI_PB_EVENT_INFO_STRU *pEvent)
{
    if (SI_PB_EVENT_INFO_IND == pEvent->PBEventType)
    {
        /* 当SIM卡且FDN使能时默认存储器是FD */
        if ((0 == pEvent->PBEvent.PBInfoInd.CardType)
         && (SI_PIH_STATE_FDN_BDN_ENABLE == pEvent->PBEvent.PBInfoInd.FdnState))
        {
            gstPBATInfo.usNameMaxLen = pEvent->PBEvent.PBInfoInd.FDNTextLen;
            gstPBATInfo.usNumMaxLen  = pEvent->PBEvent.PBInfoInd.FDNNumberLen;
            gstPBATInfo.usTotal      = pEvent->PBEvent.PBInfoInd.FDNRecordNum;
            gstPBATInfo.usAnrNumLen  = pEvent->PBEvent.PBInfoInd.ANRNumberLen;
            gstPBATInfo.usEmailLen   = pEvent->PBEvent.PBInfoInd.EMAILTextLen;
        }
        else
        {
            gstPBATInfo.usNameMaxLen = pEvent->PBEvent.PBInfoInd.ADNTextLen;
            gstPBATInfo.usNumMaxLen  = pEvent->PBEvent.PBInfoInd.ADNNumberLen;
            gstPBATInfo.usTotal      = pEvent->PBEvent.PBInfoInd.ADNRecordNum;
            gstPBATInfo.usAnrNumLen  = pEvent->PBEvent.PBInfoInd.ANRNumberLen;
            gstPBATInfo.usEmailLen   = pEvent->PBEvent.PBInfoInd.EMAILTextLen;
        }
    }

    return;
}
TAF_VOID At_PbRspProc(TAF_UINT8 ucIndex,SI_PB_EVENT_INFO_STRU *pEvent)
{
    VOS_UINT32 ulResult = AT_FAILURE;
    TAF_UINT16 usLength = 0;


    if(TAF_ERR_NO_ERROR != pEvent->PBError)  /* 错误 */
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        ulResult = At_ChgTafErrorCode(ucIndex,(TAF_UINT16)pEvent->PBError);       /* 发生错误 */

        gstAtSendData.usBufLen = usLength;

        At_FormatResultData(ucIndex,ulResult);

        return;
    }

    switch(pEvent->PBEventType)
    {
        case SI_PB_EVENT_SET_CNF:
            gstPBATInfo.usNameMaxLen = pEvent->PBEvent.PBSetCnf.TextLen;
            gstPBATInfo.usNumMaxLen  = pEvent->PBEvent.PBSetCnf.NumLen;
            gstPBATInfo.usTotal      = pEvent->PBEvent.PBSetCnf.TotalNum;
            gstPBATInfo.usUsed       = pEvent->PBEvent.PBSetCnf.InUsedNum;
            gstPBATInfo.usAnrNumLen  = pEvent->PBEvent.PBSetCnf.ANRNumberLen;
            gstPBATInfo.usEmailLen   = pEvent->PBEvent.PBSetCnf.EMAILTextLen;

            AT_STOP_TIMER_CMD_READY(ucIndex);

            ulResult = AT_OK;

            break;

        case SI_PB_EVENT_READ_CNF:
        case SI_PB_EVENT_SREAD_CNF:
            if((pEvent->PBError == TAF_ERR_NO_ERROR)&&
                (pEvent->PBEvent.PBReadCnf.PBRecord.ValidFlag == SI_PB_CONTENT_VALID))/*当前的内容有效*/
            {
                if (AT_CMD_D_GET_NUMBER_BEFORE_CALL == gastAtClientTab[ucIndex].CmdCurrentOpt)
                {
                    ulResult = At_DialNumByIndexFromPb(ucIndex,pEvent);
                    if(AT_WAIT_ASYNC_RETURN == ulResult)
                    {
                        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;
                        /* 开定时器 */
                        if(AT_SUCCESS != At_StartTimer(g_stParseContext[ucIndex].pstCmdElement->ulSetTimeOut, ucIndex))
                        {
                            AT_ERR_LOG("At_PbRspProc:ERROR:Start Timer");
                            ulResult = AT_ERROR;
                        }

                        ulResult = AT_SUCCESS;
                    }
                    else
                    {
                        ulResult = AT_ERROR;
                    }

                    break;

                }

                if (TAF_FALSE == gulPBPrintTag)
                {
                     usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,(TAF_CHAR *)pgucAtSndCrLfAddr,"%s","\r\n");
                }

                gulPBPrintTag = TAF_TRUE;

                if(AT_CMD_CPBR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 ^CPBR 的方式进行打印*/
                {
                    ulResult = At_PbCPBRCmdPrint(ucIndex,&usLength,pgucAtSndCrLfAddr,pEvent);
                }
                else if(AT_CMD_CPBR2_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 +CPBR 的方式进行打印*/
                {
                    ulResult = At_PbCPBR2CmdPrint(ucIndex,&usLength,pgucAtSndCrLfAddr,pEvent);
                }
                else if(AT_CMD_SCPBR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 ^SCPBR 的方式进行打印*/
                {
                    ulResult = At_PbSCPBRCmdPrint(ucIndex,&usLength,pEvent);
                }
                else if(AT_CMD_CNUM_READ == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 CNUM 的方式进行打印*/
                {
                    ulResult = At_PbCNUMCmdPrint(ucIndex,&usLength,pgucAtSndCrLfAddr,pEvent);
                }
                else
                {
                    AT_ERR_LOG1("At_PbRspProc: the Cmd Current Opt %d is Unknow", gastAtClientTab[ucIndex].CmdCurrentOpt);

                    return ;
                }

                if(AT_SUCCESS == ulResult)
                {
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                        (TAF_CHAR *)pgucAtSndCrLfAddr + usLength,
                                        "%s","\r\n");
                }

                At_SendResultData(ucIndex, pgucAtSndCrLfAddr, usLength);

                usLength = 0;
            }

            if( (VOS_FALSE == gulPBPrintTag)
             && ( (AT_CMD_CPBR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
               || (AT_CMD_CPBR2_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) ) )
            {
                pEvent->PBError = TAF_ERR_ERROR;
            }

            AT_STOP_TIMER_CMD_READY(ucIndex);

            if(TAF_ERR_NO_ERROR != pEvent->PBError)  /* 错误 */
            {
                ulResult = At_ChgTafErrorCode(ucIndex,(TAF_UINT16)pEvent->PBError);       /* 发生错误 */

                gstAtSendData.usBufLen = usLength;
            }
            else
            {
                ulResult = AT_OK;
            }

            PS_MEM_CPY((TAF_CHAR *)pgucAtSndCrLfAddr,(TAF_CHAR *)gaucAtCrLf,2);/*AT输出Buffer的前两个字节恢复为\r\n*/
            break;

        case SI_PB_EVENT_SEARCH_CNF:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s","\r");
            AT_STOP_TIMER_CMD_READY(ucIndex);
            ulResult = AT_OK;
            break;

        case SI_PB_EVENT_ADD_CNF:
        case SI_PB_EVENT_SADD_CNF:
        case SI_PB_EVENT_MODIFY_CNF:
        case SI_PB_EVENT_SMODIFY_CNF:
        case SI_PB_EVENT_DELETE_CNF:
            AT_STOP_TIMER_CMD_READY(ucIndex);
            ulResult = AT_OK;
            break;

        case SI_PB_EVENT_QUERY_CNF:

            if(1 == pEvent->OpId)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: (\"SM\",\"EN\",\"ON\",\"FD\")",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                ulResult = AT_OK;
                AT_STOP_TIMER_CMD_READY(ucIndex);
                break;
            }

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            gstPBATInfo.usNameMaxLen = pEvent->PBEvent.PBQueryCnf.TextLen;
            gstPBATInfo.usNumMaxLen  = pEvent->PBEvent.PBQueryCnf.NumLen;
            gstPBATInfo.usTotal      = pEvent->PBEvent.PBQueryCnf.TotalNum;
            gstPBATInfo.usUsed       = pEvent->PBEvent.PBQueryCnf.InUsedNum;
            gstPBATInfo.usAnrNumLen  = pEvent->PBEvent.PBQueryCnf.ANRNumberLen;
            gstPBATInfo.usEmailLen   = pEvent->PBEvent.PBQueryCnf.EMAILTextLen;

            switch(g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
            {
                case AT_CMD_CPBR:
                case AT_CMD_CPBR2:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"(1-%d),%d,%d",gstPBATInfo.usTotal,gstPBATInfo.usNumMaxLen,gstPBATInfo.usNameMaxLen);
                    break;
                case AT_CMD_CPBW:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"(1-%d),%d,(128-255),%d",gstPBATInfo.usTotal,gstPBATInfo.usNumMaxLen,gstPBATInfo.usNameMaxLen);
                    break;
                case AT_CMD_CPBW2:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"(1-%d),%d,(128-255),%d",gstPBATInfo.usTotal,gstPBATInfo.usNumMaxLen,gstPBATInfo.usNameMaxLen);
                    break;
                case AT_CMD_SCPBR:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"(1-%d),%d,%d,%d",gstPBATInfo.usTotal,gstPBATInfo.usNumMaxLen,gstPBATInfo.usNameMaxLen,gstPBATInfo.usEmailLen);
                    break;
                case AT_CMD_SCPBW:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"(1-%d),%d,(128-255),%d,%d",gstPBATInfo.usTotal,gstPBATInfo.usNumMaxLen,gstPBATInfo.usNameMaxLen,gstPBATInfo.usEmailLen);
                    break;
                case AT_CMD_CPBS:
                    if(SI_PB_STORAGE_SM == pEvent->Storage)
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gastAtStringTab[AT_STRING_SM].pucText);
                    }
                    else if(SI_PB_STORAGE_FD == pEvent->Storage)
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gastAtStringTab[AT_STRING_FD].pucText);
                    }
                    else if(SI_PB_STORAGE_ON == pEvent->Storage)
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gastAtStringTab[AT_STRING_ON].pucText);
                    }
                    else if(SI_PB_STORAGE_BD == pEvent->Storage)
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gastAtStringTab[AT_STRING_BD].pucText);
                    }
                    else
                    {
                        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gastAtStringTab[AT_STRING_EN].pucText);
                    }

                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",pEvent->PBEvent.PBQueryCnf.InUsedNum);
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",pEvent->PBEvent.PBQueryCnf.TotalNum);

                    break;
                case AT_CMD_CPBF:
                    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,%d",gstPBATInfo.usNumMaxLen,gstPBATInfo.usNameMaxLen);
                    break;

                default:
                    break;
            }

            ulResult = AT_OK;

            AT_STOP_TIMER_CMD_READY(ucIndex);

            break;

        default:
            AT_ERR_LOG1("At_PbRspProc Unknow Event %d", pEvent->PBEventType);
            break;
    }

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex,ulResult);
}
TAF_VOID At_TAFPbMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    SI_PB_EVENT_INFO_STRU   *pEvent = TAF_NULL_PTR;
    TAF_UINT8               ucIndex = 0;

    pEvent = (SI_PB_EVENT_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, usLen);

    if(TAF_NULL_PTR == pEvent)
    {
        return;
    }

    PS_MEM_CPY(pEvent,pData,usLen);

    AT_LOG1("At_PbMsgProc pEvent->ClientId",pEvent->ClientId);
    AT_LOG1("At_PbMsgProc PBEventType",pEvent->PBEventType);
    AT_LOG1("At_PbMsgProc Event Error",pEvent->PBError);

    if (AT_FAILURE == At_ClientIdToUserId(pEvent->ClientId, &ucIndex))
    {
        PS_MEM_FREE(WUEPS_PID_AT, pEvent);
        AT_WARN_LOG("At_TAFPbMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        PS_MEM_FREE(WUEPS_PID_AT,pEvent);
        AT_WARN_LOG("At_TAFPbMsgProc: AT_BROADCAST_INDEX.");
        return;
    }

    AT_LOG1("At_PbMsgProc ucIndex",ucIndex);
    AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

    At_PbRspProc(ucIndex,pEvent);

    PS_MEM_FREE(WUEPS_PID_AT,pEvent);

    return;
}


TAF_VOID At_PbMsgProc(MsgBlock* pMsg)
{
    MN_APP_PB_AT_CNF_STRU   *pstMsg;
    TAF_UINT8               ucIndex = 0;

    pstMsg = (MN_APP_PB_AT_CNF_STRU*)pMsg;

    AT_LOG1("At_PbMsgProc pEvent->ClientId",    pstMsg->stPBAtEvent.ClientId);
    AT_LOG1("At_PbMsgProc PBEventType",         pstMsg->stPBAtEvent.PBEventType);
    AT_LOG1("At_PbMsgProc Event Error",         pstMsg->stPBAtEvent.PBError);

    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->stPBAtEvent.ClientId, &ucIndex))
    {
        AT_ERR_LOG1("At_PbMsgProc At_ClientIdToUserId FAILURE", pstMsg->stPBAtEvent.ClientId);
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_PbMsgProc: AT_BROADCAST_INDEX.");
        At_PbIndMsgProc(&pstMsg->stPBAtEvent);
        return;
    }

    AT_LOG1("At_PbMsgProc ucIndex",ucIndex);
    AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

    At_PbRspProc(ucIndex,&pstMsg->stPBAtEvent);

    return;
}


TAF_VOID At_PIHMsgProc(MsgBlock* pMsg)
{
    MN_APP_PIH_AT_CNF_STRU  *pstMsg;
    TAF_UINT8               ucIndex = 0;

    pstMsg = (MN_APP_PIH_AT_CNF_STRU*)pMsg;

    if(PIH_AT_EVENT_CNF != pstMsg->ulMsgId)
    {
        AT_ERR_LOG1("At_PIHMsgProc: The Msg Id is Wrong", pstMsg->ulMsgId);
        return;
    }

    AT_LOG1("At_PIHMsgProc pEvent->ClientId",   pstMsg->stPIHAtEvent.ClientId);
    AT_LOG1("At_PIHMsgProc EventType",          pstMsg->stPIHAtEvent.EventType);
    AT_LOG1("At_PIHMsgProc SIM Event Error",    pstMsg->stPIHAtEvent.PIHError);

    if(AT_FAILURE == At_ClientIdToUserId(pstMsg->stPIHAtEvent.ClientId,&ucIndex))
    {
        AT_ERR_LOG("At_PIHMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        At_PIHIndProc(ucIndex,&pstMsg->stPIHAtEvent);
        AT_WARN_LOG("At_PIHMsgProc : AT_BROADCAST_INDEX.");
        return;
    }

    AT_LOG1("At_PbMsgProc ucIndex",ucIndex);
    AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

    At_PIHRspProc(ucIndex,&pstMsg->stPIHAtEvent);

    return;
}

/* PC工程中AT从A核移到C核, At_sprintf有重复定义,故在此处添加条件编译宏 */
/*lint -e960 Note -- Violates MISRA 2004 Required Rule 12.5, non-primary expression used with logical operator*/
#if  !defined(__PS_WIN32_RECUR__) && !defined(NAS_STUB)
/*lint +e960*/
/*****************************************************************************
 Prototype      : At_ChangeSATCMDNo
 Description    : Sat消息处理函数
 Input          :
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---

*****************************************************************************/
VOS_UINT32 At_ChangeSTKCmdNo(VOS_UINT32 ulCmdType, VOS_UINT8 *ucCmdNo )
{
    switch(ulCmdType)
    {
        case SI_STK_REFRESH:
            *ucCmdNo = SI_AT_CMD_REFRESH;
            break;
        case SI_STK_DISPLAYTET:
            *ucCmdNo = SI_AT_CMD_DISPLAY_TEXT;
            break;
        case SI_STK_GETINKEY:
            *ucCmdNo = SI_AT_CMD_GET_INKEY;
             break;
        case SI_STK_GETINPUT:
            *ucCmdNo = SI_AT_CMD_GET_INPUT;
            break;
        case SI_STK_PLAYTONE:
            *ucCmdNo = SI_AT_CMD_PLAY_TONE;
            break;
        case SI_STK_SELECTITEM:
            *ucCmdNo = SI_AT_CMD_SELECT_ITEM;
            break;
        case SI_STK_SETUPMENU:
            *ucCmdNo = SI_AT_CMD_SETUP_MENU;
            break;
        case SI_STK_SETUPIDLETEXT:
            *ucCmdNo = SI_AT_CMD_SETUP_IDLE_MODE_TEXT;
            break;
        case SI_STK_LAUNCHBROWSER:
            *ucCmdNo = SI_AT_CMD_LAUNCH_BROWSER;
            break;
        case SI_STK_SENDSS:
            *ucCmdNo = SI_AT_CMD_SEND_SS;
            break;
        case SI_STK_LANGUAGENOTIFICATION:
            *ucCmdNo = SI_AT_CMD_LANGUAGENOTIFICATION;
            break;
        case SI_STK_SETFRAMES:
            *ucCmdNo = SI_AT_CMD_SETFRAMES;
            break;
        case SI_STK_GETFRAMESSTATUS:
            *ucCmdNo = SI_AT_CMD_GETFRAMESSTATUS;
            break;
        default:
            return VOS_ERR;
    }

    return VOS_OK;
}

#endif


TAF_UINT32 At_HexText2AsciiStringSimple(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst,TAF_UINT32 ulLen,TAF_UINT8 *pucStr)
{
    TAF_UINT16 usLen = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8 *pWrite = pucDst;
    TAF_UINT8 *pRead = pucStr;
    TAF_UINT8  ucHigh = 0;
    TAF_UINT8  ucLow = 0;

    if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + (2 * ulLen)) >= MaxLength)
    {
        AT_ERR_LOG("At_HexText2AsciiString too long");
        return 0;
    }

    if(0 != ulLen)
    {
        usLen += 1;

        *pWrite++ = '\"';

        /* 扫完整个字串 */
        while( usChkLen++ < ulLen )
        {
            ucHigh = 0x0F & (*pRead >> 4);
            ucLow = 0x0F & *pRead;
            usLen += 2;    /* 记录长度 */

            if(0x09 >= ucHigh)   /* 0-9 */
            {
                *pWrite++ = ucHigh + 0x30;
            }
            else if(0x0A <= ucHigh)    /* A-F */
            {
                *pWrite++ = ucHigh + 0x37;
            }
            else
            {

            }

            if(0x09 >= ucLow)   /* 0-9 */
            {
                *pWrite++ = ucLow + 0x30;
            }
            else if(0x0A <= ucLow)    /* A-F */
            {
                *pWrite++ = ucLow + 0x37;
            }
            else
            {

            }
            /* 下一个字符 */
            pRead++;
        }

        usLen ++;

        *pWrite++ = '\"';

        *pWrite++ = '\0';
    }

    return usLen;
}
TAF_VOID  At_StkCsinIndPrint(TAF_UINT8 ucIndex,SI_STK_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT16 usLength = 0;

    /* 打印输入AT命令类型 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gaucAtCsin);

    /* 打印输入主动命令类型长度和类型 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d",
                                       (pEvent->STKCmdStru.SatCmd.SatDataLen*2));

    /* 有主动命令时才输入 */
    if (0 != pEvent->STKCmdStru.SatCmd.SatDataLen)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            ", %d, ",
                                            pEvent->STKCmdStru.SatType);


        /* 将16进制数转换为ASCII码后输入主动命令内容 */
        usLength += (TAF_UINT16)At_HexText2AsciiStringSimple(AT_CMD_MAX_LEN,
                                                            (TAF_INT8 *)pgucAtSndCodeAddr,
                                                            (TAF_UINT8 *)pgucAtSndCodeAddr + usLength,
                                                            pEvent->STKCmdStru.SatCmd.SatDataLen,
                                                            pEvent->STKCmdStru.SatCmd.SatCmdData);
    }

    /* 打印回车换行 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",
                                        gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
}

/*****************************************************************************
 Prototype      : At_STKCMDDataPrintSimple
 Description    : Sat消息处理函数
 Input          :
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2009-07-04
    Author      : zhuli
    Modification: Created function
*****************************************************************************/
TAF_VOID At_STKCMDDataPrintSimple(TAF_UINT8 ucIndex,SI_STK_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT16 usLength = 0;

    if(SI_STK_CMD_IND_EVENT == pEvent->STKCBEvent)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",gaucAtCsin);
    }
    else
    {
        if(SI_STK_SETUPMENU != pEvent->STKCmdStru.SatType)
        {
            return;
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",gaucAtCsmn);
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                    "%d, %d, ",(pEvent->STKCmdStru.SatCmd.SatDataLen*2), pEvent->STKCmdStru.SatType);

    usLength += (TAF_UINT16)At_HexText2AsciiStringSimple(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,(TAF_UINT8 *)pgucAtSndCodeAddr + usLength,
                                    pEvent->STKCmdStru.SatCmd.SatDataLen, pEvent->STKCmdStru.SatCmd.SatCmdData);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                    "%s",gaucAtCrLf);

    At_SendResultData(ucIndex,pgucAtSndCrLfAddr,usLength+2);

    return ;
}

/*****************************************************************************
 Prototype      : At_SatCallBackFunc
 Description    : Sat消息处理函数
 Input          :
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2009-07-04
    Author      : zhuli
    Modification: Created function
*****************************************************************************/
TAF_VOID At_STKCMDSWPrintSimple(TAF_UINT8 ucIndex,STK_CALLBACK_EVENT STKCBEvent,SI_STK_SW_INFO_STRU *pSw)
{
    TAF_UINT16 usLength = 0;
    VOS_UINT8  *pucSystemAppConfig;

    /* 获取上层对接应用配置: MP/WEBUI/ANDROID */
    pucSystemAppConfig = AT_GetSystemAppConfigAddr();

    if (SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        return ;
    }

    if(SI_STK_TERMINAL_RSP_EVENT == STKCBEvent)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",gaucAtCstr);
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",gaucAtCsen);
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%d, %d%s",pSw->SW1, pSw->SW2,gaucAtCrLf);

    At_SendResultData(ucIndex,pgucAtSndCrLfAddr,usLength+2);

    return ;
}
VOS_VOID AT_SendSTKCMDTypeResultData(
    VOS_UINT8                           ucIndex,
    VOS_UINT16                          usLength
)
{
    if (AT_V_ENTIRE_TYPE == gucAtVType)
    {
        /* Code前面加\r\n */
        PS_MEM_CPY((TAF_CHAR *)pgucAtSndCrLfAddr, (TAF_CHAR *)gaucAtCrLf, 2);
        At_SendResultData(ucIndex, pgucAtSndCrLfAddr, usLength + 2);
    }
    else
    {
        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }

    return;
}
VOS_UINT32 At_STKCMDTypePrint(TAF_UINT8 ucIndex,TAF_UINT32 SatType, TAF_UINT32 EventType)
{
    VOS_UINT8                          *pucSystemAppConfig;
    TAF_UINT16                          usLength = 0;
    TAF_UINT8                           ucCmdType = 0;
    TAF_UINT32                          ulResult = AT_SUCCESS;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    /* 初始化 */
    enModemId       = MODEM_ID_0;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_STKCMDTypePrint: Get modem id fail.");
        return AT_FAILURE;
    }

    /* 对接AP不需要检查 */
    if (SYSTEM_APP_ANDROID != *pucSystemAppConfig)
    {
        if(SI_STK_CMD_END_EVENT != EventType)
        {
            ulResult = At_ChangeSTKCmdNo(SatType, &ucCmdType);
        }

        if(AT_FAILURE == ulResult)
        {
            return AT_FAILURE;
        }
    }

    if(TAF_FALSE == g_ulSTKFunctionFlag)
    {
        return AT_FAILURE;
    }

    switch (EventType)
    {
        case SI_STK_CMD_QUERY_RSP_EVENT:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s %d, 0%s",
                                               gaucAtStgi,
                                               ucCmdType,
                                               gaucAtCrLf);
            break;
        case SI_STK_CMD_IND_EVENT:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s %d, 0, 0%s",
                                               gaucAtStin,
                                               ucCmdType,
                                               gaucAtCrLf);
            break;
        case SI_STK_CMD_END_EVENT:
            if (SYSTEM_APP_ANDROID == *pucSystemAppConfig)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s 0, 0%s",
                                                   gaucAtCsin,
                                                   gaucAtCrLf);
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s 99, 0, 0%s",
                                                   gaucAtStin,
                                                   gaucAtCrLf);
            }
            break;
        default:
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s %d, 0, 1%s",
                                                gaucAtStin,
                                                ucCmdType,
                                                gaucAtCrLf);
            break;
    }

    AT_SendSTKCMDTypeResultData(ucIndex, usLength);

    return AT_SUCCESS;
}


TAF_VOID AT_STKCnfMsgProc(MN_APP_STK_AT_CNF_STRU *pstSTKCnfMsg)
{
    TAF_UINT8                           ucIndex;
    TAF_UINT32                          ulResult;
    VOS_UINT8                          *pucSystemAppConfig;

    ucIndex                             = 0;
    ulResult                            = AT_OK;
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    AT_LOG1("AT_STKCnfMsgProc pEvent->ClientId",   pstSTKCnfMsg->stSTKAtCnf.ClientId);
    AT_LOG1("AT_STKCnfMsgProc EventType",          pstSTKCnfMsg->stSTKAtCnf.STKCBEvent);
    AT_LOG1("AT_STKCnfMsgProc SIM Event Error",    pstSTKCnfMsg->stSTKAtCnf.STKErrorNo);

    gstAtSendData.usBufLen = 0;

    if(AT_FAILURE == At_ClientIdToUserId(pstSTKCnfMsg->stSTKAtCnf.ClientId,&ucIndex))
    {
        AT_ERR_LOG("AT_STKCnfMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    /* 如果不是主动命令，则停止定时器 */
    if((SI_STK_CMD_IND_EVENT != pstSTKCnfMsg->stSTKAtCnf.STKCBEvent)
        &&(SI_STK_CMD_TIMEOUT_IND_EVENT != pstSTKCnfMsg->stSTKAtCnf.STKCBEvent)
        &&(SI_STK_CMD_END_EVENT != pstSTKCnfMsg->stSTKAtCnf.STKCBEvent)
        &&(SI_STK_CC_RESULT_IND_EVENT != pstSTKCnfMsg->stSTKAtCnf.STKCBEvent)
#if (FEATURE_VSIM == FEATURE_ON)
        &&(SI_STK_SMS_PP_DL_DATA_IND_EVENT != pstSTKCnfMsg->stSTKAtCnf.STKCBEvent)
#endif
        &&(SI_STK_SMSCTRL_RESULT_IND_EVENT != pstSTKCnfMsg->stSTKAtCnf.STKCBEvent))
    {
        if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
        {
            AT_WARN_LOG("AT_STKCnfMsgProc: AT_BROADCAST_INDEX.");
            return;
        }

        AT_STOP_TIMER_CMD_READY(ucIndex);

        AT_LOG1("AT_STKCnfMsgProc ucIndex",            ucIndex);
        AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);
    }

    if(AT_SUCCESS != pstSTKCnfMsg->stSTKAtCnf.STKErrorNo)
    {
        ulResult = At_ChgTafErrorCode(ucIndex,(TAF_UINT16)pstSTKCnfMsg->stSTKAtCnf.STKErrorNo);       /* 发生错误 */

        At_FormatResultData(ucIndex,ulResult);
    }
    else
    {
        switch(pstSTKCnfMsg->stSTKAtCnf.STKCBEvent)
        {
            case SI_STK_CMD_IND_EVENT:
                if (SYSTEM_APP_ANDROID == *pucSystemAppConfig)
                {
                    At_StkCsinIndPrint(ucIndex,&(pstSTKCnfMsg->stSTKAtCnf));
                }
                else
                {
                    At_STKCMDTypePrint(ucIndex,pstSTKCnfMsg->stSTKAtCnf.STKCmdStru.SatType,pstSTKCnfMsg->stSTKAtCnf.STKCBEvent);
                }

                break;
            case SI_STK_CMD_END_EVENT:
            case SI_STK_CMD_TIMEOUT_IND_EVENT:
                At_STKCMDTypePrint(ucIndex,pstSTKCnfMsg->stSTKAtCnf.STKCmdStru.SatType,pstSTKCnfMsg->stSTKAtCnf.STKCBEvent);
                break;

            case SI_STK_CMD_QUERY_RSP_EVENT:
                At_STKCMDTypePrint(ucIndex,pstSTKCnfMsg->stSTKAtCnf.STKCmdStru.SatType,pstSTKCnfMsg->stSTKAtCnf.STKCBEvent);
                At_FormatResultData(ucIndex,ulResult);
                break;

            case SI_STK_GET_CMD_RSP_EVENT:
                if (SYSTEM_APP_ANDROID == *pucSystemAppConfig)
                {
                    At_StkCsinIndPrint(ucIndex, &(pstSTKCnfMsg->stSTKAtCnf));
                }
                else
                {
                    At_STKCMDDataPrintSimple(ucIndex, &(pstSTKCnfMsg->stSTKAtCnf));
                }

                At_FormatResultData(ucIndex,ulResult);
                break;

            case SI_STK_ENVELPOE_RSP_EVENT:
            case SI_STK_TERMINAL_RSP_EVENT:
                At_STKCMDSWPrintSimple(ucIndex,pstSTKCnfMsg->stSTKAtCnf.STKCBEvent,&pstSTKCnfMsg->stSTKAtCnf.STKSwStru);
                At_FormatResultData(ucIndex,ulResult);
                break;

            case SI_STK_CC_RESULT_IND_EVENT:
            case SI_STK_SMSCTRL_RESULT_IND_EVENT:
                At_StkCcinIndPrint(ucIndex, &(pstSTKCnfMsg->stSTKAtCnf));
                break;
#if (FEATURE_VSIM == FEATURE_ON)
            case SI_STK_SMS_PP_DL_DATA_IND_EVENT:
                At_StkHvsmrIndPrint(ucIndex, &(pstSTKCnfMsg->stSTKAtCnf));
                break;
#endif
            default:
                At_FormatResultData(ucIndex,ulResult);
                break;
        }
    }

    return;
}


TAF_VOID AT_STKPrintMsgProc(MN_APP_STK_AT_DATAPRINT_STRU *pstSTKPrintMsg)
{
    TAF_UINT8                       ucIndex = 0;

    if(AT_FAILURE == At_ClientIdToUserId(pstSTKPrintMsg->stSTKAtPrint.ClientId,&ucIndex))
    {
        AT_ERR_LOG("AT_STKPrintMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_STKPrintMsgProc: AT_BROADCAST_INDEX.");
        return;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    AT_LOG1("At_STKMsgProc pEvent->ClientId",   pstSTKPrintMsg->stSTKAtPrint.ClientId);
    AT_LOG1("At_STKMsgProc ucIndex",            ucIndex);
    AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

    gucSTKCmdQualify = pstSTKPrintMsg->stSTKAtPrint.CmdQualify;

    VOS_MemCpy(pgucAtSndCodeAddr, pstSTKPrintMsg->stSTKAtPrint.aucData, pstSTKPrintMsg->stSTKAtPrint.DataLen);

    At_SendResultData(ucIndex,pgucAtSndCrLfAddr,(VOS_UINT16)pstSTKPrintMsg->stSTKAtPrint.DataLen+2);

    At_FormatResultData(ucIndex,AT_OK);

    return;
}


TAF_VOID At_STKMsgProc(MsgBlock* pMsg)
{
    MN_APP_STK_AT_DATAPRINT_STRU    *pstSTKPrintMsg;
    MN_APP_STK_AT_CNF_STRU          *pstSTKCnfMsg;

    pstSTKCnfMsg    = (MN_APP_STK_AT_CNF_STRU*)pMsg;
    pstSTKPrintMsg  = (MN_APP_STK_AT_DATAPRINT_STRU*)pMsg;

    if(STK_AT_DATAPRINT_CNF == pstSTKCnfMsg->ulMsgId)
    {
        AT_STKPrintMsgProc(pstSTKPrintMsg);
    }
    else if(STK_AT_EVENT_CNF == pstSTKCnfMsg->ulMsgId)
    {
        AT_STKCnfMsgProc(pstSTKCnfMsg);
    }
    else
    {
        AT_ERR_LOG1("At_STKMsgProc:Msg ID Error",pstSTKPrintMsg->ulMsgId);
    }

    return;
}



TAF_VOID At_DataStatusIndProc(TAF_UINT16  ClientId,
                                  TAF_UINT8      ucDomain,
                                  TAF_UINT8      ucRabId,
                                  TAF_UINT8      ucStatus,
                                  TAF_UINT8      ucCause )
{
    AT_RRETURN_CODE_ENUM_UINT32         ulResult = AT_FAILURE;
    TAF_UINT8 ucIndex = 0;
    TAF_UINT16 usLength = 0;

    AT_LOG1("At_DataStatusIndProc ClientId",ClientId);
    AT_LOG1("At_DataStatusIndProc ucDomain",ucDomain);
    AT_LOG1("At_DataStatusIndProc ucRabId",ucRabId);
    AT_LOG1("At_DataStatusIndProc ucStatus",ucStatus);
    AT_LOG1("At_DataStatusIndProc ucRabId",ucCause);
    if(AT_FAILURE == At_ClientIdToUserId(ClientId,&ucIndex))
    {
        AT_WARN_LOG("At_DataStatusIndProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_DataStatusIndProc: AT_BROADCAST_INDEX.");
        return;
    }

    AT_LOG1("At_DataStatusIndProc ucIndex",ucIndex);
    AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

    switch(ucStatus)
    {
    case TAF_RABM_STOP_DATA:
    case TAF_DATA_STOP:
        break;

      default:
        break;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}

TAF_VOID At_DataStatusMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    TAF_UINT16  ClientId = 0;
    TAF_UINT8      ucDomain = 0;
    TAF_UINT8      ucRabId = 0;
    TAF_UINT8      ucStatus = 0;
    TAF_UINT8      ucCause = 0;
    TAF_UINT16 usAddr = 0;

    PS_MEM_CPY(&ClientId,pData,sizeof(ClientId));
    usAddr += sizeof(ClientId);

    PS_MEM_CPY(&ucDomain,pData+usAddr,sizeof(ucDomain));
    usAddr += sizeof(ucDomain);

    PS_MEM_CPY(&ucRabId,pData+usAddr,sizeof(ucRabId));
    usAddr += sizeof(ucRabId);

    PS_MEM_CPY(&ucStatus,pData+usAddr,sizeof(ucStatus));
    usAddr += sizeof(ucStatus);

    PS_MEM_CPY(&ucCause,pData+usAddr,sizeof(ucCause));
    usAddr += sizeof(ucCause);

    At_DataStatusIndProc(ClientId,ucDomain,ucRabId,ucStatus,ucCause);
}


VOS_UINT32 AT_ConvertCallError(TAF_CS_CAUSE_ENUM_UINT32 enCause)
{
    AT_CME_CALL_ERR_CODE_MAP_STRU      *pstCallErrMapTblPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulCallErrMapTblSize;
    VOS_UINT32                          ulCnt;

    pstCallErrMapTblPtr = AT_GET_CME_CALL_ERR_CODE_MAP_TBL_PTR();
    ulCallErrMapTblSize = AT_GET_CME_CALL_ERR_CODE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulCallErrMapTblSize; ulCnt++)
    {
        if (pstCallErrMapTblPtr[ulCnt].enCsCause == enCause)
        {
            return pstCallErrMapTblPtr[ulCnt].ulCmeCode;
        }
    }

    return AT_CME_UNKNOWN;
}


TAF_VOID At_CmdCnfMsgProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    AT_CMD_CNF_EVENT                    *pstCmdCnf;
    MN_CLIENT_ID_T                      clientId;
    TAF_UINT32                          ulErrorCode;
    TAF_UINT8                           ucIndex;
    TAF_UINT32                          ulResult = AT_FAILURE;
    TAF_UINT16                          usLength = 0;

    pstCmdCnf = (AT_CMD_CNF_EVENT *)pData;

    clientId    = pstCmdCnf->clientId;
    ulErrorCode = pstCmdCnf->ulErrorCode;

    if(AT_FAILURE == At_ClientIdToUserId(clientId,&ucIndex))
    {
        AT_WARN_LOG("At_CmdCnfMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_CmdCnfMsgProc: AT_BROADCAST_INDEX.");
        return;
    }

    if (AT_FW_CLIENT_STATUS_READY == g_stParseContext[ucIndex].ucClientStatus)
    {
        AT_WARN_LOG("At_CmdCnfMsgProc : AT command entity is released.");
        return;
    }

    /*
        call业务上报的是TAF_CS_CAUSE_SUCCESS，而短信业务上报的是MN_ERR_NO_ERROR,
        他们的实际值都为0
    */
    if (MN_ERR_NO_ERROR == ulErrorCode)
    {
        /* 因判断是否有呼叫在C核上实现，在无呼叫的情况下上报MN_ERR_NO_ERROR
           AT命令返回结果需要为AT_OK */
        if (AT_CMD_H_SET == gastAtClientTab[ucIndex].CmdCurrentOpt
         || AT_CMD_CHUP_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex,AT_OK);
        }

        AT_NORM_LOG("At_CmdCnfMsgProc Rsp No Err");
        return;
    }

    AT_LOG1("At_CmdCnfMsgProc ucIndex",ucIndex);
    AT_LOG1("gastAtClientTab[ucIndex].CmdCurrentOpt",gastAtClientTab[ucIndex].CmdCurrentOpt);

    switch(gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
    case AT_CMD_CDUR_READ:
    /* CCWA命令相关 */
    case AT_CMD_CCWA_DISABLE:
    case AT_CMD_CCWA_ENABLE:
    case AT_CMD_CCWA_QUERY:

    /* CCFC命令 */
    case AT_CMD_CCFC_DISABLE:
    case AT_CMD_CCFC_ENABLE:
    case AT_CMD_CCFC_QUERY:
    case AT_CMD_CCFC_REGISTRATION:
    case AT_CMD_CCFC_ERASURE:

    /* CUSD相关命令 */
    case AT_CMD_CUSD_REQ:

    /* CLCK相关命令 */
    case AT_CMD_CLCK_UNLOCK:
    case AT_CMD_CLCK_LOCK:
    case AT_CMD_CLCK_QUERY:

    /* CLOP命令 */
    case AT_CMD_COLP_READ:

    /* CLIR命令 */
    case AT_CMD_CLIR_READ:

    /* CLIP命令 */
    case AT_CMD_CLIP_READ:
    /* CPWD命令 */
    case AT_CMD_CPWD_SET:
        ulResult = AT_ConvertCallError(ulErrorCode);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    case AT_CMD_CSCA_READ:
        ulResult = At_ChgMnErrCodeToAt(ucIndex, ulErrorCode);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    case AT_CMD_CPMS_SET:
    case AT_CMD_CPMS_READ:
        ulResult = AT_CMS_UNKNOWN_ERROR;
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;


    /*
        如果ulErrorCode不为TAF_CS_CAUSE_NO_CALL_ID，则AT_CMD_D_CS_VOICE_CALL_SET
        和AT_CMD_D_CS_DATA_CALL_SET业务统一上报AT_NO_CARRIER错误值
    */

    case AT_CMD_D_CS_VOICE_CALL_SET:
    case AT_CMD_APDS_SET:
        if (TAF_CS_CAUSE_NO_CALL_ID == ulErrorCode)
        {
            ulResult = AT_ERROR;
            AT_STOP_TIMER_CMD_READY(ucIndex);
            break;
        }

    case AT_CMD_D_CS_DATA_CALL_SET:
         ulResult = AT_NO_CARRIER;
         AT_STOP_TIMER_CMD_READY(ucIndex);
         break;

    case AT_CMD_CHLD_SET:
    case AT_CMD_CHUP_SET:
    case AT_CMD_A_SET:
    case AT_CMD_CHLD_EX_SET:
    case AT_CMD_H_SET:
        ulResult = AT_ConvertCallError(ulErrorCode);                            /* 发生错误 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    case AT_CMD_CMGR_SET:
    case AT_CMD_CMGD_SET:
        if (MN_ERR_CLASS_SMS_EMPTY_REC == ulErrorCode)
        {
            ulResult = AT_OK;
            AT_STOP_TIMER_CMD_READY(ucIndex);
            break;
        }
        /* fall through */
    case AT_CMD_CSMS_SET:
    case AT_CMD_CMMS_SET:
    case AT_CMD_CMMS_READ:
    case AT_CMD_CSMP_READ:    /*del*/
    case AT_CMD_CMGS_TEXT_SET:
    case AT_CMD_CMGS_PDU_SET:
    case AT_CMD_CMGC_TEXT_SET:
    case AT_CMD_CMGC_PDU_SET:
    case AT_CMD_CMSS_SET:
    case AT_CMD_CMST_SET:
    case AT_CMD_CNMA_TEXT_SET:
    case AT_CMD_CNMA_PDU_SET:
    case AT_CMD_CMGW_PDU_SET:
    case AT_CMD_CMGW_TEXT_SET:
    case AT_CMD_CMGL_SET:
    case AT_CMD_CMGD_TEST:
    case AT_CMD_CSMP_SET:
    case AT_CMD_CSCA_SET:
#if ((FEATURE_ON == FEATURE_GCBS) || (FEATURE_ON == FEATURE_WCBS))
    case AT_CMD_CSCB_SET:
    case AT_CMD_CSCB_READ:
#endif
        ulResult = At_ChgMnErrCodeToAt(ucIndex,ulErrorCode);                     /* 发生错误 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    default:
        /*默认值不知道是不是该填这个，暂时先写这个*/
        ulResult = AT_CME_UNKNOWN;
        AT_STOP_TIMER_CMD_READY(ucIndex);
        break;
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

}
TAF_UINT32 At_PrintTimeZoneInfo(
    NAS_MM_INFO_IND_STRU                *pstMmInfo,
    TAF_UINT8                           *pDst
)
{
    TAF_INT8    cTimeZone;
    TAF_INT8    cQuarterNum;
    TAF_UINT8   ucTimeZoneValue;
    TAF_UINT8   ucQuarterNumValue;
    TAF_UINT16  usLength;

    usLength    = 0;
    cTimeZone   = AT_INVALID_TZ_VALUE;
    cQuarterNum = 0;

    /* 获得时区并转换，网络下发的时区以15分钟为单位，转成小时, 一时区对应一个小时 */
    if (NAS_MM_INFO_IE_UTLTZ == (pstMmInfo->ucIeFlg & NAS_MM_INFO_IE_UTLTZ))
    {
        cTimeZone   = pstMmInfo->stUniversalTimeandLocalTimeZone.cTimeZone / 4;
        cQuarterNum = pstMmInfo->stUniversalTimeandLocalTimeZone.cTimeZone % 4;
    }

    if (NAS_MM_INFO_IE_LTZ == (pstMmInfo->ucIeFlg & NAS_MM_INFO_IE_LTZ))
    {
        cTimeZone   = pstMmInfo->cLocalTimeZone / 4;
        cQuarterNum = pstMmInfo->cLocalTimeZone % 4;
    }

    /* 根据夏时制对时区进行调整 */
    if (NAS_MM_INFO_IE_DST == (pstMmInfo->ucIeFlg & NAS_MM_INFO_IE_DST))
    {
        cTimeZone -= (TAF_INT8)pstMmInfo->ucDST;
    }

    if (cTimeZone < 0)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pDst + usLength,
                                           "GMT-");

        ucTimeZoneValue     = (TAF_UINT8)(cTimeZone * (-1));
        ucQuarterNumValue   = (TAF_UINT8)(cQuarterNum * (-1));
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pDst + usLength,
                                           "GMT+");

        ucTimeZoneValue     = (TAF_UINT8)cTimeZone;
        ucQuarterNumValue   = (TAF_UINT8)cQuarterNum;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pDst + usLength,
                                       "%d",
                                       ucTimeZoneValue);

    if (ucQuarterNumValue != 0)
    {
        /* 显示非整数时区 */
        ucQuarterNumValue *= 15;
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pDst + usLength,
                                           ":%d",
                                           ucQuarterNumValue);
    }

    /* 显示夏时制或冬时制信息 */
    if (  (NAS_MM_INFO_IE_DST == (pstMmInfo->ucIeFlg & NAS_MM_INFO_IE_DST))
        && (0 < pstMmInfo->ucDST))
    {
        /* 夏时制: DST字段存在，且值大于0，*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pDst + usLength,
                                           ", Summer Time\"%s",
                                           gaucAtCrLf);
    }
    else
    {
        /* 冬时制 */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pDst + usLength,
                                           ", Winter Time\"%s",
                                           gaucAtCrLf);
    }

    return usLength;
}


VOS_UINT32 AT_PrintTimeZoneInfoNoAdjustment(
    NAS_MM_INFO_IND_STRU               *pstMmInfo,
    VOS_UINT8                          *pucDst
)
{
    VOS_INT8                            cTimeZone;
    VOS_UINT8                           ucTimeZoneValue;
    VOS_UINT16                          usLength;

    usLength  = 0;
    cTimeZone = AT_INVALID_TZ_VALUE;

    /* 获得时区 */
    if (NAS_MM_INFO_IE_UTLTZ == (pstMmInfo->ucIeFlg & NAS_MM_INFO_IE_UTLTZ))
    {
        cTimeZone   = pstMmInfo->stUniversalTimeandLocalTimeZone.cTimeZone;
    }


    if (cTimeZone < 0)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pucDst + usLength,
                                           "-");

        ucTimeZoneValue = (VOS_UINT8)(cTimeZone * (-1));
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pucDst + usLength,
                                           "+");

        ucTimeZoneValue = (VOS_UINT8)cTimeZone;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pucDst + usLength,
                                       "%d",
                                       ucTimeZoneValue);


    /* 显示夏时制或冬时制信息 */
    if ( (NAS_MM_INFO_IE_DST == (pstMmInfo->ucIeFlg & NAS_MM_INFO_IE_DST))
      && (pstMmInfo->ucDST > 0))
    {
        /* 夏时制: DST字段存在, 且值大于0，*/
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pucDst + usLength,
                                           ",%02d\"%s",
                                           pstMmInfo->ucDST,
                                           gaucAtCrLf);
    }
    else
    {
        /* 冬时制 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pucDst + usLength,
                                           ",00\"%s",
                                           gaucAtCrLf);
    }

    return usLength;
}

/*****************************************************************************
 函 数 名  : At_GetDaysForEachMonth
 功能描述  : 获得每个月的天数
 输入参数  : VOS_UINT8 ucYear     - 年份
             VOS_UINT8 ucMonth    - 月份
 输出参数  : 无
 返 回 值  : VOS_UINT8 : 指定年月的天数
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年7月1日
    作    者   : 欧阳飞
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 At_GetDaysForEachMonth(
    VOS_UINT8                               ucYear,
    VOS_UINT8                               ucMonth
)
{
    VOS_UINT16   usAdjustYear;

    /* 输入的year值为两位数，默认从2000年开始计算 */
    usAdjustYear = 2000 + ucYear;

    if ((1 == ucMonth) || (3 == ucMonth) || (5 == ucMonth) || (7 == ucMonth)
     || (8 == ucMonth) || (10 == ucMonth) || (12 == ucMonth) )
    {
        /* 1,3,5,7,8,10,12月有31天 */
        return 31;
    }
    else if ((4 == ucMonth) || (6 == ucMonth) || (9 == ucMonth) || (11 == ucMonth))
    {
        /* 4,6,9,11月有30天 */
        return 30;
    }
    else
    {
        /* 2月看是否为闰年，是则为29天，否则为28天 */
        if ( ((0 == (usAdjustYear % 4)) && (0 != (usAdjustYear % 100))) || (0 == (usAdjustYear % 400)))
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

/*****************************************************************************
 函 数 名  : At_AdjustLocalDate
 功能描述  : 根据时区信息，调整本地日期
 输入参数  : TIME_ZONE_TIME_STRU *pstUinversalTime - 通用时间结构指针
 输出参数  : VOS_INT8            cAdjustValue      - 日期调整值
             VOS_UINT8           *pucDay           - 指向调整后的本地日期
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年7月1日
    作    者   : 欧阳飞
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID At_AdjustLocalDate(
    TIME_ZONE_TIME_STRU                 *pstUinversalTime,
    VOS_INT8                            cAdjustValue,
    TIME_ZONE_TIME_STRU                 *pstLocalTime
)
{
    VOS_UINT8    ucDay;

    /* 调整日期 */
    ucDay = (VOS_UINT8)(pstUinversalTime->ucDay + cAdjustValue);

    if (0 == ucDay)
    {
        /* 月份减一 */
        if ( 1 == pstUinversalTime->ucMonth )
        {
            /* 调整为上一年的12月份,年份减一 */
            pstLocalTime->ucMonth = 12;

            if (0 == pstUinversalTime->ucYear)
            {
                /* 如果是2000年，调整为1999年 */
                pstLocalTime->ucYear = 99;
            }
            else
            {
                pstLocalTime->ucYear = pstUinversalTime->ucYear - 1;
            }
        }
        else
        {
            pstLocalTime->ucMonth = pstUinversalTime->ucMonth - 1;
            pstLocalTime->ucYear  = pstUinversalTime->ucYear;
        }

        /* 日期调整为上个月的最后一天, */
        pstLocalTime->ucDay = At_GetDaysForEachMonth(pstLocalTime->ucYear, pstLocalTime->ucMonth);
    }
    else if (ucDay > At_GetDaysForEachMonth(pstUinversalTime->ucYear, pstUinversalTime->ucMonth))
    {
        /*日期调整为下个月一号 */
        pstLocalTime->ucDay = 1;

        /* 月份加一 */
        if ( 12 == pstUinversalTime->ucMonth )
        {
            /* 调整为下一年的1月份,年份加一 */
            pstLocalTime->ucMonth = 1;
            pstLocalTime->ucYear = pstUinversalTime->ucYear + 1;
        }
        else
        {
            pstLocalTime->ucMonth = pstUinversalTime->ucMonth + 1;
            pstLocalTime->ucYear = pstUinversalTime->ucYear;
        }
    }
    else
    {
        pstLocalTime->ucDay   = ucDay;
        pstLocalTime->ucMonth = pstUinversalTime->ucMonth;
        pstLocalTime->ucYear  = pstUinversalTime->ucYear;
    }
}


VOS_VOID At_UniversalTime2LocalTime(
    TIME_ZONE_TIME_STRU                 *pstUinversalTime,
    TIME_ZONE_TIME_STRU                 *pstLocalTime
)
{
    VOS_INT8    cTemp;
    VOS_INT8    cAdjustValue;

    pstLocalTime->cTimeZone = pstUinversalTime->cTimeZone;

    /* 根据时区信息，将通用时间转换为本地时间。时区信息是以15分钟为单位 */

    /* 秒无需调整 */
    pstLocalTime->ucSecond  = pstUinversalTime->ucSecond;

    /* 分钟数调整 */
    cTemp = (VOS_INT8)(((pstUinversalTime->cTimeZone % 4) * 15) + pstUinversalTime->ucMinute);
    if (cTemp >= 60)
    {
        /*时区调整后，分钟数超过60分钟，小时数加 1 */
        pstLocalTime->ucMinute  = (VOS_UINT8)(cTemp - 60);
        cAdjustValue = 1;
    }
    else if (cTemp < 0)
    {
        /*时区调整后，分钟数小于0分钟，小时数减 1 */
        pstLocalTime->ucMinute  = (VOS_UINT8)(cTemp + 60);
        cAdjustValue = -1;
    }
    else
    {
        pstLocalTime->ucMinute = (VOS_UINT8)cTemp;
        cAdjustValue = 0;
    }

    /* 小时数调整 */
    cTemp = (VOS_INT8)((pstUinversalTime->cTimeZone / 4) + pstUinversalTime->ucHour + cAdjustValue);

    if (cTemp >= 24)
    {
        /*时区调整后，时间超过24小时，日期加 1 */
        pstLocalTime->ucHour = (VOS_UINT8)(cTemp - 24);
        cAdjustValue = 1;
    }
    else if (cTemp < 0)
    {
        /*时区调整后，时间小于0，日期减 1 */
        pstLocalTime->ucHour = (VOS_UINT8)(cTemp + 24);
        cAdjustValue = -1;
    }
    else
    {
        pstLocalTime->ucHour = (VOS_UINT8)cTemp;
        cAdjustValue = 0;
    }

    /* 调整本地日期 */
    At_AdjustLocalDate(pstUinversalTime, cAdjustValue, pstLocalTime);

    return;
}
TAF_UINT32 At_PrintMmTimeInfo(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent,
    TAF_UINT8                          *pDst
)
{
    TAF_UINT16                          usLength;
    TAF_INT8                            cTimeZone;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulChkTimeFlg;
    VOS_UINT32                          ulChkCtzvFlg;

    usLength = 0;

    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_PrintMmTimeInfo: Get modem id fail.");
        return usLength;
    }

    pstNetCtx = AT_GetModemNetCtxAddrFromModemId(enModemId);

    if (NAS_MM_INFO_IE_UTLTZ == (pEvent->stMmInfo.ucIeFlg & NAS_MM_INFO_IE_UTLTZ))
    {
        /* 保存网络下发的时间信息，无该字段，则使用原有值 */
        pstNetCtx->stTimeInfo.ucIeFlg |= NAS_MM_INFO_IE_UTLTZ;
        pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone = pEvent->stMmInfo.stUniversalTimeandLocalTimeZone;
    }

    /* 更新DST信息 */
    if (NAS_MM_INFO_IE_DST == (pEvent->stMmInfo.ucIeFlg & NAS_MM_INFO_IE_DST))
    {
        /* 保存网络下发的时间信息 */
        pstNetCtx->stTimeInfo.ucIeFlg |= NAS_MM_INFO_IE_DST;
        pstNetCtx->stTimeInfo.ucDST = pEvent->stMmInfo.ucDST;
    }
    else
    {
        pstNetCtx->stTimeInfo.ucIeFlg &= ~NAS_MM_INFO_IE_DST;
    }

    ulChkCtzvFlg    = AT_CheckRptCmdStatus(pEvent->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CTZV);
    ulChkTimeFlg    = AT_CheckRptCmdStatus(pEvent->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_TIME);

    /*时区显示格式: +CTZV: "GMT±tz, Summer(Winter) Time" */
    if ( (VOS_TRUE == AT_CheckRptCmdStatus(pEvent->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CTZV))
      && (VOS_TRUE == ulChkCtzvFlg) )

    {
        if (NAS_MM_INFO_IE_UTLTZ == (pEvent->stMmInfo.ucIeFlg & NAS_MM_INFO_IE_UTLTZ))
        {
            cTimeZone = pEvent->stMmInfo.stUniversalTimeandLocalTimeZone.cTimeZone;
        }
        else
        {
            cTimeZone = pEvent->stMmInfo.cLocalTimeZone;
        }

        if (cTimeZone != pstNetCtx->stTimeInfo.cLocalTimeZone)
        {
            /* 保存网络下发的时区信息 */
            pstNetCtx->stTimeInfo.ucIeFlg |= NAS_MM_INFO_IE_LTZ;
            pstNetCtx->stTimeInfo.cLocalTimeZone = cTimeZone;
            pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.cTimeZone = cTimeZone;

            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pDst + usLength,
                                               "%s%s\"",gaucAtCrLf,
                                               gastAtStringTab[AT_STRING_CTZV].pucText);

            usLength += (TAF_UINT16)At_PrintTimeZoneInfo(&(pstNetCtx->stTimeInfo),
                                                         pDst + usLength);
        }
    }

    /*时间显示格式: ^TIME: "yy/mm/dd,hh:mm:ss(+/-)tz,dst" */
    if ( (VOS_TRUE == AT_CheckRptCmdStatus(pEvent->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_TIME))
      && (VOS_TRUE == ulChkTimeFlg)
      && (NAS_MM_INFO_IE_UTLTZ == (pEvent->stMmInfo.ucIeFlg & NAS_MM_INFO_IE_UTLTZ)) )
    {
        /* "^TIME: */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "%s%s",gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_TIME].pucText);

        /* YY */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "\"%d%d/",
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucYear / 10,
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucYear % 10);
        /* MM */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "%d%d/",
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucMonth / 10,
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucMonth % 10);
        /* dd */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "%d%d,",
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucDay / 10,
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucDay % 10);

        /* hh */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "%d%d:",
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucHour / 10,
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucHour % 10);

        /* mm */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "%d%d:",
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucMinute / 10,
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucMinute % 10);

        /* ss */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pDst + usLength,
                                           "%d%d",
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucSecond / 10,
                                           pstNetCtx->stTimeInfo.stUniversalTimeandLocalTimeZone.ucSecond % 10);

        /* GMT±tz, Summer(Winter) Time" */
        usLength += (VOS_UINT16)AT_PrintTimeZoneInfoNoAdjustment(&(pstNetCtx->stTimeInfo),
                                                                 pDst + usLength);
    }

    return usLength;
}

/* begin V7R1 PhaseI Modify */
/*****************************************************************************
 函 数 名  : AT_GetSysModeName
 功能描述  : 获取当前SysMode的名称
 输入参数  : enSysMode:当前系统模式
 输出参数  : pucSysModeName:系统模式名称
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月7日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  AT_GetSysModeName(
    MN_PH_SYS_MODE_EX_ENUM_U8           enSysMode,
    VOS_CHAR                           *pucSysModeName
)
{
    VOS_UINT32                          i;

    for ( i = 0 ; i < sizeof(g_astSysModeTbl)/sizeof(AT_PH_SYS_MODE_TBL_STRU) ; i++ )
    {
        if ( g_astSysModeTbl[i].enSysMode == enSysMode)
        {
            VOS_StrNCpy(pucSysModeName,
                        g_astSysModeTbl[i].pcStrSysModeName,
                        VOS_StrLen(g_astSysModeTbl[i].pcStrSysModeName));

            return VOS_OK;
        }
    }

    return VOS_ERR;
}

/*****************************************************************************
 函 数 名  : AT_GetSubSysModeName
 功能描述  : 获取当前SubSysMode的名称
 输入参数  : enSubSysMode:当前子系统模式
 输出参数  : pucSubSysModeName:子系统模式名称
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月7日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  AT_GetSubSysModeName(
    MN_PH_SUB_SYS_MODE_EX_ENUM_U8       enSubSysMode,
    VOS_CHAR                           *pucSubSysModeName
)
{
    VOS_UINT32                          i;

    for ( i = 0 ; i < sizeof(g_astSubSysModeTbl)/sizeof(AT_PH_SUB_SYS_MODE_TBL_STRU) ; i++ )
    {
        if ( g_astSubSysModeTbl[i].enSubSysMode == enSubSysMode)
        {
            VOS_StrNCpy(pucSubSysModeName,
                        g_astSubSysModeTbl[i].pcStrSubSysModeName,
                        VOS_StrLen(g_astSubSysModeTbl[i].pcStrSubSysModeName));

            return VOS_OK;
        }
    }

    return VOS_ERR;
}

/*****************************************************************************
 函 数 名  : AT_QryParaRspSysinfoExProc
 功能描述  : 查询^SYSINFOEX的返回值
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月7日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  AT_QryParaRspSysinfoExProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_CHAR                            aucSysModeName[255];
    VOS_CHAR                            aucSubSysModeName[255];
    TAF_PH_SYSINFO_STRU                 stSysInfo;
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();



    PS_MEM_CPY(&stSysInfo, pPara, sizeof(TAF_PH_SYSINFO_STRU));

    usLength  = 0;
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s:%d",g_stParseContext[ucIndex].pstCmdElement->pszCmdName,stSysInfo.ucSrvStatus);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSrvDomain);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucRoamStatus);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSimStatus);

    if ( SYSTEM_APP_WEBUI == *pucSystemAppConfig)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSimLockStatus);
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",");
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSysMode);

    PS_MEM_SET(aucSysModeName,          0x00, sizeof(aucSysModeName));
    PS_MEM_SET(aucSubSysModeName,       0x00, sizeof(aucSubSysModeName));

    /* 获取SysMode的名字 */
    AT_GetSysModeName(stSysInfo.ucSysMode, aucSysModeName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"",aucSysModeName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stSysInfo.ucSysSubMode);

    /* 获取SubSysMode的名字 */
    AT_GetSubSysModeName(stSysInfo.ucSysSubMode, aucSubSysModeName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"",aucSubSysModeName);
    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
}
/* end V7R1 PhaseI Modify */

VOS_VOID  AT_QryParaRegisterTimeProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulCostTime;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    ulResult   = AT_OK;
    usLength   = 0;
    PS_MEM_CPY(&ulCostTime, pPara, sizeof(VOS_UINT32));

    /* MMA报给AT时，按slice上报，(32 * 1024)个slice是1S
       如果slice为0，表示没有注册成功；如果slice小于1S,AT按1S上报 */

    /* 将查询时间上报给APP*/
    if ( 0 ==  ulCostTime)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulCostTime = ulCostTime/(32 * 1024);

        if (0 == ulCostTime)
        {
            ulCostTime = 1;
        }

        ulResult   = AT_OK;

        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s",
                                          gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:%d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           ulCostTime);
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

    return;
}
VOS_VOID  AT_QryParaAnQueryProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult;
    MN_MMA_ANQUERY_PARA_STRU            stAnqueryPara;
    AT_CMD_ANTENNA_LEVEL_ENUM_UINT8     enCurAntennaLevel;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                          *pucSystemAppConfig;
    VOS_INT16                          sRsrp;
    VOS_INT16                          sRsrq;
    VOS_UINT8                          ucLevel;
    VOS_INT16                          sRssi;
#endif
    /* 初始化 */
    ulResult   = AT_OK;
    PS_MEM_CPY(&stAnqueryPara, pPara, sizeof(MN_MMA_ANQUERY_PARA_STRU));

    if((TAF_PH_RAT_GSM  == stAnqueryPara.enServiceSysMode)
    || (TAF_PH_RAT_WCDMA == stAnqueryPara.enServiceSysMode))
    {
        pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);


        /* 上报数据转换:将 Rscp、Ecio显示为非负值，若Rscp、Ecio为-145，-32，或者rssi为99，
           则转换为0 */
        if ( ((0 == stAnqueryPara.u.st2G3GCellSignInfo.sCpichRscp) && (0 == stAnqueryPara.u.st2G3GCellSignInfo.sCpichEcNo))
          || (99 == stAnqueryPara.u.st2G3GCellSignInfo.ucRssi) )
        {
            /* 丢网返回0, 对应应用的圈外 */
            enCurAntennaLevel       = AT_CMD_ANTENNA_LEVEL_0;
        }
        else
        {
            /* 调用函数AT_CalculateAntennaLevel来根据D25算法计算出信号格数 */
            enCurAntennaLevel = AT_CalculateAntennaLevel(stAnqueryPara.u.st2G3GCellSignInfo.sCpichRscp,
                                                         stAnqueryPara.u.st2G3GCellSignInfo.sCpichEcNo);
        }

        /* 信号磁滞处理 */
        AT_GetSmoothAntennaLevel(ucIndex, enCurAntennaLevel );

        stAnqueryPara.u.st2G3GCellSignInfo.sCpichRscp     = -stAnqueryPara.u.st2G3GCellSignInfo.sCpichRscp;
        stAnqueryPara.u.st2G3GCellSignInfo.sCpichEcNo     = -stAnqueryPara.u.st2G3GCellSignInfo.sCpichEcNo;

#if (FEATURE_ON == FEATURE_LTE)
        pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

        if ( SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:%d,%d,%d,%d,0,0",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.sCpichRscp,
                                                        (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.sCpichEcNo,
                                                        (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.ucRssi,
                                                        (VOS_INT32)pstNetCtx->enCalculateAntennaLevel);


            /* 回复用户命令结果 */
            At_FormatResultData(ucIndex,ulResult);

            return;
        }
#endif
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       "%s:%d,%d,%d,%d,0x%X",
                                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                       (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.sCpichRscp,
                                                       (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.sCpichEcNo,
                                                       (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.ucRssi,
                                                       (VOS_INT32)pstNetCtx->enCalculateAntennaLevel,
                                                       (VOS_INT32)stAnqueryPara.u.st2G3GCellSignInfo.ulCellId);

        /* 回复用户命令结果 */
        At_FormatResultData(ucIndex,ulResult);

        return;
    }
    else if(TAF_PH_RAT_LTE == stAnqueryPara.enServiceSysMode)
    {
 #if (FEATURE_ON == FEATURE_LTE)
            sRsrp   = stAnqueryPara.u.st4GCellSignInfo.sRsrp;
            sRsrq   = stAnqueryPara.u.st4GCellSignInfo.sRsrq;
            sRssi   = stAnqueryPara.u.st4GCellSignInfo.sRssi;

            AT_CalculateLTESignalValue(&sRssi,&ucLevel,&sRsrp,&sRsrq);

            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:0,99,%d,%d,%d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_INT32)sRssi,
                                                        (VOS_INT32)ucLevel,
                                                        (VOS_INT32)sRsrp,
                                                        (VOS_INT32)sRsrq);


            /* 回复用户命令结果 */
            At_FormatResultData(ucIndex,ulResult);

            return;
    }
    else
    {
        AT_WARN_LOG("AT_QryParaAnQueryProc:WARNING: THE RAT IS INVALID!");
        return;
    }
#endif

}



VOS_VOID  AT_QryParaHomePlmnProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;

    TAF_HPLMN_WITH_MNC_LEN_STRU         stHplmn;

    /* 初始化 */
    ulResult   = AT_OK;
    usLength   = 0;

    PS_MEM_CPY(&stHplmn, pPara, sizeof(TAF_HPLMN_WITH_MNC_LEN_STRU));

    /* 上报MCC和MNC */
    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s:",
                                       (VOS_INT8*)g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%X%X%X",
                                       (VOS_INT32)(0x0f & stHplmn.stHplmn.Mcc) ,
                                       (VOS_INT32)(0x0f00 & stHplmn.stHplmn.Mcc) >> 8,
                                       (VOS_INT32)(0x0f0000 & stHplmn.stHplmn.Mcc) >> 16);

    if (2 == stHplmn.ucHplmnMncLen)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%X%X",
                                           (VOS_INT32)(0x0f & stHplmn.stHplmn.Mnc) ,
                                           (VOS_INT32)(0x0f00 & stHplmn.stHplmn.Mnc) >> 8);
    }
    else if (3 == stHplmn.ucHplmnMncLen)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%X%X%X",
                                           (VOS_INT32)(0x0f & stHplmn.stHplmn.Mnc) ,
                                           (VOS_INT32)(0x0f00 & stHplmn.stHplmn.Mnc) >> 8,
                                           (VOS_INT32)(0x0f0000 & stHplmn.stHplmn.Mnc) >> 16);
    }
    else
    {
        AT_WARN_LOG("AT_QryParaHomePlmnProc HPLMN MNC LEN INVAILID");
    }

    gstAtSendData.usBufLen = usLength;

    /* 回复用户命令结果 */
    At_FormatResultData(ucIndex,ulResult);

    return;
}


/*****************************************************************************
 函 数 名  : AT_QryParaCsnrProc
 功能描述  : 生成AT^CSNR命令的返回结果
 输入参数  :
     VOS_UINT8                           ucIndex,
     VOS_UINT8                           OpId,
     VOS_VOID                           *pPara
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月5日
    作    者   : 欧阳飞
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  AT_QryParaCsnrProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult;
    MN_MMA_CSNR_PARA_STRU               stCsnrPara;

    /* 初始化 */
    ulResult   = AT_OK;
    PS_MEM_CPY(&stCsnrPara, pPara, sizeof(MN_MMA_CSNR_PARA_STRU));

    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR*)pgucAtSndCodeAddr,
                                                    "%s: %d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    (TAF_INT32)stCsnrPara.sCpichRscp,
                                                    (TAF_INT32)stCsnrPara.sCpichEcNo);

    /* 回复用户命令结果 */
    At_FormatResultData(ucIndex,ulResult);

    return;
}


VOS_VOID  AT_QryParaSpnProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    VOS_UINT32                          ulResult;
    MN_MMA_SPN_PARA_STRU                stMnMmaSpnInfo;
    TAF_PH_USIM_SPN_STRU                stAtSPNRslt;
    VOS_UINT16                          usLength;
    VOS_UINT16                          usDatalen;
    VOS_UINT8                          *pucSystemAppConfig;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* 初始化 */
    ulResult  = AT_OK;
    usLength  = 0;
    usDatalen = 0;
    PS_MEM_CPY(&stMnMmaSpnInfo, pPara, sizeof(MN_MMA_SPN_PARA_STRU));

    /* 0 表示SIM */
    if (1 == pstNetCtx->ucSpnType)
    {
        PS_MEM_CPY(&stAtSPNRslt, &stMnMmaSpnInfo.stUsimSpnInfo, sizeof(TAF_PH_USIM_SPN_STRU));
    }
    else
    {
        PS_MEM_CPY(&stAtSPNRslt, &stMnMmaSpnInfo.stSimSpnInfo, sizeof(TAF_PH_USIM_SPN_STRU));
    }

    if (TAF_PH_SPN_TYPE_UNKNOW == stAtSPNRslt.SpnType)
    {
        /*  SPN file not exist */
        ulResult = AT_CME_SPN_FILE_NOT_EXISTS;
    }
    else
    {
        /* display format: ^SPN:disp_rplmn,coding,spn_name */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr+usLength,
                                            "%s:",
                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName);


        if ( SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "%d,%d,",
                                               stAtSPNRslt.DispRplmnMode & 0x03,
                                               stAtSPNRslt.SpnCoding );
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "%d,%d,",
                                               stAtSPNRslt.DispRplmnMode & 0x01,
                                               stAtSPNRslt.SpnCoding );
        }

        if ( stAtSPNRslt.SpnCoding == TAF_PH_GSM_7BIT_DEFAULT )
        {
            for ( usDatalen = 0 ; usDatalen < TAF_PH_SPN_NAME_MAXLEN ;  usDatalen++ )
            {
                 if ( 0xFF == stAtSPNRslt.aucSpnName[usDatalen] )
                 {
                    break;
                 }
            }
            PS_MEM_CPY(pgucAtSndCodeAddr+usLength,stAtSPNRslt.aucSpnName,usDatalen);
            usLength = usLength + usDatalen;
        }
        else
        {
            usDatalen = TAF_PH_SPN_NAME_MAXLEN;

            usLength += (VOS_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                            (VOS_INT8 *)pgucAtSndCodeAddr,
                                                            (VOS_UINT8 *)pgucAtSndCodeAddr + usLength,
                                                            stAtSPNRslt.aucSpnName,
                                                            (VOS_UINT16)usDatalen);
        }
    }


    gstAtSendData.usBufLen = usLength;

    /* 回复用户命令结果 */
    At_FormatResultData(ucIndex,ulResult);

    return;
}
VOS_VOID AT_PrcoPsEvtErrCode(
    VOS_UINT8                           ucIndex,
    TAF_PS_CAUSE_ENUM_UINT32            enCuase
)
{
    VOS_UINT32                          ulResult;

    /* 转换错误码格式 */
    if ( TAF_PS_CAUSE_SUCCESS != enCuase )
    {
        ulResult    = AT_ERROR;
    }
    else
    {
        ulResult    = AT_OK;
    }

    /* 清除AT操作符, 并停止定时器 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulResult);
}


VOS_VOID AT_LogPrintMsgProc(TAF_MNTN_LOG_PRINT_STRU *pstMsg)
{
#if (VOS_OS_VER == VOS_LINUX)
    printk(KERN_ERR "[MDOEM:%d]%s", VOS_GetModemIDFromPid(pstMsg->ulSenderPid), pstMsg->acLog);
#endif
    return;
}


VOS_VOID AT_RcvTafPsEvt(
    TAF_PS_EVT_STRU                     *pstEvt
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    MN_PS_EVT_FUNC                      pEvtFunc;
    TAF_CTRL_STRU                      *pstCtrl;

    /* 初始化 */
    ulResult    = VOS_OK;
    pEvtFunc    = VOS_NULL_PTR;
    pstCtrl     = (TAF_CTRL_STRU*)(pstEvt->aucContent);

    if ( AT_FAILURE == At_ClientIdToUserId(pstCtrl->usClientId,
                                           &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvTafPsEvt: At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        /* 广播IDNEX不可以作为数组下标使用，需要在事件处理函数中仔细核对，避免数组越界。
           目前只有流量上报/NW ACT/NW DISCONNET为广播事件，需要添加其它广播事件，请仔细核对， */
        if (( ID_EVT_TAF_PS_REPORT_DSFLOW_IND != pstEvt->ulEvtId )
         && ( ID_EVT_TAF_PS_APDSFLOW_REPORT_IND != pstEvt->ulEvtId )
         && ( ID_EVT_TAF_PS_CALL_PDP_DISCONNECT_IND != pstEvt->ulEvtId )
         && ( ID_EVT_TAF_PS_CALL_PDP_MANAGE_IND != pstEvt->ulEvtId ))
        {
            AT_WARN_LOG("AT_RcvTafPsEvt: AT_BROADCAST_INDEX.");
            return;
        }
    }

    /* 在事件处理表中查找处理函数 */
    for ( i = 0; i < AT_ARRAY_SIZE(g_astAtPsEvtFuncTbl); i++ )
    {
        if ( pstEvt->ulEvtId == g_astAtPsEvtFuncTbl[i].ulEvtId )
        {
            /* 事件ID匹配 */
            pEvtFunc = g_astAtPsEvtFuncTbl[i].pEvtFunc;
            break;
        }
    }

    /* 如果处理函数存在则调用 */
    if ( VOS_NULL_PTR != pEvtFunc )
    {
        ulResult = pEvtFunc(ucIndex, pstEvt->aucContent);
    }
    else
    {
        AT_ERR_LOG1("AT_RcvTafPsEvt: Unexpected event received! <EvtId>",
            pstEvt->ulEvtId);
        ulResult    = VOS_ERR;
    }

    /* 根据处理函数的返回结果, 决定是否清除AT定时器以及操作符: 该阶段不考虑 */
    if ( VOS_OK != ulResult )
    {
        AT_ERR_LOG1("AT_RcvTafPsEvt: Can not handle this message! <MsgId>",
            pstEvt->ulEvtId);
    }

    return;
}


VOS_UINT32 AT_RcvTafPsCallEvtPdpActivateCnf_App(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pstEvtInfo
)
{
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent;
    VOS_UINT8                          *pucSystemAppConfig;

    /* 初始化 */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstEvent  = (TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU*)pstEvtInfo;

    if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
    {
        /* 非手机形态 */
        AT_AppPsRspEvtPdpActCnfProc(ucIndex, pstEvent);
    }
    else
    {
        /* 手机形态 */
        if (AT_CMD_CGACT_ORG_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            /* AT+CGACT拨号 */
            AT_ProcAppPsRspEvtPdpActCnf(ucIndex, pstEvent);
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex, AT_OK);
        }
        else
        {
            /* AT^NDISDUP拨号 */
            AT_PS_ProcCallConnectedEvent(pstEvent);
        }
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpActivateCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult;
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent;

    /* 初始化 */
    ulResult  = AT_FAILURE;
    pstEvent  = (TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU*)pEvtInfo;

    /* 记录<CID> */
    gastAtClientTab[ucIndex].ucCid      = pstEvent->ucCid;

    /* 记录<RabId> */
    gastAtClientTab[ucIndex].ucPsRabId  = pstEvent->ucRabId;

    /* 通知ADS承载激活 */
    /* AT_NotifyAdsWhenPdpAvtivated(pstEvent); */

    /* 清除PS域呼叫错误码 */
    AT_PS_SetPsCallErrCause(ucIndex, TAF_PS_CAUSE_SUCCESS);

    AT_PS_AddIpAddrRabIdMap(ucIndex, pstEvent);

    switch ( gastAtClientTab[ucIndex].UserType )
    {
        /* NDIS拨号处理 */
        case AT_NDIS_USER:
            AT_NdisPsRspPdpActEvtCnfProc(ucIndex, pstEvent);
            return VOS_OK;


        /* E5或闪电卡拨号处理 */
        case AT_APP_USER:
            return AT_RcvTafPsCallEvtPdpActivateCnf_App(ucIndex, pEvtInfo);

        /* 处理AP_MODEM形态通过HISC通道下发的拨号 */
        case AT_HSIC1_USER:
        case AT_HSIC2_USER:
        case AT_HSIC3_USER:
        case AT_HSIC4_USER:
            if (AT_CMD_CGACT_ORG_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
            {
                AT_HsicPsRspEvtPdpActCnfProc(pstEvent);
                AT_STOP_TIMER_CMD_READY(ucIndex);
                At_FormatResultData(ucIndex, AT_OK);
            }
            else
            {
                AT_PS_ProcCallConnectedEvent(pstEvent);
            }

            return VOS_OK;

        case AT_USBCOM_USER:
            AT_CtrlConnIndProc(pstEvent, AT_USBCOM_USER);
            break;

        default:
            break;
    }

    /* 根据操作类型 */
    switch(gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        case AT_CMD_CGACT_ORG_SET:
            ulResult    = AT_OK;
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex,ulResult);
            break;

        case AT_CMD_CGANS_ANS_SET:
            ulResult    = AT_OK;
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex,ulResult);
            break;

        case AT_CMD_CGDATA_SET:
            ulResult    = AT_CONNECT;
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_SetMode(ucIndex, AT_DATA_MODE, AT_IP_DATA_MODE);
            At_FormatResultData(ucIndex, ulResult);
            break;

        case AT_CMD_CGANS_ANS_EXT_SET:
            AT_AnswerPdpActInd(ucIndex, pstEvent);
            break;

        case AT_CMD_D_IP_CALL_SET:
        case AT_CMD_PPP_ORG_SET:
            /* Modem发起的PDP激活成功，
               AT_CMD_D_IP_CALL_SET为PPP类型
               AT_CMD_PPP_ORG_SET为IP类型
            */
            AT_ModemPsRspPdpActEvtCnfProc(ucIndex, pstEvent);
            break;

        default:
            break;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsCallEvtPdpActivateRej(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_PDP_ACTIVATE_REJ_STRU  *pstEvent;
    VOS_UINT8                          *pucSystemAppConfig;

    /* 初始化 */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstEvent  = (TAF_PS_CALL_PDP_ACTIVATE_REJ_STRU*)pEvtInfo;

    gastAtClientTab[ucIndex].ulCause = pstEvent->enCause;


    /* 记录PS域呼叫错误码 */
    AT_PS_SetPsCallErrCause(ucIndex, pstEvent->enCause);

    /* 按用户类型分别处理 */
    switch (gastAtClientTab[ucIndex].UserType)
    {
        /* MODEM拨号处理 */
        case AT_HSUART_USER:
        case AT_MODEM_USER:
            AT_ModemPsRspPdpActEvtRejProc(ucIndex, pstEvent);
            break;

        /* NDIS拨号处理 */
        case AT_NDIS_USER:
            AT_NdisPsRspPdpActEvtRejProc(ucIndex, pstEvent);
            break;

        /* E5和闪电卡使用同一个端口名 */
        case AT_APP_USER:
            if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
            {
                /* 非手机形态 */
                AT_AppPsRspEvtPdpActRejProc(ucIndex, pstEvent);
            }
            else
            {
                if (AT_CMD_CGACT_ORG_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
                {
                    /* AT+CGACT拨号 */
                    AT_STOP_TIMER_CMD_READY(ucIndex);
                    At_FormatResultData(ucIndex, AT_ERROR);
                }
                else
                {
                    /* AT^NDISDUP拨号 */
                    AT_PS_ProcCallRejectEvent(pstEvent);
                }
            }
            break;

        case AT_HSIC1_USER:
        case AT_HSIC2_USER:
        case AT_HSIC3_USER:
        case AT_HSIC4_USER:
            if (AT_CMD_CGACT_ORG_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
            {
                /* 清除CID与数传通道的映射关系 */
                AT_CleanAtChdataCfg(pstEvent->stCtrl.usClientId, pstEvent->ucCid);
                AT_STOP_TIMER_CMD_READY(ucIndex);
                At_FormatResultData(ucIndex, AT_ERROR);
            }
            else
            {
                AT_PS_ProcCallRejectEvent(pstEvent);
            }
            break;

        /* 其他端口全部返回ERROR */
        case AT_USBCOM_USER:
        default:
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex, AT_ERROR);
            break;
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpManageInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                          usLength;
    TAF_PS_CALL_PDP_MANAGE_IND_STRU    *pstEvent;
    AT_MODEM_SS_CTX_STRU               *pstModemSsCtx = VOS_NULL_PTR;

    VOS_UINT8                           aucTempValue[TAF_MAX_APN_LEN + 1];

    /* 初始化 */
    usLength  = 0;
    pstEvent  = (TAF_PS_CALL_PDP_MANAGE_IND_STRU*)pEvtInfo;

    /* 命令与协议不符 */
    pstModemSsCtx   = AT_GetModemSsCtxAddrFromClientId(ucIndex);

    if(AT_CRC_ENABLE_TYPE == pstModemSsCtx->ucCrcType)
    {
        /* +CRC -- +CRING: GPRS <PDP_type>, <PDP_addr>[,[<L2P>][,<APN>]] */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%s+CRING: GPRS ",gaucAtCrLf);

        /* <PDP_type> */
        if (TAF_PDP_IPV4 == pstEvent->stPdpAddr.enPdpType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s",gastAtStringTab[AT_STRING_IP].pucText);
        }
#if (FEATURE_ON == FEATURE_IPV6)
        else if (TAF_PDP_IPV6 == pstEvent->stPdpAddr.enPdpType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s", gastAtStringTab[AT_STRING_IPV6].pucText);
        }
        else if (TAF_PDP_IPV4V6 == pstEvent->stPdpAddr.enPdpType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s", gastAtStringTab[AT_STRING_IPV4V6].pucText);
        }
#endif
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s",gastAtStringTab[AT_STRING_PPP].pucText);
        }

        /* <PDP_addr> */
        PS_MEM_SET(aucTempValue, 0x00, (TAF_MAX_APN_LEN + 1));
        AT_Ipv4Addr2Str((VOS_CHAR *)aucTempValue, pstEvent->stPdpAddr.aucIpv4Addr);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           ",\"%s\"",aucTempValue);

        /* <L2P>没有，<APN> */
        PS_MEM_SET(aucTempValue, 0x00, (TAF_MAX_APN_LEN + 1));
        PS_MEM_CPY(aucTempValue, pstEvent->stApn.aucValue, pstEvent->stApn.ucLength);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           ",,\"%s\"%s",aucTempValue,gaucAtCrLf);
    }
    else
    {
        /* +CRC -- RING */
        if(AT_V_ENTIRE_TYPE == gucAtVType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%sRING%s",gaucAtCrLf,gaucAtCrLf);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "2\r");
        }
    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpActivateInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    /* 不处理 */
    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpModifyCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_PDP_MODIFY_CNF_STRU    *pstEvent;
    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx = VOS_NULL_PTR;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGCMOD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    pstEvent = (TAF_PS_CALL_PDP_MODIFY_CNF_STRU*)pEvtInfo;

    /* 通知ADS承载修改 */
    /*AT_NotifyAdsWhenPdpModify(pstEvent);*/

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(ucIndex);

    switch ( gastAtClientTab[ucIndex].UserType )
    {
        case AT_HSUART_USER:
        case AT_MODEM_USER:
            /* 向FC指示修改流控点 */
            AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_MODEM);
            break;

        case AT_NDIS_USER:
            /* 向FC指示修改流控点 */
            AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_NIC_1);
            break;

        case AT_APP_USER:
            AT_PS_ProcCallModifyEvent(ucIndex, pstEvent);
            break;

        case AT_HSIC1_USER:
        case AT_HSIC2_USER:
        case AT_HSIC3_USER:
        case AT_HSIC4_USER:
            switch ( pstPsModemCtx->astChannelCfg[pstEvent->ucCid].ulRmNetId)
            {
                case UDI_ACM_HSIC_ACM1_ID:
                case UDI_NCM_HSIC_NCM0_ID:
                    /* 向FC指示修改流控点 */
                    AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_DIPC_1);
                    break;
                case UDI_ACM_HSIC_ACM3_ID:
                case UDI_NCM_HSIC_NCM1_ID:
                    /* 向FC指示修改流控点 */
                    AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_DIPC_2);
                    break;
                case UDI_ACM_HSIC_ACM5_ID:
                case UDI_NCM_HSIC_NCM2_ID:
                    /* 向FC指示修改流控点 */
                    AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_DIPC_3);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpModifyRej(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGCMOD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_ERROR);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpModifiedInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_PDP_MODIFY_IND_STRU    *pstEvent;
    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx = VOS_NULL_PTR;

    pstEvent = (TAF_PS_CALL_PDP_MODIFY_IND_STRU*)pEvtInfo;

    /* 通知ADS承载修改 */
    /*AT_NotifyAdsWhenPdpModify(pstEvent);*/

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(ucIndex);

    switch ( gastAtClientTab[ucIndex].UserType )
    {
        case AT_HSUART_USER:
        case AT_MODEM_USER:
            /* 向FC指示修改流控点 */
            AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_MODEM);
            break;

        case AT_NDIS_USER:
            /* 向FC指示修改流控点 */
            AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_NIC_1);
            break;

        case AT_APP_USER:
            AT_PS_ProcCallModifyEvent(ucIndex, pstEvent);
            break;

        case AT_HSIC1_USER:
        case AT_HSIC2_USER:
        case AT_HSIC3_USER:
        case AT_HSIC4_USER:
            switch ( pstPsModemCtx->astChannelCfg[pstEvent->ucCid].ulRmNetId)
            {
                case UDI_ACM_HSIC_ACM1_ID:
                case UDI_NCM_HSIC_NCM0_ID:
                    /* 向FC指示修改流控点 */
                    AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_DIPC_1);
                    break;
                case UDI_ACM_HSIC_ACM3_ID:
                case UDI_NCM_HSIC_NCM1_ID:
                    /* 向FC指示修改流控点 */
                    AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_DIPC_2);
                    break;
                case UDI_ACM_HSIC_ACM5_ID:
                case UDI_NCM_HSIC_NCM2_ID:
                    /* 向FC指示修改流控点 */
                    AT_NotifyFcWhenPdpModify(pstEvent, FC_ID_DIPC_3);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtPdpDeactivateCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU *pstEvent;
    VOS_UINT8                           *pucSystemAppConfig;

    /* 初始化 */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstEvent  = (TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU*)pEvtInfo;

    gastAtClientTab[ucIndex].ulCause    = pstEvent->enCause;

    /* 通知ADS承载去激活 */
    /*AT_NotifyAdsWhenPdpDeactivated(pstEvent);*/

    AT_PS_DeleteIpAddrRabIdMap(ucIndex, pstEvent);

    /* NDIS拨号处理 */
    if (AT_NDIS_USER == gastAtClientTab[ucIndex].UserType)
    {
        AT_NdisPsRspPdpDeactEvtCnfProc(ucIndex, pstEvent);
        return VOS_OK;
    }

    /* E5或闪电卡拨号处理 */
    if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            /* 非手机形态 */
            AT_AppPsRspEvtPdpDeactCnfProc(ucIndex, pstEvent);
        }
        else
        {
            /* 手机形态 */
            if (AT_CMD_CGACT_END_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
            {
                /* AT+CGACT拨号 */
                AT_ProcAppPsRspEvtPdpDeActCnf(ucIndex, pstEvent);
                AT_STOP_TIMER_CMD_READY(ucIndex);
                At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);
                At_FormatResultData(ucIndex, AT_OK);
            }
            else
            {
                /* AT^NDISDUP拨号 */
                AT_PS_ProcCallEndedEvent(pstEvent);
            }
        }

        return VOS_OK;
    }

    if (VOS_TRUE == AT_CheckHsicUser(ucIndex))
    {
        if (AT_CMD_CGACT_END_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_HsicPsRspEvtPdpDeactCnfProc(pstEvent);
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);
            At_FormatResultData(ucIndex, AT_OK);
        }
        else
        {
            AT_PS_ProcCallEndedEvent(pstEvent);
        }

        return VOS_OK;
    }

    /* 还是应该先判断是否是数传状态，再决定处理 */
    switch(gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        case AT_CMD_CGACT_END_SET:

            AT_STOP_TIMER_CMD_READY(ucIndex);

            /* 返回命令模式 */
            At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);

            At_FormatResultData(ucIndex,AT_OK);
            break;

        case AT_CMD_H_PS_SET:
        case AT_CMD_PS_DATA_CALL_END_SET:
                AT_ModemPsRspPdpDeactEvtCnfProc(ucIndex, pstEvent);
            break;

        default:
            break;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsCallEvtPdpDeactivatedInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_PDP_DEACTIVATE_IND_STRU *pstEvent;
    VOS_UINT8                           *pucSystemAppConfig;
    VOS_UINT8                           ucCallId;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstEvent    = (TAF_PS_CALL_PDP_DEACTIVATE_IND_STRU*)pEvtInfo;

    gastAtClientTab[ucIndex].ulCause = pstEvent->enCause;

    /* 通知ADS承载去激活 */
    /*AT_NotifyAdsWhenPdpDeactivated(pstEvent);*/

    /* 记录PS域呼叫错误码 */
    AT_PS_SetPsCallErrCause(ucIndex, pstEvent->enCause);

    AT_PS_DeleteIpAddrRabIdMap(ucIndex, pstEvent);

    switch (gastAtClientTab[ucIndex].UserType)
    {
        case AT_HSUART_USER:
        case AT_USBCOM_USER:
        case AT_MODEM_USER:
            AT_ModemPsRspPdpDeactivatedEvtProc(ucIndex, pstEvent);
            break;

        case AT_NDIS_USER:

            /* 增加拨号状态保护, 防止端口挂死 */
            if (AT_CMD_NDISCONN_SET == AT_NDIS_GET_CURR_CMD_OPT())
            {
                /* 清除命令处理状态 */
                AT_STOP_TIMER_CMD_READY(AT_NDIS_GET_USR_PORT_INDEX());

                /* 上报断开拨号结果 */
                At_FormatResultData(AT_NDIS_GET_USR_PORT_INDEX(), AT_OK);
            }

            AT_NdisPsRspPdpDeactivatedEvtProc(ucIndex, pstEvent);
            break;

        case AT_APP_USER:

            /* 增加拨号状态保护, 防止端口挂死 */
            if (AT_CMD_NDISCONN_SET == AT_APP_GET_CURR_CMD_OPT())
            {
                /* 清除命令处理状态 */
                AT_STOP_TIMER_CMD_READY(AT_APP_GET_USR_PORT_INDEX());

                /* 上报断开拨号结果 */
                At_FormatResultData(AT_APP_GET_USR_PORT_INDEX(), AT_OK);
            }

            if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
            {
                AT_AppPsRspEvtPdpDeactivatedProc(ucIndex, pstEvent);
            }
            else
            {
                ucCallId = AT_PS_TransCidToCallId(ucIndex, pstEvent->ucCid);
                if (!AT_PS_IsCallIdValid(ucIndex, ucCallId))
                {
                    /* AT+CGACT拨号，网侧发起PDP去激活 */
                    AT_ProcAppPsRspEvtPdpDeactivated(ucIndex, pstEvent);
                }
                else
                {
                    /* AT^NDISDUP拨号，网侧发起PDP去激活 */
                    AT_PS_ProcCallEndedEvent(pstEvent);
                }
            }
            break;

        case AT_HSIC1_USER:
        case AT_HSIC2_USER:
        case AT_HSIC3_USER:
        case AT_HSIC4_USER:
            /* AT+CGACT 拨号调用AT_HsicPsRspEvtPdpDeactivatedProc
               AT^NDISDUP 拨号调用 AT_PS_ProcCallEndedEvent*/
            ucCallId = AT_PS_TransCidToCallId(ucIndex, pstEvent->ucCid);
            if (!AT_PS_IsCallIdValid(ucIndex, ucCallId))
            {
                AT_HsicPsRspEvtPdpDeactivatedProc(ucIndex, pstEvent);
            }
            else
            {
                AT_PS_ProcCallEndedEvent(pstEvent);
            }

            break;

        default:
            break;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsCallEvtCallOrigCnf_Ndis(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_ORIG_CNF_STRU          *pstCallOrigCnf
)
{
    VOS_UINT32                          ulResult;

    ulResult = AT_FAILURE;

    if (TAF_PS_CAUSE_SUCCESS != pstCallOrigCnf->enCause)
    {
        /* 清除该PDP类型的状态 */
        AT_NdisSetState(g_enAtNdisActPdpType, AT_PDP_STATE_IDLE);

        /* 清除NDIS拨号参数 */
        PS_MEM_SET(&gstAtNdisAddParam, 0x00, sizeof(AT_DIAL_PARAM_STRU));

        if (TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT == pstCallOrigCnf->enCause)
        {
            ulResult = AT_CME_PDP_ACT_LIMIT;
        }
        else
        {
            ulResult = AT_ERROR;
        }

    }
    else
    {
        ulResult = AT_OK;
    }

    /* 清除命令处理状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtCallOrigCnf_App(
    TAF_PS_CALL_ORIG_CNF_STRU          *pstCallOrigCnf
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                          *pucSystemAppConfig;

    /* 初始化 */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
    {
        if (AT_CMD_NDISCONN_SET == AT_APP_GET_CURR_CMD_OPT())
        {
            ulResult = AT_FAILURE;

            if (TAF_PS_CAUSE_SUCCESS != pstCallOrigCnf->enCause)
            {
                /* 清除该PDP类型的状态 */
                AT_AppSetPdpState(AT_APP_GetActPdpType(), AT_PDP_STATE_IDLE);

                /* 清除APP拨号参数 */
                PS_MEM_SET(AT_APP_GetDailParaAddr(), 0x00, sizeof(AT_DIAL_PARAM_STRU));

                if (TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT == pstCallOrigCnf->enCause)
                {
                    ulResult = AT_CME_PDP_ACT_LIMIT;
                }
                else
                {
                    ulResult = AT_ERROR;
                }
            }
            else
            {
                ulResult = AT_OK;
            }

            /* 清除命令处理状态 */
            AT_STOP_TIMER_CMD_READY(AT_APP_GET_USR_PORT_INDEX());

            At_FormatResultData(AT_APP_GET_USR_PORT_INDEX(), ulResult);
        }

        return VOS_OK;
    }
    else
    {
        AT_PS_ProcCallOrigCnfEvent(pstCallOrigCnf);
        return VOS_OK;
    }
}



VOS_UINT32 AT_RcvTafPsCallEvtCallOrigCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_ORIG_CNF_STRU          *pstCallOrigCnf;

    pstCallOrigCnf  = (TAF_PS_CALL_ORIG_CNF_STRU*)pEvtInfo;

    /* 记录PS域呼叫错误码 */
    if (TAF_PS_CAUSE_SUCCESS != pstCallOrigCnf->enCause)
    {
        AT_PS_SetPsCallErrCause(ucIndex, pstCallOrigCnf->enCause);
    }

    if ( (VOS_TRUE == AT_CheckNdisUser(ucIndex))
      && (AT_CMD_NDISCONN_SET == AT_NDIS_GET_CURR_CMD_OPT()) )
    {
        return AT_RcvTafPsCallEvtCallOrigCnf_Ndis(AT_NDIS_GET_USR_PORT_INDEX(), pstCallOrigCnf);
    }

    if (VOS_TRUE == AT_CheckAppUser(ucIndex))
    {
        return AT_RcvTafPsCallEvtCallOrigCnf_App(pstCallOrigCnf);
    }

    if (VOS_TRUE == AT_CheckHsicUser(ucIndex))
    {
        AT_PS_ProcCallOrigCnfEvent(pstCallOrigCnf);
        return VOS_OK;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsCallEvtCallEndCnf_App(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pstEvtInfo
)
{
    VOS_UINT32                          ulResult;
    TAF_PS_CALL_END_CNF_STRU           *pstCallEndCnf;
    VOS_UINT8                          *pucSystemAppConfig;

    /* 初始化 */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    ulResult        = AT_FAILURE;
    pstCallEndCnf   = (TAF_PS_CALL_END_CNF_STRU*)pstEvtInfo;

    if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
    {
        if (AT_CMD_NDISCONN_SET == AT_APP_GET_CURR_CMD_OPT())
        {
            if ( TAF_ERR_NO_ERROR != pstCallEndCnf->enCause)
            {
                ulResult = AT_ERROR;
            }
            else
            {
                ulResult = AT_OK;
            }

            /* 清除命令处理状态 */
            AT_STOP_TIMER_CMD_READY(AT_APP_GET_USR_PORT_INDEX());

            /* 上报断开拨号结果 */
            At_FormatResultData(AT_APP_GET_USR_PORT_INDEX(), ulResult);
        }

        return VOS_OK;
    }
    else
    {
        AT_PS_ProcCallEndCnfEvent(pstCallEndCnf);
        return VOS_OK;
    }
}
VOS_UINT32 AT_RcvTafPsCallEvtCallEndCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult;
    TAF_PS_CALL_END_CNF_STRU           *pstCallEndCnf;

    /* 初始化 */
    ulResult        = AT_FAILURE;
    pstCallEndCnf   = (TAF_PS_CALL_END_CNF_STRU*)pEvtInfo;

    if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
      || (AT_HSUART_USER == gastAtClientTab[ucIndex].UserType) )
    {
        AT_MODEM_ProcCallEndCnfEvent(ucIndex, pstCallEndCnf);
        return VOS_OK;
    }

    if ( (VOS_TRUE == AT_CheckNdisUser(ucIndex))
      && (AT_CMD_NDISCONN_SET == AT_NDIS_GET_CURR_CMD_OPT()) )
    {
        if ( TAF_ERR_NO_ERROR != pstCallEndCnf->enCause)
        {
            ulResult = AT_ERROR;
        }
        else
        {
            ulResult = AT_OK;
        }

        /* 清除命令处理状态 */
        AT_STOP_TIMER_CMD_READY(AT_NDIS_GET_USR_PORT_INDEX());

        /* 上报断开拨号结果 */
        At_FormatResultData(AT_NDIS_GET_USR_PORT_INDEX(), ulResult);

        return VOS_OK;
    }

    if (VOS_TRUE == AT_CheckAppUser(ucIndex))
    {
        return AT_RcvTafPsCallEvtCallEndCnf_App(ucIndex, pEvtInfo);
    }

    if (VOS_TRUE == AT_CheckHsicUser(ucIndex))
    {
        AT_PS_ProcCallEndCnfEvent(pstCallEndCnf);
        return VOS_OK;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsCallEvtCallModifyCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_MODIFY_CNF_STRU        *pstCallModifyCnf;

    /* 初始化 */
    pstCallModifyCnf = (TAF_PS_CALL_MODIFY_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGCMOD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /*----------------------------------------------------------
       (1)协议栈异常错误, 未发起PDP修改, 直接上报ERROR
       (2)协议栈正常, 发起PDP修改, 根据PDP修改事件返回结果
    ----------------------------------------------------------*/
    if ( TAF_PS_CAUSE_SUCCESS != pstCallModifyCnf->enCause )
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsCallEvtCallAnswerCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CALL_ANSWER_CNF_STRU        *pstCallAnswerCnf;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstCallAnswerCnf = (TAF_PS_CALL_ANSWER_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ((AT_CMD_CGANS_ANS_SET     != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CGANS_ANS_EXT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        return VOS_ERR;
    }

    /*----------------------------------------------------------
       (1)协议栈异常错误, 未发起PDP应答, 直接上报ERROR
       (2)协议栈正常, 发起PDP应答, 根据PDP激活事件返回结果
    ----------------------------------------------------------*/

    /* IP类型的应答，需要先给上层回CONNECT */
    if (TAF_ERR_AT_CONNECT == pstCallAnswerCnf->enCause)
    {
        ulResult = At_SetDialGprsPara(ucIndex,
                                      pstCallAnswerCnf->ucCid,
                                      TAF_IP_ACTIVE_TE_PPP_MT_PPP_TYPE);

        /* 如果是connect，CmdCurrentOpt不清，At_RcvTeConfigInfoReq中使用 */
        if (AT_ERROR == ulResult)
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
        }

        AT_StopRelTimer(ucIndex, &gastAtClientTab[ucIndex].hTimer);
        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;
        gastAtClientTab[ucIndex].opId = 0;
        At_FormatResultData(ucIndex, ulResult);

        return VOS_OK;
    }

    /* 其他错误，命令返回ERROR */
    if (TAF_ERR_NO_ERROR != pstCallAnswerCnf->enCause)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsCallEvtCallHangupCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult;
    TAF_PS_CALL_HANGUP_CNF_STRU        *pstCallHangUpCnf;

    pstCallHangUpCnf  = (TAF_PS_CALL_HANGUP_CNF_STRU*)pEvtInfo;

    if (TAF_ERR_NO_ERROR == pstCallHangUpCnf->enCause)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 根据操作类型 */
    switch(gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        case AT_CMD_CGANS_ANS_SET:
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex,ulResult);
            break;

        default:
            break;
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetPrimPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_CNF_STRU  *pstSetPdpCtxInfoCnf;

    pstSetPdpCtxInfoCnf = (TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGDCONT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetPdpCtxInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetPrimPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;
    TAF_PRI_PDP_QUERY_INFO_STRU         stCgdcont;
    TAF_PS_GET_PRIM_PDP_CONTEXT_INFO_CNF_STRU *pstGetPrimPdpCtxInfoCnf;

    VOS_UINT8                           aucStr[TAF_MAX_APN_LEN + 1];

    pstGetPrimPdpCtxInfoCnf = (TAF_PS_GET_PRIM_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGDCONT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstGetPrimPdpCtxInfoCnf->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stCgdcont, &pstGetPrimPdpCtxInfoCnf->astPdpContextQueryInfo[ucTmp], sizeof(TAF_PRI_PDP_QUERY_INFO_STRU));

        /* +CGDCONT:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgdcont.ucCid);
        /* <PDP_type> */
        if (TAF_PDP_IPV4 == stCgdcont.stPriPdpInfo.stPdpAddr.enPdpType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_IP].pucText);
        }
#if (FEATURE_ON == FEATURE_IPV6)
        else if (TAF_PDP_IPV6 == stCgdcont.stPriPdpInfo.stPdpAddr.enPdpType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", gastAtStringTab[AT_STRING_IPV6].pucText);
        }
        else if (TAF_PDP_IPV4V6 == stCgdcont.stPriPdpInfo.stPdpAddr.enPdpType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", gastAtStringTab[AT_STRING_IPV4V6].pucText);
        }
#endif
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_PPP].pucText);
        }
        /* <APN> */
        PS_MEM_SET(aucStr, 0x00, sizeof(aucStr));
        PS_MEM_CPY(aucStr, stCgdcont.stPriPdpInfo.stApn.aucValue, stCgdcont.stPriPdpInfo.stApn.ucLength);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"", aucStr);
        /* <PDP_addr> */
        PS_MEM_SET(aucStr, 0x00, sizeof(aucStr));
        AT_Ipv4Addr2Str((VOS_CHAR *)aucStr, stCgdcont.stPriPdpInfo.stPdpAddr.aucIpv4Addr);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"", aucStr);
        /* <d_comp> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.stPriPdpInfo.enPdpDcomp);
        /* <h_comp> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.stPriPdpInfo.enPdpHcomp);


        /* <IPv4AddrAlloc>  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.stPriPdpInfo.enIpv4AddrAlloc);
        /* <Emergency Indication> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.stPriPdpInfo.enEmergencyInd);
        /* <P-CSCF_discovery> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.stPriPdpInfo.enPcscfDiscovery);
        /* <IM_CN_Signalling_Flag_Ind> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.stPriPdpInfo.enImCnSignalFlg);

    }

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength ;
    VOS_UINT8                               ucTmp ;
    TAF_PS_GET_PDP_CONTEXT_INFO_CNF_STRU   *pstGetPdpCtxInfoCnf;

    ulResult            = AT_FAILURE;
    usLength            = 0;
    ucTmp               = 0;
    pstGetPdpCtxInfoCnf = (TAF_PS_GET_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGPADDR_TEST != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* +CGPADDR:  */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"(");

    for(ucTmp = 0; ucTmp < pstGetPdpCtxInfoCnf->ulCidNum; ucTmp++)
    {
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",pstGetPdpCtxInfoCnf->ulCid[ucTmp]);

        if ((ucTmp + 1) >= pstGetPdpCtxInfoCnf->ulCidNum)
        {
            break;
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
    }
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,")");

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetSecPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_SEC_PDP_CONTEXT_INFO_CNF_STRU   *pstSetPdpCtxInfoCnf;

    pstSetPdpCtxInfoCnf = (TAF_PS_SET_SEC_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGDSCONT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetPdpCtxInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetSecPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;
    TAF_PDP_SEC_CONTEXT_STRU            stSecPdpInfo;
    TAF_PS_GET_SEC_PDP_CONTEXT_INFO_CNF_STRU *pstGetSecPdpCtxInfoCnf;

    pstGetSecPdpCtxInfoCnf = (TAF_PS_GET_SEC_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGDSCONT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for (ucTmp = 0; ucTmp < pstGetSecPdpCtxInfoCnf->ulCidNum; ucTmp++)
    {
        if (0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stSecPdpInfo,&pstGetSecPdpCtxInfoCnf->astPdpContextQueryInfo[ucTmp], sizeof(TAF_PDP_SEC_CONTEXT_STRU));
        /* +CGDSCONT:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stSecPdpInfo.ucCid);
        /* <p_cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stSecPdpInfo.ucLinkdCid);
        /* <d_comp> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stSecPdpInfo.enPdpDcomp);
        /* <h_comp> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stSecPdpInfo.enPdpHcomp);
    }

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvTafPsEvtSetTftInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_TFT_INFO_CNF_STRU       *pstSetTftInfoCnf;

    pstSetTftInfoCnf = (TAF_PS_SET_TFT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGTFT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetTftInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetTftInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp1 = 0;
    VOS_UINT8                           ucTmp2 = 0;
    VOS_CHAR                            acIpv4StrTmp[TAF_MAX_IPV4_ADDR_STR_LEN];
    VOS_UINT8                           aucIpv6StrTmp[TAF_MAX_IPV6_ADDR_DOT_STR_LEN];
    TAF_TFT_QUREY_INFO_STRU            *pstCgtft;
    TAF_PS_GET_TFT_INFO_CNF_STRU       *pstGetTftInfoCnf;

    pstGetTftInfoCnf = (TAF_PS_GET_TFT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGTFT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 动态申请内存 */
    pstCgtft = (TAF_TFT_QUREY_INFO_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT,
                                                       sizeof(TAF_TFT_QUREY_INFO_STRU));
    if (VOS_NULL_PTR == pstCgtft)
    {
        return VOS_ERR;
    }

    for (ucTmp1 = 0; ucTmp1 < pstGetTftInfoCnf->ulCidNum; ucTmp1++)
    {

        PS_MEM_CPY(pstCgtft, &pstGetTftInfoCnf->astTftQueryInfo[ucTmp1], sizeof(TAF_TFT_QUREY_INFO_STRU));

        for (ucTmp2= 0; ucTmp2 < pstCgtft->ucPfNum; ucTmp2++)
        {
            if (!(0 == ucTmp1 && 0 == ucTmp2))
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%s",gaucAtCrLf);
            }
            /* +CGTFT:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ", g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            /* <cid> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%d", pstCgtft->ucCid);
            /* <packet filter identifier> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].ucPacketFilterId);
            /* <evaluation precedence index> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].ucPrecedence);
            /* <source address and subnet mask> */
            if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpRmtIpv4AddrAndMask)
            {
                AT_Ipv4AddrItoa(acIpv4StrTmp, pstCgtft->astPfInfo[ucTmp2].aucRmtIpv4Address);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s", acIpv4StrTmp);
                AT_Ipv4AddrItoa(acIpv4StrTmp, pstCgtft->astPfInfo[ucTmp2].aucRmtIpv4Mask);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%s\"", acIpv4StrTmp);
            }
            else if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpRmtIpv6AddrAndMask)
            {
                AT_Ipv6AddrToStr(aucIpv6StrTmp, pstCgtft->astPfInfo[ucTmp2].aucRmtIpv6Address, AT_IPV6_STR_TYPE_DEC);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s", aucIpv6StrTmp);
                AT_Ipv6AddrToStr(aucIpv6StrTmp, pstCgtft->astPfInfo[ucTmp2].aucRmtIpv6Mask, AT_IPV6_STR_TYPE_DEC);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%s\"", aucIpv6StrTmp);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <protocol number (ipv4) / next header (ipv6)> */
            if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpProtocolId)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].ucProtocolId);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <destination port range> */
            if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpSingleLocalPort)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].usLcPortLowLimit);
            }
            else if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpLocalPortRange)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%d", pstCgtft->astPfInfo[ucTmp2].usLcPortLowLimit);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%d\"", pstCgtft->astPfInfo[ucTmp2].usLcPortHighLimit);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <source port range> */
            if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpSingleRemotePort)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].usRmtPortLowLimit);
            }
            else if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpRemotePortRange)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%d", pstCgtft->astPfInfo[ucTmp2].usRmtPortLowLimit);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%d\"", pstCgtft->astPfInfo[ucTmp2].usRmtPortHighLimit);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <ipsec security parameter index (spi)> */
            if (1 == pstCgtft->astPfInfo[ucTmp2].bitOpSecuParaIndex)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%X\"", pstCgtft->astPfInfo[ucTmp2].ulSecuParaIndex);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask> */
            if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpTypeOfService)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%d", pstCgtft->astPfInfo[ucTmp2].ucTypeOfService);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%d\"", pstCgtft->astPfInfo[ucTmp2].ucTypeOfServiceMask);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <flow label (ipv6)> */
            if (VOS_TRUE == pstCgtft->astPfInfo[ucTmp2].bitOpFlowLabelType)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%X", pstCgtft->astPfInfo[ucTmp2].ulFlowLabelType);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",");
            }
            /* <direction> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].enDirection);
            /* <NW packet filter Identifier> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%d", pstCgtft->astPfInfo[ucTmp2].ucNwPacketFilterId);

        }
    }

    /* 释放动态申请的内存 */
    PS_MEM_FREE(WUEPS_PID_AT, pstCgtft);

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    /* 处理错误码 */
    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetUmtsQosInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_UMTS_QOS_INFO_CNF_STRU  *pstSetUmtsQosInfoCnf;

    pstSetUmtsQosInfoCnf = (TAF_PS_SET_UMTS_QOS_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQREQ_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetUmtsQosInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetUmtsQosInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    TAF_UINT8                           ucTmp = 0;
    TAF_PS_GET_UMTS_QOS_INFO_CNF_STRU  *pstUmtsQosInfo;
    TAF_UMTS_QOS_QUERY_INFO_STRU        stCgeq;

    pstUmtsQosInfo = (TAF_PS_GET_UMTS_QOS_INFO_CNF_STRU *)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQREQ_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstUmtsQosInfo->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stCgeq, &pstUmtsQosInfo->astUmtsQosQueryInfo[ucTmp], sizeof(TAF_UMTS_QOS_QUERY_INFO_STRU));
        /* +CGEQREQ:+CGEQMIN   */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgeq.ucCid);
        /* <Traffic class> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucTrafficClass);
        /* <Maximum bitrate UL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulMaxBitUl);
        /* <Maximum bitrate DL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulMaxBitDl);
        /* <Guaranteed bitrate UL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulGuarantBitUl);
        /* <Guaranteed bitrate DL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulGuarantBitDl);
        /* <Delivery order> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucDeliverOrder);
        /* <Maximum SDU size> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.usMaxSduSize);
        /* <SDU error ratio> */
        switch(stCgeq.stQosInfo.ucSduErrRatio)
        {
        case 0:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_0E0].pucText);
            break;

        case 1:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E2].pucText);
            break;

        case 2:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_7E3].pucText);
            break;

        case 3:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E3].pucText);
            break;

        case 4:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E4].pucText);
            break;

        case 5:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E5].pucText);
            break;

        case 6:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E6].pucText);
            break;

        case 7:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E1].pucText);
            break;

        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            break;
        }
        /* <Residual bit error ratio> */
        switch(stCgeq.stQosInfo.ucResidualBer)
        {
        case 0:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_0E0].pucText);
            break;

        case 1:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_5E2].pucText);
            break;

        case 2:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E2].pucText);
            break;

        case 3:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_5E3].pucText);
            break;

        case 4:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_4E3].pucText);
            break;

        case 5:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E3].pucText);
            break;

        case 6:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E4].pucText);
            break;

        case 7:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E5].pucText);
            break;

        case 8:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E6].pucText);
            break;

        case 9:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_6E8].pucText);
            break;

        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            break;
        }
        /* <Delivery of erroneous SDUs> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucDeliverErrSdu);
        /* <Transfer delay> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.usTransDelay);
        /* <Traffic handling priority> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucTraffHandlePrior);
    }

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetUmtsQosMinInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_UMTS_QOS_MIN_INFO_CNF_STRU  *pstSetUmtsQosMinInfoCnf;

    pstSetUmtsQosMinInfoCnf = (TAF_PS_SET_UMTS_QOS_MIN_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQMIN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetUmtsQosMinInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetUmtsQosMinInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_UINT32                              ulResult = AT_FAILURE;
    TAF_UINT16                              usLength = 0;
    TAF_UINT8                               ucTmp = 0;
    TAF_PS_GET_UMTS_QOS_MIN_INFO_CNF_STRU  *pstUmtsQosMinInfo;
    TAF_UMTS_QOS_QUERY_INFO_STRU            stCgeq;

    pstUmtsQosMinInfo = (TAF_PS_GET_UMTS_QOS_MIN_INFO_CNF_STRU *)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQMIN_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstUmtsQosMinInfo->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stCgeq, &pstUmtsQosMinInfo->astUmtsQosQueryInfo[ucTmp], sizeof(TAF_UMTS_QOS_QUERY_INFO_STRU));
        /* +CGEQREQ:+CGEQMIN   */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",stCgeq.ucCid);
        /* <Traffic class> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucTrafficClass);
        /* <Maximum bitrate UL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulMaxBitUl);
        /* <Maximum bitrate DL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulMaxBitDl);
        /* <Guaranteed bitrate UL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulGuarantBitUl);
        /* <Guaranteed bitrate DL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulGuarantBitDl);
        /* <Delivery order> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucDeliverOrder);
        /* <Maximum SDU size> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.usMaxSduSize);
        /* <SDU error ratio> */
        switch(stCgeq.stQosInfo.ucSduErrRatio)
        {
        case 0:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_0E0].pucText);
            break;

        case 1:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E2].pucText);
            break;

        case 2:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_7E3].pucText);
            break;

        case 3:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E3].pucText);
            break;

        case 4:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E4].pucText);
            break;

        case 5:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E5].pucText);
            break;

        case 6:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E6].pucText);
            break;

        case 7:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E1].pucText);
            break;

        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            break;
        }
        /* <Residual bit error ratio> */
        switch(stCgeq.stQosInfo.ucResidualBer)
        {
        case 0:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_0E0].pucText);
            break;

        case 1:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_5E2].pucText);
            break;

        case 2:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E2].pucText);
            break;

        case 3:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_5E3].pucText);
            break;

        case 4:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_4E3].pucText);
            break;

        case 5:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E3].pucText);
            break;

        case 6:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E4].pucText);
            break;

        case 7:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E5].pucText);
            break;

        case 8:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E6].pucText);
            break;

        case 9:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_6E8].pucText);
            break;

        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            break;
        }
        /* <Delivery of erroneous SDUs> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucDeliverErrSdu);
        /* <Transfer delay> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.usTransDelay);
        /* <Traffic handling priority> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucTraffHandlePrior);
    }

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtGetDynamicUmtsQosInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                                  ulResult = AT_FAILURE;
    VOS_UINT16                                  usLength = 0;
    VOS_UINT8                                   ucTmp = 0;
    TAF_PS_GET_DYNAMIC_UMTS_QOS_INFO_CNF_STRU  *pstDynUmtsQosMinInfo;
    TAF_UMTS_QOS_QUERY_INFO_STRU                stCgeq;

    pstDynUmtsQosMinInfo = (TAF_PS_GET_DYNAMIC_UMTS_QOS_INFO_CNF_STRU *)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQNEG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstDynUmtsQosMinInfo->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stCgeq, &pstDynUmtsQosMinInfo->astUmtsQosQueryInfo[ucTmp], sizeof(TAF_UMTS_QOS_QUERY_INFO_STRU));
        /* +CGEQREQ:+CGEQMIN   */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgeq.ucCid);
        /* <Traffic class> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucTrafficClass);
        /* <Maximum bitrate UL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulMaxBitUl);
        /* <Maximum bitrate DL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulMaxBitDl);
        /* <Guaranteed bitrate UL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulGuarantBitUl);
        /* <Guaranteed bitrate DL> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ulGuarantBitDl);
        /* <Delivery order> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucDeliverOrder);
        /* <Maximum SDU size> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.usMaxSduSize);
        /* <SDU error ratio> */
        switch(stCgeq.stQosInfo.ucSduErrRatio)
        {
        case 0:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_0E0].pucText);
            break;

        case 1:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E2].pucText);
            break;

        case 2:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_7E3].pucText);
            break;

        case 3:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E3].pucText);
            break;

        case 4:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E4].pucText);
            break;

        case 5:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E5].pucText);
            break;

        case 6:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E6].pucText);
            break;

        case 7:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E1].pucText);
            break;

        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            break;
        }
        /* <Residual bit error ratio> */
        switch(stCgeq.stQosInfo.ucResidualBer)
        {
        case 0:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_0E0].pucText);
            break;

        case 1:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_5E2].pucText);
            break;

        case 2:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E2].pucText);
            break;

        case 3:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_5E3].pucText);
            break;

        case 4:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_4E3].pucText);
            break;

        case 5:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E3].pucText);
            break;

        case 6:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E4].pucText);
            break;

        case 7:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E5].pucText);
            break;

        case 8:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_1E6].pucText);
            break;

        case 9:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%s",gastAtStringTab[AT_STRING_6E8].pucText);
            break;

        default:
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            break;
        }
        /* <Delivery of erroneous SDUs> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucDeliverErrSdu);
        /* <Transfer delay> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.usTransDelay);
        /* <Traffic handling priority> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeq.stQosInfo.ucTraffHandlePrior);
    }

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetPdpStateCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_PDP_STATE_CNF_STRU      *pstSetPdpStateCnf;

    pstSetPdpStateCnf = (TAF_PS_SET_PDP_STATE_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( (AT_CMD_CGACT_ORG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_CGACT_END_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_CGDATA_SET    != gastAtClientTab[ucIndex].CmdCurrentOpt) )
    {
        return VOS_ERR;
    }

    /*----------------------------------------------------------
       (1)协议栈异常错误, 未发起PDP激活, 直接上报ERROR
       (2)协议栈正常, 发起PDP激活, 根据PDP激活事件返回结果
    ----------------------------------------------------------*/

    if (TAF_PS_CAUSE_SUCCESS != pstSetPdpStateCnf->enCause)
    {
        /* 记录PS域呼叫错误码 */
        AT_PS_SetPsCallErrCause(ucIndex, pstSetPdpStateCnf->enCause);

        AT_STOP_TIMER_CMD_READY(ucIndex);

        if (TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT == pstSetPdpStateCnf->enCause)
        {
            At_FormatResultData(ucIndex, AT_CME_PDP_ACT_LIMIT);
        }
        else
        {
            At_FormatResultData(ucIndex, AT_ERROR);
        }
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtCgactQryCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;
    TAF_CID_STATE_STRU                  stCgact;
    TAF_PS_GET_PDP_STATE_CNF_STRU      *pstPdpState;

    pstPdpState = (TAF_PS_GET_PDP_STATE_CNF_STRU *)pEvtInfo;

    /* 检查当前命令的操作类型 */
    for (ucTmp = 0; ucTmp < pstPdpState->ulCidNum; ucTmp++)
    {
        if (0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stCgact, &pstPdpState->astCidStateInfo[ucTmp], sizeof(TAF_CID_STATE_STRU));
        /* +CGACT:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgact.ucCid);
        /* <state> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgact.ucState);
    }

    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtCgeqnegTestCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulQosnegNum;
    VOS_UINT32                          ulTmp;
    TAF_CID_STATE_STRU                  stCgact;
    TAF_PS_GET_PDP_STATE_CNF_STRU      *pstPdpState;

    usLength    = 0;
    ulQosnegNum = 0;
    pstPdpState = (TAF_PS_GET_PDP_STATE_CNF_STRU *)pEvtInfo;

    /* CGEQNEG的测试命令 */

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s", "(");

    for(ulTmp = 0; ulTmp < pstPdpState->ulCidNum; ulTmp++)
    {
        PS_MEM_CPY(&stCgact, &pstPdpState->astCidStateInfo[ulTmp], sizeof(TAF_CID_STATE_STRU));

        if (TAF_PDP_ACTIVE == stCgact.ucState)
        {   /*如果该CID是激活态,则打印该CID和可能的一个逗号;否则跳过该CID*/
            if (0 == ulQosnegNum )
            {   /*如果是第一个CID，则CID前不打印逗号*/
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",stCgact.ucCid);
            }
            else
            {   /*如果不是第一个CID，则CID前打印逗号*/
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stCgact.ucCid);
            }

            ulQosnegNum ++;
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s", ")");

    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}



VOS_UINT32 AT_RcvTafPsEvtGetPdpStateCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{

    /* 检查当前命令的操作类型 */
    if (AT_CMD_CGACT_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return AT_RcvTafPsEvtCgactQryCnf(ucIndex, pEvtInfo);
    }
    else if (AT_CMD_CGEQNEG_TEST == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return AT_RcvTafPsEvtCgeqnegTestCnf(ucIndex, pEvtInfo);

    }
    else
    {
        return VOS_ERR;
    }


}


VOS_UINT32 AT_RcvTafPsEvtGetPdpIpAddrInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                            usLength = 0;
    VOS_UINT8                             ucTmp = 0;
    VOS_CHAR                              aStrTmp[TAF_MAX_IPV4_ADDR_STR_LEN];
    VOS_UINT8                             aucIPv6Str[TAF_MAX_IPV6_ADDR_DOT_STR_LEN];
    TAF_PDP_ADDR_QUERY_INFO_STRU          stPdpAddrQuery;
    TAF_PS_GET_PDP_IP_ADDR_INFO_CNF_STRU *pstPdpIpAddr;

    /* 初始化 */
    pstPdpIpAddr = (TAF_PS_GET_PDP_IP_ADDR_INFO_CNF_STRU *)pEvtInfo;
    PS_MEM_SET(aStrTmp, 0x00, sizeof(aStrTmp));

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGPADDR_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for (ucTmp = 0; ucTmp < pstPdpIpAddr->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%s", gaucAtCrLf);
        }

        PS_MEM_CPY(&stPdpAddrQuery, &pstPdpIpAddr->astPdpAddrQueryInfo[ucTmp], sizeof(TAF_PDP_ADDR_QUERY_INFO_STRU));


        /* +CGPADDR:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%s: ", g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%d", stPdpAddrQuery.ucCid);

        /* <PDP_addr> */
        if ( (TAF_PDP_IPV4 == stPdpAddrQuery.stPdpAddr.enPdpType)
          || (TAF_PDP_PPP == stPdpAddrQuery.stPdpAddr.enPdpType) )
        {
            AT_Ipv4AddrItoa(aStrTmp, stPdpAddrQuery.stPdpAddr.aucIpv4Addr);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"", aStrTmp);
        }
        else if (TAF_PDP_IPV6 == stPdpAddrQuery.stPdpAddr.enPdpType)
        {
            AT_Ipv6AddrToStr(aucIPv6Str, stPdpAddrQuery.stPdpAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"", aucIPv6Str);
        }
        else if (TAF_PDP_IPV4V6 == stPdpAddrQuery.stPdpAddr.enPdpType)
        {
            AT_Ipv4AddrItoa(aStrTmp, stPdpAddrQuery.stPdpAddr.aucIpv4Addr);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"", aStrTmp);

            AT_Ipv6AddrToStr(aucIPv6Str, stPdpAddrQuery.stPdpAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"", aucIPv6Str);
        }
        else
        {
            /* TAF_PDP_TYPE_BUTT */
            return VOS_ERR;
        }
    }

    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtSetAnsModeInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_ANSWER_MODE_INFO_CNF_STRU   *pstSetAnsModeInfoCnf;

    pstSetAnsModeInfoCnf = (TAF_PS_SET_ANSWER_MODE_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGAUTO_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetAnsModeInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetAnsModeInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                              usLength;
    TAF_PS_GET_ANSWER_MODE_INFO_CNF_STRU   *pstCallAns;

    /* 初始化 */
    usLength    = 0;
    pstCallAns  = (TAF_PS_GET_ANSWER_MODE_INFO_CNF_STRU *)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGAUTO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* +CGAUTO */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",pstCallAns->ulAnsMode);

    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtGetDynamicPrimPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;
    VOS_CHAR                            acIpv4StrTmp[TAF_MAX_IPV4_ADDR_STR_LEN];
    VOS_UINT8                           aucIpv6StrTmp[TAF_MAX_IPV6_ADDR_DOT_STR_LEN];

    VOS_UINT8                           aucStr[TAF_MAX_APN_LEN + 1];

    TAF_PDP_DYNAMIC_PRIM_EXT_STRU       stCgdcont;
    TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF_STRU  *pstGetDynamicPdpCtxInfoCnf;

    pstGetDynamicPdpCtxInfoCnf = (TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGCONTRDP_SET!= gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    if( VOS_OK == pstGetDynamicPdpCtxInfoCnf->enCause )
    {
        for(ucTmp = 0; ucTmp < pstGetDynamicPdpCtxInfoCnf->ulCidNum; ucTmp++)
        {
            if(0 != ucTmp)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            }

            PS_MEM_CPY(&stCgdcont, &pstGetDynamicPdpCtxInfoCnf->astPdpContxtInfo[ucTmp], sizeof(TAF_PDP_DYNAMIC_PRIM_EXT_STRU));

            /* +CGCONTRDP:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            /* <p_cid> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgdcont.ucPrimayCid);
            /* <bearer_id> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.ucBearerId);
            /* <APN> */
            if(1 == stCgdcont.bitOpApn)
            {
                PS_MEM_SET(aucStr, 0x00, sizeof(aucStr));
                PS_MEM_CPY(aucStr, stCgdcont.aucApn, TAF_MAX_APN_LEN);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",stCgdcont.aucApn);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            /* <ip_addr> */
            if((VOS_TRUE == stCgdcont.bitOpIpAddr) && (VOS_TRUE == stCgdcont.bitOpSubMask))
            {
                if (TAF_PDP_IPV4 == stCgdcont.stPdpAddr.enPdpType)
                {
                    AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stPdpAddr.aucIpv4Addr);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s",acIpv4StrTmp);

                    AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stSubnetMask.aucIpv4Addr);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,".%s\"",acIpv4StrTmp);
                }
                else if(TAF_PDP_IPV6 == stCgdcont.stPdpAddr.enPdpType)
                {
                    AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stPdpAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s", aucIpv6StrTmp);
                    AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stSubnetMask.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%s\"", aucIpv6StrTmp);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            /* <gw_addr> */
            if(VOS_TRUE == stCgdcont.bitOpGwAddr)
            {
                if ( (TAF_PDP_IPV4 == stCgdcont.stGWAddr.enPdpType)
                  || (TAF_PDP_PPP == stCgdcont.stGWAddr.enPdpType) )
                {
                AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stGWAddr.aucIpv4Addr);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acIpv4StrTmp);
                }
                else if(TAF_PDP_IPV6 == stCgdcont.stGWAddr.enPdpType)
                {
                AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stGWAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s\"", aucIpv6StrTmp);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            /* <NDS_prim_addr> */
            if(VOS_TRUE == stCgdcont.bitOpDNSPrimAddr)
            {
                if ( (TAF_PDP_IPV4 == stCgdcont.stDNSPrimAddr.enPdpType)
                  || (TAF_PDP_PPP == stCgdcont.stDNSPrimAddr.enPdpType) )
                {
                    AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stDNSPrimAddr.aucIpv4Addr);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acIpv4StrTmp);
                }
                else if(TAF_PDP_IPV6 == stCgdcont.stDNSPrimAddr.enPdpType)
                {
                    AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stDNSPrimAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s\"", aucIpv6StrTmp);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            /* <DNS_sec_addr> */
            if(VOS_TRUE == stCgdcont.bitOpDNSSecAddr)
            {
                if ( (TAF_PDP_IPV4 == stCgdcont.stDNSSecAddr.enPdpType)
                  || (TAF_PDP_PPP == stCgdcont.stDNSSecAddr.enPdpType) )
                {
                    AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stDNSSecAddr.aucIpv4Addr);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acIpv4StrTmp);
                }
                else if(TAF_PDP_IPV6 == stCgdcont.stDNSSecAddr.enPdpType)
                {
                    AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stDNSSecAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s\"", aucIpv6StrTmp);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            /* <P-CSCF_prim_addr> */
            if(VOS_TRUE == stCgdcont.bitOpPCSCFPrimAddr)
            {
                if ( (TAF_PDP_IPV4 == stCgdcont.stPCSCFPrimAddr.enPdpType)
                  || (TAF_PDP_PPP == stCgdcont.stPCSCFPrimAddr.enPdpType) )
                {
                    AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stPCSCFPrimAddr.aucIpv4Addr);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acIpv4StrTmp);
                }
                else if(TAF_PDP_IPV6 == stCgdcont.stPCSCFPrimAddr.enPdpType)
                {
                    AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stPCSCFPrimAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s\"", aucIpv6StrTmp);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            /* <P-CSCF_sec_addr> */
            if(VOS_TRUE == stCgdcont.bitOpPCSCFSecAddr)
            {
                if ( (TAF_PDP_IPV4 == stCgdcont.stPCSCFSecAddr.enPdpType)
                  || (TAF_PDP_PPP == stCgdcont.stPCSCFSecAddr.enPdpType))
                {
                    AT_Ipv4AddrItoa(acIpv4StrTmp, stCgdcont.stPCSCFSecAddr.aucIpv4Addr);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acIpv4StrTmp);
                }
                else if(TAF_PDP_IPV6 == stCgdcont.stPCSCFSecAddr.enPdpType)
                {
                    AT_Ipv6AddrToStr(aucIpv6StrTmp, stCgdcont.stPCSCFSecAddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s\"", aucIpv6StrTmp);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }

            if (VOS_TRUE == stCgdcont.bitOpImCnSignalFlg)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdcont.enImCnSignalFlg);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }
        }
        ulResult                = AT_OK;
        gstAtSendData.usBufLen  = usLength;

    }
    else
    {
        ulResult                = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtGetDynamicSecPdpContextInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;

    TAF_PDP_DYNAMIC_SEC_EXT_STRU       stCgdscont;
    TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF_STRU  *pstGetDynamicPdpCtxInfoCnf;

    pstGetDynamicPdpCtxInfoCnf = (TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGSCONTRDP_SET!= gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    if( VOS_OK == pstGetDynamicPdpCtxInfoCnf->enCause )
    {
        for(ucTmp = 0; ucTmp < pstGetDynamicPdpCtxInfoCnf->ulCidNum; ucTmp++)
        {
            if(0 != ucTmp)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            }

            PS_MEM_CPY(&stCgdscont, &pstGetDynamicPdpCtxInfoCnf->astPdpContxtInfo[ucTmp], sizeof(TAF_PDP_DYNAMIC_SEC_EXT_STRU));

            /* +CGSCONTRDP:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            /* <cid> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgdscont.ucCid);
            /* <p_cid> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdscont.ucPrimaryCid);
            /* <bearer_id> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgdscont.ucBearerId);
        }

        ulResult                = AT_OK;
        gstAtSendData.usBufLen  = usLength;
    }
    else
    {
        ulResult                = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtGetDynamicTftInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucIndex1 = 0;
    VOS_UINT8                           ucIndex2 = 0;
    VOS_CHAR                            acIpv4StrTmp[TAF_MAX_IPV4_ADDR_STR_LEN];
    VOS_UINT8                           aucIpv6StrTmp[TAF_MAX_IPV6_ADDR_DOT_STR_LEN];

    TAF_PF_TFT_STRU                       *pstCgtft = NULL;
    TAF_PS_GET_DYNAMIC_TFT_INFO_CNF_STRU  *pstGetDynamicTftInfoCnf;

    pstGetDynamicTftInfoCnf = (TAF_PS_GET_DYNAMIC_TFT_INFO_CNF_STRU*)pEvtInfo;

    pstCgtft = (TAF_PF_TFT_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_PF_TFT_STRU));
    if (VOS_NULL_PTR == pstCgtft)
    {
        return VOS_ERR;
    }
    PS_MEM_SET(pstCgtft, 0x00, sizeof(TAF_PF_TFT_STRU));

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGTFTRDP_SET!= gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        PS_MEM_FREE(WUEPS_PID_AT, pstCgtft);
        return VOS_ERR;
    }

    if ( VOS_OK == pstGetDynamicTftInfoCnf->enCause)
    {
        for (ucIndex1 = 0; ucIndex1 < pstGetDynamicTftInfoCnf->ulCidNum; ucIndex1++)
        {
            for (ucIndex2 = 0; ucIndex2 < pstGetDynamicTftInfoCnf->astPfTftInfo[ucIndex1].ulPFNum; ucIndex2++)
            {
                if (!(0 == ucIndex1 && 0 == ucIndex2))
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
                }

                PS_MEM_CPY(pstCgtft, &pstGetDynamicTftInfoCnf->astPfTftInfo[ucIndex1], sizeof(TAF_PF_TFT_STRU));

                /* +CGTFTRDP:  */
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                /* <cid> */
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",pstCgtft->ulCid);
                /* <packet filter identifier> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpPktFilterId)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",pstCgtft->astTftInfo[ucIndex2].ucPacketFilterId);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }

                /* <evaluation precedence index> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpPrecedence)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",pstCgtft->astTftInfo[ucIndex2].ucPrecedence);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }

                /* <source address and subnet> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpSrcIp)
                {
                    if ( (TAF_PDP_IPV4 == pstCgtft->astTftInfo[ucIndex2].stSourceIpaddr.enPdpType)
                      || (TAF_PDP_PPP == pstCgtft->astTftInfo[ucIndex2].stSourceIpaddr.enPdpType) )
                    {
                        AT_Ipv4AddrItoa(acIpv4StrTmp, pstCgtft->astTftInfo[ucIndex2].stSourceIpaddr.aucIpv4Addr);
                        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s",acIpv4StrTmp);

                        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,".");

                        AT_Ipv4AddrItoa(acIpv4StrTmp, pstCgtft->astTftInfo[ucIndex2].stSourceIpMask.aucIpv4Addr);
                        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s\"",acIpv4StrTmp);
                    }
                    else if(TAF_PDP_IPV6 == pstCgtft->astTftInfo[ucIndex2].stSourceIpaddr.enPdpType)
                    {
                        AT_Ipv6AddrToStr(aucIpv6StrTmp, pstCgtft->astTftInfo[ucIndex2].stSourceIpaddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",\"%s", aucIpv6StrTmp);
                        AT_Ipv6AddrToStr(aucIpv6StrTmp, pstCgtft->astTftInfo[ucIndex2].stSourceIpaddr.aucIpv6Addr, AT_IPV6_STR_TYPE_DEC);
                        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ".%s\"", aucIpv6StrTmp);
                    }
                    else
                    {
                        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                    }
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }

                /* <protocal number(ipv4)/next header ipv6> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpProtocolId)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",pstCgtft->astTftInfo[ucIndex2].ucProtocolId);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
                /* <destination port range> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpDestPortRange)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%d.%d\"",pstCgtft->astTftInfo[ucIndex2].usLowDestPort,pstCgtft->astTftInfo[ucIndex2].usHighDestPort);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
                /* <source port range> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpSrcPortRange)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%d.%d\"",pstCgtft->astTftInfo[ucIndex2].usLowSourcePort,pstCgtft->astTftInfo[ucIndex2].usHighSourcePort);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }

                /* <ipsec security parameter index(spi)> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpSpi)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%X",pstCgtft->astTftInfo[ucIndex2].ulSecuParaIndex);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
                /* <type os service(tos) (ipv4) and mask> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpTosMask)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%d.%d\"",pstCgtft->astTftInfo[ucIndex2].ucTypeOfService,pstCgtft->astTftInfo[ucIndex2].ucTypeOfServiceMask);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
                /* <traffic class (ipv6) and mask> */

                /* <flow lable (ipv6)> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpFlowLable)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%X",pstCgtft->astTftInfo[ucIndex2].ulFlowLable);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }
                /* <direction> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpDirection)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",pstCgtft->astTftInfo[ucIndex2].ucDirection);
                }
                else
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
                }

                /* <NW packet filter Identifier> */
                if(1 == pstCgtft->astTftInfo[ucIndex2].bitOpNwPktFilterId)
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",pstCgtft->astTftInfo[ucIndex2].ucNwPktFilterId);
                }
            }

            /* <3,0,0,"192.168.0.2.255.255.255.0">,0,"0.65535","0.65535",0,"0.0",0,0 */
        }

        ulResult                = AT_OK;
        gstAtSendData.usBufLen  = usLength;
    }
    else
    {
        ulResult                = AT_ERROR;
       gstAtSendData.usBufLen   = 0;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    PS_MEM_FREE(WUEPS_PID_AT, pstCgtft);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtSetEpsQosInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{

    TAF_PS_SET_EPS_QOS_INFO_CNF_STRU  *pstSetEpsqosInfoCnf;

    pstSetEpsqosInfoCnf = (TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQOS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetEpsqosInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetEpsQosInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;

    TAF_EPS_QOS_EXT_STRU                       stCgeqos;
    TAF_PS_GET_EPS_QOS_INFO_CNF_STRU          *pstGetEpsQosInfoCnf;

    pstGetEpsQosInfoCnf = (TAF_PS_GET_EPS_QOS_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQOS_READ!= gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstGetEpsQosInfoCnf->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stCgeqos, &pstGetEpsQosInfoCnf->astEpsQosInfo[ucTmp], sizeof(TAF_EPS_QOS_EXT_STRU));

        /* +CGEQOS:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgeqos.ucCid);
        /* <QCI> */
        if(1 == stCgeqos.bitOpQCI)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ucQCI);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
        }
        /* <DL GBR> */
        if(1 == stCgeqos.bitOpDLGBR)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulDLGBR);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
        }
        /* <UL GBR> */
        if(1 == stCgeqos.bitOpULGBR)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulULGBR);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
        }
        /* <DL MBR> */
        if(1 == stCgeqos.bitOpDLMBR)
        {
           usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulDLMBR);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
        }
        /* <UL MBR> */
        if(1 == stCgeqos.bitOpULMBR)
        {
           usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulULMBR);
        }
    }

    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetDynamicEpsQosInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;

    TAF_EPS_QOS_EXT_STRU                       stCgeqos;
    TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF_STRU  *pstGetDynamicEpsQosInfoCnf;

    pstGetDynamicEpsQosInfoCnf = (TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_CGEQOSRDP_SET!= gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    if(VOS_OK == pstGetDynamicEpsQosInfoCnf->enCause)
    {
        for(ucTmp = 0; ucTmp < pstGetDynamicEpsQosInfoCnf->ulCidNum; ucTmp++)
        {
            if(0 != ucTmp)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            }

            PS_MEM_CPY(&stCgeqos, &pstGetDynamicEpsQosInfoCnf->astEpsQosInfo[ucTmp], sizeof(TAF_EPS_QOS_EXT_STRU));

            /* +CGEQOSRDP:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            /* <cid> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stCgeqos.ucCid);
            /* <QCI> */
            if(1 == stCgeqos.bitOpQCI)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ucQCI);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }
            /* <DL GBR> */
            if(1 == stCgeqos.bitOpDLGBR)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulDLGBR);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }
            /* <UL GBR> */
            if(1 == stCgeqos.bitOpULGBR)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulULGBR);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }
            /* <DL MBR> */
            if(1 == stCgeqos.bitOpDLMBR)
            {
               usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulDLMBR);
            }
            else
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
            }
            /* <UL MBR> */
            if(1 == stCgeqos.bitOpULMBR)
            {
               usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stCgeqos.ulULMBR);
            }
        }

        ulResult                = AT_OK;
        gstAtSendData.usBufLen  = usLength;
    }
    else
    {
        ulResult                = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetDsFlowInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                              usLength;
    TAF_DSFLOW_QUERY_INFO_STRU             *pstAccumulatedFlowInfo;
    TAF_PS_GET_DSFLOW_INFO_CNF_STRU        *pstGetDsFlowInfoCnf;

    /* 初始化 */
    usLength               = 0;
    pstGetDsFlowInfoCnf    = (TAF_PS_GET_DSFLOW_INFO_CNF_STRU*)pEvtInfo;
    pstAccumulatedFlowInfo = &pstGetDsFlowInfoCnf->stQueryInfo;



    /* 检查当前AT操作类型 */
    if ( AT_CMD_DSFLOWQRY_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 上报流量查询结果 */

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s:",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%08X",
                                       pstAccumulatedFlowInfo->stCurrentFlowInfo.ulDSLinkTime);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%08X%08X",
                                       pstAccumulatedFlowInfo->stCurrentFlowInfo.ulDSSendFluxHigh,
                                       pstAccumulatedFlowInfo->stCurrentFlowInfo.ulDSSendFluxLow);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%08X%08X",
                                       pstAccumulatedFlowInfo->stCurrentFlowInfo.ulDSReceiveFluxHigh,
                                       pstAccumulatedFlowInfo->stCurrentFlowInfo.ulDSReceiveFluxLow);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%08X",
                                       pstAccumulatedFlowInfo->stTotalFlowInfo.ulDSLinkTime);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%08X%08X",
                                       pstAccumulatedFlowInfo->stTotalFlowInfo.ulDSSendFluxHigh,
                                       pstAccumulatedFlowInfo->stTotalFlowInfo.ulDSSendFluxLow);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%08X%08X",
                                       pstAccumulatedFlowInfo->stTotalFlowInfo.ulDSReceiveFluxHigh,
                                       pstAccumulatedFlowInfo->stTotalFlowInfo.ulDSReceiveFluxLow);


    gstAtSendData.usBufLen = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtClearDsFlowInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CLEAR_DSFLOW_CNF_STRU       *pstClearDsFlowCnf;

    pstClearDsFlowCnf = (TAF_PS_CLEAR_DSFLOW_CNF_STRU*)pEvtInfo;


    /* 检查当前AT操作类型 */
    if (AT_CMD_DSFLOWCLR_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }


    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstClearDsFlowCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtConfigDsFlowRptCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_CONFIG_DSFLOW_RPT_CNF_STRU  *pstConfigDsFlowRptCnf;

    pstConfigDsFlowRptCnf = (TAF_PS_CONFIG_DSFLOW_RPT_CNF_STRU*)pEvtInfo;


    /* 检查当前AT操作类型 */
    if (AT_CMD_DSFLOWRPT_SET   != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }


    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstConfigDsFlowRptCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtReportDsFlowInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT16                          usLength;
    TAF_PS_REPORT_DSFLOW_IND_STRU      *pstDSFlowReport;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    /* 初始化 */
    usLength        = 0;
    pstDSFlowReport = (TAF_PS_REPORT_DSFLOW_IND_STRU*)pEvtInfo;
    enModemId       = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvTafPsEvtReportDsFlowInd: Get modem id fail.");
        return VOS_ERR;
    }


    /* 检查流量上报控制标记和私有命令主动上报控制标记 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                        (VOS_CHAR*)pgucAtSndCodeAddr,
                        (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                        "%s^DSFLOWRPT:%08X,%08X,%08X,%08X%08X,%08X%08X,%08X,%08X%s",
                        gaucAtCrLf,
                        pstDSFlowReport->stDsFlowRptInfo.stCurrentFlowInfo.ulDSLinkTime,
                        pstDSFlowReport->stDsFlowRptInfo.ulCurrentSendRate,
                        pstDSFlowReport->stDsFlowRptInfo.ulCurrentReceiveRate,
                        pstDSFlowReport->stDsFlowRptInfo.stCurrentFlowInfo.ulDSSendFluxHigh,
                        pstDSFlowReport->stDsFlowRptInfo.stCurrentFlowInfo.ulDSSendFluxLow,
                        pstDSFlowReport->stDsFlowRptInfo.stCurrentFlowInfo.ulDSReceiveFluxHigh,
                        pstDSFlowReport->stDsFlowRptInfo.stCurrentFlowInfo.ulDSReceiveFluxLow,
                        pstDSFlowReport->stDsFlowRptInfo.ulQosSendRate,
                        pstDSFlowReport->stDsFlowRptInfo.ulQosReceiveRate,
                        gaucAtCrLf);

    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);


    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtSetApDsFlowRptCfgCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_APDSFLOW_RPT_CFG_CNF_STRU   *pstSetRptCfgCnf;

    pstSetRptCfgCnf = (TAF_PS_SET_APDSFLOW_RPT_CFG_CNF_STRU *)pEvtInfo;

    /* 检查当前AT操作类型 */
    if (AT_CMD_APDSFLOWRPTCFG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetRptCfgCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetApDsFlowRptCfgCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_GET_APDSFLOW_RPT_CFG_CNF_STRU   *pstGetRptCfgCnf;
    VOS_UINT16                              usLength;

    pstGetRptCfgCnf = (TAF_PS_GET_APDSFLOW_RPT_CFG_CNF_STRU *)pEvtInfo;
    usLength         = 0;

    /* 检查当前AT操作类型 */
    if (AT_CMD_APDSFLOWRPTCFG_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 检查错误码 */
    if (TAF_PS_CAUSE_SUCCESS != pstGetRptCfgCnf->enCause)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_ERR;
    }

    /* 上报查询结果 */
    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%s: %d,%u",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      pstGetRptCfgCnf->stRptCfg.ulRptEnabled,
                                      pstGetRptCfgCnf->stRptCfg.ulFluxThreshold);

    gstAtSendData.usBufLen = usLength;
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtApDsFlowReportInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_APDSFLOW_REPORT_IND_STRU    *pstApDsFlowRptInd;
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;

    pstApDsFlowRptInd = (TAF_PS_APDSFLOW_REPORT_IND_STRU *)pEvtInfo;
    usLength          = 0;
    enModemId         = MODEM_ID_0;

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvTafPsEvtApDsFlowReportInd: Get modem id fail.");
        return VOS_ERR;
    }

    /* ^APDSFLOWRPT: <curr_ds_time>,<tx_rate>,<rx_rate>,<curr_tx_flow>,<curr_rx_flow>,<total_tx_flow>,<total_rx_flow> */
    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR*)pgucAtSndCodeAddr,
                                      (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                      "%s^APDSFLOWRPT: %08X,%08X,%08X,%08X%08X,%08X%08X,%08X%08X,%08X%08X%s",
                                      gaucAtCrLf,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stCurrentFlowInfo.ulDSLinkTime,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.ulCurrentTxRate,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.ulCurrentRxRate,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stCurrentFlowInfo.ulDSSendFluxHigh,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stCurrentFlowInfo.ulDSSendFluxLow,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stCurrentFlowInfo.ulDSReceiveFluxHigh,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stCurrentFlowInfo.ulDSReceiveFluxLow,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stTotalFlowInfo.ulDSSendFluxHigh,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stTotalFlowInfo.ulDSSendFluxLow,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stTotalFlowInfo.ulDSReceiveFluxHigh,
                                      pstApDsFlowRptInd->stApDsFlowRptInfo.stTotalFlowInfo.ulDSReceiveFluxLow,
                                      gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetDsFlowNvWriteCfgCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_DSFLOW_NV_WRITE_CFG_CNF_STRU    *pstSetNvWriteCfgCnf;

    pstSetNvWriteCfgCnf = (TAF_PS_SET_DSFLOW_NV_WRITE_CFG_CNF_STRU *)pEvtInfo;

    /* 检查当前AT操作类型 */
    if (AT_CMD_DSFLOWNVWRCFG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetNvWriteCfgCnf->enCause);
    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetDsFlowNvWriteCfgCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_GET_DSFLOW_NV_WRITE_CFG_CNF_STRU    *pstGetNvWriteCfgCnf;
    VOS_UINT16                                  usLength;

    pstGetNvWriteCfgCnf = (TAF_PS_GET_DSFLOW_NV_WRITE_CFG_CNF_STRU *)pEvtInfo;
    usLength            = 0;

    /* 检查当前AT操作类型 */
    if (AT_CMD_DSFLOWNVWRCFG_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 检查错误码 */
    if (TAF_PS_CAUSE_SUCCESS != pstGetNvWriteCfgCnf->enCause)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_ERR;
    }

    /* 上报查询结果 */
    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%s: %d,%d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      pstGetNvWriteCfgCnf->stNvWriteCfg.ucEnabled,
                                      pstGetNvWriteCfgCnf->stNvWriteCfg.ucInterval);

    gstAtSendData.usBufLen = usLength;
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetPdpAuthInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_PDP_AUTH_INFO_CNF_STRU  *pstSetPdpAuthInfoCnf;

    pstSetPdpAuthInfoCnf = (TAF_PS_SET_PDP_AUTH_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前AT操作类型 */
    if ( AT_CMD_CGAUTH_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetPdpAuthInfoCnf->enCause);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetPdpAuthInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_UINT16                          usLength = 0;
    TAF_UINT8                           ucTmp = 0;
    TAF_AUTH_QUERY_INFO_STRU            stAuth;
    TAF_PS_GET_PDP_AUTH_INFO_CNF_STRU  *pstAuthInfo;

    pstAuthInfo = (TAF_PS_GET_PDP_AUTH_INFO_CNF_STRU *)pEvtInfo;

    if ( AT_CMD_CGAUTH_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstAuthInfo->ulCidNum; ucTmp++)
    {
        if (0 != ucTmp)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stAuth, &pstAuthInfo->astPdpAuthQueryInfo[ucTmp], sizeof(TAF_AUTH_QUERY_INFO_STRU));
        /* +CGAUTH:  */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",stAuth.ucCid);
        /* <Auth> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stAuth.stAuthInfo.enAuth);
        /* <UserName> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"",stAuth.stAuthInfo.aucUserName);
        /* <PassWord> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"%s\"",stAuth.stAuthInfo.aucPassword);
    }

    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetPdpDnsInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_PDP_DNS_INFO_CNF_STRU   *pstSetPdpDnsInfoCnf;

    pstSetPdpDnsInfoCnf = (TAF_PS_SET_PDP_DNS_INFO_CNF_STRU*)pEvtInfo;

    if ( AT_CMD_CGDNS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetPdpDnsInfoCnf->enCause);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtGetPdpDnsInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    /* 移植At_QryParaRspCgdnsProc的实现逻辑 */
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;
    TAF_DNS_QUERY_INFO_STRU             stPdpDns;
    TAF_PS_GET_PDP_DNS_INFO_CNF_STRU   *pstPdpDnsInfo;
    VOS_INT8                            acDnsAddr[TAF_MAX_IPV4_ADDR_STR_LEN];

    PS_MEM_SET(acDnsAddr, 0, TAF_MAX_IPV4_ADDR_STR_LEN);
    pstPdpDnsInfo = (TAF_PS_GET_PDP_DNS_INFO_CNF_STRU *)pEvtInfo;

    if ( AT_CMD_CGDNS_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstPdpDnsInfo->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stPdpDns, &pstPdpDnsInfo->astPdpDnsQueryInfo[ucTmp],sizeof(TAF_DNS_QUERY_INFO_STRU));
        /* +CGDNS:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stPdpDns.ucCid);
        /* <PriDns> */
        if(1 == stPdpDns.stDnsInfo.bitOpPrimDnsAddr)
        {
            AT_Ipv4Addr2Str(acDnsAddr, stPdpDns.stDnsInfo.aucPrimDnsAddr);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acDnsAddr);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
        }
        /* <SecDns> */
        if(1 == stPdpDns.stDnsInfo.bitOpSecDnsAddr)
        {
             AT_Ipv4Addr2Str(acDnsAddr, stPdpDns.stDnsInfo.aucSecDnsAddr);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",acDnsAddr);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",");
        }
    }


    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtSetAuthDataInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_AUTHDATA_INFO_CNF_STRU  *pstSetAuthDataInfoCnf;

    /* 初始化 */
    pstSetAuthDataInfoCnf = (TAF_PS_SET_AUTHDATA_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前AT操作类型 */
    if ( AT_CMD_AUTHDATA_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetAuthDataInfoCnf->enCause);

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtGetAuthDataInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucTmp = 0;
    TAF_AUTHDATA_QUERY_INFO_STRU        stPdpAuthData;
    TAF_PS_GET_AUTHDATA_INFO_CNF_STRU  *pstPdpAuthData;

    pstPdpAuthData = (TAF_PS_GET_AUTHDATA_INFO_CNF_STRU *)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_AUTHDATA_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    for(ucTmp = 0; ucTmp < pstPdpAuthData->ulCidNum; ucTmp++)
    {
        if(0 != ucTmp)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        }

        PS_MEM_CPY(&stPdpAuthData,&pstPdpAuthData->astAuthDataQueryInfo[ucTmp], sizeof(TAF_AUTHDATA_QUERY_INFO_STRU));
        /* ^AUTHDATA:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* <cid> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d",stPdpAuthData.ucCid);

        /* <Auth_type> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d",stPdpAuthData.stAuthDataInfo.enAuthType);

        /* <passwd> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",stPdpAuthData.stAuthDataInfo.aucPassword);

        /* <username> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",stPdpAuthData.stAuthDataInfo.aucUsername);

        /* <PLMN> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",\"%s\"",stPdpAuthData.stAuthDataInfo.aucPlmn);
    }


    ulResult                = AT_OK;
    gstAtSendData.usBufLen  = usLength;

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetGprsActiveTypeCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                              ulResult;
    TAF_PS_GET_D_GPRS_ACTIVE_TYPE_CNF_STRU *pstGetGprsActiveTypeCnf;

    /* 初始化 */
    ulResult                = AT_FAILURE;
    pstGetGprsActiveTypeCnf = (TAF_PS_GET_D_GPRS_ACTIVE_TYPE_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_D_GPRS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 转换错误码格式 */
    if ( TAF_PARA_OK == pstGetGprsActiveTypeCnf->enCause )
    {
        ulResult = At_SetDialGprsPara(ucIndex,
                        pstGetGprsActiveTypeCnf->stCidGprsActiveType.ucCid,
                        pstGetGprsActiveTypeCnf->stCidGprsActiveType.enActiveType);
    }
    else
    {
        ulResult = AT_ERROR;
    }

    if ( AT_WAIT_ASYNC_RETURN != ulResult )
    {
        if ( AT_ERROR == ulResult )
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
        }

        AT_StopRelTimer(ucIndex, &gastAtClientTab[ucIndex].hTimer);
        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;
        gastAtClientTab[ucIndex].opId = 0;

        At_FormatResultData(ucIndex, ulResult);
    }

    return VOS_OK;
}
VOS_UINT32 AT_RcvTafPsEvtPppDialOrigCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulErrInfo;
    TAF_PS_PPP_DIAL_ORIG_CNF_STRU      *pstPppDialOrigCnf;

    /* 初始化 */
    ulResult          = AT_FAILURE;
    pstPppDialOrigCnf = (TAF_PS_PPP_DIAL_ORIG_CNF_STRU*)pEvtInfo;

    /* MODEM拨号错误处理 */
    if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
      || (AT_HSUART_USER  == gastAtClientTab[ucIndex].UserType))
    {
        if (TAF_PS_CAUSE_SUCCESS != pstPppDialOrigCnf->enCause)
        {
            /* 记录PS域呼叫错误码 */
            AT_PS_SetPsCallErrCause(ucIndex, pstPppDialOrigCnf->enCause);

            if ((AT_CMD_D_PPP_CALL_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
             || (AT_CMD_PPP_ORG_SET    == gastAtClientTab[ucIndex].CmdCurrentOpt))
            {
                ulResult = AT_NO_CARRIER;

                PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_REL_PPP_REQ);

                /* 向PPP发送HDLC去使能操作 */
                PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_HDLC_DISABLE);

                /* 返回命令模式 */
                At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);

                ulErrInfo = AT_OM_GREENCHANNEL_PS_IP_TYPE_DIAL_FAIL;

                OM_AppGreenChannel(OM_GREEN_FUNC,
                                   AT_OM_GREEN_CHANNEL_PS,
                                   (VOS_UINT8 *)&ulErrInfo,
                                   sizeof(ulErrInfo));
            }
            else if (AT_CMD_D_IP_CALL_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
            {
                if (TAF_PS_CAUSE_PDP_ACTIVATE_LIMIT == pstPppDialOrigCnf->enCause)
                {
                    ulResult = AT_CME_PDP_ACT_LIMIT;
                }
                else
                {
                    ulResult = AT_ERROR;
                }

                PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_REL_PPP_RAW_REQ);

                /*向PPP发送HDLC去使能操作*/
                PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_HDLC_DISABLE);

                ulErrInfo = AT_OM_GREENCHANNEL_PS_PPP_TYPE_DIAL_FAIL;

                OM_AppGreenChannel(OM_GREEN_FUNC,
                                   AT_OM_GREEN_CHANNEL_PS,
                                   (VOS_UINT8 *)&ulErrInfo,
                                   sizeof(ulErrInfo));
            }
            else
            {
                ;
            }

            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex, ulResult);
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafPsEvtGetCidSdfInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    return VOS_OK;
}


VOS_BOOL AT_PH_IsPlmnValid(TAF_PLMN_ID_STRU *pstPlmnId)
{
    VOS_UINT32                          i;

    for (i=0; i<3; i++)
    {
        if ((((pstPlmnId->Mcc >> (i*4)) & 0x0F) > 9)
         || ((((pstPlmnId->Mnc >> (i*4)) & 0x0F) > 9) && (i != 2 ))
         || ((((pstPlmnId->Mnc >> (i*4)) & 0x0F) > 9) && (((pstPlmnId->Mnc >> (i*4)) & 0x0F) != 0x0F)))
        {
            /* PLMN ID无效 */
            return VOS_FALSE;
        }
    }
    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_IPV6)
VOS_UINT32 AT_RcvTafPsEvtReportRaInfo(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_IPV6_INFO_IND_STRU *pstRaInfoNotifyInd;
    AT_PDP_ENTITY_STRU                  *pstPdpEntity;
    MODEM_ID_ENUM_UINT16                 enModemId;
    VOS_UINT32                           ulRet;
    VOS_UINT8                           *pucSystemAppConfig;

    /* 初始化 */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    pstRaInfoNotifyInd = (TAF_PS_IPV6_INFO_IND_STRU *)pEvtInfo;

    enModemId          = MODEM_ID_0;

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("AT_RcvTafPsEvtReportRaInfo:Get Modem Id fail");
        return VOS_ERR;
    }

    if (AT_NDIS_USER == gastAtClientTab[ucIndex].UserType)
    {
        pstPdpEntity = AT_NDIS_GetPdpEntInfoAddr();

        /* 检查IPv6承载激活状态, 激活场景下才配置NDIS */
        if ( (AT_PDP_STATE_ACTED == pstPdpEntity->enIpv6State)
          || (AT_PDP_STATE_ACTED == pstPdpEntity->enIpv4v6State) )
        {
            /* 向NDIS模块发送IPV6 PDN信息 */
            AT_SendNdisIPv6PdnInfoCfgReq(enModemId, pstRaInfoNotifyInd);

            AT_PS_ProcSharePdpIpv6RaInfo(pstRaInfoNotifyInd);
        }
    }

    if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            pstPdpEntity = AT_APP_GetPdpEntInfoAddr();

            /* 检查IPv6承载激活状态, 激活场景下才处理RA报文信息 */
            if ( (AT_PDP_STATE_ACTED == pstPdpEntity->enIpv6State)
              || (AT_PDP_STATE_ACTED == pstPdpEntity->enIpv4v6State) )
            {
                AT_AppProcIpv6RaInfo(pstRaInfoNotifyInd);
            }
        }
        else
        {
            AT_PS_ProcIpv6RaInfo(pstRaInfoNotifyInd);
        }
    }

    if (VOS_TRUE == AT_CheckHsicUser(ucIndex))
    {
        AT_PS_ProcIpv6RaInfo(pstRaInfoNotifyInd);
    }

    return VOS_OK;
}
#endif
VOS_UINT32 AT_RcvTafPsEvtPdpDisconnectInd(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulErrInfo;
    VOS_UINT16                          usLength;

    usLength = 0;

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        /* 未应答的场景直接上报NO CARRIER*/
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                    "%s",gaucAtCrLf);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s","NO CARRIER");

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        return VOS_OK;
    }

    /* IP类型网络激活^CGANS应答过程中上报ID_EVT_TAF_PS_CALL_PDP_DISCONNECT_IND */
    if (AT_CMD_PPP_ORG_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        ulResult = AT_NO_CARRIER;

        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_REL_PPP_REQ);

        /* 向PPP发送HDLC去使能操作 */
        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_HDLC_DISABLE);

        /* 返回命令模式 */
        At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);

        ulErrInfo = AT_OM_GREENCHANNEL_PS_IP_TYPE_DIAL_FAIL;

        OM_AppGreenChannel(OM_GREEN_FUNC,
                          AT_OM_GREEN_CHANNEL_PS,
                          (VOS_UINT8 *)&ulErrInfo,
                          sizeof(ulErrInfo));

    }
    else if ((AT_CMD_CGANS_ANS_EXT_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
          || (AT_CMD_CGANS_ANS_SET == gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        /*
        1.PPP类型网络激活^CGANS应答过程中上报ID_EVT_TAF_PS_CALL_PDP_DISCONNECT_IND
        2.+CGANS应答
        以上两种情况都还没有切数传通道，直接回ERROR
        */
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_VOID AT_SetMemStatusRspProc(
    VOS_UINT8                           ucIndex,
    MN_MSG_EVENT_INFO_STRU             *pstEvent
)
{
    VOS_UINT32                          ulResult;

    /* 检查用户索引值 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_SetMemStatusRspProc: AT_BROADCAST_INDEX.");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_CSASM_SET */
    if ( AT_CMD_CSASM_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( VOS_TRUE == pstEvent->u.stMemStatusInfo.bSuccess)
    {
        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return;
}
VOS_UINT32 AT_RcvTafPsEvtGetDynamicDnsInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    VOS_UINT32                          ulResult    = AT_ERROR;
    VOS_UINT16                          usLength    = 0;
    TAF_PS_GET_NEGOTIATION_DNS_CNF_STRU *pstNegoDnsCnf;
    VOS_INT8                            acDnsAddr[TAF_MAX_IPV4_ADDR_STR_LEN];

    PS_MEM_SET(acDnsAddr, 0, TAF_MAX_IPV4_ADDR_STR_LEN);

    pstNegoDnsCnf = (TAF_PS_GET_NEGOTIATION_DNS_CNF_STRU *)pEvtInfo;

    /* 检查用户索引值 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafPsEvtGetDynamicDnsInfoCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if ( AT_CMD_DNSQUERY_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_PARA_OK != pstNegoDnsCnf->enCause)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* <PriDns> */
        if(VOS_TRUE == pstNegoDnsCnf->stNegotiationDns.stDnsInfo.bitOpPrimDnsAddr)
        {
            AT_Ipv4Addr2Str(acDnsAddr, pstNegoDnsCnf->stNegotiationDns.stDnsInfo.aucPrimDnsAddr);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "\"%s\"",
                                               acDnsAddr);
        }
        /* <SecDns> */
        if(VOS_TRUE == pstNegoDnsCnf->stNegotiationDns.stDnsInfo.bitOpSecDnsAddr)
        {
            AT_Ipv4Addr2Str(acDnsAddr, pstNegoDnsCnf->stNegotiationDns.stDnsInfo.aucSecDnsAddr);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               ",\"%s\"",
                                               acDnsAddr);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               ",");
        }
        ulResult = AT_OK;
    }

    gstAtSendData.usBufLen  = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


#if(FEATURE_ON == FEATURE_LTE)
VOS_UINT32 atReadLtecsCnfProc(VOS_UINT8   ucIndex,VOS_VOID    *pEvtInfo)
{
    TAF_PS_LTECS_CNF_STRU *pLtecsReadCnf = NULL;
    VOS_UINT16 usLength = 0;
    VOS_UINT32 ulResult;

    pLtecsReadCnf = (TAF_PS_LTECS_CNF_STRU *)pEvtInfo;

    if(pLtecsReadCnf->enCause == VOS_OK)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
        "^LTECS:");
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
        "%d,%d",pLtecsReadCnf->stLteCs.ucSG,pLtecsReadCnf->stLteCs.ucIMS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
         ",%d,%d",pLtecsReadCnf->stLteCs.ucCSFB,pLtecsReadCnf->stLteCs.ucVCC);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
        ",%d",pLtecsReadCnf->stLteCs.ucVoLGA);

         ulResult                = AT_OK;
        gstAtSendData.usBufLen  = usLength;
    }
    else
    {
         ulResult                = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }


    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);
    return VOS_OK;
}

VOS_UINT32 atReadCemodeCnfProc(VOS_UINT8   ucIndex,VOS_VOID    *pEvtInfo)
{
    TAF_PS_CEMODE_CNF_STRU *pCemodeReadCnf = NULL;
    VOS_UINT16 usLength = 0;
    VOS_UINT32 ulResult;




    pCemodeReadCnf = (TAF_PS_CEMODE_CNF_STRU *)pEvtInfo;

    if(pCemodeReadCnf->enCause == VOS_OK)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
        "+CEMODE:");
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
        "%d",pCemodeReadCnf->stCemode.enCurrentUeMode);

         ulResult                = AT_OK;
        gstAtSendData.usBufLen  = usLength;
    }
    else
    {
         ulResult                = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }


    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex,ulResult);
    return VOS_OK;
}



VOS_UINT32 AT_RcvTafPsEvtSetPdprofInfoCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pEvtInfo
)
{
    TAF_PS_SET_PDP_PROF_INFO_CNF_STRU     *pstSetPdprofInfoCnf;
    pstSetPdprofInfoCnf = (TAF_PS_SET_PDP_PROF_INFO_CNF_STRU*)pEvtInfo;

    /* 检查当前命令的操作类型 */
    if ( AT_CMD_PDPROFMOD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 处理错误码 */
    AT_PrcoPsEvtErrCode(ucIndex, pstSetPdprofInfoCnf->enCause);

    return VOS_OK;
}
#endif



VOS_VOID AT_ConvertNasMccToBcdType(
    VOS_UINT32                          ulNasMcc,
    VOS_UINT32                         *pulMcc
)
{
    VOS_UINT32                          i;
    VOS_UINT8                           aucTmp[4];

    *pulMcc = 0;

    for (i = 0; i < 3 ; i++ )
    {
        aucTmp[i]   = ulNasMcc & 0x0f;
        ulNasMcc  >>=  8;
    }

    *pulMcc = ((VOS_UINT32)aucTmp[0] << 8)
             |((VOS_UINT32)aucTmp[1] << 4)
             | aucTmp[2];

}


VOS_VOID At_RcvMnCallEccNumIndProc(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    MN_CALL_ECC_NUM_INFO_STRU          *pstEccNumInfo = VOS_NULL_PTR;
    VOS_UINT8                           aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1];
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulMcc;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    PS_MEM_SET(aucAsciiNum, 0x00, sizeof(aucAsciiNum));


    /* 获取上报的紧急呼号码信息 */
    pstEccNumInfo = (MN_CALL_ECC_NUM_INFO_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEccNumInfo->usClientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvMnCallEccNumIndProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_RcvMnCallEccNumIndProc: Get modem id fail.");
        return;
    }


    /* 向APP逐条上报紧急呼号码 */
    for (i = 0; i < pstEccNumInfo->ulEccNumCount; i++)
    {
        /* 将BCD码转化为ASCII码 */
        AT_BcdNumberToAscii(pstEccNumInfo->astCustomEccNumList[i].aucEccNum,
                            pstEccNumInfo->astCustomEccNumList[i].ucEccNumLen,
                            (VOS_CHAR*)aucAsciiNum);

        /* 将NAS格式的MCC转化为BCD格式 */
        AT_ConvertNasMccToBcdType(pstEccNumInfo->astCustomEccNumList[i].ulMcc, &ulMcc);

        usLength = 0;
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%s^XLEMA:",
                                           gaucAtCrLf);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%d,%d,%s,%d,%d,",
                                           (i+1),
                                           pstEccNumInfo->ulEccNumCount,
                                           aucAsciiNum,
                                           pstEccNumInfo->astCustomEccNumList[i].ucCategory,
                                           pstEccNumInfo->astCustomEccNumList[i].ucValidSimPresent);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%x%x%x",
                                           (ulMcc & 0x0f00)>>8,
                                           (ulMcc & 0xf0)>>4,
                                           (ulMcc & 0x0f));

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           ",%d%s",
                                           pstEccNumInfo->astCustomEccNumList[i].ucAbnormalServiceFlg,
                                           gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        PS_MEM_SET(aucAsciiNum, 0x00, sizeof(aucAsciiNum));

    }

    return;
}
VOS_VOID At_RcvMmaPlmnChangeIndProc(
    TAF_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength       = 0;

    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_RcvMmaPlmnChangeIndProc: Get modem id fail.");
        return;
    }


    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s^PLMN:",gaucAtCrLf);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr +
                                       usLength,
                                       "%x%x%x,",
                                       (pEvent->stCurPlmn.Mcc & 0x0f00)>>8,
                                       (pEvent->stCurPlmn.Mcc & 0xf0)>>4,
                                       (pEvent->stCurPlmn.Mcc & 0x0f));

    if (0x0f00 == (pEvent->stCurPlmn.Mnc & 0x0f00))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%x%x",
                                           (pEvent->stCurPlmn.Mnc & 0xf0)>>4,
                                           (pEvent->stCurPlmn.Mnc & 0x0f));
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%x%x%x",
                                           (pEvent->stCurPlmn.Mnc & 0x0f00)>>8,
                                           (pEvent->stCurPlmn.Mnc & 0xf0)>>4,
                                           (pEvent->stCurPlmn.Mnc & 0x0f));
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)ucIndex, pgucAtSndCodeAddr, usLength);
}



VOS_VOID AT_RcvMmaNsmStatusInd(
    TAF_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    VOS_UINT16                          usLength;

    usLength = 0;
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s+PACSP",gaucAtCrLf);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%d",pEvent->ucPlmnMode);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr,usLength);

    return;
}


VOS_VOID AT_RcvMmaRssiChangeInd(
    TAF_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pEvent
)
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulRptCmdRssi;
    VOS_UINT32                          ulRptCmdCerssi;
#if (FEATURE_LTE == FEATURE_ON)
    VOS_UINT32                          ulRptCmdAnlevel;
    VOS_INT16                           sRsrp;
    VOS_INT16                           sRsrq;
    VOS_UINT8                           ucLevel;
    VOS_INT16                           sRssi;
#endif
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength       = 0;

    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvMmaRssiChangeInd: Get modem id fail.");
        return;
    }

    ulRptCmdRssi   = AT_CheckRptCmdStatus(pEvent->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_RSSI);
    ulRptCmdCerssi = AT_CheckRptCmdStatus(pEvent->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CERSSI);
    ulRptCmdAnlevel = AT_CheckRptCmdStatus(pEvent->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_ANLEVEL);

    if ((VOS_TRUE == AT_CheckRptCmdStatus(pEvent->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_RSSI))
     && (VOS_TRUE == ulRptCmdRssi)
     && (VOS_TRUE == pEvent->OP_Rssi))
    {

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_RSSI].pucText,
                                           pEvent->RssiValue.aRssi[0].ucRssiValue,
                                           gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr,usLength);
    }

    usLength = 0;

    if ((VOS_TRUE == AT_CheckRptCmdStatus(pEvent->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_CERSSI))
     && (VOS_TRUE == ulRptCmdCerssi)
     && (VOS_TRUE == pEvent->OP_Rssi))
    {
        if (TAF_PH_RAT_GSM == pEvent->RssiValue.enRatType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s%s%d,%d,%d,%d,%d,%d,%d%s",
                                               gaucAtCrLf,
                                               gastAtStringTab[AT_STRING_CERSSI].pucText,
                                               pEvent->RssiValue.aRssi[0].u.stGCellSignInfo.sRssiValue,
                                               0,
                                               255,
                                               0,
                                               0,
                                               0,
                                               0,
                                               gaucAtCrLf);

            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
            return;
        }

        if (TAF_PH_RAT_WCDMA == pEvent->RssiValue.enRatType)
        {
            if ( TAF_UTRANCTRL_UTRAN_MODE_FDD == pEvent->RssiValue.ucCurrentUtranMode)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s%s%d,%d,%d,%d,%d,%d,%d%s",
                                                   gaucAtCrLf,
                                                   gastAtStringTab[AT_STRING_CERSSI].pucText,
                                                   0,      /* rssi */
                                                   pEvent->RssiValue.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                                   pEvent->RssiValue.aRssi[0].u.stWCellSignInfo.sEcioValue,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   gaucAtCrLf);
            }
            else
            {
                /* 非fdd 3g 小区，ecio值为无效值255 */
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s%s%d,%d,%d,%d,%d,%d,%d%s",
                                                   gaucAtCrLf,
                                                   gastAtStringTab[AT_STRING_CERSSI].pucText,
                                                   0,      /* rssi */
                                                   pEvent->RssiValue.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                                   255,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   gaucAtCrLf);

            }
            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

            return;
        }
        /* 上报LTE 的CERSSI */
#if (FEATURE_LTE == FEATURE_ON)
        if (TAF_PH_RAT_LTE == pEvent->RssiValue.enRatType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s%s0,0,255,%d,%d,%d,%d,%d,%d,%s",
                                               gaucAtCrLf,
                                               gastAtStringTab[AT_STRING_CERSSI].pucText,
                                               pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsrp,
                                               pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsrq,
                                               pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.lSINR,
                                               pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.stCQI.usRI,
                                               pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[0],
                                               pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[1],
                                               gaucAtCrLf);

            At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
        }
#endif
    }

    /*上报ANLEVEL */
#if (FEATURE_LTE == FEATURE_ON)

    usLength = 0;

    if ((VOS_TRUE == AT_CheckRptCmdStatus(pEvent->aucUnsolicitedRptCfg, AT_CMD_RPT_CTRL_BY_UNSOLICITED, AT_RPT_CMD_ANLEVEL))
     && (VOS_TRUE == ulRptCmdAnlevel)
     && (1 == pEvent->OP_Rssi))
    {


        /* Modified by l00305157 for for Service_State_Optimize_PhaseI, 2014-11-29, begin */
        sRsrp = pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsrp;
        sRsrq = pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRsrq;
        sRssi = pEvent->RssiValue.aRssi[0].u.stLCellSignInfo.sRssi;

        AT_CalculateLTESignalValue(&sRssi,&ucLevel,&sRsrp,&sRsrq);
        /* Modified by l00305157 for for Service_State_Optimize_PhaseI, 2014-11-29, end */

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s0,99,%d,%d,%d,%d%s",
                                           gaucAtCrLf,
                                           "^ANLEVEL:",
                                           sRssi,
                                           ucLevel,
                                           sRsrp,
                                           sRsrq,
                                           gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr,usLength);
    }
#endif

    return;
}
VOS_VOID At_StkNumPrint(
    VOS_UINT16                         *pusLength,
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usDataLen,
    VOS_UINT8                           ucNumType
)
{
    VOS_UINT16                          usLength = *pusLength;
    /* 打印数据 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",\"");

    PS_MEM_CPY(pgucAtSndCodeAddr + usLength, pucData, usDataLen);

    usLength += usDataLen;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "\"");

    /* 打印类型 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           ucNumType);

    *pusLength = usLength;

    return;
}


VOS_VOID AT_BcdHalfByteToAscii(
    VOS_UINT8                           ucBcdHalfByte,
    VOS_UINT8                          *pucAsciiNum
)
{
    if(ucBcdHalfByte <= 9)  /*转换数字*/
    {
        *pucAsciiNum = ucBcdHalfByte + 0x30;
    }
    else if(0x0A == ucBcdHalfByte)   /*转换*字符*/
    {
        *pucAsciiNum = 0x2a;
    }
    else if(0x0B == ucBcdHalfByte)   /*转换#字符*/
    {
        *pucAsciiNum = 0x23;
    }
    else if(0x0C == ucBcdHalfByte)   /*转换'P'字符*/
    {
        *pucAsciiNum = 0x50;
    }
    else if(0x0D == ucBcdHalfByte)   /*转换'?'字符*/
    {
        *pucAsciiNum = 0x3F;
    }
    else                                    /*转换字母*/
    {
        *pucAsciiNum = ucBcdHalfByte + 0x57;
    }

    return;
}


VOS_VOID AT_BcdToAscii(
    VOS_UINT8                           ucBcdNumLen,
    VOS_UINT8                          *pucBcdNum,
    VOS_UINT8                          *pucAsciiNum,
    VOS_UINT8                          *pucLen
)
{

    VOS_UINT8       ucTmp;
    VOS_UINT8       ucLen = 0;
    VOS_UINT8       ucFirstNumber;
    VOS_UINT8       ucSecondNumber;

    for (ucTmp = 0; ucTmp < ucBcdNumLen; ucTmp++)
    {
        if(0xFF == pucBcdNum[ucTmp])
        {
            break;
        }

        ucFirstNumber  = (VOS_UINT8)(pucBcdNum[ucTmp] & 0x0F); /*取出高半字节*/

        ucSecondNumber = (VOS_UINT8)((pucBcdNum[ucTmp] >> 4) & 0x0F);/*取出低半字节*/

        AT_BcdHalfByteToAscii(ucFirstNumber, pucAsciiNum);

        pucAsciiNum++;

        ucLen++;

        if(0x0F == ucSecondNumber)
        {
            break;
        }

        AT_BcdHalfByteToAscii(ucSecondNumber, pucAsciiNum);

        pucAsciiNum++;

        ucLen++;
    }

    *pucLen = ucLen;

    return;
}

#if (FEATURE_VSIM == FEATURE_ON)

VOS_VOID  At_StkHvsmrIndPrint(
    VOS_UINT8                           ucIndex,
    SI_STK_EVENT_INFO_STRU             *pstEvent
)
{
    VOS_UINT16                          usLength = 0;

    /* 打印输入AT命令类型 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       "^HVSMR: ");

    /* 打印输入主动命令类型长度和类型 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,",
                                       (pstEvent->STKCmdStru.CmdStru.SmsPpDlDataInfo.ucLen*2));

    if (VOS_NULL != pstEvent->STKCmdStru.CmdStru.SmsPpDlDataInfo.ucLen)
    {
        /* 将16进制数转换为ASCII码后输入主动命令内容 */
        usLength += (TAF_UINT16)At_HexText2AsciiStringSimple(AT_CMD_MAX_LEN,
                                                            (TAF_INT8 *)pgucAtSndCodeAddr,
                                                            (TAF_UINT8 *)pgucAtSndCodeAddr + usLength,
                                                            pstEvent->STKCmdStru.CmdStru.SmsPpDlDataInfo.ucLen,
                                                            pstEvent->STKCmdStru.CmdStru.SmsPpDlDataInfo.aucData);
    }

    /* 打印回车换行 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",
                                        gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}
#endif


VOS_VOID  At_StkCcinIndPrint(
    VOS_UINT8                           ucIndex,
    SI_STK_EVENT_INFO_STRU             *pstEvent
)
{
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           aucAscii[250] = {0};
    VOS_UINT8                           ucAsciiLen = 0;
    /* 打印输入AT命令类型 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCcin);

    /* 打印call/sms control 类型 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d",
                                       (pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.ucType));

    /* 打印Call/SMS Control的结果 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%d",
                                       (pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.ucResult));

    /* 打印ALPHAID标识 */
    if (0 != pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.stAlphaIdInfo.ulAlphaLen)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",\"%s",
                                       (pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.stAlphaIdInfo.aucAlphaId));

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "\"");
     }
     else
     {
         usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                            ",\"\"");

     }

    if (SI_STK_SMS_CTRL == pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.ucType)
    {
        /* 将目的号码由BCD码转换成acsii */
        AT_BcdToAscii(pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stMoSmsCtrlInfo.stDstAddrInfo.ucAddrLen,
                      pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stMoSmsCtrlInfo.stDstAddrInfo.aucAddr,
                      aucAscii,
                      &ucAsciiLen);

        /* 打印目的地址和类型 */
        At_StkNumPrint(&usLength,
                       aucAscii,
                       ucAsciiLen,
                       pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stMoSmsCtrlInfo.stDstAddrInfo.ucNumType);

        /* 将服务中心号码由BCD码转换成acsii */
        AT_BcdToAscii(pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stMoSmsCtrlInfo.stSerCenterAddrInfo.ucAddrLen,
                      pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stMoSmsCtrlInfo.stSerCenterAddrInfo.aucAddr,
                      aucAscii,
                      &ucAsciiLen);

        /* 打印服务中心地址和类型 */
        At_StkNumPrint(&usLength,
                       aucAscii,
                       ucAsciiLen,
                       pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stMoSmsCtrlInfo.stSerCenterAddrInfo.ucNumType);

    }
    else if (SI_STK_USSD_CALL_CTRL == pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.ucType)
    {

        /* 打印dcs字段和data字段 */
        At_StkNumPrint(&usLength,
                     pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stCtrlDataInfo.aucData,
                     pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stCtrlDataInfo.usDataLen,
                     pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stCtrlDataInfo.ucDataType);
    }
    else
    {

        /* 将目的号码由BCD码转换成acsii */
        AT_BcdToAscii((VOS_UINT8)pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stCtrlDataInfo.usDataLen,
                      pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stCtrlDataInfo.aucData,
                      aucAscii,
                      &ucAsciiLen);

        /* 打印目的地址和类型 */
        At_StkNumPrint(&usLength,
                       aucAscii,
                       ucAsciiLen,
                       pstEvent->STKCmdStru.CmdStru.STKCcIndInfo.uInfo.stCtrlDataInfo.ucDataType);
    }

    /* 打印回车换行 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s",
                                        gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCrLfAddr, usLength + 2);

    return;
}


VOS_VOID AT_ReportCCallstateResult(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    VOS_UINT8                          *pucRptCfg,
    AT_CS_CALL_STATE_ENUM_UINT8         enCallState,
    TAF_CALL_VOICE_DOMAIN_ENUM_UINT8    enVoiceDomain
)
{
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucIndex;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    /* 获取client id对应的Modem Id */
    ulRslt = AT_GetModemIdFromClient(usClientId, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_WARN_LOG("AT_ReportCCallstateResult: WARNING:MODEM ID NOT FOUND!");
        return;
    }

    /* CCALLSTATE需要广播上报，根据MODEM ID设置广播上报的Index */
    if (MODEM_ID_0 == enModemId)
    {
        ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    }
    else
    {
        ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_1;
    }

    if (VOS_TRUE == AT_CheckRptCmdStatus(pucRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CALLSTATE))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s: %d,%d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CCALLSTATE].pucText,
                                           ucCallId,
                                           enCallState,
                                           enVoiceDomain,
                                           gaucAtCrLf);

    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}


VOS_VOID AT_ReportCCallstateHoldList(
    MN_CALL_EVT_HOLD_STRU              *pstHoldEvt,
    AT_CS_CALL_STATE_ENUM_UINT8         enCallState
)
{
    VOS_UINT8                           ucLoop;

    for (ucLoop = 0; ucLoop < (pstHoldEvt->ucCallNum); ucLoop++)
    {
        AT_ReportCCallstateResult(pstHoldEvt->usClientId,
                                  pstHoldEvt->aucCallId[ucLoop],
                                  pstHoldEvt->aucCurcRptCfg,
                                  enCallState,
                                  pstHoldEvt->enVoiceDomain);
    }

    return;
}


VOS_VOID AT_ReportCCallstateRetrieveList(
    MN_CALL_EVT_RETRIEVE_STRU          *pstRetrieveEvt,
    AT_CS_CALL_STATE_ENUM_UINT8         enCallState
)
{
    VOS_UINT8                           ucLoop;

    for (ucLoop = 0; ucLoop < (pstRetrieveEvt->ucCallNum); ucLoop++)
    {
        AT_ReportCCallstateResult(pstRetrieveEvt->usClientId,
                                  pstRetrieveEvt->aucCallId[ucLoop],
                                  pstRetrieveEvt->aucCurcRptCfg,
                                  enCallState,
                                  pstRetrieveEvt->enVoiceDomain);
    }

    return;
}

VOS_VOID AT_CSCallStateReportProc(
    MN_AT_IND_EVT_STRU                 *pstData )
{
    MN_CALL_EVENT_ENUM_U32              enEvent;
    TAF_UINT32                          ulEventLen;
    MN_CALL_INFO_STRU                  *pstCallInfo     = VOS_NULL_PTR;
    MN_CALL_EVT_HOLD_STRU              *pstHoldEvt      = VOS_NULL_PTR;
    MN_CALL_EVT_RETRIEVE_STRU          *pstRetrieveEvt  = VOS_NULL_PTR;

    ulEventLen      = sizeof(MN_CALL_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);
    pstCallInfo     = (MN_CALL_INFO_STRU *)&pstData->aucContent[ulEventLen];

    switch(enEvent)
    {
        case MN_CALL_EVT_ORIG:
            AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_ORIG, pstCallInfo->enVoiceDomain);
            return;
        case MN_CALL_EVT_CALL_PROC:
            AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_CALL_PROC, pstCallInfo->enVoiceDomain);
            return;
        case MN_CALL_EVT_ALERTING:
            AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_ALERTING, pstCallInfo->enVoiceDomain);
            return;
        case MN_CALL_EVT_CONNECT:
            AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_CONNECT, pstCallInfo->enVoiceDomain);
            return;
        case MN_CALL_EVT_RELEASED:
            AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_RELEASED, pstCallInfo->enVoiceDomain);
            return;
        case MN_CALL_EVT_INCOMING:
            if ( MN_CALL_S_INCOMING == pstCallInfo->enCallState )
            {
                AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_INCOMMING, pstCallInfo->enVoiceDomain);
            }
            else if ( MN_CALL_S_WAITING == pstCallInfo->enCallState )
            {
                AT_ReportCCallstateResult(pstData->clientId, pstCallInfo->callId, pstCallInfo->aucCurcRptCfg, AT_CS_CALL_STATE_WAITING, pstCallInfo->enVoiceDomain);
            }
            else
            {
                ;
            }
            return;
        case MN_CALL_EVT_HOLD:
            pstHoldEvt      = (MN_CALL_EVT_HOLD_STRU *)(pstData->aucContent);
            AT_ReportCCallstateHoldList(pstHoldEvt,
                                    AT_CS_CALL_STATE_HOLD);
            return;
        case MN_CALL_EVT_RETRIEVE:
            pstRetrieveEvt  = (MN_CALL_EVT_RETRIEVE_STRU *)(pstData->aucContent);
            AT_ReportCCallstateRetrieveList(pstRetrieveEvt,
                                    AT_CS_CALL_STATE_RETRIEVE);
            return;

        default:
            break;
    }

    return;
}


VOS_VOID AT_ReportCendResult(
    VOS_UINT8                           ucIndex,
    MN_CALL_INFO_STRU                  *pstCallInfo
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    usLength  = 0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_ReportCendResult: Get modem id fail.");
        return;
    }

    if (VOS_TRUE == AT_CheckRptCmdStatus(pstCallInfo->aucCurcRptCfg, AT_CMD_RPT_CTRL_BY_CURC, AT_RPT_CMD_CEND))
    {
        usLength    += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s^CEND:%d,%d,%d,%d%s",
                                              gaucAtCrLf,
                                              pstCallInfo->callId,
#if defined (DMT) /* DMT工程匹配通话时长一律为0 */
                                              0,
#else
                                              pstCallInfo->ulPreCallTime,
#endif
                                              pstCallInfo->enNoCliCause,
                                              pstCallInfo->enCause,
                                              gaucAtCrLf);
        At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
    }

    return;
}
VOS_UINT16 At_PrintClprInfo(
        VOS_UINT8                           ucIndex,
        MN_CALL_CLPR_GET_CNF_STRU          *pstClprGetCnf)
{
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucType;
    VOS_CHAR                            aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2) + 1];
    VOS_UINT32                          ulAsiciiLen;

    /* 初始化 */
    usLength = 0;
    PS_MEM_SET(aucAsciiNum, 0x00, sizeof(aucAsciiNum));

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                             (VOS_CHAR *)pgucAtSndCodeAddr,
                             (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                             "%s: ",
                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /* 输出<PI>参数 */
    if (VOS_TRUE == pstClprGetCnf->stRedirectInfo.bitOpPI)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "%d,",
                                 pstClprGetCnf->stRedirectInfo.enPI);

    }
    else
    {
         usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 ",");
    }

    /* 输出<no_CLI_cause>参数 */
    if (VOS_TRUE == pstClprGetCnf->stRedirectInfo.bitOpNoCLICause)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "%d,",
                                 pstClprGetCnf->stRedirectInfo.enNoCLICause);

    }
    else
    {
         usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 ",");
    }

    /* 输出<redirect_num>和<num_type>参数 */
    if (VOS_TRUE == pstClprGetCnf->stRedirectInfo.bitOpRedirectNum)
    {
        AT_BcdNumberToAscii(pstClprGetCnf->stRedirectInfo.stRedirectNum.aucBcdNum,
                            pstClprGetCnf->stRedirectInfo.stRedirectNum.ucNumLen,
                            aucAsciiNum);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "\"%s\",%d,",
                                 aucAsciiNum,
                                 (pstClprGetCnf->stRedirectInfo.stRedirectNum.enNumType| AT_NUMBER_TYPE_EXT));

    }
    else
    {
         usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 ",,");
    }

    /* 输出<redirect_subaddr>和<num_type>参数 */
    ucType = (MN_CALL_IS_EXIT | (MN_CALL_SUBADDR_NSAP << 4));
    if ((VOS_TRUE == pstClprGetCnf->stRedirectInfo.bitOpRedirectSubaddr)
     && (ucType == pstClprGetCnf->stRedirectInfo.stRedirectSubaddr.Octet3))
    {
        ulAsiciiLen = pstClprGetCnf->stRedirectInfo.stRedirectSubaddr.LastOctOffset
                    - sizeof(pstClprGetCnf->stRedirectInfo.stRedirectSubaddr.Octet3);

        PS_MEM_CPY(aucAsciiNum,
                   pstClprGetCnf->stRedirectInfo.stRedirectSubaddr.SubAddrInfo,
                   ulAsiciiLen);

        aucAsciiNum[ulAsiciiLen] = '\0';

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "\"%s\",%d",
                                 aucAsciiNum,
                                 pstClprGetCnf->stRedirectInfo.stRedirectSubaddr.Octet3);
    }
    else
    {
         usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 ",");
    }

    return usLength;
}
VOS_VOID At_SetClprCnf(MN_AT_IND_EVT_STRU *pstData)
{
    MN_CALL_CLPR_GET_CNF_STRU          *pstClprGetCnf;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;


    ucIndex = 0;

    /* 初始化 */
    pstClprGetCnf = (MN_CALL_CLPR_GET_CNF_STRU *)pstData->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstClprGetCnf->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("At_SetClprCnf: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    /* 如果为广播类型，则返回AT_ERROR */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_SetClprCnf: WARNING:AT_BROADCAST_INDEX!");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_CLPR_GET */
    if (AT_CMD_CLPR_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_SetClprCnf: WARNING:Not AT_CMD_CLPR_GET!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if (TAF_ERR_NO_ERROR != pstClprGetCnf->ulRet)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
        return;
    }

    usLength = At_PrintClprInfo(ucIndex, pstClprGetCnf);

    /* 打印结果 */
    gstAtSendData.usBufLen  = usLength;
    At_FormatResultData(ucIndex, AT_OK);

    return;

}



VOS_VOID AT_RcvMmaAcInfoChangeInd(
    VOS_UINT8                           ucIndex,
    TAF_PHONE_EVENT_INFO_STRU          *pstEvent
)
{
    VOS_UINT16                          usLength;
    TAF_MMA_CELL_AC_INFO_STRU          *pstCellAcInfo = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    enModemId = MODEM_ID_0;

    usLength = 0;
    pstCellAcInfo = (TAF_MMA_CELL_AC_INFO_STRU*)(&pstEvent->stCellAcInfo);

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvMmaAcInfoChangeInd: Get modem id fail.");
        return;
    }

    /* 通过AT^CURC控制紧急呼叫号码不上报 */
    if (VOS_FALSE == pstEvent->Op_CellAcInfo)
    {
        return;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%d,%d,%d,%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_ACINFO].pucText,
                                       pstCellAcInfo->ucSrvDomain,/* 上报服务域 */
                                       pstCellAcInfo->enCellAcType,/* 上报小区禁止接入类型 */
                                       pstCellAcInfo->ucRestrictRegister,/* 上报是否注册受限 */
                                       pstCellAcInfo->ucRestrictPagingRsp,/* 上报是否寻呼受限 */
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}



VOS_VOID AT_PhRegRejInfoProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    VOS_UINT16                          usLength;
    TAF_PH_REG_REJ_INFO_STRU           *pstRejInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength    = 0;
    enModemId   = MODEM_ID_0;
    pstRejInfo  = (TAF_PH_REG_REJ_INFO_STRU*)pData;

    if (AT_FAILURE == At_ClientIdToUserId(pstRejInfo->ClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_PhRegRejInfoProc At_ClientIdToUserId FAILURE");
        return;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_PhRegRejInfoProc: Get modem id fail.");
        return;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_REJINFO].pucText);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%x%x%x",
                                       (pstRejInfo->stPlmnId.Mcc & 0x0f00)>>8,
                                       (pstRejInfo->stPlmnId.Mcc & 0xf0)>>4,
                                       (pstRejInfo->stPlmnId.Mcc & 0x0f));

    if (0x0f00 == (pstRejInfo->stPlmnId.Mnc & 0x0f00))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x,",
                                           (pstRejInfo->stPlmnId.Mnc & 0xf0)>>4,
                                           (pstRejInfo->stPlmnId.Mnc & 0x0f));
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x%x,",
                                           (pstRejInfo->stPlmnId.Mnc & 0x0f00)>>8,
                                           (pstRejInfo->stPlmnId.Mnc & 0xf0)>>4,
                                           (pstRejInfo->stPlmnId.Mnc & 0x0f));
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d,%d,%d%s",
                                       pstRejInfo->ucSrvDomain,
                                       pstRejInfo->ulRejCause,
                                       pstRejInfo->enRat,
                                       pstRejInfo->ucRejType,
                                       gaucAtCrLf);


    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    return;
}
VOS_VOID AT_PhPlmnSelectInfoProc(TAF_UINT8* pData,TAF_UINT16 usLen)
{
    VOS_UINT16                          usLength;
    TAF_PH_PLMN_SELECTION_INFO_STRU    *pstPlmnSeleRlst = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength         = 0;
    enModemId        = MODEM_ID_0;
    pstPlmnSeleRlst  = (TAF_PH_PLMN_SELECTION_INFO_STRU*)pData;

    if (AT_FAILURE == At_ClientIdToUserId(pstPlmnSeleRlst->ClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_PhPlmnSelectInfoProc At_ClientIdToUserId FAILURE");
        return;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_PhPlmnSelectInfoProc: Get modem id fail.");
        return;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_PLMNSELEINFO].pucText);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d%s",
                                       pstPlmnSeleRlst->ulPlmnSelectFlag,
                                       pstPlmnSeleRlst->ulPlmnSelectRlst,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}


VOS_VOID AT_PhNetScanReportSuccess(
    VOS_UINT8                           ucIndex,
    TAF_MMA_NET_SCAN_CNF_STRU          *pstNetScanCnf
)
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;
    VOS_UINT32                          ulMcc;
    VOS_UINT32                          ulMnc;

    usLength       = 0;

    /* 组装成功时的消息结果 */
    for ( i = 0; i < pstNetScanCnf->ucFreqNum; i++ )
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gastAtStringTab[AT_STRING_NETSCAN].pucText);

        ulMcc = pstNetScanCnf->astNetScanInfo[i].ulMcc;
        ulMnc = pstNetScanCnf->astNetScanInfo[i].ulMnc;

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,,,%x,%x%x%x,%x%x",
                                       pstNetScanCnf->astNetScanInfo[i].usArfcn,
                                       pstNetScanCnf->astNetScanInfo[i].usLac,
                                      (ulMcc & 0x0f),
                                      (ulMcc & 0x0f00) >> 8,
                                      (ulMcc & 0x0f0000) >> 16,
                                      (ulMnc & 0x0f),
                                      (ulMnc & 0x0f00) >> 8);

        /* 如果最后一个不是F，则需要显示 */
        if ( 0x0f != ((ulMnc & 0x0f0000) >> 16) )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%x",
                                       (ulMnc & 0x0f0000) >> 16);
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",%d,%d,%x,",
                                       pstNetScanCnf->astNetScanInfo[i].usBsic,
                                       pstNetScanCnf->astNetScanInfo[i].sRxlev,
                                       pstNetScanCnf->astNetScanInfo[i].ulCellId);

        /* 根据不同的频段设置不同的显示 */
        if ( 0 == pstNetScanCnf->astNetScanInfo[i].stBand.ulBandHigh )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%X",
                                       pstNetScanCnf->astNetScanInfo[i].stBand.ulBandLow);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%X%08X",
                                       pstNetScanCnf->astNetScanInfo[i].stBand.ulBandHigh,
                                       pstNetScanCnf->astNetScanInfo[i].stBand.ulBandLow);
        }

        if ( i != (pstNetScanCnf->ucFreqNum - 1) )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);
        }
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 如果FREQNUM ==0，则只上报OK */
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);

    return;
}


VOS_VOID AT_PhNetScanReportFailure(
    VOS_UINT8                           ucIndex,
    TAF_MMA_NET_SCAN_CAUSE_ENUM_UINT8   enNetScanCause
)
{
    VOS_UINT32                          ulResult;

    /* 根据NetScan上报的错误码转换成AT显示的错误码 */
    switch ( enNetScanCause )
    {
        case TAF_MMA_NET_SCAN_CAUSE_SIGNAL_EXIST :
            ulResult = AT_ERROR;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_STATE_NOT_ALLOWED :
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_FREQ_LOCK :
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_PARA_ERROR :
            ulResult = AT_CME_INCORRECT_PARAMETERS;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_CONFLICT :
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_SERVICE_EXIST :
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_NOT_CAMPED :
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_TIMER_EXPIRED :
            ulResult = AT_ERROR;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_RAT_TYPE_ERROR :
            ulResult = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_MMA_STATE_DISABLE :
            ulResult = AT_ERROR;
            break;

        default:
            ulResult = AT_ERROR;
            break;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulResult);

    return;
}
VOS_VOID AT_PhNetScanCnfProc(
    TAF_UINT8                          *pData,
    TAF_UINT16                          usLen
)
{
    TAF_MMA_NET_SCAN_CNF_STRU          *pstNetScanCnf       = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    pstNetScanCnf  = (TAF_MMA_NET_SCAN_CNF_STRU *)pData;

    if (AT_FAILURE == At_ClientIdToUserId(pstNetScanCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_PhNetScanCnfProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_PhNetScanCnfProc : AT_BROADCAST_INDEX.");
        return;
    }

    if ( TAF_MMA_NET_SCAN_RESULT_SUCCESS == pstNetScanCnf->enResult )
    {
        AT_PhNetScanReportSuccess(ucIndex, pstNetScanCnf);
    }
    else
    {
        AT_PhNetScanReportFailure(ucIndex, pstNetScanCnf->enCause);
    }

    return;
}



VOS_UINT32 AT_RcvDrvAgentSwverSetCnf(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_SWVER_SET_CNF_STRU       *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_SWVER_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSwverSetCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSwverSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待SWVER查询命令的结果事件上报 */
    if (AT_CMD_SWVER_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvDrvAgentSwverSetCnf: WARNING:Not AT_CMD_SWVER_SET!");
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_NO_ERROR == pstEvent->enResult)
    {
        /* 设置错误码为AT_OK           构造结构为<CR><LF>^SWVER: <SwVer>_(<VerTime>)<CR><LF>
             <CR><LF>OK<CR><LF>格式 */
        ulRet                  = AT_OK;

        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s: %s_(%s)",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstEvent->stSwverInfo.aucSWVer,
                                          pstEvent->stSwverInfo.acVerTime);

    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet                  = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_VOID At_RcvMnCallSetCssnCnf(MN_AT_IND_EVT_STRU *pstData)
{
    MN_CALL_SET_CSSN_CNF_STRU      *pstCssnCnf;
    VOS_UINT8                       ucIndex;
    VOS_UINT32                      ulResult;

    pstCssnCnf = (MN_CALL_SET_CSSN_CNF_STRU *)pstData->aucContent;

    if (AT_FAILURE == At_ClientIdToUserId(pstCssnCnf->ClientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvMnCallSetCssnCnf: AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvMnCallSetCssnCnf : AT_BROADCAST_INDEX.");
        return;
    }

    /* AT模块在等待设置命令的回复结果事件上报 */
    if (AT_CMD_CSSN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RcvMnCallSetCssnCnf: WARNING:Not AT_CMD_APP_SET_CSSN_REQ!");
        return;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstCssnCnf->ulRet)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_CME_INCORRECT_PARAMETERS;
    }

    At_FormatResultData(ucIndex,ulResult);

}


VOS_VOID AT_RcvMnCallChannelInfoInd(VOS_VOID *pEvtInfo)
{
    MN_CALL_EVT_CHANNEL_INFO_STRU      *pstChannelInfoInd;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    usLength          = 0;
    pstChannelInfoInd = (MN_CALL_EVT_CHANNEL_INFO_STRU *)pEvtInfo;

    if ((pstChannelInfoInd->enCodecType == MN_CALL_CODEC_TYPE_BUTT)
     && (VOS_FALSE == pstChannelInfoInd->ucIsLocalAlertingFlag))
    {
        AT_WARN_LOG("AT_RcvMnCallChannelInfoInd: WARNING: CodecType BUTT!");
        return;
    }

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstChannelInfoInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMnCallChannelInfoInd:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    /* 本地回铃音上报^CSCHANNELINFO:0 */
    if (VOS_TRUE == pstChannelInfoInd->ucIsLocalAlertingFlag)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s %d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CS_CHANNEL_INFO].pucText,
                                           0,    /* 网络振铃无带内音信息 */
                                           pstChannelInfoInd->enVoiceDomain,
                                           gaucAtCrLf);

    }
    else if ( pstChannelInfoInd->enCodecType == MN_CALL_CODEC_TYPE_AMRWB )
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s %d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CS_CHANNEL_INFO].pucText,
                                           2,    /* 宽带语音 */
                                           pstChannelInfoInd->enVoiceDomain,
                                           gaucAtCrLf);
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s %d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CS_CHANNEL_INFO].pucText,
                                           1,    /* 窄带语音 */
                                           pstChannelInfoInd->enVoiceDomain,
                                           gaucAtCrLf);
    }

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}


VOS_VOID At_RcvPlmnQryCnf(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    TAF_PLMN_ID_STRU                   *pstPlmnId = VOS_NULL_PTR;
    VOS_UINT16                          usLength;

    pstPlmnId = (TAF_PLMN_ID_STRU *)pPara;
    usLength  = 0;

    /* 无效的PLMN */
    if ((0 == pstPlmnId->Mcc)
     && (0 == pstPlmnId->Mnc))
    {
        At_FormatResultData(ucIndex, AT_OK);
        return;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s^PLMN: ",
                                       gaucAtCrLf);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%x%x%x,",
                                       (pstPlmnId->Mcc & 0x0f00)>>8,
                                       (pstPlmnId->Mcc & 0xf0)>>4,
                                       (pstPlmnId->Mcc & 0x0f));

    if (0x0f00 == (pstPlmnId->Mnc & 0x0f00))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x",
                                           (pstPlmnId->Mnc & 0xf0)>>4,
                                           (pstPlmnId->Mnc & 0x0f));
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x%x",
                                           (pstPlmnId->Mnc & 0x0f00)>>8,
                                           (pstPlmnId->Mnc & 0xf0)>>4,
                                           (pstPlmnId->Mnc & 0x0f));
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);
    return;

}
VOS_VOID At_RcvXlemaQryCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    MN_CALL_ECC_NUM_INFO_STRU          *pstEccNumInfo = VOS_NULL_PTR;
    VOS_UINT8                           aucAsciiNum[(MN_CALL_MAX_BCD_NUM_LEN*2)+1];
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulMcc;

    ucIndex = 0;
    ulMcc   = 0;
    PS_MEM_SET(aucAsciiNum, 0x00, sizeof(aucAsciiNum));

    /* 获取上报的紧急呼号码信息 */
    pstEccNumInfo = (MN_CALL_ECC_NUM_INFO_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEccNumInfo->usClientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvXlemaQryCnf:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    /* 如果为广播类型，则返回 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvXlemaQryCnf: WARNING:AT_BROADCAST_INDEX!");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_XLEMA_QRY */
    if (AT_CMD_XLEMA_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_RcvXlemaQryCnf: WARNING:Not AT_CMD_XLEMA_QRY!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 向APP逐条上报紧急呼号码 */
    for (i = 0; i < pstEccNumInfo->ulEccNumCount; i++)
    {
        /* 将BCD码转化为ASCII码 */
        AT_BcdNumberToAscii(pstEccNumInfo->astCustomEccNumList[i].aucEccNum,
                            pstEccNumInfo->astCustomEccNumList[i].ucEccNumLen,
                            (VOS_CHAR*)aucAsciiNum);

        /* 将NAS格式的MCC转化为BCD格式 */
        AT_ConvertNasMccToBcdType(pstEccNumInfo->astCustomEccNumList[i].ulMcc, &ulMcc);

        usLength = 0;
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%s^XLEMA: ",
                                           gaucAtCrLf);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%d,%d,%s,%d,%d,",
                                           (i+1),
                                           pstEccNumInfo->ulEccNumCount,
                                           aucAsciiNum,
                                           pstEccNumInfo->astCustomEccNumList[i].ucCategory,
                                           pstEccNumInfo->astCustomEccNumList[i].ucValidSimPresent);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%x%x%x",
                                           (ulMcc & 0x0f00)>>8,
                                           (ulMcc & 0xf0)>>4,
                                           (ulMcc & 0x0f));

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           ",%d%s",
                                           pstEccNumInfo->astCustomEccNumList[i].ucAbnormalServiceFlg,
                                           gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        PS_MEM_SET(aucAsciiNum, 0x00, sizeof(aucAsciiNum));
    }

    At_FormatResultData(ucIndex, AT_OK);

    return;
}
VOS_VOID AT_RcvUserSrvStateQryCnf(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           OpId,
    VOS_VOID                           *pPara
)
{
    TAF_USER_SRV_STATE_STRU            *pstUserSrvState = VOS_NULL_PTR;
    VOS_UINT16                          usLength;

    pstUserSrvState = (TAF_USER_SRV_STATE_STRU *)pPara;
    usLength  = 0;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "^USERSRVSTATE: ");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d",
                                       pstUserSrvState->ulCsSrvExistFlg,
                                       pstUserSrvState->ulPsSrvExistFlg);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);
    return;

}



VOS_VOID AT_RcvTafCallStartDtmfCnf(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    VOS_UINT8                           ucIndex;
    TAF_CALL_EVT_DTMF_CNF_STRU         *pstDtmfCNf;

    /* 根据ClientID获取通道索引 */
    if(AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafCallStartDtmfCnf: Get Index Fail!");
        return;
    }

    /* AT模块在等待^DTMF命令/+VTS命令的操作结果事件上报 */
    if ( (AT_CMD_DTMF_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_VTS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt) )
    {
        AT_WARN_LOG("AT_RcvTafCallStartDtmfCnf: Error Option!");
        return;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 根据临时响应的错误码打印命令的结果 */
    pstDtmfCNf = (TAF_CALL_EVT_DTMF_CNF_STRU *)(pstData->aucContent
                                              + sizeof(MN_CALL_EVENT_ENUM_U32));
    if (TAF_CS_CAUSE_SUCCESS != pstDtmfCNf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}


VOS_VOID AT_RcvTafCallStopDtmfCnf(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    VOS_UINT8                           ucIndex;
    TAF_CALL_EVT_DTMF_CNF_STRU         *pstDtmfCNf;

    /* 根据ClientID获取通道索引 */
    if(AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafCallStopDtmfCnf: Get Index Fail!");
        return;
    }

    /* AT模块在等待^DTMF命令的操作结果事件上报 */
    if (AT_CMD_DTMF_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvTafCallStopDtmfCnf: Error Option!");
        return;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 根据临时响应的错误码打印命令的结果 */
    pstDtmfCNf = (TAF_CALL_EVT_DTMF_CNF_STRU *)(pstData->aucContent
                                              + sizeof(MN_CALL_EVENT_ENUM_U32));
    if (TAF_CS_CAUSE_SUCCESS != pstDtmfCNf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}



VOS_VOID At_RcvTafCallOrigCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    TAF_UINT16                          usLen
)
{
    TAF_UINT8                           ucIndex;
    MN_CALL_INFO_STRU                  *pstCallInfo         = VOS_NULL_PTR;
    MN_CALL_EVENT_ENUM_U32              enEvent;
    TAF_UINT32                          ulEventLen;
    TAF_UINT32                          ulResult;
    TAF_UINT16                          usLength;

    ulResult = AT_FAILURE;
    usLength = 0;
    ucIndex  = 0;

    AT_LOG1("At_RcvTafCallOrigCnf pEvent->ClientId",pstData->clientId);
    AT_LOG1("At_RcvTafCallOrigCnf usMsgName",pstData->usMsgName);

    ulEventLen = sizeof(MN_CALL_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);
    pstCallInfo = (MN_CALL_INFO_STRU *)&pstData->aucContent[ulEventLen];

    if (AT_FAILURE == At_ClientIdToUserId(pstCallInfo->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_CsEventProc At_ClientIdToUserId FAILURE");
        return;
    }

    /* 成功时，回复OK；失败时，回复NO CARRIER */
    if (TAF_CS_CAUSE_SUCCESS == pstCallInfo->enCause)
    {
        /* cs域的可视电话里面，这里不能上报OK */
        if (PS_TRUE == At_CheckOrigCnfCallType(pstCallInfo, ucIndex))
        {
            ulResult = AT_OK;
        }
        else
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            return;
        }

    }
    else
    {
        if (TAF_CS_CAUSE_NO_CALL_ID == pstCallInfo->enCause)
        {
            ulResult = AT_ERROR;
        }
        else
        {
            ulResult = AT_NO_CARRIER;
        }

        AT_SetCsCallErrCause(ucIndex, pstCallInfo->enCause);
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);


}


VOS_VOID At_RcvTafCallSupsCmdCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    TAF_UINT16                          usLen
)
{
    TAF_UINT8                           ucIndex;
    MN_CALL_INFO_STRU                  *pstCallInfo         = VOS_NULL_PTR;
    MN_CALL_EVENT_ENUM_U32              enEvent;
    TAF_UINT32                          ulEventLen;
    TAF_UINT32                          ulResult;
    TAF_UINT16                          usLength;

    ulResult    = AT_FAILURE;
    usLength    = 0;
    ucIndex     = 0;

    AT_LOG1("At_RcvTafCallOrigCnf pEvent->ClientId",pstData->clientId);
    AT_LOG1("At_RcvTafCallOrigCnf usMsgName",pstData->usMsgName);

    ulEventLen = sizeof(MN_CALL_EVENT_ENUM_U32);
    PS_MEM_CPY(&enEvent,  pstData->aucContent, ulEventLen);
    pstCallInfo = (MN_CALL_INFO_STRU *)&pstData->aucContent[ulEventLen];

    if(AT_FAILURE == At_ClientIdToUserId(pstCallInfo->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCallSupsCmdCnf At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCallSupsCmdCnf: AT_BROADCAST_INDEX.");
        return;
    }

    /* AT口已经释放 */
    if (AT_FW_CLIENT_STATUS_READY == g_stParseContext[ucIndex].ucClientStatus)
    {
        AT_WARN_LOG("At_RcvTafCallSupsCmdCnf : AT command entity is released.");
        return;
    }

    /* 挂断电话成功时回复OK */
    if (TAF_CS_CAUSE_SUCCESS == pstCallInfo->enCause)
    {

        /* 因判断是否有呼叫在C核上实现，在无呼叫的情况下上报MN_ERR_NO_ERROR
           AT命令返回结果需要为AT_OK */
        if ((AT_CMD_H_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
         || (AT_CMD_CHUP_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
         || (((AT_CMD_A_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
           || (AT_CMD_CHLD_SET == gastAtClientTab[ucIndex].CmdCurrentOpt))
           && (VOS_TRUE == pstCallInfo->ucAtaReportOkAsyncFlag)))
        {
            ulResult = AT_OK;
        }
        else
        {
            return;
        }

    }
    else
    {
        switch(gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            case AT_CMD_CHLD_SET:
            case AT_CMD_CHUP_SET:
            case AT_CMD_A_SET:
            case AT_CMD_CHLD_EX_SET:
            case AT_CMD_H_SET:

                ulResult = AT_ConvertCallError(pstCallInfo->enCause);
                break;

            default:

                /* 默认值 */
                ulResult = AT_CME_UNKNOWN;
                break;
        }
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex,ulResult);

}


#if (FEATURE_ON == FEATURE_VSIM)
VOS_UINT32 AT_RcvDrvAgentHvpdhSetCnf(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_HVPDH_CNF_STRU           *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_HVPDH_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHvpdhSetCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHvpdhSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待HVPDH设置命令的结果事件上报 */
    if (AT_CMD_HVPDH_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvDrvAgentHvpdhSetCnf: WARNING:Not AT_CMD_HVPDH_SET!");
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (DRV_AGENT_HVPDH_NO_ERROR == pstEvent->enResult)
    {
        ulRet = AT_OK;
    }
    else
    {
        /* 查询失败返回ERROR字符串 */
        ulRet = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData输出结果 */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}
#endif
VOS_VOID AT_PhEOPlmnQueryCnfProc(TAF_UINT8 *pData)
{
    VOS_UINT16                                              usLen;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulRslt;
    TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU                    *pstEOPlmnQryCnf = VOS_NULL_PTR;

    usLen            = 0;
    pstEOPlmnQryCnf  = (TAF_PHONE_EVENT_EOPLMN_QRY_CNF_STRU *)pData;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEOPlmnQryCnf->ClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_PhEOPlmnQueryCnfProc At_ClientIdToUserId FAILURE");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_EOPLMN_QRY */
    if (AT_CMD_EOPLMN_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_PhEOPlmnQueryCnfProc: WARNING:Not AT_CMD_EOPLMN_QRY!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (TAF_ERR_NO_ERROR == pstEOPlmnQryCnf->ulResult)
    {
        usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        "%s: \"%s\",%d,",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        pstEOPlmnQryCnf->aucVersion,
                                        pstEOPlmnQryCnf->usOPlmnNum * TAF_AT_PLMN_WITH_RAT_LEN);


        /* 将16进制数转换为ASCII码后输入主动命令内容 */
        usLen += (TAF_UINT16)At_HexText2AsciiStringSimple(AT_CMD_MAX_LEN,
                                                            (TAF_INT8 *)pgucAtSndCodeAddr,
                                                            (TAF_UINT8 *)pgucAtSndCodeAddr + usLen,
                                                            pstEOPlmnQryCnf->usOPlmnNum * TAF_SIM_PLMN_WITH_RAT_LEN,
                                                            pstEOPlmnQryCnf->aucOPlmnList);

        gstAtSendData.usBufLen = usLen;

        ulRslt = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulRslt = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return;
}
VOS_VOID AT_PhEOPlmnSetCnfProc(TAF_UINT8 *pData)
{
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulRslt;
    TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU                    *pstEOPlmnSetCnf = VOS_NULL_PTR;

    pstEOPlmnSetCnf  = (TAF_PHONE_EVENT_EOPLMN_SET_CNF_STRU *)pData;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEOPlmnSetCnf->ClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_PhEOPlmnSetCnfProc At_ClientIdToUserId FAILURE");
        return;
    }

    /* 如果为广播类型，则返回 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_PhEOPlmnSetCnfProc : AT_BROADCAST_INDEX.");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_EOPLMN_SET */
    if (AT_CMD_EOPLMN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_PhEOPlmnSetCnfProc: WARNING:Not AT_CMD_EOPLMN_SET!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 打印结果 */
    if( TAF_ERR_NO_ERROR == pstEOPlmnSetCnf->ulResult)
    {
        ulRslt = AT_OK;
    }
    else
    {
        ulRslt = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex,ulRslt);

    return;
}


VOS_UINT32 AT_RcvNvManufactureExtSetCnf(VOS_VOID *pMsg)
{
    VOS_UINT8                                       ucIndex;
    DRV_AGENT_NVMANUFACTUREEXT_SET_CNF_STRU        *pstEvent;
    DRV_AGENT_MSG_STRU                             *pstRcvMsg;

    /* 初始化 */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_NVMANUFACTUREEXT_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvNvManufactureExtSetCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvNvManufactureExtSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待NvManufactureExt设置命令的结果事件上报 */
    if (AT_CMD_NVMANUFACTUREEXT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvNvManufactureExtSetCnf: WARNING:Not AT_CMD_NVMANUFACTUREEXT_SET!");
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (NV_OK == pstEvent->ulRslt)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%d",
                                                        pstEvent->ulRslt);
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


PS_BOOL_ENUM_UINT8 At_CheckOrigCnfCallType(
    MN_CALL_INFO_STRU                  *pstCallInfo,
    VOS_UINT8                           ucIndex
)
{
    switch (pstCallInfo->enCallType)
    {
        case MN_CALL_TYPE_VOICE :
        case MN_CALL_TYPE_EMERGENCY :
        case MN_CALL_TYPE_VIDEO_RX:
        case MN_CALL_TYPE_VIDEO_TX:
            return PS_TRUE;
        case MN_CALL_TYPE_VIDEO:
            if (AT_CMD_APDS_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
            {
                return PS_TRUE;
            }
            else
            {
                return PS_FALSE;
            }
        default:
            return PS_FALSE;
    }

}

#if (FEATURE_ON == FEATURE_IMS)

VOS_VOID At_RcvTafCallModifyCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    MN_CALL_MODIFY_CNF_STRU            *pstModifyCnf;
    VOS_UINT8                           ucIndex;

    ucIndex = 0;

    pstModifyCnf = (MN_CALL_MODIFY_CNF_STRU *)pstData->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstModifyCnf->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("At_RcvTafCallModifyCnf: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCallModifyCnf: WARNING:AT_BROADCAST_INDEX!");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_CALL_MODIFY_INIT_SET */
    if (AT_CMD_CALL_MODIFY_INIT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_RcvTafCallModifyCnf: WARNING:Not AT_CMD_CALL_MODIFY_INIT_SET!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断操作是否成功 */
    if (TAF_CS_CAUSE_SUCCESS != pstModifyCnf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}
VOS_VOID At_RcvTafCallAnswerRemoteModifyCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    MN_CALL_MODIFY_CNF_STRU            *pstModifyCnf;
    VOS_UINT8                           ucIndex;

    ucIndex = 0;

    pstModifyCnf = (MN_CALL_MODIFY_CNF_STRU *)pstData->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstModifyCnf->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("At_RcvTafCallAnswerRemoteModifyCnf: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCallAnswerRemoteModifyCnf: WARNING:AT_BROADCAST_INDEX!");
        return;
    }

    /* 判断当前操作类型是否为AT_CMD_CALL_MODIFY_ANS_SET */
    if (AT_CMD_CALL_MODIFY_ANS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_RcvTafCallAnswerRemoteModifyCnf: WARNING:Not AT_CMD_CALL_MODIFY_CNF_SET!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断操作是否成功 */
    if (TAF_CS_CAUSE_SUCCESS != pstModifyCnf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}
VOS_VOID At_RcvTafCallModifyStatusInd(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    MN_CALL_EVT_MODIFY_STATUS_IND_STRU *pstStatusInd;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    usLength          = 0;
    pstStatusInd      = (MN_CALL_EVT_MODIFY_STATUS_IND_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstStatusInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCallModifyStatusInd:WARNING:AT INDEX NOT FOUND!");
        return;
    }


    if (MN_CALL_MODIFY_REMOTE_USER_REQUIRE_TO_MODIFY == pstStatusInd->enModifyStatus)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s %d,%d,%d,%d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CALL_MODIFY_IND].pucText,
                                           pstStatusInd->ucCallId,
                                           pstStatusInd->enCurrCallType,
                                           pstStatusInd->enVoiceDomain,
                                           pstStatusInd->enExpectCallType,
                                           pstStatusInd->enVoiceDomain,
                                           gaucAtCrLf);

    }
    else if (MN_CALL_MODIFY_PROC_BEGIN == pstStatusInd->enModifyStatus)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s %d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CALL_MODIFY_BEG].pucText,
                                           pstStatusInd->ucCallId,
                                           pstStatusInd->enVoiceDomain,
                                           gaucAtCrLf);
    }
    else if (MN_CALL_MODIFY_PROC_END == pstStatusInd->enModifyStatus)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s %d,%d,%d%s",
                                           gaucAtCrLf,
                                           gastAtStringTab[AT_STRING_CALL_MODIFY_END].pucText,
                                           pstStatusInd->ucCallId,
                                           pstStatusInd->enVoiceDomain,
                                           pstStatusInd->enCause,
                                           gaucAtCrLf);
    }
    else
    {
        return;
    }

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}
VOS_VOID AT_RcvTafGetEconfInfoCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    TAF_CALL_ECONF_INFO_QRY_CNF_STRU   *pstCallInfos;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    ucIndex = 0;
    ulRet   = VOS_ERR;

    /* 获取呼叫信息 */
    pstCallInfos = (TAF_CALL_ECONF_INFO_QRY_CNF_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCallInfos->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafGetEconfInfoCnf: WARNING: AT INDEX NOT FOUND!");
        return;
    }


    if (AT_CMD_CLCCECONF_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvTafGetEconfInfoCnf: WARNING: CmdCurrentOpt != AT_CMD_CLCCECONF_QRY!");
        return;
    }

    /* ^CLCCECONF?命令的结果回复 */
    ulRet = At_ProcQryClccEconfResult(pstCallInfos, ucIndex);

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}
VOS_VOID AT_RcvTafEconfDialCnf(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    TAF_CALL_ECONF_DIAL_CNF_STRU       *pstEconfDialCnf;
    VOS_UINT8                           ucIndex;

    ucIndex = 0;

    pstEconfDialCnf = (TAF_CALL_ECONF_DIAL_CNF_STRU *)pstData->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEconfDialCnf->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvTafEconfDialCnf: WARNING:AT INDEX NOT FOUND!");
        return;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafEconfDialCnf: WARNING:AT_BROADCAST_INDEX!");
        return;
    }

    /* 判断当前操作类型 */
    if ((AT_CMD_ECONF_DIAL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CACMIMS_SET    != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        AT_WARN_LOG("AT_RcvTafEconfDialCnf: WARNING:Not AT_CMD_ECONF_DIAL_SET or AT_CMD_CACMIMS_SET!");
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断操作是否成功 */
    if (TAF_CS_CAUSE_SUCCESS != pstEconfDialCnf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}
VOS_VOID AT_RcvTafEconfNotifyInd(
    MN_AT_IND_EVT_STRU                 *pstData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;
    TAF_CALL_EVT_ECONF_NOTIFY_IND_STRU *pstNotifyInd = VOS_NULL_PTR;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;

    usLength     = 0;
    pstNotifyInd = (TAF_CALL_EVT_ECONF_NOTIFY_IND_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstNotifyInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafEconfNotifyInd:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    pstCcCtx = AT_GetModemCcCtxAddrFromClientId(pstNotifyInd->usClientId);

    if (pstNotifyInd->ucNumOfCalls > TAF_CALL_MAX_ECONF_CALLED_NUM)
    {
        pstCcCtx->stEconfInfo.ucNumOfCalls   = TAF_CALL_MAX_ECONF_CALLED_NUM;
    }
    else
    {
        pstCcCtx->stEconfInfo.ucNumOfCalls   = pstNotifyInd->ucNumOfCalls;
    }

    PS_MEM_CPY(pstCcCtx->stEconfInfo.astCallInfo,
               pstNotifyInd->astCallInfo,
               (sizeof(TAF_CALL_ECONF_INFO_PARAM_STRU) * pstCcCtx->stEconfInfo.ucNumOfCalls));

    /* call_num取pstNotifyInd->ucNumOfCalls，而不是pstCcCtx->stEconfInfo.ucNumOfCalls，可以方便发现错误 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s %d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_ECONFSTATE].pucText,
                                       pstNotifyInd->ucNumOfCalls,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return;
}
#endif


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif


