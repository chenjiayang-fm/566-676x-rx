
#include "UI_BUCCU[WSVGA]_MENU.h"
#ifdef BSP_D_SNCC71_GM8285C_RX_V2
/*************************************************************Home Menu**********************************************************************************/
#define UI_MENUICON_NUM 8
#define DRAW_HEATING_ICON_SPACE_CHANGE 53

void updateDigitalZoomSlide(UI_CamNum_t chn, uint8_t ubPreVolumeLvl);
void draw_restart_cam_box(void);

static uint8_t ubdigitalZoomScaleFlage=0;
extern uint8_t  ubisShowZoomflag;
extern uint8_t isShowFoucsRestart;
extern uint32_t ulDigitalZoomCount;
extern uint8_t ubZoomRestartFlag;

//------------------------------------------------------------------------------

bool is_any_heating_cam_paired()
{
	printf("Heating Cam Flag: %d %d %d %d\n", tUI_CuSetting.ubiHeatingCamFlag[0], tUI_CuSetting.ubiHeatingCamFlag[1], tUI_CuSetting.ubiHeatingCamFlag[2], tUI_CuSetting.ubiHeatingCamFlag[3]);
	return	tUI_CuSetting.ubiHeatingCamFlag[0] || 
			tUI_CuSetting.ubiHeatingCamFlag[1] ||
			tUI_CuSetting.ubiHeatingCamFlag[2] || 
			tUI_CuSetting.ubiHeatingCamFlag[3];
}

//------------------------------------------------------------------------------
void UI_DrawMenuPage(void)
{
	uint8_t i;
	OSD_IMG_INFO tOsdImgInfo[UI_MENUICON_NUM*2];
	MenuOnFlag = TRUE;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ADO_MODENOR_MENUICON, (UI_MENUICON_NUM*2), &tOsdImgInfo[0]);

	// draw heating icon?
	if(!is_any_heating_cam_paired())
	{
		for(i = 0; i < UI_MENUICON_NUM-2; i++)
			tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[2*(UI_MENUICON_NUM-1)], OSD_QUEUE);
		tOSD_Img2(&tOsdImgInfo[2*tUI_MenuItem.ubItemIdx + UI_ICON_HIGHLIGHT], OSD_UPDATE);
	}else{
		// start -> 93, +6*106(=636), ->729, last one on 835
		// 1-6
		for(i = 0; i < UI_MENUICON_NUM-2; i++)
		{
			tOsdImgInfo[2*i].uwXStart -= DRAW_HEATING_ICON_SPACE_CHANGE;
			tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
		}
		
		// Heating
		tOSD_Img2(&tOsdImgInfo[2*(UI_MENUICON_NUM-2)], OSD_QUEUE);
		
		// PowOff
		tOsdImgInfo[2*(UI_MENUICON_NUM-1)].uwXStart += DRAW_HEATING_ICON_SPACE_CHANGE;
		tOSD_Img2(&tOsdImgInfo[2*(UI_MENUICON_NUM-1)], OSD_QUEUE);
		
		// highlight
		OSD_IMG_INFO *highlightImgInfo = &tOsdImgInfo[2*tUI_MenuItem.ubItemIdx + UI_ICON_HIGHLIGHT];
		if(tUI_MenuItem.ubItemIdx == POWER_ITEM){
			highlightImgInfo->uwXStart += DRAW_HEATING_ICON_SPACE_CHANGE;
		}else if(tUI_MenuItem.ubItemIdx != HEATING_ITEM){
			highlightImgInfo->uwXStart -= DRAW_HEATING_ICON_SPACE_CHANGE;
		}
		tOSD_Img2(highlightImgInfo, OSD_UPDATE);
	}
	
	OSD_Weight(OSD_WEIGHT_8DIV8);
}
//------------------------------------------------------------------------------

static void UI_MenuDrawMenuItem(void)
{
	uint16_t uwMenuItemOsdImg[UI_MENUICON_NUM] = {
													OSD2IMG_ADO_MODENOR_MENUICON,
													OSD2IMG_VDO_MODENOR_MENUICON,
													OSD2IMG_PAIRNOR_MENUICON,
													OSD2IMG_RECNOR_MENUICON,
													OSD2IMG_PLYNOR_MENUICON,
													OSD2IMG_SETTINGNOR_MENUICON,
													OSD2IMG_HEATINGNOR_MENUICON,
													OSD2IMG_POWERNOR_MENUICON,
													};
	if(!is_any_heating_cam_paired()){
		UI_DrawHLandNormalIcon(uwMenuItemOsdImg[tUI_MenuItem.ubItemPreIdx], 
			(uwMenuItemOsdImg[tUI_MenuItem.ubItemIdx] + UI_ICON_HIGHLIGHT));
	}else{
		OSD_IMG_INFO tOsdImgInfo;
		
		uint16_t uwNormalOsdImgIdx = uwMenuItemOsdImg[tUI_MenuItem.ubItemPreIdx];
		uint16_t uwHighLigthOsdImgIdx = uwMenuItemOsdImg[tUI_MenuItem.ubItemIdx];
		
		//! Draw normal item
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwNormalOsdImgIdx, 1, &tOsdImgInfo);
		
		if(uwNormalOsdImgIdx == OSD2IMG_POWERNOR_MENUICON){
			tOsdImgInfo.uwXStart += DRAW_HEATING_ICON_SPACE_CHANGE;
		}else if(uwNormalOsdImgIdx != OSD2IMG_HEATINGNOR_MENUICON){
			tOsdImgInfo.uwXStart -= DRAW_HEATING_ICON_SPACE_CHANGE;
		}
		
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		
		//! Draw highlight item
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwHighLigthOsdImgIdx+UI_ICON_HIGHLIGHT, 1, &tOsdImgInfo);
		
		if(uwHighLigthOsdImgIdx == OSD2IMG_POWERNOR_MENUICON){
			tOsdImgInfo.uwXStart += DRAW_HEATING_ICON_SPACE_CHANGE;
		}else if(uwHighLigthOsdImgIdx != OSD2IMG_HEATINGNOR_MENUICON){
			tOsdImgInfo.uwXStart -= DRAW_HEATING_ICON_SPACE_CHANGE;
		}
		
		tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	}
}

void UI_HomeMenu_Key(UI_ArrowKey_t tArrowKey)
{
	tUI_MenuItem.ubItemPreIdx = tUI_MenuItem.ubItemIdx;
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			if(tUI_MenuItem.ubItemIdx == ADO_MODE_ITEM)
				tUI_MenuItem.ubItemIdx = POWER_ITEM;
			else
				if(tUI_MenuItem.ubItemIdx == POWER_ITEM && !is_any_heating_cam_paired())
					tUI_MenuItem.ubItemIdx -= 2;
				else
					tUI_MenuItem.ubItemIdx--;
			break;
		case RIGHT_ARROW:
			if(tUI_MenuItem.ubItemIdx == POWER_ITEM)
				tUI_MenuItem.ubItemIdx = ADO_MODE_ITEM;
			else
				if(tUI_MenuItem.ubItemIdx == SETTING_ITEM && !is_any_heating_cam_paired())
					tUI_MenuItem.ubItemIdx+=2;
				else
					tUI_MenuItem.ubItemIdx++;
			break;
		case EXIT_ARROW:
			UI_ClearOsdImageNoUpdate();
			MenuOnFlag = FALSE;
			tUI_MenuItem.ubItemPreIdx = ADO_MODE_ITEM;
			tUI_MenuItem.ubItemIdx 	  = ADO_MODE_ITEM;
			tUI_State = UI_DISPLAY_STATE;
			UI_DrawDesktopIcon();
			return;
		case ENTER_ARROW:
			if(tUI_MenuItem.ubItemIdx != RECORD_ITEM)
				UI_SubKeyMenu(ENTER_ARROW);
			else//����¼��
			{
				UI_MenuKey();
				UI_EnterKey();
			}
			
		default:
			return;
	}
	UI_MenuDrawMenuItem();
}

/*************************************************************Volume Menu**********************************************************************************/

//------------------------------------------------------------------------------
void UI_DrawVolumeMenuPage(void)
{
	OSD_IMG_INFO tOsdImgInfo[4];
	MenuOnFlag = TRUE;
	tUI_State = UI_SET_VOLUME_STATE;
	SendIrCodeFlag = 1;
	if(TriggerLock && DeskTopShowView <= CAM4 && tUI_CuSetting.tParkingLineEnable[DeskTopShowView] != PARKING_LINE_OFF)//����������
	{
		UI_EraseParkingLine(DeskTopShowView,OSD_QUEUE);
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SOUND_SLIDER, 4, &tOsdImgInfo[0]);
	
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	if(tUI_CuSetting.ubMuteFlag)
		tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	
	
	//����
	tOsdImgInfo[3].uwXStart = UI_Map(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,0,99,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[3].uwHSize);
	tOSD_Img2(&tOsdImgInfo[3], OSD_QUEUE);
	//��ֵ
	if(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl < 10)
	{
		UI_ShowButtonValueHighLight(0,tOsdImgInfo[3].uwXStart + 4,tOsdImgInfo[3].uwYStart + 9,OSD_QUEUE);
		UI_ShowButtonValueHighLight(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,tOsdImgInfo[3].uwXStart + 24,tOsdImgInfo[3].uwYStart + 9,OSD_UPDATE);
	}
	else
		UI_ShowButtonValueHighLight(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,tOsdImgInfo[3].uwXStart + 4,tOsdImgInfo[3].uwYStart + 9,OSD_UPDATE);
			
}
//------------------------------------------------------------------------------
void UI_VolumeMenuDrawMenuItem(uint8_t ubPreVolumeLvl)
{
	OSD_IMG_INFO tOsdImgInfo[5];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SOUND_SLIDER, 5, &tOsdImgInfo[0]);

	if(tUI_CuSetting.ubMuteFlag)
		tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	//����
	tOsdImgInfo[4].uwXStart = UI_Map(ubPreVolumeLvl,0,99,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[4].uwHSize);
	tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
	
	tOsdImgInfo[3].uwXStart = UI_Map(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,0,99,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[3].uwHSize);
	tOSD_Img2(&tOsdImgInfo[3], OSD_QUEUE);

	//��ֵ
	if(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl < 10)
	{
		UI_ShowButtonValueHighLight(0,tOsdImgInfo[3].uwXStart + 4,tOsdImgInfo[3].uwYStart + 9,OSD_QUEUE);
		UI_ShowButtonValueHighLight(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,tOsdImgInfo[3].uwXStart + 24,tOsdImgInfo[3].uwYStart + 9,OSD_UPDATE);
	}
	else
		UI_ShowButtonValueHighLight(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl,tOsdImgInfo[3].uwXStart + 4,tOsdImgInfo[3].uwYStart + 9,OSD_UPDATE);

}
//------------------------------------------------------------------------------

void UI_VolumeMenuPage(UI_ArrowKey_t tArrowKey)
{
	uint8_t ubPreVolumeLvl = tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl;
	
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			if(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl > 0)
				tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl --;
			else 
				return;
			break;
		case RIGHT_ARROW:
			if(tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl < 99)
				tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl ++;
			else 
				return;
			break;
		case EXIT_ARROW:
			SendIrCodeFlag = 0;
			UI_UpdateDevStatusInfo();
			UI_ClearOsdImageNoUpdate();
			MenuOnFlag = FALSE;
			tUI_State = UI_DISPLAY_STATE;
			UI_DrawDesktopIcon();
			if(TriggerLock && DeskTopShowView <= CAM4 && tUI_CuSetting.tParkingLineEnable[DeskTopShowView] != PARKING_LINE_OFF)
			{
				UI_DrawParkingLine(DeskTopShowView,OSD_UPDATE);
			}
			return;
		case ENTER_ARROW:
			tUI_CuSetting.ubMuteFlag = 1 - tUI_CuSetting.ubMuteFlag;
			break;
		case MUTE_ARROW:
			tUI_CuSetting.ubMuteFlag = 1 - tUI_CuSetting.ubMuteFlag;
			break;
		default:
			return;
	}

	UI_VolumeMenuDrawMenuItem(ubPreVolumeLvl);
	UI_AudioOn();
}
//------------------------------------------------------------------------------
void UI_ShowSdCardInfo(UI_ArrowKey_t tArrowKey)
{
	switch(tArrowKey)
	{
		case EXIT_ARROW:
			UI_ClearOsdImageNoUpdate();
			MenuOnFlag = FALSE;
			tUI_State = UI_DISPLAY_STATE;
			UI_DrawDesktopIcon();
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void UI_ShowTxVersion(UI_ArrowKey_t tArrowKey)
{
	switch(tArrowKey)
	{
		case EXIT_ARROW:
			UI_ClearOsdImageNoUpdate();
			MenuOnFlag = FALSE;
			tUI_State = UI_DISPLAY_STATE;
			UI_DrawDesktopIcon();
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
void draw_restart_cam_box(void)
{
	OSD_IMG_INFO tOsdImgInfo[5];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_FOCUS_RESTART_EN:
								(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_FOCUS_RESTART_GER:
								(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_FOCUS_RESTART_FR:OSD2IMG_FOCUS_RESTART_CHN, 1, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);

	tOSD_GetOsdImgInfor(1, OSD_IMG2,(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_FORMAT_BG:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_FORMAT_BG_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_FORMAT_BG_FR:OSD2IMG_SDCARD_FORMAT_BG_CHN, 5, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
	tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
}

static void UI_ZoomTouchHL(uint8_t zoomSettingItemIndex)
{
	OSD_IMG_INFO tOsdImgInfo[4];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_IN_TOUCH_HL, 4, &tOsdImgInfo[0]);
	switch (zoomSettingItemIndex)
	{
		case ZOOM_IN:
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
			break;
		case ZOOM_OUT:
			tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
			break;
		case ZOOM_NEAR:
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
			break;
		case ZOOM_FAR:
			tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
			break;
		default:
			break;
	}
}

static void UI_ZoomSettingCameraExecute() //���޸�
{
	OSD_IMG_INFO tOsdImgInfo[5];
	TWC_STATUS send_ret = TWC_FAIL;
	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	uint8_t ubSubMenuItemIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
	
	if(ubSubMenuItemIdx == ZOOM_IN)
	{
		
		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
		UI_ZoomTouchHL(ZOOM_IN);
//		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
		osDelay(100);
	}
	else if(ubSubMenuItemIdx == ZOOM_OUT)
	{
		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
		UI_ZoomTouchHL(ZOOM_OUT);
//		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
		osDelay(100);
	}
	else if(ubSubMenuItemIdx == ZOOM_NEAR)
	{	
		UI_ZoomTouchHL(ZOOM_NEAR);
		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_near[0], 7, 8);
		osDelay(100);
	}
	else if(ubSubMenuItemIdx == ZOOM_FAR)
	{
		UI_ZoomTouchHL(ZOOM_FAR);
		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_far[0], 7, 8);
		osDelay(100);
	}
	else if(ubSubMenuItemIdx == ZOOM_AUTO)
	{
		send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_auto[0], 7, 8);
		osDelay(100);
	}
	else if(ubSubMenuItemIdx == ZOOM_RESTART)
	{
		draw_restart_cam_box();
		isShowFoucsRestart = 1;
	}
	else if(ubSubMenuItemIdx == ZOOM_SLIDER)
	{
		ubdigitalZoomScaleFlage = 1 - ubdigitalZoomScaleFlage;
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_SLIDE, 5, &tOsdImgInfo[0]);
		if(ubdigitalZoomScaleFlage)
		    tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
		else
		    tOSD_Img2(&tOsdImgInfo[3], OSD_QUEUE);

		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
		tOsdImgInfo[2].uwXStart = UI_Map(tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale, 10, 50,
			tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
		
		tOsdImgInfo[1].uwXStart = UI_Map(tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale, 10, 50,
			tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
		tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
	}
	else if(ubSubMenuItemIdx == ZOOM_TRCAE)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo[0]);
		tUI_CamStatus[ubZoomItemCurIdx].ubTrace = 1 - tUI_CamStatus[ubZoomItemCurIdx].ubTrace;
		tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubTrace*2 + 1], OSD_UPDATE);
		tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_Trace[0 + 7*tUI_CamStatus[ubZoomItemCurIdx].ubTrace], 7, 10);
	}
	else if(ubSubMenuItemIdx == ZOOM_TRACE_CALIBRATE)
	{
		UI_EnterAdjustZoomArea();
	}
	else if(ubSubMenuItemIdx == ZOOM_BSD)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo[0]);
		tUI_CamStatus[ubZoomItemCurIdx].ubBSD = 1 - tUI_CamStatus[ubZoomItemCurIdx].ubBSD;
		tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2 + 1].uwYStart = BSD_OFF_NOR_DIST;
		tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2 + 1], OSD_UPDATE);
		tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_BSD[0 + 7*tUI_CamStatus[ubZoomItemCurIdx].ubBSD], 7, 10);
	}
	else if(ubSubMenuItemIdx == ZOOM_BSD_CALIBRATE)
	{
		UI_EnterAdjustZoomBSDArea();
	}
	else
		return;
}

void Draw_Zoom_slider(uint8_t ubZoomItemCurIdx)
{
	OSD_IMG_INFO tOsdImgInfo[3];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_SLIDE, 3, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	tOsdImgInfo[2].uwXStart = UI_Map(tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale, 10, 50,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
	tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	
	tOsdImgInfo[1].uwXStart = UI_Map(tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale, 10, 50,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
	tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
}


void UI_ZoomDrawSettingCameraItem()
{
	OSD_IMG_INFO tOsdImgInfo[4];
	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	uint8_t ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemCurIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;

//	printf("UI_ZoomDrawSubTouchMenuItem\n");
//	printf("tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx = %d\n", tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx);
//	printf("tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = %d\n", tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx);
	
	//----------------------------pre----------------------------------------------
	if(ubItemPreIdx != ubItemCurIdx)
	{
		ubdigitalZoomScaleFlage = 0;
		
		if(ubItemPreIdx == ZOOM_SLIDER)//slider
		{
			//����ZOOMѡ�п�
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_GREEN_BOX, 1, &tOsdImgInfo[0]);
			OSD_EraserImg2_NoUpdate(&tOsdImgInfo[0]);

			//��ʾ����
			Draw_Zoom_slider(ubZoomItemCurIdx);
		}
		else if(ubItemPreIdx == ZOOM_OUT)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_OUT_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_IN)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_IN_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_NEAR)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_NEAR_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_FAR)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_FAR_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_RESTART)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_AUTO)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_AUTO_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_TRCAE)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubTrace*2], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_TRACE_CALIBRATE)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_CALIBRATE_NOR, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_BSD)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo[0]);
			tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2].uwYStart = BSD_OFF_NOR_DIST;
			tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2], OSD_UPDATE);
		}
		else if(ubItemPreIdx == ZOOM_BSD_CALIBRATE)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_CALIBRATE_NOR, 1, &tOsdImgInfo[0]);
			tOsdImgInfo[0].uwYStart = BSD_OFF_NOR_DIST;
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
	}

	//----------------------------cur----------------------------------------------
	if(ubItemCurIdx == ZOOM_SLIDER)//slider
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_GREEN_BOX, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		Draw_Zoom_slider(ubZoomItemCurIdx);
	}
	else if(ubItemCurIdx == ZOOM_OUT)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_OUT_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_IN)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_IN_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_NEAR)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_NEAR_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_FAR)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_FAR_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_RESTART)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_OUT_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_AUTO)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_AUTO_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_TRCAE)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubTrace*2 + 1], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_TRACE_CALIBRATE)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_CALIBRATE_HL, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_BSD)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo[0]);
		tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2 + 1].uwYStart = BSD_OFF_NOR_DIST;
		tOSD_Img2(&tOsdImgInfo[tUI_CamStatus[ubZoomItemCurIdx].ubBSD*2 + 1], OSD_UPDATE);
	}
	else if(ubItemCurIdx == ZOOM_BSD_CALIBRATE)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_CALIBRATE_HL, 1, &tOsdImgInfo[0]);
		tOsdImgInfo[0].uwYStart = BSD_OFF_NOR_DIST;
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
}

void UI_DrawZoomSettingCameraPage(void)
{	
	UI_ClearOsdImageNoUpdate();

	UI_DrawZoomButton(1);
}


//------------------------------------------------------------------------------zoom-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------zoom-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------zoom-----------------------------------------------------------------------------------
uint8_t curr_indx = 0;


uint8_t handle_zoom_key_restart_ops(UI_ArrowKey_t tArrowKey)
{
	OSD_IMG_INFO tOsdImgInfo[5],tOsdImgInfo1;
	uint8_t touch_idx = 100;
	TWC_STATUS send_ret = TWC_FAIL;
	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	tOSD_GetOsdImgInfor(1, OSD_IMG2,(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_FORMAT_BG:
								(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_FORMAT_BG_GER:
								(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_FORMAT_BG_FR:OSD2IMG_SDCARD_FORMAT_BG_CHN, 5, &tOsdImgInfo[0]);

	switch (tArrowKey)
	{
		case ENTER_ARROW: // 
		{
			touch_idx = curr_indx;
			break;
		}
		case LEFT_ARROW: //
		{
			curr_indx = 1 - curr_indx;
			if(curr_indx)
			{
				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
				tOSD_Img2(&tOsdImgInfo[4], OSD_UPDATE);
			}
			else
			{
			    tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
				tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
			}
			break;
		}
		case RIGHT_ARROW: //
		{
			curr_indx = 1 - curr_indx;
			if(curr_indx)
			{
				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
				tOSD_Img2(&tOsdImgInfo[4], OSD_UPDATE);
			}
			else
			{
			    tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
				tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
			}
			break;
		}
		default:
		{
			break;
		}
	}
	if (touch_idx != 100)
	{
		switch (touch_idx)
		{
			case 0: //
			{
				//printf("restart button\n");
				if(tUI_CuSetting.ubSD_Switch)
				{
					ubZoomRestartFlag = TRUE;
					tUI_CuSetting.ubSD_Switch = 0;
					UI_SwitchViewType(ubZoomItemCurIdx,FALSE);
				}
				send_ret = tTWC_Send(TWC_STA1 + ubZoomItemCurIdx, TWC_PELCO, (uint8_t *)&pelco_restart[0], 7, 8);
				osDelay(100);
				tUI_CamStatus[ubZoomItemCurIdx].digitalZoomScale = 10;
				isShowFoucsRestart = 0;
				curr_indx = 0;
				UI_ClearOsdImageNoUpdate();
				UI_DrawZoomButton(1);
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = ZOOM_SLIDER;
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_SLIDER;
				return 1;
			}
			case 1: // restart device
			{
				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
				tOSD_Img2(&tOsdImgInfo[4], OSD_UPDATE);
				isShowFoucsRestart = 0;
				curr_indx = 0;
				UI_ClearOsdImageNoUpdate();
				UI_DrawZoomButton(1);
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = ZOOM_SLIDER;
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_SLIDER;
				return 1;
			}
			default:
			{
				return 0;
			}
		}
	}
	return 0;
}




static void UI_ZoomSettingCameraExit()
{
	if(isShowFoucsRestart){
		isShowFoucsRestart = 0;
		curr_indx = 0;
		UI_ClearOsdImageNoUpdate();
		UI_DrawZoomButton(1);
		tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = ZOOM_SLIDER;
		tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_SLIDER;
		return;
	}
	if(ubisShowZoomflag)
	{
		SendIrCodeFlag = 0;
		ubdigitalZoomScaleFlage = 0;
		isShowFoucsRestart = 0;
		UI_SwitchViewType(DeskTopShowView,TRUE);
		ubisShowZoomflag = 0;

//		tUI_State = UI_DISPLAY_STATE;
//		MenuOnFlag = FALSE;
		UI_ClearOsdImageNoUpdate();
		UI_DrawDesktopIcon();

		//���Ϊ���佹���е�display���棬�����е��佹ѡ�����
		if(isSingleZoomCamera)
		{
			isSingleZoomCamera = FALSE;
			UI_SubKeyMenu(EXIT_ARROW);
		}
		else
		{
			UI_SubKeyMenu(ENTER_ARROW);
		}
		
//		if(ubUI_RestartRec && ubUI_FinishViewSwitch)//��������¼��
//		{
//			UI_VideoRecordingExec(UI_REC_START);
//			ubUI_RestartRec = FALSE;
//		}
		if(ubZoomRestartFlag)
		{
			ubZoomRestartFlag = FALSE;
			tUI_CuSetting.ubSD_Switch = 1;
		}
		
		UI_UpdateDevStatusInfo();
		
		return;
	}
}

void UI_EnterZoomSettingCamera()
{
	MenuOnFlag = TRUE;
	isShowFoucsRestart = 0;
	ubdigitalZoomScaleFlage = 0;
	ubisShowZoomflag = 1;
	SendIrCodeFlag = 1;
	uint8_t ubZoomItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	//����UI_ADJUST_ZOOM_AREA_STATE��UI_ADJUST_ZOOM_BSD_AREA_STATE״̬�Ż���ձ佹ҳ��ǰ������״̬��Relink
	if(tUI_State != UI_ADJUST_ZOOM_AREA_STATE && tUI_State != UI_ADJUST_ZOOM_BSD_AREA_STATE)
	{
		if(tUI_CuSetting.ubisZoomCameraFlag[ubZoomItemCurIdx] == 1)
		{
//			DeskTopShowView = (UI_CamNum_t)ubItemCurIdx;
			UI_SwitchViewType((UI_CamNum_t)ubZoomItemCurIdx,FALSE);
		}
		memset(&tZoomSettingCameraItem.tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	}
	UI_DrawZoomSettingCameraPage();
	tUI_State = UI_SET_ZOOM_CAMERA_STATE;
	return;
}




void UI_ZoomSettingCamera(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;
	uint8_t ubPreVolumeLvl = 10;

	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	
	if(tUI_CamStatus[uwZoomItemIdx].tCamConnSts == CAM_ONLINE && isShowFoucsRestart)
	{
		handle_zoom_key_restart_ops(tArrowKey);
		return;
	}

	if(tUI_CamStatus[uwZoomItemIdx].tCamConnSts == CAM_ONLINE || tArrowKey == EXIT_ARROW)
	{
		if(tArrowKey == ENTER_ARROW || !ubdigitalZoomScaleFlage)
		{
			tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tZoomSettingCameraItem);
			switch(tMenuAct)
			{
				case DRAW_HIGHLIGHT_MENUICON:
				{
					UI_ZoomDrawSettingCameraItem();
					break;
				}
				case DRAW_MENUPAGE:
				{
					UI_ZoomSettingCameraExecute();
					break;
				}
				case EXIT_MENUFUNC:
					UI_ZoomSettingCameraExit();
					break;
				default:
					break;
			}
		}
		else
		{
			switch(tArrowKey)
			{
				case LEFT_ARROW:
				{
					ubPreVolumeLvl = tUI_CamStatus[uwZoomItemIdx].digitalZoomScale;
					tUI_CamStatus[uwZoomItemIdx].digitalZoomScale--;
					if(tUI_CamStatus[uwZoomItemIdx].digitalZoomScale < 10)
						tUI_CamStatus[uwZoomItemIdx].digitalZoomScale = 10;
					updateDigitalZoomSlide(uwZoomItemIdx, ubPreVolumeLvl);
					break;
				}
				case RIGHT_ARROW:
				{
					ubPreVolumeLvl = tUI_CamStatus[uwZoomItemIdx].digitalZoomScale;
					tUI_CamStatus[uwZoomItemIdx].digitalZoomScale++;
					if(tUI_CamStatus[uwZoomItemIdx].digitalZoomScale > 50)
						tUI_CamStatus[uwZoomItemIdx].digitalZoomScale = 50;
					updateDigitalZoomSlide(uwZoomItemIdx, ubPreVolumeLvl);
					break;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------Zoom
#define ZOOM_CALIBRATE_ENHANCE 5

uint8_t handle_zoom_touch_ops(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo[4],tOsdImgInfo2[4],tOsdImgInfo3[2];
//	OSD_IMG_INFO tOsdImgInfo1[2];
	uint8_t touch_idx = 100;
	TWC_STATUS send_ret = TWC_FAIL;

	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_AUTO_NOR, 4, &tOsdImgInfo[0]);
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 2, &tOsdImgInfo1[0]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo2[0]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_CALIBRATE_NOR, 2, &tOsdImgInfo3[0]);

	for (uint8_t i = 0; i < 2; i++)
	{
		if (Touch_Info->startX > tOsdImgInfo[2*i].uwXStart && \
			Touch_Info->startX < tOsdImgInfo[2*i].uwXStart + tOsdImgInfo[2*i].uwHSize && \
			Touch_Info->startY > tOsdImgInfo[2*i].uwYStart && \
			Touch_Info->startY < tOsdImgInfo[2*i].uwYStart + tOsdImgInfo[2*i].uwVSize)
		{
			touch_idx = i;
			break;
		}
//		if (Touch_Info->startX > tOsdImgInfo1[0].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize && \
//			Touch_Info->startY > tOsdImgInfo1[0].uwYStart && \
//			Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
//		{
//			touch_idx = 2;
//			break;
//		}
		if (Touch_Info->startX > tOsdImgInfo2[0].uwXStart && \
			Touch_Info->startX < tOsdImgInfo2[0].uwXStart + tOsdImgInfo2[0].uwHSize && \
			Touch_Info->startY > tOsdImgInfo2[0].uwYStart && \
			Touch_Info->startY < tOsdImgInfo2[0].uwYStart + tOsdImgInfo2[0].uwVSize)
		{
			touch_idx = 2;
			tUI_CamStatus[uwZoomItemIdx].ubTrace = 1 - tUI_CamStatus[uwZoomItemIdx].ubTrace;
//			tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[uwZoomItemIdx].ubTrace*2 + 1], OSD_UPDATE);
			break;
		}
		if (Touch_Info->startX > tOsdImgInfo3[0].uwXStart && \
			Touch_Info->startX < tOsdImgInfo3[0].uwXStart + tOsdImgInfo3[0].uwHSize && \
			Touch_Info->startY > (tOsdImgInfo3[0].uwYStart + ZOOM_CALIBRATE_ENHANCE) && \
			Touch_Info->startY < (tOsdImgInfo3[0].uwYStart + tOsdImgInfo3[0].uwVSize + 2*ZOOM_CALIBRATE_ENHANCE))
		{
			touch_idx = 3;
			tOSD_Img2(&tOsdImgInfo3[1], OSD_UPDATE);
			break;
		}
		if (Touch_Info->startX > tOsdImgInfo2[0].uwXStart && \
			Touch_Info->startX < tOsdImgInfo2[0].uwXStart + tOsdImgInfo2[0].uwHSize && \
			Touch_Info->startY > BSD_OFF_NOR_DIST && \
			Touch_Info->startY < BSD_OFF_NOR_DIST + tOsdImgInfo2[0].uwVSize)
		{
			touch_idx = 4;
			tUI_CamStatus[uwZoomItemIdx].ubBSD = 1 - tUI_CamStatus[uwZoomItemIdx].ubBSD;
//			tOsdImgInfo2[tUI_CamStatus[uwZoomItemIdx].ubBSD*2 + 1].uwYStart = BSD_OFF_NOR_DIST;
//			tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[uwZoomItemIdx].ubBSD*2 + 1], OSD_UPDATE);
			break;
		}
		if (Touch_Info->startX > tOsdImgInfo3[0].uwXStart && \
			Touch_Info->startX < tOsdImgInfo3[0].uwXStart + tOsdImgInfo3[0].uwHSize && \
			Touch_Info->startY > (BSD_OFF_NOR_DIST + ZOOM_CALIBRATE_ENHANCE) && \
			Touch_Info->startY < (BSD_OFF_NOR_DIST + tOsdImgInfo3[0].uwVSize + 2*ZOOM_CALIBRATE_ENHANCE))
		{
			touch_idx = 5;
			tOsdImgInfo3[1].uwYStart = BSD_OFF_NOR_DIST;
			tOSD_Img2(&tOsdImgInfo3[1], OSD_UPDATE);
			break;
		}
	}


	if (touch_idx != 100)
	{
		switch (touch_idx)
		{
			case 0: // auto focus
			{
				//printf("auto button\n");
				send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_auto[0], 7, 8);
				osDelay(100);
				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_AUTO;
				return 1;
			}
			case 1: // restart device
			{
				//printf("restart button\n");
				draw_restart_cam_box();
				isShowFoucsRestart = 1;
				//osDelay(100);
				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_RESTART;
				return 1;
			}
//			case 2:
//			{
//				SendIrCodeFlag = 0;
//				ubdigitalZoomScaleFlage = 0;
//				if(ubZoomRestartFlag)
//				{
//					ubZoomRestartFlag = FALSE;
//					tUI_CuSetting.ubSD_Switch = 1;
//				}
//				tOSD_Img2(&tOsdImgInfo1[1], OSD_UPDATE);
//				TIMER_Delay_ms(60);
//				//UI_ZoomSettingCamera(EXIT_ARROW);
//				UI_MenuKey();
//				return 1;
//			}
			case 2:
			{
				tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_Trace[0 + 7*tUI_CamStatus[uwZoomItemIdx].ubTrace], 7, 10);
				//tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubTrace*2], OSD_UPDATE);
				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_TRCAE;
				return 1;
			}
			case 3:
			{
				UI_EnterAdjustZoomArea();
				return 0;
			}
			case 4:
			{
				tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_BSD[0 + 7*tUI_CamStatus[uwZoomItemIdx].ubBSD], 7, 10);
				tOsdImgInfo2[tUI_CamStatus[uwZoomItemIdx].ubBSD*2].uwYStart = BSD_OFF_NOR_DIST;
				//tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubBSD*2], OSD_UPDATE);
				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_BSD;
				return 1;
			}
			case 5:
			{
				UI_EnterAdjustZoomBSDArea();
				return 0;
			}
			default:
			{
				return 0;
			}
		}
	}
	return 0;
}

uint8_t handle_zoom_touch_restart_ops(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo[5];
	uint8_t touch_idx = 100;
	TWC_STATUS send_ret = TWC_FAIL;

	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	tOSD_GetOsdImgInfor(1, OSD_IMG2,(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_FORMAT_BG:
								(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_FORMAT_BG_GER:
								(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_FORMAT_BG_FR:OSD2IMG_SDCARD_FORMAT_BG_CHN, 5, &tOsdImgInfo[0]);
	for (uint8_t i = 0; i < 2; i++)
	{
		if (Touch_Info->startX > tOsdImgInfo[2*i+1].uwXStart && \
			Touch_Info->startX < tOsdImgInfo[2*i+1].uwXStart + tOsdImgInfo[2*i+1].uwHSize && \
			Touch_Info->startY > tOsdImgInfo[2*i+1].uwYStart && \
			Touch_Info->startY < tOsdImgInfo[2*i+1].uwYStart + tOsdImgInfo[2*i+1].uwVSize)
		{
			touch_idx = i;
			break;
		}
	}

	if (touch_idx != 100)
	{
		switch (touch_idx)
		{
			case 0: // auto focus
			{
				//printf("restart button\n");
				if(tUI_CuSetting.ubSD_Switch)
				{
					ubZoomRestartFlag = TRUE;
					tUI_CuSetting.ubSD_Switch = 0;
					UI_SwitchViewType((UI_CamNum_t)uwZoomItemIdx,FALSE);
				}
				send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_restart[0], 7, 8);
				osDelay(100);
				tUI_CamStatus[uwZoomItemIdx].digitalZoomScale = 10;
				isShowFoucsRestart = 0;
				UI_ClearOsdImageNoUpdate();
				UI_DrawZoomButton(1);
				return 1;
			}
			case 1: // restart device
			{
				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
				tOSD_Img2(&tOsdImgInfo[4], OSD_UPDATE);
				isShowFoucsRestart = 0;
				UI_ClearOsdImageNoUpdate();
				UI_DrawZoomButton(1);
				return 1;
			}
			default:
			{
				return 0;
			}
		}
	}
	return 0;
}

void sendDigitalZoomScale(uint8_t val)
{
	uint16_t sum;
	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	sum = 0x92 + val;
	pelco_digital_zoom[3] = val;
	pelco_digital_zoom[6] = (uint8_t)(sum&0x00FF);
	tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_digital_zoom[0], 7, 1);
}


void updateDigitalZoomSlide(UI_CamNum_t chn, uint8_t ubPreVolumeLvl)
{
	
	OSD_IMG_INFO tOsdImgInfo[5];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_SLIDE, 3, &tOsdImgInfo[0]);


	tOsdImgInfo[2].uwXStart = UI_Map(ubPreVolumeLvl, 10, 50,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
	tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	
	tOsdImgInfo[1].uwXStart = UI_Map(tUI_CamStatus[chn].digitalZoomScale, 10, 50,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
	tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);

	OSD_ImagePrintf(OSD_IMG_ROTATION_0, 500, 480, tUI_TransparentCharOsdImgInfo, OSD_UPDATE, "%d.%d", tUI_CamStatus[chn].digitalZoomScale/10, tUI_CamStatus[chn].digitalZoomScale%10);

	if (ulDigitalZoomCount == 0)
	{
		ulDigitalZoomCount = 300;
		sendDigitalZoomScale(tUI_CamStatus[chn].digitalZoomScale);
	}
}


void UI_ZoomSettingCameraTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	TWC_STATUS send_ret = TWC_FAIL;
	OSD_IMG_INFO tOsdImgInfo[12],tOsdImgInfo1[2],tOsdImgInfo2;
	static uint8_t touch_repaet_cnt = 0; // 
	static uint8_t isZoomExecute = 0, isZoomSlideChange = 0, isZoomExecuteNearOrFar = 0;
	uint8_t ubPreVolumeLvl = 10;
	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_IN_NOR, 12, &tOsdImgInfo[0]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 2, &tOsdImgInfo1[0]);
	//ubdigitalZoomScaleFlage = 0;

	tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
	
	switch(Touch_Info->Gesture)
	{
        case TOUCH_PRESSDOWN: //
        	if(tUI_CamStatus[uwZoomItemIdx].tCamConnSts == CAM_ONLINE)
        	{
				if (isShowFoucsRestart==1)
				{
					break;
				}


				//���Ƴ���ʱ�������Ŵ���С������ť�⣬û��stopָ���
				if(isZoomExecute == 1 && \
					!((Touch_Info->endX > tOsdImgInfo[0].uwXStart && \
					Touch_Info->endX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
					Touch_Info->endY > tOsdImgInfo[0].uwYStart && \
					Touch_Info->endY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize) || 
					(Touch_Info->endX > tOsdImgInfo[2].uwXStart && \
					Touch_Info->endX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
					Touch_Info->endY > tOsdImgInfo[2].uwYStart && \
					Touch_Info->endY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize)))
				{
					send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
					printf("TOUCH_PRESSDOWN stop\n");
					isZoomExecute = 0;
					UI_ZoomDrawSettingCameraItem();
					break;
				}

				//���Ƴ���ʱ����������仯��ť�⣬��ˢͼ
				if(isZoomExecuteNearOrFar == 1 && \
					!((Touch_Info->endX > tOsdImgInfo[4].uwXStart && \
					Touch_Info->endX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
					Touch_Info->endY > tOsdImgInfo[4].uwYStart && \
					Touch_Info->endY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize) || \
					(Touch_Info->endX > tOsdImgInfo[6].uwXStart && \
					Touch_Info->endX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
					Touch_Info->endY > tOsdImgInfo[6].uwYStart && \
					Touch_Info->endY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize)))
				{
					isZoomExecuteNearOrFar = 0;
					UI_ZoomDrawSettingCameraItem();
					break;
				}
				
				if (Touch_Info->endX > tOsdImgInfo[0].uwXStart && \
					Touch_Info->endX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
					Touch_Info->endY > tOsdImgInfo[0].uwYStart && \
					Touch_Info->endY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize)
				{
					if (isZoomExecute == 0)
					{
						touch_repaet_cnt = 25;
					}
					else
					{
						touch_repaet_cnt++;
					}

		        	if (touch_repaet_cnt >= 25)
					{
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
						isZoomExecute = 1;
						//tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
						if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx != ZOOM_IN)
						{
							tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
							tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_IN;
							UI_ZoomDrawSettingCameraItem();
						}
						UI_ZoomTouchHL(ZOOM_IN);
						touch_repaet_cnt = 0;
					}
				}
				else if (Touch_Info->endX > tOsdImgInfo[2].uwXStart && \
						 Touch_Info->endX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
						 Touch_Info->endY > tOsdImgInfo[2].uwYStart && \
						 Touch_Info->endY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize)
				{
					if (isZoomExecute == 0)
					{
						touch_repaet_cnt = 25;
					}
					else
					{
						touch_repaet_cnt++;
					}

		        	if (touch_repaet_cnt >= 25)
					{
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
						isZoomExecute = 1;
						if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx != ZOOM_OUT)
						{
							tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
							tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_OUT;
							UI_ZoomDrawSettingCameraItem();
						}
//						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_OUT;
						UI_ZoomTouchHL(ZOOM_OUT);
						touch_repaet_cnt = 0;
					}
				}
				else if (Touch_Info->endX > tOsdImgInfo[4].uwXStart && \
						 Touch_Info->endX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
						 Touch_Info->endY > tOsdImgInfo[4].uwYStart && \
						 Touch_Info->endY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize)
				{
					if (isZoomExecuteNearOrFar == 0)
					{
						touch_repaet_cnt = 25;
					}
					else
					{
						touch_repaet_cnt++;
					}

		        	if (touch_repaet_cnt >= 25)
					{
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_near[0], 7, 8);
						isZoomExecuteNearOrFar = 1;
						if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx != ZOOM_NEAR)
						{
							tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
							tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_NEAR;
							UI_ZoomDrawSettingCameraItem();
						}
//						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_NEAR;
//						UI_ZoomDrawSettingCameraItem();
						UI_ZoomTouchHL(ZOOM_NEAR);
						touch_repaet_cnt = 0;
					}
				}
				else if (Touch_Info->endX > tOsdImgInfo[6].uwXStart && \
						 Touch_Info->endX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
						 Touch_Info->endY > tOsdImgInfo[6].uwYStart && \
						 Touch_Info->endY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize)
				{
					if (isZoomExecuteNearOrFar == 0)
					{
						touch_repaet_cnt = 25;
					}
					else
					{
						touch_repaet_cnt++;
					}

		        	if (touch_repaet_cnt >= 25)
					{
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_far[0], 7, 8);
						isZoomExecuteNearOrFar = 1;
						//tOSD_Img2(&tOsdImgInfo[7], OSD_UPDATE);
						if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx != ZOOM_FAR)
						{
							tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
							tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_FAR;
							UI_ZoomDrawSettingCameraItem();
						}
//						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_FAR;
//						UI_ZoomDrawSettingCameraItem();
						UI_ZoomTouchHL(ZOOM_FAR);
						touch_repaet_cnt = 0;
					}
				}
				else
				{
					OSD_IMG_INFO tOsdImgInfo1[3];
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_SLIDE, 3, &tOsdImgInfo1[0]);
					if(Touch_Info->endX >= tOsdImgInfo1[0].uwXStart - 10 
						&& Touch_Info->endX <= tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize + 10
						&& Touch_Info->endY >= tOsdImgInfo1[0].uwYStart - 40
						&& Touch_Info->endY <= tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize + 40)
					{
						if(Touch_Info->endX < tOsdImgInfo1[0].uwXStart)
							Touch_Info->endX = tOsdImgInfo1[0].uwXStart;

						if(Touch_Info->endX > tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize)
							Touch_Info->endX = tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize;

						//
						ubPreVolumeLvl = tUI_CamStatus[uwZoomItemIdx].digitalZoomScale;
							uint8_t ubValue = (uint8_t)UI_Map(Touch_Info->endX, \
															  tOsdImgInfo1[0].uwXStart, tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize, \
															  10, 50);

						tUI_CamStatus[uwZoomItemIdx].digitalZoomScale = ubValue;
						//printf("new digital zoom scale value = %d\n", ubValue);

						if(tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx != ZOOM_SLIDER)
						{
							tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
							tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_SLIDER;
							UI_ZoomDrawSettingCameraItem();
						}
						
						updateDigitalZoomSlide(uwZoomItemIdx, ubPreVolumeLvl);
						isZoomSlideChange = 1;
					}
				}
        	}
			break;
		case TOUCH_PRESS:
			touch_repaet_cnt = 0;
			if(Touch_Info->startX > tOsdImgInfo1[0].uwXStart 
				&& Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize
				&& Touch_Info->startY > tOsdImgInfo1[0].uwYStart 
				&& Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
			{
//				SendIrCodeFlag = 0;
//				ubdigitalZoomScaleFlage = 0;
//				if(ubZoomRestartFlag)
//				{
//					ubZoomRestartFlag = FALSE;
//					tUI_CuSetting.ubSD_Switch = 1;
//				}
				tOSD_Img2(&tOsdImgInfo1[1], OSD_UPDATE);
				TIMER_Delay_ms(60);
				//UI_ZoomSettingCamera(EXIT_ARROW);
				UI_MenuKey();
			}
			else if(tUI_CamStatus[uwZoomItemIdx].tCamConnSts == CAM_ONLINE)
			{
				if(isShowFoucsRestart==1){
					if(handle_zoom_touch_restart_ops(Touch_Info)==1)
					{
						break;
					}
					return;
				}
				if (isZoomExecute == 1 || isZoomExecuteNearOrFar == 1 || (\
											((Touch_Info->startX > tOsdImgInfo[0].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[0].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize) ||\
											(Touch_Info->startX > tOsdImgInfo[2].uwXStart &&\
											Touch_Info->startX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[2].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize) ||\
											(Touch_Info->startX > tOsdImgInfo[4].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[4].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize) ||\
											(Touch_Info->startX > tOsdImgInfo[6].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[6].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize))))
				{
	                if (isZoomExecute == 1 && ((Touch_Info->startX > tOsdImgInfo[0].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[0].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize) ||\
											(Touch_Info->startX > tOsdImgInfo[2].uwXStart &&\
											Touch_Info->startX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[2].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize)))
	                {
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
						UI_ZoomDrawSettingCameraItem();
						isZoomExecute = 0;
					}
					else if(isZoomExecuteNearOrFar == 1 && ((Touch_Info->startX > tOsdImgInfo[4].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[4].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize) ||\
											(Touch_Info->startX > tOsdImgInfo[6].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[6].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize)))
					{
						UI_ZoomDrawSettingCameraItem();
						isZoomExecuteNearOrFar = 0;
					}
					else if((Touch_Info->startX > tOsdImgInfo[0].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[0].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize))
					{
						//tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
						UI_ZoomTouchHL(ZOOM_IN);
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_IN;
					}
					else if((Touch_Info->startX > tOsdImgInfo[2].uwXStart &&\
											Touch_Info->startX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[2].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize))
					{
						//tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
						UI_ZoomTouchHL(ZOOM_OUT);
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_OUT;
					}
					else if((Touch_Info->startX > tOsdImgInfo[4].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[4].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize))
					{
						//tOSD_Img2(&tOsdImgInfo[5], OSD_UPDATE);
						UI_ZoomTouchHL(ZOOM_NEAR);
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_near[0], 7, 8);
						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_NEAR;
					}
					else if((Touch_Info->startX > tOsdImgInfo[6].uwXStart && \
											Touch_Info->startX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
											Touch_Info->startY > tOsdImgInfo[6].uwYStart && \
											Touch_Info->startY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize))
					{
						//tOSD_Img2(&tOsdImgInfo[7], OSD_UPDATE);
						UI_ZoomTouchHL(ZOOM_FAR);
						send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_far[0], 7, 8);
						tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_FAR;
					}
					UI_ZoomDrawSettingCameraItem();
					break;
				}
				else if (uwZoomItemIdx <= CAM4VIEW_ITEM)
				{
					if (handle_zoom_touch_ops(Touch_Info) == 1)
					{
						UI_ZoomDrawSettingCameraItem();
						break;
					}
					else if (isZoomSlideChange == 1)
					{
						isZoomSlideChange = 0;
						sendDigitalZoomScale(tUI_CamStatus[uwZoomItemIdx].digitalZoomScale);
						UI_UpdateDevStatusInfo();
					}
				}
			}
			break;
		default :
			touch_repaet_cnt = 0;
			if(tUI_CamStatus[uwZoomItemIdx].tCamConnSts == CAM_ONLINE)
			{
				if (isZoomExecute == 1)
				{
					send_ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
					isZoomExecute = 0;
					UI_ZoomDrawSettingCameraItem();
					break;
				}
				else if(isZoomExecuteNearOrFar == 1)
				{
					isZoomExecuteNearOrFar = 0;
					UI_ZoomDrawSettingCameraItem();
				}
				else if (isZoomSlideChange == 1)
				{
					isZoomSlideChange = 0;
					sendDigitalZoomScale(tUI_CamStatus[uwZoomItemIdx].digitalZoomScale);
					UI_UpdateDevStatusInfo();
					break;
				}
			}
			break;
	}
}


//------------------------------------------------------------------------------
uint8_t AIareaResolutionPakt[]       = {0xFF,0x40, 0x04, 0x00, 0x02, 0x58, 0x9E};
uint8_t AIareaLeftTopCoordPakt[]     = {0xFF,0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t AIareaRightBottomCoordPakt[] = {0xFF,0x81, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t AlarmAreaPakt[]                      = {0xFF,0x45, 0x04, 0x00, 0x02, 0x58, 0xA3};
uint8_t AlarmAreaBigLeftTopCoordPakt[]       = {0xFF,0x82, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t AlarmAreaBigRightBottomCoordPakt[]   = {0xFF,0x83, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t AlarmAreaSmallLeftTopCoordPakt[]     = {0xFF,0x84, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t AlarmAreaSmallRightBottomCoordPakt[] = {0xFF,0x85, 0x00, 0x00, 0x00, 0x00, 0x00};


uint8_t ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM;

uint8_t ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM;

uint8_t IsZoomTracePressDownVirtualKey = TRUE;

uint8_t IsZoomBSDPressDownVirtualKey = TRUE;

uint8_t sentAIareaCoord(void)
{
	uint8_t x1, y1, x2, y2, checksum, ret;
	uint16_t sum;
	uint16_t usTempZoomRedbox[4] = {0};
	uint16_t tmpx1,tmpx2,tmpy1,tmpy2;
	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	

	if (uwZoomItemIdx > CAM4)
	{
		return TWC_FAIL;
	}
	
	for(int i = 0; i < 4; i++)
	{
		usTempZoomRedbox[i] = tUI_CuSetting.usZoomRedbox[uwZoomItemIdx][i];
	}

//	if(!UI_IsZoomNewVersion(uwZoomItemIdx))
	if(!tUI_CamStatus[uwZoomItemIdx].tZoomCamISP && !UI_IsZoomNewVersion(uwZoomItemIdx))
	{
		if(tUI_CamStatus[uwZoomItemIdx].tCamImgFlip) //V_Mirror
		{
			usTempZoomRedbox[1] = usTempZoomRedbox[1] + (300 - usTempZoomRedbox[1])*2;
			tmpy1 = usTempZoomRedbox[1];
			usTempZoomRedbox[3] = usTempZoomRedbox[3] + (300 - usTempZoomRedbox[3])*2;
			tmpy2 = usTempZoomRedbox[3];
			usTempZoomRedbox[1] = tmpy2;
			usTempZoomRedbox[3] = tmpy1;
		}
		if(tUI_CamStatus[uwZoomItemIdx].tCamImgMirror) //H_Mirror
		{
			usTempZoomRedbox[0] = usTempZoomRedbox[0] + (512 - usTempZoomRedbox[0])*2;
			tmpx1 = usTempZoomRedbox[0];
			usTempZoomRedbox[2] = usTempZoomRedbox[2] + (512 - usTempZoomRedbox[2])*2;
			tmpx2 = usTempZoomRedbox[2];
			usTempZoomRedbox[0] = tmpx2;
			usTempZoomRedbox[2] = tmpx1;
		}
	}


	ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AIareaResolutionPakt[0], 7, 5);

	x1 = (uint8_t)((usTempZoomRedbox[0] & 0xFF00) >> 8);
	x2 = (uint8_t)((usTempZoomRedbox[0] & 0x00FF) >> 0);
	y1 = (uint8_t)((usTempZoomRedbox[1] & 0xFF00) >> 8);
	y2 = (uint8_t)((usTempZoomRedbox[1] & 0x00FF) >> 0);
	sum = 0x80 + x1 + x2 + y1 + y2;
	AIareaLeftTopCoordPakt[2] = x1;
	AIareaLeftTopCoordPakt[3] = x2;
	AIareaLeftTopCoordPakt[4] = y1;
	AIareaLeftTopCoordPakt[5] = y2;
	AIareaLeftTopCoordPakt[6] = (uint8_t)(sum&0x00FF);
	ret += tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AIareaLeftTopCoordPakt[0], 7, 5);

	x1 = (uint8_t)((usTempZoomRedbox[2] & 0xFF00) >> 8);
	x2 = (uint8_t)((usTempZoomRedbox[2] & 0x00FF) >> 0);
	y1 = (uint8_t)((usTempZoomRedbox[3] & 0xFF00) >> 8);
	y2 = (uint8_t)((usTempZoomRedbox[3] & 0x00FF) >> 0);
	sum = 0x81 + x1 + x2 + y1 + y2;
	AIareaRightBottomCoordPakt[2] = x1;
	AIareaRightBottomCoordPakt[3] = x2;
	AIareaRightBottomCoordPakt[4] = y1;
	AIareaRightBottomCoordPakt[5] = y2;
	AIareaRightBottomCoordPakt[6] = (uint8_t)(sum&0x00FF);
	ret += tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AIareaRightBottomCoordPakt[0], 7, 5);

	return ret;
}



uint8_t sentBSDAIareaCoord(void)
{
	uint8_t x1, y1, x2, y2, checksum, ret;
	uint16_t sum;
	uint16_t usTempZoomBSDSmallRedbox[4] = {0};
	uint16_t usTempZoomBSDBigRedbox[4] = {0};
	uint16_t tmpx1,tmpx2,tmpy1,tmpy2;
	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	if (uwZoomItemIdx > CAM4)
	{
		return TWC_FAIL;
	}
	
	for(int i = 0; i < 4; i++)
	{
		usTempZoomBSDSmallRedbox[i] = tUI_CuSetting.usZoomBSDSmallRedbox[uwZoomItemIdx][i];
		usTempZoomBSDBigRedbox[i] = tUI_CuSetting.usZoomBSDBigRedbox[uwZoomItemIdx][i];
	}

//	if(!UI_IsZoomNewVersion(uwZoomItemIdx))
	if(!tUI_CamStatus[uwZoomItemIdx].tZoomCamISP && !UI_IsZoomNewVersion(uwZoomItemIdx))
	{
		if(tUI_CamStatus[uwZoomItemIdx].tCamImgFlip) //V_Mirror
		{
			usTempZoomBSDSmallRedbox[1] = usTempZoomBSDSmallRedbox[1] + (300 - usTempZoomBSDSmallRedbox[1])*2;
			tmpy1 = usTempZoomBSDSmallRedbox[1];
			usTempZoomBSDSmallRedbox[3] = usTempZoomBSDSmallRedbox[3] + (300 - usTempZoomBSDSmallRedbox[3])*2;
			tmpy2 = usTempZoomBSDSmallRedbox[3];
			usTempZoomBSDSmallRedbox[1] = tmpy2;
			usTempZoomBSDSmallRedbox[3] = tmpy1;
			
			usTempZoomBSDBigRedbox[1] = usTempZoomBSDBigRedbox[1] + (300 - usTempZoomBSDBigRedbox[1])*2;
			tmpy1 = usTempZoomBSDBigRedbox[1];
			usTempZoomBSDBigRedbox[3] = usTempZoomBSDBigRedbox[3] + (300 - usTempZoomBSDBigRedbox[3])*2;
			tmpy2 = usTempZoomBSDBigRedbox[3];
			usTempZoomBSDBigRedbox[1] = tmpy2;
			usTempZoomBSDBigRedbox[3] = tmpy1;
		}
		if(tUI_CamStatus[uwZoomItemIdx].tCamImgMirror) //H_Mirror
		{
			usTempZoomBSDSmallRedbox[0] = usTempZoomBSDSmallRedbox[0] + (512 - usTempZoomBSDSmallRedbox[0])*2;
			tmpx1 = usTempZoomBSDSmallRedbox[0];
			usTempZoomBSDSmallRedbox[2] = usTempZoomBSDSmallRedbox[2] + (512 - usTempZoomBSDSmallRedbox[2])*2;
			tmpx2 = usTempZoomBSDSmallRedbox[2];
			usTempZoomBSDSmallRedbox[0] = tmpx2;
			usTempZoomBSDSmallRedbox[2] = tmpx1;

			usTempZoomBSDBigRedbox[0] = usTempZoomBSDBigRedbox[0] + (512 - usTempZoomBSDBigRedbox[0])*2;
			tmpx1 = usTempZoomBSDBigRedbox[0];
			usTempZoomBSDBigRedbox[2] = usTempZoomBSDBigRedbox[2] + (512 - usTempZoomBSDBigRedbox[2])*2;
			tmpx2 = usTempZoomBSDBigRedbox[2];
			usTempZoomBSDBigRedbox[0] = tmpx2;
			usTempZoomBSDBigRedbox[2] = tmpx1;
		}
	}


	ret = tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AlarmAreaPakt[0], 7, 5);

	printf("usTempZoomRedbox[0] = 0x%x, [1] = 0x%x, [2] = 0x%x, [3] = 0x%x\n",\
		usTempZoomBSDBigRedbox[0],usTempZoomBSDBigRedbox[1],usTempZoomBSDBigRedbox[2],usTempZoomBSDBigRedbox[3]);

	x1 = (uint8_t)((usTempZoomBSDBigRedbox[0] & 0xFF00) >> 8);
	x2 = (uint8_t)((usTempZoomBSDBigRedbox[0] & 0x00FF) >> 0);
	y1 = (uint8_t)((usTempZoomBSDBigRedbox[1] & 0xFF00) >> 8);
	y2 = (uint8_t)((usTempZoomBSDBigRedbox[1] & 0x00FF) >> 0);
	sum = 0x82 + x1 + x2 + y1 + y2;
	AlarmAreaBigLeftTopCoordPakt[2] = x1;
	AlarmAreaBigLeftTopCoordPakt[3] = x2;
	AlarmAreaBigLeftTopCoordPakt[4] = y1;
	AlarmAreaBigLeftTopCoordPakt[5] = y2;
	AlarmAreaBigLeftTopCoordPakt[6] = (uint8_t)(sum&0x00FF);
	ret += tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AlarmAreaBigLeftTopCoordPakt[0], 7, 5);

	x1 = (uint8_t)((usTempZoomBSDBigRedbox[2] & 0xFF00) >> 8);
	x2 = (uint8_t)((usTempZoomBSDBigRedbox[2] & 0x00FF) >> 0);
	y1 = (uint8_t)((usTempZoomBSDBigRedbox[3] & 0xFF00) >> 8);
	y2 = (uint8_t)((usTempZoomBSDBigRedbox[3] & 0x00FF) >> 0);
	sum = 0x83 + x1 + x2 + y1 + y2;
	AlarmAreaBigRightBottomCoordPakt[2] = x1;
	AlarmAreaBigRightBottomCoordPakt[3] = x2;
	AlarmAreaBigRightBottomCoordPakt[4] = y1;
	AlarmAreaBigRightBottomCoordPakt[5] = y2;
	AlarmAreaBigRightBottomCoordPakt[6] = (uint8_t)(sum&0x00FF);
	ret += tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AlarmAreaBigRightBottomCoordPakt[0], 7, 5);

	
	printf("usTempZoomRedbox[0] = 0x%x, [1] = 0x%x, [2] = 0x%x, [3] = 0x%x\n",\
		usTempZoomBSDSmallRedbox[0],usTempZoomBSDSmallRedbox[1],usTempZoomBSDSmallRedbox[2],usTempZoomBSDSmallRedbox[3]);
	x1 = (uint8_t)((usTempZoomBSDSmallRedbox[0] & 0xFF00) >> 8);
	x2 = (uint8_t)((usTempZoomBSDSmallRedbox[0] & 0x00FF) >> 0);
	y1 = (uint8_t)((usTempZoomBSDSmallRedbox[1] & 0xFF00) >> 8);
	y2 = (uint8_t)((usTempZoomBSDSmallRedbox[1] & 0x00FF) >> 0);
	sum = 0x84 + x1 + x2 + y1 + y2;
	AlarmAreaSmallLeftTopCoordPakt[2] = x1;
	AlarmAreaSmallLeftTopCoordPakt[3] = x2;
	AlarmAreaSmallLeftTopCoordPakt[4] = y1;
	AlarmAreaSmallLeftTopCoordPakt[5] = y2;
	AlarmAreaSmallLeftTopCoordPakt[6] = (uint8_t)(sum&0x00FF);
	ret += tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AlarmAreaSmallLeftTopCoordPakt[0], 7, 5);

	x1 = (uint8_t)((usTempZoomBSDSmallRedbox[2] & 0xFF00) >> 8);
	x2 = (uint8_t)((usTempZoomBSDSmallRedbox[2] & 0x00FF) >> 0);
	y1 = (uint8_t)((usTempZoomBSDSmallRedbox[3] & 0xFF00) >> 8);
	y2 = (uint8_t)((usTempZoomBSDSmallRedbox[3] & 0x00FF) >> 0);
	sum = 0x85 + x1 + x2 + y1 + y2;
	AlarmAreaSmallRightBottomCoordPakt[2] = x1;
	AlarmAreaSmallRightBottomCoordPakt[3] = x2;
	AlarmAreaSmallRightBottomCoordPakt[4] = y1;
	AlarmAreaSmallRightBottomCoordPakt[5] = y2;
	AlarmAreaSmallRightBottomCoordPakt[6] = (uint8_t)(sum&0x00FF);
	ret += tTWC_Send(TWC_STA1 + uwZoomItemIdx, TWC_PELCO, (uint8_t *)&AlarmAreaSmallRightBottomCoordPakt[0], 7, 5);
	
	return ret;
}



static void AdjustZoomTraceAreaReset(void)
{
	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	tUI_CuSetting.usZoomRedbox[uwZoomItemIdx][0] = 412;
	tUI_CuSetting.usZoomRedbox[uwZoomItemIdx][1] = 200;
	tUI_CuSetting.usZoomRedbox[uwZoomItemIdx][2] = 612;
	tUI_CuSetting.usZoomRedbox[uwZoomItemIdx][3] = 400;

	ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM;
}

void UI_AdjustZoomVirtualItem(void)
{
	OSD_IMG_INFO tOsdImgInfo[2];
	uint8_t ubItemPreIdx = tZoomVirtualKeyItem.tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemCurIdx = tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx;

	//----------------------------pre----------------------------------------------
	if(ubItemPreIdx != ubItemCurIdx)
	{
		switch(ubItemPreIdx)
		{
			case ZOOM_VIRTUAL_LEFT_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_LEFT_KEY_NOR, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_UP_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_UP_KEY_NOR, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_RIGHT_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_RIGHT_KEY_NOR, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_DOWN_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_DOWN_KEY_NOR, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_OK_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_OK_KEY_NOR, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_RESET_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_RESTART_KEY_NOR, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			
		}
		switch(ubItemCurIdx)
		{
			case ZOOM_VIRTUAL_LEFT_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_LEFT_KEY_HL, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_UP_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_UP_KEY_HL, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_RIGHT_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_RIGHT_KEY_HL, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_DOWN_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_DOWN_KEY_HL, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_OK_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_OK_KEY_HL, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			case ZOOM_VIRTUAL_RESET_KEY:
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_RESTART_KEY_HL, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
				break;
			}
			
		}
	}
}

static void AdjustZoomTraceArea(UI_ZoomVirtualKeyItemList_t ZoomVirtualKey)
{
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t tempSize = 0;

	switch(ZoomTraceBoxMoveSelect)
	{
		case ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM:
			printf("AdjustZoomTraceArea ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM\n");
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][0]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][1]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						//printf("tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = %d, tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = %d", tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]);
						tUI_CuSetting.usZoomRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] = tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] = tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_UP_ITEM:
			printf("AdjustZoomTraceArea ZOOM_BSD_AI_AREA_SMALL_UP_ITEM\n");
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] = tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][1]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2] + BSD_BOX_MOVE_SIZE) <= OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] = tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM:
			printf("AdjustZoomTraceArea ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM\n");
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] = tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] = tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2] + BSD_BOX_MOVE_SIZE) <= OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) <= OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM:
			printf("AdjustZoomTraceArea ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM\n");
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][0]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] = tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						//printf("tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = %d, tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = %d", tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]);
						tUI_CuSetting.usZoomRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] = tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) <= OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM:
			printf("AdjustZoomTraceArea ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM\n");
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][0]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = tUI_CuSetting.usZoomRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomRedbox[CAM][2] -= tempSize;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][1]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = tUI_CuSetting.usZoomRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomRedbox[CAM][3] -= tempSize;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][2]+BSD_BOX_MOVE_SIZE) <= OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = OSD_WIDTH  - ZOOM_BOX_DIST_LIMIT - tUI_CuSetting.usZoomRedbox[CAM][2];
						tUI_CuSetting.usZoomRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomRedbox[CAM][0] += tempSize;
					}
					break;	
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) <= OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT - tUI_CuSetting.usZoomRedbox[CAM][3];
						tUI_CuSetting.usZoomRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomRedbox[CAM][1] += tempSize;
					}
					break;
			}	
			break;
	}
}


void UI_AdjustZoomTraceExecute(void)
{
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	OSD_IMG_INFO tOsdImgInfo;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo);
	tOsdImgInfo.uwXStart = 0;
	tOsdImgInfo.uwYStart = 0;
	//����ȫ��
	tOsdImgInfo.uwHSize = OSD_WIDTH;
	tOsdImgInfo.uwVSize = OSD_HEIGHT;
	
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo);

//	printf("tUI_CuSetting.usZoomRedbox[0] = %d, [1] = %d, [2] = %d, [3] = %d\n",\
//		tUI_CuSetting.usZoomRedbox[CAM][0], tUI_CuSetting.usZoomRedbox[CAM][1],\
//		tUI_CuSetting.usZoomRedbox[CAM][2], tUI_CuSetting.usZoomRedbox[CAM][3]);

	//��ѡ��
	if(ZoomTraceBoxMoveSelect)
	{
		switch(tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx)
		{
			case ZOOM_VIRTUAL_LEFT_KEY:
				AdjustZoomTraceArea(ZOOM_VIRTUAL_LEFT_KEY);
				break;
			case ZOOM_VIRTUAL_UP_KEY:
				AdjustZoomTraceArea(ZOOM_VIRTUAL_UP_KEY);
				break;
			case ZOOM_VIRTUAL_RIGHT_KEY:
				AdjustZoomTraceArea(ZOOM_VIRTUAL_RIGHT_KEY);
				break;
			case ZOOM_VIRTUAL_DOWN_KEY:
				AdjustZoomTraceArea(ZOOM_VIRTUAL_DOWN_KEY);
				break;
			case ZOOM_VIRTUAL_OK_KEY:
				ZoomTraceBoxMoveSelect++;
				if(ZoomTraceBoxMoveSelect == ZOOM_BSD_AI_AREA_ITEM_MAX)
				{
					ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM;
				}
				break;
			case ZOOM_VIRTUAL_RESET_KEY:
				AdjustZoomTraceAreaReset();
				break;
		}
	}
	
	UI_DrawRedBox(tUI_CuSetting.usZoomRedbox[CAM][0], tUI_CuSetting.usZoomRedbox[CAM][1], \
		          tUI_CuSetting.usZoomRedbox[CAM][2], tUI_CuSetting.usZoomRedbox[CAM][3]);

	//��ʾ���ⰴ��
	UI_ZoomDrawVirtualKey();
}


void UI_AdjustZoomTraceArea(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tZoomVirtualKeyItem);
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_AdjustZoomVirtualItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_AdjustZoomTraceExecute();
			break;
		}
		case EXIT_MENUFUNC:
			sentAIareaCoord();
			UI_UpdateDevStatusInfo();

			UI_EnterAdjustAreaClosePakt(CAM, TRUE);
			//����ʱ����ΪZOOM_TRACE_CALIBRATE
//			tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_TRACE_CALIBRATE;
			UI_EnterZoomSettingCamera();
			break;
		default:
			break;
	}
}

//static void AdjustZoomBSDSmallAndBigArea(UI_ArrowKey_t tArrowKey)
//{
//	uint8_t CAM = DeskTopShowView;

//	switch(tArrowKey)
//	{
//		case LEFT_ARROW:
//			switch (ZoomBSDAIAreaRemoteMode)
//			{
//				case ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0]-4) >= 0)
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_BIG_UP_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1]-4) >= 0)
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_BIG_RIGHT_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-4 - tUI_CuSetting.usZoomBSDBigRedbox[CAM][0]) >= AI_AREA_MIN_SIZE && \
//						(tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-4) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2])
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_BIG_DOWN_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-4 - tUI_CuSetting.usZoomBSDBigRedbox[CAM][1]) >= AI_AREA_MIN_SIZE && \
//						(tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-4) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3])
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]-4) > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0])
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_UP_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]-4) > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1])
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]-4 - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]-4 - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] -= 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM :
//					if(1)
//					{
//						
//					}
//					break;
//			}
//			break;
//		case RIGHT_ARROW:
//			switch (ZoomBSDAIAreaRemoteMode)
//			{
//				case ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0]+4) < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0])
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_BIG_UP_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1]+4) < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1])
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_BIG_RIGHT_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]+4) <= 1024)
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_BIG_DOWN_ITEM :
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]+4) <= 600)
//					{
//						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_UP_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]+4) < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2])
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM :
//					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]+4) < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3])
//					{
//						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] += 4;
//					}
//					break;
//				case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM :
//					if(1)
//					{
//						
//					}
//					break;
//			}
//			break;
//	}
//	
//}


static void AdjustZoomBSDAreaReset(void)
{
	uint8_t uwZoomItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	tUI_CuSetting.usZoomBSDSmallRedbox[uwZoomItemIdx][0] = BSD_REDBOX_POINX_X1;
	tUI_CuSetting.usZoomBSDSmallRedbox[uwZoomItemIdx][1] = BSD_REDBOX_POINX_Y1;
	tUI_CuSetting.usZoomBSDSmallRedbox[uwZoomItemIdx][2] = BSD_REDBOX_POINX_X2;
	tUI_CuSetting.usZoomBSDSmallRedbox[uwZoomItemIdx][3] = BSD_REDBOX_POINX_Y2;

	tUI_CuSetting.usZoomBSDBigRedbox[uwZoomItemIdx][0] = BSD_YELLOWBOX_POINX_X1;
	tUI_CuSetting.usZoomBSDBigRedbox[uwZoomItemIdx][1] = BSD_YELLOWBOX_POINX_Y1;
	tUI_CuSetting.usZoomBSDBigRedbox[uwZoomItemIdx][2] = BSD_YELLOWBOX_POINX_X2;
	tUI_CuSetting.usZoomBSDBigRedbox[uwZoomItemIdx][3] = BSD_YELLOWBOX_POINX_Y2;

	
	ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM;
//	ZoomBSDAIAreaRemoteMode = ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM;
}

static void AdjustZoomBSDSmallAndBigArea(UI_ZoomVirtualKeyItemList_t ZoomVirtualKey)
{
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t tempSize = 0;

	switch(ZoomBSDBoxMoveSelect)
	{
		case ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] - BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] - BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_BIG_UP_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDBigRedbox[CAM][0]) >= AI_AREA_MIN_SIZE && \
//						(tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2])
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] - BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] + BSD_BOX_MOVE_SIZE) <= OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}
			break;
		case ZOOM_BSD_AI_AREA_BIG_RIGHT_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDBigRedbox[CAM][0]) >= AI_AREA_MIN_SIZE && \
//						(tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2])
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDBigRedbox[CAM][1]) >= BSD_BOX_MOVE_SIZE && \
//						(tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3])
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] + BSD_BOX_MOVE_SIZE) <= OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) <= OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_BIG_DOWN_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] - BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
//					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDBigRedbox[CAM][1]) >= BSD_BOX_MOVE_SIZE && \
//						(tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3])
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) <= OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						//printf("tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = %d, tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = %d", tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]);
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - AI_AREA_MIN_SIZE;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_UP_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]+BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - AI_AREA_MIN_SIZE;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]+BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]-BSD_BOX_MOVE_SIZE) > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1]) >= AI_AREA_MIN_SIZE)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2]-BSD_BOX_MOVE_SIZE - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]) >= AI_AREA_MIN_SIZE)
					{
						//printf("tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = %d, tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = %d", tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0]);
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - AI_AREA_MIN_SIZE;
					}
					break;
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT;
					}
					break;
			}	
			break;
		case ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM:
			switch(ZoomVirtualKey)
			{
				case ZOOM_VIRTUAL_LEFT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][0]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] -= tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] -= tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] -= tempSize;
					}
					break;
				case ZOOM_VIRTUAL_UP_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][1]-BSD_BOX_MOVE_SIZE) >= ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] -= BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] -= BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] -= tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] -= tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] -= tempSize;
					}
					break;
				case ZOOM_VIRTUAL_RIGHT_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][2]+BSD_BOX_MOVE_SIZE) <= OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = OSD_WIDTH - tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] += tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] += tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] += tempSize;
					}
					break;	
				case ZOOM_VIRTUAL_DOWN_KEY:
					if((tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]+BSD_BOX_MOVE_SIZE) <= OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
					{
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] += BSD_BOX_MOVE_SIZE;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] += BSD_BOX_MOVE_SIZE;
					}
					else
					{
						tempSize = OSD_HEIGHT - tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] += tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] += tempSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] += tempSize;
					}
					break;
			}	
			break;
	}
}


void UI_AdjustZoomBSDExecute(void)
{
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	OSD_IMG_INFO tOsdImgInfo;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo);
	tOsdImgInfo.uwXStart = 0;
	tOsdImgInfo.uwYStart = 0;

	//����ȫ��
	tOsdImgInfo.uwHSize = OSD_WIDTH;
	tOsdImgInfo.uwVSize = OSD_HEIGHT;
	
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo);

	//��ѡ��
	if(ZoomBSDBoxMoveSelect)
	{
		switch(tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx)
		{
			case ZOOM_VIRTUAL_LEFT_KEY:
				AdjustZoomBSDSmallAndBigArea(ZOOM_VIRTUAL_LEFT_KEY);
				break;
			case ZOOM_VIRTUAL_UP_KEY:
				AdjustZoomBSDSmallAndBigArea(ZOOM_VIRTUAL_UP_KEY);
				break;
			case ZOOM_VIRTUAL_RIGHT_KEY:
				AdjustZoomBSDSmallAndBigArea(ZOOM_VIRTUAL_RIGHT_KEY);
				break;
			case ZOOM_VIRTUAL_DOWN_KEY:
				AdjustZoomBSDSmallAndBigArea(ZOOM_VIRTUAL_DOWN_KEY);
				break;
			case ZOOM_VIRTUAL_OK_KEY:
				ZoomBSDBoxMoveSelect++;
				if(ZoomBSDBoxMoveSelect == ZOOM_BSD_AI_AREA_ITEM_MAX)
				{
					ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM;
				}
				break;
			case ZOOM_VIRTUAL_RESET_KEY:
				AdjustZoomBSDAreaReset();
				break;
		}
	}
	
	UI_DrawBSDSmallRedBox(tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1], \
		          tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]);
	UI_DrawBSDBigRedBox(tUI_CuSetting.usZoomBSDBigRedbox[CAM][0], tUI_CuSetting.usZoomBSDBigRedbox[CAM][1], \
		          tUI_CuSetting.usZoomBSDBigRedbox[CAM][2], tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]);

	//��ʾ���ⰴ��
	UI_ZoomDrawVirtualKey();
}

void UI_AdjustZoomBSDArea(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tZoomVirtualKeyItem);
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_AdjustZoomVirtualItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			printf("UI_AdjustZoomBSDArea DRAW_MENUPAGE\n");
			UI_AdjustZoomBSDExecute();
			break;
		}
		case EXIT_MENUFUNC:
			sentBSDAIareaCoord();
			UI_UpdateDevStatusInfo();
			
			UI_EnterAdjustAreaClosePakt(CAM, TRUE);
//			tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_BSD_CALIBRATE;
			UI_EnterZoomSettingCamera();
			break;
		default:
			break;
	}
}


/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/

/*************************************************************Home Menu**********************************************************************************/

void UI_HomeMenuTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	uint16_t i = 0;
	OSD_IMG_INFO tOsdImgInfo,tOsdImgInfo1[2];
	tUI_MenuItem.ubItemPreIdx = tUI_MenuItem.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ADO_MODENOR_MENUICON, 1, &tOsdImgInfo);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOMNOR_MENUICON, 2, &tOsdImgInfo1[0]);
			if((!TriggerLock)&&(tUI_CuSetting.ubisZoomCameraFlag[0] || tUI_CuSetting.ubisZoomCameraFlag[1] || tUI_CuSetting.ubisZoomCameraFlag[2] || tUI_CuSetting.ubisZoomCameraFlag[3]))
			{
				if(Touch_Info->startX > tOsdImgInfo1[0].uwXStart
						&& Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[0].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
				{  
					tOSD_Img2(&tOsdImgInfo1[1], OSD_UPDATE);
					UI_ZoomSubMenuPage(ENTER_ARROW);
					break;	
				}
			}	
			if(Touch_Info->startY > tOsdImgInfo.uwYStart)
			{
				if(!is_any_heating_cam_paired()){
					for(i = 0; i < UI_MENUICON_NUM;i++)//�жϾ��������ĸ�ͼ����
					{
						if(i == HEATING_ITEM) continue;
						
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ADO_MODENOR_MENUICON + 2*i, 1, &tOsdImgInfo);	
					
						if(Touch_Info->startX > tOsdImgInfo.uwXStart
							&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
							&& Touch_Info->startY > tOsdImgInfo.uwYStart 
							&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
						{
							tUI_MenuItem.ubItemIdx = i;
							break;
						}
					}
				}else{
					for(i = 0; i < UI_MENUICON_NUM ;i++) //�жϾ��������ĸ�ͼ����
					{						
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ADO_MODENOR_MENUICON + 2*i, 1, &tOsdImgInfo);	
					
						if(i == POWER_ITEM)
							tOsdImgInfo.uwXStart += DRAW_HEATING_ICON_SPACE_CHANGE;
						else if(i != HEATING_ITEM)
							tOsdImgInfo.uwXStart -= DRAW_HEATING_ICON_SPACE_CHANGE;
							
						if(Touch_Info->startX > tOsdImgInfo.uwXStart
							&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
							&& Touch_Info->startY > tOsdImgInfo.uwYStart 
							&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
						{
							tUI_MenuItem.ubItemIdx = i;
							break;
						}
					}
				}
				if(i == UI_MENUICON_NUM)
					return;
				//��tUI_MenuItem.ubItemIdx��tUI_MenuItem.ubItemPreIdx�����ʱ���ʹ����������
				if(tUI_MenuItem.ubItemIdx != tUI_MenuItem.ubItemPreIdx)
				{
					UI_MenuDrawMenuItem();
				}

				if(tUI_MenuItem.ubItemIdx != RECORD_ITEM)
				{
					UI_EnterKey();
				}
				else
				{
					UI_MenuKey();
					UI_EnterKey();
				}
			}
			else if(Touch_Info->startY < tOsdImgInfo.uwYStart - 30)//������ui�Ϸ����˳�menu
			{
				UI_MenuKey();
			}
			
			break;

		default :

			break;
	}
}



/*************************************************************Volume Menu**********************************************************************************/
void UI_VolumeMenuTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t ubPreVolumeLvl = tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_VOLUME_MUTE, 1, &tOsdImgInfo);	//���ƾ���
			if(Touch_Info->startX > tOsdImgInfo.uwXStart - 10
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 10
					&& Touch_Info->startY > tOsdImgInfo.uwYStart - 10
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 10)
			{
				
				tUI_CuSetting.ubMuteFlag = 1 - tUI_CuSetting.ubMuteFlag;
				UI_VolumeMenuDrawMenuItem(ubPreVolumeLvl);
				UI_AudioOn();
				return;
			}
			else
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SOUND_SLIDER, 1, &tOsdImgInfo);	//����ڻ�������
				if(Touch_Info->startX > tOsdImgInfo.uwXStart - 10
						&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 10
						&& Touch_Info->startY > tOsdImgInfo.uwYStart - 40 
						&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 40)
				{
					return;
				}
				else
					UI_MenuKey();
			}
			break;
		case TOUCH_PRESSDOWN:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SOUND_SLIDER,1,&tOsdImgInfo);
			if(Touch_Info->endX >= tOsdImgInfo.uwXStart - 10 
				&& Touch_Info->endX <= tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 10
				&& Touch_Info->endY >= tOsdImgInfo.uwYStart - 40
				&& Touch_Info->endY <= tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 40)
			{
				if(Touch_Info->endX < tOsdImgInfo.uwXStart)
					Touch_Info->endX = tOsdImgInfo.uwXStart;
				if(Touch_Info->endX > tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize)
					Touch_Info->endX = tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize;


				//����λ��ת�ɽ�������ֵ
				uint8_t ubValue = 
					(uint8_t)UI_Map(Touch_Info->endX,tOsdImgInfo.uwXStart,tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize,0,99);
					
				tUI_CamStatus[UI_GetAudioSrc()].ubVolumeLvl = ubValue;		
				
				UI_VolumeMenuDrawMenuItem(ubPreVolumeLvl);
				UI_AudioOn();
			}
			
			break;
		default:
	
			return;
	}
	
	

}
/*************************************************************SdCard Info**********************************************************************************/

//------------------------------------------------------------------------------
void UI_ShowSdCardInfoTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
				TIMER_Delay_ms(30);
				UI_MenuKey();
			}
			break;
		
		default:
	
			return;
	}
	
	

}
/*************************************************************Tx Version**********************************************************************************/

//------------------------------------------------------------------------------
void UI_ShowTxVersionTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
				TIMER_Delay_ms(30);
				UI_MenuKey();
			}
			break;
		
		default:
	
			return;
	}

}

//------------------------------------------------------------------------------
void UI_AdjustZoomAreaTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo, tOsdImgInfo1[12], tOsdImgInfo2;;
	uint16_t uwHSize = 0, uwVSize = 0, tempSize = 0;
	static uint8_t ubTouchPressDownCount = 0, drag_dot = 0xff;//�ñ����Ǹ�TOUCH_PRESSDOWN������
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	uint8_t IsTouchPress = FALSE;

	tZoomVirtualKeyItem.tSubMenuInfo.ubItemPreIdx = tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx;

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_LEFT_KEY_NOR, 12, &tOsdImgInfo1);
	
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
//			printf("TOUCH_PRESS\n");
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
				TIMER_Delay_ms(30);
				UI_MenuKey();
			}
			else if(IsZoomTracePressDownVirtualKey == FALSE)
			{
				IsZoomTracePressDownVirtualKey = TRUE;
				UI_ZoomDrawVirtualKey();
			}
			else
			{
//				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo);	
//				if(Touch_Info->startX > BSD_RESTART_ITEM_DIST_X 
//					&& Touch_Info->startX < BSD_RESTART_ITEM_DIST_X + tOsdImgInfo.uwHSize
//					&& Touch_Info->startY > BSD_RESTART_ITEM_DIST_Y 
//					&& Touch_Info->startY < BSD_RESTART_ITEM_DIST_Y + tOsdImgInfo.uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RESET_KEY;
//					IsTouchPress = TRUE;
//				}
				if(Touch_Info->startX > tOsdImgInfo1[0].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[0].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
				{
					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
					IsTouchPress = TRUE;
				}
				else if(Touch_Info->startX > tOsdImgInfo1[2].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[2].uwXStart + tOsdImgInfo1[2].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[2].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[2].uwYStart + tOsdImgInfo1[2].uwVSize)
				{
					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_UP_KEY;
					IsTouchPress = TRUE;
				}
				else if(Touch_Info->startX > tOsdImgInfo1[4].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[4].uwXStart + tOsdImgInfo1[4].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[4].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[4].uwYStart + tOsdImgInfo1[4].uwVSize)
				{
					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RIGHT_KEY;
					IsTouchPress = TRUE;
				}
				else if(Touch_Info->startX > tOsdImgInfo1[6].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[6].uwXStart + tOsdImgInfo1[6].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[6].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[6].uwYStart + tOsdImgInfo1[6].uwVSize)
				{
					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_OK_KEY;
					IsTouchPress = TRUE;
				}
				else if(Touch_Info->startX > tOsdImgInfo1[8].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[8].uwXStart + tOsdImgInfo1[8].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[8].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[8].uwYStart + tOsdImgInfo1[8].uwVSize)
				{
					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_DOWN_KEY;
					IsTouchPress = TRUE;
				}
				else if(Touch_Info->startX > tOsdImgInfo1[10].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[10].uwXStart + tOsdImgInfo1[10].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[10].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[10].uwYStart + tOsdImgInfo1[10].uwVSize)
				{
					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RESET_KEY;
					IsTouchPress = TRUE;
				}

				
				if(IsTouchPress == TRUE)
				{
					UI_EnterKey();
					IsTouchPress = FALSE;
				}
			}
			//��ʼ��ѡ��
			drag_dot = 0xff;
			break;
		case TOUCH_PRESSDOWN:
//			printf("TOUCH_PRESSDOWN\n");
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				return;
			}
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo);
			if(Touch_Info->startX > BSD_RESTART_ITEM_DIST_X 
				&& Touch_Info->startX < BSD_RESTART_ITEM_DIST_X + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > BSD_RESTART_ITEM_DIST_Y 
				&& Touch_Info->startY < BSD_RESTART_ITEM_DIST_Y + tOsdImgInfo.uwVSize)
			{
				return;
			}

			//����pressDown�ٶ�
			if(++ ubTouchPressDownCount == 0xff)
				ubTouchPressDownCount = 0;
			if(ubTouchPressDownCount%8 != 0)
			{
				return;				
			}


			if(Touch_Info->pressPoints == 1)
			{
				if(IsZoomTracePressDownVirtualKey)
				{
					if(Touch_Info->startX > tOsdImgInfo1[0].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[0].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
					}
					else if(Touch_Info->startX > tOsdImgInfo1[2].uwXStart 
						&& Touch_Info->startX < tOsdImgInfo1[2].uwXStart + tOsdImgInfo1[2].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[2].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[2].uwYStart + tOsdImgInfo1[2].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_UP_KEY;
					}
					else if(Touch_Info->startX > tOsdImgInfo1[4].uwXStart 
						&& Touch_Info->startX < tOsdImgInfo1[4].uwXStart + tOsdImgInfo1[4].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[4].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[4].uwYStart + tOsdImgInfo1[4].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RIGHT_KEY;
					}
					else if(Touch_Info->startX > tOsdImgInfo1[8].uwXStart 
						&& Touch_Info->startX < tOsdImgInfo1[8].uwXStart + tOsdImgInfo1[8].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[8].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[8].uwYStart + tOsdImgInfo1[8].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_DOWN_KEY;
					}
					//���ⰴ������
					else if(Touch_Info->startX > 0 
					&& Touch_Info->startX < tOsdImgInfo1[4].uwXStart + tOsdImgInfo1[4].uwHSize + BSD_BOX_MOVE_SIZE
					&& Touch_Info->startY > tOsdImgInfo1[2].uwYStart - BSD_BOX_MOVE_SIZE
					&& Touch_Info->startY < OSD_HEIGHT)
					{
						return;
					}
					else
					{
						IsZoomTracePressDownVirtualKey = FALSE;
						break;
					}
					UI_EnterKey();
					return;
				}

//				printf("not handle\n");
				//�����߻������ƶ�
				tOsdImgInfo.uwXStart = 0;
				tOsdImgInfo.uwYStart = 0;

				//��������ȫ��
				tOsdImgInfo.uwHSize = OSD_WIDTH;
				tOsdImgInfo.uwVSize = OSD_HEIGHT;
				OSD_EraserImg2_NoUpdate(&tOsdImgInfo);

				if((Touch_Info->startX > tUI_CuSetting.usZoomRedbox[CAM][0] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomRedbox[CAM][0] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomRedbox[CAM][1] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomRedbox[CAM][1] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 4)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM;
						drag_dot = 4;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomRedbox[CAM][2] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomRedbox[CAM][2] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomRedbox[CAM][1] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomRedbox[CAM][1] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 5)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_UP_ITEM;
						drag_dot = 5;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomRedbox[CAM][2] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomRedbox[CAM][2] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomRedbox[CAM][3] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomRedbox[CAM][3] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 6)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM;
						drag_dot = 6;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomRedbox[CAM][0] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomRedbox[CAM][0] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomRedbox[CAM][3] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomRedbox[CAM][3] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 7)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM;
						drag_dot = 7;
					}
				}
				else
				{
					if (drag_dot == 0xff)
					{
						ZoomTraceBoxMoveSelect = ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM;
						drag_dot = 8;
					}
				}

				switch (drag_dot)
				{
					//С��
					case 4://��
					{
						if (Touch_Info->endX < tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE && Touch_Info->endX > ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = Touch_Info->endX;
						}
						else if(Touch_Info->endX >= tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						}

						if (Touch_Info->endY < tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE && Touch_Info->endY > ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = Touch_Info->endY;
						}
						else if(Touch_Info->endY >= tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					case 5://��
					{
						if (Touch_Info->endY < tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE && Touch_Info->endY > ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = Touch_Info->endY;
						}
						else if(Touch_Info->endY >= tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = tUI_CuSetting.usZoomRedbox[CAM][3] - AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						}

						if (Touch_Info->endX > tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE && Touch_Info->endX < OSD_WIDTH - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][2] = Touch_Info->endX;
						}
						else if(Touch_Info->endX <= tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][2] = tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					case 6://��
					{
						if (Touch_Info->endX > tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE && Touch_Info->endX < OSD_WIDTH - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][2] = Touch_Info->endX;
						}
						else if(Touch_Info->endX <= tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][2] = tUI_CuSetting.usZoomRedbox[CAM][0] + AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
						}

						if (Touch_Info->endY > tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE && Touch_Info->endY < OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][3] = Touch_Info->endY;
						}
						else if(Touch_Info->endY <= tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][3] = tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					case 7://��
					{
						if (Touch_Info->endY > tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE && Touch_Info->endY < OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][3] = Touch_Info->endY;
						}
						else if(Touch_Info->endY <= tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][3] = tUI_CuSetting.usZoomRedbox[CAM][1] + AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
						}

						if (Touch_Info->endX < tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE && Touch_Info->endX > ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = Touch_Info->endX;
						}
						else if(Touch_Info->endX >= tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE)
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = tUI_CuSetting.usZoomRedbox[CAM][2] - AI_AREA_MIN_SIZE;
						}
						else
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					//�����ƶ�
					case 8:
					{
						uwHSize = tUI_CuSetting.usZoomRedbox[CAM][2] - tUI_CuSetting.usZoomRedbox[CAM][0];
						uwVSize = tUI_CuSetting.usZoomRedbox[CAM][3] - tUI_CuSetting.usZoomRedbox[CAM][1];

						//small box�����ƶ� x����
						if((Touch_Info->endX - uwHSize/2 > ZOOM_BOX_DIST_LIMIT) && (Touch_Info->endX + uwHSize/2 < OSD_WIDTH - ZOOM_BOX_DIST_LIMIT))
						{
							//small big box x1 y1�ľ���λ��
							tUI_CuSetting.usZoomRedbox[CAM][0] = (Touch_Info->endX - uwHSize/2);
						}
						else if((Touch_Info->endX - uwHSize/2 < ZOOM_BOX_DIST_LIMIT))
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						}
						else if(Touch_Info->endX + uwHSize/2 > OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomRedbox[CAM][0] = OSD_WIDTH  - ZOOM_BOX_DIST_LIMIT - uwHSize;
						}

						//small box�����ƶ� y����
						if((Touch_Info->endY - uwVSize/2 > ZOOM_BOX_DIST_LIMIT) && (Touch_Info->endY + uwVSize/2 < OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT))
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = (Touch_Info->endY - uwVSize/2);
						}
						else if((Touch_Info->endY - uwVSize/2 < 0))
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						}
						else if(Touch_Info->endY + uwVSize/2 > OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomRedbox[CAM][1] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT - uwVSize;
						}

						//��С����
						tUI_CuSetting.usZoomRedbox[CAM][2] = tUI_CuSetting.usZoomRedbox[CAM][0] + uwHSize;
						tUI_CuSetting.usZoomRedbox[CAM][3] = tUI_CuSetting.usZoomRedbox[CAM][1] + uwVSize;
					}
					break;
				}
				UI_DrawRedBox(tUI_CuSetting.usZoomRedbox[CAM][0], tUI_CuSetting.usZoomRedbox[CAM][1], \
		          tUI_CuSetting.usZoomRedbox[CAM][2], tUI_CuSetting.usZoomRedbox[CAM][3]);
				//restart��������ʧ
				OSD_UpdateImg2();
//				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo2);
//				tOsdImgInfo2.uwXStart = BSD_RESTART_ITEM_DIST_X;
//				tOsdImgInfo2.uwYStart = BSD_RESTART_ITEM_DIST_Y;
//				tOSD_Img2(&tOsdImgInfo2, OSD_UPDATE);
			}
			break;
		default:
			drag_dot = 0xff;
			//������ʾ���ⰴ��
			IsZoomTracePressDownVirtualKey = TRUE;
			UI_ZoomDrawVirtualKey();
			return;
	}
}


//------------------------------------------------------------------------------
void UI_AdjustZoomBSDAreaTouchFunc(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo, tOsdImgInfo1[12], tOsdImgInfo2;
	uint16_t BSDSmallBoxHSize = 0, BSDSmallBoxVsize = 0, BSDBigBoxHSize = 0, BSDBigBoxVsize = 0, tempSize = 0;
	static uint8_t ubTouchPressDownCount = 0, drag_dot = 0xff;//�ñ����Ǹ�TOUCH_PRESSDOWN������
	uint8_t CAM = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	uint8_t IsTouchPress = FALSE;

	tZoomVirtualKeyItem.tSubMenuInfo.ubItemPreIdx = tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx;

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_VIRTUAL_LEFT_KEY_NOR, 12, &tOsdImgInfo1);
	
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
//			printf("TOUCH_PRESS\n");
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
				TIMER_Delay_ms(30);
				UI_MenuKey();
			}
			//���ⰴ������
			else if(IsZoomBSDPressDownVirtualKey == FALSE)
			{
				IsZoomBSDPressDownVirtualKey = TRUE;
				UI_ZoomDrawVirtualKey();
			}
			else
			{
//				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo);	
//				if(Touch_Info->startX > BSD_RESTART_ITEM_DIST_X 
//					&& Touch_Info->startX < BSD_RESTART_ITEM_DIST_X + tOsdImgInfo.uwHSize
//					&& Touch_Info->startY > BSD_RESTART_ITEM_DIST_Y 
//					&& Touch_Info->startY < BSD_RESTART_ITEM_DIST_Y + tOsdImgInfo.uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RESET_KEY;
//					IsTouchPress = TRUE;
//				}
				for(uint8_t i = 0; i < ZOOM_VIRTUAL_KEY_MAX; i++)
				{
					if(Touch_Info->startX > tOsdImgInfo1[i*2].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[i*2].uwXStart + tOsdImgInfo1[i*2].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[i*2].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[i*2].uwYStart + tOsdImgInfo1[i*2].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = i;
						IsTouchPress = TRUE;
						break;
					}
				}
//				if(Touch_Info->startX > tOsdImgInfo1[0].uwXStart 
//					&& Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize
//					&& Touch_Info->startY > tOsdImgInfo1[0].uwYStart 
//					&& Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
//					IsTouchPress = TRUE;
//				}
//				else if(Touch_Info->startX > tOsdImgInfo1[2].uwXStart 
//					&& Touch_Info->startX < tOsdImgInfo1[2].uwXStart + tOsdImgInfo1[2].uwHSize
//					&& Touch_Info->startY > tOsdImgInfo1[2].uwYStart 
//					&& Touch_Info->startY < tOsdImgInfo1[2].uwYStart + tOsdImgInfo1[2].uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_UP_KEY;
//					IsTouchPress = TRUE;
//				}
//				else if(Touch_Info->startX > tOsdImgInfo1[4].uwXStart 
//					&& Touch_Info->startX < tOsdImgInfo1[4].uwXStart + tOsdImgInfo1[4].uwHSize
//					&& Touch_Info->startY > tOsdImgInfo1[4].uwYStart 
//					&& Touch_Info->startY < tOsdImgInfo1[4].uwYStart + tOsdImgInfo1[4].uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RIGHT_KEY;
//					IsTouchPress = TRUE;
//				}
//				else if(Touch_Info->startX > tOsdImgInfo1[6].uwXStart 
//					&& Touch_Info->startX < tOsdImgInfo1[6].uwXStart + tOsdImgInfo1[6].uwHSize
//					&& Touch_Info->startY > tOsdImgInfo1[6].uwYStart 
//					&& Touch_Info->startY < tOsdImgInfo1[6].uwYStart + tOsdImgInfo1[6].uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_OK_KEY;
//					IsTouchPress = TRUE;
//				}
//				else if(Touch_Info->startX > tOsdImgInfo1[8].uwXStart 
//					&& Touch_Info->startX < tOsdImgInfo1[8].uwXStart + tOsdImgInfo1[8].uwHSize
//					&& Touch_Info->startY > tOsdImgInfo1[8].uwYStart 
//					&& Touch_Info->startY < tOsdImgInfo1[8].uwYStart + tOsdImgInfo1[8].uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_DOWN_KEY;
//					IsTouchPress = TRUE;
//				}
//				else if(Touch_Info->startX > tOsdImgInfo1[10].uwXStart 
//					&& Touch_Info->startX < tOsdImgInfo1[10].uwXStart + tOsdImgInfo1[10].uwHSize
//					&& Touch_Info->startY > tOsdImgInfo1[10].uwYStart 
//					&& Touch_Info->startY < tOsdImgInfo1[10].uwYStart + tOsdImgInfo1[10].uwVSize)
//				{
//					tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RESET_KEY;
//					IsTouchPress = TRUE;
//				}

				
				if(IsTouchPress == TRUE)
				{
					UI_EnterKey();
					IsTouchPress = FALSE;
				}
			}
			//��ʼ��ѡ��
			drag_dot = 0xff;
			break;
		case TOUCH_PRESSDOWN:
			//���غ����ú�OK��������
//			printf("TOUCH_PRESSDOWN\n");
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				return;                                                                                         
			}
//			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo);
//			if(Touch_Info->startX > BSD_RESTART_ITEM_DIST_X 
//				&& Touch_Info->startX < BSD_RESTART_ITEM_DIST_X + tOsdImgInfo.uwHSize
//				&& Touch_Info->startY > BSD_RESTART_ITEM_DIST_Y 
//				&& Touch_Info->startY < BSD_RESTART_ITEM_DIST_Y + tOsdImgInfo.uwVSize)
//			{
//				return;
//			}

			//����pressDown�ٶ�
			if(++ ubTouchPressDownCount == 0xff)
				ubTouchPressDownCount = 0;
			if(ubTouchPressDownCount%8 != 0)
			{
				return;				
			}
				
			//��ָ
			if(Touch_Info->pressPoints == 1)
			{
				//���ⰴ����
				if(IsZoomBSDPressDownVirtualKey)
				{
					if(Touch_Info->startX > tOsdImgInfo1[0].uwXStart 
					&& Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize
					&& Touch_Info->startY > tOsdImgInfo1[0].uwYStart 
					&& Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_LEFT_KEY;
					}
					else if(Touch_Info->startX > tOsdImgInfo1[2].uwXStart 
						&& Touch_Info->startX < tOsdImgInfo1[2].uwXStart + tOsdImgInfo1[2].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[2].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[2].uwYStart + tOsdImgInfo1[2].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_UP_KEY;
					}
					else if(Touch_Info->startX > tOsdImgInfo1[4].uwXStart 
						&& Touch_Info->startX < tOsdImgInfo1[4].uwXStart + tOsdImgInfo1[4].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[4].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[4].uwYStart + tOsdImgInfo1[4].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_RIGHT_KEY;
					}
					else if(Touch_Info->startX > tOsdImgInfo1[8].uwXStart 
						&& Touch_Info->startX < tOsdImgInfo1[8].uwXStart + tOsdImgInfo1[8].uwHSize
						&& Touch_Info->startY > tOsdImgInfo1[8].uwYStart 
						&& Touch_Info->startY < tOsdImgInfo1[8].uwYStart + tOsdImgInfo1[8].uwVSize)
					{
						tZoomVirtualKeyItem.tSubMenuInfo.ubItemIdx = ZOOM_VIRTUAL_DOWN_KEY;
					}
					//���ⰴ������
					else if(Touch_Info->startX > 0 
					&& Touch_Info->startX < tOsdImgInfo1[4].uwXStart + tOsdImgInfo1[4].uwHSize + BSD_BOX_MOVE_SIZE
					&& Touch_Info->startY > tOsdImgInfo1[2].uwYStart - BSD_BOX_MOVE_SIZE
					&& Touch_Info->startY < OSD_HEIGHT)
					{
						return;
					}
					else
					{
						IsZoomBSDPressDownVirtualKey = FALSE;
						break;
					}
					UI_EnterKey();
					return;
				}

				
//				printf("not handle\n");
				//�����߻������ƶ�
				tOsdImgInfo.uwXStart = 0;
				tOsdImgInfo.uwYStart = 0;

				//��������ȫ��
				tOsdImgInfo.uwHSize = OSD_WIDTH;
				tOsdImgInfo.uwVSize = OSD_HEIGHT;
				OSD_EraserImg2_NoUpdate(&tOsdImgInfo);

				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_BOX_SELECT, 1, &tOsdImgInfo2);
				//printf("drag_dot = %d ========== \n", drag_dot);
				if((Touch_Info->startX > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 0)) // �����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_LEFT_ITEM;
						drag_dot = 0;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 1)) // �����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_UP_ITEM;
						drag_dot = 1;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 2)) // �����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_RIGHT_ITEM;
						drag_dot = 2;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 3)) // �����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_BIG_DOWN_ITEM;
						drag_dot = 3;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 4)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_LEFT_ITEM;
						drag_dot = 4;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 5)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_UP_ITEM;
						drag_dot = 5;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 6)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_RIGHT_ITEM;
						drag_dot = 6;
					}
				}
				else if((Touch_Info->startX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - BSD_AREA_WIDTH/2 - ZOOM_BOX_SELECT_RAD*2
					&& Touch_Info->startY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + BSD_AREA_WIDTH/2 + ZOOM_BOX_SELECT_RAD*2) || (drag_dot == 7)) // С����
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_SMALL_DOWN_ITEM;
						drag_dot = 7;
					}
				}
				else
				{
					if (drag_dot == 0xff)
					{
						ZoomBSDBoxMoveSelect = ZOOM_BSD_AI_AREA_ALL_MOVE_ITEM;
						drag_dot = 8;
					}
				}

				switch (drag_dot)
				{
					//���
					case 0://��
					{
						//if (Touch_Info->endX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - AI_AREA_MIN_SIZE)
						if (Touch_Info->endX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT && Touch_Info->endX > ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = Touch_Info->endX;
						}
						else if(Touch_Info->endX >= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						}
						
						//if (Touch_Info->endY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - AI_AREA_MIN_SIZE)
						if (Touch_Info->endY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT && Touch_Info->endY > ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = Touch_Info->endY;
						}
						else if(Touch_Info->endY >= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					case 1://��
					{
						//if (Touch_Info->endY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - AI_AREA_MIN_SIZE)
						if (Touch_Info->endY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT && Touch_Info->endY > ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = Touch_Info->endY;
						}
						else if(Touch_Info->endY >= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
						}

						if (Touch_Info->endX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT && Touch_Info->endX < OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = Touch_Info->endX;
						}
						else if(Touch_Info->endX <= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT)
						{
							//tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + AI_AREA_MIN_SIZE;
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					case 2://��
					{
						if (Touch_Info->endX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT && Touch_Info->endX < OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = Touch_Info->endX;
						}
						else if(Touch_Info->endX <= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT)
						{
							//tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + AI_AREA_MIN_SIZE;
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = OSD_WIDTH - ZOOM_BOX_DIST_LIMIT;
						}

						//if (Touch_Info->endY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + AI_AREA_MIN_SIZE) 
						if (Touch_Info->endY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT && Touch_Info->endY < OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = Touch_Info->endY;
						}
						else if(Touch_Info->endY <= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT)
						{
							//tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + AI_AREA_MIN_SIZE;
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					case 3://��
					{
						//if (Touch_Info->endY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + AI_AREA_MIN_SIZE) 
						if (Touch_Info->endY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT && Touch_Info->endY < OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = Touch_Info->endY;
						}
						else if(Touch_Info->endY <= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT)
						{
							//tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + AI_AREA_MIN_SIZE;
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT;
						}

						//if (Touch_Info->endX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - AI_AREA_MIN_SIZE)
						if (Touch_Info->endX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT && Touch_Info->endX > ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = Touch_Info->endX;
						}
						else if(Touch_Info->endX >= tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
						}
					}
					break;
					//С��
					case 4://��
					{
						if (Touch_Info->endX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - AI_AREA_MIN_SIZE && Touch_Info->endX > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = Touch_Info->endX;
						}
						else if(Touch_Info->endX <= tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - AI_AREA_MIN_SIZE;
						}

						if (Touch_Info->endY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - AI_AREA_MIN_SIZE && Touch_Info->endY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = Touch_Info->endY;
						}
						else if(Touch_Info->endY <= tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - AI_AREA_MIN_SIZE;
						}
					}
					break;
					case 5://��
					{
						if (Touch_Info->endY < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - AI_AREA_MIN_SIZE && Touch_Info->endY > tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = Touch_Info->endY;
						}
						else if(Touch_Info->endY <= tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - AI_AREA_MIN_SIZE;
						}

						if (Touch_Info->endX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + AI_AREA_MIN_SIZE && Touch_Info->endX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = Touch_Info->endX;
						}
						else if(Touch_Info->endX >= tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + AI_AREA_MIN_SIZE;
						}
					}
					break;
					case 6://��
					{
						if (Touch_Info->endX > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + AI_AREA_MIN_SIZE && Touch_Info->endX < tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = Touch_Info->endX;
						}
						else if(Touch_Info->endX >= tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + AI_AREA_MIN_SIZE;
						}

						
						if (Touch_Info->endY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + AI_AREA_MIN_SIZE && Touch_Info->endY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = Touch_Info->endY;
						}
						else if(Touch_Info->endY >= tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + AI_AREA_MIN_SIZE;
						}
					}
					break;
					case 7://��
					{
						if (Touch_Info->endY > tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + AI_AREA_MIN_SIZE && Touch_Info->endY < tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = Touch_Info->endY;
						}
						else if(Touch_Info->endY >= tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + AI_AREA_MIN_SIZE;
						}

						if (Touch_Info->endX < tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - AI_AREA_MIN_SIZE && Touch_Info->endX > tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT) 
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = Touch_Info->endX;
						}
						else if(Touch_Info->endX <= tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + ZOOM_BOX_DIST_LIMIT;
						}
						else
						{
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - AI_AREA_MIN_SIZE;
						}
					}
					break;
					//�����ƶ�
					case 8:
					{
						BSDSmallBoxHSize = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0];
						BSDSmallBoxVsize = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] - tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1];
						BSDBigBoxHSize = tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] - tUI_CuSetting.usZoomBSDBigRedbox[CAM][0];
						BSDBigBoxVsize = tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] - tUI_CuSetting.usZoomBSDBigRedbox[CAM][1];
						
						//Big box small box�����ƶ� x����
						tempSize = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] - tUI_CuSetting.usZoomBSDBigRedbox[CAM][0];
						if((Touch_Info->endX - BSDBigBoxHSize/2 > ZOOM_BOX_DIST_LIMIT) && (Touch_Info->endX + BSDBigBoxHSize/2 < OSD_WIDTH - ZOOM_BOX_DIST_LIMIT))
						{
							//small big box x1 y1�ľ���λ��
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = (Touch_Info->endX - BSDBigBoxHSize/2);
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + tempSize;
							 
						}
						else if((Touch_Info->endX - BSDBigBoxHSize/2 < ZOOM_BOX_DIST_LIMIT))
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = ZOOM_BOX_DIST_LIMIT;
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + tempSize;
						}
						else if(Touch_Info->endX + BSDBigBoxHSize/2 > OSD_WIDTH - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] = OSD_WIDTH  - ZOOM_BOX_DIST_LIMIT - BSDBigBoxHSize;
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + tempSize;
						}

						//Big box small box�����ƶ� y����
						tempSize = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] - tUI_CuSetting.usZoomBSDBigRedbox[CAM][1];
						if((Touch_Info->endY - BSDBigBoxVsize/2 > ZOOM_BOX_DIST_LIMIT) && (Touch_Info->endY + BSDBigBoxVsize/2 < OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT))
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = (Touch_Info->endY - BSDBigBoxVsize/2);
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + tempSize;
						}
						else if((Touch_Info->endY - BSDBigBoxVsize/2 < ZOOM_BOX_DIST_LIMIT))
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = ZOOM_BOX_DIST_LIMIT;
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + tempSize;
						}
						else if(Touch_Info->endY + BSDBigBoxVsize/2 > OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT)
						{
							tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] = OSD_HEIGHT - ZOOM_BOX_DIST_LIMIT - BSDBigBoxVsize;
							tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + tempSize;
						}

						//��С����
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][2] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][0] + BSDBigBoxHSize;
						tUI_CuSetting.usZoomBSDBigRedbox[CAM][3] = tUI_CuSetting.usZoomBSDBigRedbox[CAM][1] + BSDBigBoxVsize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0] + BSDSmallBoxHSize;
						tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3] = tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1] + BSDSmallBoxVsize;
					}
					break;
				}
				UI_DrawBSDSmallRedBox(tUI_CuSetting.usZoomBSDSmallRedbox[CAM][0], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][1], \
		          tUI_CuSetting.usZoomBSDSmallRedbox[CAM][2], tUI_CuSetting.usZoomBSDSmallRedbox[CAM][3]);
				UI_DrawBSDBigRedBox(tUI_CuSetting.usZoomBSDBigRedbox[CAM][0], tUI_CuSetting.usZoomBSDBigRedbox[CAM][1], \
		          tUI_CuSetting.usZoomBSDBigRedbox[CAM][2], tUI_CuSetting.usZoomBSDBigRedbox[CAM][3]);
				OSD_UpdateImg2();
				//restart��������ʧ
//				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RESTART_NOR, 1, &tOsdImgInfo2);
//				tOsdImgInfo2.uwXStart = BSD_RESTART_ITEM_DIST_X;
//				tOsdImgInfo2.uwYStart = BSD_RESTART_ITEM_DIST_Y;
//				tOSD_Img2(&tOsdImgInfo2, OSD_UPDATE);
			}
			break;
		default:
//			printf("TOUCH_default\n");
			drag_dot = 0xff;
			//������ʾ���ⰴ��
			IsZoomBSDPressDownVirtualKey = TRUE;
			UI_ZoomDrawVirtualKey();
			return;
	}
}




#endif

