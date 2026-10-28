/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		UI_BUCCU.c
	\brief		User Interface of BUC Center Unit (for High Speed Mode)
	\author		Hanyi Chiu
	\version	1.3
	\date		2020/06/24
	\copyright	Copyright (C) 2020 SONiX Technology Co., Ltd. All rights reserved.
*/
//------------------------------------------------------------------------------
#include "BSP.h"

#ifdef BSP_D_SNCC71_GM8285C_RX_V2

#include <string.h>
#include "UI_BUCCU[WSVGA].h"
#include "SF_API.h"
#include "EN_API.h"
#include "FWU_API.h"
#include "TIMER.h"
#include "VDO.h"
#include "Buzzer.h"
#include "PLY_API.h"
#include "REC_API.h"
#include "RTC676x_API.h"

#define osUI_SIGNALS	0x66

extern uint8_t isHeatingCamAndOnline[4];
extern uint8_t is_heating[4];
extern uint8_t heating_level[4];

/**
 * Key event mapping table
 *
 * @param ubKeyID  			Key ID
 * @param ubKeyCnt 			Key count	(100ms per 1 count, ex.long press 5s, the count set to 50)
 * @param KeyEventFuncPtr 	Key event mapping to function
 */
const UI_KeyEventMap_t UiKeyEventMap[] =
{
	{NULL,				0,			NULL,						NULL},
	{AKEY_MENU, 		0,			UI_MenuKey, 				NULL},
	{AKEY_LEFT, 		0,			UI_LeftArrowKey,			NULL},
	{AKEY_RIGHT,		0,			UI_RightArrowKey,			NULL},
	{AKEY_OK,			0,			UI_EnterKey,				NULL},
	{AKEY_SEL,			0,			UI_SelKey,					NULL},
	{AKEY_POWER,		0,			UI_PowerKey,				NULL},
	{AKEY_POWER,		20, 		UI_PowerLongKey,			NULL},
	{AKEY_SEL,			20,			UI_SelLongKeyinZoom,		NULL},
	//20201222 IR
	{REMO_MENU, 		0,			UI_MenuKey, 				NULL},
	{REMO_OK,			0,			UI_EnterKey,				NULL},
	{REMO_LEFT, 		0,			UI_LeftArrowKey,			NULL},
	{REMO_RIGHT,		0,			UI_RightArrowKey,			NULL},
	{REMO_POWER,		0,			UI_PowerKey,				NULL},
	{REMO_PAIRING,		0,			UI_PairingKey,				NULL},
	{REMO_SEL,			0,			UI_SelKey,					NULL},
	{REMO_MIRROR,		0,			UI_MirrorKey,				NULL},
	{REMO_MUTE, 		0,			UI_MuteKey, 				NULL},
	{REMO_VERSION, 		0,			UI_VersionKey, 				NULL},
	{REMO_POWER,        20,         UI_PowerLongKey,            NULL},
	{REMO_SEL,			20,			UI_SelLongKeyinZoom,		NULL},
};
UI_State_t tUI_State;
APP_State_t tUI_SyncAppState;
UI_CamStatus_t tUI_CamStatus[CAM_4T];
UI_CUSetting_t tUI_CuSetting;
const static UI_MenuFuncPtr_t tUI_StateMap2MenuFunc[UI_STATE_MAX] =
{
	[UI_DISPLAY_STATE]				= UI_DisplayArrowKeyFunc,
	[UI_MAINMENU_STATE] 			= UI_HomeMenu_Key,
	[UI_SUBMENU_STATE]				= UI_SubKeyMenu,
	[UI_SUBSUBMENU_STATE]			= UI_SubSubKeyMenu,
	[UI_SUBSUBSUBMENU_STATE]		= UI_SubSubSubKeyMenu,
	[UI_SUBSUBSUBSUBMENU_STATE] 	= UI_SubSubSubSubKeyMenu,
	[UI_SET_VOLUME_STATE]			= UI_VolumeMenuPage,
	[UI_SET_CUPSMODE_STATE] 		= NULL,
	[UI_SET_CAMECOMODE_STATE]		= NULL,
	[UI_CAMSETTINGMENU_STATE]		= NULL, 
	[UI_SET_CAMCOLOR_STATE] 		= NULL,//UI_SUBSUBMENU_STATE
	[UI_DPTZ_CONTROL_STATE] 		= NULL,
	[UI_MD_WINDOW_STATE]			= NULL, 
	[UI_PAIRING_STATE]				= UI_PairingControl,	
	[UI_SDFWUPG_STATE]				= NULL,
	
	[UI_RECFOLDER_SEL_STATE]		= NULL,//UI_SUBMENU_STATE
	[UI_RECFILES_SEL_STATE] 		= NULL,//UI_SUBSUBMENU_STATE
	[UI_RECPLAYLIST_STATE]			= NULL,//UI_SUBSUBSUBMENU_STATE
	[UI_RECPLAYADOSRC_SEL_STATE]	= NULL,
	[UI_RECPLAYDISPTYPE_SEL_STATE]	= UI_PlayDispTypeSelection,

	[UI_PHOTOPLAYNRDY_STATE]		= NULL,
	[UI_PHOTOPLAYLIST_STATE]		= NULL,
	[UI_SDCARDFMT_STATE]			= NULL,//UI_SUBSUBMENU_STATE
	[UI_ENGMODE_STATE]				= UI_EngModeCtrl,
	[UI_SHOW_SDCARD_INFO_STATE] 	= UI_ShowSdCardInfo,
	[UI_SHOW_TX_VERSION_STATE] 		= UI_ShowTxVersion,
	[UI_SET_ZOOM_CAMERA_STATE]      = UI_ZoomSettingCamera,
	[UI_ADJUST_ZOOM_AREA_STATE]		= UI_AdjustZoomTraceArea,
	[UI_ADJUST_ZOOM_BSD_AREA_STATE] = UI_AdjustZoomBSDArea,
};


//-----------------------------------------------TOUCH FUNCTION 20201230-----------------------------------------------------
const static UI_MenuTouchFuncPtr_t tUI_StateMap2TouchMenuFunc[UI_STATE_MAX] =
{
	[UI_DISPLAY_STATE]				= UI_DisplayTouchFunc,
	[UI_MAINMENU_STATE] 			= UI_HomeMenuTouchFunc,
	[UI_SUBMENU_STATE]				= UI_SubTouchMenu,
	[UI_SUBSUBMENU_STATE]			= UI_SubSubTouchMenu,
	[UI_SUBSUBSUBMENU_STATE]		= UI_SubSubSubTouchMenu,
	[UI_SUBSUBSUBSUBMENU_STATE] 	= UI_SubSubSubSubTouchMenu,
	[UI_SET_VOLUME_STATE]			= UI_VolumeMenuTouchFunc,
	[UI_SET_CUPSMODE_STATE] 		= NULL,
	[UI_SET_CAMECOMODE_STATE]		= NULL,
	[UI_CAMSETTINGMENU_STATE]		= NULL, 
	[UI_SET_CAMCOLOR_STATE] 		= NULL,
	[UI_DPTZ_CONTROL_STATE] 		= NULL,
	[UI_MD_WINDOW_STATE]			= NULL, // UI_MD_Window
	[UI_PAIRING_STATE]				= UI_PairingTouchControl,
	[UI_SDFWUPG_STATE]				= NULL,
	[UI_RECFOLDER_SEL_STATE]		= NULL,
	[UI_RECFILES_SEL_STATE] 		= NULL,
	[UI_RECPLAYLIST_STATE]			= NULL,
	[UI_RECPLAYADOSRC_SEL_STATE]	= NULL,
	[UI_RECPLAYDISPTYPE_SEL_STATE]	= UI_PlayDispTypeTouchSelection,
	[UI_PHOTOPLAYNRDY_STATE]		= NULL,
	[UI_PHOTOPLAYLIST_STATE]		= NULL,
	[UI_SDCARDFMT_STATE]			= NULL,
	[UI_ENGMODE_STATE]				= NULL,
	[UI_SHOW_SDCARD_INFO_STATE] 	= UI_ShowSdCardInfoTouchFunc,
	[UI_SHOW_TX_VERSION_STATE] 		= UI_ShowTxVersionTouchFunc,
	[UI_SET_ZOOM_CAMERA_STATE]      = UI_ZoomSettingCameraTouchFunc,
	[UI_ADJUST_ZOOM_AREA_STATE]		= UI_AdjustZoomAreaTouchFunc,
	[UI_ADJUST_ZOOM_BSD_AREA_STATE] = UI_AdjustZoomBSDAreaTouchFunc,
};

UI_MenuItem_t tUI_MenuItem;
//------------------------------------------------------------------------------sub
UI_SubMenuItem_t tUI_SubMenuItem[MENUITEM_MAX] =
{
	{0,	4	},			//ado sel
	{CAM1,VDO_VIEW_MAX	},		//vdo sel
	{PAIRINGCAM1_ITEM,	PAIRING_ITEM_MAX	},//pairing
	{NULL,				NULL},//record
	{NULL,				NULL			},//playback
	{CAMERASET_ITEM, 	SETTINGITEM_MAX},//system
	{0, 16}, // heating
	{POWER_SCREENOFF,	POWER_ITEM_MAX},//power
	{ZOOM_CAM1,		ZOOM_CAM_MAX},//ZOOM
};
//------------------------------------------------------------------------------sub sub 

UI_SubMenuItem_t tSystemSubSubMenuItem = 
{
	DIMMER_ITEM,SYSTEMITEM_MAX,{0,0}
};
UI_SubMenuItem_t tDualSubSubMenuItem = 
{
	DUAL_LEFT_CAM1,DUAL_MAX,{0,0}
};
UI_SubMenuItem_t tQuadSubSubMenuItem = 
{
	QUAD_UPPER_LEFT_CAM1,QUAD_MAX,{0,0}
};

UI_SubMenuItem_t tPowerOnSubSubMenuItem = 
{
	POWERON_CAM1,POWERON_MAX - 1,{0,0}
};

UI_SubMenuItem_t tZoomSettingCameraItem = 
{
	ZOOM_SLIDER,ZOOM_ITEM_MAX,{0,0}
};

UI_SubMenuItem_t tZoomVirtualKeyItem = 
{
	ZOOM_VIRTUAL_LEFT_KEY, ZOOM_VIRTUAL_KEY_MAX, {0,0}
};


UI_SettingSubSubMenuItem_t tSettingSubSubMenuItem =
{
	{
	   { CAM1, CAM_4T, { 0, 0 } },      		//camera 
	   { RECTIME_1MIN_ITEM, RECITEM_MAX, { 0, 0 } }, //record
	   { DIMMER_ITEM, SYSTEMITEM_MAX, { 0, 0 } },//system
	   { 0, 0, { 0, 0 } },		//dual
	   { 0, 0, { 0, 0 } }, //quad
	   { TRIGGER_DELAY_CAM1, TRIGGER_MAX, { 0, 0 } },	//trigger
	   { AUTOSCAN_ONOFF_CAM1, AUTOSCAN_MAX, { 0, 0 } },//auto scan
	   {0,0, { 0, 0 } },//power on
	   {PARKINGLINE_CAM1_ONOFF, PARKINGLINE_MAX, { 0, 0 } },//parking line
	   {PARKINGLINE_CAM1_ONOFF, PARKINGLINE_MAX, { 0, 0 } },//Guide line
	 },
};
//------------------------------------------------------------------------------sub sub sub
/********************camera************************/
uint8_t CameraSubSubSubRemoteMode = REMOTE_SELECT_ITEM;

static UI_SubMenuItem_t tCameraCam1MenuItem = 
{
	0,CAMERA_MAX,{0,0}
};
static UI_SubMenuItem_t tCameraCam2MenuItem = 
{
	0,CAMERA_MAX,{0,0}
};
static UI_SubMenuItem_t tCameraCam3MenuItem = 
{
	0,CAMERA_MAX,{0,0}
};
static UI_SubMenuItem_t tCameraCam4MenuItem = 
{
	0,CAMERA_MAX,{0,0}
};

UI_SubMenuItem_t *tCameraSubSubSubMenuItem[] = 
{
	&tCameraCam1MenuItem,
	&tCameraCam2MenuItem,
	&tCameraCam3MenuItem,
	&tCameraCam4MenuItem	
};

/********************system************************/
uint8_t SystemSubSubSubRemoteMode = REMOTE_SELECT_ITEM;

UI_SubMenuItem_t tSystemSubSubSubMenuItem[] = 
{
	{ 0, 0, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ DAY_SLIDER, DIMMER_SLIDER_MAX, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ SYSTEM_UPGRADE_YES, SYSTEM_UPGRADE_MAX, { 0, 0 } },
	{ LANGUAGE_ENGLISH, LANGUAGE_MAX, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ 0, 0, { 0, 0 } },
	{ DATE_TIME_YEAR, DATE_TIME_MAX, { 0, 0 } },
};
/********************setting************************/
uint8_t SettingSubSubSubRemoteMode = REMOTE_SELECT_ITEM;

//------------------------------------------------------------------------------sub sub sub sub
/********************playback************************/

UI_MenuItem_t tUI_RecPlayListItem = {UI_RECPLAYPAUSE_ITEM,UI_RECPLAYPAUSE_ITEM};

UI_MenuItem_t tUI_PlyDispTypeSelItem;

UI_MenuItem_t tUI_RecPlayAdoSrcItem;


/********************date time************************/
UI_SubMenuItem_t tDateTimeSubSubSubSubMenuItem = 
{
	KEYBOARD_KEY1,KEYBOARD_KEY_MAX,{0,0}
};


const UI_ReportFuncPtr_t tUiReportMap2Func[] =
{
	[UI_UPDATE_CAMSTS] 			= UI_UpdateCamStatus,
	[UI_VOX_TRIG]				= NULL,
	[UI_MD_TRIG]				= NULL,
	[UI_VOICE_TRIG]				= UI_VoiceTrigger,
};

ADO_R2R_VOL tUI_VOLTable[] = {R2R_VOL_n45DB, R2R_VOL_n32p4DB, R2R_VOL_n26p2DB, R2R_VOL_n21p4DB, R2R_VOL_n14p6DB, R2R_VOL_n8p2DB};

osSemaphoreId osUI_CuSemId;
osSemaphoreId osUI_CuUiCtr;//在显示SUB界面的时候，避免信号sd卡这些会动态刷新的图标影响
osSemaphoreId osUI_CuRecCtr;
osSemaphoreId osUI_CuTriggerCtr;
osSemaphoreId osUI_UpdateStatusCtr;

UI_CamViewSelect_t tCamViewSel;
UI_PairingInfo_t tPairInfo;
static UI_ThreadNotify_t tosUI_Notify;
UI_CamNum_t tUI_ViewModeSel;	//vdo mode界面的选项
UI_CamNum_t tUI_SaveViewNum;//保存点击单画面跳转的多分割画面

UI_CamNum_t tUI_AutoScanNumSel;//当前自动扫描扫到哪个画面

uint8_t MenuOnFlag = FALSE;//显示操作菜单标记
uint8_t TriggerLock = FALSE;//为TRUE表示当前正在触发状态
uint8_t TriggerSource = 0xff;//当前触发源
uint8_t FootSwitchLock = FALSE; //脚踏是否触发跳转


UI_CamNum_t DeskTopShowView;//DeskTop当前显示的画面，有了tUI_ViewNumSel之所以还需要该变量是因为tUI_ViewNumSel标记着当前选的view，不能变

static uint8_t ubCamDiscCount[4] = {0};//用来计数每个tx丢失连接的次数，只有计数值达到设定的才会刷no signal

UI_SdSts_t tUI_SdCardSts;
static uint8_t ubUI_CuStartUpFlag;


uint16_t uwUI_ViewSwitchTime;//画面切换耗时

uint8_t ubUI_FinishViewSwitch;//画面切换是否已经结束
uint8_t ubUI_RestartRec;//画面切换完是否重启录像
uint8_t ubUI_ReOpenPanel;//画面切换完是否打开屏幕


uint8_t ubUI_CuStandbyFlag;//待机状态
uint8_t ubUI_CuPowerOffFlag;//睡眠

uint8_t ubUI_CuPowerDiscFlag;//电源是否断开
uint8_t ubPelcoZoomStart = FALSE;//
uint8_t ubPelcoZoomFlag = FALSE;//
uint8_t ubPelcoZoomType = 0;// 
uint8_t ubZoomRestartFlag = FALSE;
uint8_t ubZoomPairingFlag = FALSE; //重新pairing后刷camera标志
uint8_t  ubisShowZoomflag = 0;
uint8_t ubisCameraConfigFlag = FALSE; //是否进入Camera设置


extern uint8_t curr_indx;

uint8_t ubUI_CurDimmerLvl;//当前背光亮度


static uint8_t ubUI_StopUpdateStsBarFlag;

UI_CharOsdImgDb_t tCharOsdImgDB;
UI_CharOsdImgDb_t tTransparentCharOsdImgDB;

uint32_t ulMenuAutoOffCount;//退出菜单倒计时
uint32_t ulAutoScanCount;//自动扫描画面倒计时
uint32_t ulPanelOffCount;//息屏倒计时
uint32_t ulTriggerOverCount;//触发结束倒计时
uint32_t ulStartRecCompleteCount;//启动录像完成倒计时
uint16_t uwCurVideoPlayIdx[4];//当前播放的视频
uint32_t ulPelcoZoomCount = 500;
uint32_t ulDigitalZoomCount = 200;
uint32_t ulZoomRestartCount;
uint32_t ubZoomPairingCount = 1000; //重新pairing后刷camera时间 ms

uint8_t ubAutoScanReady = TRUE;

UI_CamNum_t TriggerBackUp_View;

static uint16_t uwTxType[4] = {0};
static uint8_t ubTxTypeCount[4] = {0};
osMutexId osUI_TxTypeMutex;

uint8_t ubUI_TouchPanelSts = TRUE;
uint8_t isShowFoucsRestart = 0;
uint8_t isSingleZoomCamera = FALSE;

UI_ZoomVersionDate_t tZoomVersionDate[4] = {0};


//! Record
static void UI_OsdLoadingDisplayThread(void const *argument);
osThreadId osUI_OsdLdDispThdId;
osMessageQId osUI_OsdLdDispQueue;
osMessageQId osUI_RecRptQueue;
osSemaphoreId osUI_OsdLdStsUpdSem;
static UI_OsdLdDispSts_t tUI_OsdLdDispSts;
UI_RecFoldersInfo_t tUI_RecFoldersInfo;
UI_RecFilesInfo_t tUI_RecFilesInfo;
static UI_RecOsdImgDb_t tUI_RecOsdImgDB;
OSD_IMGIDXARRARY_t tUI_RecOsdImgInfo;
OSD_IMGIDXARRARY_t tUI_CharOsdImgInfo;
OSD_IMGIDXARRARY_t tUI_TransparentCharOsdImgInfo;
UI_RecPlayAct_t tUI_RecPlayAct;
static uint8_t ubUI_VdoRecChkFlag;
//! Functions Execute
static void UI_FuncsExecuteThread(void const *argument);
osThreadId osUI_FuncsExecThdId;
osMessageQId osUI_FuncsExecQue;
osMessageQId osUI_FuncsFinExecQue;
//! Performance Debug
#define MAX_DBG_ITEM	3
osMutexId osUI_PerDbgMutex;
static OSD_IMG_INFO tUI_PerDbgOsdImgInfo[17];
static uint32_t ulUI_FrameErrCnt[CAM_4T][MAX_DBG_ITEM];
static uint8_t ubUI_PerDebugEn;
#define UI_HVIEW_ICON_XOFFSET		756

char zoomchar[] = "-Z-";
char Laserchar[] = "LAS";
char heatingchar[] = "HLAS";	// including laser

extern uint16_t HeatingPageItemPos[16][4];
extern uint16_t HeatingYesNoPos[4][2];

/*PELCO code*/
                                                                               // sync  addr  cmd1  cmd2  data1 data2 checksum
uint8_t pelco_out[]  = {/*0xAA, 0xFF, 0xE9, 0x4F, 0x00, 0x04, 0x00, 0x00, 0x07,*/ 0xFF, 0x00, 0x00, 0x40, 0x00, 0x00, 0x40};
uint8_t pelco_in[]   = {/*0xAA, 0xFF, 0xC1, 0x51, 0x00, 0x04, 0x00, 0x00, 0x07,*/ 0xFF, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20};
uint8_t pelco_stop[] = {/*0xAA, 0xFF, 0xD9, 0x5B, 0x00, 0x04, 0x00, 0x00, 0x07,*/ 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t pelco_near[]    = {0xFF, 0x91, 0x00, 0x00, 0x00, 0x00, 0x91};  
uint8_t pelco_far[]     = {0xFF, 0x91, 0x00, 0x01, 0x00, 0x00, 0x92};
uint8_t pelco_auto[]    = {0xFF, 0x90, 0x00, 0x00, 0x00, 0x00, 0x90}; 
uint8_t pelco_restart[] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF};

uint8_t pelco_digital_zoom[7] = {0xFF, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t pelco_BSD[]   = {0xFF, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xA0, \
                         0xFF, 0xA0, 0x00, 0x01, 0x00, 0x00, 0xA1};
uint8_t pelco_Trace[] = {0xFF, 0xB0, 0x00, 0x00, 0x00, 0x00, 0xB0, \
	                     0xFF, 0xB0, 0x00, 0x01, 0x00, 0x00, 0xB1};
uint8_t pelco_Picture_Mirror_Flip[] = {0xFF, 0x46, 0x00, 0x00, 0x00, 0x00, 0x46};


uint8_t CheckSensorStatus[] = {0xFF, 0x41, 0x00, 0x00, 0x00, 0x00, 0x41};
uint8_t CheckAlarmRoi[]     = {0xFF, 0x43, 0x00, 0x00, 0x00, 0x00, 0x43};
uint8_t CheckPictureMirrorFlip[]     = {0xFF, 0x47, 0x00, 0x00, 0x00, 0x00, 0x47};








//------------------------------------------------------------------------------
//20201229
void UI_TouchEventExec(void *pvTouchEvent)
{
	TOUCH_EVENT_t *ptTouchEvent = (TOUCH_EVENT_t*)pvTouchEvent;

	if(tUI_StateMap2TouchMenuFunc[tUI_State].pvFuncPtr)
		tUI_StateMap2TouchMenuFunc[tUI_State].pvFuncPtr(ptTouchEvent);
}

//uint8_t isRespondRepeat = 0;
void UI_KeyEventExec(void *pvKeyEvent)
{
	static uint8_t ubUI_KeyEventIdx = 0;
	static uint8_t ubCount = 0;
	KEY_Event_t *ptKeyEvent;
	uint16_t uwUiKeyEvent_Cnt = 0, uwIdx;
	ptKeyEvent = (KEY_Event_t *)pvKeyEvent;
	uwUiKeyEvent_Cnt = sizeof UiKeyEventMap / sizeof(UI_KeyEventMap_t);
	if (TriggerLock || FootSwitchLock)
		return;

	if (tUI_CuSetting.ubKeyLock)
	{
	    if (!((ptKeyEvent->ubKeyAction == KEY_CNT_ACT) &&
	          (ptKeyEvent->uwKeyCnt == 20) &&
	          (ptKeyEvent->ubKeyID == AKEY_POWER || ptKeyEvent->ubKeyID == REMO_POWER)))
	    {
			if(!((ubUI_CuPowerOffFlag || (ubUI_CuStandbyFlag && (!ubUI_TouchPanelSts || tUI_CuSetting.ubMenuLock))) 
				&& (ptKeyEvent->ubKeyID == AKEY_POWER || ptKeyEvent->ubKeyID == REMO_POWER))) 
			{
		        return;
			}
	    }
	}

	//�̰�
	if(ptKeyEvent->ubKeyAction == KEY_UP_ACT)
	{
		if((ubUI_KeyEventIdx) && (ubUI_KeyEventIdx < uwUiKeyEvent_Cnt))
		{
			if(UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr)
			{
//				if ((ptKeyEvent->ubKeyID == AKEY_LEFT || ptKeyEvent->ubKeyID == AKEY_RIGHT || ptKeyEvent->ubKeyID == AKEY_OK))
//				{
//					isRespondRepeat = 0;
//				}
				
				#if 0 //防止按一次按键响应两次蜂鸣器
				if ((ptKeyEvent->ubKeyID == AKEY_POWER) && (!ubUI_CuStandbyFlag))
				{
					BUZZER_DI(150);
				}
				#endif
				
				//printf("%d exec key func\n", __LINE__);
				UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
				//取消变焦四个按钮的长按
				if(tUI_CamStatus[tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx].tCamConnSts == CAM_ONLINE \
				&& (ptKeyEvent->ubKeyID == AKEY_OK || ptKeyEvent->ubKeyID == REMO_OK) && ubisShowZoomflag && \
				(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_IN \
				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_OUT \
				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_NEAR \
				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_FAR )) //变焦操作OK键红外长按
				{
					UI_ZoomDrawSettingCameraItem();
					if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_IN || tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_OUT)
					{
						tTWC_Send(TWC_STA1 + tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
					}
				}
				if(UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone && tUI_CuSetting.ubBuzzer)
					UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone();
			}
		}
		ubUI_KeyEventIdx = 0;
		ubCount = 0;
		return;
	}

	//长按
	for(uwIdx = 1; uwIdx < uwUiKeyEvent_Cnt; uwIdx++)
	{
		//按压时间达到设定值就会进入，单点的设定值是0
		if((ptKeyEvent->ubKeyID  == UiKeyEventMap[uwIdx].ubKeyID) &&
		   (ptKeyEvent->uwKeyCnt == UiKeyEventMap[uwIdx].uwKeyCnt))
		{			
			ubUI_KeyEventIdx = uwIdx;
			if((ptKeyEvent->uwKeyCnt) && (UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr))//uwKeyCnt != 0, 长按在这里响应
			{
				UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
				if(UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone && tUI_CuSetting.ubBuzzer)
					UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone();
				ubUI_KeyEventIdx = 0;
			}
		}
	}
	//左右键、OK键连发
	if((ptKeyEvent->ubKeyID == AKEY_LEFT || ptKeyEvent->ubKeyID == AKEY_RIGHT || ptKeyEvent->ubKeyID == AKEY_OK \
		|| ptKeyEvent->ubKeyID == REMO_OK || ptKeyEvent->ubKeyID == REMO_LEFT || ptKeyEvent->ubKeyID == REMO_RIGHT) \
		&& SendIrCodeFlag == TRUE)
	{
		if(ubCount < 3)//延迟300ms再响应连发
			ubCount ++;
		else
		{
			for(uwIdx = 1; uwIdx < uwUiKeyEvent_Cnt; uwIdx++)
			{
				if(ptKeyEvent->ubKeyID	== UiKeyEventMap[uwIdx].ubKeyID)
				{
					if((ptKeyEvent->ubKeyID == AKEY_OK || ptKeyEvent->ubKeyID == REMO_OK) \
						&& (!ubisShowZoomflag || !(tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx == ZOOM_IN \
						|| tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx == ZOOM_OUT \
						|| tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx == ZOOM_NEAR \
						|| tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx == ZOOM_FAR \
						|| (tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx >= ZOOM_VIRTUAL_LEFT_KEY && tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx <= ZOOM_VIRTUAL_DOWN_KEY)))) //变焦操作OK键长按
					{
						break;
					}			
					if(UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr)
					{
//						isRespondRepeat = 1;
						UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
					}
				}
			}
		}
	}
}

//void UI_KeyEventExec(void *pvKeyEvent)
//{
//	static uint8_t ubUI_KeyEventIdx = 0;
//	static uint8_t ubCount = 0;
//	KEY_Event_t *ptKeyEvent;
//	uint16_t uwUiKeyEvent_Cnt = 0, uwIdx;
//	ptKeyEvent = (KEY_Event_t *)pvKeyEvent;
//	uwUiKeyEvent_Cnt = sizeof UiKeyEventMap / sizeof(UI_KeyEventMap_t);
//	if (TriggerLock)
//		return;
//	if(ptKeyEvent->ubKeyAction == KEY_UP_ACT)
//	{
//		if((ubUI_KeyEventIdx) && (ubUI_KeyEventIdx < uwUiKeyEvent_Cnt))
//		{
//			if(UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr)
//			{
//				if ((ptKeyEvent->ubKeyID == AKEY_LEFT || ptKeyEvent->ubKeyID == AKEY_RIGHT || ptKeyEvent->ubKeyID == AKEY_OK))
//				{
//					isRespondRepeat = 0;
//				}
//				//printf("%d exec key func\n", __LINE__);
//				if ((ptKeyEvent->ubKeyID == AKEY_POWER) && (!ubUI_CuStandbyFlag))
//				{
//					BUZZER_DI(150);
//				}
//				
//				UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
//				
//				//取消变焦四个按钮的长按
//				if(tUI_CamStatus[tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx].tCamConnSts == CAM_ONLINE \
//				&& (ptKeyEvent->ubKeyID == AKEY_OK || ptKeyEvent->ubKeyID == REMO_OK) && ubisShowZoomflag && \
//				(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_IN \
//				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_OUT \
//				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_NEAR \
//				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_FAR )) //变焦操作OK键红外长按
//				{
//					UI_ZoomDrawSettingCameraItem();
//				}
//				
//				if(UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone && tUI_CuSetting.ubBuzzer)
//					UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone();
//			}
//		}
//		//红外长按取消高亮
//		if(ptKeyEvent->uwKeyCnt == 1 && tUI_CamStatus[tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx].tCamConnSts == CAM_ONLINE \
//		&& ptKeyEvent->ubKeyID == REMO_OK && ubisShowZoomflag && \
//		(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_IN \
//		|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_OUT \
//		|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_NEAR \
//		|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_FAR ))
//		{
//			UI_ZoomDrawSettingCameraItem();
//		}
//		ubUI_KeyEventIdx = 0;
//		ubCount = 0;
//		return;
//	}
//	for(uwIdx = 1; uwIdx < uwUiKeyEvent_Cnt; uwIdx++)
//	{
//		//按压时间达到设定值就会进入，单点的设定值是0
//		if((ptKeyEvent->ubKeyID  == UiKeyEventMap[uwIdx].ubKeyID) &&
//		   (ptKeyEvent->uwKeyCnt == UiKeyEventMap[uwIdx].uwKeyCnt))
//		{			
//			ubUI_KeyEventIdx = uwIdx;
//			if((ptKeyEvent->uwKeyCnt) && (UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr))//长按在这里响应
//			{
//				UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
//				if(UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone && tUI_CuSetting.ubBuzzer)
//					UiKeyEventMap[ubUI_KeyEventIdx].pvKeyTone();
//				ubUI_KeyEventIdx = 0;
//			}
//		}
//		else if((ptKeyEvent->ubKeyID  == UiKeyEventMap[uwIdx].ubKeyID) && (ptKeyEvent->ubKeyID == REMO_OK) && ptKeyEvent->uwKeyCnt == 1) 
//		{
//			if(ubisShowZoomflag && (tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_IN \
//				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_OUT \
//				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_NEAR \
//				|| tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_FAR )) //变焦操作OK键红外长按
//			{
//				ubUI_KeyEventIdx = uwIdx;
//				if(UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr)
//				{
//					UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
//					printf("*****************************************11111111111111111111\n");
//				}
//				ubUI_KeyEventIdx = 0;
//			}
//		}
//	}
//	//左右键连发
//	if((ptKeyEvent->ubKeyID == AKEY_LEFT || ptKeyEvent->ubKeyID == AKEY_RIGHT || ptKeyEvent->ubKeyID == AKEY_OK) && SendIrCodeFlag == TRUE)
//	{
//		if(ubCount < 5)//延迟500ms再响应连发
//			ubCount ++;
//		else
//		{
//			for(uwIdx = 1; uwIdx < uwUiKeyEvent_Cnt; uwIdx++)
//			{
//				if(ptKeyEvent->ubKeyID	== UiKeyEventMap[uwIdx].ubKeyID)
//				{
//					if(ptKeyEvent->ubKeyID == AKEY_OK && (!ubisShowZoomflag || !(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_IN \
//					   || tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_OUT \
//					   || tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_NEAR \
//					   || tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_FAR \
//					   || (tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx >= ZOOM_VIRTUAL_LEFT_KEY && tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx <= ZOOM_VIRTUAL_DOWN_KEY)))) //变焦操作OK键长按
//					{
//						break;
//					}			
//					if(UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr)
//					{
//						isRespondRepeat = 1;
//						UiKeyEventMap[ubUI_KeyEventIdx].KeyEventFuncPtr();
//					}
//				}
//			}
//		}
//	}
//}

//------------------------------------------------------------------------------

void UI_1MsTimer(void)
{
	if(ulMenuAutoOffCount && (tUI_State != UI_PAIRING_STATE) 
		&& !((tUI_State == UI_SUBSUBSUBMENU_STATE || tUI_State == UI_SUBSUBSUBSUBMENU_STATE) && tUI_MenuItem.ubItemIdx == PLAYBACK_ITEM))
	{
		ulMenuAutoOffCount --;
	}
	
	if(ulAutoScanCount && !MenuOnFlag && !ubUI_CuStandbyFlag && !TriggerLock && !ubUI_CuPowerDiscFlag)
	{
		ulAutoScanCount --;
	}
	if(ulPanelOffCount && !MenuOnFlag && !TriggerLock)
	{
		ulPanelOffCount --;
	}
	if(ulTriggerOverCount && TriggerLock && !MenuOnFlag)
	{
		ulTriggerOverCount --;
	}

	if(ulPelcoZoomCount && ubPelcoZoomFlag)
	{
		ulPelcoZoomCount --;
	}
	
	if(ubZoomPairingFlag && ubZoomPairingCount)
	{
		ubZoomPairingCount --;
	}

	if (ulDigitalZoomCount)
	{
		ulDigitalZoomCount--;
	}

	if(ulStartRecCompleteCount)
	{
		ulStartRecCompleteCount --;
		if(ulStartRecCompleteCount == 0)
		osSemaphoreRelease(osUI_CuRecCtr);
	}
	
	if((ubUI_FinishViewSwitch == FALSE) && (tUI_RecPlayAct.tRecAct == UI_REC_STOP))
	{
		uwUI_ViewSwitchTime ++;
		if(uwUI_ViewSwitchTime >= 5000)//通道切换时间如果超过5s就重启
		{
			UI_Event_t tRebootEvent;
			osMessageQId *pUI_RebootEventQH = NULL;
			
			tRebootEvent.tEventType = REBOOT_EVENT;
			tRebootEvent.pvEvent 	  = NULL;
			pUI_RebootEventQH 	  = pUI_GetEventQueueHandle();
			osMessagePut(*pUI_RebootEventQH, &tRebootEvent, 0);
			
		}
	}
	else
		uwUI_ViewSwitchTime = 0;
}

//------------------------------------------------------------------------------
void UI_1MSTimerInit(void)
{
	TIMER_SETUP_t TmSetup;

	TmSetup.tCLK 		= TIMER_CLK_EXTCLK;
	TmSetup.ulTmLoad 	= 10000;
	TmSetup.ulTmCounter = TmSetup.ulTmLoad;
	TmSetup.ulTmMatch1 	= TmSetup.ulTmLoad + 1;
	TmSetup.ulTmMatch2 	= TmSetup.ulTmLoad + 1;
	TmSetup.tOF 		= TIMER_OF_ENABLE;
	TmSetup.tDIR 		= TIMER_DOWN_CNT;
	TmSetup.tEM 		= TIMER_CB;
	TmSetup.pvEvent 	= UI_1MsTimer;
	TIMER_Start(TIMER2_1, TmSetup);
}

//------------------------------------------------------------------------------
void UI_OnInitDialog(void)
{
	if(NULL == osUI_OsdLdDispQueue)
	{
		osMessageQDef(UiLdDispQue, 10, uint16_t);
		osUI_OsdLdDispQueue = osMessageCreate(osMessageQ(UiLdDispQue), NULL);
	}
	if(NULL == osUI_RecRptQueue)
	{
		osMessageQDef(UiRecRptQue, 1, uint8_t);
		osUI_RecRptQueue = osMessageCreate(osMessageQ(UiRecRptQue), NULL);
	}
	if(NULL == osUI_OsdLdDispThdId)
	{
		osThreadDef(UiOsdLdDisp, UI_OsdLoadingDisplayThread, THREAD_PRIO_UIEVENT_HANDLER, 1, 512);
		osUI_OsdLdDispThdId = osThreadCreate(osThread(UiOsdLdDisp), NULL);
	}
	if(NULL == osUI_FuncsExecQue)
	{
		osMessageQDef(UiFuncExecQue, 300, UI_FuncExecMsg_t);
		osUI_FuncsExecQue = osMessageCreate(osMessageQ(UiFuncExecQue), NULL);
	}
	if(NULL == osUI_FuncsFinExecQue)
	{
		osMessageQDef(UiFuncsFinExecQue, 1, uint8_t);
		osUI_FuncsFinExecQue = osMessageCreate(osMessageQ(UiFuncsFinExecQue), NULL);
	}
	if(NULL == osUI_FuncsExecThdId)
	{
		osThreadDef(UiFuncsExec, UI_FuncsExecuteThread, osPriorityNormal, 1, 8192);
		osUI_FuncsExecThdId = osThreadCreate(osThread(UiFuncsExec), NULL);
	}
	MenuOnFlag = FALSE;

	tCamViewSel.tCamViewType	= (VDO_DISP_TYPE == KNL_DISP_H)?H_VIEW:
								  (VDO_DISP_TYPE == KNL_DISP_QUAD)?QUAD_VIEW:
								  ((VDO_DISP_TYPE == KNL_DISP_DUAL_C) || (VDO_DISP_TYPE == KNL_DISP_DUAL_U))?DUAL_VIEW:SINGLE_VIEW;
	tCamViewSel.tCamViewPool[0] = ((VDO_DISP_TYPE == KNL_DISP_QUAD) || (VDO_DISP_TYPE == KNL_DISP_H))?CAM_4T:CAM1;
	tCamViewSel.tCamViewPool[1] = ((VDO_DISP_TYPE == KNL_DISP_DUAL_C) || (VDO_DISP_TYPE == KNL_DISP_DUAL_U))?CAM2:NO_CAM;


}

static void UI_RecvCamHeatingStatusReport(TWC_TAG tRecv_StaNum, uint8_t *pTwc_Data);
//------------------------------------------------------------------------------
void UI_StateReset(void)
{
	osSemaphoreDef(UiCuSem);
	osUI_CuSemId = osSemaphoreCreate(osSemaphore(UiCuSem), 1);
	
	osSemaphoreDef(UiCuRecCtr);
	osUI_CuRecCtr = osSemaphoreCreate(osSemaphore(UiCuRecCtr), 1);
	
	osSemaphoreDef(UiCuTriggerCtr);
	osUI_CuTriggerCtr = osSemaphoreCreate(osSemaphore(UiCuTriggerCtr), 1);
	
	osSemaphoreDef(UiCuUiCtr);
	osUI_CuUiCtr = osSemaphoreCreate(osSemaphore(UiCuUiCtr), 1);

	osSemaphoreDef(UI_UpdateStatusCtr);
	osUI_UpdateStatusCtr = osSemaphoreCreate(osSemaphore(UI_UpdateStatusCtr), 1);

	osSemaphoreDef(UiOsdLdStsSem);
	osUI_OsdLdStsUpdSem = osSemaphoreCreate(osSemaphore(UiOsdLdStsSem), 1); 

	osMutexDef(UI_TxTypeSyncMutex);
	osUI_TxTypeMutex = osMutexCreate(osMutex(UI_TxTypeSyncMutex));
	
	tosUI_Notify.thread_id		 = NULL;
	tosUI_Notify.iSignals		 = 0;
	ubUI_CuStartUpFlag			 = FALSE;
	ubUI_StopUpdateStsBarFlag	 = FALSE;
	tUI_SdCardSts				 = UI_SD_CFM;
	osUI_OsdLdDispQueue 		 = NULL;
	osUI_OsdLdDispThdId 		 = NULL;
	osUI_RecRptQueue			 = NULL;
	tUI_OsdLdDispSts			 = UI_OSDLDDISP_OFF;
	osUI_FuncsExecThdId 		 = NULL;
	osUI_FuncsExecQue			 = NULL;
	osUI_FuncsFinExecQue		 = NULL;
	ubUI_FinishViewSwitch		 = TRUE;
	ubUI_RestartRec 			 = FALSE;
	ubUI_ReOpenPanel			 = FALSE;
	ubUI_CuStandbyFlag			 = FALSE;
	ubUI_CuPowerOffFlag 		 = FALSE;
	ubUI_CuPowerDiscFlag		 = FALSE;
	tUI_SaveViewNum 			 = QUALVIEW_ITEM;
	ulPanelOffCount 			 = PANELAUTOOFFCOUNT;
	memset(&tUI_RecFoldersInfo, 0, sizeof(UI_RecFoldersInfo_t));
	memset(&tUI_RecFilesInfo,	0, sizeof(UI_RecFilesInfo_t));
	tUI_RecPlayAct.tRecAct		 = UI_REC_STOP;
	tUI_RecPlayAct.tPlaySts 	 = UI_RECFILE_STOP;
	ubUI_VdoRecChkFlag			 = TRUE;
	ubUI_PerDebugEn 			 = FALSE;
	osMutexDef(UiPerDbgMutex);
	osUI_PerDbgMutex			 = osMutexCreate(osMutex(UiPerDbgMutex));
	tUI_State					 = UI_DISPLAY_STATE;
	tUI_SyncAppState			 = APP_STATE_NULL;
	if(tTWC_RegTransCbFunc(TWC_UI_SETTING, UI_RecvCamResponse, UI_RecvCamRequest) != TWC_SUCCESS)
		printd(DBG_ErrorLvl, "UI Setting 2-way command fail!\n");
	if(tTWC_RegTransCbFunc(TWC_Laser_CTRL, UI_RecvCamResponse, NULL) != TWC_SUCCESS) //Alan
		printd(DBG_ErrorLvl, "TWC_Laser_CTRL Setting 2-way command fail!\n");
	if(tTWC_RegTransCbFunc(TWC_Led_CTRL, UI_RecvCamResponse, NULL) != TWC_SUCCESS)
		printd(DBG_ErrorLvl, "TWC_Led_CTRL Setting 2-way command fail!\n");
	if(tTWC_RegTransCbFunc(TWC_HEAT_CTRL, UI_RecvCamResponse, UI_RecvCamHeatingStatusReport) != TWC_SUCCESS) //Alan
		printd(DBG_ErrorLvl, "TWC_HEAT_CTRL Setting 2-way command fail!\n");
	if(tTWC_RegTransCbFunc(TWC_TX_TYPE, NULL, UI_RecvTxType) != TWC_SUCCESS)
		printd(DBG_ErrorLvl, "Register TWC_TX_TYPE TWC Fail !\n");
	if(tTWC_RegTransCbFunc(TWC_TX_VERSION, NULL, UI_RecvTxVersion) != TWC_SUCCESS)
		printd(DBG_ErrorLvl, "Register TWC_TX_VERSION TWC Fail !\n");
	if(tTWC_RegTransCbFunc(TWC_PELCO, NULL, PELCO_RecvPacket) != TWC_SUCCESS)
		printd(DBG_ErrorLvl, "Register TWC_TX_VERSION TWC Fail !\n");	
	UI_LoadDevStatusInfo();
	UI_InitCharArray();
	UI_InitTransparentCharArray();
}

//------------------------------------------------------------------------------
void UI_UpdateFwUpgStatus(void *ptUpgStsReport)
{
	APP_StatusReport_t *pFWU_StsRpt = (APP_StatusReport_t *)ptUpgStsReport;
	OSD_IMG_INFO tOsdImgInfo;
	static UI_State_t tUI_PreState;
	switch(pFWU_StsRpt->ubAPP_Report[0])
	{
		case FWU_UPG_INPROGRESS:
			if(UI_REC_START == tUI_RecPlayAct.tRecAct)
				UI_VideoRecordingExec(UI_REC_STOP);

			ubUI_StopUpdateStsBarFlag = TRUE;
			OSD_Weight(OSD_WEIGHT_8DIV8);
			tOsdImgInfo.uwHSize  = uwOSD_GetHSize();
			tOsdImgInfo.uwVSize  = uwOSD_GetVSize();
			tOsdImgInfo.uwXStart = 0;
			tOsdImgInfo.uwYStart = 0;
			OSD_EraserImg1(&tOsdImgInfo);
			tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_FWUSTARTBG, 1, &tOsdImgInfo);
			tOSD_Img1(&tOsdImgInfo, OSD_UPDATE);
			KNL_ResetLcdChannel();
			tUI_PreState = tUI_State;
			tUI_State = UI_FWUPG_STATE;
			break;
		case FWU_UPG_SUCCESS:
		case FWU_UPG_FAIL:
		case FWU_UPG_DEVTAG_FAIL:
		{
			uint16_t uwImgIdx = (FWU_UPG_SUCCESS == pFWU_StsRpt->ubAPP_Report[0])?1:0;

			tOSD_GetOsdImgInfor(1, OSD_IMG2, (OSD2IMG_FWUFAILED_ICON + uwImgIdx), 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			osDelay(2000);
			if(FWU_UPG_SUCCESS != pFWU_StsRpt->ubAPP_Report[0])
			{
				tOsdImgInfo.uwHSize  = uwOSD_GetHSize();
				tOsdImgInfo.uwVSize  = uwOSD_GetVSize();
				tOsdImgInfo.uwXStart = 0;
				tOsdImgInfo.uwYStart = 0;
				OSD_EraserImg1(&tOsdImgInfo);
			}
			ubUI_StopUpdateStsBarFlag = FALSE;
			tUI_State = tUI_PreState;
			break;
		}
		default:
		{
			/*
			uint8_t ubProgScaleIdx = (pFWU_StsRpt->ubAPP_Report[0] / pFWU_StsRpt->ubAPP_Report[1]);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, (OSD2IMG_FWUPROG0P_ICON + ubProgScaleIdx), 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			if(100 == pFWU_StsRpt->ubAPP_Report[0])
				osDelay(1000);
			*/
			printf("pFWU_StsRpt->ubAPP_Report[0] = %d\n",pFWU_StsRpt->ubAPP_Report[0]);
			
			static uint16_t uwOldVal;
			static OSD_IMG_INFO tOsdImgInfo[3];
			if(pFWU_StsRpt->ubAPP_Report[0] == 0)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PROGRESS_BAR, 3, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				tOsdImgInfo[1].uwXStart = tOsdImgInfo[0].uwXStart + 2;
			}
			else
			{
				for(uint16_t i = uwOldVal;i < pFWU_StsRpt->ubAPP_Report[0];i ++)
				{
					tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
					tOsdImgInfo[1].uwXStart += 2;
				}
			}
			uwOldVal = pFWU_StsRpt->ubAPP_Report[0];
			break;
		}
	}
}
//------------------------------------------------------------------
void UI_HandlePreviewStatus(void *ptAppStsReport)
{
	static uint32_t ulDelayCheck = 5;
	uint8_t uChnCount = 0;
	if(ulDelayCheck)
	{
		ulDelayCheck --;
		return;
	}
	APP_StatusReport_t *pAppStsRpt = (APP_StatusReport_t *)ptAppStsReport;
	
	if(tCamViewSel.tCamViewType == SINGLE_VIEW)
		uChnCount = 1;
	else if(tCamViewSel.tCamViewType == DUAL_VIEW)
		uChnCount = 2;
	else if(tCamViewSel.tCamViewType == V3_2T1B_VIEW)
		uChnCount = 3;
	else if(tCamViewSel.tCamViewType == QUAD_VIEW)
		uChnCount = 4;
	
	if(pAppStsRpt->tAPP_State != APP_PAIRING_STATE && tUI_State == UI_DISPLAY_STATE)
	{
 		for(uint8_t i = 0;i < uChnCount;i ++)
		{
			uint8_t uCamNum = tCamViewSel.tCamViewPool[i];
			if(tUI_CamStatus[uCamNum].ulFrameCount == pAppStsRpt->ulAPP_Report[uCamNum])
			{
				tUI_CamStatus[uCamNum].ulImageStuckCount ++;
				if(tUI_CamStatus[uCamNum].ulImageStuckCount == 3)//3次帧数没变就算卡图了
				{
					LCD_ChDisable(LCD_CH0 + i);
					printf("---------LCD_ChDisable chn = %d\n",i);
				}
				else if(tUI_CamStatus[uCamNum].ulImageStuckCount == 20)
				{
					if(pAppStsRpt->ulAPP_Report[4 + uCamNum] != 0)//卡图了，但是实际帧率没有为0就代表出现异常了
					{
						UI_SwitchViewType(DeskTopShowView,TRUE);
						return;
					}
				}
			}
			else
			{
				tUI_CamStatus[uCamNum].ulFrameCount = pAppStsRpt->ulAPP_Report[uCamNum];
				tUI_CamStatus[uCamNum].ulImageStuckCount = 0;
 			}
					
 		}
	}
}

//------------------------------------------------------------------------------
uint8_t UI_UpdateBSDAndTraceCtrl(UI_CamNum_t tCamNum)
{
	printf("\nUI_UpdateBSDAndTraceCtrl!!! !\n");
	uint8_t ubTryCount;
	ubTryCount = 5;
	while(--ubTryCount)
	{
		if(tTWC_Send(TWC_STA1 + tCamNum, TWC_PELCO, (uint8_t *)&CheckSensorStatus[0], 7, 10) == rUI_SUCCESS)
			break;
		osDelay(10);
	}
	if(ubTryCount <= 0) return 0;
	return 1;
}
uint8_t UI_UpdateLaserAndLedCtrl(UI_CamNum_t tCamNum)
{
	printf("UI_UpdateTxImgProc  tCamNum = %d\n",tCamNum);
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
	tCamSetCmd.ubCmd_Len				 = 4;

	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamLaser;
	if(UI_SendLaserorLedToCAM(osThreadGetId(), &tCamSetCmd, TWC_Laser_CTRL) != rUI_SUCCESS)
		return 0;

	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamLed;
	if(UI_SendLaserorLedToCAM(osThreadGetId(), &tCamSetCmd, TWC_Led_CTRL) != rUI_SUCCESS)
		return 0;

	return 1;
}

//------------------------------------------------------------------------------
UI_Event_t ZOOMCamera_Event;
TRIGGER_EVENT_t ZOOMCamera_View;

void SendZOOMCameraRelink(UI_CamNum_t tCamNum)
{
//	printf("SendZOOMCameraRelink tUI_CuSetting.ubisZoomCameraFlag[%d] = %d\n", tCamNum, tUI_CuSetting.ubisZoomCameraFlag[tCamNum]);
	if(tUI_CuSetting.ubisZoomCameraFlag[tCamNum] != 1)
		return;
	
	ZOOMCamera_View.tUI_ViewSel = tCamNum;
	ZOOMCamera_Event.pvEvent = (void *)(&ZOOMCamera_View);
	ZOOMCamera_Event.tEventType = PELCO_ZOOM_EVENT;
	if(osMessagePut(UI_EventQueue, &ZOOMCamera_Event, 0) != osOK)
			printd(DBG_ErrorLvl, "Relink Q Full\n");
}


void UI_UpdateZOOMCamAndLaserCam(uint8_t ubFirstCall[][2])
{
//	static uint8_t uFirstLinkZoomCameraFlag = 0;
//	printf("UI_UpdateZOOMCamAndLaserCam\n");
	for(uint8_t i=0;i<4;i++)
	{	
		if(ubFirstCall[i][0] && (ubZoomPairingFlag && ubZoomPairingCount==0))
		{
			if(tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].tCamConnSts == CAM_ONLINE && tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].cCamVersion[0] != '\0')
			{
				printf("ZOOMCamAndLaserCam ubFirstCall[%d][1] = %d, version = %s\n", i, ubFirstCall[i][1], tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].cCamVersion);
				ubFirstCall[i][0] = 0;
				if(UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].cCamVersion,30,zoomchar,3))
				{	
					//第一次连摄像头参数设置会把参数发一遍
//					printf("CAMERA_BSD And CAMERA_Trace Setting Reset!!!\n");
//					UI_SetTxImgProc2((UI_CamNum_t)ubFirstCall[i][1]);
					
//					for(uint8_t i=0;i<4;i++) //zoom 先复位一遍，不然cam2->cam3出问题
//					{
//						tUI_CuSetting.ubisZoomCameraFlag[i] = 0;
//					}

					tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)ubFirstCall[i][1]] = 1;
					SendZOOMCameraRelink((UI_CamNum_t)ubFirstCall[i][1]);
//					UI_GetZoomVersionDateFromString((UI_CamNum_t)ubFirstCall[i][1], tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].cCamVersion);
//					uFirstLinkZoomCameraFlag = 1;
				}
				if(UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].cCamVersion,30,Laserchar,3) ||
					UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)ubFirstCall[i][1]].cCamVersion,30,heatingchar,4))
				{
					if(UI_UpdateLaserAndLedCtrl((UI_CamNum_t)ubFirstCall[i][1]) != rUI_SUCCESS)
					{
						printf("\nCAMERA_LASER And CAMERA_LED Setting Fail!!! !\n");
					}
				}
				if(!ubFirstCall[0][0] && !ubFirstCall[1][0] && !ubFirstCall[2][0] && !ubFirstCall[3][0])
				{
					ubZoomPairingFlag = FALSE;
					ubZoomPairingCount = 3000;
				}
			}
		}

//		printf(" not ubFirstCall i = %d\n", i);
		if(tUI_CamStatus[i].ulCAM_ID == INVALID_ID)
		{
			tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)i] = 0;
		}
		if (tUI_CamStatus[(UI_CamNum_t)i].tCamConnSts == CAM_ONLINE)
		{
			if(tUI_CamStatus[(UI_CamNum_t)i].cCamVersion[0] != '\0')
			{
				tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)i] = 0;
			}
			if(UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)i].cCamVersion,30,zoomchar,3))
			{
				tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)i] = 1;
			}
		}
	}
}

void UI_UpdateAppStatus(void *ptAppStsReport)
{
	APP_StatusReport_t *pAppStsRpt = (APP_StatusReport_t *)ptAppStsReport;
	UI_CamNum_t tCamNum;
	static uint8_t 	ubFirstCall[4][2] = {0}; //Alan

	switch(pAppStsRpt->tAPP_ReportType)
	{
		case APP_PAIRSTS_RPT:
		{
			UI_Result_t tPair_Result  = (UI_Result_t)pAppStsRpt->ubAPP_Report[0];
			uint8_t ubAppPairFlag	  = pAppStsRpt->ubAPP_Report[2];
			void (*UI_RptPairFunc[])(UI_Result_t) =
			{
				UI_ReportPairingResult,
				UI_ReportAppPairingResult,
			};

			if(TRUE == ubAppPairFlag)
			{
				tPairInfo.tPairSelCam	= (UI_CamNum_t)pAppStsRpt->ubAPP_Report[3];
			}
			UI_RptPairFunc[ubAppPairFlag](tPair_Result);
			tCamNum = (UI_CamNum_t)pAppStsRpt->ubAPP_Report[3];
			if(rUI_SUCCESS == tPair_Result)
			{
				tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL = 50;
				tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast =	50;
				tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation = 50;
				tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue = 50;
				tUI_CamStatus[tCamNum].tCamImgFlip = CAMIMGFLIP_DISABLE;
				tUI_CamStatus[tCamNum].tCamImgMirror = CAMIMGMIRROR_DISABLE;
				tUI_CamStatus[tCamNum].tZoomCamISP = ZOOMCAMISP_DISABLE; 
				tUI_CamStatus[tCamNum].tCamLaser = CAMLASER_ENABLE;  //Alan
		        tUI_CamStatus[tCamNum].tCamLed = CAMLED_ENABLE;
				tUI_CamStatus[tCamNum].ubBSD = 1;
				tUI_CamStatus[tCamNum].ubTrace = 0;
				tUI_CamStatus[tCamNum].digitalZoomScale = 10;
				//重新刷新trace画框程序
				tUI_CuSetting.usZoomRedbox[tCamNum][0] = 412;
				tUI_CuSetting.usZoomRedbox[tCamNum][1] = 200;
				tUI_CuSetting.usZoomRedbox[tCamNum][2] = 612;
				tUI_CuSetting.usZoomRedbox[tCamNum][3] = 400;

				//重新刷新BSD画框程序

//				// ZoomBSDSmallRedbox
//				tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][0] = 412;
//				tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][1] = 200;
//				tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][2] = 612;
//				tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][3] = 400;

//				// ZoomBSDBigRedbox
//				tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][0] = 312;
//				tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][1] = 100;
//				tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][2] = 712;
//				tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][3] = 500;

				tUI_CuSetting.ubUpdateTxParam[tCamNum] = FALSE;

				tUI_CuSetting.bColorBLGain[tCamNum] = 0;
				tUI_CuSetting.bColorContrastGain[tCamNum] = 0;
				tUI_CuSetting.bColorSaturationGain[tCamNum] = 0;
				tUI_CuSetting.bColorHueGain[tCamNum] = 0;

				//Alan 链接后发送ubBSD和ubTrace打开信息给摄像头
				for(uint8_t i=0;i<4;i++)
				{
					if(!ubFirstCall[i][0])
					{
						ubFirstCall[i][0] = 1;
						ubFirstCall[i][1] = tCamNum;
						break;
					}
				}
				ubZoomPairingFlag  = TRUE;
				ubZoomPairingCount = 3000;

				//Alan 链接后发送Laser和Led打开信息给摄像头
				
				UI_SetTxType(tCamNum,0);
				UI_UpdateDevStatusInfo();
			}
			UI_RefreshBandwidth();//避免出现对接完后没有带宽出图

			break;
		}
		case APP_LINKSTS_RPT:
			UI_ReportCamConnectionStatus(pAppStsRpt->ubAPP_Report);
			UI_HandlePreviewStatus(ptAppStsReport);
			UI_UpdateZOOMCamAndLaserCam(ubFirstCall);
			break;
		case APP_VWMODESTS_RPT:
			//画面切换结束
			if(ubUI_ReOpenPanel)//开启背光
			{
				ubUI_ReOpenPanel = FALSE;
				if(!ubUI_CuStandbyFlag || (ubUI_CuStandbyFlag == TRUE && TriggerLock == TRUE))
					UI_PanelOn();
			}
			if(ubUI_RestartRec && !ubUI_CuPowerDiscFlag && (!ubUI_CuStandbyFlag || (ubUI_CuStandbyFlag && TriggerLock)))//重启录像
			{
				ubUI_RestartRec = FALSE;
				UI_VideoRecordingExec(UI_REC_START);
			}
			
			ubUI_FinishViewSwitch = TRUE;
			printf("uwUI_ViewSwitchTime = %d\n",uwUI_ViewSwitchTime);
			uwUI_ViewSwitchTime = 0;
			printf("ubUI_FinishViewSwitch\n");
			if(!ubUI_CuPowerDiscFlag)
			{
				osSemaphoreRelease(osUI_CuTriggerCtr);
				osSemaphoreRelease(osUI_UpdateStatusCtr);
			}
			break;
		case APP_VOXMODESTS_RPT:
			ubUI_StopUpdateStsBarFlag = pAppStsRpt->ubAPP_Report[0];
			if(TRUE == ubUI_StopUpdateStsBarFlag)
				break;
			break;
		case APP_PAIRUDCAM_PRT:
		{
			UI_CamNum_t tDelCam 	= (UI_CamNum_t)pAppStsRpt->ubAPP_Report[0];
			if(tUI_CuSetting.ubiHeatingCamFlag[tDelCam] == 1)
			{
				tUI_CuSetting.ubiHeatingCamFlag[tDelCam] = 0;
				isHeatingCamAndOnline[tDelCam] = 0;
				is_heating[tDelCam] = 0;
			}
			UI_UnBindCam(tDelCam);
			//清除对接成功标志
			OSD_IMG_INFO tOsdImgInfo,tDelOsdInfo;
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tDelOsdInfo);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRCAM1NOR_ICON + 2*tDelCam, 1, &tOsdImgInfo);
			tDelOsdInfo.uwXStart = tOsdImgInfo.uwXStart + (tOsdImgInfo.uwHSize - tDelOsdInfo.uwHSize)/2;
			OSD_EraserImg2(&tDelOsdInfo);
			
			UI_RefreshBandwidth();//避免出现对接完后没有带宽出图
			break;
		}
		case APP_DISPPAIRICON_RPT:
		{
			UI_DisplayAppPairingScreen();
			return;
		}
		case APP_TRXBWSWRET_RPT:
		{
			
			break;
		}
		default:
			break;
	}

	if(FALSE == ubUI_CuStartUpFlag)
	{
		ADO_SetDacR2RVol(R2R_VOL_n0DB);
		if((UI_RECORDING_MODE == tUI_CuSetting.tVdoMode) && (tUI_CuSetting.ubPowerOnRecord == TRUE))
		{
			UI_VideoRecordingExec(UI_REC_START);
		}

		if(tUI_ViewModeSel == SCANVIEW_ITEM)
		{
			ulAutoScanCount = tUI_CuSetting.AutoScanDuty[tUI_AutoScanNumSel]* 1000;
		}
		if(tUI_CuSetting.tPowerOnMode == POWERON_STANDBY 
			|| (tUI_CuSetting.tPowerOnMode == POWERON_LAST &&(tUI_CuSetting.tLastMode == LASTMODE_STANDBY || tUI_CuSetting.tLastMode == LASTMODE_SCAN_STANDBY)))
			UI_EnterStandby();
		
		ubUI_CuStartUpFlag = TRUE;
	}
	osSemaphoreWait(osUI_CuSemId, osWaitForever);
	tUI_CuSetting.IconSts.ubClearThdCntFlag = (tUI_SyncAppState == pAppStsRpt->tAPP_State)?FALSE:TRUE;
	tUI_SyncAppState = pAppStsRpt->tAPP_State;
	osSemaphoreRelease(osUI_CuSemId);
}
//------------------------------------------------------------------------------
void UI_CheckLightSensor(uint8_t ubFirstPanelOn)
{
	if(!tUI_CuSetting.ubAutoDimmer)
		return;
	uint16_t uwVal = 0;
	static uint8_t ubStatus = 0;//标记亮度的状态
	static uint16_t uwStableTime = 0;
	uwVal = uwSADC_GetReport(SADC_CH4);

	if(uwVal < 124)//环境亮度底
	{
		if(ubStatus)
		{
			ubStatus = 0;
			uwStableTime = 0;
		}
		if(ubUI_CurDimmerLvl > tUI_CuSetting.ubAutoDimmer_MinVal)
		{
			uwStableTime ++;
			if(uwStableTime >= 10)
			{
				for(uint8_t i = ubUI_CurDimmerLvl;i >= tUI_CuSetting.ubAutoDimmer_MinVal;(i > 2)?(i -= 2):(i = tUI_CuSetting.ubAutoDimmer_MinVal))
				{
					ubUI_CurDimmerLvl = i;
					uint16_t ubValue = (uint16_t)UI_Map(ubUI_CurDimmerLvl ,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
					SET_PANEL_PWM(ubValue);
					osDelay(3);
					if(i == tUI_CuSetting.ubAutoDimmer_MinVal)
						break;
				}
				uwStableTime = 0;
			}
		}
		else if(ubUI_CurDimmerLvl <= tUI_CuSetting.ubAutoDimmer_MinVal)
		{
			uwStableTime  = 0;
			ubUI_CurDimmerLvl = tUI_CuSetting.ubAutoDimmer_MinVal;
			uint16_t ubValue = (uint16_t)UI_Map(ubUI_CurDimmerLvl ,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
			SET_PANEL_PWM(ubValue);
		}
	}
	else//环境亮度高
	{
		if(!ubStatus)
		{
			ubStatus = 1;
			uwStableTime = 0;
		}
		if(ubUI_CurDimmerLvl < tUI_CuSetting.ubAutoDimmer_MaxVal)
		{
			if(ubFirstPanelOn)//不进行渐变
			{
				ubUI_CurDimmerLvl = tUI_CuSetting.ubAutoDimmer_MaxVal;
				uint16_t ubValue = (uint16_t)UI_Map(ubUI_CurDimmerLvl ,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
				SET_PANEL_PWM(ubValue);
			}
			else
			{
				uwStableTime ++;
				if(uwStableTime >= 10)
				{
					for(uint8_t i = ubUI_CurDimmerLvl;i <= tUI_CuSetting.ubAutoDimmer_MaxVal;
						((i + 2)<=tUI_CuSetting.ubAutoDimmer_MaxVal)?(i += 2):(i = tUI_CuSetting.ubAutoDimmer_MaxVal))
					{
						ubUI_CurDimmerLvl = i;
						uint16_t ubValue = (uint16_t)UI_Map(ubUI_CurDimmerLvl ,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
						SET_PANEL_PWM(ubValue);
						osDelay(3);
						if((i == tUI_CuSetting.ubAutoDimmer_MaxVal))
							break;
					}
					uwStableTime = 0;
				}
			}
		}
		else if(ubUI_CurDimmerLvl >= tUI_CuSetting.ubAutoDimmer_MaxVal)
		{
			uwStableTime  = 0;
			ubUI_CurDimmerLvl = tUI_CuSetting.ubAutoDimmer_MaxVal;
			uint16_t ubValue = (uint16_t)UI_Map(ubUI_CurDimmerLvl ,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
			SET_PANEL_PWM(ubValue);

		}

	}
}
//------------------------------------------------------------------------------
void UI_PwrCtrl(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	if(!ubUI_CuPowerDiscFlag)//电源还连接着
	{
		if(GET_PWR_REPORT)//电源断开
		{
			ubUI_CuPowerDiscFlag = TRUE;
			
			ADO_SetDacR2RVol(R2R_VOL_n45DB);//关掉芯片的音频输出
			
			uint8_t ubValue = (uint16_t)UI_Map(0,0,99,LCD_PWM_MIN,LCD_PWM_MAX);//；亮度降到最低
			SET_PANEL_PWM(ubValue);
			
			if(tUI_State == UI_SUBSUBSUBMENU_STATE &&  tUI_MenuItem.ubItemIdx == PLAYBACK_ITEM)//如果当前正在播放视频就需要先退出播放才能返回
			{
				UI_MenuKey();
			}
			if(tUI_State == UI_PAIRING_STATE)//如果当前正在对接就先停止对接
			{
				UI_EnterKey();
				osDelay(100);
			}
			if(UI_REC_START == tUI_RecPlayAct.tRecAct)
			{
				UI_VideoRecordingExec(UI_REC_STOP);
				ubUI_RestartRec = TRUE;
			}
			//返回桌面
			if(tUI_State != UI_DISPLAY_STATE && tUI_State != UI_MAINMENU_STATE)
				UI_BackToDesktop(TRUE);
			else
				UI_BackToDesktop(FALSE);
			OSD_Weight(OSD_WEIGHT_4DIV8);
			tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_POWER_DISCONNECT, 1, &tOsdImgInfo);
			tOSD_Img1(&tOsdImgInfo, OSD_UPDATE);
			UI_AudioOff();
		}
	}
	else
	{
		if(!GET_PWR_REPORT)//电源重新连接上
		{
			if(ubUI_CuStandbyFlag && !TriggerLock)
			{
				UI_PanelOff();
				UI_AudioOff();
				UI_DisconnectTx();
			}
			else
			{
				UI_PanelOn();
				UI_AudioOn();
				tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_POWER_CONNECT, 1, &tOsdImgInfo);
				tOSD_Img1(&tOsdImgInfo, OSD_UPDATE);
				if(ubUI_RestartRec)
				{
					UI_VideoRecordingExec(UI_REC_START);
					ubUI_RestartRec = FALSE;
				}
				osDelay(500);
				
			}
			OSD_Weight(OSD_WEIGHT_8DIV8);
			UI_BackToDesktop(FALSE);
			ubUI_CuPowerDiscFlag = FALSE;
			
			ADO_SetDacR2RVol(R2R_VOL_n0DB);
			
		}
	}
}
//------------------------------------------------------------------------------
void UI_DrawNoSignalIcon(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t uwCam_X[4] = {0};
	uint16_t uwCam_Y[4] = {0};
	uint8_t ubCamNum ;
	uint16_t uwLcd_HSize = uwLCD_GetLcdHoSize();
	uint16_t uwLcd_VSize = uwLCD_GetLcdVoSize();
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_DESKTOP_NOSIGNAL_WORD:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_DESKTOP_NOSIGNAL_WORD_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_DESKTOP_NOSIGNAL_WORD_FR:OSD2IMG_DESKTOP_NOSIGNAL_WORD_CHN, 1, &tOsdImgInfo);
	
	switch(tCamViewSel.tCamViewType)//计算nosignal文字显示位置
	{
		case SINGLE_VIEW:
			uwCam_X[tCamViewSel.tCamViewPool[0]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8*3;
			uwCam_Y[tCamViewSel.tCamViewPool[0]] = tOsdImgInfo.uwYStart;
			ubCamNum = 1;
			break;
		case DUAL_VIEW:
			//left
			uwCam_X[tCamViewSel.tCamViewPool[0]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8;
			uwCam_Y[tCamViewSel.tCamViewPool[0]] = tOsdImgInfo.uwYStart;
			//right
			uwCam_X[tCamViewSel.tCamViewPool[1]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8*5;
			uwCam_Y[tCamViewSel.tCamViewPool[1]] = tOsdImgInfo.uwYStart;
			ubCamNum = 2;
			break;
		case QUAD_VIEW:
			//upper left
			uwCam_X[tCamViewSel.tCamViewPool[0]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8;
			uwCam_Y[tCamViewSel.tCamViewPool[0]] = tOsdImgInfo.uwYStart - uwLcd_VSize/4;
			//upper right
			uwCam_X[tCamViewSel.tCamViewPool[1]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8*5;
			uwCam_Y[tCamViewSel.tCamViewPool[1]] = tOsdImgInfo.uwYStart - uwLcd_VSize/4;
			//lower left
			uwCam_X[tCamViewSel.tCamViewPool[2]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8;
			uwCam_Y[tCamViewSel.tCamViewPool[2]] = tOsdImgInfo.uwYStart + uwLcd_VSize/4;
			//lower right
			uwCam_X[tCamViewSel.tCamViewPool[3]] = tOsdImgInfo.uwXStart + uwLcd_HSize/8*5;
			uwCam_Y[tCamViewSel.tCamViewPool[3]] = tOsdImgInfo.uwYStart + uwLcd_VSize/4;
			
			ubCamNum = 4;
			break;

		default:
			break;
	}
	//显示或擦除no signal
	for(uint8_t ubCam = 0;ubCam < ubCamNum;ubCam ++)
	{
		tOsdImgInfo.uwXStart = uwCam_X[tCamViewSel.tCamViewPool[ubCam]];
		tOsdImgInfo.uwYStart = uwCam_Y[tCamViewSel.tCamViewPool[ubCam]];
		if(tUI_CamStatus[tCamViewSel.tCamViewPool[ubCam]].tCamConnSts == CAM_OFFLINE)
		{
			ubCamDiscCount[tCamViewSel.tCamViewPool[ubCam]] ++;
			if(ubCamDiscCount[tCamViewSel.tCamViewPool[ubCam]] == 10)
			{
				ubCamDiscCount[tCamViewSel.tCamViewPool[ubCam]] = 0;
				
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			}
		}
		else
		{
			ubCamDiscCount[tCamViewSel.tCamViewPool[ubCam]] = 0;
			if(tCamViewSel.tCamViewType == SINGLE_VIEW)
			{
				if(tUI_CuSetting.GuideLineEnable[tCamViewSel.tCamViewPool[0]] || tUI_CuSetting.tParkingLineEnable[tCamViewSel.tCamViewPool[0]] == PARKING_LINE_ON || tUI_CuSetting.tParkingLineEnable[tCamViewSel.tCamViewPool[0]] == PARKING_LINE_AUTO)//有引导线
				{
					OSD_EraserImg2_NoUpdate(&tOsdImgInfo);
					if(tUI_CuSetting.tParkingLineEnable[tCamViewSel.tCamViewPool[0]] == PARKING_LINE_ON)
					{
						UI_DrawParkingLine(tCamViewSel.tCamViewPool[0],OSD_UPDATE);
					}
					if(TriggerLock == TRUE && tUI_CuSetting.tParkingLineEnable[tCamViewSel.tCamViewPool[0]] == PARKING_LINE_AUTO && !(tUI_CuSetting.ubZoomfootswitch))
					{
						UI_DrawParkingLine(tCamViewSel.tCamViewPool[0],OSD_UPDATE);
					}
					if(tUI_CuSetting.GuideLineEnable[tCamViewSel.tCamViewPool[0]])
					{
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GUIDELINE_SIZE1 + tUI_CuSetting.GuideLineSize[tCamViewSel.tCamViewPool[0]], 1, &tOsdImgInfo);
						tOsdImgInfo.uwXStart = (tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][0] << 8) + tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][1];
						tOsdImgInfo.uwYStart = (tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][2] << 8) + tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][3];
						tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
					}
				}
				else
					OSD_EraserImg2(&tOsdImgInfo);
			}
			else
				OSD_EraserImg2(&tOsdImgInfo);
		}
	}

}
//------------------------------------------------------------------------------

void UI_DrawSdCardInfoIcon(void)
{
	static uint8_t ubShowSdinfo = TRUE;

	OSD_IMG_INFO tOsdImgInfo[2],tSdCardImgInfo[2],tReturnImgInfo;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SDCARD_NO, 2, &tOsdImgInfo[0]);

	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_DESKTOP_SD_NEED_FORMAT:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_DESKTOP_SD_NEED_FORMAT_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_DESKTOP_SD_NEED_FORMAT_FR:OSD2IMG_DESKTOP_SD_NEED_FORMAT_CHN, 2, &tSdCardImgInfo[0]);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tReturnImgInfo);

	if(FS_ChkMediaExist(KNL_GetFsMedia()) == FS_MEDIA_OUT)
		OSD_EraserImg2(&tOsdImgInfo[0]);
	else 
	{
		if(FS_ChkMediaRdy(KNL_GetFsMedia()) != FS_MEDIA_RDY)
		{
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
			if(ubShowSdinfo == TRUE)
			{	
				if(FS_ChkMediaRdy(KNL_GetFsMedia()) == FS_MEDIA_NOT_RDY) //sd卡还没准备好
				{
					printf("sd card not ready to access\n");
					return;
				}
				UI_ClearOsdImageNoUpdate();
				
				if(ubUI_TouchPanelSts)
					tOSD_Img2(&tReturnImgInfo, OSD_QUEUE);
				
				if(FS_ChkMediaRdy(KNL_GetFsMedia()) == FS_MEDIA_RE_PLUGIN)
					tOSD_Img2(&tSdCardImgInfo[1], OSD_UPDATE);
				else if(FS_ChkMediaRdy(KNL_GetFsMedia()) == FS_MEDIA_GET_INFO_FAIL)
					tOSD_Img2(&tSdCardImgInfo[0], OSD_UPDATE);
			
				ubShowSdinfo = FALSE;
				MenuOnFlag = TRUE;
				ulMenuAutoOffCount = MENUAUTOOFFCOUNT;
				tUI_State = UI_SHOW_SDCARD_INFO_STATE;
			}
		}
		else
			tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
	}

}
//------------------------------------------------------------------------------
void UI_DrawTxBatteryHeatingInfoIcon(void)
{
	OSD_IMG_INFO tOsdImgInfo, heatingOsdImgInfo;
	uint16_t uwLcd_HSize = uwLCD_GetLcdHoSize();
	uint16_t uwLcd_VSize = uwLCD_GetLcdVoSize();
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1, 1, &heatingOsdImgInfo);
	uint16_t ori_uwXStart = heatingOsdImgInfo.uwXStart;
	uint16_t ori_uwYStart = heatingOsdImgInfo.uwYStart;
	
	switch(tCamViewSel.tCamViewType)
	{
		case SINGLE_VIEW:
			if(is_heating[tCamViewSel.tCamViewPool[0]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[0]], 1, &heatingOsdImgInfo);
				tOSD_Img2(&heatingOsdImgInfo, OSD_UPDATE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[0]].tCamBatLvl, 1, &tOsdImgInfo);
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			}
			break;
		case DUAL_VIEW:
			//----------------------left---------------------------------
			if(is_heating[tCamViewSel.tCamViewPool[0]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[0]], 1, &heatingOsdImgInfo);
				tOSD_Img2(&heatingOsdImgInfo, OSD_QUEUE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[0]].tCamBatLvl, 1, &tOsdImgInfo);
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			}
			//---------------------right----------------------------------
			if(is_heating[tCamViewSel.tCamViewPool[1]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[1]], 1, &heatingOsdImgInfo);
				heatingOsdImgInfo.uwXStart = ori_uwXStart+uwLcd_HSize/2;
				tOSD_Img2(&heatingOsdImgInfo, OSD_UPDATE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[1]].tCamBatLvl, 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart += uwLcd_HSize/2;
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			}
			break;
		case QUAD_VIEW:
			//--------------------------------upper left------------------------------
			if(is_heating[tCamViewSel.tCamViewPool[0]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[0]], 1, &heatingOsdImgInfo);
				tOSD_Img2(&heatingOsdImgInfo, OSD_QUEUE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[0]].tCamBatLvl, 1, &tOsdImgInfo);
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE); 
			}
			//-------------------------------upper right---------------------------------
			if(is_heating[tCamViewSel.tCamViewPool[1]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[1]], 1, &heatingOsdImgInfo);
				heatingOsdImgInfo.uwXStart = ori_uwXStart+uwLcd_HSize/2;
				tOSD_Img2(&heatingOsdImgInfo, OSD_QUEUE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[1]].tCamBatLvl, 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart += uwLcd_HSize/2;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE); 
			}
			//-----------------------------lower left------------------------------------
			if(is_heating[tCamViewSel.tCamViewPool[2]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[2]], 1, &heatingOsdImgInfo);
				heatingOsdImgInfo.uwXStart = ori_uwXStart;
				heatingOsdImgInfo.uwYStart = ori_uwYStart + uwLcd_VSize/2;
				tOSD_Img2(&heatingOsdImgInfo, OSD_QUEUE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[2]].tCamBatLvl, 1, &tOsdImgInfo);
				tOsdImgInfo.uwYStart += uwLcd_VSize/2;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE); 
			}
			//---------------------------lower right-------------------------------------
			if(is_heating[tCamViewSel.tCamViewPool[3]]){
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_LEVEL1 + heating_level[tCamViewSel.tCamViewPool[3]], 1, &heatingOsdImgInfo);
				heatingOsdImgInfo.uwXStart = ori_uwXStart + uwLcd_HSize/2;
				heatingOsdImgInfo.uwYStart = ori_uwYStart + uwLcd_VSize/2;
				tOSD_Img2(&heatingOsdImgInfo, OSD_UPDATE);
			}else{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_BATTERY_NO + tUI_CamStatus[tCamViewSel.tCamViewPool[3]].tCamBatLvl, 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart += uwLcd_HSize/2;
				tOsdImgInfo.uwYStart += uwLcd_VSize/2;
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			}
			break;

		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_DrawSignalInfoIcon(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t uwLcd_HSize = uwLCD_GetLcdHoSize();
	uint16_t uwLcd_VSize = uwLCD_GetLcdVoSize();
	switch(tCamViewSel.tCamViewType)
	{
		case SINGLE_VIEW:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[0]].tCamAntLvl, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			break;
		case DUAL_VIEW:
			//----------------------left---------------------------------
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[0]].tCamAntLvl, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			//---------------------right----------------------------------
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[1]].tCamAntLvl, 1, &tOsdImgInfo);
			tOsdImgInfo.uwXStart += uwLcd_HSize/2;
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			break;
		case QUAD_VIEW:
			//--------------------------------upper left------------------------------
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[0]].tCamAntLvl, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			//-------------------------------upper right---------------------------------
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[1]].tCamAntLvl, 1, &tOsdImgInfo);
			tOsdImgInfo.uwXStart += uwLcd_HSize/2;
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			//-----------------------------lower left------------------------------------
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[2]].tCamAntLvl, 1, &tOsdImgInfo);
			tOsdImgInfo.uwYStart += uwLcd_VSize/2;
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			//---------------------------lower right-------------------------------------
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_SIGNAL_LEVEL0 + tUI_CamStatus[tCamViewSel.tCamViewPool[3]].tCamAntLvl, 1, &tOsdImgInfo);
			tOsdImgInfo.uwXStart += uwLcd_HSize/2;
			tOsdImgInfo.uwYStart += uwLcd_VSize/2;
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_DrawTxVersion(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	
	osSemaphoreWait(osUI_CuUiCtr, osWaitForever);
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
	tOSD_Img1(&tOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}
	for(UI_CamNum_t tCamNum = CAM1;tCamNum < CAM_4T;tCamNum ++)
	{
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, 350, 200 + tCamNum*50, tUI_CharOsdImgInfo, OSD_UPDATE, (tCamNum == CAM4)?"CAM4:":(tCamNum == CAM3)?"CAM3:":(tCamNum == CAM2)?"CAM2:":"CAM1:");
		if(tUI_CamStatus[tCamNum].ulCAM_ID != INVALID_ID)
			OSD_ImagePrintf(OSD_IMG_ROTATION_0, 430, 200 + tCamNum*50, tUI_CharOsdImgInfo, OSD_UPDATE, tUI_CamStatus[tCamNum].cCamVersion);
	}
	MenuOnFlag = TRUE;
	tUI_State = UI_SHOW_TX_VERSION_STATE;
	osSemaphoreRelease(osUI_CuUiCtr);
}
//------------------------------------------------------------------------------
void UI_DrawZoomButton(uint8_t isDraw)
{
	OSD_IMG_INFO tOsdImgInfo[19],tOsdImgInfo1,tOsdImgInfo2[11];
	
	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_IN_NOR, 19, &tOsdImgInfo[0]);
    char val[3];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo1);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_BSD_WORD, 11, &tOsdImgInfo2[0]);
	if (isDraw)
	{
		// button
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[6], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[8], OSD_QUEUE);
		if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_RESTART)
			tOSD_Img2(&tOsdImgInfo[11], OSD_QUEUE);
		else
			tOSD_Img2(&tOsdImgInfo[10], OSD_QUEUE);
		
		if(ubUI_TouchPanelSts)
		{
			tOSD_Img2(&tOsdImgInfo1, OSD_QUEUE);
		}

		//Switch BSD and Trace word
		tOSD_Img2(&tOsdImgInfo2[0], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo2[(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?1:(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?2:(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?3:4], OSD_QUEUE);
		//switch 
		tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[ubZoomItemCurIdx].ubTrace*2 + 5], OSD_QUEUE); //Trace switch
		tOsdImgInfo2[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2 + 5].uwYStart = BSD_OFF_NOR_DIST;
		tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2 + 5], OSD_QUEUE); //BSD switch


		//calibrate
		if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_TRACE_CALIBRATE)//Trace calibrate
			tOSD_Img2(&tOsdImgInfo2[10], OSD_QUEUE);
		else
			tOSD_Img2(&tOsdImgInfo2[9], OSD_QUEUE);
		
		if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_BSD_CALIBRATE)//BSD calibrate
		{
			tOsdImgInfo2[10].uwYStart = BSD_OFF_NOR_DIST;
			tOSD_Img2(&tOsdImgInfo2[10], OSD_QUEUE);
		}
		else
		{
			tOsdImgInfo2[9].uwYStart = BSD_OFF_NOR_DIST;
			tOSD_Img2(&tOsdImgInfo2[9], OSD_QUEUE);
		}
		
		// slide
		tOSD_Img2(&tOsdImgInfo[12], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[13], OSD_QUEUE);
		if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx == ZOOM_SLIDER)
			tOSD_Img2(&tOsdImgInfo[17], OSD_QUEUE);

		tOSD_Img2(&tOsdImgInfo[14], OSD_QUEUE);
		tOsdImgInfo[15].uwXStart = UI_Map(tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale, 10, 50,
						tOsdImgInfo[14].uwXStart, tOsdImgInfo[14].uwXStart + tOsdImgInfo[14].uwHSize - tOsdImgInfo[15].uwHSize);
		tOSD_Img2(&tOsdImgInfo[15], OSD_QUEUE);
		sprintf(val, "%d.%d", tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale/10, tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale%10);
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, 500, 480, tUI_TransparentCharOsdImgInfo, OSD_UPDATE, val);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo[0]);
		OSD_EraserImg2_NoUpdate(&tOsdImgInfo[0]);
		UI_DrawDesktopIcon();
		UI_DrawSignalInfoIcon();
	}
}
//------------------------------------------------------------------------------
void UI_WakeUpTx(void)
{
	APP_EventMsg_t tUI_Message = {0};
	tUI_Message.ubAPP_Event 	 = APP_WAKEUPTX_EVENT;
	tUI_Message.ubAPP_Message[0] = 1;		
	uint8_t ubRole = 0;
	
	if(tCamViewSel.tCamViewType == SINGLE_VIEW)
		ubRole = 1;
	else if(tCamViewSel.tCamViewType == DUAL_VIEW)
		ubRole = 2;
	else
		ubRole = 4;
	
	for(uint8_t i= 0;i < ubRole;i++)
	{
		if(tUI_CamStatus[tCamViewSel.tCamViewPool[i]].tCamConnSts == CAM_OFFLINE && tUI_CamStatus[tCamViewSel.tCamViewPool[i]].ulCAM_ID != INVALID_ID)
		{
			tUI_Message.ubAPP_Message[1] = tCamViewSel.tCamViewPool[i];
			UI_SendMessageToAPP(&tUI_Message);
			osDelay(20);
		}
	}	

}
//------------------------------------------------------------------------------
void UI_SleepTx(void)
{
	APP_EventMsg_t tUI_Message = {0};
	tUI_Message.ubAPP_Event 	 = APP_SLEEPTX_EVENT;
	tUI_Message.ubAPP_Message[0] = 1;		
	
	uint8_t ubRole = 0; 

	if(tCamViewSel.tCamViewType == SINGLE_VIEW)
		ubRole = 1;
	else if(tCamViewSel.tCamViewType == DUAL_VIEW)
		ubRole = 2;
	else
		ubRole = 4;
	
	for(uint8_t i= 0;i < ubRole;i++)
	{
		if(tUI_CamStatus[tCamViewSel.tCamViewPool[i]].tCamConnSts == CAM_ONLINE && tUI_CamStatus[tCamViewSel.tCamViewPool[i]].tCamBatLvl != BAT_NO)
		{
			tUI_Message.ubAPP_Message[1] = tCamViewSel.tCamViewPool[i];
			UI_SendMessageToAPP(&tUI_Message);
			osDelay(20);
		}
	}	
}
//------------------------------------------------------------------------------
void UI_DisconnectTx(void)
{
	APP_EventMsg_t tUI_Message = {0};
	tUI_Message.ubAPP_Event 	 = APP_DISCONNECT_EVENT;
	tUI_Message.ubAPP_Message[0] = 1;	
	UI_SendMessageToAPP(&tUI_Message);
}
//------------------------------------------------------------------------------
void UI_RefreshBandwidth(void)
{
	APP_EventMsg_t tUI_Message = {0};
	tUI_Message.ubAPP_Event 	 = APP_SETBANDWIDTH_EVENT;
	tUI_Message.ubAPP_Message[0] = 5;		//! Message Length	
	if(tCamViewSel.tCamViewType == SINGLE_VIEW)
		tUI_Message.ubAPP_Message[1] = 1;
	else if(tCamViewSel.tCamViewType == DUAL_VIEW)
		tUI_Message.ubAPP_Message[1] = 2;
	else
		tUI_Message.ubAPP_Message[1] = 4;
	
	tUI_Message.ubAPP_Message[2] = tCamViewSel.tCamViewPool[0];
	tUI_Message.ubAPP_Message[3] = tCamViewSel.tCamViewPool[1];
	tUI_Message.ubAPP_Message[4] = tCamViewSel.tCamViewPool[2];
	tUI_Message.ubAPP_Message[5] = tCamViewSel.tCamViewPool[3];
	printf("tCamViewSel.tCamViewPool[0] = %d,tCamViewSel.tCamViewPool[1] = %d,tCamViewSel.tCamViewPool[2] = %d,tCamViewSel.tCamViewPool[3] = %d\n",
		tCamViewSel.tCamViewPool[0],tCamViewSel.tCamViewPool[1],tCamViewSel.tCamViewPool[2],tCamViewSel.tCamViewPool[3]);
	UI_SendMessageToAPP(&tUI_Message);
}

//------------------------------------------------------------------------------
void UI_SetTxType(UI_CamNum_t tCamNum, uint16_t uwVal)
{
	osMutexWait(osUI_TxTypeMutex, osWaitForever);
	uwTxType[tCamNum] = uwVal;
	osMutexRelease(osUI_TxTypeMutex);
}
//------------------------------------------------------------------------------
uint16_t uwUI_GetTxType(UI_CamNum_t tCamNum)
{
	uint16_t uwVal;
	osMutexWait(osUI_TxTypeMutex, osWaitForever);
	uwVal = uwTxType[tCamNum];
	osMutexRelease(osUI_TxTypeMutex);
	return uwVal;
}
//------------------------------------------------------------------------------
void UI_RecvTxType (TWC_TAG tRecv_StaNum, uint8_t *pData)
{
	if(pData[0] != tRecv_StaNum)
		return;
	uint16_t uwVal;
	uwVal = (pData[1] << 8) + pData[2];
	UI_SetTxType((UI_CamNum_t)tRecv_StaNum,uwVal);
}
//------------------------------------------------------------------
uint8_t UI_UpdateTxImgProc(UI_CamNum_t tCamNum)
{
	printf("UI_UpdateTxImgProc  tCamNum = %d\n",tCamNum);
	uint8_t ubValue,ubResult = 0;
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
	tCamSetCmd.ubCmd_Len				 = 4;

		
	tUI_CuSetting.bColorBLGain[tCamNum] = 10;
	tUI_CuSetting.bColorContrastGain[tCamNum] = -15;
	tUI_CuSetting.bColorSaturationGain[tCamNum] = -10;
	tUI_CuSetting.bColorHueGain[tCamNum] = 10;
	
	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL + tUI_CuSetting.bColorBLGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGBL_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;

	if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		return 0;


	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast + tUI_CuSetting.bColorContrastGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGCONTRAST_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;

	if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		return 0;

	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation + tUI_CuSetting.bColorSaturationGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGSATURATION_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;

	if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		return 0;
	
	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue + tUI_CuSetting.bColorHueGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGHUE_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;

	if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		return 0;
	

	tCamSetCmd.ubCmd[UI_SETTING_ITEM]   = UI_SAVEPARAMETERS;
	tCamSetCmd.ubCmd_Len				= 2;

	if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		return 0;

	return 1;
	
}
//------------------------------------------------------------------
void UI_CheckTxType(void)//如果当前对接的不是479就重新设置图像参数
{
	uint8_t ubRole = 0;
	UI_CamNum_t tCamNum;
	if(tCamViewSel.tCamViewType == SINGLE_VIEW)
		ubRole = 1;
	else if(tCamViewSel.tCamViewType == DUAL_VIEW)
		ubRole = 2;
	else
		ubRole = 4;
	for(uint8_t i = 0;i < ubRole;i ++)
	{
		tCamNum = tCamViewSel.tCamViewPool[i];
		
		if(tUI_CuSetting.ubUpdateTxParam[tCamNum])
			continue;
		
		if(tUI_CamStatus[tCamNum].tCamConnSts == CAM_ONLINE)
		{
			if(uwUI_GetTxType(tCamNum) != 479)
				ubTxTypeCount[tCamNum] ++;
			else
			{
				tUI_CuSetting.ubUpdateTxParam[tCamNum] = TRUE;
				UI_UpdateDevStatusInfo();
			}
			
			if(ubTxTypeCount[tCamNum] == 5)
			{
				if(UI_UpdateTxImgProc(tCamNum))
				{
					tUI_CuSetting.ubUpdateTxParam[tCamNum] = TRUE;
					UI_UpdateDevStatusInfo();
				}
				else
					ubTxTypeCount[tCamNum] = 0;
			}
		}
		else
		{	
			ubTxTypeCount[tCamNum] = 0;
		}
	}	

}
//------------------------------------------------------------------------------
void UI_DrawZoomInfoIcon(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOMNOR_MENUICON, 1, &tOsdImgInfo);
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
}
//------------------------------------------------------------------------------
void UI_UpdateStatus(uint16_t *pThreadCnt)
{
	if(!ubUI_CuStartUpFlag)
		return;
	APP_EventMsg_t tUI_GetLinkStsMsg = {0};
	uint8_t ubUI_SendMsg2AppFlag = FALSE;
	UI_PwrCtrl();
	osSemaphoreWait(osUI_CuUiCtr, osWaitForever);
	if(((tUI_State == UI_DISPLAY_STATE || tUI_State == UI_MAINMENU_STATE || tUI_State == UI_SET_VOLUME_STATE) || \
		(tUI_MenuItem.ubItemIdx == ZOOM_ITEM && tUI_State == UI_SUBMENU_STATE)) && ubUI_CuPowerDiscFlag == FALSE)
	{
		if((*pThreadCnt % UI_UPDATESTS_PERIOD) == 0)
		{
			UI_DrawSignalInfoIcon();
			if(tUI_State == UI_DISPLAY_STATE)
				UI_DrawNoSignalIcon();
			UI_DrawTxBatteryHeatingInfoIcon();
			
			UI_DrawSdCardInfoIcon();
			for (uint8_t i = CAM1; i <= CAM4; i++)
			{
				if(tUI_CuSetting.ubisZoomCameraFlag[i] == 1)
				{
					UI_DrawZoomInfoIcon();
					break;
				}
			}
		}	
		if((*pThreadCnt % UI_RECORDINGSTS_PERIOD) == 0)
		{
			UI_ShowRecordingStatus();
		}
	
	}
	UI_UpdateRecStsIcon();
	UI_UpdateWarningNoteIcon();
	//paly time
	if(tUI_MenuItem.ubItemIdx == PLAYBACK_ITEM && tUI_State == UI_SUBSUBSUBMENU_STATE)
	{
		UI_ShowPlayTime(ulPLY_GetSchedule(),ulPLY_GetTotalPlayTime(),830,515);
	}
	osSemaphoreRelease(osUI_CuUiCtr);
		
	//自动背光
	if((!ubUI_CuStandbyFlag || (ubUI_CuStandbyFlag && TriggerLock)) && !ubUI_CuPowerDiscFlag)
		UI_CheckLightSensor(FALSE);

	UI_CheckMenuOffCount();//自动退出菜单
	UI_CheckScanModeCount();//自动扫描
	UI_CheckTriggerOverCount();
	UI_CheckPelcoZoomOverCount();
	UI_CheckTxType();
#if 0
	UI_CheckPanelOffCount();//自动息屏
#endif

	if((*pThreadCnt % UI_RF433CTR_PERIOD) == 0)
	{
		if((!ubUI_CuStandbyFlag || (ubUI_CuStandbyFlag && TriggerLock)) && !ubUI_CuPowerDiscFlag && tUI_MenuItem.ubItemIdx != PLAYBACK_ITEM)
			UI_WakeUpTx();
			
		//if((ubUI_CuStandbyFlag || ubUI_CuPowerDiscFlag) && !TriggerLock)
		//	UI_SleepTx();
	}
	
	if((*pThreadCnt % UI_SYNCTIMESTAMP_PERIOD) == 0)
	{
		UI_SyncTimeStamp2TX();
	}
	
	
	osSemaphoreWait(osUI_CuSemId, osWaitForever);
	switch(tUI_SyncAppState)
	{
		case APP_IDLE_STATE:
			ubUI_SendMsg2AppFlag = TRUE;
			break;
		case APP_LOSTLINK_STATE:
			ubUI_SendMsg2AppFlag = TRUE;
			break;
		case APP_LINK_STATE:
			ubUI_SendMsg2AppFlag = TRUE;
			
			break;
		case APP_PAIRING_STATE:
			UI_DrawPairingStatusIcon();
			osSemaphoreRelease(osUI_CuSemId);
			return;
		default:
			break;
	}
	if(ubUI_SendMsg2AppFlag == TRUE)//发消息给app层获取tx的连接等相关信息
	{
		if(tUI_MenuItem.ubItemIdx == PLAYBACK_ITEM && tUI_State > UI_MAINMENU_STATE)
		{
			osSemaphoreRelease(osUI_CuSemId);
			return;
		}
		tUI_GetLinkStsMsg.ubAPP_Event		= APP_LINKSTATUS_REPORT_EVENT;
		UI_SendMessageToAPP(&tUI_GetLinkStsMsg);
	}	
	
	osSemaphoreRelease(osUI_CuSemId);
	
	tUI_CuSetting.IconSts.ubRdPairIconFlag	= FALSE;
	(*pThreadCnt)++;
	if(*pThreadCnt == 0xffff)
		*pThreadCnt = 0;

}
void UI_MenuReturnEventExec(void *pvTriggerEvent)
{
	MenuOnFlag = FALSE;
	ubisCameraConfigFlag = FALSE;
	if(ubisShowZoomflag)
	{
		curr_indx = 0;
		SendIrCodeFlag = 0;
		ubisShowZoomflag = 0;
		if(tUI_ViewModeSel != SCANVIEW_ITEM)
			DeskTopShowView = tUI_ViewModeSel;
		else 
			DeskTopShowView = tUI_AutoScanNumSel;

		if(ubZoomRestartFlag)
		{
			ubZoomRestartFlag = FALSE;
			tUI_CuSetting.ubSD_Switch = 1;
		}
	}
	if(tUI_State == UI_SUBSUBMENU_STATE && tUI_MenuItem.ubItemIdx == PLAYBACK_ITEM)
	{
		UI_MenuKey();
	}
	if(tUI_State == UI_SUBMENU_STATE && tUI_MenuItem.ubItemIdx == PAIRING_ITEM)
	{
		uint8_t LinkCameraNum = 0, LinkCameraIndex = 0;

		for(uint8_t i = 0; i < 4; i++)
		{
			if(tUI_CamStatus[(UI_CamNum_t)i].tCamConnSts == CAM_ONLINE)
			{
				LinkCameraNum++;
				LinkCameraIndex = i;
			}
		}
//		if(LinkCameraNum == 1 &&  tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)LinkCameraIndex] == 1 && UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)LinkCameraIndex].cCamVersion,30,zoomchar,3))
		if(LinkCameraNum == 1 && UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)LinkCameraIndex].cCamVersion,30,zoomchar,3))
		{
			tUI_CuSetting.tPowerOnMode = tUI_CuSetting.tOldPowerOnMode = (UI_PowerOnMode_t)LinkCameraIndex;
			DeskTopShowView = tUI_ViewModeSel= (UI_CamNum_t)LinkCameraIndex;
//			UI_UpdateDevStatusInfo();
		}
	}

	if(tUI_State == UI_ADJUST_ZOOM_AREA_STATE || tUI_State == UI_ADJUST_ZOOM_BSD_AREA_STATE)
	{
		UI_EnterAdjustAreaClosePakt(tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx, TRUE);
	}
	
	UI_BackToDesktop(TRUE);
	UI_UpdateDevStatusInfo();
	ulPanelOffCount = PANELAUTOOFFCOUNT;
}

//------------------------------------------------------------------------------
void UI_EventHandles(UI_Event_t *ptEventPtr)
{
	if(FALSE == ubUI_CuStartUpFlag || ubUI_CuPowerDiscFlag)
		return;
	switch(ptEventPtr->tEventType)
	{
		case AKEY_EVENT:
		case PKEY_EVENT:
			//20201221
		case IRKEY_EVENT:
			ulMenuAutoOffCount = MENUAUTOOFFCOUNT;
			ulPanelOffCount = PANELAUTOOFFCOUNT;
			UI_KeyEventExec(ptEventPtr->pvEvent);
			break;
		case SCANMODE_EVENT:
			UI_ScanModeExec();
			break;
		case FWUPG_EVENT:
			KNL_SDUpgradeFwFunc();
			break;
			//20210302
		case TRIGGER_EVENT:
			UI_TriggerEventExec(ptEventPtr->pvEvent);
			break;
		case PELCO_ZOOM_EVENT:
            UI_PelcoZoomEventExec(ptEventPtr->pvEvent);
			break;

		//20201229
		case TOUCH_EVENT:
			if(tUI_CuSetting.ubMenuLock || FootSwitchLock)//菜单锁开启后机子不响应任何触摸
				return;
			ulMenuAutoOffCount = MENUAUTOOFFCOUNT;
			ulPanelOffCount = PANELAUTOOFFCOUNT;
			UI_TouchEventExec(ptEventPtr->pvEvent);
			break;
		case MENUOFF_EVENT:
			UI_MenuReturnEventExec(ptEventPtr->pvEvent);
			break;	
		case GESTURE_EVENT:
			
			ulPanelOffCount = PANELAUTOOFFCOUNT;
			if(ubUI_CuStandbyFlag)
			{
				UI_WakeupStandby();
			}

			break;
		case REBOOT_EVENT:
			SYS_Reboot();
			break;
		default:
			break;
	}
	
}

//------------------------------------------------------------------------------
APP_EventMsg_t *tUI_ViewTypeSetup(UI_CamViewType_t *tViewType)
{
	static APP_EventMsg_t tUI_ViewTypeParam = {0};
	UI_CamNum_t tCamNum;
	if(tUI_CuSetting.tPowerOnMode <= POWERON_QUAD)
	{	
		UI_PanelOn();
		DeskTopShowView = tUI_ViewModeSel = tUI_CuSetting.tPowerOnMode; 	
		tUI_CuSetting.tLastMode = (UI_LastMode_t)tUI_CuSetting.tPowerOnMode;
	}
	else if(tUI_CuSetting.tPowerOnMode == POWERON_SCAN)
	{
		UI_PanelOn();
		tUI_ViewModeSel = SCANVIEW_ITEM;
		//找出自动扫描第一个画面
		for(tCamNum = CAM1;tCamNum < AUTOSCAN_MAX/2;tCamNum ++)
		{
			if(tUI_CuSetting.AutoScanEnable[tCamNum])
				break;
		}

		DeskTopShowView = tCamNum;
		tUI_AutoScanNumSel = tCamNum;
		tUI_CuSetting.tLastMode = LASTMODE_SCAN;
	}
	else if(tUI_CuSetting.tPowerOnMode == POWERON_STANDBY)
	{
		UI_PanelOff();
		UI_AudioOff();
		DeskTopShowView = tUI_ViewModeSel = QUALVIEW_ITEM;
		tUI_CuSetting.tLastMode = LASTMODE_STANDBY;
		
	}
	else if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)//上一次开机最后显示的状态
	{
		if(tUI_CuSetting.tLastMode <= LASTMODE_QUAD)
		{
			UI_PanelOn();
			DeskTopShowView = tUI_ViewModeSel = tUI_CuSetting.tLastMode;
		}
		else if(tUI_CuSetting.tLastMode == LASTMODE_SCAN)
		{
			UI_PanelOn();
			tUI_ViewModeSel = SCANVIEW_ITEM;
			tUI_AutoScanNumSel = DeskTopShowView = tUI_CuSetting.tLastView;
		}
		else if(tUI_CuSetting.tLastMode == LASTMODE_STANDBY)
		{
			UI_PanelOff();
			UI_AudioOff();
			DeskTopShowView = tUI_ViewModeSel = (UI_CamNum_t)tUI_CuSetting.tLastView;
		}
		else if(tUI_CuSetting.tLastMode == LASTMODE_SCAN_STANDBY)
		{
			UI_PanelOff();
			UI_AudioOff();
			tUI_ViewModeSel = SCANVIEW_ITEM;
			DeskTopShowView = (UI_CamNum_t)tUI_CuSetting.tLastView;
		}
	}
	else
	{
		UI_PanelOn();
		DeskTopShowView = tUI_ViewModeSel = POWERON_QUAD; 	
		tUI_CuSetting.tLastMode = LASTMODE_QUAD;
	}
	tUI_CuSetting.tLastView = DeskTopShowView;

	if(DeskTopShowView <= CAM4)
		tCamViewSel.tCamViewType = SINGLE_VIEW;
	else if(DeskTopShowView == DUALVIEW_ITEM)
		tCamViewSel.tCamViewType = DUAL_VIEW;
	else if(DeskTopShowView == QUALVIEW_ITEM)
		tCamViewSel.tCamViewType = QUAD_VIEW;
	
	*tViewType = tCamViewSel.tCamViewType;

	tCamViewSel.tCamViewPool[0] = NO_CAM;
	tCamViewSel.tCamViewPool[1] = NO_CAM;
	tCamViewSel.tCamViewPool[2] = NO_CAM;
	tCamViewSel.tCamViewPool[3] = NO_CAM;

	switch(tCamViewSel.tCamViewType)
	{
		case SINGLE_VIEW:
			tCamViewSel.tCamViewPool[0] = DeskTopShowView;
			tUI_ViewTypeParam.ubAPP_Message[0] = tCamViewSel.tCamViewPool[0];
			break;
		
		case DUAL_VIEW:
			for(tCamNum = CAM1;tCamNum <= CAM4;tCamNum++)
			{
				if(tUI_CamStatus[tCamNum].tCamDispLocation_Dual == DISP_LEFT)
					tCamViewSel.tCamViewPool[0] = tCamNum;
				if(tUI_CamStatus[tCamNum].tCamDispLocation_Dual == DISP_RIGHT)
					tCamViewSel.tCamViewPool[1] = tCamNum;
			}
			tUI_ViewTypeParam.ubAPP_Message[0] = tCamViewSel.tCamViewPool[0];
			tUI_ViewTypeParam.ubAPP_Message[1] = tCamViewSel.tCamViewPool[1];
			break;
			
		case QUAD_VIEW:
			
			for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
			{
				if(tUI_CamStatus[tCamNum].tCamDispLocation_Quad == DISP_UPPER_LEFT)
				{
					tUI_ViewTypeParam.ubAPP_Message[0] = tCamViewSel.tCamViewPool[0] = tCamNum;
				}
				else if(tUI_CamStatus[tCamNum].tCamDispLocation_Quad == DISP_UPPER_RIGHT)
				{
					tUI_ViewTypeParam.ubAPP_Message[1] = tCamViewSel.tCamViewPool[1] = tCamNum;
				}
				else if(tUI_CamStatus[tCamNum].tCamDispLocation_Quad == DISP_LOWER_LEFT)
				{
					tUI_ViewTypeParam.ubAPP_Message[2] = tCamViewSel.tCamViewPool[2] = tCamNum;
				}
				else if(tUI_CamStatus[tCamNum].tCamDispLocation_Quad == DISP_LOWER_RIGHT)
				{
					tUI_ViewTypeParam.ubAPP_Message[3] = tCamViewSel.tCamViewPool[3] = tCamNum;
				}
			}
			break;
		default:
			return NULL;
	}
	tUI_ViewTypeParam.ubAPP_Message[4] = UI_GetAudioSrc();
	tUI_ViewTypeParam.ubAPP_Message[6] = tUI_CuSetting.ubSD_Switch;
	UI_DrawDesktopIcon();

	return &tUI_ViewTypeParam;
}


//------------------------------------------------------------------------------
void UI_ClearOsdImage(void)
{
	OSD_IMG_INFO tOsdImgInfo;

	tOsdImgInfo.uwHSize  = uwOSD_GetHSize();
	tOsdImgInfo.uwVSize  = uwOSD_GetVSize();
	tOsdImgInfo.uwXStart = 0;
	tOsdImgInfo.uwYStart = 0;
	OSD_EraserImg1(&tOsdImgInfo);
}
//------------------------------------------------------------------------------
void UI_ClearOsdImageNoUpdate(void)
{
	OSD_IMG_INFO tOsdImgInfo;

	tOsdImgInfo.uwHSize  = uwOSD_GetHSize();
	tOsdImgInfo.uwVSize  = uwOSD_GetVSize();
	tOsdImgInfo.uwXStart = 0;
	tOsdImgInfo.uwYStart = 0;
	OSD_EraserImg1_NOUpdate(&tOsdImgInfo);
}
//------------------------------------------------------------------------------
void UI_PowerKey(void)
{
	if(TriggerLock || !ubUI_FinishViewSwitch)
		return;
	if(!ubUI_CuStandbyFlag)
	{
		if((tUI_State == UI_PAIRING_STATE) || (tUI_MenuItem.ubItemIdx == PLAYBACK_ITEM && tUI_State > UI_MAINMENU_STATE))//正在对接或者进入播放界面都禁用power键
		{
			return;
		}
		
		UI_EnterStandby();
	}
	else
	{
		UI_WakeupStandby();
	}

}

//------------------------------------------------------------------------------
void UI_PowerLongKey(void)
{
	if(TriggerLock || ubUI_CuStandbyFlag)
		return;
	if(tUI_State == UI_DISPLAY_STATE)
	{
		BUZ_Call_DI(150);
		UI_MenuLock();
	}
}

//------------------------------------------------------------------------------
void UI_MenuKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SUBSUBSUBSUBMENU_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_SET_CUPSMODE_STATE:
		case UI_SET_CAMECOMODE_STATE:
		case UI_ENGMODE_STATE:
		case UI_CAMSETTINGMENU_STATE:
		case UI_SET_CAMCOLOR_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_SDFWUPG_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_RECPLAYLIST_STATE:
		case UI_RECPLAYADOSRC_SEL_STATE:
		case UI_RECPLAYDISPTYPE_SEL_STATE:
		case UI_PHOTOPLAYLIST_STATE:
		case UI_SDCARDFMT_STATE:
		case UI_SHOW_SDCARD_INFO_STATE:
		case UI_SHOW_TX_VERSION_STATE:
		case UI_SET_ZOOM_CAMERA_STATE:
		case UI_ADJUST_ZOOM_AREA_STATE:
		case UI_ADJUST_ZOOM_BSD_AREA_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(EXIT_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_UpArrowKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SET_CAMCOLOR_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_ENGMODE_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(UP_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_DownArrowKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_SET_CAMCOLOR_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_ENGMODE_STATE:
		case UI_ADJUST_ZOOM_AREA_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(DOWN_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_LeftArrowKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SUBSUBSUBSUBMENU_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_SET_CUPSMODE_STATE:
		case UI_SET_CAMECOMODE_STATE:
		case UI_CAMSETTINGMENU_STATE:
		case UI_SET_CAMCOLOR_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_SDFWUPG_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECPLAYLIST_STATE:
		case UI_RECPLAYADOSRC_SEL_STATE:
		case UI_RECPLAYDISPTYPE_SEL_STATE:
		case UI_SDCARDFMT_STATE:
		case UI_SET_ZOOM_CAMERA_STATE:
		case UI_ADJUST_ZOOM_AREA_STATE:
		case UI_ADJUST_ZOOM_BSD_AREA_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(LEFT_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_RightArrowKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SUBSUBSUBSUBMENU_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_SET_CUPSMODE_STATE:
		case UI_SET_CAMECOMODE_STATE:
		case UI_CAMSETTINGMENU_STATE:
		case UI_SET_CAMCOLOR_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_SDFWUPG_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_RECPLAYLIST_STATE:
		case UI_RECPLAYADOSRC_SEL_STATE:
		case UI_RECPLAYDISPTYPE_SEL_STATE:
		case UI_SDCARDFMT_STATE:
		case UI_SET_ZOOM_CAMERA_STATE:
		case UI_ADJUST_ZOOM_AREA_STATE:
		case UI_ADJUST_ZOOM_BSD_AREA_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(RIGHT_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_EnterKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SUBSUBSUBSUBMENU_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_SET_CUPSMODE_STATE:
		case UI_SET_CAMECOMODE_STATE:
		case UI_CAMSETTINGMENU_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_SDFWUPG_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_RECPLAYLIST_STATE:
		case UI_RECPLAYADOSRC_SEL_STATE:
		case UI_RECPLAYDISPTYPE_SEL_STATE:
		case UI_SDCARDFMT_STATE:
		case UI_ENGMODE_STATE:
		case UI_PAIRING_STATE:
		case UI_SET_ZOOM_CAMERA_STATE:
		case UI_ADJUST_ZOOM_AREA_STATE:
		case UI_ADJUST_ZOOM_BSD_AREA_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(ENTER_ARROW);
			break;
		default:
			break;
	}
}
//-------------------------------------------------------------------------------
void UI_SelKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_MAINMENU_STATE:
		case UI_SUBMENU_STATE:
		case UI_SUBSUBMENU_STATE:
		case UI_SUBSUBSUBMENU_STATE:
		case UI_SUBSUBSUBSUBMENU_STATE:
		case UI_SET_VOLUME_STATE:
		case UI_SET_CUPSMODE_STATE:
		case UI_SET_CAMECOMODE_STATE:
		case UI_CAMSETTINGMENU_STATE:
		case UI_DPTZ_CONTROL_STATE:
		case UI_MD_WINDOW_STATE:
		case UI_SDFWUPG_STATE:
		case UI_RECFILES_SEL_STATE:
		case UI_RECFOLDER_SEL_STATE:
		case UI_RECPLAYLIST_STATE:
		case UI_RECPLAYADOSRC_SEL_STATE:
		case UI_RECPLAYDISPTYPE_SEL_STATE:
		case UI_SDCARDFMT_STATE:
		case UI_ENGMODE_STATE:
		case UI_SET_ZOOM_CAMERA_STATE:
		case UI_ADJUST_ZOOM_AREA_STATE:
		case UI_ADJUST_ZOOM_BSD_AREA_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(SELECT_ARROW);
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_PairingKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(PAIRING_ARROW);
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_MirrorKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(MIRROR_ARROW);
			break;
		default:
			break;
	}

}
//------------------------------------------------------------------------------

void UI_MuteKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
		case UI_SET_VOLUME_STATE:
			
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(MUTE_ARROW);
			break;
		default:
			break;
	}

}
//------------------------------------------------------------------------------

void UI_VersionKey(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:			
			if(tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr)
				tUI_StateMap2MenuFunc[tUI_State].pvFuncPtr(VERSION_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_SelLongKeyinZoom(void)
{
	switch(tUI_State)
	{
		case UI_DISPLAY_STATE:
			if(tUI_CuSetting.ubisZoomCameraFlag[0] || tUI_CuSetting.ubisZoomCameraFlag[1] || tUI_CuSetting.ubisZoomCameraFlag[2] || tUI_CuSetting.ubisZoomCameraFlag[3])			
				UI_ZoomSubMenuPage(ENTER_ARROW);
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------
#define UI_MENUICON_NUM		8
#define UI_WRICON_OFFSET	2


//------------------------------------------------------------------------------
void UI_DisplayArrowKeyFunc(UI_ArrowKey_t tArrowKey)
{
	if(ubUI_CuStandbyFlag && !TriggerLock)
		return;
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			UI_DrawVolumeMenuPage();
			break;
		case RIGHT_ARROW:
			UI_DrawVolumeMenuPage();

			break;
		case MUTE_ARROW:
			tUI_CuSetting.ubMuteFlag = 1 - tUI_CuSetting.ubMuteFlag;
			UI_AudioOn();
			UI_DrawVolumeMenuPage();

			break;
		
		case EXIT_ARROW:
			if(!TriggerLock)//触发时禁止操作菜单
			{
				tUI_State = UI_MAINMENU_STATE;
				tUI_MenuItem.ubItemPreIdx = ADO_MODE_ITEM;
				tUI_MenuItem.ubItemIdx 	  = ADO_MODE_ITEM;
				osSemaphoreWait(osUI_CuUiCtr, osWaitForever);
				UI_DrawMenuPage();
				osSemaphoreRelease(osUI_CuUiCtr);
			}
			break;
		case ENTER_ARROW:
			if(UI_RECORDING_MODE == tUI_CuSetting.tVdoMode)
            {
                UI_VideoRecordingExec((UI_REC_START == tUI_RecPlayAct.tRecAct)?UI_REC_STOP:UI_REC_START);
            }
			break;
		case PAIRING_ARROW:
			if(TriggerLock)
				return;
			if(DeskTopShowView <= CAM4)//单画面才能进入对接
			{
				//转到对接界面
				MenuOnFlag = TRUE;
				tUI_State = UI_MAINMENU_STATE;
				tUI_MenuItem.ubItemIdx = PAIRING_ITEM;
				UI_EnterKey();
				tUI_State = UI_SUBMENU_STATE;
				//执行对接
				for(uint8_t i = 0;i < DeskTopShowView;i++)
					UI_RightArrowKey();
				UI_EnterKey();
			}
			break;
		case SELECT_ARROW:
			if((tUI_ViewModeSel == SCANVIEW_ITEM) || (ubUI_FinishViewSwitch == FALSE) || TriggerLock)
				return;
	
			if(tUI_ViewModeSel < QUALVIEW_ITEM)
				tUI_ViewModeSel ++;
			else
				tUI_ViewModeSel = CAM1;
			
			DeskTopShowView = tUI_ViewModeSel;
			UI_SwitchViewType(tUI_ViewModeSel,TRUE);
			if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
				UI_SaveLastMode();
			
			UI_ClearOsdImageNoUpdate();
			UI_DrawDesktopIcon();
			
			break;
		case MIRROR_ARROW:
			if(DeskTopShowView <= CAM4)
			{
				if(tUI_CamStatus[DeskTopShowView].tCamConnSts == CAM_ONLINE)
				{
					uint8_t ubVal = tUI_CamStatus[DeskTopShowView].tCamImgFlip * 2 + tUI_CamStatus[DeskTopShowView].tCamImgMirror;//00 01 10 11
					if(ubVal < 3)
						ubVal ++;
					else
						ubVal = 0;
					tUI_CamStatus[DeskTopShowView].tCamImgFlip = ubVal >> 1;
					tUI_CamStatus[DeskTopShowView].tCamImgMirror = ubVal & 0x01;

					UI_CamNum_t tCamNum = DeskTopShowView;
					
//					if(tUI_CuSetting.ubisZoomCameraFlag[tCamNum] && UI_IsZoomNewVersion(tCamNum))
					if(tUI_CuSetting.ubisZoomCameraFlag[tCamNum] && (tUI_CamStatus[tCamNum].tZoomCamISP || UI_IsZoomNewVersion(tCamNum)))
					{
						UI_SetZoomMirrorAndFlip(tCamNum);
					}
					else
					{
						UI_CUReqCmd_t tCamSetCmd;
						tCamSetCmd.tDS_CamNum				= tCamNum;
						tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
						tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
						tCamSetCmd.ubCmd_Len				 = 4;

						tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGMIRROR_SETTING;
						tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgMirror;
						uint8_t ubTryCount = 5;

						while(--ubTryCount)
						{
							if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
							{
								printf("set UI_IMGMIRROR_SETTING Fail !\n");
								osDelay(100);	
							}
							else
								break;
						}

						tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGFLIP_SETTING;
						tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgFlip;
						ubTryCount = 5;
						while(--ubTryCount)
						{
							if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
							{
								printf("set UI_IMGFLIP_SETTING Fail !\n");
								osDelay(100);	
							}
							else
								break;
						}
					}
					
					if(tUI_ViewModeSel == SCANVIEW_ITEM)
						ulAutoScanCount = tUI_CuSetting.AutoScanDuty[tUI_AutoScanNumSel]* 1000;
					UI_UpdateDevStatusInfo();
					UI_UpdateTxDevStatusInfo(tCamNum);
				}
				
			}
			
			break;
		
		case VERSION_ARROW:
			if(TriggerLock)
				return;
			UI_DrawTxVersion();
			
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_DrawPairingStatusIcon(void)
{
	static OSD_IMG_INFO tPairOsdImgInfo[2] = {0},tCamsOsdImgInfo[8] = {0};
	static uint8_t ubIconOffset = 0;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRCAM1NOR_ICON, 8, &tCamsOsdImgInfo[0]);
	if(FALSE == tPairInfo.ubDrawFlag)
		return;
	if(FALSE == tUI_CuSetting.IconSts.ubRdPairIconFlag)
	{
		if(tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRING_STS1_ICON, 2, &tPairOsdImgInfo[0]) != OSD_OK)
		{
			printd(DBG_ErrorLvl, "Load OSD Image FAIL, pls check (%d) !\n", __LINE__);
			return;
		}
		tUI_CuSetting.IconSts.ubRdPairIconFlag 	= TRUE;
		ubIconOffset     						= 1;
	}	

	tPairOsdImgInfo[ubIconOffset].uwXStart = 
		tCamsOsdImgInfo[2*tPairInfo.tPairSelCam].uwXStart + (tCamsOsdImgInfo[2*tPairInfo.tPairSelCam].uwHSize - tPairOsdImgInfo[ubIconOffset].uwHSize)/2;
	tOSD_Img2(&tPairOsdImgInfo[ubIconOffset], OSD_UPDATE);

	ubIconOffset = 1 - ubIconOffset;
}
//------------------------------------------------------------------------------
void UI_ReportPairingResult(UI_Result_t tResult)
{
	OSD_IMG_INFO tRdyOsdImgInfo, tEraseOsdImgInfo,tCamOsdImgInfo;

	tPairInfo.ubDrawFlag = FALSE;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRING_STS1_ICON, 1, &tEraseOsdImgInfo);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (((tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_PAIRCAM1NOR_ICON:
								OSD2IMG_PAIRCAM1NOR_ICON_GER)+(tPairInfo.tPairSelCam*2)+UI_ICON_HIGHLIGHT), 1, &tCamOsdImgInfo);
	switch(tResult)
	{
		case rUI_SUCCESS:
			tUI_CuSetting.ubPairedCamNum += (tUI_CuSetting.ubPairedCamNum >= tUI_CuSetting.ubTotalCamNum)?0:1;
			tUI_CamStatus[tPairInfo.tPairSelCam].ulCAM_ID = tPairInfo.tPairSelCam;
			
			
			tOSD_Img2(&tCamOsdImgInfo, OSD_QUEUE);
			
			tEraseOsdImgInfo.uwXStart = tCamOsdImgInfo.uwXStart + (tCamOsdImgInfo.uwHSize - tEraseOsdImgInfo.uwHSize)/2;
			OSD_EraserImg2(&tEraseOsdImgInfo);
			
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tRdyOsdImgInfo);
			tRdyOsdImgInfo.uwXStart = tCamOsdImgInfo.uwXStart + (tCamOsdImgInfo.uwHSize - tRdyOsdImgInfo.uwHSize)/2;

//			osDelay(1000);
			tOSD_Img2(&tRdyOsdImgInfo, OSD_UPDATE);
			
			UI_PairingSubMenuPage(RIGHT_ARROW);
			UI_UpdateDevStatusInfo();
			break;
		case rUI_FAIL:
			tEraseOsdImgInfo.uwXStart = tCamOsdImgInfo.uwXStart + (tCamOsdImgInfo.uwHSize - tEraseOsdImgInfo.uwHSize)/2;
			OSD_EraserImg2(&tEraseOsdImgInfo);
			if(tUI_CamStatus[tPairInfo.tPairSelCam].ulCAM_ID != INVALID_ID)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tRdyOsdImgInfo);
				tRdyOsdImgInfo.uwXStart = tCamOsdImgInfo.uwXStart + (tCamOsdImgInfo.uwHSize - tRdyOsdImgInfo.uwHSize)/2;
				tOSD_Img2(&tRdyOsdImgInfo, OSD_UPDATE);

			}
			
			break;
		default:
			break;
	}
	tUI_State = UI_SUBMENU_STATE;
}
//------------------------------------------------------------------------------
void UI_ReportAppPairingResult(UI_Result_t tResult)
{
	UI_ClearOsdImage();
	switch(tResult)
	{
		case rUI_SUCCESS:
			tUI_CuSetting.ubPairedCamNum += (tUI_CuSetting.ubPairedCamNum >= tUI_CuSetting.ubTotalCamNum)?0:1;
			tUI_CamStatus[tPairInfo.tPairSelCam].ulCAM_ID = tPairInfo.tPairSelCam;
			
			UI_UpdateDevStatusInfo();
			break;
		default:
			break;
	}
	ubUI_StopUpdateStsBarFlag = FALSE;
	tUI_State = UI_DISPLAY_STATE;
}
//------------------------------------------------------------------------------
void UI_SetLdDispStatus(UI_OsdLdDispSts_t tDispSts)
{
	osSemaphoreWait(osUI_OsdLdStsUpdSem, osWaitForever);
	tUI_OsdLdDispSts = tDispSts;
	osSemaphoreRelease(osUI_OsdLdStsUpdSem);
}
//------------------------------------------------------------------------------
UI_OsdLdDispSts_t UI_GetLdDispStatus(void)
{
	UI_OsdLdDispSts_t tDispSts;

	osSemaphoreWait(osUI_OsdLdStsUpdSem, osWaitForever);
	tDispSts = tUI_OsdLdDispSts;
	osSemaphoreRelease(osUI_OsdLdStsUpdSem);
	return tDispSts;
}
//------------------------------------------------------------------------------
void UI_ClearRecordStatusOsdImg(void)
{
	OSD_IMG_INFO tRecEreOsdImgInfo;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECORDING_BLACK_ICON, 1, &tRecEreOsdImgInfo);
	OSD_EraserImg2(&tRecEreOsdImgInfo);
}
//------------------------------------------------------------------------------
static void UI_OsdLoadingDisplayThread(void const *argument)
{
	uint32_t ulUI_OsdLdWaitTickTime = osWaitForever;
	uint16_t uwUI_OsdLdDispSte;
	uint8_t ubLdOsdIdx = 0;
	OSD_IMG_INFO tWorkOsdImg[9], tRecOsdImgInfo[3];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_WORKING_P1_ICON, 8, &tWorkOsdImg);

	while(1)
	{
		osMessageGet(osUI_OsdLdDispQueue, &uwUI_OsdLdDispSte, ulUI_OsdLdWaitTickTime);
		switch(uwUI_OsdLdDispSte)
		{
			case UI_SEARCH_DCIMFOLDER:
			case UI_SEARCH_RECFILES:
			{
				ubLdOsdIdx = 4;
				uint16_t uwTmpXStart, uwTmpYStart;
				uwTmpXStart = (OSD_WIDTH - tWorkOsdImg[ubLdOsdIdx].uwHSize)/2;
				uwTmpYStart = (OSD_HEIGHT - tWorkOsdImg[ubLdOsdIdx].uwVSize)/2;
				while(UI_OSDLDDISP_ON == UI_GetLdDispStatus())
				{
					tWorkOsdImg[ubLdOsdIdx].uwXStart = uwTmpXStart;
					tWorkOsdImg[ubLdOsdIdx].uwYStart = uwTmpYStart;
					tOSD_Img2(&tWorkOsdImg[ubLdOsdIdx++], OSD_UPDATE);
					ubLdOsdIdx = (ubLdOsdIdx >= 8)?4:ubLdOsdIdx;
					osDelay(200);
				}
				tWorkOsdImg[8].uwXStart = 420;
				tWorkOsdImg[8].uwYStart = 209;
				tWorkOsdImg[8].uwHSize  = 160;
				tWorkOsdImg[8].uwVSize  = 183;
				OSD_EraserImg2(&tWorkOsdImg[8]);
				uwUI_OsdLdDispSte = UI_LD_DEFU;
			}
				break;
			case UI_SDCARD_FORMAT:
			
				ubLdOsdIdx = 0;
				while(UI_OSDLDDISP_ON == UI_GetLdDispStatus())
				{
					tOSD_Img2(&tWorkOsdImg[ubLdOsdIdx], OSD_UPDATE);
					ubLdOsdIdx = (++ubLdOsdIdx >= 4)?0:ubLdOsdIdx;
					osDelay(200);
				}
				uwUI_OsdLdDispSte = UI_LD_DEFU;
				break;
			
			default:
				
				break;
		}
	}
}
//------------------------------------------------------------------------------
static void UI_FuncsExecuteThread(void const *argument)
{
	UI_FuncExecMsg_t tUI_FuncsExecMsg;
	uint8_t ubUI_FuncsExecRet = rUI_SUCCESS;
	while(1)
	{
		osMessageGet(osUI_FuncsExecQue, &tUI_FuncsExecMsg, osWaitForever);
		switch(tUI_FuncsExecMsg.uwFunc)
		{
			case UI_SDCARDFMT_ACT:
			{
				uint16_t uwLdState = UI_SDCARD_FORMAT;


				UI_SetLdDispStatus(UI_OSDLDDISP_ON);
				osMessagePut(osUI_OsdLdDispQueue, &uwLdState, 0);
				ubUI_FuncsExecRet = (FORMAT_FAIL == FS_MediaFormat(KNL_GetFsMedia()))?rUI_FAIL:rUI_SUCCESS;
				UI_SetLdDispStatus(UI_OSDLDDISP_OFF);
				if((rUI_SUCCESS == ubUI_FuncsExecRet) &&
				   ((UI_RECORDING_MODE == tUI_CuSetting.tVdoMode) && (REC_LOOPING == tUI_CuSetting.RecInfo.tREC_Mode)))
					UI_VideoRecordingExec(UI_REC_START);
				break;
			}
			case UI_PERDBGRPT_ACT:
				UI_DisplayTrxInfo(tUI_FuncsExecMsg);
				continue;
			default:
				continue;
		}
		osMessagePut(osUI_FuncsFinExecQue, &ubUI_FuncsExecRet, 0);
	}
}

//------------------------------------------------------------------------------
void UI_SetRecImgColor(UI_RecImgColor_t tColorNum)
{
	static UI_RecImgColor_t tUI_RecImgColor = UI_RECIMG_DEFU;
	uint8_t i;
	
	if(tUI_RecImgColor == tColorNum)
		return;
	for(i = 0; i < 10; i++)
		tUI_RecOsdImgDB.uwUI_RecNumArray[i] = OSD2IMG_FILE_C1_NUM0 + (tColorNum * 10) + i;
																			 
	tUI_RecOsdImgInfo.pNumImgIdxArray = tUI_RecOsdImgDB.uwUI_RecNumArray;
	for(i = 0; i < 26; i++)
	{
		tUI_RecOsdImgDB.uwUI_RecUpperLetterArray[i] = OSD2IMG_FILE_C1_UPA + (tColorNum * 26) + i;
																				      
		tUI_RecOsdImgDB.uwUI_RecLowerLetterArray[i] = OSD2IMG_FILE_C1_LWA + (tColorNum * 26) + i;
																					
	}
	tUI_RecOsdImgInfo.pUpperLetterImgIdxArray = tUI_RecOsdImgDB.uwUI_RecUpperLetterArray;
	tUI_RecOsdImgInfo.pLowerLetterImgIdxArray = tUI_RecOsdImgDB.uwUI_RecLowerLetterArray;
	for(i = 0; i < 4; i++)
		tUI_RecOsdImgDB.uwUI_RecSymbolArray[i] = (OSD2IMG_FILE_C1_COLON + (tColorNum * 4)) + i;
																				 
	tUI_RecOsdImgInfo.pSymbolImgIdxArray = tUI_RecOsdImgDB.uwUI_RecSymbolArray;
	tUI_RecImgColor = tColorNum;
}	



//------------------------------------------------------------------------------
void UI_DrawHLandNormalIcon(uint16_t uwNormalOsdImgIdx, uint16_t uwHighLigthOsdImgIdx)
{
	OSD_IMG_INFO tOsdImgInfo;

	//! Draw normal item
	tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNormalOsdImgIdx, 1, &tOsdImgInfo);
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	//! Draw highlight item
	tOSD_GetOsdImgInfor(1, OSD_IMG2, uwHighLigthOsdImgIdx, 1, &tOsdImgInfo);
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
}

//------------------------------------------------------------------------------
void UI_UpdateOsdImg4MultiView(UI_CamViewType_t tView_Type, OSD_RESULT(*pOsdImgFuncPtr)(OSD_IMG_INFO *, OSD_UPDATE_TYP), OSD_IMG_INFO *pOsdImgInfo)
{
	uint16_t uwLcd_HSize = uwOSD_GetHSize();
	uint16_t uwLcd_VSize = uwOSD_GetVSize();
	uint16_t uwOriXStart = pOsdImgInfo->uwXStart, uwOriYStart = pOsdImgInfo->uwYStart;

	if(H_VIEW == tView_Type)
		return;
	pOsdImgInfo->uwXStart += (uwLcd_HSize/2);
	if(QUAD_VIEW == tView_Type)
	{
		pOsdImgFuncPtr(pOsdImgInfo, OSD_QUEUE);
		pOsdImgInfo->uwYStart += (uwLcd_VSize/2);
		pOsdImgFuncPtr(pOsdImgInfo, OSD_QUEUE);
		pOsdImgInfo->uwXStart = uwOriXStart;
	}
	pOsdImgFuncPtr(pOsdImgInfo, OSD_QUEUE);
	pOsdImgInfo->uwXStart = uwOriXStart;
	pOsdImgInfo->uwYStart = uwOriYStart;
	pOsdImgFuncPtr(pOsdImgInfo, OSD_UPDATE);
}

//------------------------------------------------------------------------------
void UI_ReportCamConnectionStatus(void *pvConnectionSts)
{
	uint8_t *pCamConnSts = (uint8_t *)pvConnectionSts;
	uint8_t heatingUseLinkStatus[4] = {0, 0, 0, 0};
	static uint8_t last_heatingUseLinkStatus[4] = {0, 0, 0, 0};
	UI_CamNum_t tCamNum;
	UI_PerMap2AntLvl_t tAntMap[] =
	{
		{ANT_NOSIGNAL, 		 10},
		{ANT_SIGNALLVL1, 	 20},
		{ANT_SIGNALLVL2, 	 40},
		{ANT_SIGNALLVL3, 	 60},
		{ANT_SIGNALLVL4, 	 80},
		{ANT_SIGNALLVL5, 	100},
	};

	UI_Map2BatLvl_t tBatMap[] = 
	{
		{BAT_NO, 	 0},
		{BAT_LVL0, 	 1},
		{BAT_LVL1, 	 2},
		{BAT_LVL2, 	 3},
		{BAT_LVL3, 	 4},
		{BAT_LVL4, 	 5},
		{BAT_FULL, 	 6},
		{BAT_CHARGE, 7},
	};
	
	uint8_t ubUI_AntLvlCnt = sizeof tAntMap / sizeof(UI_PerMap2AntLvl_t), ubIdx,ubUI_BatLvlCnt = sizeof tBatMap / sizeof(UI_Map2BatLvl_t);

	switch(tCamViewSel.tCamViewType)
	{
		case SINGLE_VIEW:
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[0]] = (pCamConnSts[tCamViewSel.tCamViewPool[0]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
			break;
		case DUAL_VIEW:
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[0]] = (pCamConnSts[tCamViewSel.tCamViewPool[0]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[1]] = (pCamConnSts[tCamViewSel.tCamViewPool[1]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
//			printf("tCamViewPool[%d][%d], heatingUseLinkStatus[%d][%d]\n", 
//				tCamViewSel.tCamViewPool[0], tCamViewSel.tCamViewPool[1],
//				heatingUseLinkStatus[0], heatingUseLinkStatus[1]);
			break;
		case QUAD_VIEW:
//			printf("tCamViewPool[%d][%d][%d][%d], heatingUseLinkStatus[%d][%d][%d][%d]\n", 
//				tCamViewSel.tCamViewPool[0], tCamViewSel.tCamViewPool[1], tCamViewSel.tCamViewPool[2], tCamViewSel.tCamViewPool[3],
//				heatingUseLinkStatus[0], heatingUseLinkStatus[1], heatingUseLinkStatus[2], heatingUseLinkStatus[3]);
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[0]] = (pCamConnSts[tCamViewSel.tCamViewPool[0]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[1]] = (pCamConnSts[tCamViewSel.tCamViewPool[1]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[2]] = (pCamConnSts[tCamViewSel.tCamViewPool[2]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
			heatingUseLinkStatus[tCamViewSel.tCamViewPool[3]] = (pCamConnSts[tCamViewSel.tCamViewPool[3]] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
			break;
		case SCAN_VIEW:
			printf("pCamConnSts[0]=%d, [1]=%d, [2]=%d, [3]=%d\n", 
				pCamConnSts[0] == rLINK, pCamConnSts[1] == rLINK, pCamConnSts[2] == rLINK, pCamConnSts[3] == rLINK);
			break;
		default:
			break;
	}
	
	for(tCamNum = CAM1; tCamNum < tUI_CuSetting.ubTotalCamNum; tCamNum++)
	{
		UI_CamConnectStatus_t tOldCamConnSts = tUI_CamStatus[tCamNum].tCamConnSts;
		
		tUI_CamStatus[tCamNum].tCamConnSts 	= (pCamConnSts[APP_GetSTANumMappingTable(tCamNum)->tKNL_StaNum] == rLINK)?CAM_ONLINE:CAM_OFFLINE;
	
		if (tUI_CamStatus[tCamNum].tCamConnSts != tOldCamConnSts) 
		{
			if (tUI_CamStatus[tCamNum].tCamConnSts == CAM_ONLINE) 
			{
				if(tOldCamConnSts == CAM_OFFLINE)
				{
//					printd(DBG_InfoLvl, ("CAM%d -> ONLINE\n"), tCamNum+1);
//					printf(("CAM%d -> ONLINE\n"), tCamNum+1);
					SendZOOMCameraRelink(tCamNum);
				}
			}
			else
			{
				if(tOldCamConnSts == CAM_ONLINE)
				{
//					printf(("CAM%d -> ONLINE\n"), tCamNum+1);
//					printd(DBG_InfoLvl, ("CAM%d -> OFFLINE\n"), tCamNum+1);
				}
			}
		}
		
		// deal with heating cam
		if(tUI_CuSetting.ubiHeatingCamFlag[tCamNum] && 
			heatingUseLinkStatus[tCamNum] != last_heatingUseLinkStatus[tCamNum])
		{
			if(last_heatingUseLinkStatus[tCamNum] == CAM_ONLINE)	// offline
			{
				is_heating[tCamNum] = 0;
				isHeatingCamAndOnline[tCamNum] = 0;
				if(tUI_State == UI_SUBMENU_STATE && tUI_MenuItem.ubItemIdx == HEATING_ITEM)
				{
					OSD_IMG_INFO tOsdImgInfo;
					// yes or no
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_NO, 1, &tOsdImgInfo);
					tOsdImgInfo.uwXStart = HeatingYesNoPos[tCamNum][0];
					tOsdImgInfo.uwYStart = HeatingYesNoPos[tCamNum][1];
					tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
					// on off
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 1, &tOsdImgInfo);
					tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tCamNum][0];
					tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tCamNum][1];
					tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
					// time
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_3MIN_NOR, 1, &tOsdImgInfo);
					tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tCamNum+1][0];
					tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tCamNum+1][1];
					tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_5MIN_NOR, 1, &tOsdImgInfo);
					tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tCamNum+2][0];
					tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tCamNum+2][1];
					tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_10MIN_NOR, 1, &tOsdImgInfo);
					tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tCamNum+3][0];
					tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tCamNum+3][1];
					tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
					// itemIndex
					if((tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx/4) == tCamNum)
					{
						tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx = 255;
						tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx = 255;
					}
				}
			}else{	// online
				isHeatingCamAndOnline[tCamNum] = 1;
				if(tUI_State == UI_SUBMENU_STATE && tUI_MenuItem.ubItemIdx == HEATING_ITEM)
				{
					OSD_IMG_INFO tOsdImgInfo;
					// yes or no
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tOsdImgInfo);
					tOsdImgInfo.uwXStart = HeatingYesNoPos[tCamNum][0];
					tOsdImgInfo.uwYStart = HeatingYesNoPos[tCamNum][1];
					tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
					// on off // TWC with set
					// time
					if(tUI_CuSetting.heating_time[tCamNum] == 0){
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_SELECT_3MIN_NOR, 1, &tOsdImgInfo);
					}else if(tUI_CuSetting.heating_time[tCamNum] == 1){
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_SELECT_5MIN_NOR, 1, &tOsdImgInfo);
					}else{
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_SELECT_10MIN_NOR, 1, &tOsdImgInfo);
					}
					tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tCamNum+1+tUI_CuSetting.heating_time[tCamNum]][0];
					tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tCamNum+1+tUI_CuSetting.heating_time[tCamNum]][1];
					tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
				}
			}
		}
		last_heatingUseLinkStatus[tCamNum] = heatingUseLinkStatus[tCamNum];
		
		//signal
		tUI_CamStatus[tCamNum].tCamAntLvl = ANT_NOSIGNAL;
		for(ubIdx = 0; ubIdx < ubUI_AntLvlCnt; ubIdx++)
		{
			if((pCamConnSts[(APP_GetSTANumMappingTable(tCamNum)->tKNL_StaNum) + 4]) <= tAntMap[ubIdx].ubPerValue)
			{
				tUI_CamStatus[tCamNum].tCamAntLvl = tAntMap[ubIdx].tAntLvl;
				break;
			}
		}
		//battery
		tUI_CamStatus[tCamNum].tCamBatLvl = BAT_NO;
		for(ubIdx = 0; ubIdx < ubUI_BatLvlCnt; ubIdx++)
		{
			if((pCamConnSts[(APP_GetSTANumMappingTable(tCamNum)->tKNL_StaNum) + 12]) == tBatMap[ubIdx].ubValue)
			{
				tUI_CamStatus[tCamNum].tCamBatLvl = tBatMap[ubIdx].tBatLvl;
				break;
			}
		}
	}
	
	tUI_CamStatus[tCamNum].tCamAntLvl = tAntMap[ubIdx].tAntLvl;
//	printf("tCamAntLvl [%d][%d][%d][%d]\n", 
//		tUI_CamStatus[0].tCamAntLvl, tUI_CamStatus[1].tCamAntLvl, tUI_CamStatus[2].tCamAntLvl, tUI_CamStatus[3].tCamAntLvl);
}
//------------------------------------------------------------------------------
void UI_UpdateCamStatus(UI_CamNum_t tCamNum, void *pvStatus)
{
//	uint8_t *pUI_BuSts = (uint8_t *)pvStatus;

//	if(tCamNum > CAM4)
//		return;

//	tUI_CamStatus[tCamNum].tCamBatLvl = pUI_BuSts[0];
}

//------------------------------------------------------------------------------
void UI_UnBindCam(UI_CamNum_t tUI_DelCam)
{
	APP_EventMsg_t tUI_UnindBuMsg = {0};

	if(tUI_CuSetting.ubiHeatingCamFlag[tUI_DelCam] == 1)
	{
		tUI_CuSetting.ubiHeatingCamFlag[tUI_DelCam] = 0;
		isHeatingCamAndOnline[tUI_DelCam] = 0;
		is_heating[tUI_DelCam] = 0;
	}

	if(INVALID_ID == tUI_CamStatus[tUI_DelCam].ulCAM_ID)
		return;
	tUI_CamStatus[tUI_DelCam].ulCAM_ID 	  = INVALID_ID;
	tUI_CamStatus[tUI_DelCam].tCamConnSts = CAM_OFFLINE;
	tUI_CamStatus[tUI_DelCam].cCamVersion[0] = '\0';
	tUI_CuSetting.ubPairedCamNum -= (tUI_CuSetting.ubPairedCamNum == 0)?0:1;
	if(tUI_CuSetting.ubisZoomCameraFlag[tUI_DelCam] == 1)
	{
		tUI_CuSetting.ubisZoomCameraFlag[tUI_DelCam] = 0;
	}
	UI_UpdateDevStatusInfo();
	tUI_UnindBuMsg.ubAPP_Event 		= APP_UNBIND_CAM_EVENT;
	tUI_UnindBuMsg.ubAPP_Message[0] = 1;		//! Message Length
	tUI_UnindBuMsg.ubAPP_Message[1] = tUI_DelCam;
	UI_SendMessageToAPP(&tUI_UnindBuMsg);
}
//------------------------------------------------------------------------------
void UI_VoxTrigger(UI_CamNum_t tCamNum, void *pvTrig)
{
	if(tCamNum > CAM4)
		return;

	if(PS_VOX_MODE == tUI_CuSetting.tPsMode)
		UI_DisableVox();
}
//------------------------------------------------------------------------------
void UI_EnableVox(void)
{
	APP_EventMsg_t tUI_PsMessage = {0};

	tUI_PsMessage.ubAPP_Event 	   = APP_POWERSAVE_EVENT;
	tUI_PsMessage.ubAPP_Message[0] = 2;		//! Message Length
	tUI_PsMessage.ubAPP_Message[1] = PS_VOX_MODE;
	tUI_PsMessage.ubAPP_Message[2] = TRUE;
	UI_SendMessageToAPP(&tUI_PsMessage);
	tUI_CuSetting.tPsMode = PS_VOX_MODE;
	UI_UpdateDevStatusInfo();
}
//------------------------------------------------------------------------------
void UI_DisableVox(void)
{
	APP_EventMsg_t tUI_PsMessage = {0};
	UI_CUReqCmd_t tPsCmd;
	UI_CamNum_t tCamNum;

	for(tCamNum = CAM1; tCamNum < tUI_CuSetting.ubTotalCamNum; tCamNum++)
	{
		if(PS_ECO_MODE == tUI_CamStatus[tCamNum].tCamPsMode)
			continue;
		tUI_CamStatus[tCamNum].tCamPsMode = POWER_NORMAL_MODE;
		if(CAM_OFFLINE == tUI_CamStatus[tCamNum].tCamConnSts)
			continue;
		tPsCmd.tDS_CamNum 				= tCamNum;
		tPsCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
		tPsCmd.ubCmd[UI_SETTING_ITEM]   = UI_VOXMODE_SETTING;
		tPsCmd.ubCmd[UI_SETTING_DATA]   = POWER_NORMAL_MODE;
		tPsCmd.ubCmd_Len  				= 3;
		if(UI_SendRequestToCAM(osThreadGetId(), &tPsCmd) != rUI_SUCCESS)
			printd(DBG_ErrorLvl, "CAM%d:Disable VOX Notify Fail !\n", tCamNum);
	}
	tUI_PsMessage.ubAPP_Event 	   = APP_POWERSAVE_EVENT;
	tUI_PsMessage.ubAPP_Message[0] = 2;		//! Message Length
	tUI_PsMessage.ubAPP_Message[1] = PS_VOX_MODE;
	tUI_PsMessage.ubAPP_Message[2] = FALSE;
	UI_SendMessageToAPP(&tUI_PsMessage);
	tUI_CuSetting.tPsMode = POWER_NORMAL_MODE;
	UI_UpdateDevStatusInfo();
	tUI_State = UI_DISPLAY_STATE;
}
//------------------------------------------------------------------------------
void UI_DisableCuAdoOnlyMode(void)
{
	APP_EventMsg_t tUI_PsMessage = {0};

	tUI_PsMessage.ubAPP_Event 	   = APP_POWERSAVE_EVENT;
	tUI_PsMessage.ubAPP_Message[0] = 2;		//! Message Length
	tUI_PsMessage.ubAPP_Message[1] = PS_ADOONLY_MODE;
	tUI_PsMessage.ubAPP_Message[2] = FALSE;
	UI_SendMessageToAPP(&tUI_PsMessage);
	tUI_CuSetting.tPsMode = POWER_NORMAL_MODE;
	tUI_State = UI_DISPLAY_STATE;
	switch(tCamViewSel.tCamViewType)
	{
		case SINGLE_VIEW:
			
			if(SINGLE_VIEW == tCamViewSel.tCamViewType)
				break;
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_MDTrigger(UI_CamNum_t tCamNum, void *pvTrig)
{
	printd(DBG_InfoLvl, ("%s: CAM%d\n"), __func__, tCamNum);
}
//------------------------------------------------------------------------------
void UI_VoiceTrigger(UI_CamNum_t tCamNum, void *pvTrig)
{
	if(DISPLAY_1T1R == tUI_CuSetting.ubTotalCamNum)
		return;
	tUI_ViewModeSel = tCamViewSel.tCamViewPool[0];
	tCamViewSel.tCamViewType = SINGLE_VIEW;
	tCamViewSel.tCamViewPool[0] = tCamNum;
	UI_SwitchCameraSource();	
	tUI_State = UI_DISPLAY_STATE;
}
//------------------------------------------------------------------------------
UI_Result_t UI_SendRequestToCAM(osThreadId thread_id, UI_CUReqCmd_t *ptReqCmd)
{
	UI_Result_t tReq_Result = rUI_SUCCESS;
	osEvent tReq_Event;
	APP_StaNumMap_t *pUI_CamNumMap = APP_GetSTANumMappingTable(ptReqCmd->tDS_CamNum);
	uint8_t ubUI_TwcRetry = 5;

	tosUI_Notify.thread_id = thread_id;
	tosUI_Notify.iSignals  = osUI_SIGNALS;
	while(--ubUI_TwcRetry)
	{
		if(tTWC_Send(pUI_CamNumMap->tTWC_StaNum, TWC_UI_SETTING, ptReqCmd->ubCmd, ptReqCmd->ubCmd_Len, 10) == TWC_SUCCESS)
			break;
		osDelay(10);
	}
	if(!ubUI_TwcRetry)
	{
		tTWC_StopTwcSend(pUI_CamNumMap->tTWC_StaNum, TWC_UI_SETTING);
		tosUI_Notify.thread_id = NULL;
		return rUI_FAIL;
	}
	if(tosUI_Notify.thread_id != NULL)
	{
		tReq_Event = osSignalWait(tosUI_Notify.iSignals, UI_TWC_TIMEOUT);
		tReq_Result = (tReq_Event.status == osEventSignal)?(tReq_Event.value.signals == tosUI_Notify.iSignals)?tosUI_Notify.tReportSts:rUI_FAIL:rUI_FAIL;
		tTWC_StopTwcSend(pUI_CamNumMap->tTWC_StaNum, TWC_UI_SETTING);
		tosUI_Notify.thread_id  = NULL;
		tosUI_Notify.iSignals   = NULL;
		tosUI_Notify.tReportSts = rUI_SUCCESS;
	}
	return tReq_Result;
}

UI_Result_t UI_SendLaserorLedToCAM(osThreadId thread_id, UI_CUReqCmd_t *ptReqCmd,TWC_OPC Opc)
{
	UI_Result_t tReq_Result = rUI_SUCCESS;
	osEvent tReq_Event;
	APP_StaNumMap_t *pUI_CamNumMap = APP_GetSTANumMappingTable(ptReqCmd->tDS_CamNum);
	uint8_t ubUI_TwcRetry = 5;

	tosUI_Notify.thread_id = thread_id;
	tosUI_Notify.iSignals  = osUI_SIGNALS;
	while(--ubUI_TwcRetry)
	{
		if(tTWC_Send(pUI_CamNumMap->tTWC_StaNum, Opc, &ptReqCmd->ubCmd[UI_SETTING_DATA+1], 1, 10) == TWC_SUCCESS)
			break;
		osDelay(10);
	}
	if(!ubUI_TwcRetry)
	{
		tTWC_StopTwcSend(pUI_CamNumMap->tTWC_StaNum, Opc);
		tosUI_Notify.thread_id = NULL;
		return rUI_FAIL;
	}
	if(tosUI_Notify.thread_id != NULL)
	{
		tReq_Event = osSignalWait(tosUI_Notify.iSignals, UI_TWC_TIMEOUT);
		tReq_Result = (tReq_Event.status == osEventSignal)?(tReq_Event.value.signals == tosUI_Notify.iSignals)?tosUI_Notify.tReportSts:rUI_FAIL:rUI_FAIL;
		tTWC_StopTwcSend(pUI_CamNumMap->tTWC_StaNum, Opc);
		tosUI_Notify.thread_id  = NULL;
		tosUI_Notify.iSignals   = NULL;
		tosUI_Notify.tReportSts = rUI_SUCCESS;
	}
	return tReq_Result;
}
//------------------------------------------------------------------------------
void UI_RecvCamResponse(TWC_TAG tRecv_StaNum, TWC_STATUS tStatus)
{
	if(NULL == tosUI_Notify.thread_id)
		return;
	tosUI_Notify.tReportSts = (tStatus == TWC_SUCCESS)?rUI_SUCCESS:rUI_FAIL;
	if(osSignalSet(tosUI_Notify.thread_id, osUI_SIGNALS) != osOK)
		printd(DBG_ErrorLvl, "UI thread notify fail !\n");
}
//------------------------------------------------------------------------------
void UI_RecvCamRequest(TWC_TAG tRecv_StaNum, uint8_t *pTwc_Data)
{
	switch(pTwc_Data[UI_TWC_TYPE])
	{
		case UI_REPORT:
		{
			UI_CamNum_t tCamNum = NO_CAM;

			APP_TwcTagMap2CamNum(tRecv_StaNum, tCamNum);
			if(tUiReportMap2Func[pTwc_Data[UI_REPORT_ITEM]].pvAction)
				tUiReportMap2Func[pTwc_Data[UI_REPORT_ITEM]].pvAction(tCamNum, (uint8_t *)(&pTwc_Data[UI_REPORT_DATA]));
			break;
		}
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_RecvCamHeatingStatusReport(TWC_TAG tRecv_StaNum, uint8_t *pTwc_Data)
{
	static uint8_t time_map[3] = {3, 5, 10};

	if(pTwc_Data[0]){	// is heating
		is_heating[tRecv_StaNum] = 1;
		if(3*pTwc_Data[0] > 2*time_map[tUI_CuSetting.heating_time[tRecv_StaNum]])
			heating_level[tRecv_StaNum] = 2;
		else if(3*pTwc_Data[0] > time_map[tUI_CuSetting.heating_time[tRecv_StaNum]])
			heating_level[tRecv_StaNum] = 1;
		else 
			heating_level[tRecv_StaNum] = 0;
		if(tUI_State == UI_SUBMENU_STATE && tUI_MenuItem.ubItemIdx == HEATING_ITEM)
		{
			OSD_IMG_INFO tOsdImgInfo;
			if(tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx == 4*tRecv_StaNum)
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_ON_HL, 1, &tOsdImgInfo);
			else
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_ON_NOR, 1, &tOsdImgInfo);
			tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tRecv_StaNum][0];
			tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tRecv_StaNum][1];
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
		}
	}else{	// is not heating
		if(is_heating[tRecv_StaNum])
		{
			is_heating[tRecv_StaNum] = 0;
			if(tUI_State == UI_SUBMENU_STATE && tUI_MenuItem.ubItemIdx == HEATING_ITEM)
			{
				OSD_IMG_INFO tOsdImgInfo;
				if(tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx == 4*tRecv_StaNum)
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_HL, 1, &tOsdImgInfo);
				else
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart = HeatingPageItemPos[4*tRecv_StaNum][0];
				tOsdImgInfo.uwYStart = HeatingPageItemPos[4*tRecv_StaNum][1];
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			}
		}
	}
}
//------------------------------------------------------------------------------
void UI_RecvTxVersion (TWC_TAG tRecv_StaNum, uint8_t *pData)
{
	memcpy(tUI_CamStatus[(UI_CamNum_t)tRecv_StaNum].cCamVersion,pData + 1, *pData);
	UI_GetZoomVersionDateFromString((UI_CamNum_t)tRecv_StaNum, tUI_CamStatus[(UI_CamNum_t)tRecv_StaNum].cCamVersion);
	
	if(UI_CheckTxVersion(tUI_CamStatus[tRecv_StaNum].cCamVersion,30,heatingchar,4))
	{				
		if(tUI_CuSetting.ubiHeatingCamFlag[tRecv_StaNum] == 0 && 
			tUI_CamStatus[tRecv_StaNum].tCamConnSts == CAM_ONLINE && 
			tUI_CamStatus[tRecv_StaNum].ulCAM_ID != INVALID_ID) {
			// first online
			tUI_CuSetting.ubiHeatingCamFlag[tRecv_StaNum] = 1;
			isHeatingCamAndOnline[tRecv_StaNum] = 1;
		}
	} else if(tUI_CuSetting.ubiHeatingCamFlag[tRecv_StaNum] == 1){
		tUI_CuSetting.ubiHeatingCamFlag[tRecv_StaNum] = 0;
		isHeatingCamAndOnline[tRecv_StaNum] = 0;
		is_heating[tRecv_StaNum] = 0;
	}
	
//	printf("UI_RecvTxVersion cCamVersion = %s\n", tUI_CamStatus[(UI_CamNum_t)tRecv_StaNum].cCamVersion);
}

void UI_RecvZoomBSDBoxHVHandle(TWC_TAG tRecv_StaNum, uint16_t usTempZoomRedbox[4])
{
	uint16_t tmpx1 = 0, tmpx2 = 0, tmpy1 = 0, tmpy2 = 0;
	if(tUI_CamStatus[tRecv_StaNum].tCamImgFlip) //V_Mirror
	{
		usTempZoomRedbox[1] = usTempZoomRedbox[1] + (300 - usTempZoomRedbox[1])*2;
		tmpy1 = usTempZoomRedbox[1];
		usTempZoomRedbox[3] = usTempZoomRedbox[3] + (300 - usTempZoomRedbox[3])*2;
		tmpy2 = usTempZoomRedbox[3];
		usTempZoomRedbox[1] = tmpy2;
		usTempZoomRedbox[3] = tmpy1;
	}
	if(tUI_CamStatus[tRecv_StaNum].tCamImgMirror) //H_Mirror
	{
		usTempZoomRedbox[0] = usTempZoomRedbox[0] + (512 - usTempZoomRedbox[0])*2;
		tmpx1 = usTempZoomRedbox[0];
		usTempZoomRedbox[2] = usTempZoomRedbox[2] + (512 - usTempZoomRedbox[2])*2;
		tmpx2 = usTempZoomRedbox[2];
		usTempZoomRedbox[0] = tmpx2;
		usTempZoomRedbox[2] = tmpx1;
	}
}

void PELCO_Reset_Img(UI_CamNum_t tCamNum)
{
	uint8_t ubValue,ubTryCount;
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
	tCamSetCmd.ubCmd_Len				= 4;
	
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGMIRROR_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgMirror;
	ubTryCount = 5;
	
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGMIRROR_SETTING  Fail !\n");
			osDelay(20);
		}
		else
			break;
	}

	tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGFLIP_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgFlip;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGFLIP_SETTING Fail !\n");
			osDelay(20);
		}
		else
			break;

	}
}

void PELCO_RecvPacket(TWC_TAG tRecv_StaNum, uint8_t *pData)
{
	// printf("\nPELCO_RecvPacket:\t");
	uint8_t uDataLen = 0;
	uint8_t uRecvDate[128] = {0};
	uint16_t crc16_payload, crc16_pkt;
	uint16_t usTempZoomRedbox[4] = {0};
	uint8_t x_high = 0, x_low = 0, y_high = 0, y_low = 0;
	if(*pData == 0xFF && *(pData+1) == 0XAA)
	{
		uDataLen = *(pData+2);
		// printf("uDataLen = %d\n", uDataLen);
		memcpy(uRecvDate, pData + 3, uDataLen);
		crc16_pkt = (uRecvDate[uDataLen-1] << 8) | uRecvDate[uDataLen-2];
		if(crc16_pkt = crc16_ccitt(uRecvDate,uDataLen))
		{
			switch(uRecvDate[1])
			{
				case UI_TRACE_CONFIG:
				{
					printf("UI_TRACE_CONFIG\n");
//					printf("BSD = %d\n",uRecvDate[2]);
//					printf("Trace = %d\n",uRecvDate[3]); //tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&CheckSensorStatus[0], 7, 10); //get BSD and Trace status
					if((tUI_CamStatus[tRecv_StaNum].ubBSD != uRecvDate[2] || tUI_CamStatus[tRecv_StaNum].ubTrace != uRecvDate[3]))
					{
						tUI_CamStatus[tRecv_StaNum].ubBSD = uRecvDate[2];
//						printf("BSD = %d\n",uRecvDate[2]);
						tUI_CamStatus[tRecv_StaNum].ubTrace = uRecvDate[3];
//						printf("Trace = %d\n",uRecvDate[2]);
						if(tUI_State == UI_SET_ZOOM_CAMERA_STATE && tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx == tRecv_StaNum)
						{
							OSD_IMG_INFO tOsdImgInfo2[4];
							tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo2[0]);
							tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[tRecv_StaNum].ubTrace*2], OSD_QUEUE);
							tOsdImgInfo2[tUI_CamStatus[tRecv_StaNum].ubBSD*2].uwYStart = BSD_OFF_NOR_DIST;
							tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[tRecv_StaNum].ubBSD*2 ], OSD_UPDATE);
						}
					}
				}
				break;
				case UI_BSD_YELLOW_BOX_DATA:
				{
					printf("UI_BSD_YELLOW_BOX_DATA\n");
//					printf("uRecvDate[2] = 0x%x, [3] = 0x%x, [4] = 0x%x, [5] = 0x%x, [6] = 0x%x, [7] = 0x%x, [8] = 0x%x, [9] = 0x%x, uDataLen = %d\n", uRecvDate[2], uRecvDate[3], uRecvDate[4], uRecvDate[5]\
//						,uRecvDate[6], uRecvDate[7], uRecvDate[8], uRecvDate[9], uDataLen);
					x_high = uRecvDate[2];
					x_low = uRecvDate[3];
					
					y_high = uRecvDate[4];
					y_low = uRecvDate[5];
					
					usTempZoomRedbox[0] = ((x_high << 8) & 0xFF00) | x_low;
					usTempZoomRedbox[1] = ((y_high << 8) & 0xFF00) | y_low;

					x_high = uRecvDate[6];
					x_low = uRecvDate[7];
					
					y_high = uRecvDate[8];
					y_low = uRecvDate[9];
					
					usTempZoomRedbox[2] = ((x_high << 8) & 0xFF00) | x_low;
					usTempZoomRedbox[3] = ((y_high << 8) & 0xFF00) | y_low;
//					printf("UI_BSD_YELLOW_BOX_DATA usTempZoomRedbox[0] = 0x%x, [1] = 0x%x, [2] = 0x%x, [3] = 0x%x\n",\
//						usTempZoomRedbox[0], usTempZoomRedbox[1], usTempZoomRedbox[2], usTempZoomRedbox[3]);
//					if(!UI_IsZoomNewVersion((UI_CamNum_t)tRecv_StaNum))
					if(!tUI_CamStatus[(UI_CamNum_t)tRecv_StaNum].tZoomCamISP && !UI_IsZoomNewVersion((UI_CamNum_t)tRecv_StaNum))
					{
						UI_RecvZoomBSDBoxHVHandle(tRecv_StaNum, usTempZoomRedbox);
					}
//					printf("UI_RecvZoomBSDBoxHVHandle yellow usTempZoomRedbox[0] = 0x%x, [1] = 0x%x, [2] = 0x%x, [3] = 0x%x\n",\
//						usTempZoomRedbox[0], usTempZoomRedbox[1], usTempZoomRedbox[2], usTempZoomRedbox[3]);
					
					if(tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][0] != usTempZoomRedbox[0] || \
					   tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][1] != usTempZoomRedbox[1] || \
					   tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][2] != usTempZoomRedbox[2] || \
					   tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][3] != usTempZoomRedbox[3])
					{
						tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][0] = usTempZoomRedbox[0];
						tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][1] = usTempZoomRedbox[1];
						tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][2] = usTempZoomRedbox[2];
						tUI_CuSetting.usZoomBSDBigRedbox[tRecv_StaNum][3] = usTempZoomRedbox[3];
					}
				}
				break;
				case UI_BSD_RED_BOX_DATA:
				{
					printf("UI_BSD_RED_BOX_DATA\n");
					x_high = uRecvDate[2];
					x_low = uRecvDate[3];
					
					y_high = uRecvDate[4];
					y_low = uRecvDate[5];
					
					usTempZoomRedbox[0] = ((x_high << 8) & 0xFF00) | x_low;
					usTempZoomRedbox[1] = ((y_high << 8) & 0xFF00) | y_low;

					x_high = uRecvDate[6];
					x_low = uRecvDate[7];
					
					y_high = uRecvDate[8];
					y_low = uRecvDate[9];
					
					usTempZoomRedbox[2] = ((x_high << 8) & 0xFF00) | x_low;
					usTempZoomRedbox[3] = ((y_high << 8) & 0xFF00) | y_low;

//					printf("UI_BSD_RED_BOX_DATA usTempZoomRedbox[0] = 0x%x, [1] = 0x%x, [2] = 0x%x, [3] = 0x%x\n",\
//						usTempZoomRedbox[0], usTempZoomRedbox[1], usTempZoomRedbox[2], usTempZoomRedbox[3]);
//					if(!UI_IsZoomNewVersion((UI_CamNum_t)tRecv_StaNum))
					if(!tUI_CamStatus[(UI_CamNum_t)tRecv_StaNum].tZoomCamISP && !UI_IsZoomNewVersion((UI_CamNum_t)tRecv_StaNum))
					{
						UI_RecvZoomBSDBoxHVHandle(tRecv_StaNum, usTempZoomRedbox);
					}
//					printf("UI_RecvZoomBSDBoxHVHandle red usTempZoomRedbox[0] = 0x%x, [1] = 0x%x, [2] = 0x%x, [3] = 0x%x\n",\
//						usTempZoomRedbox[0], usTempZoomRedbox[1], usTempZoomRedbox[2], usTempZoomRedbox[3]);
					if(tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][0] != usTempZoomRedbox[0] || \
					   tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][1] != usTempZoomRedbox[1] || \
					   tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][2] != usTempZoomRedbox[2] || \
					   tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][3] != usTempZoomRedbox[3])
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][0] = usTempZoomRedbox[0];
						tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][1] = usTempZoomRedbox[1];
						tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][2] = usTempZoomRedbox[2];
						tUI_CuSetting.usZoomBSDSmallRedbox[tRecv_StaNum][3] = usTempZoomRedbox[3];
					}
				}
				break;
				case UI_MIRROR_FLIP_CONFIG:
				{
					printf("UI_MIRROR_FLIP_CONFIG\n");
//					printf("UI_MIRROR_FLIP_CONFIG uRecvDate[2] = %d, uRecvDate[3] = %d\n", uRecvDate[2], uRecvDate[3]);
					//第一次接收到标志位则会重置WT479的翻转
					if(!tUI_CamStatus[tRecv_StaNum].tZoomCamISP)
					{
						tUI_CamStatus[tRecv_StaNum].tZoomCamISP = ZOOMCAMISP_ENABLE;
						if(tUI_CamStatus[tRecv_StaNum].tCamImgFlip == CAMIMGFLIP_ENABLE || tUI_CamStatus[tRecv_StaNum].tCamImgMirror == CAMIMGMIRROR_ENABLE)
						{
							tUI_CamStatus[tRecv_StaNum].tCamImgFlip = CAMIMGFLIP_DISABLE;
							tUI_CamStatus[tRecv_StaNum].tCamImgMirror = CAMIMGMIRROR_DISABLE;
							PELCO_Reset_Img((UI_CamNum_t)tRecv_StaNum);
						}
					}

//					if((tUI_CamStatus[tRecv_StaNum].tCamImgMirror != uRecvDate[2] || tUI_CamStatus[tRecv_StaNum].tCamImgFlip != uRecvDate[3]) && UI_IsZoomNewVersion((UI_CamNum_t)tRecv_StaNum))
					if((tUI_CamStatus[tRecv_StaNum].tCamImgMirror != uRecvDate[2] || tUI_CamStatus[tRecv_StaNum].tCamImgFlip != uRecvDate[3]))
					{
//						printf("tUI_CamStatus[%d].tCamImgMirror = %d, tUI_CamStatus[%d].tCamImgFlip = %d\n",\
//							tRecv_StaNum, tUI_CamStatus[tRecv_StaNum].tCamImgMirror, tRecv_StaNum, tUI_CamStatus[tRecv_StaNum].tCamImgFlip);
						tUI_CamStatus[tRecv_StaNum].tCamImgMirror = uRecvDate[2];
						// printf("BSD = %d\n",uRecvDate[2]);
						tUI_CamStatus[tRecv_StaNum].tCamImgFlip = uRecvDate[3];
//						tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx == CAMERASET_ITEM		
					}
					if(ubisCameraConfigFlag == TRUE && tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx == tRecv_StaNum)
					{
						OSD_IMG_INFO tOsdImgInfo[8];
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_H_MIRROR_OFF_NOR, 8, &tOsdImgInfo[0]);
						tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[tRecv_StaNum].tCamImgMirror * 2], OSD_QUEUE);
						tOSD_Img2(&tOsdImgInfo[4 + tUI_CamStatus[tRecv_StaNum].tCamImgFlip * 2], OSD_UPDATE);
					}
				}
				break;
				default:
				break;
			}
		}
	}
}
//------------------------------------------------------------------------------
//判断TX版本号
uint8_t UI_CheckTxVersion(char *pTxVersion, uint8_t ubTxVersionLen, char *pTxDst, uint8_t ubTxDst)
{
	int found = 0;
    for (int i = 0; pTxVersion[i] != '\0'; i++) {
        if (pTxVersion[i] == '\0' || pTxVersion[i] == ' ') {
            i++; // Skip spaces
        }
        if (pTxVersion[i] == pTxDst[0]) {
            found = 1;
            for (int j = 1; pTxDst[j] != '\0'; j++) {
                if (pTxVersion[i + j] != pTxDst[j]) {
                    found = 0;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
	return found;
}
//------------------------------------------------------------------------------
void UI_ResetUIParameter(void)
{
	printf("---------------------UI_ResetUIParameter--------------------------\n");
	UI_CamNum_t tCamNum;
	uint16_t uwTemp;
	/****************************************tUI_CamStatus*******************************************/
	for(tCamNum = CAM1; tCamNum < CAM_4T; tCamNum++)
	{	
		
		tUI_CamStatus[tCamNum].tCamDispLocation_Quad = DISP_UPPER_LEFT + tCamNum;//quad
		
		if(tCamNum < 2)//dual
			tUI_CamStatus[tCamNum].tCamDispLocation_Dual = DISP_LEFT + tCamNum;
		else	
			tUI_CamStatus[tCamNum].tCamDispLocation_Dual = DISP_LOCATION_TYPE_MAX;

		tUI_CamStatus[tCamNum].tCamDispLocation = DISP_H_L + tCamNum;//H
		tUI_CamStatus[tCamNum].tCamPsMode = POWER_NORMAL_MODE;
		tUI_CamStatus[tCamNum].tCamAnrMode = CAMSET_OFF;
		tUI_CamStatus[tCamNum].tCam3DNRMode = CAMSET_OFF;
		tUI_CamStatus[tCamNum].tCamvLDCMode = CAMSET_OFF;
		tUI_CamStatus[tCamNum].tCamAecMode = CAMSET_OFF;
		tUI_CamStatus[tCamNum].tCamDisMode = CAMSET_OFF;
		tUI_CamStatus[tCamNum].tCamFlicker = CAMFLICKER_50HZ;
		tUI_CamStatus[tCamNum].tCamCbrMode = CAMSET_ON;
		tUI_CamStatus[tCamNum].tCamCondenseMode = CAMSET_OFF;
		tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL = 50;
		tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast =	50;
		tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation = 50;
		tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue = 50;
		tUI_CamStatus[tCamNum].tCamImgFlip = CAMIMGFLIP_DISABLE;
		tUI_CamStatus[tCamNum].tCamImgMirror = CAMIMGMIRROR_DISABLE;
		tUI_CamStatus[tCamNum].tZoomCamISP = ZOOMCAMISP_DISABLE; 
		tUI_CamStatus[tCamNum].tCamLaser = CAMLASER_ENABLE;
		tUI_CamStatus[tCamNum].tCamLed = CAMLED_ENABLE;
		tUI_CamStatus[tCamNum].ubVolumeLvl = 50;
		tUI_CamStatus[tCamNum].ubBSD   = 1;
		tUI_CamStatus[tCamNum].ubTrace = 0;
		tUI_CamStatus[tCamNum].digitalZoomScale = 10;
		tUI_CamStatus[tCamNum].tCamAntLvl = ANT_NOSIGNAL;
		tUI_CamStatus[tCamNum].tCamBatLvl = BAT_NO;

		tUI_CuSetting.ubUpdateTxParam[tCamNum] = FALSE;
		tUI_CuSetting.bColorBLGain[tCamNum] = 0;
		tUI_CuSetting.bColorContrastGain[tCamNum] = 0;
		tUI_CuSetting.bColorSaturationGain[tCamNum] = 0;
		tUI_CuSetting.bColorHueGain[tCamNum] = 0;

	}
	/****************************************tUI_CuSetting*******************************************/

	//AutoScan
	for(tCamNum = 0; tCamNum < 7; tCamNum++)
	{
		tUI_CuSetting.AutoScanDuty[tCamNum] = 10;
		tUI_CuSetting.AutoScanEnable[tCamNum] = 1;
	}
	
	//Trigger
	for(tCamNum = CAM1; tCamNum < 5; tCamNum++)
	{
		tUI_CuSetting.TriggerDelay[tCamNum] = 2;
	}
	tUI_CuSetting.TriggerPriority[CAM1] = 1;
	tUI_CuSetting.TriggerPriority[CAM2] = 2;
	tUI_CuSetting.TriggerPriority[CAM3] = 3;
	tUI_CuSetting.TriggerPriority[CAM4] = 0;
	tUI_CuSetting.TriggerPriority[4] = 4;
	
	tUI_CuSetting.TriggerSplitView = QUALVIEW_ITEM;
	
	//PowerOn
	tUI_CuSetting.tOldPowerOnMode = tUI_CuSetting.tPowerOnMode = POWERON_QUAD;
	
	//parkingLine	
	for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.ParkingLineSize[tCamNum] = 5;
		tUI_CuSetting.tParkingLineEnable[tCamNum] = PARKING_LINE_AUTO;
		uwTemp = (1024 - 486)/2;
		tUI_CuSetting.ParkingLine_XY[tCamNum][0] = uwTemp >> 8;
		tUI_CuSetting.ParkingLine_XY[tCamNum][1] = (uint8_t)uwTemp;
		uwTemp = (600 - 295)/2;
		tUI_CuSetting.ParkingLine_XY[tCamNum][2] = uwTemp >> 8;
		tUI_CuSetting.ParkingLine_XY[tCamNum][3] = (uint8_t)uwTemp;
	}
	
	//record
	tUI_CuSetting.RecInfo.tREC_Time = RECTIME_5MIN;
	tUI_CuSetting.ubPowerOnRecord = 1;
	tUI_CuSetting.ubTimeStame = 1;
    //  0  FS_TZ_DST       = 0xD0,             //!< UTC-12:00, Dateline Standard Time
	//  1  FS_TZ_SST       = 0xD4,             //!< UTC-11:00, Samoa Standard Time
	//  2  FS_TZ_HST       = 0xD8,             //!< UTC-10:00, Hawaii Standard Time
	//  3  FS_TZ_ALASKAST  = 0xDC,             //!< UTC-09:00, Alaska Standard Time
	//  4  FS_TZ_PST       = 0xE0,             //!< UTC-08:00, Pacific Standard Time
	//  5  FS_TZ_MOUNTST   = 0xE4,             //!< UTC-07:00, Mountain Standard Time
	//  6  FS_TZ_CST       = 0xE8,             //!< UTC-06:00, Central Standard Time
	//  7  FS_TZ_EST       = 0xEC,             //!< UTC-05:00, Eastern Standard Time
	//  8  FS_TZ_ATLANTST  = 0xF0,             //!< UTC-04:00, Atlantic Standard time
	//  9  FS_TZ_NST       = 0xF2,             //!< UTC-03:30, Newfoundland Standard Time
	// 10  FS_TZ_GREENLANDST = 0xF4,           //!< UTC-03:00, Greenland Standard Time
	// 11  FS_TZ_MAST      = 0xF8,             //!< UTC-02:00, Mid-Atlantic Standard Time
	// 12  FS_TZ_AZORESST  = 0xFC              //!< UTC-01:00, Azores Standard Time
	// 13  FS_TZ_GMT       = 0x80,             //!< UTC=GMT, Greenwich Standard Time
	// 14  FS_TZ_CET       = 0x84,             //!< UTC+01:00, Central Europe Time
	// 15  FS_TZ_EEST      = 0x88,             //!< UTC+02:00, Eastern Europe Standard Time
	// 16  FS_TZ_MST       = 0x8C,             //!< UTC+03:00, Moscow Standard Time
	// 17  FS_TZ_AST       = 0x90,             //!< UTC+04:00, Arabian Standard Time
	// 18  FS_TZ_WAST      = 0x94,             //!< UTC+05:00, West Asia Standard Time
	// 19  FS_TZ_CAST      = 0x98,             //!< UTC+06:00, Central Asia Standard Time
	// 20  FS_TZ_NAST      = 0x9C,             //!< UTC+07:00, North Asia Standard Time
	// 21  FS_TZ_NAEST     = 0xA0,             //!< UTC+08:00, North Asia East Standard Time,GMT+08:00
	// 22  FS_TZ_TOKYOST   = 0xA4,             //!< UTC+09:00, Tokyo Standard Time
	// 23  FS_TZ_WPST      = 0xA8,             //!< UTC+10:00, West Pacific Standard Time
	// 24  FS_TZ_CPST      = 0xAC,             //!< UTC+11:00, Central Pacific Standard Time
	// 25  FS_TZ_NZST      = 0xB0,             //!< UTC+12:00, New Zealand Standard Time
	// 26  FS_TZ_TONGAST   = 0xB4,             //!< UTC+13:00, Tonga Standard Time
    //Time Zone
    tUI_CuSetting.ubtimezone = 6;

	//backlight
	tUI_CuSetting.BriLvL.ulBL_UpdateLvL 	 = 99;
	//volume
	tUI_CuSetting.VolLvL.ulVOL_UpdateLvL	 = 50;
	
	//menu lock
	tUI_CuSetting.ubMenuLock = false;

	tUI_CuSetting.ubKeyLock = false;
	
	//hide channel name
	tUI_CuSetting.ubHideChannelName = false;
	
	//SD Switch
	tUI_CuSetting.ubSD_Switch = false;
	
	//buzzer
	tUI_CuSetting.ubBuzzer = true;

	//audio Source
	tUI_CuSetting.tAdoSrcCamNum_Dual = CAM1;
	tUI_CuSetting.tAdoSrcCamNum_H = CAM1;
	tUI_CuSetting.tAdoSrcCamNum_Quad = CAM1;
	
	//auto dimmer
	tUI_CuSetting.ubAutoDimmer = FALSE;
	tUI_CuSetting.ubAutoDimmer_MinVal = 0;
	tUI_CuSetting.ubAutoDimmer_MaxVal = 99;

	//mute 
	tUI_CuSetting.ubMuteFlag = FALSE;

	//guideLine	
	for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.GuideLineSize[tCamNum] = 10;
		tUI_CuSetting.GuideLineEnable[tCamNum] = FALSE;
		uwTemp = (1024 - 704)/2;
		tUI_CuSetting.GuideLine_XY[tCamNum][0] = uwTemp >> 8;
		tUI_CuSetting.GuideLine_XY[tCamNum][1] = (uint8_t)uwTemp;
		uwTemp = (600 - 4)/2;
		tUI_CuSetting.GuideLine_XY[tCamNum][2] = uwTemp >> 8;
		tUI_CuSetting.GuideLine_XY[tCamNum][3] = (uint8_t)uwTemp;
	}

	// ZoomRedbox
	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.usZoomRedbox[tCamNum][0] = 412;
		tUI_CuSetting.usZoomRedbox[tCamNum][1] = 200;
		tUI_CuSetting.usZoomRedbox[tCamNum][2] = 612;
		tUI_CuSetting.usZoomRedbox[tCamNum][3] = 400;
	}

	// ZoomBSDSmallRedbox
	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][0] = BSD_REDBOX_POINX_X1;
		tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][1] = BSD_REDBOX_POINX_Y1;
		tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][2] = BSD_REDBOX_POINX_X2;
		tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][3] = BSD_REDBOX_POINX_Y2;
	}

	
	// ZoomBSDBigRedbox
	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][0] = BSD_YELLOWBOX_POINX_X1;
		tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][1] = BSD_YELLOWBOX_POINX_Y1;
		tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][2] = BSD_YELLOWBOX_POINX_X2;
		tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][3] = BSD_YELLOWBOX_POINX_Y2;
	}

	//footswitch
	tUI_CuSetting.ubZoomfootswitch = 0;

	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.ubisZoomCameraFlag[0] = 0;
		tUI_CuSetting.ubisZoomCameraFlag[1] = 0;
		tUI_CuSetting.ubisZoomCameraFlag[2] = 0;
		tUI_CuSetting.ubisZoomCameraFlag[3] = 0;
	}

	tUI_CuSetting.tLanguage = LANGUAGE_ENGLISH;
	UI_UpdateDevStatusInfo();
	
	// heating
	tUI_CuSetting.ubiHeatingCamFlag[0] = 0;
	tUI_CuSetting.ubiHeatingCamFlag[1] = 0;
	tUI_CuSetting.ubiHeatingCamFlag[2] = 0;
	tUI_CuSetting.ubiHeatingCamFlag[3] = 0;
	tUI_CuSetting.heating_time[0] = 0;
	tUI_CuSetting.heating_time[1] = 0;
	tUI_CuSetting.heating_time[2] = 0;
	tUI_CuSetting.heating_time[3] = 0;
}
//------------------------------------------------------------------------------
uint8_t UI_CheckUIParameter(void)
{
	UI_CamNum_t tCamNum;
	for(tCamNum = CAM1; tCamNum < CAM_4T; tCamNum++)
	{	
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamDispLocation_Quad,DISP_LOWER_RIGHT + 1,DISP_UPPER_LEFT);
		
		if(tUI_CamStatus[tCamNum].tCamDispLocation_Dual != DISP_LOCATION_TYPE_MAX)
			UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamDispLocation_Dual,DISP_RIGHT + 1,DISP_LEFT);
		
		
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamDispLocation,DISP_H_CL + 1,DISP_H_L);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamPsMode,POWER_NORMAL_MODE + 1,POWER_NORMAL_MODE);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamAnrMode,CAMSET_ON + 1,CAMSET_OFF);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCam3DNRMode,CAMSET_ON + 1,CAMSET_OFF);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamvLDCMode,CAMSET_ON + 1,CAMSET_OFF);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamAecMode,CAMSET_ON + 1,CAMSET_OFF);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamDisMode,CAMSET_ON + 1,CAMSET_OFF);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamFlicker,CAMFLICKER_60HZ + 1,CAMFLICKER_50HZ);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamCbrMode,CAMSET_ON + 1,CAMSET_ON);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamCondenseMode,CAMSET_ON + 1,CAMSET_OFF);

		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL,99 + 1,50);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast,99 + 1,50);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation,99 + 1,50);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue,99 + 1,50);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamImgFlip,CAMIMGFLIP_ENABLE + 1,CAMIMGFLIP_DISABLE);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamImgMirror,CAMIMGFLIP_ENABLE + 1,CAMIMGFLIP_DISABLE);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tZoomCamISP, ZOOMCAMISP_ENABLE + 1,ZOOMCAMISP_DISABLE);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamLaser,CAMLASER_ENABLE + 1,CAMLASER_ENABLE);  //Alan
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamLed,CAMLED_ENABLE + 1,CAMLED_ENABLE);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].ubVolumeLvl,99 + 1,50);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].ubBSD, 1 + 1, 1);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].ubTrace, 1 + 1, 0);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].digitalZoomScale, 50 + 1, 10);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamAntLvl, ANT_SIGNALLVL5 + 1, ANT_NOSIGNAL);
		UI_CHK_MYSYS(tUI_CamStatus[tCamNum].tCamBatLvl, BAT_CHARGE + 1, BAT_NO);
		
		UI_CHK_MYSYS(tUI_CuSetting.ubUpdateTxParam[tCamNum],TRUE + 1,FALSE);


	}
	//AutoScan
	for(tCamNum = 0; tCamNum < 6; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.AutoScanDuty[tCamNum],60 + 1,10);
		UI_CHK_MYSYS(tUI_CuSetting.AutoScanEnable[tCamNum],TRUE + 1,TRUE);
	}
	
	//Trigger
	for(tCamNum = CAM1; tCamNum < 5; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.TriggerDelay[tCamNum],60 + 1,2);
	}
	
	UI_CHK_MYSYS(tUI_CuSetting.TriggerPriority[CAM1],4 + 1,0);
	UI_CHK_MYSYS(tUI_CuSetting.TriggerPriority[CAM2],4 + 1,0);
	UI_CHK_MYSYS(tUI_CuSetting.TriggerPriority[CAM3],4 + 1,0);
	UI_CHK_MYSYS(tUI_CuSetting.TriggerPriority[CAM4],4 + 1,0);
	UI_CHK_MYSYS(tUI_CuSetting.TriggerPriority[4],4 + 1,0);
	
	UI_CHK_MYSYS(tUI_CuSetting.TriggerSplitView,QUALVIEW_ITEM + 1,QUALVIEW_ITEM);
		
	UI_CHK_MYSYS(tUI_CuSetting.tOldPowerOnMode,POWERON_MAX,POWERON_QUAD);
	UI_CHK_MYSYS(tUI_CuSetting.tPowerOnMode,POWERON_MAX,POWERON_QUAD);

	//parkingLine	
		for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{	
		UI_CHK_MYSYS(tUI_CuSetting.ParkingLineSize[tCamNum],11 + 1 ,3);
		UI_CHK_MYSYS(tUI_CuSetting.tParkingLineEnable[tCamNum],PARKING_LINE_MAX,PARKING_LINE_AUTO);
	}
	
	//record
	UI_CHK_MYSYS(tUI_CuSetting.RecInfo.tREC_Time,RECTIME_MAX,RECTIME_5MIN);
	UI_CHK_MYSYS(tUI_CuSetting.ubPowerOnRecord,TRUE + 1,TRUE);
	UI_CHK_MYSYS(tUI_CuSetting.ubTimeStame,TRUE + 1,TRUE);
    //  0  FS_TZ_DST       = 0xD0,             //!< UTC-12:00, Dateline Standard Time
	//  1  FS_TZ_SST       = 0xD4,             //!< UTC-11:00, Samoa Standard Time
	//  2  FS_TZ_HST       = 0xD8,             //!< UTC-10:00, Hawaii Standard Time
	//  3  FS_TZ_ALASKAST  = 0xDC,             //!< UTC-09:00, Alaska Standard Time
	//  4  FS_TZ_PST       = 0xE0,             //!< UTC-08:00, Pacific Standard Time
	//  5  FS_TZ_MOUNTST   = 0xE4,             //!< UTC-07:00, Mountain Standard Time
	//  6  FS_TZ_CST       = 0xE8,             //!< UTC-06:00, Central Standard Time
	//  7  FS_TZ_EST       = 0xEC,             //!< UTC-05:00, Eastern Standard Time
	//  8  FS_TZ_ATLANTST  = 0xF0,             //!< UTC-04:00, Atlantic Standard time
	//  9  FS_TZ_NST       = 0xF2,             //!< UTC-03:30, Newfoundland Standard Time
	// 10  FS_TZ_GREENLANDST = 0xF4,           //!< UTC-03:00, Greenland Standard Time
	// 11  FS_TZ_MAST      = 0xF8,             //!< UTC-02:00, Mid-Atlantic Standard Time
	// 12  FS_TZ_AZORESST  = 0xFC              //!< UTC-01:00, Azores Standard Time
	// 13  FS_TZ_GMT       = 0x80,             //!< UTC=GMT, Greenwich Standard Time
	// 14  FS_TZ_CET       = 0x84,             //!< UTC+01:00, Central Europe Time
	// 15  FS_TZ_EEST      = 0x88,             //!< UTC+02:00, Eastern Europe Standard Time
	// 16  FS_TZ_MST       = 0x8C,             //!< UTC+03:00, Moscow Standard Time
	// 17  FS_TZ_AST       = 0x90,             //!< UTC+04:00, Arabian Standard Time
	// 18  FS_TZ_WAST      = 0x94,             //!< UTC+05:00, West Asia Standard Time
	// 19  FS_TZ_CAST      = 0x98,             //!< UTC+06:00, Central Asia Standard Time
	// 20  FS_TZ_NAST      = 0x9C,             //!< UTC+07:00, North Asia Standard Time
	// 21  FS_TZ_NAEST     = 0xA0,             //!< UTC+08:00, North Asia East Standard Time,GMT+08:00
	// 22  FS_TZ_TOKYOST   = 0xA4,             //!< UTC+09:00, Tokyo Standard Time
	// 23  FS_TZ_WPST      = 0xA8,             //!< UTC+10:00, West Pacific Standard Time
	// 24  FS_TZ_CPST      = 0xAC,             //!< UTC+11:00, Central Pacific Standard Time
	// 25  FS_TZ_NZST      = 0xB0,             //!< UTC+12:00, New Zealand Standard Time
	// 26  FS_TZ_TONGAST   = 0xB4,             //!< UTC+13:00, Tonga Standard Time
    //time zone
	UI_CHK_MYSYS(tUI_CuSetting.ubtimezone,27, 21);

	//backlight
	UI_CHK_MYSYS(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,99 + 1,99);
	
	//volume
	UI_CHK_MYSYS(tUI_CuSetting.VolLvL.ulVOL_UpdateLvL,99 + 1,50);
	
	//menu lock	
	UI_CHK_MYSYS(tUI_CuSetting.ubMenuLock,TRUE + 1,FALSE);

	UI_CHK_MYSYS(tUI_CuSetting.ubKeyLock,TRUE + 1,FALSE);
	
	//hide channel name
	UI_CHK_MYSYS(tUI_CuSetting.ubHideChannelName,TRUE + 1,FALSE);

	//SD Switch
	UI_CHK_MYSYS(tUI_CuSetting.ubSD_Switch,TRUE + 1,FALSE);
	
	//buzzer
	UI_CHK_MYSYS(tUI_CuSetting.ubBuzzer,TRUE + 1,TRUE);

	//audio Source
	UI_CHK_MYSYS(tUI_CuSetting.tAdoSrcCamNum_Dual,CAM4 + 1,CAM1);
	UI_CHK_MYSYS(tUI_CuSetting.tAdoSrcCamNum_H,CAM4+ 1,CAM1);
	UI_CHK_MYSYS(tUI_CuSetting.tAdoSrcCamNum_Quad,CAM4 + 1,CAM1);
	
	//auto dimmer
	UI_CHK_MYSYS(tUI_CuSetting.ubAutoDimmer,TRUE + 1,FALSE);
	UI_CHK_MYSYS(tUI_CuSetting.ubAutoDimmer_MinVal,99 + 1,0);
	UI_CHK_MYSYS(tUI_CuSetting.ubAutoDimmer_MaxVal,99 + 1,99);

	//mute 
	UI_CHK_MYSYS(tUI_CuSetting.ubMuteFlag,TRUE + 1,FALSE);

	//guideLine	
	for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.GuideLineSize[tCamNum],17 + 1,10);
		UI_CHK_MYSYS(tUI_CuSetting.GuideLineEnable[tCamNum],TRUE + 1,FALSE);
	}

	// ZoomRedbox
	for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.usZoomRedbox[tCamNum][0],1025,412);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomRedbox[tCamNum][1],601,200);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomRedbox[tCamNum][2],1025,612);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomRedbox[tCamNum][3],601,400);
	}

	for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][0],1025,BSD_REDBOX_POINX_X1);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][1],601,BSD_REDBOX_POINX_Y1);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][2],1025,BSD_REDBOX_POINX_X2);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDSmallRedbox[tCamNum][3],601,BSD_REDBOX_POINX_Y2);
	}

	for(tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][0],1025,BSD_YELLOWBOX_POINX_X1);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][1],601,BSD_YELLOWBOX_POINX_Y1);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][2],1025,BSD_YELLOWBOX_POINX_X2);
		UI_CHK_MYSYS(tUI_CuSetting.usZoomBSDBigRedbox[tCamNum][3],601,BSD_YELLOWBOX_POINX_Y2);
	}

	//footswitch
	UI_CHK_MYSYS(tUI_CuSetting.ubZoomfootswitch,2,0);

	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		UI_CHK_MYSYS(tUI_CuSetting.ubisZoomCameraFlag[0],2,0);
		UI_CHK_MYSYS(tUI_CuSetting.ubisZoomCameraFlag[1],2,0);
		UI_CHK_MYSYS(tUI_CuSetting.ubisZoomCameraFlag[2],2,0);
		UI_CHK_MYSYS(tUI_CuSetting.ubisZoomCameraFlag[3],2,0);
	}

	//language
	UI_CHK_MYSYS(tUI_CuSetting.tLanguage,LANGUAGE_MAX,LANGUAGE_ENGLISH);
	
	// heating
	UI_CHK_MYSYS(tUI_CuSetting.heating_time[0], 3, 0);
	UI_CHK_MYSYS(tUI_CuSetting.heating_time[1], 3, 0);
	UI_CHK_MYSYS(tUI_CuSetting.heating_time[2], 3, 0);
	UI_CHK_MYSYS(tUI_CuSetting.heating_time[3], 3, 0);
	return rUI_SUCCESS;
}
//------------------------------------------------------------------------------
void UI_LoadDevStatusInfo(void)
{
	uint32_t ulUI_SFAddr = pSF_Info->ulSize - (UI_SF_START_SECTOR * pSF_Info->ulSecSize);
	UI_DeviceStatusInfo_t tUI_DevStsInfo = {{0}, {0}, {0}, {0}};
	UI_CamNum_t tCamNum;

	SF_Read(ulUI_SFAddr, sizeof(UI_DeviceStatusInfo_t), (uint8_t *)&tUI_DevStsInfo);
	memcpy(tUI_CamStatus, tUI_DevStsInfo.tCAM_StatusInfo, (CAM_4T * sizeof(UI_CamStatus_t)));
	memcpy(&tUI_CuSetting, &tUI_DevStsInfo.tCU_SettingInfo, sizeof(UI_CUSetting_t));
	printd(DBG_InfoLvl, "UI TAG:%s\n",tUI_DevStsInfo.cbUI_DevStsTag);
	printd(DBG_InfoLvl, "UI VER:%s\n",tUI_DevStsInfo.cbUI_FwVersion);

	if((strncmp(tUI_DevStsInfo.cbUI_DevStsTag, SF_AP_UI_SECTOR_TAG, sizeof(tUI_DevStsInfo.cbUI_DevStsTag) - 1)) ||
	   (strncmp(tUI_DevStsInfo.cbUI_FwVersion, SN937XX_FW_VERSION,  sizeof(tUI_DevStsInfo.cbUI_FwVersion) - 1)))
	{		
		printf("--------------------------------------------------------------------------------------------\n");
		printf("-----------------------------------device reset---------------------------------------------\n");
		printf("--------------------------------------------------------------------------------------------\n");
		/****************************************tUI_CamStatus*******************************************/
		UI_ResetUIParameter();
		
		SF_Read(ulUI_SFAddr, sizeof(UI_DeviceStatusInfo_t), (uint8_t *)&tUI_DevStsInfo);
		memcpy(tUI_CamStatus, tUI_DevStsInfo.tCAM_StatusInfo, (CAM_4T * sizeof(UI_CamStatus_t)));
		memcpy(&tUI_CuSetting, &tUI_DevStsInfo.tCU_SettingInfo, sizeof(UI_CUSetting_t));
		printd(DBG_InfoLvl, "UI TAG:%s\n",tUI_DevStsInfo.cbUI_DevStsTag);
		printd(DBG_InfoLvl, "UI VER:%s\n",tUI_DevStsInfo.cbUI_FwVersion);
	}
	//检查参数是否有效
	if(UI_CheckUIParameter() == rUI_FAIL)
		UI_ResetUIParameter();
		
	tUI_CuSetting.ubTotalCamNum 			 = DISPLAY_MODE;

	tUI_CuSetting.IconSts.ubRdPairIconFlag   = FALSE;
	tUI_CuSetting.IconSts.ubClearThdCntFlag	 = FALSE;
	tUI_CuSetting.IconSts.ubShowLostLogoFlag = FALSE;
	tUI_CuSetting.IconSts.ubDrawMdTrigFlag 	 = FALSE;
	tUI_CuSetting.ubPairedCamNum			 = 0;
	tUI_CuSetting.tCamsBwMode				 = CAMS_4T_BWMODE;

	
	tUI_CuSetting.RecInfo.tREC_Mode = REC_LOOPING;
	tUI_CuSetting.tVdoMode = UI_RECORDING_MODE;

	
		
    if(tUI_CuSetting.RecInfo.tREC_Time == RECTIME_1MIN)
        REC_TimeSet(0,60);
    else if(tUI_CuSetting.RecInfo.tREC_Time == RECTIME_3MIN)
        REC_TimeSet(0,180);
    else if(tUI_CuSetting.RecInfo.tREC_Time == RECTIME_5MIN)
        REC_TimeSet(0,300);

	for(tCamNum = CAM1; tCamNum < CAM_4T; tCamNum++)
	{
		if(INVALID_ID != tUI_CamStatus[tCamNum].ulCAM_ID)
		{
			tUI_CuSetting.ubPairedCamNum += 1;
			tUI_CamStatus[tCamNum].tCamConnSts = CAM_OFFLINE;
		}
		tUI_CamStatus[tCamNum].cCamVersion[0] = '\0';
		tUI_CamStatus[tCamNum].ulFrameCount = 0;
		tUI_CamStatus[tCamNum].ulImageStuckCount = 0;
	}

}
//------------------------------------------------------------------------------
void UI_UpdateTxDevStatusInfo(UI_CamNum_t tCamNum)
{
	uint8_t ubTryCount;
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_SAVEPARAMETERS;
	tCamSetCmd.ubCmd_Len				 = 4;
	
	ubTryCount = 5;
	while(--ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("Camera UI_SAVEPARAMETERS Fail !\n");
			osDelay(20);	
		}
		else
			break;
	}
	printf("UI_UpdateTxDevStatusInfo tCamNum = %d success\n",tCamNum);
}


//------------------------------------------------------------------------------
void UI_ResetDigitalZoomScale(UI_CamNum_t chn)
{
	uint16_t sum;
	uint8_t val = 10;

	sum = 0x92 + val;
	pelco_digital_zoom[3] = val;
	pelco_digital_zoom[6] = (uint8_t)(sum&0x00FF);
	tTWC_Send(TWC_STA1 + chn, TWC_PELCO, (uint8_t *)&pelco_digital_zoom[0], 7, 1);
}


//------------------------------------------------------------------------------
void UI_UpdateDevStatusInfo(void)
{
	uint32_t ulUI_SFAddr = pSF_Info->ulSize - (UI_SF_START_SECTOR * pSF_Info->ulSecSize);
	UI_DeviceStatusInfo_t tUI_DevStsInfo = {{0}, {0}, {0}, {0}};
	
	osMutexWait(APP_UpdateMutex, osWaitForever);
	memcpy(tUI_DevStsInfo.cbUI_DevStsTag, SF_AP_UI_SECTOR_TAG, sizeof(tUI_DevStsInfo.cbUI_DevStsTag) - 1);
	memcpy(tUI_DevStsInfo.cbUI_FwVersion, SN937XX_FW_VERSION, sizeof(tUI_DevStsInfo.cbUI_FwVersion) - 1);
	memcpy(tUI_DevStsInfo.tCAM_StatusInfo, tUI_CamStatus, (CAM_4T * sizeof(UI_CamStatus_t)));
	memcpy(&tUI_DevStsInfo.tCU_SettingInfo, &tUI_CuSetting, sizeof(UI_CUSetting_t));
	SF_DisableWrProtect();
	SF_Erase(SF_SE, ulUI_SFAddr, pSF_Info->ulSecSize, 1);
	SF_Write(ulUI_SFAddr, sizeof(UI_DeviceStatusInfo_t), (uint8_t *)&tUI_DevStsInfo);
	SF_EnableWrProtect();
	osMutexRelease(APP_UpdateMutex);
}


//------------------------------------------------------------------------------
void UI_CheckScanModeCount(void)
{
	UI_Event_t tScanEvent;
	osMessageQId *pUI_ScanEventQH = NULL;
	if(ulAutoScanCount == 0 && ubAutoScanReady && !MenuOnFlag && !ubUI_CuStandbyFlag && !ubUI_CuPowerDiscFlag && !TriggerLock && (tUI_ViewModeSel == SCANVIEW_ITEM))
	{
		ubAutoScanReady = FALSE;
		tScanEvent.tEventType = SCANMODE_EVENT;
		tScanEvent.pvEvent 	  = NULL;
		pUI_ScanEventQH 	  = pUI_GetEventQueueHandle();
	    osMessagePut(*pUI_ScanEventQH, &tScanEvent, 0);
	}
}
//------------------------------------------------------------------------------
void UI_ScanModeExec(void)
{
	printf("UI_ScanModeExec\n");
	UI_CamNum_t tUI_ViewSel;
	
	if(MenuOnFlag || TriggerLock || ubUI_CuStandbyFlag || tUI_ViewModeSel != SCANVIEW_ITEM)
	{
		return;
	}
	
	tUI_AutoScanNumSel ++;
	if(tUI_AutoScanNumSel >= 6)
		tUI_AutoScanNumSel = (UI_CamNum_t)0; 
	for(tUI_ViewSel = tUI_AutoScanNumSel; tUI_ViewSel < 6; tUI_ViewSel++)
	{
		if(!tUI_CuSetting.AutoScanEnable[tUI_ViewSel])
		{
			//最后一个画面都没有使能就回到最开始的画面
			if(tUI_ViewSel == 5)
			{
				tUI_ViewSel = 0;
				if(!tUI_CuSetting.AutoScanEnable[tUI_ViewSel])
					continue;
			}
			else
				continue;
		}
		
		DeskTopShowView = tUI_ViewSel;
		UI_SwitchViewType(tUI_ViewSel,TRUE);
		if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
			UI_SaveLastMode();
		
		UI_ClearOsdImageNoUpdate();
		UI_DrawDesktopIcon();
		break;
		
	}
	tUI_AutoScanNumSel = tUI_ViewSel;
	printf("UI_ScanModeExec tUI_ViewSel = %d\n",tUI_ViewSel);
	ulAutoScanCount = tUI_CuSetting.AutoScanDuty[tUI_AutoScanNumSel]* 1000;
	ubAutoScanReady = TRUE;
}

//------------------------------------------------------------------------------
void UI_SwitchCameraSource(void)
{
	APP_EventMsg_t tUI_SwitchBuMsg = {0};

	for(uint8_t i = 0;i < 4;i++)
	{
		tUI_CamStatus[i].tCamConnSts = CAM_OFFLINE;
		ubCamDiscCount[i] = 0;
		tUI_CamStatus[i].tCamAntLvl = ANT_NOSIGNAL;
		tUI_CamStatus[i].ulFrameCount = 0;
		tUI_CamStatus[i].ulImageStuckCount = 0;
	}
	
	tUI_SwitchBuMsg.ubAPP_Event 	 = APP_VIEWTYPECHG_EVENT;
	tUI_SwitchBuMsg.ubAPP_Message[0] = 5;		//! Message Length	
	tUI_SwitchBuMsg.ubAPP_Message[1] = tCamViewSel.tCamViewType;
	tUI_SwitchBuMsg.ubAPP_Message[2] = tCamViewSel.tCamViewPool[0];
	tUI_SwitchBuMsg.ubAPP_Message[3] = tCamViewSel.tCamViewPool[1];
	tUI_SwitchBuMsg.ubAPP_Message[4] = tCamViewSel.tCamViewPool[2];
	tUI_SwitchBuMsg.ubAPP_Message[5] = tCamViewSel.tCamViewPool[3];
	tUI_SwitchBuMsg.ubAPP_Message[6] = tUI_CuSetting.ubSD_Switch;
	UI_SendMessageToAPP(&tUI_SwitchBuMsg);
	printf("---------------UI_SwitchCameraSource\n");

}

//------------------------------------------------------------------------------
void UI_SwitchAudioSource(UI_CamNum_t tCamNum)
{
	APP_EventMsg_t tUI_SwitchAdoSrcMsg = {0};

	tUI_SwitchAdoSrcMsg.ubAPP_Event 	 = APP_ADOSRCSEL_EVENT;
	tUI_SwitchAdoSrcMsg.ubAPP_Message[0] = 1;		//! Message Length
	tUI_SwitchAdoSrcMsg.ubAPP_Message[1] = tCamNum;
	tUI_SwitchAdoSrcMsg.ubAPP_Message[2] = FALSE;	//! (PS_ADOONLY_MODE == tUI_CuSetting.tPsMode)?FALSE:TRUE;
	UI_SendMessageToAPP(&tUI_SwitchAdoSrcMsg);
}
//------------------------------------------------------------------------------
void UI_DrawDesktopIcon(void)
{
	OSD_IMG_INFO tOsdImgInfo,tVoiceOsdImgInfo;
	uint16_t uwLcd_HSize = uwLCD_GetLcdHoSize();
	uint16_t uwLcd_VSize = uwLCD_GetLcdVoSize();
	UI_CamViewType_t tUI_CamViewType = tCamViewSel.tCamViewType;
	uint16_t uwCam1WordOsdIndex = (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DESKTOP_CAM1_WORD:OSD2IMG_DESKTOP_CAM1_WORD_GER;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_VOICE_MUTE + 1 - tUI_CuSetting.ubMuteFlag, 1, &tVoiceOsdImgInfo);
	switch(tUI_CamViewType)
	{
		case SINGLE_VIEW:
			//voice
			tVoiceOsdImgInfo.uwXStart+= uwLcd_HSize/2;
			tOSD_Img2(&tVoiceOsdImgInfo, OSD_UPDATE);
			if(!tUI_CuSetting.ubHideChannelName)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[0], 1, &tOsdImgInfo);
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			}
			//倒车线
			if(tUI_CuSetting.tParkingLineEnable[tCamViewSel.tCamViewPool[0]] == PARKING_LINE_ON)
			{
				UI_DrawParkingLine(tCamViewSel.tCamViewPool[0],OSD_UPDATE);
			}
			//引导线
			if(tUI_CuSetting.GuideLineEnable[tCamViewSel.tCamViewPool[0]])
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GUIDELINE_SIZE1 + tUI_CuSetting.GuideLineSize[tCamViewSel.tCamViewPool[0]], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart = (tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][0] << 8) + tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][1];
				tOsdImgInfo.uwYStart = (tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][2] << 8) + tUI_CuSetting.GuideLine_XY[tCamViewSel.tCamViewPool[0]][3];
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);				
			}
			break;
		case DUAL_VIEW:
			if(tUI_CuSetting.tAdoSrcCamNum_Dual == tCamViewSel.tCamViewPool[0])//voice
				tOSD_Img2(&tVoiceOsdImgInfo, OSD_QUEUE);
			else if(tUI_CuSetting.tAdoSrcCamNum_Dual == tCamViewSel.tCamViewPool[1])
			{
				tVoiceOsdImgInfo.uwXStart += uwLcd_HSize/2;
				tOSD_Img2(&tVoiceOsdImgInfo, OSD_QUEUE);
			}
			
			if(!tUI_CuSetting.ubHideChannelName)
			{
				//LEFT
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[0], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart -= uwLcd_HSize/4;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
				//RIGHT
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[1], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart += uwLcd_HSize/4;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			}
			//LINE
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_LINE_DUAL, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			break;
		case QUAD_VIEW:
			if(tUI_CuSetting.tAdoSrcCamNum_Quad == tCamViewSel.tCamViewPool[0])//voice
			{
				tVoiceOsdImgInfo.uwYStart -= uwLcd_VSize/2;
				tOSD_Img2(&tVoiceOsdImgInfo, OSD_QUEUE);
			}			
			else if(tUI_CuSetting.tAdoSrcCamNum_Quad == tCamViewSel.tCamViewPool[1])
			{
				tVoiceOsdImgInfo.uwYStart -= uwLcd_VSize/2;
				tVoiceOsdImgInfo.uwXStart += uwLcd_HSize/2;
				tOSD_Img2(&tVoiceOsdImgInfo, OSD_QUEUE);
			}
			else if(tUI_CuSetting.tAdoSrcCamNum_Quad == tCamViewSel.tCamViewPool[2])
			{
				tOSD_Img2(&tVoiceOsdImgInfo, OSD_QUEUE);
			}
			else if(tUI_CuSetting.tAdoSrcCamNum_Quad == tCamViewSel.tCamViewPool[3])
			{
				tVoiceOsdImgInfo.uwXStart += uwLcd_HSize/2;
				tOSD_Img2(&tVoiceOsdImgInfo, OSD_QUEUE);
			}
			
			if(!tUI_CuSetting.ubHideChannelName)
			{
				//UPPER LEFT
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[0], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart -= uwLcd_HSize/4;
				tOsdImgInfo.uwYStart -= uwLcd_VSize/2;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
				//UPPER RIGHT
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[1], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart += uwLcd_HSize/4;
				tOsdImgInfo.uwYStart -= uwLcd_VSize/2;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
				//LOWER LEFT
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[2], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart -= uwLcd_HSize/4;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
				//LOWER RIGHT
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwCam1WordOsdIndex + tCamViewSel.tCamViewPool[3], 1, &tOsdImgInfo);
				tOsdImgInfo.uwXStart += uwLcd_HSize/4;
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			}
			//LINE
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_LINE_QUAD_H, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_LINE_QUAD_V, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_DrawParkingLine(uint8_t ubCamNum,OSD_UPDATE_TYP tMode)
{
	OSD_IMG_INFO tOsdImgInfo_left,tOsdImgInfo_right,tOsdImgInfo_upper,tOsdImgInfo_lower;
	//left
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LEFT, 1, &tOsdImgInfo_left);
	tOsdImgInfo_left.uwXStart = (tUI_CuSetting.ParkingLine_XY[ubCamNum][0] << 8) + tUI_CuSetting.ParkingLine_XY[ubCamNum][1];
	tOsdImgInfo_left.uwYStart = (tUI_CuSetting.ParkingLine_XY[ubCamNum][2] << 8) + tUI_CuSetting.ParkingLine_XY[ubCamNum][3];
	tOSD_Img2(&tOsdImgInfo_left, OSD_QUEUE);
	//upper
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_UPPER1 + tUI_CuSetting.ParkingLineSize[ubCamNum], 1, &tOsdImgInfo_upper);
	tOsdImgInfo_upper.uwXStart = tOsdImgInfo_left.uwXStart + tOsdImgInfo_left.uwHSize;
	tOsdImgInfo_upper.uwYStart = tOsdImgInfo_left.uwYStart;
	tOSD_Img2(&tOsdImgInfo_upper, OSD_QUEUE);
	//lower
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LOWER1 + tUI_CuSetting.ParkingLineSize[ubCamNum], 1, &tOsdImgInfo_lower);
	tOsdImgInfo_lower.uwXStart = tOsdImgInfo_left.uwXStart + tOsdImgInfo_left.uwHSize;
	tOsdImgInfo_lower.uwYStart = tOsdImgInfo_left.uwYStart + tOsdImgInfo_left.uwVSize - tOsdImgInfo_lower.uwVSize; 
	tOSD_Img2(&tOsdImgInfo_lower, OSD_QUEUE);
	//right
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_RIGHT, 1, &tOsdImgInfo_right);
	tOsdImgInfo_right.uwXStart = tOsdImgInfo_upper.uwXStart + tOsdImgInfo_upper.uwHSize;
	tOsdImgInfo_right.uwYStart = tOsdImgInfo_upper.uwYStart;
	tOSD_Img2(&tOsdImgInfo_right, tMode);
	
}
//------------------------------------------------------------------------------
void UI_EraseParkingLine(uint8_t ubCamNum,OSD_UPDATE_TYP tMode)
{
	OSD_IMG_INFO tOsdImgInfo_left,tOsdImgInfo_right,tOsdImgInfo_upper,tOsdImgInfo_lower;
	//left
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LEFT, 1, &tOsdImgInfo_left);
	tOsdImgInfo_left.uwXStart = (tUI_CuSetting.ParkingLine_XY[ubCamNum][0] << 8) + tUI_CuSetting.ParkingLine_XY[ubCamNum][1];
	tOsdImgInfo_left.uwYStart = (tUI_CuSetting.ParkingLine_XY[ubCamNum][2] << 8) + tUI_CuSetting.ParkingLine_XY[ubCamNum][3];
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo_left);
	//upper
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_UPPER1 + tUI_CuSetting.ParkingLineSize[ubCamNum], 1, &tOsdImgInfo_upper);
	tOsdImgInfo_upper.uwXStart = tOsdImgInfo_left.uwXStart + tOsdImgInfo_left.uwHSize;
	tOsdImgInfo_upper.uwYStart = tOsdImgInfo_left.uwYStart;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo_upper);
	//lower
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LOWER1 + tUI_CuSetting.ParkingLineSize[ubCamNum], 1, &tOsdImgInfo_lower);
	tOsdImgInfo_lower.uwXStart = tOsdImgInfo_left.uwXStart + tOsdImgInfo_left.uwHSize;
	tOsdImgInfo_lower.uwYStart = tOsdImgInfo_left.uwYStart + tOsdImgInfo_left.uwVSize - tOsdImgInfo_lower.uwVSize;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo_lower);
	//right
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_RIGHT, 1, &tOsdImgInfo_right);
	tOsdImgInfo_right.uwXStart = tOsdImgInfo_upper.uwXStart + tOsdImgInfo_upper.uwHSize;
	tOsdImgInfo_right.uwYStart = tOsdImgInfo_upper.uwYStart;
	if(tMode == OSD_UPDATE)
		OSD_EraserImg2(&tOsdImgInfo_right);
	else
		OSD_EraserImg2_NoUpdate(&tOsdImgInfo_right);

}
//------------------------------------------------------------------------------
void UI_SwitchViewType(UI_CamNum_t tUI_CamSel,uint8_t ubClosePanel)
{	
	if(!ubUI_CuPowerDiscFlag)
	{
		osSemaphoreWait(osUI_UpdateStatusCtr, osWaitForever);	
		osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
	}
	printf("UI_SwitchViewType tUI_CamSel = %d\n",tUI_CamSel);
	ubUI_FinishViewSwitch = FALSE;
	UI_CamViewType_t tCamViewTypeSel;
	if(ubClosePanel)
	{
		ubUI_ReOpenPanel = TRUE;
		UI_PanelOff();
	}
	if(UI_REC_START == tUI_RecPlayAct.tRecAct)
	{
		UI_VideoRecordingExec(UI_REC_STOP);
		ubUI_RestartRec = TRUE;
	}

	//H画面的位置设置还是按照原厂的设置方法，原厂的代码一开始是不支持dual画面和quad画面的位置设置，是后期自己加的
	if(tUI_CamSel <= CAM4)
	{
		tCamViewSel.tCamViewPool[0] = tUI_CamSel;

	}
	else if(tUI_CamSel == DUALVIEW_ITEM)//dual
	{
		for(int i = 0;i < 4;i++)
		{
			if(tUI_CamStatus[i].tCamDispLocation_Dual == DISP_LEFT)
				tCamViewSel.tCamViewPool[0] = (UI_CamNum_t)i;			//tCamViewSel.tCamViewPool[0]存的是dual_left的tx序号
			if(tUI_CamStatus[i].tCamDispLocation_Dual == DISP_RIGHT)
				tCamViewSel.tCamViewPool[1] = (UI_CamNum_t)i;			//tCamViewSel.tCamViewPool[1]存的是dual_right的tx序号
		}
			
	}
	else if(tUI_CamSel == QUALVIEW_ITEM)//quad
	{
		for(int i = 0;i < 4;i++)
		{
			if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_UPPER_LEFT)
				tCamViewSel.tCamViewPool[0] = (UI_CamNum_t)i;			//tCamViewSel.tCamViewPool[0]存的是DISP_UPPER_LEFT的tx序号
			if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_UPPER_RIGHT)
				tCamViewSel.tCamViewPool[1] = (UI_CamNum_t)i;			//tCamViewSel.tCamViewPool[1]存的是DISP_UPPER_RIGHT的tx序号
			if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_LOWER_LEFT)
				tCamViewSel.tCamViewPool[2] = (UI_CamNum_t)i;			//tCamViewSel.tCamViewPool[1]存的是DISP_LOWER_LEFT的tx序号
			if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_LOWER_RIGHT)
				tCamViewSel.tCamViewPool[3] = (UI_CamNum_t)i;			//tCamViewSel.tCamViewPool[1]存的是DISP_LOWER_RIGHT的tx序号
		}

	}
	
	if(tUI_CamSel <= CAM4)
		tCamViewTypeSel = SINGLE_VIEW;
	else if(tUI_CamSel == DUALVIEW_ITEM)
		tCamViewTypeSel = DUAL_VIEW;
	else if(tUI_CamSel == QUALVIEW_ITEM)
		tCamViewTypeSel = QUAD_VIEW;
	
	tCamViewSel.tCamViewType = tCamViewTypeSel;
	
	UI_SwitchCameraSource();
	//切换音频
	UI_SwitchAudioSource(UI_GetAudioSrc());

	if(!ubUI_CuStandbyFlag || (ubUI_CuStandbyFlag == TRUE && TriggerLock == TRUE))
	{
		UI_AudioOn();
	}

}

void UI_PelcoZoomEventExec(void *pvTriggerEvent)
{
	uint8_t u_TryCnt = 5;
    TRIGGER_EVENT_t * ptTriggerEvent = (TRIGGER_EVENT_t*)pvTriggerEvent;
	UI_CamNum_t tUI_ViewSel = ptTriggerEvent->tUI_ViewSel;
	//先查询ISP翻转，置位ISP标志位
	if(tUI_CuSetting.ubisZoomCameraFlag[tUI_ViewSel])
	{
		u_TryCnt = 5;
		while(u_TryCnt--)
		{
			printf("UI_PelcoZoomEventExec CheckPictureMirrorFlip\n");
			if(tTWC_Send(tUI_ViewSel, TWC_PELCO, (uint8_t *)&CheckPictureMirrorFlip[0], 7, 10) == TWC_SUCCESS)
				break;
			osDelay(10);
		} //查询摄像头镜像和翻转
	}
	
	if(ubisShowZoomflag)
	{
		u_TryCnt = 5;
		while(u_TryCnt--)
		{
			printf("UI_PelcoZoomEventExec CheckSensorStatus\n");
			if(tTWC_Send(tUI_ViewSel, TWC_PELCO, (uint8_t *)&CheckSensorStatus[0], 7, 10) == TWC_SUCCESS)
				break;
			osDelay(10);
		} //get BSD and Trace status


		u_TryCnt = 5;
		while(u_TryCnt--)
		{
			printf("UI_PelcoZoomEventExec CheckAlarmRoi\n");
			if(tTWC_Send(tUI_ViewSel, TWC_PELCO, (uint8_t *)&CheckAlarmRoi[0], 7, 10) == TWC_SUCCESS)
				break;
			osDelay(10);
		} //查询行人检测区域坐标


	}
//	printf("UI_PelcoZoomEventExec tUI_CuSetting.ubisZoomCameraFlag[%d] = %d\n", tUI_ViewSel, tUI_CuSetting.ubisZoomCameraFlag[tUI_ViewSel]);
	
}


void UI_TriggerEventExec(void *pvTriggerEvent)
{
	printf("UI_TriggerEventExec\n");
	TRIGGER_EVENT_t * ptTriggerEvent = (TRIGGER_EVENT_t*)pvTriggerEvent;
	UI_CamNum_t tUI_ViewSel = ptTriggerEvent->tUI_ViewSel;
	
	
//	DeskTopShowView = tUI_ViewSel;
			
	if(!TriggerLock && ubUI_CuStandbyFlag)
		UI_DisconnectTx();
	else
		UI_SwitchViewType(tUI_ViewSel,TRUE);
	
	UI_ClearOsdImageNoUpdate();
	UI_DrawDesktopIcon();
	//倒车光标
	if(TriggerLock && tUI_ViewSel <= CAM4 && !(tUI_CuSetting.ubZoomfootswitch))
	{
		if(tUI_CuSetting.tParkingLineEnable[tUI_ViewSel] == PARKING_LINE_AUTO)
		{
			UI_DrawParkingLine(tUI_ViewSel,OSD_UPDATE);
		}
	}
	
}

//------------------------------------------------------------------------------
void UI_EngModeKey(void)
{
}
//------------------------------------------------------------------------------
void UI_EngModeCtrl(UI_ArrowKey_t tArrowKey)
{
	pvUiFuncPtr UI_EngFuncPrt[] = {[UP_ARROW] 	 = EN_UpKey,
								   [DOWN_ARROW]	 = EN_DownKey,
								   NULL, NULL,
								   [ENTER_ARROW] = EN_EnterKey,
								   NULL};
	switch(tArrowKey)
	{
		case EXIT_ARROW:
		{
			EN_OpenEnMode(FALSE);
			tUI_State = UI_DISPLAY_STATE;
			break;
		}
		default:
			if(UI_EngFuncPrt[tArrowKey])
				UI_EngFuncPrt[tArrowKey]();
			break;
	}
}
//------------------------------------------------------------------------------
void UI_DisplayAppPairingScreen(void)
{
	OSD_IMG_INFO tOsdImgInfo;

	ubUI_StopUpdateStsBarFlag = TRUE;
	UI_ClearOsdImage();
	tUI_State = UI_PAIRING_STATE;
}
//------------------------------------------------------------------------------
void UI_PairingControl(UI_ArrowKey_t tArrowKey)
{
	APP_EventMsg_t tUI_PairMessage = {0};

	tUI_PairMessage.ubAPP_Event = APP_PAIRING_STOP_EVENT;
	UI_SendMessageToAPP(&tUI_PairMessage);
}

void UI_CheckIsPairing(void)
{
	if (APP_PAIRING_STATE == tUI_SyncAppState)
	{
		APP_EventMsg_t tUI_PairMessage = {0};

		tUI_PairMessage.ubAPP_Event = APP_PAIRING_STOP_EVENT;
		UI_SendMessageToAPP(&tUI_PairMessage);

		osDelay(500);
	}
}
//------------------------------------------------------------------------------
void UI_FwUpgViaSdCard(void)
{
	OSD_IMG_INFO tOsdImgInfo[5];

	if(UI_DISPLAY_STATE != tUI_State)
		return;
/*
	if(tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FWUSDUPGDIAG_ICON, 5, &tOsdImgInfo[0]) != OSD_OK)
	{
		printd(DBG_ErrorLvl, "Load OSD Image FAIL, pls check (%d) !\n", __LINE__);
		return;
	}
	UI_ClearOsdImage();
	OSD_Weight(OSD_WEIGHT_8DIV8);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[4], OSD_UPDATE);
	tUI_State = UI_SDFWUPG_STATE;
*/

}

void UI_MDTriggerSetting(void)
{
	//#define MD_H_WINDOWSIZE		48
	//#define MD_V_WINDOWSIZE		64
	#define MD_H_WINDOWSIZE		64	// 1024/64=16
	#define MD_V_WINDOWSIZE		48	// 600/40=15

	uint32_t ulLcd_HSize = uwLCD_GetLcdHoSize();
	uint32_t ulLcd_VSize = uwLCD_GetLcdVoSize();
	//uint8_t ubMD_V_WinNum = ulLcd_VSize / MD_V_WINDOWSIZE;
	//uint8_t ubMD_H_WinNum = ulLcd_HSize / MD_H_WINDOWSIZE;
	uint16_t uwMD_StartIdx = 2;
	uint8_t ubMD_RightCnt = 2;
	uint8_t ubMD_DownCnt = 2;
	
	{
		UI_CUReqCmd_t tMdCmd;
		
		printd(DBG_ErrorLvl, "UI_MD_SETTING (%04X) - (%d, %d)\n", uwMD_StartIdx, ubMD_RightCnt, ubMD_DownCnt);

		tMdCmd.tDS_CamNum 				= tCamViewSel.tCamViewPool[0];
		tMdCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
		tMdCmd.ubCmd[UI_SETTING_ITEM]   = UI_MD_SETTING;
		tMdCmd.ubCmd[UI_SETTING_DATA]   = (uwMD_StartIdx & 0xFF);
		tMdCmd.ubCmd[UI_SETTING_DATA+1] = (uwMD_StartIdx >> 8);
		tMdCmd.ubCmd[UI_SETTING_DATA+2] = ubMD_RightCnt;
		tMdCmd.ubCmd[UI_SETTING_DATA+3] = ubMD_DownCnt;
		tMdCmd.ubCmd_Len  				= 6;
		
		if (UI_SendRequestToCAM(osThreadGetId(), &tMdCmd) != rUI_SUCCESS)
		{
			printd(DBG_InfoLvl, ("%s: MD Setting Fail !\n"), __func__);
		}
	}
}

//------------------------------------------------------------------------------
void UI_UpdateRecStsIcon(void)
{
	if((UI_SHOWSTSICON_STATE != tUI_State) ||
	   (!tUI_CuSetting.ubVdoRecStsCnt))
		return;
	if(!(--tUI_CuSetting.ubVdoRecStsCnt))
	{
		OSD_IMG_INFO tOsdImgInfo;

		tOsdImgInfo.uwHSize  = 152;
		tOsdImgInfo.uwVSize  = 100;
		tOsdImgInfo.uwXStart = uwOSD_GetHSize() - tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = 80;
		OSD_EraserImg2(&tOsdImgInfo);
		tUI_State = UI_DISPLAY_STATE;
	}
}
//------------------------------------------------------------------------------
void UI_UpdateWarningNoteIcon(void)
{
	if((UI_SHOWSTSICON_STATE != tUI_State) ||
	   (!tUI_CuSetting.WarnIcon.ubWarnUpdateCnt))
		return;
	if(!(--tUI_CuSetting.WarnIcon.ubWarnUpdateCnt))
	{
		OSD_IMG_INFO tOsdImgInfo;

		tOsdImgInfo.uwXStart = 315;
		tOsdImgInfo.uwYStart = 170;
		tOsdImgInfo.uwHSize  = 370;
		tOsdImgInfo.uwVSize  = 108;
		OSD_EraserImg2(&tOsdImgInfo);
		tUI_State = UI_DISPLAY_STATE;
	}
}

//------------------------------------------------------------------------------
void UI_VideoRecordingStsRpt(uint8_t ubRecSts)
{
	switch((KNL_Status_t)ubRecSts)
	{
		case KNL_ErrorNoCard:
        case KNL_ErrorTimeout:
		case KNL_ErrorCardNRdy:
		{
            uint8_t ubRecRpt = ubRecSts;
			OSD_IMG_INFO tOsdImgInfo;

			if(UI_DISPLAY_STATE != tUI_State)
				break;
			if((UI_SUBSUBMENU_STATE == tUI_State) || (UI_SUBMENU_STATE == tUI_State))
			{
				UI_ClearOsdImage();
				osDelay(100);
			}
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PHOTOCAPNOCARD_ICON, 1, &tOsdImgInfo);
			tOsdImgInfo.uwXStart = uwOSD_GetHSize() - tOsdImgInfo.uwHSize;
			tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
			tUI_CuSetting.ubVdoRecStsCnt = UI_RECGRAPHSTS_PERIOD;
			tUI_State = UI_SHOWSTSICON_STATE;
			if(UI_REC_START == tUI_RecPlayAct.tRecAct)
			{
                osMessagePut(osUI_RecRptQueue, &ubRecRpt, 0);
				UI_ClearRecordStatusOsdImg();
			}
			break;
		}
		case KNL_ErrorFsFmt:
			printf("-------------------------UI_VideoRecordingStsRpt KNL_ErrorFsFmt\n");
			break;
        case KNL_VDOREC_STOP_TIMEOUT: 
        {
			uint8_t ubRecRpt = ubRecSts;
            tUI_RecPlayAct.tRecAct = UI_REC_STOP;
			osMessagePut(osUI_RecRptQueue, &ubRecRpt, 0);
            break;
		}
		case KNL_VDOREC_STOP:
		{
			uint8_t ubRecRpt = ubRecSts;
            tUI_RecPlayAct.tRecAct = UI_REC_STOP;
			osMessagePut(osUI_RecRptQueue, &ubRecRpt, 0);
			break;
		}
		case KNL_VDOREC_START:
		{
			uint16_t uwLdState = UI_LD_DEFU;
			tUI_RecPlayAct.tRecAct = UI_REC_START;
			if(UI_RECORDING_MODE != tUI_CuSetting.tVdoMode)
			{
				ubUI_VdoRecChkFlag = FALSE;
				UI_VideoRecordingExec(UI_REC_STOP);
				break;
			}
			osMessagePut(osUI_OsdLdDispQueue, &uwLdState, 0);
			break;
		}
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_VideoRecordingExec(UI_RecordingAct_t tRecAct)
{
	
	KNL_RecordAct_t tUI_RecAct = {KNL_RECORDFUNC_DISABLE, NULL,};
	KNL_Status_t tUI_RecSts;
	osStatus osUI_EventStauts;

	if((TRUE == ubUI_VdoRecChkFlag) &&
	   ((tUI_RecPlayAct.tRecAct == tRecAct) || (UI_PHOTOCAP_MODE == tUI_CuSetting.tVdoMode)))
	{
		return;
	}

	tUI_RecAct.tRecordFunc	   = ((REC_LOOPING == tUI_CuSetting.RecInfo.tREC_Mode) && (UI_REC_START == tRecAct))?KNL_RECORDFUNC_LOOP:KNL_RECORDFUNC_MANU;
	tUI_RecAct.pRecordStsNtyCb = UI_VideoRecordingStsRpt;
	tUI_RecAct.ubRecOnceStopCBFlag = NULL;
	
	osSemaphoreWait(osUI_CuRecCtr, osWaitForever);
	tUI_RecSts = tKNL_ExecRecordFunc(tUI_RecAct);
	
	if(KNL_ERR != tUI_RecSts)
	{
		tUI_RecPlayAct.tRecAct = tRecAct;
		if(UI_REC_STOP == tUI_RecPlayAct.tRecAct)
		{
			uint8_t ubRecRpt;
			osMessageReset(osUI_RecRptQueue);
			osUI_EventStauts = osMessageGet(osUI_RecRptQueue, &ubRecRpt, 10000);
			if(osUI_EventStauts == osEventTimeout)
			{
				printf("Record Stop TimeOut\n");
				SYS_Reboot();
			}
		}
	}
	ubUI_VdoRecChkFlag = TRUE;
	ulStartRecCompleteCount = 500;
	
}

//------------------------------------------------------------------------------
//! Performance Debug
void UI_OsdDisplayFrmErrItem(UI_DisplayLocation_t tDispLoc)
{
#define DBGDISP_ICON_OFFSET 20
	uint16_t uwLcd_HSize  = uwOSD_GetHSize();
	uint16_t uwLcd_VSize  = uwOSD_GetVSize();
	uint16_t uwXOffset[11] = {[DISP_UPPER_LEFT] = 0,               [DISP_UPPER_RIGHT] = (uwLcd_HSize/2),
	                          [DISP_LOWER_LEFT] = 0, 			   [DISP_LOWER_RIGHT] = (uwLcd_HSize/2),
							  [DISP_LEFT] 	    = 0,			   [DISP_RIGHT] 	  = (uwLcd_HSize/2),
							  [DISP_H_L] 	    = 0, 			   [DISP_H_R] 		  = UI_HVIEW_ICON_XOFFSET,
							  [DISP_H_CU] 	    = 300, 		       [DISP_H_CL] 		  = 300};
	uint16_t uwYOffset[11] = {[DISP_UPPER_LEFT] = 0,               [DISP_UPPER_RIGHT] = 0,
	                          [DISP_LOWER_LEFT] = (uwLcd_VSize/2), [DISP_LOWER_RIGHT] = (uwLcd_VSize/2),
							  [DISP_LEFT] 	    = 0, 		       [DISP_RIGHT] 	  = 0,
							  [DISP_H_L] 	    = 0,  			   [DISP_H_R] 		  = 0,
							  [DISP_H_CU] 	    = 0, 		       [DISP_H_CL] 		  = (uwLcd_VSize/2)};
	uint8_t ubDbgItem;

	for(ubDbgItem = 0; ubDbgItem < MAX_DBG_ITEM; ubDbgItem++)
	{
		tUI_PerDbgOsdImgInfo[11+ubDbgItem].uwXStart = uwXOffset[tDispLoc];
		tUI_PerDbgOsdImgInfo[11+ubDbgItem].uwYStart = 150 + (DBGDISP_ICON_OFFSET * ubDbgItem) + uwYOffset[tDispLoc];
		tOSD_Img2(&tUI_PerDbgOsdImgInfo[11+ubDbgItem], OSD_QUEUE);
		tUI_PerDbgOsdImgInfo[11+ubDbgItem].uwXStart = 0;
		tUI_PerDbgOsdImgInfo[11+ubDbgItem].uwYStart = 0;
	}
}
//------------------------------------------------------------------------------
void UI_OsdDisplayFrameSeqFunc(uint8_t ubCamNum, uint16_t uwFrameSeq, uint16_t uwXPos, uint16_t uwYPos)
{
	uint8_t ubTen = 0, ubUnit = 0, ubHun = 0, ubThus = 0;
	uint16_t uwXStart = 0;

	if(UI_DISPLAY_STATE != tUI_State)
		return;
	osMutexWait(osUI_PerDbgMutex, osWaitForever);
	ubThus =  uwFrameSeq / 1000;
	ubHun  = (uwFrameSeq - (ubThus * 1000)) / 100;
	ubTen  = (uwFrameSeq - ((ubThus * 1000) + (ubHun * 100))) / 10;
	ubUnit = (uwFrameSeq - ((ubThus * 1000) + (ubHun * 100) + (ubTen * 10)));
	tUI_PerDbgOsdImgInfo[ubThus].uwXStart  = uwXPos;
	tUI_PerDbgOsdImgInfo[ubThus].uwYStart  = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubThus], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubThus].uwXStart;
	tUI_PerDbgOsdImgInfo[ubThus].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubThus].uwYStart  = 0;
	tUI_PerDbgOsdImgInfo[ubHun].uwXStart  = uwXStart + tUI_PerDbgOsdImgInfo[ubThus].uwHSize;
	tUI_PerDbgOsdImgInfo[ubHun].uwYStart  = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubHun], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubHun].uwXStart;
	tUI_PerDbgOsdImgInfo[ubHun].uwXStart   = 0;
	tUI_PerDbgOsdImgInfo[ubHun].uwYStart   = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart  = uwXStart + tUI_PerDbgOsdImgInfo[ubHun].uwHSize;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart  = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubTen], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubTen].uwXStart;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart   = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart   = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart =  uwXStart + tUI_PerDbgOsdImgInfo[ubTen].uwHSize;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubUnit], OSD_UPDATE);
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart  = 0;
	osMutexRelease(osUI_PerDbgMutex);
}
//------------------------------------------------------------------------------
void UI_OsdDisplayFpsFunc(uint8_t ubCamNum, uint16_t uwFps, uint16_t uwXPos, uint16_t uwYPos)
{
	uint8_t ubTen = 0, ubUnit = 0;
	uint16_t uwXStart = 0;

	if(UI_DISPLAY_STATE != tUI_State)
		return;
	osMutexWait(osUI_PerDbgMutex, osWaitForever);
	tUI_PerDbgOsdImgInfo[10].uwXStart = 60 + uwXPos;
	tUI_PerDbgOsdImgInfo[10].uwYStart = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[10], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[10].uwXStart;
	tUI_PerDbgOsdImgInfo[10].uwXStart  	  = 0;
	tUI_PerDbgOsdImgInfo[10].uwYStart  	  = 0;
	ubTen  = uwFps / 10;
	ubUnit = uwFps - (ubTen * 10);
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart  = uwXStart + tUI_PerDbgOsdImgInfo[10].uwHSize;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart  = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubTen], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubTen].uwXStart;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart  = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart = uwXStart + tUI_PerDbgOsdImgInfo[ubTen].uwHSize;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart = 100 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubUnit], OSD_UPDATE);
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart = 0;
	osMutexRelease(osUI_PerDbgMutex);
}
//------------------------------------------------------------------------------
void UI_OsdDisplayRfBwFunc(uint8_t ubCamNum, uint16_t uwBw, uint16_t uwXPos, uint16_t uwYPos)
{
	uint8_t ubHun = 0, ubTen = 0, ubUnit = 0;
	uint16_t uwXStart = 0;

	if(UI_DISPLAY_STATE != tUI_State)
		return;
	osMutexWait(osUI_PerDbgMutex, osWaitForever);
	tUI_PerDbgOsdImgInfo[10].uwXStart = uwXPos;
	tUI_PerDbgOsdImgInfo[10].uwYStart = 125 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[10], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[10].uwXStart;
	tUI_PerDbgOsdImgInfo[10].uwXStart  	  = 0;
	tUI_PerDbgOsdImgInfo[10].uwYStart  	  = 0;
	ubHun  = uwBw / 100;
	ubTen  = (uwBw -  (ubHun* 100)) / 10;
	ubUnit = (uwBw - ((ubHun * 100) + (ubTen * 10)));
	tUI_PerDbgOsdImgInfo[ubHun].uwXStart  = uwXStart + tUI_PerDbgOsdImgInfo[10].uwHSize;
	tUI_PerDbgOsdImgInfo[ubHun].uwYStart  = 125 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubHun], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubHun].uwXStart;
	tUI_PerDbgOsdImgInfo[ubHun].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubHun].uwYStart  = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart  = uwXStart + tUI_PerDbgOsdImgInfo[ubHun].uwHSize;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart  = 125 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubTen], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubTen].uwXStart;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart  = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart = uwXStart + tUI_PerDbgOsdImgInfo[ubTen].uwHSize;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart = 125 + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubUnit], OSD_UPDATE);
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart = 0;
	osMutexRelease(osUI_PerDbgMutex);
}
//------------------------------------------------------------------------------
void UI_OsdDisplayFrameErrType(uint8_t ubCamNum, uint16_t uwErrorType, uint16_t uwXPos, uint16_t uwYPos)
{
	uint8_t ubTen = 0, ubUnit = 0, ubHun = 0, ubThus = 0;
	uint16_t uwXStart = 0;

	if(UI_DISPLAY_STATE != tUI_State)
		return;
	osMutexWait(osUI_PerDbgMutex, osWaitForever);
	ubThus = ulUI_FrameErrCnt[ubCamNum][uwErrorType] / 1000;
	ubHun  = (ulUI_FrameErrCnt[ubCamNum][uwErrorType] - (ubThus * 1000)) / 100;
	ubTen  = (ulUI_FrameErrCnt[ubCamNum][uwErrorType] - ((ubThus * 1000) + (ubHun * 100))) / 10;
	ubUnit = (ulUI_FrameErrCnt[ubCamNum][uwErrorType] - ((ubThus * 1000) + (ubHun * 100) + (ubTen * 10)));
	tUI_PerDbgOsdImgInfo[ubThus].uwXStart  = 20 + uwXPos;
	tUI_PerDbgOsdImgInfo[ubThus].uwYStart  = 150 + (DBGDISP_ICON_OFFSET * uwErrorType) + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubThus], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubThus].uwXStart;
	tUI_PerDbgOsdImgInfo[ubThus].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubThus].uwYStart  = 0;
	tUI_PerDbgOsdImgInfo[ubHun].uwXStart   = uwXStart + tUI_PerDbgOsdImgInfo[ubThus].uwHSize;
	tUI_PerDbgOsdImgInfo[ubHun].uwYStart   = 150 + (DBGDISP_ICON_OFFSET * uwErrorType) + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubHun], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubHun].uwXStart;
	tUI_PerDbgOsdImgInfo[ubHun].uwXStart   = 0;
	tUI_PerDbgOsdImgInfo[ubHun].uwYStart   = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart   = uwXStart + tUI_PerDbgOsdImgInfo[ubHun].uwHSize;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart   = 150 + (DBGDISP_ICON_OFFSET * uwErrorType) + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubTen], OSD_QUEUE);
	uwXStart = tUI_PerDbgOsdImgInfo[ubTen].uwXStart;
	tUI_PerDbgOsdImgInfo[ubTen].uwXStart   = 0;
	tUI_PerDbgOsdImgInfo[ubTen].uwYStart   = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart  = uwXStart + tUI_PerDbgOsdImgInfo[ubTen].uwHSize;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart  = 150 + (DBGDISP_ICON_OFFSET * uwErrorType) + uwYPos;
	tOSD_Img2(&tUI_PerDbgOsdImgInfo[ubUnit], OSD_UPDATE);
	tUI_PerDbgOsdImgInfo[ubUnit].uwXStart  = 0;
	tUI_PerDbgOsdImgInfo[ubUnit].uwYStart  = 0;
	osMutexRelease(osUI_PerDbgMutex);
}
//------------------------------------------------------------------------------
void UI_DisplayTrxInfo(UI_FuncExecMsg_t tPerRpt)
{
	static void (*pvUI_PerRptFunc[])(uint8_t, uint16_t, uint16_t, uint16_t) =
	{
		[PER_FRMSTS_RPT] = UI_OsdDisplayFrameErrType,
		[PER_FRMSEQ_RPT] = UI_OsdDisplayFrameSeqFunc,
		[PER_FPS_RPT] 	 = UI_OsdDisplayFpsFunc,
		[PER_RFBW_RPT] 	 = UI_OsdDisplayRfBwFunc,
	};
	uint16_t uwLcd_HSize  = uwOSD_GetHSize();
	uint16_t uwLcd_VSize  = uwOSD_GetVSize();
	uint16_t uwXOffset[11] = {[DISP_UPPER_LEFT] = 0,               [DISP_UPPER_RIGHT] = (uwLcd_HSize/2),
	                          [DISP_LOWER_LEFT] = 0, 			   [DISP_LOWER_RIGHT] = (uwLcd_HSize/2),
							  [DISP_LEFT] 	    = 0,			   [DISP_RIGHT] 	  = (uwLcd_HSize/2),
							  [DISP_H_L] 	    = 0, 			   [DISP_H_R] 		  = UI_HVIEW_ICON_XOFFSET,
							  [DISP_H_CU] 	    = 300, 		       [DISP_H_CL] 		  = 300};
	uint16_t uwYOffset[11] = {[DISP_UPPER_LEFT] = 0,               [DISP_UPPER_RIGHT] = 0,
	                          [DISP_LOWER_LEFT] = (uwLcd_VSize/2), [DISP_LOWER_RIGHT] = (uwLcd_VSize/2),
							  [DISP_LEFT] 	    = 0, 		       [DISP_RIGHT] 	  = 0,
							  [DISP_H_L] 	    = 0,  			   [DISP_H_R] 		  = 0,
							  [DISP_H_CU] 	    = 0, 		       [DISP_H_CL] 		  = (uwLcd_VSize/2)};
	UI_DisplayLocation_t tUI_DispLoc;
	UI_DisplayLocation_t t4TQuadDefuLoc[CAM_4T] = 
	{
		tUI_CamStatus[0].tCamDispLocation_Quad, 
		tUI_CamStatus[1].tCamDispLocation_Quad,
		tUI_CamStatus[2].tCamDispLocation_Quad,
		tUI_CamStatus[3].tCamDispLocation_Quad
	};

	if((SINGLE_VIEW == tCamViewSel.tCamViewType) && (((UI_CamNum_t)tPerRpt.ubCamNum != tCamViewSel.tCamViewPool[0])))
		return;
	if((VDO_DISP_TYPE == KNL_DISP_H) && (QUAD_VIEW == tCamViewSel.tCamViewType))
		tUI_DispLoc = t4TQuadDefuLoc[tPerRpt.ubCamNum];
	else
		tUI_DispLoc = (tCamViewSel.tCamViewType == SINGLE_VIEW)?DISP_UPPER_LEFT:
					  (tCamViewSel.tCamViewType == DUAL_VIEW)?((tPerRpt.ubCamNum == tCamViewSel.tCamViewPool[0])?DISP_LEFT:DISP_RIGHT):tUI_CamStatus[tPerRpt.ubCamNum].tCamDispLocation;
	if(pvUI_PerRptFunc[tPerRpt.ubType])
		pvUI_PerRptFunc[tPerRpt.ubType](tPerRpt.ubCamNum, tPerRpt.uwFuncMsg, uwXOffset[tUI_DispLoc], uwYOffset[tUI_DispLoc]);
}
//------------------------------------------------------------------------------
void UI_PerReportFunc(uint8_t ubRptType, uint8_t ubCamNum, uint16_t uwValue)
{
	UI_FuncExecMsg_t tWorkAct;

	if(INVALID_ID == tUI_CamStatus[ubCamNum].ulCAM_ID)
		return;
	switch(ubRptType)
	{
		case PER_FRMSTS_RPT:
		case PER_FRMSEQ_RPT:
		case PER_FPS_RPT:
		case PER_RFBW_RPT:
			if(PER_FRMSTS_RPT == ubRptType)
			{
				osMutexWait(osUI_PerDbgMutex, osWaitForever);
				++ulUI_FrameErrCnt[ubCamNum][uwValue];
				osMutexRelease(osUI_PerDbgMutex);
			}
			if((SINGLE_VIEW == tCamViewSel.tCamViewType) && (((UI_CamNum_t)ubCamNum != tCamViewSel.tCamViewPool[0])))
				return;
			tWorkAct.uwFunc 	= UI_PERDBGRPT_ACT;
			tWorkAct.ubCamNum 	= ubCamNum;
			tWorkAct.ubType 	= ubRptType;
			tWorkAct.uwFuncMsg  = uwValue;
			osMessagePut(osUI_FuncsExecQue, &tWorkAct, 0);
		default:
			break;
	}
}

//------------------------------------------------------------------------------
void UI_ShowButtonValueNormal(uint16_t uwValue,uint16_t uwXStart,uint16_t uwYStart,OSD_UPDATE_TYP tUpdateMod)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubUnits = 0, ubTens = 0, ubHunds = 0,ubThous = 0;
	uint8_t *ubVal[4] = {&ubUnits,&ubTens,&ubHunds,&ubThous};
	for(uint8_t i = 0;i < 4;i++)
	{
		*ubVal[i] = uwValue % 10;
		uwValue /= 10;
	}
	if(ubThous)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubThous, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 40;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 60;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);

	}
	else if(ubHunds)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 40;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else if(ubTens)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_NOR+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
}

//------------------------------------------------------------------------------
void UI_ShowButtonValueHighLight(uint16_t uwValue,uint16_t uwXStart,uint16_t uwYStart,OSD_UPDATE_TYP tUpdateMod)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubUnits = 0, ubTens = 0, ubHunds = 0,ubThous = 0;
	uint8_t *ubVal[4] = {&ubUnits,&ubTens,&ubHunds,&ubThous};
	for(uint8_t i = 0;i < 4;i++)
	{
		*ubVal[i] = uwValue % 10;
		uwValue /= 10;
	}
	if(ubThous)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubThous, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 40;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 60;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);

	}
	else if(ubHunds)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 40;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else if(ubTens)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BUTTON_NUM0_HL+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
}

//------------------------------------------------------------------------------
void UI_ShowKeyBoardValue(uint16_t uwValue,OSD_UPDATE_TYP tUpdateMod)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubUnits = 0, ubTens = 0, ubHunds = 0,ubThous = 0;

	uint8_t *ubVal[4] = {&ubUnits,&ubTens,&ubHunds,&ubThous};
	for(uint8_t i = 0;i < 4;i++)
	{
		*ubVal[i] = uwValue % 10;
		uwValue /= 10;
	}
	if(ubThous)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubThous, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 40;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubUnits , 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 60;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);

	}
	else if(ubHunds)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubHunds, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 40;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM_COVERING, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 60;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else if(ubTens)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubTens, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM_COVERING, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 40;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM0+ubUnits, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_SHOWNUM_COVERING, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOsdImgInfo.uwXStart = tOsdImgInfo.uwXStart + 20;
		tOsdImgInfo.uwYStart = tOsdImgInfo.uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
}
//------------------------------------------------------------------
void UI_ShowPlaybackPageNum(uint16_t uwValue,uint16_t uwXStart,uint16_t uwYStart,OSD_UPDATE_TYP tUpdateMod)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubUnits = 0, ubTens = 0, ubHunds = 0,ubThous = 0;
	uint8_t *ubVal[4] = {&ubUnits,&ubTens,&ubHunds,&ubThous};
	for(uint8_t i = 0;i < 4;i++)
	{
		*ubVal[i] = uwValue % 10;
		uwValue /= 10;
	}
	if(ubThous)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubThous, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 2*tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 3*tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);

	}
	else if(ubHunds)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 2*tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else if(ubTens)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + tOsdImgInfo.uwHSize;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CHAR_NUM0+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
}
//------------------------------------------------------------------
void UI_ShowPlayTime(uint16_t uwCurTime,uint16_t uwTotalTime,uint16_t uwXStart,uint16_t uwYStart)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubminute,ubsecond;
	uint8_t *ubVal[2] = {&ubsecond,&ubminute};
	for(uint8_t i = 0;i < 2;i ++)
	{
		*ubVal[i] = uwCurTime % 60;
		uwCurTime /= 60;
	}
	//slash
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_SLASH, 1, &tOsdImgInfo); 
	tOsdImgInfo.uwXStart = uwXStart + 4*tOsdImgInfo.uwHSize + 5;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	/***********CurTime****************/
	//Colon
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_COLON, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 2*tOsdImgInfo.uwHSize - 5;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	//minute
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubminute/10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubminute%10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + tOsdImgInfo.uwHSize;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);

	//second
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubsecond/10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 3*tOsdImgInfo.uwHSize - 10;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubsecond%10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 4*tOsdImgInfo.uwHSize - 10;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	/***********TotalTime****************/

	for(uint8_t i = 0;i < 2;i ++)
	{
		*ubVal[i] = uwTotalTime % 60;
		uwTotalTime /= 60;
	}

	//Colon
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_COLON, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 7*tOsdImgInfo.uwHSize - 5;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	//minute
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubminute/10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 5*tOsdImgInfo.uwHSize;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubminute%10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 6*tOsdImgInfo.uwHSize;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);

	//second
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubsecond/10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 8*tOsdImgInfo.uwHSize - 10;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_NUM0 + ubsecond%10, 1, &tOsdImgInfo);	
	tOsdImgInfo.uwXStart = uwXStart + 9*tOsdImgInfo.uwHSize - 10;
	tOsdImgInfo.uwYStart = uwYStart;
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	
}
//------------------------------------------------------------------------------
void UI_ShowNumber(uint16_t uwValue,uint16_t uwXStart,uint16_t uwYStart,uint16_t uwNum0_ImageIndex,OSD_UPDATE_TYP tUpdateMod)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubUnits = 0, ubTens = 0, ubHunds = 0,ubThous = 0;
	uint8_t *ubVal[4] = {&ubUnits,&ubTens,&ubHunds,&ubThous};
	for(uint8_t i = 0;i < 4;i++)
	{
		*ubVal[i] = uwValue % 10;
		uwValue /= 10;
	}
	if(ubThous)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubThous, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 40;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 60;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);

	}
	else if(ubHunds)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubHunds, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 40;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else if(ubTens)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubTens, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart + 20;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNum0_ImageIndex+ubUnits, 1, &tOsdImgInfo);
		tOsdImgInfo.uwXStart = uwXStart;
		tOsdImgInfo.uwYStart = uwYStart;
		tOSD_Img2(&tOsdImgInfo, tUpdateMod);
	}
}


//------------------------------------------------------------------
void UI_InitCharArray(void)
{
	uint8_t i;
	
	for(i = 0; i < 10; i++)
		tCharOsdImgDB.uwUI_NumArray[i] = OSD2IMG_CHAR_NUM0 + i;
																			 
	tUI_CharOsdImgInfo.pNumImgIdxArray = tCharOsdImgDB.uwUI_NumArray;
	for(i = 0; i < 26; i++)
	{
		tCharOsdImgDB.uwUI_UpperLetterArray[i] = OSD2IMG_CHAR_UPA + i;
																				      
		tCharOsdImgDB.uwUI_LowerLetterArray[i] = OSD2IMG_CHAR_UPA + i;
																					
	}
	tUI_CharOsdImgInfo.pUpperLetterImgIdxArray = tCharOsdImgDB.uwUI_UpperLetterArray;
	tUI_CharOsdImgInfo.pLowerLetterImgIdxArray = tCharOsdImgDB.uwUI_LowerLetterArray;
	for(i = 0; i < 4; i++)
		tCharOsdImgDB.uwUI_SymbolArray[i] = OSD2IMG_CHAR_COLON + i;
																				 
	tUI_CharOsdImgInfo.pSymbolImgIdxArray = tCharOsdImgDB.uwUI_SymbolArray;
}
//初始化无背景数字和标点
void UI_InitTransparentCharArray(void)
{
	uint8_t i;
	
	for(i = 0; i < 10; i++)
		tTransparentCharOsdImgDB.uwUI_NumArray[i] = OSD2IMG_TRANS_CHAR_NUM0 + i;
																			 
	tUI_TransparentCharOsdImgInfo.pNumImgIdxArray = tTransparentCharOsdImgDB.uwUI_NumArray;
	for(i = 0; i < 26; i++)
	{
		tTransparentCharOsdImgDB.uwUI_UpperLetterArray[i] = OSD2IMG_CHAR_UPA + i;
																				      
		tTransparentCharOsdImgDB.uwUI_LowerLetterArray[i] = OSD2IMG_CHAR_UPA + i;
																					
	}
	tUI_TransparentCharOsdImgInfo.pUpperLetterImgIdxArray = tTransparentCharOsdImgDB.uwUI_UpperLetterArray;
	tUI_TransparentCharOsdImgInfo.pLowerLetterImgIdxArray = tTransparentCharOsdImgDB.uwUI_LowerLetterArray;
	for(i = 0; i < 4; i++)
		tTransparentCharOsdImgDB.uwUI_SymbolArray[i] = OSD2IMG_CHAR_COLON + i;
	tTransparentCharOsdImgDB.uwUI_SymbolArray[2] = OSD2IMG_TRANS_CHAR_DOT;
																				 
	tUI_TransparentCharOsdImgInfo.pSymbolImgIdxArray = tTransparentCharOsdImgDB.uwUI_SymbolArray;
}	

//------------------------------------------------------------------
long UI_Map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//------------------------------------------------------------------
uint32_t UI_GetBackLightPwm(void)
{
	if(tUI_CuSetting.ubAutoDimmer)
	{
		if(uwSADC_GetReport(SADC_CH4) < 200)//低亮度
		{
			ubUI_CurDimmerLvl = tUI_CuSetting.ubAutoDimmer_MinVal;
			
		}
		else
		{
			ubUI_CurDimmerLvl = tUI_CuSetting.ubAutoDimmer_MaxVal;
		}
		return (uint16_t)UI_Map(ubUI_CurDimmerLvl,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
	}
	else
		return (uint16_t)UI_Map(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
}
//------------------------------------------------------------------
uint32_t UI_GetVolumePwm(void)
{
	if(tUI_CuSetting.ubMuteFlag)
		return 0;
	else
	{
		uint16_t uwValue = UI_Map(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,0,99,VOLUME_PWM_MIN,VOLUME_PWM_MAX);
		if(uwValue == VOLUME_PWM_MIN)
			return 0;
		else
			return uwValue;
	}
}

//------------------------------------------------------------------
UI_CamNum_t UI_GetAudioSrc(void)
{
	UI_CamNum_t tAdoCamNum;
	if(tCamViewSel.tCamViewType == SINGLE_VIEW)
		tAdoCamNum = tCamViewSel.tCamViewPool[0];
	else if(tCamViewSel.tCamViewType == DUAL_VIEW)
		tAdoCamNum = tUI_CuSetting.tAdoSrcCamNum_Dual;
	else if(tCamViewSel.tCamViewType == QUAD_VIEW)
		tAdoCamNum = tUI_CuSetting.tAdoSrcCamNum_Quad;
	return tAdoCamNum;
}
void UI_SetAudioSrc(UI_CamNum_t tAdoSrc)
{
	if(tCamViewSel.tCamViewType == DUAL_VIEW)
		tUI_CuSetting.tAdoSrcCamNum_Dual = tAdoSrc;
	else if(tCamViewSel.tCamViewType == QUAD_VIEW)
		tUI_CuSetting.tAdoSrcCamNum_Quad = tAdoSrc;

}
//------------------------------------------------------------------
void UI_PanelOn(void)
{
	if(tUI_CuSetting.ubAutoDimmer)
	{
		UI_CheckLightSensor(TRUE);
	}
	else
	{
		ubUI_CurDimmerLvl = tUI_CuSetting.BriLvL.ulBL_UpdateLvL;
		uint8_t ubValue = (uint16_t)UI_Map(tUI_CuSetting.BriLvL.ulBL_UpdateLvL ,0,99,LCD_PWM_MIN,LCD_PWM_MAX);
		SET_PANEL_PWM(ubValue);
	}
		
}
//------------------------------------------------------------------
void UI_PanelOff(void)
{
	ubUI_CurDimmerLvl = 0;
	SET_PANEL_PWM(0);
}
//------------------------------------------------------------------
void UI_AudioOn(void)
{
	uint16_t uwValue;
	if(tUI_CuSetting.ubMuteFlag)
	{
		SET_VOLUME_PWM(0);
	}
	else
	{
		uwValue = (uint16_t)UI_Map(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,0,99,VOLUME_PWM_MIN,VOLUME_PWM_MAX);			
		SET_VOLUME_PWM((uwValue == VOLUME_PWM_MIN)?0:uwValue);

	}
}
//------------------------------------------------------------------
void UI_AudioOff(void)
{
	SET_VOLUME_PWM(0);
}
//------------------------------------------------------------------
UI_Result_t SendLaserorLedToCAM(osThreadId thread_id, UI_CUReqCmd_t *ptReqCmd,TWC_OPC Opc)
{
	UI_Result_t tReq_Result = rUI_SUCCESS;
	osEvent tReq_Event;
	APP_StaNumMap_t *pUI_CamNumMap = APP_GetSTANumMappingTable(ptReqCmd->tDS_CamNum);
	uint8_t ubUI_TwcRetry = 5;

	tosUI_Notify.thread_id = thread_id;
	tosUI_Notify.iSignals  = osUI_SIGNALS;
	while(--ubUI_TwcRetry)
	{
		if(tTWC_Send(pUI_CamNumMap->tTWC_StaNum, Opc, &ptReqCmd->ubCmd[UI_SETTING_DATA+1], 1, 10) == TWC_SUCCESS)
			break;
		osDelay(10);
	}
	if(!ubUI_TwcRetry)
	{
		tTWC_StopTwcSend(pUI_CamNumMap->tTWC_StaNum, Opc);
		tosUI_Notify.thread_id = NULL;
		return rUI_FAIL;
	}
	if(tosUI_Notify.thread_id != NULL)
	{
		tReq_Event = osSignalWait(tosUI_Notify.iSignals, UI_TWC_TIMEOUT);
		tReq_Result = (tReq_Event.status == osEventSignal)?(tReq_Event.value.signals == tosUI_Notify.iSignals)?tosUI_Notify.tReportSts:rUI_FAIL:rUI_FAIL;
		tTWC_StopTwcSend(pUI_CamNumMap->tTWC_StaNum, Opc);
		tosUI_Notify.thread_id  = NULL;
		tosUI_Notify.iSignals   = NULL;
		tosUI_Notify.tReportSts = rUI_SUCCESS;
	}
	return tReq_Result;
}

//------------------------------------------------------------------
void UI_CheckMenuOffCount(void)
{
	UI_Event_t tMenuOffEvent;
	osMessageQId *pUI_MenuOffEventQH = NULL;
	static int cnt = 0;
    cnt++;
    if (cnt >= 15)
    {
		cnt = 0;
		printf("ulMenuAutoOffCount = %d, MenuOnFlag = %d\n", ulMenuAutoOffCount, MenuOnFlag);
    }
	
	if(ulMenuAutoOffCount == 0 && MenuOnFlag == TRUE)
	{
		tMenuOffEvent.tEventType = MENUOFF_EVENT;
		tMenuOffEvent.pvEvent 	  = NULL;
		pUI_MenuOffEventQH 	  = pUI_GetEventQueueHandle();
	    osMessagePut(*pUI_MenuOffEventQH, &tMenuOffEvent, 0);
		UI_UpdateDevStatusInfo();
	}

}
//------------------------------------------------------------------
void UI_CheckPanelOffCount(void)
{
	if(ulPanelOffCount == 0 && !MenuOnFlag && !ubUI_CuStandbyFlag)
	{
		UI_EnterStandby();
	}
}
//------------------------------------------------------------------
void UI_CheckTriggerOverCount(void)
{
	osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
	if(ulTriggerOverCount == 0 && TriggerLock)
	{
		TriggerLock = FALSE;
		TriggerSource = 0xff;
		Trigger_View.tUI_ViewSel = TriggerBackUp_View;
		
		if(ubUI_CuStandbyFlag)
		{
			UI_PanelOff();
			UI_AudioOff();
			POWER_LED_IO = 1;

			if(UI_REC_START == tUI_RecPlayAct.tRecAct)
			{
				UI_VideoRecordingExec(UI_REC_STOP);
				ubUI_RestartRec = TRUE;
			}
			UI_DisconnectTx();
		}
		
		UI_Event_t Trigger_Event;
		Trigger_Event.pvEvent = (void *)(&Trigger_View);
		Trigger_Event.tEventType = TRIGGER_EVENT;
		 if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
			printd(DBG_ErrorLvl, "Trigger Q Full\n");
	}
	osSemaphoreRelease(osUI_CuTriggerCtr);
}
//------------------------------------------------------------------
void UI_CheckPelcoZoomOverCount(void)
{
    uint8_t retry_time = 10;
    TWC_STATUS send_ret = TWC_FAIL;

	if (DeskTopShowView > CAM4VIEW_ITEM)
	{
		return;
	}

    if(ubPelcoZoomFlag && (ubPelcoZoomStart == TRUE))
	{
        //
        //ubPelcoZoomFlag = FALSE;
        if (ubPelcoZoomFlag && (ubPelcoZoomType == 0))
        {
        	//
            while (retry_time--)
	        {
	        	send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_in[0], COMMAND_LENGTH, 8);
	        	if (send_ret == TWC_SUCCESS)
	        	{
	        		break;
	        	}
	        }
        }
        else if (ubPelcoZoomFlag && (ubPelcoZoomType == 1))
        {
        	//
            while (retry_time--)
	        {
	        	send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_out[0], COMMAND_LENGTH, 8);
	        	if (send_ret == TWC_SUCCESS)
	        	{
	        		break;
	        	}
	        }
        }

		osDelay(150);
		retry_time = 10;
		while (retry_time--)
		{
			send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_stop[0], COMMAND_LENGTH, 8);
			if (send_ret == TWC_SUCCESS)
			{
				break;
			}
		}
		ubPelcoZoomStart = FALSE;
        // ulPelcoZoomCount = 500;
		ubPelcoZoomFlag = FALSE;

        if (send_ret != TWC_SUCCESS)
        {
        	printf("send pelco zomm command failed!!!\n");
        }
	}
}

//------------------------------------------------------------------
void UI_ShowRecordingStatus(void)
{
	OSD_IMG_INFO tOsdImgInfo[3];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECORDING_BLACK_ICON, 3, &tOsdImgInfo[0]);
	static uint8_t ubImgIdx = 0;
	if(UI_REC_START == tUI_RecPlayAct.tRecAct)
	{
		if(tUI_SyncAppState == APP_LINK_STATE)
		{
			ubImgIdx = 1 - ubImgIdx;
			tOSD_Img2(&tOsdImgInfo[ubImgIdx], OSD_UPDATE);
		}
		else
		{
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
		}
	}
	else
	{	
		ubImgIdx = 0;
		UI_ClearRecordStatusOsdImg();
	}

}

//------------------------------------------------------------------
void UI_BackToDesktop(uint8_t ubSwitchView)
{
	printf("UI_BackToDesktop\n");
	if(ubSwitchView && !TriggerLock)
		UI_SwitchViewType(DeskTopShowView,FALSE);
	
	UI_ClearOsdImageNoUpdate();
	MenuOnFlag = FALSE;
	SendIrCodeFlag = FALSE;
	tUI_State = UI_DISPLAY_STATE;
	UI_DrawDesktopIcon();

	if(TriggerLock && DeskTopShowView <= CAM4 && tUI_CuSetting.tParkingLineEnable[DeskTopShowView] == PARKING_LINE_AUTO)
	{
		UI_DrawParkingLine(DeskTopShowView,OSD_UPDATE);
	}

}
//------------------------------------------------------------------
void UI_EnterStandby(void)
{
	printf("---UI_EnterStandby\n");
	ubUI_CuStandbyFlag = TRUE;
	UI_PanelOff();
	UI_AudioOff();
	
	if(UI_REC_START == tUI_RecPlayAct.tRecAct)
	{
		UI_VideoRecordingExec(UI_REC_STOP);
		ubUI_RestartRec = TRUE;
	}
	UI_BackToDesktop(FALSE);
	POWER_LED_IO = 1;
	if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
		UI_SaveLastMode();
	UI_DisconnectTx();
}
//------------------------------------------------------------------
void UI_WakeupStandby(void)
{
	printf("UI_WakeupStandby\n");
	if(ubUI_CuPowerOffFlag)
	{
		SYS_Reboot();
		while(1);
	}
	ubUI_CuStandbyFlag = FALSE;
	UI_SwitchViewType(DeskTopShowView,TRUE);
	UI_ClearOsdImageNoUpdate();
	UI_DrawDesktopIcon();
	BUZZER_DI(150);
	POWER_LED_IO = 0;
	if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
		UI_SaveLastMode();
	ubAutoScanReady = TRUE;
}
//------------------------------------------------------------------
void UI_MenuLock(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	
	osSemaphoreWait(osUI_CuUiCtr, osWaitForever);
	if(ubUI_TouchPanelSts) {
		if(tUI_CuSetting.ubMenuLock|| tUI_CuSetting.ubKeyLock) {
			tUI_CuSetting.ubMenuLock = 0;
			tUI_CuSetting.ubKeyLock = 0;
		}else{
			tUI_CuSetting.ubMenuLock = 1;
			tUI_CuSetting.ubKeyLock = 1;
		}
	} else {
		if(tUI_CuSetting.ubKeyLock) {
			tUI_CuSetting.ubKeyLock = 0;
		}else{
			tUI_CuSetting.ubKeyLock = 1;
		}
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_MENUUNLOCK + tUI_CuSetting.ubKeyLock, 1, &tOsdImgInfo);
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	osDelay(500);
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo);
	UI_DrawDesktopIcon();
	osSemaphoreRelease(osUI_CuUiCtr);
	UI_UpdateDevStatusInfo();
}

//------------------------------------------------------------------
void UI_SetZoomMirrorAndFlip(UI_CamNum_t tCamNum)
{
	pelco_Picture_Mirror_Flip[3] = tUI_CamStatus[tCamNum].tCamImgMirror;
	pelco_Picture_Mirror_Flip[5] = tUI_CamStatus[tCamNum].tCamImgFlip;
	pelco_Picture_Mirror_Flip[6] = pelco_Picture_Mirror_Flip[1] + pelco_Picture_Mirror_Flip[2] + pelco_Picture_Mirror_Flip[3]\
	+ pelco_Picture_Mirror_Flip[4] + pelco_Picture_Mirror_Flip[5];
	tTWC_Send(TWC_STA1 + tCamNum, TWC_PELCO, (uint8_t *)&pelco_Picture_Mirror_Flip[0], 7, 8);
}

//------------------------------------------------------------------
void UI_GetZoomVersionDateFromString(UI_CamNum_t tCamNum, char* ubVersionString)
{
	if(!strlen(ubVersionString))
		return;
//	printf("ubVersionString = %s\n", ubVersionString);
	// 找到末尾的日期部分
    const char *datePart = ubVersionString + strlen(ubVersionString) - 6;
	
	uint8_t year = (datePart[0] - '0') * 10 + (datePart[1] - '0');
    uint8_t month = (datePart[2] - '0') * 10 + (datePart[3] - '0');
    uint8_t day = (datePart[4] - '0') * 10 + (datePart[5] - '0');

    // 赋值到结构体
    tZoomVersionDate[tCamNum].ubVersionYear = year;
    tZoomVersionDate[tCamNum].ubVersionMonth = month;
    tZoomVersionDate[tCamNum].ubVersionDay = day;

//	printf("tCamNum: %d, Year: %02u, Month: %02u, Day: %02u\n", tCamNum, 
//           tZoomVersionDate[tCamNum].ubVersionYear, 
//           tZoomVersionDate[tCamNum].ubVersionMonth, 
//           tZoomVersionDate[tCamNum].ubVersionDay);
}

uint8_t UI_IsZoomNewVersion(UI_CamNum_t tCamNum)
{
	if(tZoomVersionDate[tCamNum].ubVersionYear > ZOOM_OLD_VERSION_YEAR)
	{
		return TRUE;
		
	}
	else if(tZoomVersionDate[tCamNum].ubVersionYear == ZOOM_OLD_VERSION_YEAR)
	{
		if(tZoomVersionDate[tCamNum].ubVersionMonth > ZOOM_OLD_VERSION_MONTH)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------
//强制复位摄像头状态
void UI_SetTxImgProc2(UI_CamNum_t tCamNum) //for zoom
{
	uint8_t ubValue,ubTryCount;
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
	tCamSetCmd.ubCmd_Len				 = 4;
	ubTryCount = 10;

	tUI_CamStatus[tCamNum].tCamImgMirror = 0;
	tUI_CamStatus[tCamNum].tCamImgFlip = 0;
	UI_SetZoomMirrorAndFlip(tCamNum);
	
	while(-- ubTryCount)
	{
		if(tUI_CamStatus[tCamNum].tCamConnSts == CAM_OFFLINE)
			osDelay(100);			
		else
			break;
	}
	if(ubTryCount == 0)
	{
		printf("tUI_CamStatus[%d].tCamConnSts == CAM_OFFLINE\n",tCamNum);
		return;
	}
	ubTryCount = 5;
	while(-- ubTryCount)
	{
//		if(tTWC_Send(tCamNum, TWC_PELCO, (uint8_t *)&pelco_BSD[0 + 7*tUI_CamStatus[tCamNum].ubBSD], 7, 8) == TWC_SUCCESS)
		if(tTWC_Send(tCamNum, TWC_PELCO, (uint8_t *)&pelco_BSD[7], 7, 8) == TWC_SUCCESS)
			break;
		osDelay(10);
	}
	if(ubTryCount == 0)
	{
		printf("UI_SetTxImgProc2 pelco_BSD send fail\n");
	}
	ubTryCount = 5;
	while(-- ubTryCount)
	{
//		if(tTWC_Send(tCamNum, TWC_PELCO, (uint8_t *)&pelco_Trace[0 + 7*tUI_CamStatus[tCamNum].ubTrace], 7, 8) == TWC_SUCCESS)
		if(tTWC_Send(tCamNum, TWC_PELCO, (uint8_t *)&pelco_Trace[0], 7, 8) == TWC_SUCCESS)
			break;
		osDelay(10);
	}
	if(ubTryCount == 0)
	{
		printf("UI_SetTxImgProc2 pelco_Trace send fail\n");
	}
	
//	Picture_Mirror_Flip[3] = tUI_CamStatus[tCamNum].tCamImgMirror;
//	Picture_Mirror_Flip[5] = tUI_CamStatus[tCamNum].tCamImgFlip;
//	Picture_Mirror_Flip[6] = Picture_Mirror_Flip[1] + Picture_Mirror_Flip[2] + Picture_Mirror_Flip[3]\
//		+ Picture_Mirror_Flip[4] + Picture_Mirror_Flip[5];
//	ubTryCount = 5;
//	while(-- ubTryCount)
//	{
//		if(	tTWC_Send(tCamNum, TWC_PELCO, (uint8_t *)&Picture_Mirror_Flip[0], 7, 8) == TWC_SUCCESS)
//			break;
//	}
	printf("UI_SetTxImgProc2 tCamNum = %d success\n",tCamNum);
}

void UI_SetTxImgProc(UI_CamNum_t tCamNum)
{
	uint8_t ubValue,ubTryCount;
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
	tCamSetCmd.ubCmd_Len				 = 4;
	ubTryCount = 10;;
	while(-- ubTryCount)
	{
		if(tUI_CamStatus[tCamNum].tCamConnSts == CAM_OFFLINE)
			osDelay(100);			
		else
			break;
	}
	if(ubTryCount == 0)
	{
		printf("tUI_CamStatus[%d].tCamConnSts == CAM_OFFLINE\n",tCamNum);
		return;
	}
		
	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL + tUI_CuSetting.bColorBLGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGBL_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGBL_SETTING Fail !\n");
			osDelay(20);
		}
		else
			break;
	}
	
	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast + tUI_CuSetting.bColorContrastGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGCONTRAST_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGCONTRAST_SETTING Fail !\n");
			osDelay(20);
		}
		else
			break;
	}
	
	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation + tUI_CuSetting.bColorSaturationGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGSATURATION_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGSATURATION_SETTING Fail !\n");
			osDelay(20);
		}
		else
			break;
	}

	ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue + tUI_CuSetting.bColorHueGain[tCamNum],0,99,0,127);
	tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGHUE_SETTING ;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGHUE_SETTING Fail !\n");
			osDelay(20);
		}
		else
			break;
	}

	tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGMIRROR_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgMirror;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGMIRROR_SETTING  Fail !\n");
			osDelay(20);
		}
		else
			break;
	}

	tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGFLIP_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgFlip;
	ubTryCount = 5;
	while(-- ubTryCount)
	{
		if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
		{
			printf("UI_IMGFLIP_SETTING Fail !\n");
			osDelay(20);
		}
		else
			break;

	}
	printf("UI_SetTxImgProc tCamNum = %d success\n",tCamNum);
}
//------------------------------------------------------------------
void UI_SaveLastMode(void)
{
	if(!ubUI_CuStandbyFlag)
		tUI_CuSetting.tLastMode = (UI_LastMode_t)tUI_ViewModeSel;
	else if(ubUI_CuStandbyFlag && tUI_ViewModeSel != SCANVIEW_ITEM)
		tUI_CuSetting.tLastMode = LASTMODE_STANDBY;
	else if(ubUI_CuStandbyFlag && tUI_ViewModeSel == SCANVIEW_ITEM)
		tUI_CuSetting.tLastMode = LASTMODE_SCAN_STANDBY;
	
	tUI_CuSetting.tLastView = DeskTopShowView;
	UI_UpdateDevStatusInfo();
}
//------------------------------------------------------------------
void UI_SyncTimeStamp2TX(void)
{
	APP_EventMsg_t tUI_Message = {0};
	tUI_Message.ubAPP_Event 	 = APP_SYNCTIMESTAMP_EVENT;
	tUI_Message.ubAPP_Message[0] = 1;	
	tUI_Message.ubAPP_Message[1] = tUI_CuSetting.ubTimeStame;
	UI_SendMessageToAPP(&tUI_Message);
}

void UI_DrawRedBox(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t x_temp = 0, y_temp = 0;
	OSD_IMG_INFO tInfor_H_line, tInfor_V_line, tOsdImgInfo;
	OSD_IMG_INFO tInforZoomBoxSelect;

	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	if (tUI_State != UI_ADJUST_ZOOM_AREA_STATE)
	{
		return;
	}


	printf("trace box x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
	if (((x2 - x1) < AI_AREA_MIN_SIZE) || ((y2 - y1) < AI_AREA_MIN_SIZE))
	{
		printf("trace box too small x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][0] = 412;
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][1] = 200;
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][2] = 612;
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][3] = 400;
		x1 = 412;
		y1 = 200;
		x2 = 612;
		y1 = 400;
	}
	else if(((x2 - x1) > 1024) || ((y2 - y1) > 600))
	{
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][0] = 412;
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][1] = 200;
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][2] = 612;
		tUI_CuSetting.usZoomRedbox[ubZoomItemCurIdx][3] = 400;
		x1 = 412;
		y1 = 200;
		x2 = 612;
		y1 = 400;
	}

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RED_HORI_LINE_30x4, 1, &tInfor_H_line);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RED_VERT_LINE_4x30, 1, &tInfor_V_line);

	for(x_temp = x1; x_temp <= x2 - BSD_AREA_LENGTH; x_temp += BSD_AREA_LENGTH)
	{
		//up
		tInfor_H_line.uwXStart = x_temp;
		tInfor_H_line.uwYStart = y1;
		tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
		{
			tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		}

		//down
		tInfor_H_line.uwXStart = x_temp;
		tInfor_H_line.uwYStart = y2 - BSD_AREA_WIDTH;
		tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
		{
			tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		}
	}

	for(y_temp = y1; y_temp <= y2 - BSD_AREA_LENGTH; y_temp += 30)
	{
		//left
		tInfor_V_line.uwXStart = x1 ;
		tInfor_V_line.uwYStart = y_temp;
		tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
		{
			tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		}

		//right
		tInfor_V_line.uwXStart = x2 - BSD_AREA_WIDTH;
		tInfor_V_line.uwYStart = y_temp;
		tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
		{
			tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		}
	}

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_BOX_SELECT, 1, &tInforZoomBoxSelect);
	switch (ZoomTraceBoxMoveSelect)
	{
		case ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM  :
			//A
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
		case ZOOM_BSD_AI_AREA_SMALL_UP_ITEM  :
			//B
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;

		case ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM :
			//C
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
		case ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM :
			//D
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
		case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM:
			//A
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//B
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//C
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//D
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
	}

	
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}

	
}


//void UI_DrawRedBox(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
//{
//	OSD_IMG_INFO tInfor_H_line, tInfor_V_line, tOsdImgInfo[4];
//	if (tUI_State != UI_ADJUST_ZOOM_AREA_STATE)
//	{
//		return;
//	}

//	if (((x2 - x1) < AI_AREA_MIN_SIZE) || ((y2 - y1) < AI_AREA_MIN_SIZE))
//	{
//		printf("rectangle too small x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][0] = 412;
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][1] = 200;
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][2] = 612;
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][3] = 400;
//		x1 = 412;
//		y1 = 200;
//		x2 = 612;
//		y1 = 400;
//	}
//	else if(((x2 - x1) > 1024) || ((y2 - y1) > 600))
//	{
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][0] = 412;
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][1] = 200;
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][2] = 612;
//		tUI_CuSetting.usZoomRedbox[DeskTopShowView][3] = 400;
//		x1 = 412;
//		y1 = 200;
//		x2 = 612;
//		y1 = 400;
//	}

//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RED_HORI_LINE_30x4, 1, &tInfor_H_line);
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RED_VERT_LINE_4x30, 1, &tInfor_V_line);

//    tInfor_H_line.uwXStart = x1;
//	tInfor_H_line.uwYStart = y1;
//	tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//	tInfor_V_line.uwXStart = x1;
//	tInfor_V_line.uwYStart = y1;
//	tOSD_Img2(&tInfor_V_line, OSD_QUEUE);

//	tInfor_H_line.uwXStart = x2 - 30;
//	tInfor_H_line.uwYStart = y1;
//	tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//	tInfor_V_line.uwXStart = x2 - 4;
//	tInfor_V_line.uwYStart = y1;
//	tOSD_Img2(&tInfor_V_line, OSD_QUEUE);

//	tInfor_H_line.uwXStart = x1;
//	tInfor_H_line.uwYStart = y2 - 4;
//	tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//	tInfor_V_line.uwXStart = x1;
//	tInfor_V_line.uwYStart = y2 - 30;
//	tOSD_Img2(&tInfor_V_line, OSD_QUEUE);

//	tInfor_H_line.uwXStart = x2 - 30;
//	tInfor_H_line.uwYStart = y2 - 4;
//	tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//	tInfor_V_line.uwXStart = x2 - 4;
//	tInfor_V_line.uwYStart = y2 - 30;
//	tOSD_Img2(&tInfor_V_line, OSD_QUEUE);

//	if(ubUI_TouchPanelSts)
//	{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo[0]);
//		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
//	}

//	if(ZoomAIAreaRemoteMode < ZOOM_AI_AREA_SIZEUD_MODE)
//	{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_L_R_FLAG, 4, &tOsdImgInfo[0]);
//		tOsdImgInfo[ZoomAIAreaRemoteMode].uwXStart = (x1 + (x2 - x1 - tOsdImgInfo[ZoomAIAreaRemoteMode].uwHSize)/2);
//		tOsdImgInfo[ZoomAIAreaRemoteMode].uwYStart = (y1 + (y2 - y1 - tOsdImgInfo[ZoomAIAreaRemoteMode].uwVSize)/2);
//		tOSD_Img2(&tOsdImgInfo[ZoomAIAreaRemoteMode], OSD_UPDATE);
//	}
//	else{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AI_AREA_SIZEUD_FLAG, 1, &tOsdImgInfo[3]);
//		tOsdImgInfo[ZoomAIAreaRemoteMode].uwXStart = (x1 + (x2 - x1 - tOsdImgInfo[ZoomAIAreaRemoteMode].uwHSize)/2);
//		tOsdImgInfo[ZoomAIAreaRemoteMode].uwYStart = (y1 + (y2 - y1 - tOsdImgInfo[ZoomAIAreaRemoteMode].uwVSize)/2);
//		tOSD_Img2(&tOsdImgInfo[ZoomAIAreaRemoteMode], OSD_UPDATE);
//	}
//}


uint16_t ustempZoomRedbox[4][4];
void UI_DrawBSDSmallRedBox(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t x_temp = 0, y_temp = 0;
	OSD_IMG_INFO tInfor_H_line, tInfor_V_line;
	OSD_IMG_INFO tInforZoomBoxSelect;

	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	if (tUI_State != UI_ADJUST_ZOOM_BSD_AREA_STATE)
	{
		return;
	}

//	printf("small x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
	if (((x2 - x1) < AI_AREA_MIN_SIZE) || ((y2 - y1) < AI_AREA_MIN_SIZE)|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][3] < tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][3] \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][1] > tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][1] \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][2] < tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][2] \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][0] > tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][0])
	{
		printf("rectangle too small x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][0] = BSD_REDBOX_POINX_X1;
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][1] = BSD_REDBOX_POINX_Y1;
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][2] = BSD_REDBOX_POINX_X2;
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][3] = BSD_REDBOX_POINX_Y2;
		x1 = BSD_REDBOX_POINX_X1;
		y1 = BSD_REDBOX_POINX_Y1;
		x2 = BSD_REDBOX_POINX_X2;
		y2 = BSD_REDBOX_POINX_Y2;
	}
	else if(((x2 - x1) > 1024) || ((y2 - y1) > 600))
	{
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][0] = BSD_REDBOX_POINX_X1;
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][1] = BSD_REDBOX_POINX_Y1;
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][2] = BSD_REDBOX_POINX_X2;
		tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][3] = BSD_REDBOX_POINX_Y2;
		x1 = BSD_REDBOX_POINX_X1;
		y1 = BSD_REDBOX_POINX_Y1;
		x2 = BSD_REDBOX_POINX_X2;
		y2 = BSD_REDBOX_POINX_Y2;
	}

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RED_HORI_LINE_30x4, 1, &tInfor_H_line);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RED_VERT_LINE_4x30, 1, &tInfor_V_line);

	for(x_temp = x1; x_temp <= x2 - BSD_AREA_LENGTH; x_temp += BSD_AREA_LENGTH)
	{
		//up
		tInfor_H_line.uwXStart = x_temp;
		tInfor_H_line.uwYStart = y1;
		tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
		{
			tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		}

		//down
		tInfor_H_line.uwXStart = x_temp;
		tInfor_H_line.uwYStart = y2 - BSD_AREA_WIDTH;
		tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
		{
			tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		}
	}

	for(y_temp = y1; y_temp <= y2 - BSD_AREA_LENGTH; y_temp += 30)
	{
		//left
		tInfor_V_line.uwXStart = x1 ;
		tInfor_V_line.uwYStart = y_temp;
		tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
		{
			tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		}

		//right
		tInfor_V_line.uwXStart = x2 - BSD_AREA_WIDTH;
		tInfor_V_line.uwYStart = y_temp;
		tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
		{
			tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		}
	}

	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_BOX_SELECT, 1, &tInforZoomBoxSelect);
	switch (ZoomBSDBoxMoveSelect)
	{
		case ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM  :
			//A
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
		case ZOOM_BSD_AI_AREA_SMALL_UP_ITEM  :
			//B
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;

		case ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM :
			//C
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
		case ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM :
			//D
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
		case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM:
			//A
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//B
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//C
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//D
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			break;
	}

	
//	if(ubUI_TouchPanelSts)
//	{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo[0]);
//		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
//	}

	
}

void UI_DrawBSDBigRedBox(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t x_temp = 0, y_temp = 0;
	OSD_IMG_INFO tInfor_H_line, tInfor_V_line, tOsdImgInfo;
	OSD_IMG_INFO tInforZoomBoxSelect;
	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	if (tUI_State != UI_ADJUST_ZOOM_BSD_AREA_STATE)
	{
		return;
	}

//	printf("big x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
	
	if (((x2 - x1) < AI_AREA_MIN_SIZE) || ((y2 - y1) < AI_AREA_MIN_SIZE) \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][3] < tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][3] \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][1] > tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][1] \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][2] < tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][2] \
		|| tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][0] > tUI_CuSetting.usZoomBSDSmallRedbox[ubZoomItemCurIdx][0])
	{
		printf("rectangle too small x1 = %d y1 = %d x2 = %d y2 = %d \n", x1, y1, x2, y2);
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][0] = BSD_YELLOWBOX_POINX_X1;
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][1] = BSD_YELLOWBOX_POINX_Y1;
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][2] = BSD_YELLOWBOX_POINX_X2;
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][3] = BSD_YELLOWBOX_POINX_Y2;
		x1 = BSD_YELLOWBOX_POINX_X1;;
		y1 = BSD_YELLOWBOX_POINX_Y1;
		x2 = BSD_YELLOWBOX_POINX_X2;
		y2 = BSD_YELLOWBOX_POINX_Y2;
	}
	else if(((x2 - x1) > 1024) || ((y2 - y1) > 600))
	{
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][0] = BSD_YELLOWBOX_POINX_X1;
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][1] = BSD_YELLOWBOX_POINX_Y1;
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][2] = BSD_YELLOWBOX_POINX_X2;
		tUI_CuSetting.usZoomBSDBigRedbox[ubZoomItemCurIdx][3] = BSD_YELLOWBOX_POINX_Y2;
		x1 = BSD_YELLOWBOX_POINX_X1;;
		y1 = BSD_YELLOWBOX_POINX_Y1;
		x2 = BSD_YELLOWBOX_POINX_X2;
		y2 = BSD_YELLOWBOX_POINX_Y2;
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_YELLOW_HORI_LINE_30x4, 1, &tInfor_H_line);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_YELLOW_VERT_LINE_4x30, 1, &tInfor_V_line);

	for(x_temp = x1; x_temp <= x2 - BSD_AREA_LENGTH; x_temp += BSD_AREA_LENGTH)
	{
		//up
		tInfor_H_line.uwXStart = x_temp;
		tInfor_H_line.uwYStart = y1;
		tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
		{
			tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		}

		//down
		tInfor_H_line.uwXStart = x_temp;
		tInfor_H_line.uwYStart = y2 - BSD_AREA_WIDTH;
		tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
		{
			tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
		}
	}

	for(y_temp = y1; y_temp <= y2 - BSD_AREA_LENGTH; y_temp += BSD_AREA_LENGTH)
	{
		//left
		tInfor_V_line.uwXStart = x1 ;
		tInfor_V_line.uwYStart = y_temp;
		tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
		{
			tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		}

		//right
		tInfor_V_line.uwXStart = x2 - BSD_AREA_WIDTH;
		tInfor_V_line.uwYStart = y_temp;
		tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
		{
			tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
			tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
		}
	}

	//Big Box HL
//	if(ZoomBSDBoxMoveSelect)
//	{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GREEN_HORI_LINE_30x8, 1, &tInfor_H_line);
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GREEN_VERT_LINE_8x30, 1, &tInfor_V_line);	
//	}
//	else
//	{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BULL_HORI_LINE_30x8, 1, &tInfor_H_line);
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_BULL_VERT_LINE_8x30, 1, &tInfor_V_line);
//	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_BOX_SELECT, 1, &tInforZoomBoxSelect);
	switch (ZoomBSDBoxMoveSelect)
	{
		case ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM  :
			//A
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
//			for(y_temp = y1; y_temp <= y2 - BSD_AREA_LENGTH; y_temp += BSD_AREA_LENGTH)
//			{
//				//left
//				tInfor_V_line.uwXStart = x1;
//				tInfor_V_line.uwYStart = y_temp;
//				tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
//				{
//					tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				}
//			}
			break;
		case ZOOM_BSD_AI_AREA_BIG_UP_ITEM :
			//B
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
//			for(x_temp = x1; x_temp <= x2 - BSD_AREA_LENGTH; x_temp += BSD_AREA_LENGTH)
//			{
//				//up
//				tInfor_H_line.uwXStart = x_temp;
//				tInfor_H_line.uwYStart = y1;
//				tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
//				{
//					tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				}
//			}
			break;
		case ZOOM_BSD_AI_AREA_BIG_RIGHT_ITEM  :
			//C
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//right
//			for(y_temp = y1; y_temp <= y2 - BSD_AREA_LENGTH; y_temp += BSD_AREA_LENGTH)
//			{
//				tInfor_V_line.uwXStart = x2 - 8;
//				tInfor_V_line.uwYStart = y_temp;
//				tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
//				{
//					tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				}
//			}
			break;
		case ZOOM_BSD_AI_AREA_BIG_DOWN_ITEM :
			//D
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
//			for(x_temp = x1; x_temp <= x2 - BSD_AREA_LENGTH; x_temp += BSD_AREA_LENGTH)
//			{
//				//down
//				tInfor_H_line.uwXStart = x_temp;
//				tInfor_H_line.uwYStart = y2 - 8;
//				tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
//				{
//					tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				}
//			}
			break;
		case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM:
			//A
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//B
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//C
			tInforZoomBoxSelect.uwXStart = x2 - ZOOM_BOX_SELECT_RAD - BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
			//D
			tInforZoomBoxSelect.uwXStart = x1 - ZOOM_BOX_SELECT_RAD + BSD_AREA_WIDTH/2;
			tInforZoomBoxSelect.uwYStart = y2 - ZOOM_BOX_SELECT_RAD;
			tOSD_Img2(&tInforZoomBoxSelect, OSD_QUEUE);
//			for(x_temp = x1; x_temp <= x2 - BSD_AREA_LENGTH; x_temp += BSD_AREA_LENGTH)
//			{
//				//up
//				tInfor_H_line.uwXStart = x_temp;
//				tInfor_H_line.uwYStart = y1;
//				tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
//				{
//					tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				}

//				//down
//				tInfor_H_line.uwXStart = x_temp;
//				tInfor_H_line.uwYStart = y2 - 8;
//				tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				if(x_temp + BSD_AREA_LENGTH > x2 - BSD_AREA_LENGTH)
//				{
//					tInfor_H_line.uwXStart = x2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_H_line, OSD_QUEUE);
//				}
//			}

//			for(y_temp = y1; y_temp <= y2 - BSD_AREA_LENGTH; y_temp += BSD_AREA_LENGTH)
//			{
//				//left
//				tInfor_V_line.uwXStart = x1 ;
//				tInfor_V_line.uwYStart = y_temp;
//				tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
//				{
//					tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				}

//				//right
//				tInfor_V_line.uwXStart = x2 - 8;
//				tInfor_V_line.uwYStart = y_temp;
//				tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				if(y_temp + BSD_AREA_LENGTH > y2 - BSD_AREA_LENGTH)
//				{
//					tInfor_V_line.uwYStart = y2 - BSD_AREA_LENGTH;
//					tOSD_Img2(&tInfor_V_line, OSD_QUEUE);
//				}
//			}
			break;
	}
	
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}
	
	//OSD_UpdateImg2();
}


//void UI_DrawBSDBox(uint16_t usZoomBSDSmallRedbox[4], uint16_t usZoomBSDBigRedbox[4])
//{
//	OSD_IMG_INFO tOsdImgInfo;
//	if (tUI_State != UI_ADJUST_ZOOM_BSD_AREA_STATE)
//	{
//		return;
//	}

//	printf("usZoomBSDSmallRedbox = 0x%x", usZoomBSDSmallRedbox);
//	printf("usZoomBSDBigRedbox = 0x%x", usZoomBSDBigRedbox);
//	
//	//画小框和大框	
//	UI_DrawBSDSmallRedBox(usZoomBSDSmallRedbox[0], usZoomBSDSmallRedbox[1], usZoomBSDSmallRedbox[2], usZoomBSDSmallRedbox[3]);
//	UI_DrawBSDBigRedBox(usZoomBSDBigRedbox[0], usZoomBSDBigRedbox[1], usZoomBSDBigRedbox[2], usZoomBSDBigRedbox[3]);


//	//返回键
//	if(ubUI_TouchPanelSts)
//	{
//		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);
//		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
//	}

//	OSD_UpdateImg2();
//}


void storeAIarea(void)
{
	UI_CamNum_t tCamNum;

	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		ustempZoomRedbox[tCamNum][0] = tUI_CuSetting.usZoomRedbox[tCamNum][0];
		ustempZoomRedbox[tCamNum][1] = tUI_CuSetting.usZoomRedbox[tCamNum][1];
		ustempZoomRedbox[tCamNum][2] = tUI_CuSetting.usZoomRedbox[tCamNum][2];
		ustempZoomRedbox[tCamNum][3] = tUI_CuSetting.usZoomRedbox[tCamNum][3];
	}
}

void loadAIarea(void)
{
	UI_CamNum_t tCamNum;

	for (tCamNum = CAM1; tCamNum <= CAM4; tCamNum++)
	{
		tUI_CuSetting.usZoomRedbox[tCamNum][0] = ustempZoomRedbox[tCamNum][0];
		tUI_CuSetting.usZoomRedbox[tCamNum][1] = ustempZoomRedbox[tCamNum][1];
		tUI_CuSetting.usZoomRedbox[tCamNum][2] = ustempZoomRedbox[tCamNum][2];
		tUI_CuSetting.usZoomRedbox[tCamNum][3] = ustempZoomRedbox[tCamNum][3];
	}
}

void UI_EnterAdjustAreaClosePakt(uint8_t CAM, uint8_t isExit)
{
	//关闭BSD和Trace
	if(tUI_CamStatus[CAM].ubTrace)
	{
		tTWC_Send(TWC_STA1 + CAM, TWC_PELCO, (uint8_t *)&pelco_Trace[0 + 7 * isExit], 7, 10);
	}

	if(tUI_CamStatus[CAM].ubBSD)
	{
		tTWC_Send(TWC_STA1 + CAM, TWC_PELCO, (uint8_t *)&pelco_BSD[0 + 7 * isExit], 7, 10);
	}
}

void UI_EnterAdjustZoomArea(void)
{
	OSD_IMG_INFO tEraserOsdImgInfo, tOsdImgInfo;
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	// if (tUI_State != UI_DISPLAY_STATE)
	// {
	// 	return;
	// }

	storeAIarea();

	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tEraserOsdImgInfo);
	OSD_EraserImg1(&tEraserOsdImgInfo);

	tUI_State = UI_ADJUST_ZOOM_AREA_STATE;
	MenuOnFlag = TRUE;

	tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_TRACE_CALIBRATE;

	//设置为选中左边框

	ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM;

	UI_EnterAdjustAreaClosePakt(CAM, FALSE);

	printf("Draw zoom box \n");
	UI_DrawRedBox(tUI_CuSetting.usZoomRedbox[CAM][0], tUI_CuSetting.usZoomRedbox[CAM][1], \
		          tUI_CuSetting.usZoomRedbox[CAM][2], tUI_CuSetting.usZoomRedbox[CAM][3]);

	//显示虚拟按键
	tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
	tZoomVirtualKeyItem.tSubMenuInfo.ubItemPreIdx = ZOOM_VIRTUAL_LEFT_KEY;
	UI_ZoomDrawVirtualKey();
}

void UI_ZoomDrawVirtualKey(void)
{
	OSD_IMG_INFO tOsdImgInfo[12], tOsdImgInfo1;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_LEFT_KEY_NOR, 12, &tOsdImgInfo);
	
	//显示上、下、左、OK、右、重置虚拟按键，并将index指向左按键
	for(int i = 0; i < ZOOM_VIRTUAL_KEY_MAX; i++)
	{
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
	}

	//restart按键
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo1);
//	tOsdImgInfo1.uwXStart = BSD_RESTART_ITEM_DIST_X;
//	tOsdImgInfo1.uwYStart = BSD_RESTART_ITEM_DIST_Y;
//	tOSD_Img2(&tOsdImgInfo1, OSD_QUEUE);

	//virtual key hl
	tOSD_Img2(&tOsdImgInfo[tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx*2 + 1], OSD_UPDATE);

	//tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
}

void UI_EnterAdjustZoomBSDArea(void)
{
	OSD_IMG_INFO tEraserOsdImgInfo;
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tEraserOsdImgInfo);
	OSD_EraserImg1(&tEraserOsdImgInfo);

	tUI_State = UI_ADJUST_ZOOM_BSD_AREA_STATE;
	MenuOnFlag = TRUE;
	tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_BSD_CALIBRATE;

	//设置为选中左边框
	ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM;

	UI_EnterAdjustAreaClosePakt(CAM, FALSE);
	
//	UI_DrawBSDBox(tUI_CuSetting.usZoomBSDSmallRedbox[CAM],tUI_CuSetting.usZoomBSDBigRedbox[CAM]);
	UI_DrawBSDSmallRedBox(tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1], \
		          tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]);
	UI_DrawBSDBigRedBox(tUI_CuSetting.usZoomBSDBigRedbox[CAM][0], tUI_CuSetting.usZoomBSDBigRedbox[CAM][1], \
		          tUI_CuSetting.usZoomBSDBigRedbox[CAM][2], tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]);

	//显示虚拟按键
	tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
	tZoomVirtualKeyItem.tSubMenuInfo.ubItemPreIdx = ZOOM_VIRTUAL_LEFT_KEY;
	UI_ZoomDrawVirtualKey();
}


/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/

//------------------------------------------------------------------------------
void UI_DisplayTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	if(ubUI_CuStandbyFlag && Touch_Info->Gesture != TOUCH_TURNUP)
		return;
	OSD_IMG_INFO tOsdImgInfo[2];
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			if((!TriggerLock)&&(tUI_CuSetting.ubisZoomCameraFlag[0] || tUI_CuSetting.ubisZoomCameraFlag[1] || tUI_CuSetting.ubisZoomCameraFlag[2] || tUI_CuSetting.ubisZoomCameraFlag[3]))
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOMNOR_MENUICON, 2, &tOsdImgInfo[0]);
				if(Touch_Info->startX > tOsdImgInfo[0].uwXStart
						&& Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize
						&& Touch_Info->startY > tOsdImgInfo[0].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize)
				{
					tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
					UI_ZoomSubMenuPage(ENTER_ARROW);
					break;
				}
			}
			if(Touch_Info->startY > 510)//进入home menu
			{
				UI_MenuKey();
			}
			else//画面跳转
			{
				if((tUI_ViewModeSel == SCANVIEW_ITEM) || (ubUI_FinishViewSwitch == FALSE) || TriggerLock)
					return;
				switch(tCamViewSel.tCamViewType)
				{
					case SINGLE_VIEW:	
						tUI_ViewModeSel = tUI_SaveViewNum;
						break;
					case DUAL_VIEW:	
						tUI_SaveViewNum = tUI_ViewModeSel;
						if(Touch_Info->startX < 512)
						{
							tUI_ViewModeSel = tCamViewSel.tCamViewPool[0];
						}
						else
						{
							tUI_ViewModeSel = tCamViewSel.tCamViewPool[1];
						}
							
						break;
					case QUAD_VIEW:	
						tUI_SaveViewNum = tUI_ViewModeSel;
						if(Touch_Info->startX < 512 && Touch_Info->startY < 300)
						{
							tUI_ViewModeSel = tCamViewSel.tCamViewPool[0];
						}
						else if(Touch_Info->startX >= 512 && Touch_Info->startY < 300)
						{
							tUI_ViewModeSel = tCamViewSel.tCamViewPool[1];
						}
						else if(Touch_Info->startX < 512 && Touch_Info->startY >= 300)
						{
							tUI_ViewModeSel = tCamViewSel.tCamViewPool[2];
						}
						else
						{
							tUI_ViewModeSel = tCamViewSel.tCamViewPool[3];
						}
						break;
					default:
						break;
				}
				DeskTopShowView = tUI_ViewModeSel;
				UI_SwitchViewType(tUI_ViewModeSel,TRUE);
				if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
					UI_SaveLastMode();
				
				UI_ClearOsdImageNoUpdate();
				UI_DrawDesktopIcon();
			}
			
			break;
		case TOUCH_TURNUP:
			if(ubUI_CuStandbyFlag && !ubUI_CuPowerOffFlag && !TriggerLock)
			{
				UI_WakeupStandby();
			}
			break;
		case TOUCH_TURNDOWN:
			if(!ubUI_CuStandbyFlag && !TriggerLock)	
			{
				UI_EnterStandby();
			}
			break;
		case TOUCH_TURNLEFT:
			if((tUI_ViewModeSel == SCANVIEW_ITEM) || (ubUI_FinishViewSwitch == FALSE) || TriggerLock)
					return;
			
			if(tUI_ViewModeSel > CAM1)
				tUI_ViewModeSel --;
			else
				tUI_ViewModeSel = QUALVIEW_ITEM;
			
			DeskTopShowView = tUI_ViewModeSel;
			UI_SwitchViewType(tUI_ViewModeSel,TRUE);
			if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
				UI_SaveLastMode();
			
			UI_ClearOsdImageNoUpdate();
			UI_DrawDesktopIcon();
			break;
			
		case TOUCH_TURNRIGHT:
			if((tUI_ViewModeSel == SCANVIEW_ITEM) || (ubUI_FinishViewSwitch == FALSE) || TriggerLock)
					return;
			if(tUI_ViewModeSel < QUALVIEW_ITEM)
				tUI_ViewModeSel ++;
			else
				tUI_ViewModeSel = CAM1;
			
			DeskTopShowView = tUI_ViewModeSel;
			UI_SwitchViewType(tUI_ViewModeSel,TRUE);
			if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
				UI_SaveLastMode();
			
			UI_ClearOsdImageNoUpdate();
			UI_DrawDesktopIcon();
			break;

		default :

			break;
	}	
}

//------------------------------------------------------------------------------
void UI_PairingTouchControl(TOUCH_EVENT_t *Touch_Info)
{
	APP_EventMsg_t tUI_PairMessage;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:	
			tUI_PairMessage.ubAPP_Event = APP_PAIRING_STOP_EVENT;
			UI_SendMessageToAPP(&tUI_PairMessage);
			
			break;

		default :

			break;
	}	

}

#endif		//! End of #ifdef BSP_D_SNCC71_GM8285C_RX_V2
