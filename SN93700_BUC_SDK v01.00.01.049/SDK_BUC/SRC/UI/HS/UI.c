/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		UI.c
	\brief		User Interface (for High Speed Mode)
	\author		Hanyi Chiu
	\version	0.11
	\date		2019/12/12
	\copyright	Copyright(C) 2019 SONiX Technology Co., Ltd. All rights reserved.
*/
//------------------------------------------------------------------------------
#include "UI.h"
#include "APP_HS.h"
#include "LCD.h"
#include "OSD.h"
#include "BUF.h"
#ifdef BSP_PWRCTRL_FUNC
#include "SADC.h"
#include "PDET.h"
#endif
#if (defined(BSP_SN93711_FHD_REC_RX_V4)||defined(BSP_D_SN93701_SSD2828_RX_V5))
#include "UI_BUCCU[HD].h"
#endif
osThreadId osUI_ThreadId;
osMessageQId UI_EventQueue;
osMessageQId *pAPP_MessageQH;
static void UI_Thread(void const *argument);
static void UI_EventThread(void const *argument);

uint8_t isHeatingCamAndOnline[4] = {0, 0, 0, 0};
uint8_t is_heating[4] = {0, 0, 0, 0};
uint8_t heating_level[4] = {0, 0, 0, 0};
uint16_t HeatingYesNoPos[4][2] = {{0, 0}};	// pos_x, pos_y
uint16_t HeatingPageItemPos[16][4] = {{0, 0, 0, 0}};  // pos_x, pos_y, width, height

//------------------------------------------------------------------------------
void UI_Init(osMessageQId *pvMsgQId)
{
	UI_StateReset();
	pAPP_MessageQH = pvMsgQId;
    osMessageQDef(UI_EventQueue, UI_Q_SIZE, UI_Event_t);
    UI_EventQueue = osMessageCreate(osMessageQ(UI_EventQueue), NULL);
	KEY_Init(&UI_EventQueue);
#ifdef BSP_PWRCTRL_FUNC
	PDET_Init();
#endif
	UI_1MSTimerInit();
	osThreadDef(UI_EventThread, UI_EventThread, THREAD_PRIO_UIEVENT_HANDLER, 1, THREAD_STACK_UIEVENT_HANDLER);
	osThreadCreate(osThread(UI_EventThread), NULL);
	KNL_SetBbFrmMonitCbFunc(UI_FrameTRXFinish);
}
//------------------------------------------------------------------------------
#define OSD_WINSHIFT_PIXEL	0
uint32_t ulUI_BufSetup(uint32_t ulBUF_StartAddr)
{
	uint32_t ulUI_BufSize = 0;
#ifdef BUC_CU
	#if (defined(BSP_SN93711_FHD_REC_RX_V4)||defined(BSP_D_SN93701_SSD2828_RX_V5))
	ulUI_BufSize = ulOSD_CalBufSize(HD_WIDTH, HD_HEIGHT);
	#endif
	#if (defined(BSP_D_SNCC71_GM8285C_RX_V2))
	ulUI_BufSize = ulOSD_CalBufSize((WSVGA_WIDTH - (OSD_WINSHIFT_PIXEL * 2)), WSVGA_HEIGHT);
	#elif (defined(BSP_D_SNCC71_TP2915_AHD_RX_V3))
	ulUI_BufSize = ulOSD_CalBufSize(uwOSD_GetOsdImgHsize(), uwOSD_GetOsdImgVsize());
	#endif
	OSD_SetOsdBufAddr(ulBUF_StartAddr);
	printd(DBG_Debug2Lvl, "OSD Buffer Addr 0x%X[0x%X]\n", ulBUF_StartAddr, ulUI_BufSize);
#endif
	return ulUI_BufSize;
}
//------------------------------------------------------------------------------
void UI_PlugIn(void)
{
#ifdef BUC_CU
	LCD_Init(LCD_LCD_PANEL);
	LCD_SetLcdBufAddr(ulBUF_GetBlkBufAddr(0, BUF_LCD_IP));	
    #if (defined(BSP_SN93711_FHD_REC_RX_V4)||defined(BSP_D_SN93701_SSD2828_RX_V5))
	tOSD_Init(OSD_WEIGHT_8DIV8, uwLCD_GetLcdHoSize(), uwLCD_GetLcdVoSize(), 0, 0, OSD_SCALE_1X, OSD_SCALE_1X);
	#endif
	#if (defined(BSP_D_SNCC71_GM8285C_RX_V2))
	tOSD_Init(OSD_WEIGHT_8DIV8, (uwLCD_GetLcdHoSize() - (OSD_WINSHIFT_PIXEL * 2)), uwLCD_GetLcdVoSize(), OSD_WINSHIFT_PIXEL, 0, OSD_SCALE_1X, OSD_SCALE_1X);
	#elif (defined(BSP_D_SNCC71_TP2915_AHD_RX_V3))
	{
		uint8_t ubX, ubY;
		UI_GetOsdSft(&ubX, &ubY);
		tOSD_Init(OSD_WEIGHT_8DIV8, uwOSD_GetOsdImgHsize(), uwOSD_GetOsdImgVsize(),
			 uwLCD_GetLcdHoSize() - uwOSD_GetOsdImgHsize() - ubX, ubY, OSD_SCALE_1X, 
																	   OSD_SCALE_1X);
	}
	#endif
	UI_OnInitDialog();
#endif	//! End of #ifdef BUC_CU
	osThreadDef(UI_Thread, UI_Thread, osPriorityBelowNormal, 1, THREAD_STACK_UI_HANDLER);
	osUI_ThreadId = osThreadCreate(osThread(UI_Thread), NULL);
#ifdef BSP_PWRCTRL_FUNC	
	osThreadDef(UI_PwrCtrlThread, UI_PwrCtrlThread, THREAD_PRIO_UI_PWRCTRL_HANDLER, 1, THREAD_STACK_UI_PWRCTRL_HANDLER);
	osThreadCreate(osThread(UI_PwrCtrlThread), NULL);
#endif
}
//------------------------------------------------------------------------------
static void UI_Thread(void const *argument)
{
	static uint16_t uwUI_TaskCnt = 0;
	while(1)
	{	
		extern osSemaphoreId osUI_UpdateStatusCtr;
		osSemaphoreWait(osUI_UpdateStatusCtr, osWaitForever);
		UI_UpdateStatus(&uwUI_TaskCnt);
		osSemaphoreRelease(osUI_UpdateStatusCtr);
		osDelay(UI_TASK_PERIOD);
	}
}
//------------------------------------------------------------------------------
static void UI_EventThread(void const *argument)
{
	UI_Event_t tUI_Event;
	while(1)
	{
        osMessageGet(UI_EventQueue, &tUI_Event, osWaitForever);
		UI_EventHandles(&tUI_Event);
	}
}
//------------------------------------------------------------------------------
#ifdef BSP_PWRCTRL_FUNC
#define ACTIVE_CD_TH			30	//Active Count-Down Threshold, 30*50ms = 1.5 Sec
void UI_ReGetPwrActionNoRec(void)
{
	printf("Re-Get Power(No Rec)\r\n");	
	SYS_KEEP_PWR;


}

void UI_ReGetPwrActionWithRec(void)
{
	printf("Re-Get Power(With Rec)\r\n");	
	SYS_KEEP_PWR;

	UI_VideoRecordingExec(UI_REC_START);
}
//------------------------------------------------------------------------------
void UI_LostPwrAction(void)
{
	UI_RecPlayAct_t tRecPlayAct;	
	uint8_t ubMaxWaitTh = 70;	//70*50ms = 3.5 Sec
	uint8_t ubMaxWaitCnt = 0;	

	printf("Lost Power\r\n");
	

	RTC_WriteUserRam(RTC_RECORD_PWRSTS_ADDR, RTC_PWRSTS_KEEP_TAG);
	
	tRecPlayAct = tUI_GetRecPlayState();
	if(UI_REC_START == tRecPlayAct.tRecAct)
	{		
		printf("Stop Rec\r\n");
		UI_VideoRecordingExec(UI_REC_STOP);
		
		while(1)
		{			
			ubMaxWaitCnt++;
			
			if(REC_UpdateChkFinish())
			{
				printf("Release Pwr\r\n");								
				SYS_RELEASE_PWR;
				break;					
			}
			else
			{
				if(ubMaxWaitCnt >= ubMaxWaitTh)
				{
					printf("Over Max.Wait Time\r\n");
					SYS_RELEASE_PWR;
					break;
				}
			}
			osDelay(50);						
		}				
	}
	else	
	{
		printf("Release Pwr\r\n");		
		SYS_RELEASE_PWR;
	}	
}
//------------------------------------------------------------------------------
static void UI_PwrCtrlThread(void const *argument)
{
	UI_RecPlayAct_t tRecPlayAct;	
	uint8_t ubActiveCnt = 0;
	uint8_t ubPreRecFlg = 0;	

	while(1)
	{		
		if((ubActiveCnt == 0) && (GET_PWR_REPORT <= LOW_PWR_TH))		
		{			
			ubActiveCnt = ACTIVE_CD_TH;				
			
			tRecPlayAct = tUI_GetRecPlayState();
			if(UI_REC_START == tRecPlayAct.tRecAct)
				ubPreRecFlg = 1;
			else
				ubPreRecFlg = 0;	
		
			UI_LostPwrAction();			
		}	
		else if(ubActiveCnt)
		{			
			ubActiveCnt--;
			if((ubActiveCnt == 0) && (GET_PWR_REPORT > LOW_PWR_TH))									//Count down is finish
			{				
				if(ubPreRecFlg)
				{					
					UI_ReGetPwrActionWithRec();					
				}
				else
				{					
					UI_ReGetPwrActionNoRec();					
				}				
				ubPreRecFlg = 0;	//Reset								
			}					
			else if((ubActiveCnt >= 1) && (GET_PWR_REPORT > LOW_PWR_TH) && REC_UpdateChkFinish())	//Count down is not finish. Need to Check "Close file is done"
			{				
				ubActiveCnt = 0;
				
				if(ubPreRecFlg)
				{					
					UI_ReGetPwrActionWithRec();				
				}
				else
				{					
					UI_ReGetPwrActionNoRec();					
				}
				
				ubPreRecFlg = 0;	//Reset									
			}			
		}
		osDelay(UI_PWRCTRL_TASK_PERIOD);
	}
}
#endif
//------------------------------------------------------------------------------
osMessageQId *pUI_GetEventQueueHandle(void)
{
	return &UI_EventQueue;
}
//------------------------------------------------------------------------------
void UI_SendMessageToAPP(void *pvMessage)
{
	// 强制转换 void* 到 APP_EventMsg_t* 并验证
    APP_EventMsg_t *pEventMsg = (APP_EventMsg_t *)pvMessage;
//	printf("pvMessage event = %d\n", pEventMsg->ubAPP_Event);
	if(osMessagePut(*pAPP_MessageQH, pvMessage, 0) != osOK)
	{
		
		printd(DBG_ErrorLvl, "APP Q Full pvMessage event = %d\n", pEventMsg->ubAPP_Event);
	}
}
//------------------------------------------------------------------------------
void UI_StopUpdateThread(void)
{
	if((NULL != osUI_ThreadId) && (osOK != osThreadIsSuspended(osUI_ThreadId)))
		osThreadSuspend(osUI_ThreadId);
}
//------------------------------------------------------------------------------
void UI_StartUpdateThread(void)
{
	if((NULL != osUI_ThreadId) && (osOK == osThreadIsSuspended(osUI_ThreadId)))
		osThreadResume(osUI_ThreadId);
}
//------------------------------------------------------------------------------
void UI_FrameTRXFinish(uint8_t ubFrmRpt)
{
	switch(ubFrmRpt)
	{
	#ifdef BUC_CU
		case 0x0:
		case 0x1:
		case 0x2:
		case 0x3:
	#endif
	#ifdef BUC_CAM
		case 0xF:
	#endif
//			SIGNAL_LED_IO(!GET_SIGNAL_LED_IO);
			break;
		default:
//			SIGNAL_LED_IO(0);
			break;
	}
}
//------------------------------------------------------------------------------
#if (defined(BUC_CU)&&(APP_DOORPHONE_ENABLE==1))
void UI_DP_Init(uint8_t *st)
{
    UI_DP_SycAPPStatus(st);
}
#endif

