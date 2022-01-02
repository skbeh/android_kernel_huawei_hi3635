

/*****************************************************************************
1 头文件包含
*****************************************************************************/
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_gut.h"

#if defined(_DRV_LLT_)
#include "drv_mailbox_stub.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
	可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef	_MAILBOX_FILE_
#define _MAILBOX_FILE_	 "table"
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*定义所有已存在的共享内存通道的物理内存及硬件资源配置，全局定义*/
MAILBOX_EXTERN struct mb_cfg g_mailbox_global_cfg_tbl[MAILBOX_GLOBAL_CHANNEL_NUM+1] = {
	/*CCPU到其他核的通道*/
	MAILBOX_CHANNEL_COMPOSE(CCPU, HIFI, MSG),

	/*ACPU到其他核的通道*/
	MAILBOX_CHANNEL_COMPOSE(ACPU, HIFI, MSG),

	/*HIFI到其他核的通道*/
	MAILBOX_CHANNEL_COMPOSE(HIFI, CCPU, MSG),
	MAILBOX_CHANNEL_COMPOSE(HIFI, ACPU, MSG),

	/*结束标志*/
	{MAILBOX_MAILCODE_INVALID,	0,	0, 0}

};

/*平台邮箱通道句柄的内存池空间*/
MAILBOX_EXTERN struct mb_buff	  g_mailbox_channel_handle_pool[MAILBOX_CHANNEL_NUM];

/*平台邮箱用户回调句柄的内存池空间*/
MAILBOX_EXTERN struct mb_cb    g_mailbox_user_cb_pool[MAILBOX_USER_NUM];

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

