#include <hi_base.h>
#include <hi_timer.h>
#include <soc_interrupts.h>
#include <bsp_memmap.h>
#include <bsp_hardtimer.h>
#include <osl_irq.h>

/*宏定义*/
#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR (-1)
#endif

#ifndef SEM_EMPTY
#define SEM_EMPTY 0
#endif

#ifndef SEM_FULL
#define SEM_FULL 1
#endif
#define SOFTTIMER_MAX_NUM 40

#ifdef __KERNEL__
typedef u32             SOFTTIMER_TASK_ID;
#define ST_WAKE_HARDTIMER_ID             ACORE_SOFTTIMER_ID
#define ST_NORMAL_HARDTIMER_ID           ACORE_SOFTTIMER_NOWAKE_ID
#define TIMER_TASK_WAKE_PRI         92
#define TIMER_TASK_NOWAKE_PRI   92

#elif defined(__VXWORKS__)
typedef u32 SOFTTIMER_TASK_ID;
#define ST_WAKE_HARDTIMER_ID             CCORE_SOFTTIMER_ID
#define ST_NORMAL_HARDTIMER_ID           CCORE_SOFTTIMER_NOWAKE_ID
#define TIMER_TASK_WAKE_PRI         6
#define TIMER_TASK_NOWAKE_PRI   6
#endif

#define  TIMER_TASK_STK_SIZE            (0x1000)
#define ELAPESD_TIME_INVAILD             0xFFFFFFFF

#define TIMERn_EN_ACK                      (1<<4)
#define TIMERn_INT_MASK                    (1<<2)
#define TIMERn_MODE_PERIOD                 (1<<1)
#define HARD_TIMER_ENABLE                  1
#define HARD_TIMER_DISABLE                 0
#define TIMER_ONCE_COUNT                   0
#define TIMER_PERIOD_COUNT                 1

