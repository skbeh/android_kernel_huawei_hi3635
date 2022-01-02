/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : cpm.c
  版 本 号   : 初稿
  作    者   : 甘兰 47350
  生成日期   : 2011年9月29日
  最近修改   :
  功能描述   : 实现OAM通道的管理功能,channel port manager.
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月29日
    作    者   : 甘兰 47350
    修改内容   : 创建文件

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_CPM_C


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "cpm.h"
#include "NVIM_Interface.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

CPM_PHY_PORT_CFG_STRU                   g_astCPMPhyPortCfg[CPM_PORT_BUTT - CPM_IND_PORT];
CPM_LOGIC_PORT_CFG_STRU                 g_astCPMLogicPortCfg[CPM_COMM_BUTT];

/* 端口配置全局变量 */
OM_CHANNLE_PORT_CFG_STRU                g_stPortCfg;

/*****************************************************************************
  3 函数体申明
*****************************************************************************/

/*****************************************************************************
  4 函数体定义
*****************************************************************************/

/*****************************************************************************
 函 数 名  : CPM_PhySendReg
 功能描述  : 提供给外部的注册函数，用来物理通道接收到数据的处理
 输入参数  : enPhyPort：  注册的物理通道号
             pRecvFunc：  数据接收函数
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_PhySendReg(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, CPM_SEND_FUNC pSendFunc)
{
    if (CPM_PORT_BUTT > enPhyPort)
    {
        CPM_PHY_SEND_FUNC(enPhyPort - CPM_IND_PORT) = pSendFunc;
    }

    return;
}

/*****************************************************************************
 函 数 名  : CPM_LogicRcvReg
 功能描述  : 给逻辑通道注册接收函数
 输入参数  : enLogicPort： 注册的逻辑通道号
             pRecvFunc：   数据接收函数
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_LogicRcvReg(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort, CPM_RCV_FUNC pRcvFunc)
{
    if (CPM_COMM_BUTT > enLogicPort)
    {
        CPM_LOGIC_RCV_FUNC(enLogicPort) = pRcvFunc;
    }

    return;
}

/*****************************************************************************
 函 数 名  : CPM_QueryPhyPort
 功能描述  : 查询当前逻辑通道使用的物理端口
 输入参数  : enLogicPort：  逻辑通道号
 输出参数  : 无
 返 回 值  : 物理通道号

*****************************************************************************/
CPM_PHY_PORT_ENUM_UINT32 CPM_QueryPhyPort(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    return CPM_LOGIC_PHY_PORT(enLogicPort);
}

/*****************************************************************************
 函 数 名  : CPM_ConnectPorts
 功能描述  : 将物理通道和逻辑通道连接上
 输入参数  : enPhyPort：    物理通道号
             enLogicPort：  逻辑通道号
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_ConnectPorts(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    if ((CPM_PORT_BUTT <= enPhyPort) || (CPM_COMM_BUTT <= enLogicPort))
    {
        return;
    }

    /* 连接发送通道 */
    CPM_LOGIC_SEND_FUNC(enLogicPort)= CPM_PHY_SEND_FUNC(enPhyPort - CPM_IND_PORT);

    /* 连接接收通道 */
    CPM_PHY_RCV_FUNC(enPhyPort - CPM_IND_PORT) = CPM_LOGIC_RCV_FUNC(enLogicPort);

    /* 将物理发送函数注册给逻辑通道 */
    CPM_LOGIC_PHY_PORT(enLogicPort) = enPhyPort;

    return;
}

/*****************************************************************************
 函 数 名  : CPM_DisconnectPorts
 功能描述  : 断开物理通道和逻辑通道连接
 输入参数  : enPhyPort：    物理通道号
             enLogicPort：  逻辑通道号
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_DisconnectPorts(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    if ((CPM_PORT_BUTT <= enPhyPort) || (CPM_COMM_BUTT <= enLogicPort))
    {
        return;
    }

    /* 假如当前逻辑通道并没有使用此物理通道，则不用处理 */
    if (enPhyPort != CPM_LOGIC_PHY_PORT(enLogicPort))
    {
        return;
    }

    /* 断开接收通道 */
    CPM_PHY_RCV_FUNC(enPhyPort - CPM_IND_PORT) = VOS_NULL_PTR;

    /* 断开发送通道 */
    CPM_LOGIC_SEND_FUNC(enLogicPort)= VOS_NULL_PTR;
    CPM_LOGIC_PHY_PORT(enLogicPort) = CPM_PORT_BUTT;

    return;
}


/*****************************************************************************
 函 数 名  : CPM_PortAssociateInit
 功能描述  : 根据端口类型关联物理端口和逻辑端口
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败
*****************************************************************************/
VOS_UINT32 CPM_PortAssociateInit(VOS_VOID)
{
    VOS_UINT32                          i;

    for (i = 0; i < CPM_COMM_BUTT; i++)
    {
        g_astCPMLogicPortCfg[i].enPhyPort = CPM_PORT_BUTT;
    }

#if (VOS_OS_VER == VOS_WIN32)
    g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_WIFI;
#else
    /* 产品支持HSIC特性，直接返回成功，不做端口关联 */
    if (BSP_MODULE_SUPPORT == DRV_GET_HSIC_SUPPORT())
    {
        return VOS_OK;
    }

    /* 读取OM的物理输出通道 */
    if (NV_OK != NV_Read(en_NV_Item_Om_Port_Type, &g_stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
    {
        return VOS_ERR;
    }
#endif

    /* 检测参数*/
    if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
    {
        CPM_ConnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);
    }
    else if (CPM_OM_PORT_TYPE_VCOM == g_stPortCfg.enPortNum)
    {
        CPM_ConnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);
    }
    else if (CPM_OM_PORT_TYPE_WIFI == g_stPortCfg.enPortNum)
    {
        CPM_ConnectPorts(CPM_WIFI_AT_PORT,     CPM_AT_COMM);
        CPM_ConnectPorts(CPM_WIFI_OM_IND_PORT, CPM_OM_IND_COMM);
        CPM_ConnectPorts(CPM_WIFI_OM_CFG_PORT, CPM_OM_CFG_COMM);
    }
    /* NV项不正确时按USB输出处理 */
    else
    {
        CPM_ConnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_USB;
    }
    /*如果当前连接为USB输出，需要设置SOCP默认超时*/
    if(g_stPortCfg.enPortNum == CPM_OM_PORT_TYPE_USB)
    {
        DRV_SOCP_VOTE(SOCP_VOTE_PPM_RCV, SOCP_VOTE_FOR_WAKE);
        (VOS_VOID)DRV_SOCP_SET_TIMEOUT(SOCP_TIMEOUT_TRF, 0x17);
    }
    DRV_SOCP_VOTE(SOCP_VOTE_PPM_RCV, SOCP_VOTE_FOR_SLEEP);

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : CPM_ComSend
 功能描述  : 发送数据函数，提供给逻辑通道使用
 输入参数  : enLogicPort：逻辑通道号
             pucVirData:  数据虚拟地址
             pucPHYData:  数据物理地址
             pucData：    发送数据的指针
             ulLen:       发送数据的长度

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 CPM_ComSend(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort, VOS_UINT8 *pucVirData, VOS_UINT8 *pucPHYData, VOS_UINT32 ulLen)
{
    /* 参数检测 */
    if ((CPM_COMM_BUTT <= enLogicPort) || (VOS_NULL_PTR == pucVirData) || (0 == ulLen))
    {
        return CPM_SEND_PARA_ERR;
    }

    if (VOS_NULL_PTR == CPM_LOGIC_SEND_FUNC(enLogicPort))
    {
        return CPM_SEND_FUNC_NULL;
    }

    return CPM_LOGIC_SEND_FUNC(enLogicPort)(pucVirData, pucPHYData, ulLen);
}

/*****************************************************************************
 函 数 名  : CPM_ComRcv
 功能描述  : 接收数据函数，提供给物理通道使用
 输入参数  : enPhyPort：  物理通道号
             pucData：    接收数据的指针
             ulLen:       接收数据的长度
 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 CPM_ComRcv(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    /* 参数检测 */
    if ((CPM_PORT_BUTT <= enPhyPort) || (VOS_NULL_PTR == pucData) || (0 == ulLen))
    {
        return VOS_ERR;
    }

    if (VOS_NULL_PTR == CPM_PHY_RCV_FUNC(enPhyPort - CPM_IND_PORT))
    {
        LogPrint1("\r\nCPM_ComRcv The Phy Port %d Rec Func is NULL", (VOS_INT)enPhyPort);

        return VOS_ERR;
    }

    return CPM_PHY_RCV_FUNC(enPhyPort - CPM_IND_PORT)(pucData, ulLen);
}

/*****************************************************************************
 函 数 名  : CPM_Show
 功能描述  : 显示当前的逻辑和物理端口对应关系
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_Show(VOS_VOID)
{
    CPM_PHY_PORT_ENUM_UINT32    enPhyPort;
    CPM_LOGIC_PORT_ENUM_UINT32  enLogicPort;

    vos_printf("\r\nCPM_Show The Logic and Phy Relation is :");

    for(enLogicPort=CPM_AT_COMM; enLogicPort<CPM_COMM_BUTT; enLogicPort++)
    {
        enPhyPort = CPM_QueryPhyPort(enLogicPort);

        vos_printf("\r\nThe Logic Port %d is connnect PHY Port %d.", enLogicPort, enPhyPort);
    }

    vos_printf("\r\nCPM_Show The Phy Info is :");

    for(enPhyPort=0; enPhyPort<(CPM_PORT_BUTT - CPM_IND_PORT); enPhyPort++)
    {
        vos_printf("\r\nThe Phy %d Port's Rec Func is 0x%x, Send Func is 0x%x",
                        enPhyPort,
                        g_astCPMPhyPortCfg[enPhyPort].pRcvFunc,
                        g_astCPMPhyPortCfg[enPhyPort].pSendFunc);
    }

    for(enLogicPort=0; enLogicPort<CPM_COMM_BUTT; enLogicPort++)
    {
        vos_printf("\r\nThe Logic %d Port's Rec Func is 0x%x, Send Func is 0x%x",
                        enLogicPort,
                        g_astCPMLogicPortCfg[enLogicPort].pRcvFunc,
                        g_astCPMLogicPortCfg[enLogicPort].pSendFunc);
    }

    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
