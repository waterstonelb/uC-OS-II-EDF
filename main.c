/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                             (c) Copyright 1998-2004, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
*                                            WIN32 Sample Code
*
* File : APP.C
* By   : Eric Shufro
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                                CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE    128
#define  TASK_START_PRIO    5

/*
*********************************************************************************************************
*                                                VARIABLES
*********************************************************************************************************
*/

OS_STK        AppStartTaskStk[3][TASK_STK_SIZE];
OS_EVENT  *   xhl;//信号量

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppStartTask(void *p_arg);
static  void  Task1(void *p_arg);
static  void  Task2(void *p_arg);
static  void  Task3(void *p_arg);

#if OS_VIEW_MODULE > 0
static  void  AppTerminalRx(INT8U rx_data);
#endif

/*
*********************************************************************************************************
*                                                _tmain()
*
* Description : This is the standard entry point for C++ WIN32 code.  
* Arguments   : none
*********************************************************************************************************
*/

void main(int argc, char *argv[])
{
	INT8U  err;


#if 0
    BSP_IntDisAll();                       /* For an embedded target, disable all interrupts until we are ready to accept them */
#endif

    OSInit();                              /* Initialize "uC/OS-II, The Real-Time Kernel"                                      */

	xhl = OSSemCreate(1);//信号量创建

    /*OSTaskCreateExt(AppStartTask,
                    (void *)0,
                    (OS_STK *)&AppStartTaskStk[TASK_STK_SIZE-1],
                    TASK_START_PRIO,
                    TASK_START_PRIO,
                    (OS_STK *)&AppStartTaskStk[0],
                    TASK_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);*/
	OSTaskCreate(Task1, (void*)0, (OS_STK *)&AppStartTaskStk[0][TASK_STK_SIZE - 1], 1);//新建三个任务
	OSTaskCreate(Task2, (void*)0, (OS_STK *)&AppStartTaskStk[1][TASK_STK_SIZE - 1], 2);
	OSTaskCreate(Task3, (void*)0, (OS_STK *)&AppStartTaskStk[2][TASK_STK_SIZE - 1], 3);
	OSTCBPrioTbl[1]->T = 4;//在TCB结构体新增T，C参数，在此处进行初始化
	OSTCBPrioTbl[2]->T = 5;
	OSTCBPrioTbl[3]->T = 10;
	OSTCBPrioTbl[62]->T = -1;
	OSTCBPrioTbl[63]->T = -1;
	OSTCBPrioTbl[1]->C = 0;
	OSTCBPrioTbl[2]->C = 0;
	OSTCBPrioTbl[3]->C = 0;
	OSTCBPrioTbl[62]->C = 0;
	OSTCBPrioTbl[63]->C = 0;
	

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(APP_TASK_START_PRIO, (INT8U *)"Start Task", &err);
#endif

#if OS_TASK_NAME_SIZE > 14
    OSTaskNameSet(OS_IDLE_PRIO, (INT8U *)"uC/OS-II Idle", &err);
#endif

#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
    OSTaskNameSet(OS_STAT_PRIO, "uC/OS-II Stat", &err);
#endif

    OSStart();                             /* Start multitasking (i.e. give control to uC/OS-II)                               */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg   is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

void  AppStartTask (void *p_arg)
{
    p_arg = p_arg;

#if 0
    BSP_Init();                                  /* For embedded targets, initialize BSP functions                             */
#endif


#if OS_TASK_STAT_EN > 0
    OSStatInit();                                /* Determine CPU capacity                                                     */
#endif
    
    while (TRUE)                                 /* Task body, always written as an infinite loop.                             */
	{       		
		OS_Printf("Delay 1 second and print\n");  /* your code here. Create more tasks, etc.                                    */
        OSTimeDlyHMSM(0, 0, 1, 0);       
    }
}
void  Task1(void *p_arg)//任务定义
{

	int end, toDly, C = 1;//定义end，任务执行完之后的系统时间，toDly，需要延时时间
	INT8U err;//信号量错误地址
	while (TRUE)                                 /* Task body, always written as an infinite loop.                             */
	{
		OSSemPend(xhl, 9999, &err);//上锁
		while (OSTCBCur->C < C);//任务执行C个时间
		OSSemPost(xhl);//解锁
		end = OSTimeGet();
		toDly = OSTCBCur->T-end;
		if (toDly < 0)
		{
			OS_Printf("\nfailed");//调度失败时的输出
		}
		OSTCBCur->C = 0;
		OSTCBCur->T += 4;
		OSTimeDly(toDly);
	}
}
void  Task2(void *p_arg)
{


	int end, toDly,C=2;
	while (TRUE)                                 /* Task body, always written as an infinite loop.                             */
	{
		while (OSTCBCur->C < C);
		end = OSTimeGet();
		toDly = OSTCBCur->T - end;
		OSTCBCur->C = 0;
		OSTCBCur->T += 5;
		OSTimeDly(toDly);
	}
}
void  Task3(void *p_arg)
{

	int end, toDly,C=2;
	INT8U err;
	while (TRUE)                                 /* Task body, always written as an infinite loop.                             */
	{
		OSSemPend(xhl, 9999, &err);
		while (OSTCBCur->C < C);
		OSSemPost(xhl);
		end = OSTimeGet();
		toDly = OSTCBCur->T - end;
		OSTCBCur->C = 0;
		OSTCBCur->T += 10;
		OSTimeDly(toDly);
	}
}
