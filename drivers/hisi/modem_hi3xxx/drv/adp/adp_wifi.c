/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  adp_wifi.c
*
*   作    者 :  wangweichao
*
*   描    述 :  本文件给出V7R2 wifi 对外接口
*
*   修改记录 :  2013年4月3日  v1.00  wangweichao
*************************************************************************/


unsigned short BSP_WifiCalDataLen(unsigned short usLen)
{
	return 0;
}


/*****************************************************************************
 函 数 名  : WIFI_TEST_CMD
 功能描述  : 测试命令
 输入参数  : cmdStr；命令字符串
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
 void WIFI_TEST_CMD(char * cmdStr)
{
}

/*****************************************************************************
 函 数 名  : WIFI_GET_TCMD_MODE
 功能描述  : 获取测试命令的模式
 输入参数  : 无
 输出参数  : 16：校准测试模式
             17：表示处于发射模式
             18：表示接收模式
 返回值    ：函数执行的状态结果值
*****************************************************************************/
 int WIFI_GET_TCMD_MODE(void)
{
	return 16;
}

/*****************************************************************************
 函 数 名  : WIFI_POWER_START
 功能描述  : WIFI上电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
 int WIFI_POWER_START(void)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : WIFI_POWER_SHUTDOWN
 功能描述  : WIFI下电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
 int WIFI_POWER_SHUTDOWN(void)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : WIFI_GET_STATUS
 功能描述  : WIFI状态获取
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
 int WIFI_GET_STATUS(void)
{
	return 1;
}

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_DETAIL_REPORT
 功能描述  : get result of rx report: totalPkt, GoodPkt, ErrorPkt
 输入参数  : 无
 输出参数  : totalPkt、goodPkt、badPkt
 返回值    ：无
*****************************************************************************/
 void WIFI_GET_RX_DETAIL_REPORT(int* totalPkt,int* goodPkt,int* badPkt)
{
}

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_PACKET_REPORT
 功能描述  : get result of rx ucast&mcast packets
 输入参数  : 无
 输出参数  : ucastPkts、mcastPkts
 返回值    ：无
*****************************************************************************/
 void  WIFI_GET_RX_PACKET_REPORT(unsigned int *ucastPkts, unsigned int *mcastPkts)
{
}

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_CUR_MODE
 功能描述  : get the currrent PA mode of the wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
             -1: wifi chip is in abnormal mode
*****************************************************************************/
 int WIFI_GET_PA_CUR_MODE(void)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_MODE
 功能描述  : get the support PA mode of wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
             -1: failed
*****************************************************************************/
 int WIFI_GET_PA_MODE(void)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : WIFI_SET_PA_MODE
 功能描述  : set the PA mode of wifi chip
 输入参数  : 0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
 输出参数  : 无
 返回值    ：0: success
             -1: failed
*****************************************************************************/
 int WIFI_SET_PA_MODE(int wifiPaMode)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DRV_WIFI_DATA_RESERVED_TAIL
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 数据区的尾部预留的长度
*****************************************************************************/
 unsigned int DRV_WIFI_DATA_RESERVED_TAIL(unsigned int len)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DRV_WIFI_SET_RX_FCTL
 功能描述  : 设置WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : BSP_OK/BSP_ERROR
*****************************************************************************/
 unsigned long DRV_WIFI_SET_RX_FCTL(unsigned long para1, unsigned long para2)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DRV_WIFI_CLR_RX_FCTL
 功能描述  : 清除WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
 unsigned long DRV_WIFI_CLR_RX_FCTL(unsigned long para1, unsigned long para2)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DRV_AT_GET_USER_EXIST_FLAG
 功能描述  : 返回当前是否有USB连接或者WIFI用户连接(C核调用)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
 unsigned long  DRV_AT_GET_USER_EXIST_FLAG(void)
{
	return 1;
}

#if 0

/*****************************************************************************
 函 数 名  : WIFI_GET_STATUS
 功能描述  : WIFI状态获取
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
int WIFI_GET_STATUS(void)
{
    return 0;
}
#endif
