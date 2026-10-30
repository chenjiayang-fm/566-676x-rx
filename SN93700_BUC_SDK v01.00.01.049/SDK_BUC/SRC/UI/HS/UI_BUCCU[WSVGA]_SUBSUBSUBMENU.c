
#include "UI_BUCCU[WSVGA]_SUBSUBSUBMENU.h"
#include "math.h"
#include <stdlib.h>
#ifdef BSP_D_SNCC71_GM8285C_RX_V2

void UI_CameraSubSubSubMenuPage(UI_ArrowKey_t tArrowKey);
void UI_CameraSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_SystemSubSubSubMenuPage(UI_ArrowKey_t tArrowKey);
void UI_SystemSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info);

void UI_ParkingLineSubSubSubMenuPage(UI_ArrowKey_t tArrowKey);
void UI_ParkingLIneSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info);

void UI_GuideLineSubSubSubMenuPage(UI_ArrowKey_t tArrowKey);
void UI_GuideLineSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info);


void UI_SubSubSubKeyMenu(UI_ArrowKey_t tArrowKey)
{
	switch(tUI_MenuItem.ubItemIdx)
	{	
		case PLAYBACK_ITEM:
			UI_PlaybackSubSubSubMenuPage(tArrowKey);
			break;
		case SETTING_ITEM:
			{
				UI_SettingSubMenuItemList_t tSubMenuItem = 
					(UI_SettingSubMenuItemList_t)tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;//sub
				 switch(tSubMenuItem)
				 {
				 	case CAMERASET_ITEM:
						UI_CameraSubSubSubMenuPage(tArrowKey);
						break;
					case SYSTEMSET_ITEM:
						UI_SystemSubSubSubMenuPage(tArrowKey);
						break;
					case PARKINGLINE_ITEM:
						UI_ParkingLineSubSubSubMenuPage(tArrowKey);
						break;
					case AI_ITEM:
						UI_AISubSubSubMenuPage(tArrowKey);
						break;
					case GUIDELINE_ITEM:
						UI_GuideLineSubSubSubMenuPage(tArrowKey);
						break;
					default:
						UI_SettingSubSubSubMenuPage(tArrowKey);
						break;
				 }
					
			}
			
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------------playback------------------------------------------------------------------------------
//------------------------------------------------------------------------------------playback------------------------------------------------------------------------------
//------------------------------------------------------------------------------------playback------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
static UI_RecPlayStatus_t tUI_RecPlaySts = UI_RECFILE_PLAY;
static uint16_t uwRecPlayListImgIdx[UI_RECPLAYLISTITEM_MAX] = {
																	OSD2IMG_REC_ADOSRCSELNOR_ICON,
																	OSD2IMG_REC_SKIPBACKWARDNOR_ICON,
																	OSD2IMG_REC_PAUSENOR_ICON,
																	OSD2IMG_REC_SKIPFORWARDNOR_ICON, 
																	OSD2IMG_REC_RETURNNOR_ICON, 
											                      };

void UI_PlaybackSubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	
	OSD_IMG_INFO tRecPlayOsdImgInfo[2];
	tUI_RecPlayListItem.ubItemPreIdx = tUI_RecPlayListItem.ubItemIdx;

	if(UI_SUBSUBSUBMENU_STATE != tUI_State)
		return;
	
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			if(UI_RECADOSRCSEL_ITEM == tUI_RecPlayListItem.ubItemIdx)
				tUI_RecPlayListItem.ubItemIdx = UI_RECRETURN_ITEM;
			else
				tUI_RecPlayListItem.ubItemIdx--;
			break;
		case RIGHT_ARROW:
			if(tUI_RecPlayListItem.ubItemIdx  == UI_RECRETURN_ITEM)
				tUI_RecPlayListItem.ubItemIdx = UI_RECADOSRCSEL_ITEM;
			else
				tUI_RecPlayListItem.ubItemIdx++;
			break;
		case ENTER_ARROW:
			if(UI_RECFILE_STOP == tUI_RecPlayAct.tPlaySts)//��ǰ��ֹͣ����
			{
				if(UI_RECPLAYPAUSE_ITEM == tUI_RecPlayListItem.ubItemIdx)
				{
					uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PAUSENOR_ICON;
					UI_StartPlayRecordFile((UI_RecPlayDispType_t)tUI_RecPlayAct.ubPlayMode);
				}
				else{
					uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PLAYNOR_ICON;
				}
			}
			else//��ǰ���ڲ��Ż�����ͣ
			{
				if(UI_RECPLAYPAUSE_ITEM == tUI_RecPlayListItem.ubItemIdx)//����ֹͣ��ť
				{
					if(UI_RECFILE_PLAY == tUI_RecPlaySts)
					{ 
						if(ubPLY_Pause(PLY_PAUSE_ON) == 0)
	                    	return;
						uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PLAYNOR_ICON;
						tUI_RecPlaySts = UI_RECFILE_PAUSE;
					}
					else if(UI_RECFILE_PAUSE == tUI_RecPlaySts)
					{
						if(ubPLY_Pause(PLY_PAUSE_OFF) == 0)
	                    	return;
						uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PAUSENOR_ICON;
						tUI_RecPlaySts = UI_RECFILE_PLAY;                   
					}
				}
				if(UI_RECFILE_PAUSE != tUI_RecPlaySts)//ֻ���ڲ���״̬�ſ��Խ��н��ȵ��ں���Դѡ��
				{
		            if(UI_RECSKIPBKFWD_ITEM == tUI_RecPlayListItem.ubItemIdx)
					{
						ubPLY_Jump(PLY_JUMP_BWD);
					}
		            else if(UI_RECSKIPFRFWD_ITEM == tUI_RecPlayListItem.ubItemIdx)
					{
						ubPLY_Jump(PLY_JUMP_FWD);
					}
					else if(UI_RECADOSRCSEL_ITEM == tUI_RecPlayListItem.ubItemIdx)
					{
						if(tUI_RecPlayAct.ubPlayMode == UI_RECPLAY_SINGLEVIEW)
							return;
						if(ubPLY_Pause(PLY_PAUSE_ON) == 0)//ֹͣ���ţ�ѡ����Ƶ���ټ�������
	                    	return;
						OSD_IMG_INFO tOsdImgInfo[8];
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELADOCAM1_NOR_ICON, 8, &tOsdImgInfo[0]);
						
						for(uint8_t i = 0;i < 4;i++)
							tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);	
						tOSD_Img2(&tOsdImgInfo[2*tUI_RecPlayAct.tAdoPlayCam + UI_ICON_HIGHLIGHT], OSD_UPDATE);

						tUI_RecPlayAdoSrcItem.ubItemIdx = tUI_RecPlayAct.tAdoPlayCam;
						tUI_State = UI_SUBSUBSUBSUBMENU_STATE;
					}
				}
			}
			UI_DrawHLandNormalIcon(uwRecPlayListImgIdx[tUI_RecPlayListItem.ubItemPreIdx], (uwRecPlayListImgIdx[tUI_RecPlayListItem.ubItemIdx] + UI_ICON_HIGHLIGHT));
			if(tUI_RecPlayListItem.ubItemIdx != UI_RECRETURN_ITEM)//return
				return;
		case EXIT_ARROW:
			tUI_RecPlayAct.tPlaySts = UI_RECFILE_STOP;
			KNL_VideoPlayStop();
			tUI_RecPlaySts      = UI_RECFILE_PLAY;
			tUI_RecPlayListItem.ubItemIdx = UI_RECPLAYPAUSE_ITEM;
			uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PAUSENOR_ICON;
				
			osSemaphoreWait(osUI_CuUiCtr, osWaitForever);
			tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tRecPlayOsdImgInfo[0]);
			tOSD_Img1(&tRecPlayOsdImgInfo[0], OSD_UPDATE);
			if(!ubUI_TouchPanelSts)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tRecPlayOsdImgInfo[0]);
				tOSD_Img2(&tRecPlayOsdImgInfo[0], OSD_QUEUE);
			}
			UI_DrawRecordFileMenu();
			osSemaphoreRelease(osUI_CuUiCtr);
			return;
		case DOWN_ARROW:
			if(UI_RECFILE_PLAY != tUI_RecPlayAct.tPlaySts)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_REC_PLAYHL_ICON, 1, &tRecPlayOsdImgInfo[0]);
				if(UI_RECPLAYPAUSE_ITEM != tUI_RecPlayListItem.ubItemIdx)
				{
					tUI_RecPlayListItem.ubItemPreIdx = tUI_RecPlayListItem.ubItemIdx;
					tUI_RecPlayListItem.ubItemIdx = UI_RECPLAYPAUSE_ITEM;
					uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PLAYNOR_ICON;
					tOSD_Img2(&tRecPlayOsdImgInfo[0], OSD_QUEUE);
					break;
				}
				else
				{
					uwRecPlayListImgIdx[UI_RECPLAYPAUSE_ITEM] = OSD2IMG_REC_PLAYNOR_ICON;
					tOSD_Img2(&tRecPlayOsdImgInfo[0], OSD_UPDATE);
				}
					
			}
		default:
			return;
	}
	UI_DrawHLandNormalIcon(uwRecPlayListImgIdx[tUI_RecPlayListItem.ubItemPreIdx], (uwRecPlayListImgIdx[tUI_RecPlayListItem.ubItemIdx] + UI_ICON_HIGHLIGHT));
}


//------------------------------------------------------------------------------------setting------------------------------------------------------------------------------
//------------------------------------------------------------------------------------setting------------------------------------------------------------------------------
//------------------------------------------------------------------------------------setting------------------------------------------------------------------------------

//------------------------------------------------------------------------------------camera------------------------------------------------------------------------------

//------------------------------------------------------------------------------------

static void UI_CameraSubSubSubMenuExecute()
{
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx;
	uint8_t ubItemIdx = tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;
	uint8_t ubValue;
	UI_CamNum_t tCamNum = (UI_CamNum_t)uwSubSubMenuItemIdx;
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= tCamNum;
	tCamSetCmd.ubCmd[UI_TWC_TYPE]		= UI_SETTING;
	tCamSetCmd.ubCmd[UI_SETTING_ITEM]	= UI_IMGPROC_SETTING;
	tCamSetCmd.ubCmd_Len				 = 4;
	

	if(ubItemIdx == CAMERA_BRIGHTNESS)
	{
		if(tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL + tUI_CuSetting.bColorBLGain[tCamNum] > 99)
			return;
		ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorBL + tUI_CuSetting.bColorBLGain[tCamNum],0,99,0,127);
		tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGBL_SETTING ;
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	}
	else if(ubItemIdx == CAMERA_CONTRAST)
	{
		if(tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast + tUI_CuSetting.bColorContrastGain[tCamNum] < 0)
			return;
		ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorContrast + tUI_CuSetting.bColorContrastGain[tCamNum],0,99,0,127);
		tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGCONTRAST_SETTING ;
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	}
	else if(ubItemIdx == CAMERA_SATURATION)
	{
		if(tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation + tUI_CuSetting.bColorSaturationGain[tCamNum] < 0)
			return;
		ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorSaturation + tUI_CuSetting.bColorSaturationGain[tCamNum],0,99,0,127);
		tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGSATURATION_SETTING ;
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	}
	else if(ubItemIdx == CAMERA_HUE)
	{
		if(tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue + tUI_CuSetting.bColorHueGain[tCamNum] > 99)
			return;
		ubValue = UI_Map(tUI_CamStatus[tCamNum].tCamColorParam.ubColorHue + tUI_CuSetting.bColorHueGain[tCamNum],0,99,0,127);
		tCamSetCmd.ubCmd[UI_SETTING_DATA]	 = UI_IMGHUE_SETTING ;
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = ubValue;
	}
	else if(ubItemIdx == CAMERA_H_MIRROR)
	{
		tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGMIRROR_SETTING;
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgMirror;
	}
	else if(ubItemIdx == CAMERA_V_MIRROR)
	{
		tCamSetCmd.ubCmd[UI_SETTING_DATA]	= UI_IMGFLIP_SETTING;
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamImgFlip;
	}
	else if(ubItemIdx == CAMERA_LASER)  //Alan
	{
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamLaser;
	}
	else if(ubItemIdx == CAMERA_LED)
	{
		tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = tUI_CamStatus[tCamNum].tCamLed;
	}

	if(ubItemIdx != CAMERA_VOLUME)
	{
		if(ubItemIdx != CAMERA_LASER && ubItemIdx != CAMERA_LED)
		{
			//���Ϊ�佹����ͷ������ͷ�ת��ΪISP��ת
//			if(tUI_CuSetting.ubisZoomCameraFlag[tCamNum] && (ubItemIdx == CAMERA_H_MIRROR || ubItemIdx == CAMERA_V_MIRROR) && UI_IsZoomNewVersion(tCamNum))
			if(tUI_CuSetting.ubisZoomCameraFlag[tCamNum] && (tUI_CamStatus[tCamNum].tZoomCamISP || UI_IsZoomNewVersion(tCamNum)) && (ubItemIdx == CAMERA_H_MIRROR || ubItemIdx == CAMERA_V_MIRROR))
			{
				UI_SetZoomMirrorAndFlip(tCamNum);
				return;
			}
			if(UI_SendRequestToCAM(osThreadGetId(), &tCamSetCmd) != rUI_SUCCESS)
			{
				printf("Camera Setting Fail !\n");
				return;
			}
		}
		else if(ubItemIdx == CAMERA_LASER)
		{
			// if(ubKNL_TwcSend((TWC_TAG)tCamNum,TWC_Laser_CTRL,&tCamSetCmd.ubCmd[UI_SETTING_DATA+1],1,10) != rUI_SUCCESS)
			if(UI_SendLaserorLedToCAM(osThreadGetId(), &tCamSetCmd, TWC_Laser_CTRL) != rUI_SUCCESS)
			{
				printf("CAMERA_LASER Setting Fail!!! !\n");
				return;
			}
		}
		else
		{
			// if(ubKNL_TwcSend((TWC_TAG)tCamNum,TWC_Led_CTRL,&tCamSetCmd.ubCmd[UI_SETTING_DATA+1],1,10) != rUI_SUCCESS)
			if(UI_SendLaserorLedToCAM(osThreadGetId(), &tCamSetCmd, TWC_Led_CTRL) != rUI_SUCCESS)
			{
				printf("CAMERA_LED Setting Fail!!! !\n");
				return;
			}
		}

	}
	else
	{
		uint16_t uwValue = (uint16_t)UI_Map(tUI_CamStatus[tCamNum].ubVolumeLvl,0,99,VOLUME_PWM_MIN,VOLUME_PWM_MAX);
		SET_VOLUME_PWM((uwValue == VOLUME_PWM_MIN)?0:uwValue);
	}
}

//----------------------------------------------------------------------

static void UI_CameraDrawSubSubSubMenuItem()
{
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx;
	uint8_t ubItemPreIdx = tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemCurIdx = tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;
	uint8_t *pUI_ColorParm[5] = {
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorBL,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorContrast,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorSaturation,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorHue,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].ubVolumeLvl
								 };
	OSD_IMG_INFO tOsdImgInfo[16];
	//-----------------------------------------preIdx------------------------------------------
	if(ubItemPreIdx != ubItemCurIdx)//ֻ�е��ؼ�������ת����Ҫ��ǰһ���ؼ��ָ�Ϊnormal״̬
	{
		if(ubItemPreIdx <= CAMERA_VOLUME)
		{
			//�Ȳ���ѡ�п�
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_BRIGHTNESS_GREEN_HL, 1, &tOsdImgInfo[0]);
			tOsdImgInfo[0].uwYStart = CAMERA_HL_Y + CAMERA_Y_STEP*ubItemPreIdx;
			OSD_EraserImg2_NoUpdate(&tOsdImgInfo[0]);
			//word
			tOSD_GetOsdImgInfor(1, OSD_IMG2, ((tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_CAMS_BRIGHTNESS_WORD:
											(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_CAMS_BRIGHTNESS_WORD_GER:
											(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_CAMS_BRIGHTNESS_WORD_FR:OSD2IMG_CAMS_BRIGHTNESS_WORD_CHN) + ubItemPreIdx, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
			//slider
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_BRIGHTNESS_SLIDER, 1, &tOsdImgInfo[0]);
			tOsdImgInfo[0].uwYStart = CAMERA_SLIDER_Y + CAMERA_Y_STEP*ubItemPreIdx;
			tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
			//round
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_ROUND_HL, 1, &tOsdImgInfo[1]);
			tOsdImgInfo[1].uwXStart = UI_Map(*pUI_ColorParm[ubItemPreIdx],0,99,
											tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
			tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart;
			tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
			//��ֵ
			if(*pUI_ColorParm[ubItemPreIdx] < 10)
			{
				UI_ShowButtonValueHighLight(0,tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_QUEUE);
				UI_ShowButtonValueHighLight(*pUI_ColorParm[ubItemPreIdx],tOsdImgInfo[1].uwXStart + 24,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
			}
			else
				UI_ShowButtonValueHighLight(*pUI_ColorParm[ubItemPreIdx],tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
				
		}
		else
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_H_MIRROR_OFF_NOR, 16, &tOsdImgInfo[0]);
			if(ubItemPreIdx == CAMERA_H_MIRROR)
			{
				if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgMirror)
					tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
				else
					tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
			}
			else if(ubItemPreIdx == CAMERA_V_MIRROR)
			{
				if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgFlip)
					tOSD_Img2(&tOsdImgInfo[4 + 2], OSD_UPDATE);
				else
					tOSD_Img2(&tOsdImgInfo[4 + 0], OSD_UPDATE);
			}
			else if(ubItemPreIdx == CAMERA_LASER)
			{
				if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamLaser && 
					( (UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3)) ||
					(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3)) )
				)
					tOSD_Img2(&tOsdImgInfo[8 + 2], OSD_UPDATE);
				else
					tOSD_Img2(&tOsdImgInfo[8 + 0], OSD_UPDATE);
			}
			else
			{
				if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamLed && 
					( (UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3)) ||
					(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3)) )
				)
					tOSD_Img2(&tOsdImgInfo[12 + 2], OSD_UPDATE);
				else
					tOSD_Img2(&tOsdImgInfo[12 + 0], OSD_UPDATE);
			}
		}
	}
	//--------------------------------CurIdx----------------------------------------------------
	if(ubItemCurIdx <= CAMERA_VOLUME)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_BRIGHTNESS_GREEN_HL + CameraSubSubSubRemoteMode, 1, &tOsdImgInfo[0]);
		tOsdImgInfo[0].uwYStart = CAMERA_HL_Y + CAMERA_Y_STEP*ubItemCurIdx;
		tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
		//word
		tOSD_GetOsdImgInfor(1, OSD_IMG2, ((tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_CAMS_BRIGHTNESS_WORD:
										(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_CAMS_BRIGHTNESS_WORD_GER:
										(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_CAMS_BRIGHTNESS_WORD_FR:OSD2IMG_CAMS_BRIGHTNESS_WORD_CHN) + ubItemCurIdx, 1, &tOsdImgInfo[0]);
		tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
		//slider
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_BRIGHTNESS_SLIDER, 1, &tOsdImgInfo[0]);
		tOsdImgInfo[0].uwYStart = CAMERA_SLIDER_Y + CAMERA_Y_STEP*ubItemCurIdx;
		tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
		//round
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_ROUND_HL, 1, &tOsdImgInfo[1]);
		tOsdImgInfo[1].uwXStart = UI_Map(*pUI_ColorParm[ubItemCurIdx],0,99,
										tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart;
		tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
		//��ֵ
		if(*pUI_ColorParm[ubItemCurIdx] < 10)
		{
			UI_ShowButtonValueHighLight(0,tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_QUEUE);
			UI_ShowButtonValueHighLight(*pUI_ColorParm[ubItemCurIdx],tOsdImgInfo[1].uwXStart + 24,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
		}
		else
			UI_ShowButtonValueHighLight(*pUI_ColorParm[ubItemCurIdx],tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_H_MIRROR_OFF_NOR, 16, &tOsdImgInfo[0]);
		if(ubItemCurIdx == CAMERA_H_MIRROR)
		{
			if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgMirror)
				tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
			else
				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
		}
		else if(ubItemCurIdx == CAMERA_V_MIRROR)
		{
			if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgFlip)
				tOSD_Img2(&tOsdImgInfo[4 + 3], OSD_UPDATE);
			else
				tOSD_Img2(&tOsdImgInfo[4 + 1], OSD_UPDATE);
		}
		else if(ubItemCurIdx == CAMERA_LASER)
		{
			if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamLaser && 
				( (UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3)) ||
				(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3)) )
			)
				tOSD_Img2(&tOsdImgInfo[8 + 3], OSD_UPDATE);
			else
				tOSD_Img2(&tOsdImgInfo[8 + 1], OSD_UPDATE);
		}
		else
		{
			if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamLed && 
				( (UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3)) ||
				(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3)) )
			)
				tOSD_Img2(&tOsdImgInfo[12 + 3], OSD_UPDATE);
			else
				tOSD_Img2(&tOsdImgInfo[12 + 1], OSD_UPDATE);
		}
	}

}
//----------------------------------------------------------------------
void UI_CameraSubSubSubMenuExitToSubSub(uint16_t uwSubSubMenuItemIdx)
{
	OSD_IMG_INFO tOsdImgInfo;

	tOSD_GetOsdImgInfor(1, OSD_IMG2, ((tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_CAMSCAM1_SELECTED_HL_ICON:OSD2IMG_CAMSCAM1_SELECTED_HL_ICON_GER) + 2*uwSubSubMenuItemIdx, 1, &tOsdImgInfo);
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	tUI_State = UI_SUBSUBMENU_STATE;
	tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = CAMERA_BRIGHTNESS;
	ubisCameraConfigFlag = FALSE;
	for(uint8_t i = 0;i < 4;i++)
		UI_UpdateTxDevStatusInfo((UI_CamNum_t)i);
}
//----------------------------------------------------------------------
void UI_CameraSubSubSubMenuExitToSub()
{
	OSD_IMG_INFO tOsdImgInfo;
	UI_CamNum_t tCamNum = tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx;
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
	tOSD_Img1(&tOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}
	OSD_IMG_INFO tOsdImgInfosub[22];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_CAMERA_SETTING_WORD:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_CAMERA_SETTING_WORD_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_CAMERA_SETTING_WORD_FR:OSD2IMG_CAMERA_SETTING_WORD_CHN, SETTINGITEM_MAX, &tOsdImgInfosub[0]);
	//WORD
	for(uint8_t i = 0;i < SETTINGITEM_MAX;i++)
		tOSD_Img2(&tOsdImgInfosub[i], OSD_QUEUE);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMERA_SETTING_NOR_ICON, SETTINGITEM_MAX*2, &tOsdImgInfosub[0]);

	for(uint16_t i = 0;i < SETTINGITEM_MAX;i++)
		tOSD_Img2(&tOsdImgInfosub[2*i], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfosub[2*CAMERASET_ITEM + UI_ICON_HIGHLIGHT], OSD_UPDATE);//highlight
	
	memset(&tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	ubUI_SubSubStsUpdateFlag = FALSE;
	for(uint8_t i = 0;i < 4;i++)
		tCameraSubSubSubMenuItem[i]->tSubMenuInfo.ubItemIdx = CAMERA_BRIGHTNESS;
	tUI_State = UI_SUBMENU_STATE;

	ubisCameraConfigFlag = FALSE;
	UI_UpdateDevStatusInfo();
	UI_UpdateTxDevStatusInfo(tCamNum);
			
	UI_SwitchViewType(DeskTopShowView,FALSE);
}
//----------------------------------------------------------------------

void UI_CameraSubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx;
	
	uint8_t *pUI_ColorParm[5] = {
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorBL,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorContrast,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorSaturation,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorHue,
								(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].ubVolumeLvl
								 };

	tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx = 
		tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;

	
	if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamConnSts == CAM_ONLINE || tArrowKey == EXIT_ARROW)
	{
		//��ǰ�������ĸ�����ɫΪ��ɫ��ң�ص����Ҽ����л���ͬ��ѡ��
		if(CameraSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
		{
				switch(tArrowKey)
				{
					case LEFT_ARROW:
						if(tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx ==
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->ubFirstItem)
						{
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = 
								tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->ubItemCount - 1;
						}
						else
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx --;
						break;
					case RIGHT_ARROW:
						if(tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx == 
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->ubItemCount - 1)
						{
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = 
								tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->ubFirstItem;
						}
						else
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx ++;
						break;
					case ENTER_ARROW:
						{	
							uint8_t ubItemCurIdx = tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;
							if(tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx <= CAMERA_VOLUME)//GREEN->RED
							{
								SendIrCodeFlag = 1;//֧������
								CameraSubSubSubRemoteMode = 1- CameraSubSubSubRemoteMode;
								UI_CameraDrawSubSubSubMenuItem();
							}
							else//mirror flip
							{
								if(ubItemCurIdx == CAMERA_H_MIRROR)
									tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgMirror = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgMirror;
								else if(ubItemCurIdx == CAMERA_V_MIRROR)
									tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgFlip = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgFlip;
								else if(ubItemCurIdx == CAMERA_LASER)
								{
									if(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3) ||
										UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3))
										tUI_CamStatus[uwSubSubMenuItemIdx].tCamLaser = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamLaser;
								}
								else
								{
									if(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3) ||
										UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3))
										tUI_CamStatus[uwSubSubMenuItemIdx].tCamLed = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamLed;
								}
								UI_CameraDrawSubSubSubMenuItem();
								UI_CameraSubSubSubMenuExecute();

							}
							
						}
						return;
					case EXIT_ARROW:
						UI_CameraSubSubSubMenuExitToSubSub(uwSubSubMenuItemIdx);
						return;
					default:
						return;
				}
			
			
			UI_CameraDrawSubSubSubMenuItem();
		}//�������ĸ�����Ϊ��ɫ�����Ҽ����Ե��ڽ���������ֵ
		else
		{	
			uint8_t ubItemCurIdx = tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;
			switch(tArrowKey)
			{
				case LEFT_ARROW:
					if(*pUI_ColorParm[ubItemCurIdx] > 0)
						(*pUI_ColorParm[ubItemCurIdx]) --;
					else
						return;
					break;
				case RIGHT_ARROW:
					if(*pUI_ColorParm[ubItemCurIdx] < 99)
						(*pUI_ColorParm[ubItemCurIdx]) ++;
					else
						return;
					break;
				case ENTER_ARROW:
					SendIrCodeFlag = 0;//��ֹ����
					CameraSubSubSubRemoteMode = 1 - CameraSubSubSubRemoteMode;
					UI_CameraDrawSubSubSubMenuItem();
					return;
				case EXIT_ARROW:
					UI_CameraSubSubSubMenuExitToSubSub(uwSubSubMenuItemIdx);
					return;
				default:
					return;
			}
			UI_CameraDrawSubSubSubMenuItem();
			UI_CameraSubSubSubMenuExecute();
		}
	}
}

//------------------------------------------------------------------------------------system------------------------------------------------------------------------------

static void UI_SystemSubSubSubMenuExit(void)
{
	OSD_IMG_INFO tMenuOsdImgInfo,tOsdImgInfo[10];
	uint8_t ubSubSubMenuItem = tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx;
		
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SYSTEM_TITLE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SYSTEM_TITLE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SYSTEM_TITLE_FR:OSD2IMG_SYSTEM_TITLE_CHN, 1, &tOsdImgInfo[0]);
	//����
	for(uint8_t i = 0;i < 1;i ++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);

	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	
	//���黬��
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DIMMER_SLIDER, 6, &tOsdImgInfo[0]);
	
	//����
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	//����
	
	tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,0,99,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
	tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	//��ֵ
	if(tUI_CuSetting.BriLvL.ulBL_UpdateLvL < 10)
	{
		UI_ShowButtonValueHighLight(0,tOsdImgInfo[2].uwXStart + 4,tOsdImgInfo[2].uwYStart + 9,OSD_QUEUE);
		UI_ShowButtonValueHighLight(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,tOsdImgInfo[2].uwXStart + 24,tOsdImgInfo[2].uwYStart + 9,OSD_QUEUE);
	}
	else
		UI_ShowButtonValueHighLight(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,tOsdImgInfo[2].uwXStart + 4,tOsdImgInfo[2].uwYStart + 9,OSD_QUEUE);
			
	//auto dimmer
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 4, &tOsdImgInfo[0]);
	if(tUI_CuSetting.ubAutoDimmer)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	if(ubSubSubMenuItem == AUTO_SET_ITEM)
		tOSD_GetOsdImgInfor(1, OSD_IMG2,OSD2IMG_SYSTEM_AUTO_SETTING_HL, 1, &tOsdImgInfo[0]);
	else
		tOSD_GetOsdImgInfor(1, OSD_IMG2,OSD2IMG_SYSTEM_AUTO_SETTING_NOR, 1, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	
	//menu lock
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_MENULOCK_OFF_NOR, 4, &tOsdImgInfo[0]);
	if(tUI_CuSetting.ubMenuLock)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	//buzzer
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_BUZZER_OFF_NOR, 4, &tOsdImgInfo[0]);
	if(tUI_CuSetting.ubBuzzer)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	//upgrade
	if(ubSubSubMenuItem == UPGRADE_ITEM)
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_UPGRADE_HL, 1, &tOsdImgInfo[0]);
	else
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_UPGRADE_NOR, 1, &tOsdImgInfo[0]);
    tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	//language
	if(ubSubSubMenuItem == LANGUAGE_ITEM)
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_LANGUAGE_HL, 1, &tOsdImgInfo[0]);
	else
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_LANGUAGE_NOR, 1, &tOsdImgInfo[0]);
    tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	// reset
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_RESET_NOR, 1, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	//key lock
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_KEYLOCK_OFF_NOR, 4, &tOsdImgInfo[0]);
	if(tUI_CuSetting.ubKeyLock)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	//SD Switch
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_SD_SWITCH_OFF_NOR, 4, &tOsdImgInfo[0]);
	if(tUI_CuSetting.ubSD_Switch)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	//hide channel name
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_HIDE_CHANNEL_NAME_OFF_NOR, 4, &tOsdImgInfo[0]);
	if(tUI_CuSetting.ubHideChannelName)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	// date & time
	if(ubSubSubMenuItem == DATETIME_ITEM)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DATE_TIME_HL, 1, &tOsdImgInfo[0]);
	}
	else
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DATE_TIME_NOR, 1, &tOsdImgInfo[0]);
	}
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	OSD_ImagePrintf(OSD_IMG_ROTATION_0, 450, 570, tUI_CharOsdImgInfo, OSD_QUEUE, SOFEWARE_VERSION);

	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	
	OSD_UpdateQueueBuf();
	tUI_State = UI_SUBSUBMENU_STATE;

}
//------------------------------------------------------------------------------------
static void UI_DrawDateTimeYearButtonItem(bool isButtonHL, uint8_t DateTimeYearButtonIndex)
{
	OSD_IMG_INFO tOsdImgInfo[2];
    
	RTC_GetCalendar((RTC_Calendar_t *)(&(tUI_CuSetting.tSysCalendar)));
	uint16_t uwTemp[6];
	uwTemp[0] = tUI_CuSetting.tSysCalendar.uwYear;
	uwTemp[1] = tUI_CuSetting.tSysCalendar.ubMonth;
	uwTemp[2] = tUI_CuSetting.tSysCalendar.ubDate;
	uwTemp[3] = tUI_CuSetting.tSysCalendar.ubHour;
	uwTemp[4] = tUI_CuSetting.tSysCalendar.ubMin;
	uwTemp[5] = tUI_CuSetting.tSysCalendar.ubSec; 

//	printf("UI_DrawDateTimeYearButtonItem isButtonHL = %d DateTimeYearButtonIndex = %d\n", isButtonHL, DateTimeYearButtonIndex);
	
	//BUTTON
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_YEAR_NOR, 2, &tOsdImgInfo[0]);
	if(isButtonHL)
	{
		//HIGHLIGHT BUTTON
		tOsdImgInfo[1].uwXStart +=  (DateTimeYearButtonIndex % 3) * DATE_TIME_YEAR_DIST_X;
		if(DateTimeYearButtonIndex > 2)
		{
			tOsdImgInfo[1].uwYStart += DATE_TIME_YEAR_DIST_Y;
		}
		tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);

//		printf("tOsdImgInfo[1].uwXStart = %d, uwYStart = %d\n", tOsdImgInfo[1].uwXStart, tOsdImgInfo[1].uwYStart);

		if(uwTemp[DateTimeYearButtonIndex] < 10)
		{
			UI_ShowButtonValueHighLight(0,tOsdImgInfo[1].uwXStart + 30,tOsdImgInfo[1].uwYStart + 15,OSD_QUEUE);
			UI_ShowButtonValueHighLight(uwTemp[DateTimeYearButtonIndex],tOsdImgInfo[1].uwXStart + 50,tOsdImgInfo[1].uwYStart + 15,OSD_UPDATE);
		}
		else if(uwTemp[DateTimeYearButtonIndex] < 100)
			UI_ShowButtonValueHighLight(uwTemp[DateTimeYearButtonIndex],tOsdImgInfo[1].uwXStart + 30,tOsdImgInfo[1].uwYStart + 15,OSD_UPDATE);
		else
			UI_ShowButtonValueHighLight(tUI_CuSetting.tSysCalendar.uwYear,tOsdImgInfo[1].uwXStart + 10,tOsdImgInfo[1].uwYStart + 15,OSD_UPDATE);
	}
	else
	{
		tOsdImgInfo[0].uwXStart +=  (DateTimeYearButtonIndex % 3) * DATE_TIME_YEAR_DIST_X;
		if(DateTimeYearButtonIndex > 2)
		{
			tOsdImgInfo[0].uwYStart += DATE_TIME_YEAR_DIST_Y;
		}
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

//		printf("tOsdImgInfo[1].uwXStart = %d, uwYStart = %d\n", tOsdImgInfo[0].uwXStart, tOsdImgInfo[0].uwYStart);
		if(uwTemp[DateTimeYearButtonIndex] < 10)
		{
			UI_ShowButtonValueNormal(0,tOsdImgInfo[0].uwXStart + 30,tOsdImgInfo[0].uwYStart + 15,OSD_QUEUE);
			UI_ShowButtonValueNormal(uwTemp[DateTimeYearButtonIndex],tOsdImgInfo[0].uwXStart + 50,tOsdImgInfo[0].uwYStart + 15,OSD_QUEUE);
		}
		else if(uwTemp[DateTimeYearButtonIndex] < 100)
			UI_ShowButtonValueNormal(uwTemp[DateTimeYearButtonIndex],tOsdImgInfo[0].uwXStart + 30,tOsdImgInfo[0].uwYStart + 15,OSD_QUEUE);
		else
			UI_ShowButtonValueNormal(tUI_CuSetting.tSysCalendar.uwYear,tOsdImgInfo[0].uwXStart + 10,tOsdImgInfo[0].uwYStart + 15,OSD_QUEUE);
	}
}


static void UI_SystemDrawSubSubSubMenuItem(uint8_t ubDrawBox)
{
	uint8_t ubSubSubMenuItem = tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx;
	uint8_t ubItemPreIdx = tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemCurIdx = tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx;
	OSD_IMG_INFO tOsdImgInfo[8];
	uint16_t uwSettingItemOsdImg[16] = {0};
	switch(ubSubSubMenuItem)
	{
		case AUTO_SET_ITEM:
			uint8_t *pUI_TempVal[2] = {
										&tUI_CuSetting.ubAutoDimmer_MaxVal,
										&tUI_CuSetting.ubAutoDimmer_MinVal
										};
			//-----------------------------------------preIdx------------------------------------------
			if(ubItemPreIdx != ubItemCurIdx)//ֻ�е��ؼ�������ת����Ҫ��ǰһ���ؼ��ָ�Ϊnormal״̬
			{
				//�Ȳ���ѡ�п�
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_GREEN_HL + 2*ubItemPreIdx, 1, &tOsdImgInfo[0]);
				OSD_EraserImg2_NoUpdate(&tOsdImgInfo[0]);
			
				//��ʾ����
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_SLIDER + ubItemPreIdx, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
				//��ʾ����
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_ROUND_HL + ubItemPreIdx, 1, &tOsdImgInfo[1]);
				
				tOsdImgInfo[1].uwXStart = UI_Map(*pUI_TempVal[ubItemPreIdx],0,99,
					tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);	
				tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
				//��ֵ
				if(*pUI_TempVal[ubItemPreIdx] < 10)
				{
					UI_ShowButtonValueHighLight(0,tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_QUEUE);
					UI_ShowButtonValueHighLight(*pUI_TempVal[ubItemPreIdx],tOsdImgInfo[1].uwXStart + 24,tOsdImgInfo[1].uwYStart + 9,OSD_QUEUE);
				}
				else
					UI_ShowButtonValueHighLight(*pUI_TempVal[ubItemPreIdx],tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
						
			
			}
			//--------------------------------CurIdx----------------------------------------------------
			//��ʾ��
			if(ubDrawBox)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_GREEN_HL + 2*ubItemCurIdx + SystemSubSubSubRemoteMode, 1, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
			}
			//slider
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_SLIDER + ubItemCurIdx, 1, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0],OSD_QUEUE);
			//round
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_ROUND_HL + ubItemCurIdx, 1, &tOsdImgInfo[1]);
			tOsdImgInfo[1].uwXStart = UI_Map(*pUI_TempVal[ubItemCurIdx],0,99,
											tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
			tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
			//��ֵ
			if(*pUI_TempVal[ubItemCurIdx] < 10)
			{
				UI_ShowButtonValueHighLight(0,tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_QUEUE);
				UI_ShowButtonValueHighLight(*pUI_TempVal[ubItemCurIdx],tOsdImgInfo[1].uwXStart + 24,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
			}
			else
				UI_ShowButtonValueHighLight(*pUI_TempVal[ubItemCurIdx],tOsdImgInfo[1].uwXStart + 4,tOsdImgInfo[1].uwYStart + 9,OSD_UPDATE);
			break;
			
		case UPGRADE_ITEM:
			for(uint8_t i = 0;i < 2;i ++)
			{
				uwSettingItemOsdImg[i] = ((tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_YES_NOR:
										(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_YES_NOR_GER:
										(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_YES_NOR_FR:OSD2IMG_SDCARD_YES_NOR_CHN) + 2*i;
			}
			UI_DrawHLandNormalIcon(uwSettingItemOsdImg[ubItemPreIdx],(uwSettingItemOsdImg[ubItemCurIdx] + UI_ICON_HIGHLIGHT));
			break;	
			
		case LANGUAGE_ITEM:
			for(uint8_t i = 0;i < LANGUAGE_MAX;i ++)
			{
				uwSettingItemOsdImg[i] = OSD2IMG_SYSTEM_ENGLISH_NOR + 2*i;
				if(tUI_CuSetting.tLanguage == i)
					uwSettingItemOsdImg[i] = OSD2IMG_SYSTEM_ENGLISH_SELECT_NOR + 2*i;
		
			}
			UI_DrawHLandNormalIcon(uwSettingItemOsdImg[ubItemPreIdx],(uwSettingItemOsdImg[ubItemCurIdx] + UI_ICON_HIGHLIGHT));
			break;
		case DATETIME_ITEM:
			//pre
			if(ubItemPreIdx <= DATE_TIME_SECOND)
			{
				UI_DrawDateTimeYearButtonItem(FALSE, ubItemPreIdx);
			}
			else if(ubItemPreIdx == DATE_TIME_UTC_SELECT)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 6, &tOsdImgInfo[0]);
				if(SystemSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
				{
	                if (ubItemCurIdx != DATE_TIME_UTC_SELECT)
	                {
						OSD_EraserImg2_NoUpdate(&tOsdImgInfo[4]);
					}
				}
				else
				{
					if (ubItemCurIdx != DATE_TIME_UTC_SELECT)
	                {
						OSD_EraserImg2_NoUpdate(&tOsdImgInfo[4]);
					}
					else
					{
						tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);
					}
				}

				tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

				tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.ubtimezone,0,26,
					tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
				tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
				UI_ShowUTCVal();

			}
			else if(ubItemPreIdx == DATE_TIME_UTC_COMFIRM)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 2, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
			}

			//cur
			if(ubItemCurIdx <= DATE_TIME_SECOND)
			{
				UI_DrawDateTimeYearButtonItem(TRUE, ubItemCurIdx);
			}
			else if(ubItemCurIdx == DATE_TIME_UTC_SELECT)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 6, &tOsdImgInfo[0]);
				if (SystemSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
					tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
				else
					tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);
				tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

				tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.ubtimezone,0,26,
					tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
				tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
				UI_ShowUTCVal();
			}
			else if(ubItemCurIdx == DATE_TIME_UTC_COMFIRM)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 2, &tOsdImgInfo[0]);
				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);

			}
			break;
		default:
			break;
	}
	

}
//------------------------------------------------------------------------------------
static uint8_t ubtimezonePre;//�����޸�֮ǰ��ʱ���
static uint8_t ubtimezoneFlag;//�����޸�֮ǰʱ����ı�־
void UI_SystemSubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	
	uint8_t ubSubSubMenuItem = tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx;
	tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemPreIdx = 
		tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx;

	//��ǰ�������ĸ�����ɫΪ��ɫ��ң�ص����Ҽ����л���ͬ��ѡ��
	if(SystemSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
	{
		switch(tArrowKey)
		{
			case LEFT_ARROW:
				if(tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx ==
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].ubFirstItem)
				{
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx = 
						tSystemSubSubSubMenuItem[ubSubSubMenuItem].ubItemCount - 1;
				}
				else
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx --;
				break;
			case RIGHT_ARROW:
				if(tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx ==
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].ubItemCount - 1)
				{
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx = 
						tSystemSubSubSubMenuItem[ubSubSubMenuItem].ubFirstItem;
				}
				else
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx ++;
				break;
			case ENTER_ARROW:
				if(ubSubSubMenuItem == AUTO_SET_ITEM)
				{
					SendIrCodeFlag = 1;//֧������
					SystemSubSubSubRemoteMode = 1- SystemSubSubSubRemoteMode;
					UI_SystemDrawSubSubSubMenuItem(TRUE);
					return;
				}
				else if(ubSubSubMenuItem == LANGUAGE_ITEM)
				{
					if(tUI_CuSetting.tLanguage != (UI_Language_t)tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx)
					{
						tUI_CuSetting.tLanguage = (UI_Language_t)tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx;
							//���������޸�ʱ��
						if(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)//����
							tUI_CuSetting.ubtimezone = 6;
						else if(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)//��һ
							tUI_CuSetting.ubtimezone = 14;
						else if(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)//��һ
							tUI_CuSetting.ubtimezone = 14;
						else if(tUI_CuSetting.tLanguage == LANGUAGE_CHINESE)//����
							tUI_CuSetting.ubtimezone = 21;
						
						UI_UpdateDevStatusInfo();
						SYS_Reboot();
					}
					else
					{
						UI_SystemDrawSubSubSubMenuItem(TRUE);
						osDelay(100);
					}

				}
				else if(ubSubSubMenuItem == UPGRADE_ITEM)
				{
					if(tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx == SYSTEM_UPGRADE_YES)
					{
						UI_DisconnectTx();
						osDelay(200);
						UI_ClearOsdImage();
						MenuOnFlag = FALSE;
						SendIrCodeFlag = FALSE;
						tUI_State = UI_DISPLAY_STATE;

						UI_Event_t tSdFwUpgEvent;
						osMessageQId *pUI_EventQH = NULL;

						tSdFwUpgEvent.tEventType = FWUPG_EVENT;
						tSdFwUpgEvent.pvEvent  	 = NULL;
						pUI_EventQH 	  	 	 = pUI_GetEventQueueHandle();
						osMessagePut(*pUI_EventQH, &tSdFwUpgEvent, 0);
						
						return;
					}
				}
				else if (ubSubSubMenuItem == DATETIME_ITEM)
				{
					uint8_t uwsubsubsubItemIdx = tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx;
					OSD_IMG_INFO tOsdImgInfo[25],tMenuOsdImgInfo;

					if(ubtimezoneFlag==0)
					{
						ubtimezonePre =  tUI_CuSetting.ubtimezone;
						ubtimezoneFlag=1;
					}
					tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);

			        if(uwsubsubsubItemIdx != DATE_TIME_UTC_SELECT)
			        {
						tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
					}

					if(!ubUI_TouchPanelSts)
					{
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
						tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
					}
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_BACKGROUND, 25, &tOsdImgInfo[0]);
					
					if(uwsubsubsubItemIdx < DATE_TIME_UTC_SELECT)
					{
						tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
						for(uint8_t i = 0;i < 12;i++)
							tOSD_Img2(&tOsdImgInfo[1 + 2*i], OSD_QUEUE);
						tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
					}

					if(uwsubsubsubItemIdx == DATE_TIME_YEAR)
					{
						UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.uwYear,OSD_UPDATE);
					}
					else if((uwsubsubsubItemIdx == DATE_TIME_MONTH))
					{
						UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubMonth,OSD_UPDATE);
					}
					else if((uwsubsubsubItemIdx == DATE_TIME_DAY))
					{
						UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubDate,OSD_UPDATE);
					}
					else if((uwsubsubsubItemIdx == DATE_TIME_HOUR))
					{
						UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubHour,OSD_UPDATE);
					}
					else if((uwsubsubsubItemIdx == DATE_TIME_MINUTE))
					{
						RTC_GetCalendar((RTC_Calendar_t *)(&(tUI_CuSetting.tSysCalendar)));
						UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubMin,OSD_UPDATE);	
					}
					else if((uwsubsubsubItemIdx == DATE_TIME_SECOND))
					{
						UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubSec,OSD_UPDATE);
					}
					else if((uwsubsubsubItemIdx == DATE_TIME_UTC_SELECT))
					{
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 6, &tOsdImgInfo[0]);
			            //SettingSubSubSubRemoteMode = 1 - SettingSubSubSubRemoteMode;
						SystemSubSubSubRemoteMode = 1- SystemSubSubSubRemoteMode;
			            if(SystemSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
			            	tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
						else
							tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);
						tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
						tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.ubtimezone,0,26,
							tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
						tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
						
						return;
					}
					else if(uwsubsubsubItemIdx == DATE_TIME_UTC_COMFIRM)
					{
			            tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 2, &tOsdImgInfo[0]);
				   	    tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
						ubtimezoneFlag = 0;
			            UI_UpdateDevStatusInfo();
						SYS_Reboot();
						return;
					}
					ubUI_DateTimeSubSubSubSubStsUpdateFlag = FALSE;
					tUI_State = UI_SUBSUBSUBSUBMENU_STATE;
					return;
				}
			case EXIT_ARROW:
				if(ubtimezoneFlag==1)
				{
					tUI_CuSetting.ubtimezone = ubtimezonePre;
					ubtimezoneFlag = 0;
				}
				UI_SystemSubSubSubMenuExit();
				return;
			default:
				return;
		}
		
		UI_SystemDrawSubSubSubMenuItem(TRUE);
	}//�������ĸ�����Ϊ��ɫ�����Ҽ����Ե��ڽ���������ֵ
	else
	{	
		uint8_t ubItemCurIdx = tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx;
		if(ubtimezoneFlag==0)
		{
			ubtimezonePre =  tUI_CuSetting.ubtimezone;
			ubtimezoneFlag=1;
		}
		switch(tArrowKey)
		{
			case LEFT_ARROW:
				if (tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx == AUTO_SET_ITEM)	
				{
					if(ubItemCurIdx == DAY_SLIDER)
					{
						if(tUI_CuSetting.ubAutoDimmer_MaxVal > 0 && tUI_CuSetting.ubAutoDimmer_MaxVal > tUI_CuSetting.ubAutoDimmer_MinVal)
							tUI_CuSetting.ubAutoDimmer_MaxVal --;
						else 
							return;
					}
					else if (ubItemCurIdx == NIGHT_SLIDER)
					{
						if(tUI_CuSetting.ubAutoDimmer_MinVal > 0)
							tUI_CuSetting.ubAutoDimmer_MinVal --;
						else 
							return;
					}
				}
				else if (tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx == DATETIME_ITEM)
				{
					if(ubItemCurIdx == DATE_TIME_UTC_SELECT)
					{
						if (tUI_CuSetting.ubtimezone > 0)
						{
							tUI_CuSetting.ubtimezone--;
						}
						else
						{
							tUI_CuSetting.ubtimezone = 26;
						}
					}
				}
				break;
			case RIGHT_ARROW:
				if (tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx == AUTO_SET_ITEM)	
				{
					if(ubItemCurIdx == DAY_SLIDER)
					{
						if(tUI_CuSetting.ubAutoDimmer_MaxVal < 99)
							tUI_CuSetting.ubAutoDimmer_MaxVal ++;
						else 
							return;
					}
					else if (ubItemCurIdx == NIGHT_SLIDER)
					{
						if(tUI_CuSetting.ubAutoDimmer_MinVal < 99 && tUI_CuSetting.ubAutoDimmer_MinVal < tUI_CuSetting.ubAutoDimmer_MaxVal)
							tUI_CuSetting.ubAutoDimmer_MinVal ++;
						else 
							return;
					}
				}
				else if (tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx == DATETIME_ITEM)
				{
					if(ubItemCurIdx == DATE_TIME_UTC_SELECT)
					{
						if (tUI_CuSetting.ubtimezone < 26)
						{
							tUI_CuSetting.ubtimezone++;
						}
						else
						{
							tUI_CuSetting.ubtimezone = 0;
						}
					}
				}
				break;
			case ENTER_ARROW:
				SendIrCodeFlag = 0;//��ֹ����
				SystemSubSubSubRemoteMode = 1 - SystemSubSubSubRemoteMode;
				UI_SystemDrawSubSubSubMenuItem(TRUE);
				return;
			case EXIT_ARROW:
				if(ubtimezoneFlag==1)
				{
					tUI_CuSetting.ubtimezone = ubtimezonePre;
					ubtimezoneFlag = 0;
				}
				UI_SystemSubSubSubMenuExit();
				return;
			default:
				return;
		}
		UI_SystemDrawSubSubSubMenuItem(FALSE);
	}


}



//------------------------------------------------------------------------------------parkingLine------------------------------------------------------------------------------
#define LEFT_RIGHT_MODE 0
#define UP_DOWN_MODE    1
#define SIZE_MODE       2

uint8_t ParkingLineRemoteMode = LEFT_RIGHT_MODE;


void UI_ParkingLineSubSubSubMenuPageExit()
{
	OSD_IMG_INFO tOsdImgInfo[24],tMenuOsdImgInfo;
	uint16_t uwSubSubMenuItemIdx = 
			(uint16_t)tSettingSubSubMenuItem.tSettingS[PARKINGLINE_ITEM].tSubMenuInfo.ubItemIdx;//sub sub

	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	
	//WORD
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_PARKINGLINE_TITLE:OSD2IMG_PARKINGLINE_TITLE_GER, 5, &tOsdImgInfo[0]);
	if(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_TITLE_FR, 1, &tOsdImgInfo[0]);
	else if(tUI_CuSetting.tLanguage == LANGUAGE_CHINESE)
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_TITLE_CHN, 1, &tOsdImgInfo[0]);
	
	for(uint8_t i = 0;i < 5;i++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);
	
	//enablr or disable
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_OFF_CAM1_NOR, 4, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
	{
		for(uint8_t j = 0;j < 4;j++)
		{
			tOsdImgInfo[6*i + j] = tOsdImgInfo[j];
			tOsdImgInfo[6*i + j].uwXStart = PARKINGLINE_SWITCH_X + PARKINGLINE_X_STEP*(i%2);
			tOsdImgInfo[6*i + j].uwYStart = PARKINGLINE_SWITCH_Y + PARKINGLINE_Y_STEP*(i/2);
		}
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_CAM1_AUTO_NOR + 2*i, 2, &tOsdImgInfo[6*i + 4]);
	}
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[6*i + 2*tUI_CuSetting.tParkingLineEnable[i]], OSD_QUEUE); 
	//CALIBRATE
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_CAM1_CAL_NOR, 8, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
	//high light
	tOSD_Img2(&tOsdImgInfo[2*(uwSubSubMenuItemIdx/2) + UI_ICON_HIGHLIGHT], OSD_UPDATE);
	ParkingLineRemoteMode = LEFT_RIGHT_MODE;
	tUI_State = UI_SUBSUBMENU_STATE;

}

//-----------------------------------------------------------------------------------
void UI_ParkingLineDrawSubSubSubMenuItem()
{
	OSD_IMG_INFO tParkingLineOsdImgInfo,tOsdImgInfo;
	uint16_t uwHSize = 0,uwVSize = 0;
	uint16_t uwImgIdx;
	uint16_t uwSubSubMenuItemIdx = 
	(uint16_t)tSettingSubSubMenuItem.tSettingS[PARKINGLINE_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;	

	//��ֹ�ı��СʱԽ��
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_UPPER1 + tUI_CuSetting.ParkingLineSize[CamIdx], 1, &tParkingLineOsdImgInfo);
	uwHSize += tParkingLineOsdImgInfo.uwHSize;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LEFT, 1, &tParkingLineOsdImgInfo);
	uwHSize += 2*tParkingLineOsdImgInfo.uwHSize;
	uwVSize = tParkingLineOsdImgInfo.uwVSize;
	
	
	if((tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1] + uwHSize > OSD_WIDTH)
	{
		tUI_CuSetting.ParkingLine_XY[CamIdx][0] = (OSD_WIDTH - uwHSize) >> 8;
		tUI_CuSetting.ParkingLine_XY[CamIdx][1] = (uint8_t)(OSD_WIDTH - uwHSize);
	}
	UI_DrawParkingLine(CamIdx,OSD_QUEUE);
		//��ʾ����ͼ��
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);	
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}
	//��ʾң�ز���״̬
	if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
		uwImgIdx = OSD2IMG_PARKINGLINE_L_R_FLAG;
	else if(ParkingLineRemoteMode == UP_DOWN_MODE)
		uwImgIdx = OSD2IMG_PARKINGLINE_U_D_FLAG;
	else 
		uwImgIdx = OSD2IMG_PARKINGLINE_SIZE_FLAG;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2,uwImgIdx , 1, &tOsdImgInfo);
	tOsdImgInfo.uwXStart = (tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1] + (uwHSize - tOsdImgInfo.uwHSize)/2;
	tOsdImgInfo.uwYStart = (tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3] + (uwVSize - tOsdImgInfo.uwVSize)/2;
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);

}
//-----------------------------------------------------------------------------------

void UI_ParkingLineSubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	OSD_IMG_INFO tParkingLineOsdImgInfo,tOsdImgInfo;
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[PARKINGLINE_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;	
	uint16_t uwTempVal;
	uint16_t uwHSize = 0,uwVSize = 0; 
	//����֮ǰ�ĵ������
	UI_EraseParkingLine(CamIdx,OSD_QUEUE);
	
	//��ȡ�������Ĵ�С
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LEFT, 1, &tParkingLineOsdImgInfo);
	uwHSize += 2*tParkingLineOsdImgInfo.uwHSize;
	uwVSize = tParkingLineOsdImgInfo.uwVSize;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_UPPER1 + tUI_CuSetting.ParkingLineSize[CamIdx], 1, &tParkingLineOsdImgInfo);
	uwHSize += tParkingLineOsdImgInfo.uwHSize;

	//��������״̬
	tOSD_GetOsdImgInfor(1, OSD_IMG2,OSD2IMG_PARKINGLINE_L_R_FLAG, 1, &tOsdImgInfo);
	tOsdImgInfo.uwXStart = (tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1] + (uwHSize - tOsdImgInfo.uwHSize)/2;
	tOsdImgInfo.uwYStart = (tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3] + (uwVSize - tOsdImgInfo.uwVSize)/2;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo);
	
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			//����
			if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
			{
				if(((tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1]) >= 5)
				{
					uwTempVal = (tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1];
					uwTempVal -= 5;
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = uwTempVal >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = (uint8_t)uwTempVal;
				}
				else
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = 0;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = 0;
				}
			}//����
			else if(ParkingLineRemoteMode == UP_DOWN_MODE)
			{
				if(((tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3]) >= 5)
				{
					uwTempVal = (tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3];
					uwTempVal -= 5;
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = uwTempVal >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = (uint8_t)uwTempVal;

				}
				else
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = 0;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = 0;

				}
			}//��С
			else
			{
				if(tUI_CuSetting.ParkingLineSize[CamIdx] > 0)
					tUI_CuSetting.ParkingLineSize[CamIdx]--;
			}
			break;
		case RIGHT_ARROW:
			//����
			if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
			{
				if(((tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1])
					<= (OSD_WIDTH - uwHSize - 5))
				{
					uwTempVal = (tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1];
					uwTempVal += 5;
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = uwTempVal >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = (uint8_t)uwTempVal;

				}
				else
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = (OSD_WIDTH - uwHSize) >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = (uint8_t)(OSD_WIDTH - uwHSize);

				}

			}//����
			else if(ParkingLineRemoteMode == UP_DOWN_MODE)
			{
				if(((tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3]) 
					<= (OSD_HEIGHT - uwVSize - 5))
				{
					uwTempVal = (tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3];
					uwTempVal += 5;
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = uwTempVal >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = (uint8_t)uwTempVal;

				}
				else
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = (OSD_HEIGHT - uwVSize) >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = (uint8_t)(OSD_HEIGHT - uwVSize);

				}

			}//��С
			else
			{
				if(tUI_CuSetting.ParkingLineSize[CamIdx] < 11)
					tUI_CuSetting.ParkingLineSize[CamIdx] ++;
			}
			break;
		case ENTER_ARROW:
			if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
				ParkingLineRemoteMode = UP_DOWN_MODE;
			else if(ParkingLineRemoteMode == UP_DOWN_MODE)
				ParkingLineRemoteMode = SIZE_MODE;
			else
				ParkingLineRemoteMode = LEFT_RIGHT_MODE;
			break;
		case EXIT_ARROW:
			UI_ParkingLineSubSubSubMenuPageExit();
			SendIrCodeFlag = 0;
			return;
		default:
			return;
	}
	
	UI_ParkingLineDrawSubSubSubMenuItem();


		
}
//------------------------------------------------------------------------------------GuideLine--------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void UI_GuideLineSubSubSubMenuPageExit(void)
{
	OSD_IMG_INFO tOsdImgInfo[24],tMenuOsdImgInfo;
	uint16_t uwSubSubMenuItemIdx = 
			(uint16_t)tSettingSubSubMenuItem.tSettingS[GUIDELINE_ITEM].tSubMenuInfo.ubItemIdx;//sub sub

	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_GUIDELINE_TITLE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_GUIDELINE_TITLE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_GUIDELINE_TITLE_FR:OSD2IMG_GUIDELINE_TITLE_CHN, 1, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	//WORD
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_PARKINGLINE_CAM1_WD:OSD2IMG_PARKINGLINE_CAM1_WD_GER, 4, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);

	
	//enablr or disable
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_OFF_CAM1_NOR, 4, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
	{
		for(uint8_t j = 0;j < 4;j++)
		{
			tOsdImgInfo[6*i + j] = tOsdImgInfo[j];
			tOsdImgInfo[6*i + j].uwXStart = PARKINGLINE_SWITCH_X + PARKINGLINE_X_STEP*(i%2);
			tOsdImgInfo[6*i + j].uwYStart = PARKINGLINE_SWITCH_Y + PARKINGLINE_Y_STEP*(i/2);
		}
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_CAM1_AUTO_NOR + 2*i, 2, &tOsdImgInfo[6*i + 4]);
	}
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[6*i + 2*tUI_CuSetting.GuideLineEnable[i]], OSD_QUEUE); 
	//CALIBRATE
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_CAM1_CAL_NOR, 8, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
	//high light
	tOSD_Img2(&tOsdImgInfo[2*(uwSubSubMenuItemIdx/2) + UI_ICON_HIGHLIGHT], OSD_UPDATE);
	ParkingLineRemoteMode = LEFT_RIGHT_MODE;
	tUI_State = UI_SUBSUBMENU_STATE;

}

//-----------------------------------------------------------------------------------
void UI_GuideLineDrawSubSubSubMenuItem(void)
{
	OSD_IMG_INFO tOsdImgInfo[2];
	uint16_t uwImgIdx;
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[GUIDELINE_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;	
	//��ʾ�µĵ������
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GUIDELINE_SIZE1 + tUI_CuSetting.GuideLineSize[CamIdx], 1, &tOsdImgInfo[0]);
	//��ֹ�ı��СʱԽ��
	if((tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1] + tOsdImgInfo[0].uwHSize > OSD_WIDTH)
	{
		tUI_CuSetting.GuideLine_XY[CamIdx][0] = (OSD_WIDTH - tOsdImgInfo[0].uwHSize) >> 8;
		tUI_CuSetting.GuideLine_XY[CamIdx][1] = (uint8_t)(OSD_WIDTH - tOsdImgInfo[0].uwHSize);
	}
		
	tOsdImgInfo[0].uwXStart = (tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1];
	tOsdImgInfo[0].uwYStart = (tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3];
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	//��ʾң�ز���״̬
	if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
		uwImgIdx = OSD2IMG_PARKINGLINE_L_R_FLAG;
	else if(ParkingLineRemoteMode == UP_DOWN_MODE)
		uwImgIdx = OSD2IMG_PARKINGLINE_U_D_FLAG;
	else 
		uwImgIdx = OSD2IMG_PARKINGLINE_SIZE_FLAG;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2,uwImgIdx, 1, &tOsdImgInfo[1]);
	tOsdImgInfo[1].uwXStart = tOsdImgInfo[0].uwXStart + (tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize)/2;
	if(tOsdImgInfo[0].uwYStart > tOsdImgInfo[1].uwVSize + 20)
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart - tOsdImgInfo[1].uwVSize - 20;
	else
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize + 20;
	tOSD_Img2(&tOsdImgInfo[1], ubUI_TouchPanelSts?OSD_QUEUE:OSD_UPDATE);
	//��ʾ����ͼ��
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo[0]);	
		tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	}
}

//-----------------------------------------------------------------------------------
void UI_GuideLineSubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	OSD_IMG_INFO tOsdImgInfo[2];
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[GUIDELINE_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;	
	uint16_t uwPreVal_X = (tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1];
	uint16_t uwPreVal_Y = (tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3];
	uint16_t uwTempVal;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GUIDELINE_SIZE1 + tUI_CuSetting.GuideLineSize[CamIdx], 1, &tOsdImgInfo[0]);
	//����֮ǰ�ĵ�������ң�ز���״̬
	tOsdImgInfo[0].uwXStart = uwPreVal_X;
	tOsdImgInfo[0].uwYStart = uwPreVal_Y;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo[0]);

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_L_R_FLAG, 1, &tOsdImgInfo[1]);
	tOsdImgInfo[1].uwXStart = tOsdImgInfo[0].uwXStart + (tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize)/2;
	if(tOsdImgInfo[0].uwYStart > tOsdImgInfo[1].uwVSize + 20)
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart - tOsdImgInfo[1].uwVSize - 20;
	else
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize + 20;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo[1]);


	
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			//����
			if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
			{
				if(((tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1]) >= 5)
				{
					uwTempVal = (tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1];
					uwTempVal -= 5;
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = uwTempVal >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = (uint8_t)uwTempVal;
				}
				else
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = 0;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = 0;
				}
			}//����
			else if(ParkingLineRemoteMode == UP_DOWN_MODE)
			{
				if(((tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3]) >= 5)
				{
					uwTempVal = (tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3];
					uwTempVal -= 5;
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = uwTempVal >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = (uint8_t)uwTempVal;

				}
				else
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = 0;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = 0;

				}
			}//��С
			else
			{
				if(tUI_CuSetting.GuideLineSize[CamIdx] > 0)
					tUI_CuSetting.GuideLineSize[CamIdx]--;
			}
			break;
		case RIGHT_ARROW:
			//����
			if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
			{
				if(((tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1])
					<= (OSD_WIDTH - tOsdImgInfo[0].uwHSize - 5))
				{
					uwTempVal = (tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1];
					uwTempVal += 5;
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = uwTempVal >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = (uint8_t)uwTempVal;

				}
				else
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = (OSD_WIDTH - tOsdImgInfo[0].uwHSize) >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = (uint8_t)(OSD_WIDTH - tOsdImgInfo[0].uwHSize);

				}

			}//����
			else if(ParkingLineRemoteMode == UP_DOWN_MODE)
			{
				if(((tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3]) 
					<= (OSD_HEIGHT - tOsdImgInfo[0].uwVSize - 5))
				{
					uwTempVal = (tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3];
					uwTempVal += 5;
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = uwTempVal >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = (uint8_t)uwTempVal;

				}
				else
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = (OSD_HEIGHT - tOsdImgInfo[0].uwVSize) >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = (uint8_t)(OSD_HEIGHT - tOsdImgInfo[0].uwVSize);

				}

			}//��С
			else
			{
				if(tUI_CuSetting.GuideLineSize[CamIdx] < 17)
					tUI_CuSetting.GuideLineSize[CamIdx] ++;
			}
			break;
		case ENTER_ARROW:
			if(ParkingLineRemoteMode == LEFT_RIGHT_MODE)
				ParkingLineRemoteMode = UP_DOWN_MODE;
			else if(ParkingLineRemoteMode == UP_DOWN_MODE)
				ParkingLineRemoteMode = SIZE_MODE;
			else
				ParkingLineRemoteMode = LEFT_RIGHT_MODE;
			break;
		case EXIT_ARROW:
			UI_GuideLineSubSubSubMenuPageExit();
			SendIrCodeFlag = 0;
			return;
		default:
			return;
	}
		
	UI_GuideLineDrawSubSubSubMenuItem();

}







//setting�Ĵ󲿷ֽ���Ĳ����߼�����࣬����û����������Ľ��涼ʹ��ͬһ��ң�ؽ�������
//------------------------------------------------------------------------------------universal------------------------------------------------------------------------------
/****************record************************/
static UI_SubMenuItem_t tRecordFmtSdMenuItem = 
{
	0,2,{0,0}
};
static UI_SubMenuItem_t tSystemDateTimeMenuItem = 
{
	DATE_TIME_YEAR,DATE_TIME_MAX,{0,0}
};


static UI_SubMenuItem_t *tRecordSetMenuItem[] = 
{
	NULL,
	NULL,
	NULL,
	&tRecordFmtSdMenuItem,
	NULL,
};

static UI_SubMenuItem_t *tSystemSetMenuItem[] = 
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&tSystemDateTimeMenuItem,
};



/****************TRIGGER************************/
static UI_SubMenuItem_t tTriggerCam1DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tTriggerCam2DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tTriggerCam3DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tTriggerCam4DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tTriggerCamSplitDelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tTriggerCam1PriorityMenuItem = 
{
	0,5,{0,0}
};
static UI_SubMenuItem_t tTriggerCam2PriorityMenuItem = 
{
	0,5,{0,0}
};
static UI_SubMenuItem_t tTriggerCam3PriorityMenuItem = 
{
	0,5,{0,0}
};
static UI_SubMenuItem_t tTriggerCam4PriorityMenuItem = 
{
	0,5,{0,0}
};
static UI_SubMenuItem_t tTriggerCamSplitPriorityMenuItem = 
{
	0,5,{0,0}
};
static UI_SubMenuItem_t tTriggerDisplaySetUpMenuItem = 
{
	0,POWERON_QUAD + 1,{0,0}
};

static UI_SubMenuItem_t *tTriggerMenuItem[] = 
{
	&tTriggerCam1DelayMenuItem,
	&tTriggerCam2DelayMenuItem,
	&tTriggerCam3DelayMenuItem,
	&tTriggerCam4DelayMenuItem,
	&tTriggerCamSplitDelayMenuItem,
	&tTriggerCam1PriorityMenuItem,
	&tTriggerCam2PriorityMenuItem,
	&tTriggerCam3PriorityMenuItem,
	&tTriggerCam4PriorityMenuItem,
	&tTriggerCamSplitPriorityMenuItem,
	&tTriggerDisplaySetUpMenuItem
};

/****************AUTOSCAN************************/
static UI_SubMenuItem_t tAutoScanCam1DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tAutoScanCam2DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tAutoScanCam3DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tAutoScanCam4DelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tAutoScanDualDelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tAutoScanHDelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t tAutoScanQuadDelayMenuItem = 
{
	0,12,{0,0}
};
static UI_SubMenuItem_t *tAutoScanMenuItem[] = 
{
	NULL,
	&tAutoScanCam1DelayMenuItem,
	NULL,
	&tAutoScanCam2DelayMenuItem,
	NULL,
	&tAutoScanCam3DelayMenuItem,
	NULL,
	&tAutoScanCam4DelayMenuItem,
	NULL,
	&tAutoScanDualDelayMenuItem,
	NULL,
	&tAutoScanHDelayMenuItem,
	NULL,
	&tAutoScanQuadDelayMenuItem,
};

//----------------------------------------------------------------------
UI_SubMenuItem_t **tSettingSubSubSubMenuItem[] = 
{
	NULL,	//camera
	tRecordSetMenuItem,	//record
	tSystemSetMenuItem, //system
	NULL,	//dual
	NULL,	//quad
	tTriggerMenuItem,	//trigger
	tAutoScanMenuItem,	//auto scan
	NULL,				//power on
	NULL,//parkingLine
};


static uint8_t TempTriggerDelay;
static uint8_t TempAutoScanDelay;

//ÿ�ν�����Ӧsubsubsub�˵���Ҫ������ӦubItemIdx�ĳ�ʼֵ
uint8_t ubUI_SubSubSubStsUpdateFlag = FALSE;

//----------------------------------------------------------------------
static void UI_SettingUpdateSubSubSubMenuItemIndex(UI_SettingSubMenuItemList_t tSubMenuItem,uint16_t uwSubSubMenuItemIdx)
{
	if(!ubUI_SubSubSubStsUpdateFlag)
	{	
		memset(&(tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo), 0, sizeof(UI_MenuItem_t));
		if(tSubMenuItem == TRIGGERSET_ITEM)
		{
			if(uwSubSubMenuItemIdx <= TRIGGER_DELAY_CAMSPLIT)
			{
				TempTriggerDelay = tUI_CuSetting.TriggerDelay[uwSubSubMenuItemIdx];
			}	
			else if(uwSubSubMenuItemIdx <= TRIGGER_PRIORITY_CAMSPLIT)
			{
				tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = 
					tUI_CuSetting.TriggerPriority[uwSubSubMenuItemIdx - 5];
			}
			else
			{
				tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = 
					tUI_CuSetting.TriggerSplitView;
			}
				
		}
		else if(tSubMenuItem == AUTOSCANSET_ITEM)
		{
			TempAutoScanDelay = tUI_CuSetting.AutoScanDuty[uwSubSubMenuItemIdx/2];
		}
		ubUI_SubSubSubStsUpdateFlag = TRUE;
	}
}
//------------------------------------------------------------------
static void UI_RecordSubSubSubMenuExit(uint16_t uwSubSubMenuItemIdx)
{
	if(uwSubSubMenuItemIdx == SDCARD_ITEM)
	{
		OSD_IMG_INFO tMenuOsdImgInfo,tOsdImgInfo[18];
		tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
		tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
		if(!ubUI_TouchPanelSts)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
			tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
		}
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_RECORD_TITLE:
										(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_RECORD_TITLE_GER:
										(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_RECORD_TITLE_FR:OSD2IMG_RECORD_TITLE_CHN, 1, &tOsdImgInfo[0]);
		//����
		for(uint8_t i = 0;i < 1;i++)
			tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);
		
		//record time
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_1MIN_NOR, 12, &tOsdImgInfo[0]);
		for(uint8_t i = 0;i < 3;i++)
		{
			if(tUI_CuSetting.RecInfo.tREC_Time == i)
				tOSD_Img2(&tOsdImgInfo[2*i + 6], OSD_QUEUE);//����ͼ���ϵ�����
			else
				tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
		}
		
		//format sd card
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FORMATSD_NOR, 4, &tOsdImgInfo[0]);
		if(uwSubSubMenuItemIdx == SDCARD_ITEM)
			tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
		else
			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

		//power on record
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_POWERON_REC_OFF_NOR, 4, &tOsdImgInfo[0]);
		if(tUI_CuSetting.ubPowerOnRecord)
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
		else
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_STAMP_OFF_NOR, 4, &tOsdImgInfo[0]);
		if(tUI_CuSetting.ubTimeStame)
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
		else
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);

	}
}

//------------------------------------------------------------------
static void UI_TriggerSubSubSubMenuExit(uint16_t uwSubSubMenuItemIdx)
{
		
	OSD_IMG_INFO tOsdImgInfo_Button[22],tOsdImgInfo_Word[9],tMenuOsdImgInfo;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}

	//word
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_TRIGGER_TITLE:
								(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_TRIGGER_TITLE_GER:
								(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_TRIGGER_TITLE_FR:OSD2IMG_TRIGGER_TITLE_CHN, 1, &tOsdImgInfo_Word[0]);
	tOSD_Img2(&tOsdImgInfo_Word[0], OSD_QUEUE);

		//foot switch
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOOTSWITCH_OFF_NOR, 4, &tOsdImgInfo_Button[0]);
	if(tUI_CuSetting.ubZoomfootswitch)
	{
		tOSD_Img2(&tOsdImgInfo_Button[2], OSD_QUEUE);
	}
	else
	{
		tOSD_Img2(&tOsdImgInfo_Button[0], OSD_QUEUE);
	}

	//button
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_YEAR_NOR, 2, &tOsdImgInfo_Button[0]);
	for(uint8_t i = 0;i < 10;i++)
	{
		tOsdImgInfo_Button[2*i] = tOsdImgInfo_Button[0];
		tOsdImgInfo_Button[2*i + 1] = tOsdImgInfo_Button[1];
		tOsdImgInfo_Button[2*i].uwXStart = tOsdImgInfo_Button[2*i + 1].uwXStart = TRIGGER_X + TRIGGER_X_STEP*(i%5);
		tOsdImgInfo_Button[2*i].uwYStart = tOsdImgInfo_Button[2*i + 1].uwYStart = TRIGGER_Y + TRIGGER_Y_STEP*(i/5);
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_TRIGGER_DISPLAY_SETUP_NOR:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_TRIGGER_DISPLAY_SETUP_NOR_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_TRIGGER_DISPLAY_SETUP_NOR_FR:OSD2IMG_TRIGGER_DISPLAY_SETUP_NOR_CHN, 2, &tOsdImgInfo_Button[20]);	
	for(uint8_t i = 0;i < 11;i++)
		tOSD_Img2(&tOsdImgInfo_Button[2*i], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo_Button[2*uwSubSubMenuItemIdx + UI_ICON_HIGHLIGHT], OSD_QUEUE);
	//value
	for(uint8_t i = 0;i < 5;i++)
	{
		//delay
		UI_ShowButtonValueNormal(tUI_CuSetting.TriggerDelay[i],
			tOsdImgInfo_Button[2*i].uwXStart + ((tUI_CuSetting.TriggerDelay[i] < 10)?40:30),tOsdImgInfo_Button[2*i].uwYStart + 15,OSD_QUEUE);		
		//priority
		UI_ShowButtonValueNormal(tUI_CuSetting.TriggerPriority[i] + 1,
			tOsdImgInfo_Button[10 + 2*i].uwXStart + 40,tOsdImgInfo_Button[10 + 2*i].uwYStart + 15,OSD_QUEUE);		
	}
	//highlight
	uint16_t i = uwSubSubMenuItemIdx;
	if(uwSubSubMenuItemIdx <= TRIGGER_DELAY_CAMSPLIT)
	{
		UI_ShowButtonValueHighLight(tUI_CuSetting.TriggerDelay[i],
			tOsdImgInfo_Button[2*i].uwXStart + ((tUI_CuSetting.TriggerDelay[i] < 10)?40:30),tOsdImgInfo_Button[2*i].uwYStart + 15,OSD_UPDATE);
	}
	else if(uwSubSubMenuItemIdx <= TRIGGER_PRIORITY_CAMSPLIT)
	{
		UI_ShowButtonValueHighLight(tUI_CuSetting.TriggerPriority[i - 5] + 1,
			tOsdImgInfo_Button[2*i].uwXStart + 40,tOsdImgInfo_Button[2*i].uwYStart + 15,OSD_UPDATE);

	}
	else
		tOSD_Img2(&tOsdImgInfo_Button[21], OSD_UPDATE);


}
//------------------------------------------------------------------
static void UI_AutoScanSubSubSubMenuExit(uint16_t uwSubSubMenuItemIdx)
{
	OSD_IMG_INFO tOsdImgInfo[28],tMenuOsdImgInfo;
	uint8_t CamIdx = uwSubSubMenuItemIdx/2;
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	//word
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_TITLE, 7, &tOsdImgInfo[0]);
	if(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_CAM1_WORD_GER, 4, &tOsdImgInfo[1]);
	else if(tUI_CuSetting.tLanguage == LANGUAGE_CHINESE)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_TITLE_CHN, 1, &tOsdImgInfo[0]);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_DUAL_WORD_CHN, 2, &tOsdImgInfo[5]);
	}
	
	
	for(uint8_t i = 0;i < 7;i++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);

	//delay
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_YEAR_NOR, 2, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 6;i++)
	{
		tOsdImgInfo[2*i] = tOsdImgInfo[0];
		tOsdImgInfo[2*i + 1] = tOsdImgInfo[1];
		tOsdImgInfo[2*i].uwXStart = tOsdImgInfo[2*i + 1].uwXStart = AUTOSCAN_DELAY_X + AUTO_SCAN_OFF_DIST_X*(i%2);
		tOsdImgInfo[2*i].uwYStart = tOsdImgInfo[2*i + 1].uwYStart = AUTOSCAN_DELAY_Y + AUTO_SCAN_OFF_DIST_Y*(i/2);
	}
	for(uint8_t i = 0;i < 6;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);

	//value
	for(uint8_t i = 0;i < 6;i++)
	{
		UI_ShowButtonValueNormal(tUI_CuSetting.AutoScanDuty[i],
			tOsdImgInfo[2*i].uwXStart + ((tUI_CuSetting.AutoScanDuty[i] < 10)?40:30),tOsdImgInfo[2*i].uwYStart + 15,OSD_QUEUE);		
	}

	UI_AutoDrawOnOffButton(FALSE, 0);
	//OnOff
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_OFF_CAM1_NOR, 24, &tOsdImgInfo[0]);
//	for(uint8_t i = 0;i < 6;i++)
//	{
//		if(!tUI_CuSetting.AutoScanEnable[i])//off
//			tOSD_Img2(&tOsdImgInfo[4*i], OSD_QUEUE);
//		else
//			tOSD_Img2(&tOsdImgInfo[2 + 4*i],OSD_QUEUE);
//	}
	//highLight
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_YEAR_NOR, 2, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 6;i++)
	{
		tOsdImgInfo[2*i] = tOsdImgInfo[0];
		tOsdImgInfo[2*i + 1] = tOsdImgInfo[1];
		tOsdImgInfo[2*i].uwXStart = tOsdImgInfo[2*i + 1].uwXStart = AUTOSCAN_DELAY_X + AUTO_SCAN_OFF_DIST_X*(i%2);
		tOsdImgInfo[2*i].uwYStart = tOsdImgInfo[2*i + 1].uwYStart = AUTOSCAN_DELAY_Y + AUTO_SCAN_OFF_DIST_Y*(i/2);
	}
	tOSD_Img2(&tOsdImgInfo[2*CamIdx + UI_ICON_HIGHLIGHT], OSD_QUEUE);
	UI_ShowButtonValueHighLight(tUI_CuSetting.AutoScanDuty[CamIdx],
		tOsdImgInfo[2*CamIdx].uwXStart + ((tUI_CuSetting.AutoScanDuty[CamIdx] < 10)?40:30),tOsdImgInfo[2*CamIdx].uwYStart + 15,OSD_UPDATE);		


		
}
//------------------------------------------------------------------
static void UI_SettingSubSubSubMenuExit(void)
{
	uint16_t uwSubMenuItemIdx = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;//sub
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[uwSubMenuItemIdx].tSubMenuInfo.ubItemIdx;//sub sub
		
	if(uwSubMenuItemIdx == RECORDSET_ITEM)
	{
		UI_RecordSubSubSubMenuExit(uwSubSubMenuItemIdx);
	}
	else if(uwSubMenuItemIdx == TRIGGERSET_ITEM)
	{
		UI_TriggerSubSubSubMenuExit(uwSubSubMenuItemIdx);
	}
	else if(uwSubMenuItemIdx == AUTOSCANSET_ITEM)
	{	
		UI_AutoScanSubSubSubMenuExit(uwSubSubMenuItemIdx);
	}
	
	ubUI_SubSubSubStsUpdateFlag = FALSE;
	SettingSubSubSubRemoteMode = REMOTE_SELECT_ITEM;
	tUI_State = UI_SUBSUBMENU_STATE;

}
//----------------------------------------------------------------------
static void UI_RecordSubSubSubMenuExecute(uint16_t uwSubSubMenuItemIdx,uint16_t uwsubsubsubItemIdx)
{
	if(uwSubSubMenuItemIdx == SDCARD_ITEM)
	{
		if(uwsubsubsubItemIdx == SDCARD_FORMAT_YES)
		{
			if(UI_REC_START == tUI_RecPlayAct.tRecAct)
				UI_VideoRecordingExec(UI_REC_STOP);
			
			OSD_IMG_INFO tOsdImgInfo[3];
			UI_FuncExecMsg_t tWorkAct;
			uint8_t ubSdFmtRet;
			FS_FMT_STATUS tSDF_Ret;
			
			tWorkAct.uwFunc = UI_SDCARDFMT_ACT;
			osMessagePut(osUI_FuncsExecQue, &tWorkAct, 0);
			osMessageGet(osUI_FuncsFinExecQue, &ubSdFmtRet, osWaitForever);
			tSDF_Ret = (rUI_SUCCESS == ubSdFmtRet)?FORMAT_OK:FORMAT_FAIL;
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SDCARD_FORMAT_RETURN_BG, 3, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
			tOSD_Img2(&tOsdImgInfo[1 + tSDF_Ret], OSD_UPDATE);
			osDelay(1000);
			UI_SettingSubSubSubMenuExit();
		}
		else
		{
			UI_SettingSubSubSubMenuExit();
		}
	}
}
//----------------------------------------------------------------------
static void UI_SystemSubSubSubMenuExecute(uint16_t uwSubSubMenuItemIdx,uint16_t uwsubsubsubItemIdx)
{
	/*if(uwSubSubMenuItemIdx == DATETIME_ITEM)
	{
		OSD_IMG_INFO tOsdImgInfo[25],tMenuOsdImgInfo;
		tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
        if(uwsubsubsubItemIdx != DATE_TIME_UTC_SELECT)
        {
			tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
		}
		if(!ubUI_TouchPanelSts)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
			tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
		}
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_KEYBOARD_BACKGROUND, 25, &tOsdImgInfo[0]);
		
		if(uwsubsubsubItemIdx < DATE_TIME_UTC_SELECT)
		{
			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
			for(uint8_t i = 0;i < 12;i++)
				tOSD_Img2(&tOsdImgInfo[1 + 2*i], OSD_QUEUE);
			tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
		}

		if(uwsubsubsubItemIdx == DATE_TIME_YEAR)
		{
			UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.uwYear,OSD_UPDATE);
		}
		else if((uwsubsubsubItemIdx == DATE_TIME_MONTH))
		{
			UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubMonth,OSD_UPDATE);
		}
		else if((uwsubsubsubItemIdx == DATE_TIME_DAY))
		{
			UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubDate,OSD_UPDATE);
		}
		else if((uwsubsubsubItemIdx == DATE_TIME_HOUR))
		{
			UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubHour,OSD_UPDATE);
		}
		else if((uwsubsubsubItemIdx == DATE_TIME_MINUTE))
		{
			UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubMin,OSD_UPDATE);	
		}
		else if((uwsubsubsubItemIdx == DATE_TIME_SECOND))
		{
			UI_ShowKeyBoardValue(tUI_CuSetting.tSysCalendar.ubSec,OSD_UPDATE);
		}
		else if((uwsubsubsubItemIdx == DATE_TIME_UTC_SELECT))
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 6, &tOsdImgInfo[0]);
            SettingSubSubSubRemoteMode = 1 - SettingSubSubSubRemoteMode;
            if(SettingSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
            	tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
			else
				tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);
			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
			tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.ubtimezone,0,26,
				tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
			
			return;
		}
		else if(uwsubsubsubItemIdx == DATE_TIME_UTC_COMFIRM)
		{
            tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 2, &tOsdImgInfo[0]);
	   	    tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
            UI_UpdateDevStatusInfo();
			SYS_Reboot();
			return;
		}
		ubUI_DateTimeSubSubSubSubStsUpdateFlag = FALSE;
		tUI_State = UI_SUBSUBSUBSUBMENU_STATE;

	}
*/
}
//----------------------------------------------------------------------
static void UI_TriggerSubSubSubSubMenuExecute(uint16_t uwSubSubMenuItemIdx,uint16_t uwsubsubsubItemIdx)
{
	if(uwSubSubMenuItemIdx <= TRIGGER_DELAY_CAMSPLIT)//delay
	{
		if(uwsubsubsubItemIdx <= KEYBOARD_KEY9)
		{
			if((TempTriggerDelay*10 + uwsubsubsubItemIdx + 1) < 60)
				TempTriggerDelay = TempTriggerDelay*10 + uwsubsubsubItemIdx + 1;
			else 
				TempTriggerDelay = 60;
		}
		else if(uwsubsubsubItemIdx == KEYBOARD_KEY0)
		{
			if((TempTriggerDelay*10) <= 60)
				TempTriggerDelay = TempTriggerDelay*10;
			else
				TempTriggerDelay = 60;
		}
		else if(uwsubsubsubItemIdx == KEYBOARD_KEY_ENTER)
		{
			if(TempTriggerDelay > 0)
				tUI_CuSetting.TriggerDelay[uwSubSubMenuItemIdx] = TempTriggerDelay;
			else
				tUI_CuSetting.TriggerDelay[uwSubSubMenuItemIdx] = 1;
			UI_SettingSubSubSubMenuExit();
			return;
		}
		else if(uwsubsubsubItemIdx == KEYBOARD_KEY_DELETE)
			TempTriggerDelay  = TempTriggerDelay / 10;
		
		UI_ShowKeyBoardValue(TempTriggerDelay,OSD_UPDATE);
			
	}
	else if(uwSubSubMenuItemIdx <= TRIGGER_PRIORITY_CAMSPLIT)//priority
	{
		for(uint8_t i = 0;i < 5;i++)
		{
			if(tUI_CuSetting.TriggerPriority[i] == uwsubsubsubItemIdx)
			{
				tUI_CuSetting.TriggerPriority[i] = tUI_CuSetting.TriggerPriority[uwSubSubMenuItemIdx - 5];
				tUI_CuSetting.TriggerPriority[uwSubSubMenuItemIdx - 5] = uwsubsubsubItemIdx;
				UI_SettingSubSubSubMenuExit();
			}
		}
	}
	else//display setup
	{
		tUI_CuSetting.TriggerSplitView = uwsubsubsubItemIdx;
		UI_SettingSubSubSubMenuExit();
	}
}
//----------------------------------------------------------------------
static void UI_AutoScanSubSubSubSubMenuExecute(uint16_t uwSubSubMenuItemIdx,uint16_t uwsubsubsubItemIdx)
{
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;
	if(uwsubsubsubItemIdx <= KEYBOARD_KEY9)
	{
		if((TempAutoScanDelay*10 + uwsubsubsubItemIdx + 1) < 60)
			TempAutoScanDelay = TempAutoScanDelay*10 + uwsubsubsubItemIdx + 1;
		else 
			TempAutoScanDelay = 60;
	}
	else if(uwsubsubsubItemIdx == KEYBOARD_KEY0)
	{
		if((TempAutoScanDelay*10) <= 60)
			TempAutoScanDelay = TempAutoScanDelay*10;
		else
			TempAutoScanDelay = 60;
	}
	else if(uwsubsubsubItemIdx == KEYBOARD_KEY_ENTER)
	{
		if(TempAutoScanDelay > 5)
			tUI_CuSetting.AutoScanDuty[CamIdx] = TempAutoScanDelay;
		else
			tUI_CuSetting.AutoScanDuty[CamIdx] = 5;
			
		UI_SettingSubSubSubMenuExit();
		return;
	}
	else if(uwsubsubsubItemIdx == KEYBOARD_KEY_DELETE)
		TempAutoScanDelay  = TempAutoScanDelay / 10;
	
	UI_ShowKeyBoardValue(TempAutoScanDelay,OSD_UPDATE);

}
//----------------------------------------------------------------------

static void UI_SettingSubSubSubMenuExecute(void)
{
	uint16_t uwSubMenuItem = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;//sub
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[uwSubMenuItem].tSubMenuInfo.ubItemIdx;//sub sub
	uint16_t uwsubsubsubItemIdx = 
		tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;//sub sub sub
	if(uwSubMenuItem == RECORDSET_ITEM)
	{
		UI_RecordSubSubSubMenuExecute(uwSubSubMenuItemIdx,uwsubsubsubItemIdx);
	}
	else if(uwSubMenuItem == SYSTEMSET_ITEM)
	{
		UI_SystemSubSubSubMenuExecute(uwSubSubMenuItemIdx,uwsubsubsubItemIdx);
	}
	else if(uwSubMenuItem == TRIGGERSET_ITEM)
	{
		UI_TriggerSubSubSubSubMenuExecute(uwSubSubMenuItemIdx,uwsubsubsubItemIdx);
	}
	else if(uwSubMenuItem == AUTOSCANSET_ITEM)
	{
		UI_AutoScanSubSubSubSubMenuExecute(uwSubSubMenuItemIdx,uwsubsubsubItemIdx);
	}
	
}
//----------------------------------------------------------------------
static void UI_SettingingDrawSubSubSubMenuItem(uint16_t uwSubMenuItem,uint16_t uwSubSubMenuItemIdx)
{
	uint8_t ubItemPreIdx = tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemIdx = tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;

	uint16_t uwSettingItemOsdImg[12] = {0};
	OSD_IMG_INFO tOsdImgInfo[12];

	if(uwSubMenuItem == SYSTEMSET_ITEM && uwSubSubMenuItemIdx == DATETIME_ITEM)
	{
		//pre
		if(ubItemPreIdx <= DATE_TIME_SECOND)
		{
			UI_DrawDateTimeYearButtonItem(FALSE, ubItemPreIdx);
		}
		else if(ubItemPreIdx == DATE_TIME_UTC_SELECT)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 6, &tOsdImgInfo[0]);
			if(SettingSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
			{
                if (ubItemIdx != DATE_TIME_UTC_SELECT)
                {
					OSD_EraserImg2_NoUpdate(&tOsdImgInfo[4]);
				}
			}
			else
				tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);

			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

			tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.ubtimezone,0,26,
				tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
			UI_ShowUTCVal();

		}
		else if(ubItemPreIdx == DATE_TIME_UTC_COMFIRM)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 2, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
		}
		//cur
		if(ubItemIdx <= DATE_TIME_SECOND)
		{
			UI_DrawDateTimeYearButtonItem(TRUE, ubItemIdx);
		}
		else if(ubItemIdx == DATE_TIME_UTC_SELECT)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 6, &tOsdImgInfo[0]);
			if (SettingSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
				tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
			else
				tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);
			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

			tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.ubtimezone,0,26,
				tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
			tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
			UI_ShowUTCVal();
		}
		else if(ubItemIdx == DATE_TIME_UTC_COMFIRM)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 2, &tOsdImgInfo[0]);
			tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);

		}
	}
	else
	{
		if(uwSubMenuItem == RECORDSET_ITEM)
		{
			if(uwSubSubMenuItemIdx == SDCARD_ITEM)
			{
				for(uint8_t i = 0;i < 2;i ++)
					uwSettingItemOsdImg[i] = ((tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_YES_NOR:
											(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_YES_NOR_GER:
											(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_YES_NOR_FR:OSD2IMG_SDCARD_YES_NOR_CHN) + 2*i;
			}
		}
		else if(uwSubMenuItem == TRIGGERSET_ITEM)
		{
			if(uwSubSubMenuItemIdx <= TRIGGER_DELAY_CAMSPLIT)
			{
				for(uint8_t i = 0;i < 12;i ++)
					uwSettingItemOsdImg[i] = OSD2IMG_KEYBOARD_BUTTON1_NOR + 2*i;
			}
			else if(uwSubSubMenuItemIdx <= TRIGGER_PRIORITY_CAMSPLIT)
			{
				for(uint8_t i = 0;i < 5;i ++)
					uwSettingItemOsdImg[i] = OSD2IMG_TRIGGER_PRIORITY1_NOR + 2*i;
			}
			else
			{
				for(uint8_t i = 0;i < POWERON_QUAD + 1;i ++)
					uwSettingItemOsdImg[i] = OSD2IMG_POWERON_CAM1_NOR + 2*i;
			}
		}
		else if(uwSubMenuItem == AUTOSCANSET_ITEM)
		{
			for(uint8_t i = 0;i < 12;i ++)
				uwSettingItemOsdImg[i] = OSD2IMG_KEYBOARD_BUTTON1_NOR + 2*i;
		}

		UI_DrawHLandNormalIcon(uwSettingItemOsdImg[ubItemPreIdx],uwSettingItemOsdImg[ubItemIdx] + UI_ICON_HIGHLIGHT);
	}
	
}


//----------------------------------------------------------------------
void UI_SettingSubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	uint16_t uwSubMenuItem = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;//sub
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[uwSubMenuItem].tSubMenuInfo.ubItemIdx;//sub sub
	UI_SettingUpdateSubSubSubMenuItemIndex(uwSubMenuItem,uwSubSubMenuItemIdx);
	tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx = 
		tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;

	switch(tArrowKey)
	{
		case LEFT_ARROW:
            if(SettingSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
            {
				if(tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx ==
					tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->ubFirstItem)
				{
					tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = 
						tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->ubItemCount - 1;
				}
				else
					tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx --;
			}
			else
			{
				if(uwSubMenuItem == RECORDSET_ITEM)
				{
					if (tUI_CuSetting.ubtimezone > 0)
					{
						tUI_CuSetting.ubtimezone--;
					}
					else
					{
						tUI_CuSetting.ubtimezone = 26;
					}
				}
			}
			break;
		case RIGHT_ARROW:
			if(SettingSubSubSubRemoteMode == REMOTE_SELECT_ITEM)
            {
				if(tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx == 
					tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->ubItemCount - 1)
				{
					tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = 
						tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->ubFirstItem;
				}
				else
					tSettingSubSubSubMenuItem[uwSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx ++;
			}
			else
			{
				if(uwSubMenuItem == RECORDSET_ITEM)
				{
					if (tUI_CuSetting.ubtimezone < 26)
					{
						tUI_CuSetting.ubtimezone++;
					}
					else
					{
						tUI_CuSetting.ubtimezone = 0;
					}
				}
			}
			break;
		case ENTER_ARROW:
			UI_SettingSubSubSubMenuExecute();
			return;
		case EXIT_ARROW:
			UI_SettingSubSubSubMenuExit();
			return;
		default:
			return;
	}
	UI_SettingingDrawSubSubSubMenuItem(uwSubMenuItem,uwSubSubMenuItemIdx);
	
}








/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/

void UI_SubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	switch(tUI_MenuItem.ubItemIdx)
	{
		case PLAYBACK_ITEM:
			UI_PlaybackSubSubSubTouchMenu(Touch_Info);
			break;
		case SETTING_ITEM:
			{
				UI_SettingSubMenuItemList_t tSubMenuItem = 
					(UI_SettingSubMenuItemList_t)tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;//sub
				 switch(tSubMenuItem)
				 {
				 	case CAMERASET_ITEM:
						UI_CameraSubSubSubTouchMenu(Touch_Info);
						break;
					case SYSTEMSET_ITEM:
						UI_SystemSubSubSubTouchMenu(Touch_Info);
						break;
					case PARKINGLINE_ITEM:
						UI_ParkingLIneSubSubSubTouchMenu(Touch_Info);
						break;
					case AI_ITEM:
						UI_AISubSubSubTouchMenu(Touch_Info);
						break;
					case GUIDELINE_ITEM:
						UI_GuideLineSubSubSubTouchMenu(Touch_Info);
						break;
					default:
						UI_SettingSubSubSubTouchMenu(Touch_Info);
						break;
				 }
					
			}
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------playback-----------------------------------------------------------------------
//------------------------------------------------------------------------------playback-----------------------------------------------------------------------
//------------------------------------------------------------------------------playback-----------------------------------------------------------------------

void UI_PlaybackSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{

	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i;
	tUI_RecPlayListItem.ubItemPreIdx = tUI_RecPlayListItem.ubItemIdx;
	
	if(UI_SUBSUBSUBMENU_STATE != tUI_State)
		return;

	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:

			//������ָ������ĸ�ͼ����
			for(i = 0; i < UI_RECPLAYLISTITEM_MAX;i++)//�жϾ��������ĸ�ͼ����
			{
				//��ȡͼƬ��Ϣ
				tOSD_GetOsdImgInfor(1, OSD_IMG2, uwRecPlayListImgIdx[i],1, &tOsdImgInfo);
			
				if(Touch_Info->startX > tOsdImgInfo.uwXStart && Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart && Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_RecPlayListItem.ubItemIdx = i;
					break;
				}
			}
			if(UI_RECPLAYLISTITEM_MAX == i)//û�а�����Ч�Ŀؼ�
				return;
			
			UI_DrawHLandNormalIcon(uwRecPlayListImgIdx[tUI_RecPlayListItem.ubItemPreIdx], 
				(uwRecPlayListImgIdx[tUI_RecPlayListItem.ubItemIdx] + UI_ICON_HIGHLIGHT));
	
			UI_EnterKey();
			break;
			
		case TOUCH_TURNDOWN:
			break;

		default:

			return;
	}

}


//------------------------------------------------------------------------------setting-----------------------------------------------------------------------
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------

//------------------------------------------------------------------------------------CAMERA------------------------------------------------------------------------------

void UI_CameraSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	static uint8_t ubTouchPressDownCount = 0;//�ñ����Ǹ�TOUCH_PRESSDOWN�����õ�
	uint8_t i;
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx;
	
	tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx = 
			tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;

	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMSCAM1NOR_ICON,1, &tOsdImgInfo);
			if(Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)//sub sub����
			{
				for(i = 0;i < 4;i++)
				{
					//��ȡͼƬ��Ϣ
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMSCAM1NOR_ICON + 2*i,1, &tOsdImgInfo);
					//������ָ������ĸ�ͼ����
					if(Touch_Info->startX > tOsdImgInfo.uwXStart 
						&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
						&& Touch_Info->startY > tOsdImgInfo.uwYStart 
						&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
					{
						UI_CameraSubSubSubMenuExitToSubSub(i);
						tSettingSubSubMenuItem.tSettingS[CAMERASET_ITEM].tSubMenuInfo.ubItemIdx = i;
						UI_EnterKey();
						break;
					}
				}
			}
			else//sub sub sub����
			{
				if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamConnSts == CAM_ONLINE)
				{
					for(i = 0;i < 4;i++)  //Alan
					{
						//��ȡͼƬ��Ϣ
						tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_H_MIRROR_OFF_NOR + 4*i,1, &tOsdImgInfo);
						//������ָ������ĸ�ͼ����
						if(Touch_Info->startX > tOsdImgInfo.uwXStart 
							&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
							&& Touch_Info->startY > tOsdImgInfo.uwYStart 
							&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
						{
							tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = CAMERA_H_MIRROR + i;
							CameraSubSubSubRemoteMode = REMOTE_SELECT_ITEM;
							break;
						}
					}
					if(i < 4)
					{
						UI_CameraDrawSubSubSubMenuItem();
						//ִ��mirror��flip����ز���
						OSD_IMG_INFO tOsdImgInfo[4];
						if(tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx == CAMERA_H_MIRROR)
							tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgMirror = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgMirror;
						else if(tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx == CAMERA_V_MIRROR)
							tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgFlip = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamImgFlip;
						else if(tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx == CAMERA_LASER)
						{
							if(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3) ||
								UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3))
								tUI_CamStatus[uwSubSubMenuItemIdx].tCamLaser = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamLaser;
						}
						else
						{
							if(UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,Laserchar,3) ||
								UI_CheckTxVersion(tUI_CamStatus[uwSubSubMenuItemIdx].cCamVersion,30,heatingchar,3))
								tUI_CamStatus[uwSubSubMenuItemIdx].tCamLed = 1- tUI_CamStatus[uwSubSubMenuItemIdx].tCamLed;
						}
						UI_CameraDrawSubSubSubMenuItem();
						UI_CameraSubSubSubMenuExecute();
						return;
					}
				}
			}
			//return
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
				TIMER_Delay_ms(30);
				UI_CameraSubSubSubMenuExitToSub();
			}
			
			break;
		case TOUCH_PRESSDOWN:
		//������
			if(tUI_CamStatus[uwSubSubMenuItemIdx].tCamConnSts == CAM_ONLINE)
			{
				if(++ ubTouchPressDownCount == 0xff)
					ubTouchPressDownCount = 0;
				if(ubTouchPressDownCount%3 != 0)
					return;
				uint8_t *pUI_ColorParm[5] = {
											(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorBL,
											(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorContrast,
											(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorSaturation,
											(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].tCamColorParam.ubColorHue,
											(uint8_t *)&tUI_CamStatus[uwSubSubMenuItemIdx].ubVolumeLvl
											 };
	
				for(i = 0;i < 5;i++)
				{
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMS_BRIGHTNESS_SLIDER,1,&tOsdImgInfo);
					tOsdImgInfo.uwYStart = CAMERA_SLIDER_Y + CAMERA_Y_STEP*i;
					if(Touch_Info->endX >= tOsdImgInfo.uwXStart - 50 
						&& Touch_Info->endX <= tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 50
						&& Touch_Info->endY >= tOsdImgInfo.uwYStart - 10
						&& Touch_Info->endY <= tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 10)
					{
						if(Touch_Info->endX < tOsdImgInfo.uwXStart)
							Touch_Info->endX = tOsdImgInfo.uwXStart;
						if(Touch_Info->endX > tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize)
							Touch_Info->endX = tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize;
	
						tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = CAMERA_BRIGHTNESS + i;

						//����λ��ת�ɽ�������ֵ
						uint8_t ubValue = 
							(uint8_t)UI_Map(Touch_Info->endX,tOsdImgInfo.uwXStart,tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize,0,99);
	 					uint8_t ubItemIdx = tCameraSubSubSubMenuItem[uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;
	 					(*pUI_ColorParm[ubItemIdx]) = ubValue;
						
						UI_CameraDrawSubSubSubMenuItem();

						UI_CameraSubSubSubMenuExecute();
						break;
					}
				}
			}
			break;
		default:
	
			return;
	}

}
//------------------------------------------------------------------------------------SYSTEM------------------------------------------------------------------------------
void UI_SystemSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t i;
	uint8_t ubSubSubMenuItem = tSystemSubSubMenuItem.tSubMenuInfo.ubItemIdx;
	tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemPreIdx = 
			tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx;
	uint8_t ubMenuItemCount = tSystemSubSubSubMenuItem[ubSubSubMenuItem].ubItemCount;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			if (ubSubSubMenuItem == UPGRADE_ITEM || ubSubSubMenuItem == LANGUAGE_ITEM || ubSubSubMenuItem == DATETIME_ITEM)
			{
				for (i = 0; i < ubMenuItemCount; i ++)
				{
                    if (ubSubSubMenuItem == DATETIME_ITEM)
                    {
						if(i > DATE_TIME_SECOND)//跳过滑动�?
						{						
							i = DATE_TIME_UTC_COMFIRM;
							tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR, 1, &tOsdImgInfo);
						}
						else
						{
//							tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_YEAR_NOR + 2*i,1, &tOsdImgInfo);
							tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_YEAR_NOR, 1, &tOsdImgInfo);
							tOsdImgInfo.uwXStart +=  (i % 3) * DATE_TIME_YEAR_DIST_X;
							if(i > 2)
							{
								tOsdImgInfo.uwYStart += DATE_TIME_YEAR_DIST_Y;
							}

						}
                    }
					else
					{
						//��ȡͼƬ��Ϣ
						tOSD_GetOsdImgInfor(1, OSD_IMG2,((ubSubSubMenuItem == UPGRADE_ITEM)?OSD2IMG_SDCARD_YES_NOR : \
													      OSD2IMG_SYSTEM_ENGLISH_NOR) + 2*i,1, &tOsdImgInfo);
					}


					//������ָ������ĸ�ͼ����
					if(Touch_Info->startX > tOsdImgInfo.uwXStart 
						&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
						&& Touch_Info->startY > tOsdImgInfo.uwYStart 
						&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
					{
						SystemSubSubSubRemoteMode = REMOTE_SELECT_ITEM;
						tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx = i;
						UI_SystemDrawSubSubSubMenuItem(FALSE);
						UI_EnterKey();
						break;
					}
				}
				
			}
			
			//return
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
		case TOUCH_PRESSDOWN:
			if(ubSubSubMenuItem == DATETIME_ITEM)
			{
				if(ubtimezoneFlag==0)
				{
					ubtimezonePre =  tUI_CuSetting.ubtimezone;
					ubtimezoneFlag=1;
				}
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 1,&tOsdImgInfo);

				if(Touch_Info->endX >= tOsdImgInfo.uwXStart - 10 
					&& Touch_Info->endX <= tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 10
					&& Touch_Info->endY >= tOsdImgInfo.uwYStart - 10
					&& Touch_Info->endY <= tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 10)
				{
					if(Touch_Info->endX < tOsdImgInfo.uwXStart)
						Touch_Info->endX = tOsdImgInfo.uwXStart;
					if(Touch_Info->endX > tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize)
						Touch_Info->endX = tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize;

					uint8_t ubValue = 
						(uint8_t)UI_Map(Touch_Info->endX, tOsdImgInfo.uwXStart, tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize, 0, 26);

                    tUI_CuSetting.ubtimezone = ubValue;

					tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx = DATE_TIME_UTC_SELECT;
					//tSystemSubSubSubMenuItem[ubSubSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = DATE_TIME_UTC_SELECT;
					UI_SystemDrawSubSubSubMenuItem(FALSE);
				}
			}
		
			if(ubSubSubMenuItem != AUTO_SET_ITEM)
				return;
			for(i = 0;i < 2;i++)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DAY_SLIDER + i,1,&tOsdImgInfo);
				if(Touch_Info->endX >= tOsdImgInfo.uwXStart - 50 
					&& Touch_Info->endX <= tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 50
					&& Touch_Info->endY >= tOsdImgInfo.uwYStart - 10
					&& Touch_Info->endY <= tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 10)
				{
					if(Touch_Info->endX < tOsdImgInfo.uwXStart)
						Touch_Info->endX = tOsdImgInfo.uwXStart;
					if(Touch_Info->endX > tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize)
						Touch_Info->endX = tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize;

					//����λ��ת�ɽ�������ֵ
					uint8_t ubValue = 
						(uint8_t)UI_Map(Touch_Info->endX,tOsdImgInfo.uwXStart,tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize,0,99);
					
					uint8_t ubItemIdx = DAY_SLIDER + i;
					
	 				if(ubItemIdx == DAY_SLIDER)
					{
						if(ubValue >= tUI_CuSetting.ubAutoDimmer_MinVal)
							tUI_CuSetting.ubAutoDimmer_MaxVal = ubValue;
						else 
							return;
					}
					else
					{
						if(ubValue <= tUI_CuSetting.ubAutoDimmer_MaxVal)
							tUI_CuSetting.ubAutoDimmer_MinVal = ubValue;
						else 
							return;
					}
					tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx = DAY_SLIDER + i;
					if(tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemPreIdx != tSystemSubSubSubMenuItem[ubSubSubMenuItem].tSubMenuInfo.ubItemIdx)
						UI_SystemDrawSubSubSubMenuItem(TRUE);
					else
						UI_SystemDrawSubSubSubMenuItem(FALSE);

					break;
				}
			}
		
			break;
		default:
	
			return;
	}

}

//------------------------------------------------------------------------------------parkingLine------------------------------------------------------------------------------

void UI_ParkingLIneSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tParkingLineOsdImgInfo,tOsdImgInfo;
	static uint8_t ubTouchPressDownCount = 0;//�ñ����Ǹ�TOUCH_PRESSDOWN�����õ�
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[PARKINGLINE_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;	
	uint16_t uwHSize = 0,uwVSize = 0; 
	//����֮ǰ�ĵ������
	UI_EraseParkingLine(CamIdx,OSD_QUEUE);
	
	//��ȡ�������Ĵ�С
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LEFT, 1, &tParkingLineOsdImgInfo);
	uwHSize += 2*tParkingLineOsdImgInfo.uwHSize;
	uwVSize = tParkingLineOsdImgInfo.uwVSize;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_UPPER1 + tUI_CuSetting.ParkingLineSize[CamIdx], 1, &tParkingLineOsdImgInfo);
	uwHSize += tParkingLineOsdImgInfo.uwHSize;
	
	//��������״̬
	tOSD_GetOsdImgInfor(1, OSD_IMG2,OSD2IMG_PARKINGLINE_L_R_FLAG, 1, &tOsdImgInfo);
	tOsdImgInfo.uwXStart = (tUI_CuSetting.ParkingLine_XY[CamIdx][0] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][1] + (uwHSize - tOsdImgInfo.uwHSize)/2;
	tOsdImgInfo.uwYStart = (tUI_CuSetting.ParkingLine_XY[CamIdx][2] << 8) + tUI_CuSetting.ParkingLine_XY[CamIdx][3] + (uwVSize - tOsdImgInfo.uwVSize)/2;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo);

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
			return;
		case TOUCH_TURNDOWN:
			break;
		case TOUCH_PRESSDOWN:
			//�������ڷ���ͼ���ϾͲ�ִ��pressDown����
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);	
			if(Touch_Info->startX > tOsdImgInfo.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
				&& Touch_Info->startY > tOsdImgInfo.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
			{
				return;
			}
			//����pressDown�ٶ�
			if(++ ubTouchPressDownCount == 0xff)
				ubTouchPressDownCount = 0;
			if(ubTouchPressDownCount%3 != 0)
				return;
			if(Touch_Info->pressPoints == 1)//��ָ
			{
				//����
				if((Touch_Info->endX - uwHSize/2 > 0) && (Touch_Info->endX + uwHSize/2 < OSD_WIDTH))
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = (Touch_Info->endX - uwHSize/2) >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = (uint8_t)(Touch_Info->endX - uwHSize/2);

				}
				else if((Touch_Info->endX - uwHSize/2 < 0))
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = 0;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = 0;

				}
				else if(Touch_Info->endX + uwHSize/2 > OSD_WIDTH)
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][0] = (OSD_WIDTH - uwHSize) >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][1] = (uint8_t)(OSD_WIDTH - uwHSize);

				}
				//����
				if((Touch_Info->endY - uwVSize/2 > 0) && (Touch_Info->endY + uwVSize/2 < OSD_HEIGHT))
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = (Touch_Info->endY - uwVSize/2) >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = (uint8_t)(Touch_Info->endY - uwVSize/2);

				}
				else if((Touch_Info->endY - uwVSize/2 < 0))
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = 0;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = 0;

				}
				else if(Touch_Info->endY + uwVSize/2 > OSD_HEIGHT)
				{
					tUI_CuSetting.ParkingLine_XY[CamIdx][2] = (OSD_HEIGHT - uwVSize) >> 8;
					tUI_CuSetting.ParkingLine_XY[CamIdx][3] = (uint8_t)(OSD_HEIGHT - uwVSize);

				}

			}//˫ָ
			else
			{
				double X[2],Y[2];
				X[0] = Touch_Info->endX;
				X[1] = Touch_Info->readX[1];
				Y[0] = Touch_Info->endY;
				Y[1] = Touch_Info->readY[1];
				uint8_t i;
				for(i = 0;i < 11; i ++)
				{
					uwHSize = 0;
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_LEFT, 1, &tParkingLineOsdImgInfo);
					uwHSize += 2*tParkingLineOsdImgInfo.uwHSize;
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_UPPER1 + i, 1, &tParkingLineOsdImgInfo);
					uwHSize += tParkingLineOsdImgInfo.uwHSize;
					if(sqrt((X[0] - X[1])*(X[0] - X[1]) + (Y[0] - Y[1])*(Y[0] - Y[1])) <  uwHSize /2 + 50)
						break;
				}
				tUI_CuSetting.ParkingLineSize[CamIdx] = i;
			}
			break;
		default:

			return;
	}

	UI_ParkingLineDrawSubSubSubMenuItem();
	

}

//------------------------------------------------------------------------------------GuideLine------------------------------------------------------------------------------
void UI_GuideLineSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo[2],tOsdImgInfo_Touch;
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[GUIDELINE_ITEM].tSubMenuInfo.ubItemIdx;
	uint16_t CamIdx = uwSubSubMenuItemIdx/2;	
	uint16_t uwPreVal_X = (tUI_CuSetting.GuideLine_XY[CamIdx][0] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][1];
	uint16_t uwPreVal_Y = (tUI_CuSetting.GuideLine_XY[CamIdx][2] << 8) + tUI_CuSetting.GuideLine_XY[CamIdx][3];
	uint16_t uwTempVal;
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GUIDELINE_SIZE1 + tUI_CuSetting.GuideLineSize[CamIdx], 1, &tOsdImgInfo[0]);
	//����֮ǰ�ĵ�������ң�ز���״̬
	tOsdImgInfo[0].uwXStart = uwPreVal_X;
	tOsdImgInfo[0].uwYStart = uwPreVal_Y;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo[0]);

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_L_R_FLAG, 1, &tOsdImgInfo[1]);
	tOsdImgInfo[1].uwXStart = tOsdImgInfo[0].uwXStart + (tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize)/2;
	if(tOsdImgInfo[0].uwYStart > tOsdImgInfo[1].uwVSize + 20)
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart - tOsdImgInfo[1].uwVSize - 20;
	else
		tOsdImgInfo[1].uwYStart = tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize + 20;
	OSD_EraserImg2_NoUpdate(&tOsdImgInfo[1]);
	
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo_Touch);	
			if(Touch_Info->startX > tOsdImgInfo_Touch.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo_Touch.uwXStart + tOsdImgInfo_Touch.uwHSize
				&& Touch_Info->startY > tOsdImgInfo_Touch.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo_Touch.uwYStart + tOsdImgInfo_Touch.uwVSize)
			{
				tOSD_Img2(&tOsdImgInfo_Touch, OSD_UPDATE);
				TIMER_Delay_ms(30);
				UI_MenuKey();
			}
			return;
		case TOUCH_TURNDOWN:
			break;
		case TOUCH_PRESSDOWN:
			//�������ڷ���ͼ���ϾͲ�ִ��pressDown����
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo_Touch);	
			if(Touch_Info->startX > tOsdImgInfo_Touch.uwXStart 
				&& Touch_Info->startX < tOsdImgInfo_Touch.uwXStart + tOsdImgInfo_Touch.uwHSize
				&& Touch_Info->startY > tOsdImgInfo_Touch.uwYStart 
				&& Touch_Info->startY < tOsdImgInfo_Touch.uwYStart + tOsdImgInfo_Touch.uwVSize)
			{
				return;
			}
			
			if(Touch_Info->pressPoints == 1)//��ָ
			{
				//����
				if((Touch_Info->endX - tOsdImgInfo[0].uwHSize/2 > 0) && (Touch_Info->endX + tOsdImgInfo[0].uwHSize/2 < OSD_WIDTH))
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = (Touch_Info->endX - tOsdImgInfo[0].uwHSize/2) >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = (uint8_t)(Touch_Info->endX - tOsdImgInfo[0].uwHSize/2);

				}
				else if((Touch_Info->endX - tOsdImgInfo[0].uwHSize/2 < 0))
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = 0;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = 0;

				}
				else if(Touch_Info->endX + tOsdImgInfo[0].uwHSize/2 > OSD_WIDTH)
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][0] = (OSD_WIDTH - tOsdImgInfo[0].uwHSize) >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][1] = (uint8_t)(OSD_WIDTH - tOsdImgInfo[0].uwHSize);

				}
				//����
				if((Touch_Info->endY - tOsdImgInfo[0].uwVSize/2 > 0) && (Touch_Info->endY + tOsdImgInfo[0].uwVSize/2 < OSD_HEIGHT))
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = (Touch_Info->endY - tOsdImgInfo[0].uwVSize/2) >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = (uint8_t)(Touch_Info->endY - tOsdImgInfo[0].uwVSize/2);

				}
				else if((Touch_Info->endY - tOsdImgInfo[0].uwVSize/2 < 0))
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = 0;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = 0;

				}
				else if(Touch_Info->endY + tOsdImgInfo[0].uwVSize/2 > OSD_HEIGHT)
				{
					tUI_CuSetting.GuideLine_XY[CamIdx][2] = (OSD_HEIGHT - tOsdImgInfo[0].uwVSize) >> 8;
					tUI_CuSetting.GuideLine_XY[CamIdx][3] = (uint8_t)(OSD_HEIGHT - tOsdImgInfo[0].uwVSize);

				}

			}//˫ָ
			else
			{
				double X[2],Y[2];
				X[0] = Touch_Info->endX;
				X[1] = Touch_Info->readX[1];
				Y[0] = Touch_Info->endY;
				Y[1] = Touch_Info->readY[1];
				uint16_t i;
				for(i = 0;i < 17;i++)
				{
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_GUIDELINE_SIZE1 + i, 1, &tOsdImgInfo_Touch);
					if(sqrt((X[0] - X[1])*(X[0] - X[1]) + (Y[0] - Y[1])*(Y[0] - Y[1])) <  tOsdImgInfo_Touch.uwHSize /2 + 50)
						break;
				}
				tUI_CuSetting.GuideLineSize[CamIdx] = i;
			}
			break;
		default:

			return;
	}

	UI_GuideLineDrawSubSubSubMenuItem();
	

}



//setting�Ĵ󲿷ֽ���Ĳ����߼�����࣬����û����������Ľ��涼ʹ��ͬһ�״�����������
//------------------------------------------------------------------------------------universal------------------------------------------------------------------------------
static uint16_t uwFirstImageIndexOfRecord[] =  //record
{
	0,
	0,
	0,
	OSD2IMG_SDCARD_YES_NOR,
	0,
};

//----------------------------------------------------------------------
static uint16_t uwFirstImageIndexOfTrigger[] =  //trigger
{
	//delay
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	//priority
	OSD2IMG_TRIGGER_PRIORITY1_NOR,
	OSD2IMG_TRIGGER_PRIORITY1_NOR,
	OSD2IMG_TRIGGER_PRIORITY1_NOR,
	OSD2IMG_TRIGGER_PRIORITY1_NOR,
	OSD2IMG_TRIGGER_PRIORITY1_NOR,
	//display setup
	OSD2IMG_SELCAM1NOR_ICON
};
//----------------------------------------------------------------------
static uint16_t uwFirstImageIndexOfAutoScan[] =  //AutoScan
{
	0,//OnOff
	OSD2IMG_KEYBOARD_BUTTON1_NOR,//delay
	0,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	0,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	0,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	0,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	0,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
	0,
	OSD2IMG_KEYBOARD_BUTTON1_NOR,
};
//uwFirstImageIndexOfSubSubSub[][]�����subsubsub����ĵ�һ��ͼƬ
static uint16_t *uwFirstImageIndexOfSubSubSubSetting[] = 
{
	NULL,		//camera
	uwFirstImageIndexOfRecord,//record
	NULL,		//system
	NULL,		//dual
	NULL,		//quad
	uwFirstImageIndexOfTrigger,		//trigger
	uwFirstImageIndexOfAutoScan,	//auto scan
	NULL,							//power on
	NULL,	//parkingLine
};

//----------------------------------------------------------------------

void UI_SettingSubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	UI_SettingSubMenuItemList_t tSubMenuItem = 
		(UI_SettingSubMenuItemList_t)tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;//sub
	uint16_t uwSubSubMenuItemIdx = 
		(uint16_t)tSettingSubSubMenuItem.tSettingS[tSubMenuItem].tSubMenuInfo.ubItemIdx;//sub sub
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i = 0;
	
	uint16_t ubSubSubSubMenuItemCount = 
		tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->ubItemCount;
	
	UI_SettingUpdateSubSubSubMenuItemIndex(tSubMenuItem,uwSubSubMenuItemIdx);
	
	tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx = 
		tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx;
	
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			if(uwFirstImageIndexOfSubSubSubSetting[tSubMenuItem][uwSubSubMenuItemIdx] == 0)
				return;
			for(i = 0; i < ubSubSubSubMenuItemCount;i++)//�жϾ��������ĸ�ͼ����
			{
				//��ȡͼƬ��Ϣ
				if(tSubMenuItem == RECORDSET_ITEM && uwSubSubMenuItemIdx == DATETIME_ITEM && i > DATE_TIME_SECOND)//����������
				{						
					i = DATE_TIME_UTC_COMFIRM;
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DATE_TIME_UTC_COMFIRM_NOR,1, &tOsdImgInfo);
				}
				else
					tOSD_GetOsdImgInfor(1, OSD_IMG2, uwFirstImageIndexOfSubSubSubSetting[tSubMenuItem][uwSubSubMenuItemIdx] + 2*i,1, &tOsdImgInfo);
				
				//������ָ������ĸ�ͼ����
				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = i;
					SettingSubSubSubRemoteMode = REMOTE_SELECT_ITEM;
					break;
				}
			}
			if(i < ubSubSubSubMenuItemCount)
			{
				if(tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemPreIdx != 
							tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx)
				{
					UI_SettingingDrawSubSubSubMenuItem(tSubMenuItem,uwSubSubMenuItemIdx);
				}
				UI_EnterKey();
			}
			else//return
			{
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
			}
			break;
			
		case TOUCH_TURNDOWN:
			break;
		case TOUCH_PRESSDOWN:
			if(tSubMenuItem == RECORDSET_ITEM && uwSubSubMenuItemIdx == DATETIME_ITEM)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_TIME_ZONE_SLIDER, 1,&tOsdImgInfo);

				if(Touch_Info->endX >= tOsdImgInfo.uwXStart - 10 
					&& Touch_Info->endX <= tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize + 10
					&& Touch_Info->endY >= tOsdImgInfo.uwYStart - 10
					&& Touch_Info->endY <= tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize + 10)
				{
					if(Touch_Info->endX < tOsdImgInfo.uwXStart)
						Touch_Info->endX = tOsdImgInfo.uwXStart;
					if(Touch_Info->endX > tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize)
						Touch_Info->endX = tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize;
                    

					uint8_t ubValue = 
						(uint8_t)UI_Map(Touch_Info->endX, tOsdImgInfo.uwXStart, tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize, 0, 26);

                    tUI_CuSetting.ubtimezone = ubValue;

					tSettingSubSubSubMenuItem[tSubMenuItem][uwSubSubMenuItemIdx]->tSubMenuInfo.ubItemIdx = DATE_TIME_UTC_SELECT;
					UI_SettingingDrawSubSubSubMenuItem(RECORDSET_ITEM, DATETIME_ITEM);
				}
			}
			break;
		default:

			return;
	}

}


static uint8_t ubAIRangeCam, ubAIRangeItem, ubAIRangePoint, ubAIRangeSelected;
static UI_LinePoint_t tAIRangeHandle[AI_RANGE_HANDLE_MAX];

static void UI_AIDrawRangeLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, OSD_IMG_INFO *ptOsdImgInfo, uint8_t ubHorizontal, OSD_LAYER_TYP tOsdLayer)
{
	int16_t dx = abs(x2 - x1), dy = abs(y2 - y1);
	int16_t xmid, ymid;
	if(ubHorizontal)
	{
		if(dx < 4 && dy < 4)
			return;
		xmid = ((x1 < x2)?x1:x2) + dx/2;
		ymid = ((y1 < y2)?y1:y2) + dy/2;
	}
	else
	{
		if(dx*dx + dy*dy < 16)
			return;
		xmid = x1 + (x2 - x1)/2;
		ymid = y1 + (y2 - y1)/2;
	}
	ptOsdImgInfo->uwXStart = xmid;
	ptOsdImgInfo->uwYStart = ymid;
	if(tOsdLayer == OSD_IMG1)
		tOSD_Img1_NoCopy(ptOsdImgInfo, OSD_QUEUE);
	else
		tOSD_Img2(ptOsdImgInfo, OSD_QUEUE);
	UI_AIDrawRangeLine(x1, y1, xmid, ymid, ptOsdImgInfo, ubHorizontal, tOsdLayer);
	UI_AIDrawRangeLine(xmid, ymid, x2, y2, ptOsdImgInfo, ubHorizontal, tOsdLayer);
}

void UI_AIDrawRangeLines(UI_LinePoint_t tLinePoint[4][2], OSD_LAYER_TYP tOsdLayer)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t uwImage[3] = {OSD2IMG_AI_RANGE_GREEN, OSD2IMG_AI_RANGE_YELLOW, OSD2IMG_AI_RANGE_RED};
	for(uint8_t i = 0;i < 3;i++)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwImage[i], 1, &tOsdImgInfo);
		if(i == 0)
			UI_AIDrawRangeLine(tLinePoint[0][0].xPoint, tLinePoint[0][0].yPoint, tLinePoint[0][1].xPoint, tLinePoint[0][1].yPoint, &tOsdImgInfo, TRUE, tOsdLayer);
		for(uint8_t j = 0;j < 2;j++)
			UI_AIDrawRangeLine(tLinePoint[i][j].xPoint, tLinePoint[i][j].yPoint, tLinePoint[i + 1][j].xPoint, tLinePoint[i + 1][j].yPoint, &tOsdImgInfo, FALSE, tOsdLayer);
		UI_AIDrawRangeLine(tLinePoint[i + 1][0].xPoint, tLinePoint[i + 1][0].yPoint, tLinePoint[i + 1][1].xPoint, tLinePoint[i + 1][1].yPoint, &tOsdImgInfo, TRUE, tOsdLayer);
	}
}

static void UI_AIDrawRange(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	UI_LinePoint_t tLinePoint[4][2];
	UI_ParkinglinePoint_t tRange = tUI_CuSetting.tUI_AIDetectlinePoint[ubAIRangeCam];
	UI_LinePoint_t *ptPoint[AI_RANGE_POINT_MAX] = {
		&tRange.tupLeft, &tRange.tupRight, &tRange.tdownLeft, &tRange.tdownRight
	};
	uint16_t uwButton[AI_RANGE_ITEM_MAX] = {
		OSD2IMG_CAMS_H_MIRROR_OFF_NOR + 2*tUI_CuSetting.ubIsEnableBSDRANGE[ubAIRangeCam],
		OSD2IMG_AI_CHOOSE_UP_NOR, OSD2IMG_AI_CHOOSE_LEFT_NOR, OSD2IMG_AI_CHOOSE_OK_NOR,
		OSD2IMG_AI_CHOOSE_RIGHT_NOR, OSD2IMG_AI_CHOOSE_DOWN_NOR, OSD2IMG_AI_RESET_NOR
	};
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
	OSD_EraserImg1_NOUpdate(&tOsdImgInfo);
	for(uint8_t i = 0;i < AI_RANGE_POINT_MAX;i++)
	{
		uint8_t ubPosition = (i == 0)?0:(i == 3)?AI_RANGE_LINE_STEPS:((i == 1)?tUI_CuSetting.tLocation1[ubAIRangeCam]:tUI_CuSetting.tLocation2[ubAIRangeCam]);
		for(uint8_t j = 0;j < 2;j++)
		{
			tLinePoint[i][j].xPoint = ptPoint[j]->xPoint + (ptPoint[j + 2]->xPoint - ptPoint[j]->xPoint)*ubPosition/AI_RANGE_LINE_STEPS;
			tLinePoint[i][j].yPoint = ptPoint[j]->yPoint + (ptPoint[j + 2]->yPoint - ptPoint[j]->yPoint)*ubPosition/AI_RANGE_LINE_STEPS;
		}
	}
	UI_AIDrawRangeLines(tLinePoint, OSD_IMG2);
	for(uint8_t i = 0;i < AI_RANGE_HANDLE_MAX;i++)
	{
		if(i < AI_RANGE_POINT_MAX)
		{
			tAIRangeHandle[i] = *ptPoint[i];
		}
		else
		{
			uint8_t ubLine = (i == 4)?1:2;
			tAIRangeHandle[i].xPoint = (tLinePoint[ubLine][0].xPoint + tLinePoint[ubLine][1].xPoint)/2;
			tAIRangeHandle[i].yPoint = tLinePoint[ubLine][0].yPoint;
			if(i == 6)
			{
				tAIRangeHandle[i].xPoint = (tAIRangeHandle[5].xPoint - tAIRangeHandle[4].xPoint)/2 + tAIRangeHandle[4].xPoint;
				tAIRangeHandle[i].yPoint = (tAIRangeHandle[5].yPoint - tAIRangeHandle[4].yPoint)/2 + tAIRangeHandle[4].yPoint;
			}
		}
		if(i == ubAIRangePoint)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AI_POINT_UNSELECT + ubAIRangeSelected, 1, &tOsdImgInfo);
			int16_t X_temp = tAIRangeHandle[i].xPoint - tOsdImgInfo.uwHSize/2;
			int16_t Y_temp = tAIRangeHandle[i].yPoint - tOsdImgInfo.uwVSize/2;
			if(X_temp < 0)
				X_temp = 0;
			else if(X_temp > OSD_WIDTH - tOsdImgInfo.uwHSize)
				X_temp = OSD_WIDTH - tOsdImgInfo.uwHSize;
			if(Y_temp < 0)
				Y_temp = 0;
			else if(Y_temp > OSD_HEIGHT - tOsdImgInfo.uwVSize)
				Y_temp = OSD_HEIGHT - tOsdImgInfo.uwVSize;
			tOsdImgInfo.uwXStart = X_temp;
			tOsdImgInfo.uwYStart = Y_temp;
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		}
	}
	for(uint8_t i = 0;i < AI_RANGE_ITEM_MAX;i++)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwButton[i] + (i == ubAIRangeItem), 1, &tOsdImgInfo);
		if(i == AI_RANGE_ENABLE)
		{
			tOsdImgInfo.uwXStart = 30;
			tOsdImgInfo.uwYStart = 20;
		}
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
	}
	OSD_UpdateImg2();
}

void UI_AIEnterRange(uint8_t ubCam)
{
	ubAIRangeCam = ubCam;
	ubAIRangeItem = AI_RANGE_ENABLE;
	ubAIRangePoint = 0;
	ubAIRangeSelected = FALSE;
	UI_SwitchViewType((UI_CamNum_t)ubCam, TRUE);
	tUI_State = UI_SUBSUBSUBMENU_STATE;
	UI_AIDrawRange();
}

void UI_AISubSubSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	if(tArrowKey == EXIT_ARROW)
	{
		UI_UpdateDevStatusInfo();
		UI_SwitchViewType(DeskTopShowView, FALSE);
		UI_AISubMenuExecute();
		return;
	}
	if(tArrowKey == LEFT_ARROW)
		ubAIRangeItem = (ubAIRangeItem == 0)?AI_RANGE_ITEM_MAX - 1:ubAIRangeItem - 1;
	else if(tArrowKey == RIGHT_ARROW)
		ubAIRangeItem = (ubAIRangeItem + 1)%AI_RANGE_ITEM_MAX;
	else if(tArrowKey == ENTER_ARROW)
	{
		if(ubAIRangeItem == AI_RANGE_ENABLE)
			tUI_CuSetting.ubIsEnableBSDRANGE[ubAIRangeCam] = 1 - tUI_CuSetting.ubIsEnableBSDRANGE[ubAIRangeCam];
		else if(ubAIRangeItem == AI_RANGE_RESET)
		{
			tUI_CuSetting.tUI_AIDetectlinePoint[ubAIRangeCam] = (UI_ParkinglinePoint_t)AI_RANGE_DEFAULT_POINT;
			tUI_CuSetting.tLocation1[ubAIRangeCam] = AI_RANGE_DEFAULT_LINE1;
			tUI_CuSetting.tLocation2[ubAIRangeCam] = AI_RANGE_DEFAULT_LINE2;
			ubAIRangePoint = 0;
			ubAIRangeSelected = FALSE;
		}
		else if(ubAIRangeItem == AI_RANGE_OK)
			ubAIRangeSelected = 1 - ubAIRangeSelected;
		else if(!ubAIRangeSelected)
			ubAIRangePoint = (ubAIRangePoint + ((ubAIRangeItem == AI_RANGE_LEFT || ubAIRangeItem == AI_RANGE_UP)?AI_RANGE_HANDLE_MAX - 1:1))%AI_RANGE_HANDLE_MAX;
		else if(ubAIRangePoint == 4 || ubAIRangePoint == 5)
		{
			uint8_t *pubLocation = (ubAIRangePoint == 4)?&tUI_CuSetting.tLocation1[ubAIRangeCam]:&tUI_CuSetting.tLocation2[ubAIRangeCam];
			int16_t ubPosition = *pubLocation +
				((ubAIRangeItem == AI_RANGE_LEFT || ubAIRangeItem == AI_RANGE_UP)?-1:1);
			if(ubPosition > 0 && ubPosition < AI_RANGE_LINE_STEPS && (ubAIRangePoint == 4?
				ubPosition < tUI_CuSetting.tLocation2[ubAIRangeCam]:ubPosition > tUI_CuSetting.tLocation1[ubAIRangeCam]))
				*pubLocation = ubPosition;
		}
		else
		{
			UI_ParkinglinePoint_t tRange = tUI_CuSetting.tUI_AIDetectlinePoint[ubAIRangeCam];
			UI_LinePoint_t *ptPoint[AI_RANGE_POINT_MAX] = {
				&tRange.tupLeft, &tRange.tupRight, &tRange.tdownLeft, &tRange.tdownRight
			};
			int16_t dx = (ubAIRangeItem == AI_RANGE_LEFT)?-AI_RANGE_MOVE_STEP:(ubAIRangeItem == AI_RANGE_RIGHT)?AI_RANGE_MOVE_STEP:0;
			int16_t dy = (ubAIRangeItem == AI_RANGE_UP)?-AI_RANGE_MOVE_STEP:(ubAIRangeItem == AI_RANGE_DOWN)?AI_RANGE_MOVE_STEP:0;
			uint8_t ubValid = TRUE;
			for(uint8_t i = 0;i < AI_RANGE_POINT_MAX;i++)
			{
				if(ubAIRangePoint == 6 && ((dx < 0 && ptPoint[i]->xPoint <= AI_RANGE_MOVE_STEP) ||
					(dx > 0 && ptPoint[i]->xPoint >= OSD_WIDTH - AI_RANGE_MOVE_STEP)))
					ubValid = FALSE;
				if(i == ubAIRangePoint || ubAIRangePoint == 6)
				{
					ptPoint[i]->xPoint += dx;
					if(ptPoint[i]->xPoint < 0)
						ptPoint[i]->xPoint -= dx;
					if(ptPoint[i]->xPoint > OSD_WIDTH - AI_RANGE_MOVE_STEP)
						ptPoint[i]->xPoint = OSD_WIDTH - AI_RANGE_MOVE_STEP;
				}
				if(i/2 == ubAIRangePoint/2 || ubAIRangePoint == 6)
					ptPoint[i]->yPoint += dy;
			}
			if(tRange.tupLeft.yPoint < 0 || tRange.tdownLeft.yPoint > OSD_HEIGHT - 4 ||
				tRange.tdownLeft.yPoint < tRange.tupLeft.yPoint)
				ubValid = FALSE;
			if(ubValid)
				tUI_CuSetting.tUI_AIDetectlinePoint[ubAIRangeCam] = tRange;
		}
	}
	else
		return;
	UI_AIDrawRange();
}

void UI_AISubSubSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t uwButton[AI_RANGE_ITEM_MAX] = {OSD2IMG_CAMS_H_MIRROR_OFF_NOR, OSD2IMG_AI_CHOOSE_UP_NOR,
		OSD2IMG_AI_CHOOSE_LEFT_NOR, OSD2IMG_AI_CHOOSE_OK_NOR, OSD2IMG_AI_CHOOSE_RIGHT_NOR,
		OSD2IMG_AI_CHOOSE_DOWN_NOR, OSD2IMG_AI_RESET_NOR};
	if(Touch_Info->Gesture != TOUCH_PRESS)
		return;
	for(uint8_t i = 0;i < AI_RANGE_ITEM_MAX;i++)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwButton[i], 1, &tOsdImgInfo);
		if(i == AI_RANGE_ENABLE)
		{
			tOsdImgInfo.uwXStart = 30;
			tOsdImgInfo.uwYStart = 20;
		}
		if(Touch_Info->startX > tOsdImgInfo.uwXStart && Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize &&
			Touch_Info->startY > tOsdImgInfo.uwYStart && Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
		{
			ubAIRangeItem = i;
			UI_AISubSubSubMenuPage(ENTER_ARROW);
			return;
		}
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_HL, 1, &tOsdImgInfo);
	if(Touch_Info->startX > tOsdImgInfo.uwXStart && Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize &&
		Touch_Info->startY > tOsdImgInfo.uwYStart && Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
	{
		UI_AISubSubSubMenuPage(EXIT_ARROW);
		return;
	}
	for(uint8_t i = 0;i < AI_RANGE_HANDLE_MAX;i++)
	{
		if(abs((int)Touch_Info->startX - tAIRangeHandle[i].xPoint) < 24 && abs((int)Touch_Info->startY - tAIRangeHandle[i].yPoint) < 24)
		{
			ubAIRangePoint = i;
			ubAIRangeSelected = TRUE;
			UI_AIDrawRange();
			return;
		}
	}
}

#endif
