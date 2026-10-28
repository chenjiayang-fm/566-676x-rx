/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		UI.h
	\brief		User Interface Header file (for High Speed Mode)
	\author		Hanyi Chiu
	\version	0.11
	\date		2019/12/12
	\copyright	Copyright (C) 2019 SONiX Technology Co., Ltd. All rights reserved.
*/
//------------------------------------------------------------------------------
#ifndef _UI_HS_H_
#define _UI_HS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "BSP.h"
#include "KEY.h"
#include "APP_CFG.h"
#include "PROFILE_API.h"

#define 	UI_Q_SIZE				30
#define 	UI_TASK_PERIOD			200 					//! Unit: ms	
#define 	UI_PWRCTRL_TASK_PERIOD	50						//! Unit: ms, Power Control Task Period
#define		UI_ENABLE				TRUE
#define		UI_DISABLE				FALSE

#define		UI_AUTO_PAIR_START_TIME	4000		
typedef void (*pvKeyEventFunc)(void);

typedef enum
{
	rUI_FAIL,
	rUI_SUCCESS,
}UI_Result_t;

typedef enum
{
	CAM1,
	CAM2,
	CAM3,
	CAM4,
	CAM_2T = 2,
	CAM_4T = 4,
	NO_CAM = 0xF,
}UI_CamNum_t;

typedef enum
{
	CAM_OFFLINE,
	CAM_ONLINE,
	CAM_STSMAX,
}UI_CamConnectStatus_t;

typedef enum
{
	PKEY_EVENT,
	AKEY_EVENT,
	GKEY_EVENT,	
	SCANMODE_EVENT,
	FWUPG_EVENT,
	IRKEY_EVENT,//20201221
	TOUCH_EVENT,
	TRIGGER_EVENT,
	PELCO_ZOOM_EVENT,
	MENUOFF_EVENT,
	GESTURE_EVENT,
	REBOOT_EVENT,
	EVENT_NONE,
}UI_EventType_t;

typedef enum
{
	SINGLE_VIEW = 1,
	SCAN_VIEW,	
	DUAL_VIEW,
	QUAD_VIEW,
	H_VIEW,
	V3_2T1B_VIEW,	//Number of Video Stream : 3, 2 Top, 1 Bottom
	V3_3T_VIEW,		//Number of Video Stream : 3, 3 Top
}UI_CamViewType_t;

typedef enum
{
	//! Qual View
	DISP_UPPER_LEFT = 0,
	DISP_UPPER_RIGHT,
	DISP_LOWER_LEFT,
	DISP_LOWER_RIGHT,
	//! Dual View
	DISP_LEFT,
	DISP_RIGHT,
	//! Single View
	DISP_1T,
	//! H View
	DISP_H_L,
	DISP_H_R,
	DISP_H_CU,
	DISP_H_CL,
	
	//! Limit
	DISP_LOCATION_TYPE_MAX
}UI_DisplayLocation_t;

typedef enum
{
	PS_VOX_MODE,
	PS_ADOONLY_MODE,
	PS_WOR_MODE,	
	PS_ECO_MODE,
	POWER_NORMAL_MODE = 8,
}UI_PowerSaveMode_t;

typedef enum
{
	VOL_LVL0,
	VOL_LVL1,
	VOL_LVL2,
	VOL_LVL3,
	VOL_LVL4,
	VOL_LVL5,
}UI_VolumeLvl_t;

typedef enum
{
	CAMS_1T30_BWMODE,
	CAMS_1T60_BWMODE,
	CAMS_2T_BWMODE,
	CAMS_4T_BWMODE,
	CAMS_BWMODE_MAX,
}UI_CamsBwMode_t;

typedef enum
{
#define OSDLOGOPOOL(idx,addr)  OSDLOGO_##idx,
#if (defined(BSP_D_SNCC71_TP2915_AHD_RX_V3))			
#include "OSDLogo_Table_AHD.h"		
#else
#include "OSDLogo_Table.h"
#endif	
	OSDLOGO_MAX
}UI_OSDLogoPool_t;

typedef enum
{
#define OSD1IMGPOOL(idx,addr,posx,posy)  OSD1IMG_##idx,
#if (defined(BSP_SN93711_FHD_REC_RX_V4)||defined(BSP_D_SN93701_SSD2828_RX_V5))
#include "OSD1Image_Table[HD].h"
#endif
#ifdef BSP_D_SNCC71_GM8285C_RX_V2
#include "OSD1Image_Table[WSVGA].h"
#endif
#ifdef BSP_D_SNCC71_TP2915_AHD_RX_V3
#include "OSD1Image_Table[AHD].h"
#endif
	OSD1IMG_MAX
}UI_OSD1ImagePool_t;

typedef enum
{
#define OSD2IMGPOOL(idx,addr,posx,posy)  OSD2IMG_##idx,
#if (defined(BSP_SN93711_FHD_REC_RX_V4)||defined(BSP_D_SN93701_SSD2828_RX_V5))
#include "OSD2Image_Table[HD].h"
#endif
#ifdef BSP_D_SNCC71_GM8285C_RX_V2
#include "OSD2Image_Table[WSVGA].h"
#endif
	OSD2IMG_MAX
}UI_OSD2ImagePool_t;

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1) /* set alignment to 1 byte boundary */
typedef struct
{
	UI_EventType_t 	tEventType;
	void 	    	*pvEvent;
}UI_Event_t;

typedef struct
{
	uint8_t ubKeyID;
	uint8_t uwKeyCnt;					//!< Periodic: 100ms
	pvKeyEventFunc KeyEventFuncPtr;
	void (*pvKeyTone)(void);
}UI_KeyEventMap_t;
#pragma pack(pop)

//------------------------------------------------------------------------------
void UI_Init(osMessageQId *pvMsgQId);
uint32_t ulUI_BufSetup(uint32_t ulBUF_StartAddr);
void UI_PlugIn(void);
osMessageQId *pUI_GetEventQueueHandle(void);
void UI_StopUpdateThread(void);
void UI_StartUpdateThread(void);
void UI_SendMessageToAPP(void *pvMessage);
//------------------------------------------------------------------------------
void UI_1MSTimerInit(void);
void UI_OnInitDialog(void);
void UI_StateReset(void);
void UI_UpdateFwUpgStatus(void *ptUpgStsReport);
void UI_UpdateAppStatus(void *ptAppStsReport);
void UI_UpdateStatus(uint16_t *pThreadCnt);
void UI_EventHandles(UI_Event_t *ptEventPtr);
APP_EventMsg_t *tUI_ViewTypeSetup(UI_CamViewType_t *tViewType);
void UI_FrameTRXFinish(uint8_t ubFrmRpt);
void UI_GetOsdSft (uint8_t *pubX, uint8_t *pubY);
void UI_PairingKey(void);
uint32_t UI_GetBackLightPwm(void);
uint32_t UI_GetVolumePwm(void);


#ifdef BSP_PWRCTRL_FUNC
void UI_LostPwrAction(void);
void UI_ReGetPwrActionNoRec(void);
void UI_ReGetPwrActionWithRec(void);
static void UI_PwrCtrlThread(void const *argument);
#endif
//------------------------------------------------------------------------------
#if (defined(BUC_CU)&&(APP_DOORPHONE_ENABLE==1))
void UI_DP_Init(uint8_t *st);
#endif
#endif

