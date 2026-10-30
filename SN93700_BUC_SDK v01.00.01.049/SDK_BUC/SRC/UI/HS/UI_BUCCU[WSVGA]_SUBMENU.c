#include "UI_BUCCU[WSVGA]_SUBMENU.h"
#ifdef BSP_D_SNCC71_GM8285C_RX_V2

extern uint8_t isHeatingCamAndOnline[4];
extern uint8_t is_heating[4];
extern uint8_t heating_level[4];
extern uint16_t HeatingYesNoPos[4][2];
extern uint16_t HeatingPageItemPos[16][4];

static uint8_t isRollBackZoomSubMenu = FALSE;


void UI_AdoSelSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_CamSelSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_PairingSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_PlaybackSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_SettingSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_HeatingSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_PowerSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_ZoomSubTouchMenu(TOUCH_EVENT_t *Touch_Info);
void UI_SettingSubMenuPage(UI_ArrowKey_t tArrowKey);


//------------------------------------------------------------------------------
UI_MenuAct_t UI_KeyEventMap2SubMenuInfo(UI_ArrowKey_t *ptArrowKey, UI_SubMenuItem_t *ptSubMenu)
{
	uint8_t ubCurrentSubMenuItemIdx = ptSubMenu->tSubMenuInfo.ubItemIdx;
	uint8_t ubNextSubMenuItemIdx = 0;

	switch(*ptArrowKey)
	{
		case LEFT_ARROW:
			if(ubCurrentSubMenuItemIdx <= ptSubMenu->ubFirstItem)
				ubNextSubMenuItemIdx = ptSubMenu->ubItemCount -1;
			else
				ubNextSubMenuItemIdx = ubCurrentSubMenuItemIdx - 1;
			break;
		case RIGHT_ARROW:
			if((ubCurrentSubMenuItemIdx+1) >= ptSubMenu->ubItemCount)
				ubNextSubMenuItemIdx = ptSubMenu->ubFirstItem;
			else
				ubNextSubMenuItemIdx = ubCurrentSubMenuItemIdx + 1;
			break;
		case ENTER_ARROW:
			return DRAW_MENUPAGE;
		case EXIT_ARROW:
			return EXIT_MENUFUNC;
		default:
			return NOT_ACTION;
	}
	ptSubMenu->tSubMenuInfo.ubItemPreIdx = ubCurrentSubMenuItemIdx;
	ptSubMenu->tSubMenuInfo.ubItemIdx    = ubNextSubMenuItemIdx;
	return DRAW_HIGHLIGHT_MENUICON;
}
//------------------------------------------------------------------------------
UI_MenuAct_t UI_HeatingKeyEventMap2SubMenuInfo(UI_ArrowKey_t *ptArrowKey, UI_SubMenuItem_t *ptSubMenu)
{
	uint8_t ubCurrentSubMenuItemIdx = ptSubMenu->tSubMenuInfo.ubItemIdx;
	uint8_t ubNextSubMenuItemIdx = 0;
	uint8_t cam_i = 0, cam_next = 0;

	switch(*ptArrowKey)
	{
		case LEFT_ARROW:
			if(ubCurrentSubMenuItemIdx <= ptSubMenu->ubFirstItem)
				ubNextSubMenuItemIdx = ptSubMenu->ubItemCount - 1;
			else
				ubNextSubMenuItemIdx = ubCurrentSubMenuItemIdx - 1;
			break;
		case RIGHT_ARROW:
			if((ubCurrentSubMenuItemIdx+1) >= ptSubMenu->ubItemCount)
				ubNextSubMenuItemIdx = ptSubMenu->ubFirstItem;
			else
				ubNextSubMenuItemIdx = ubCurrentSubMenuItemIdx + 1;
			break;
		case ENTER_ARROW:
			return DRAW_MENUPAGE;
		case EXIT_ARROW:
			return EXIT_MENUFUNC;
		default:
			return NOT_ACTION;
	}
	
	// cam off line when stay in heating page will set ubItemIdx = 255
	if(ubCurrentSubMenuItemIdx == 255)
	{
		for(uint8_t i=0; i<4; ++i)
		{
			if(isHeatingCamAndOnline[i])
			{
				ptSubMenu->tSubMenuInfo.ubItemPreIdx = 4*i;
				ptSubMenu->tSubMenuInfo.ubItemIdx = 4*i;
				return DRAW_HIGHLIGHT_MENUICON;
			}
		}
		return NOT_ACTION;
	}
	
	if(!isHeatingCamAndOnline[ubCurrentSubMenuItemIdx/4]) {	// not any online heating cam
		return NOT_ACTION;
	}
		
	cam_i = ubNextSubMenuItemIdx/4;
	if(!isHeatingCamAndOnline[cam_i])
	{
		if(*ptArrowKey == LEFT_ARROW){
			for(uint8_t i=1; i<4; ++i)
			{
				cam_next = (cam_i+4-i)%4;
				if(isHeatingCamAndOnline[cam_next])
				{
					ubNextSubMenuItemIdx = cam_next*4 + 3;
					break;
				}
			}
		}else{
			for(uint8_t i=1; i<4; ++i)
			{
				cam_next = (cam_i+i)%4;
				if(isHeatingCamAndOnline[cam_next])
				{
					ubNextSubMenuItemIdx = cam_next*4;
					break;
				}
			}
		}
	}
	
	ptSubMenu->tSubMenuInfo.ubItemPreIdx = ubCurrentSubMenuItemIdx;
	ptSubMenu->tSubMenuInfo.ubItemIdx    = ubNextSubMenuItemIdx;
	
	return DRAW_HIGHLIGHT_MENUICON;
}
//------------------------------------------------------------------------------
void UI_DrawAdoSelSubMenuPage(void)
{
	OSD_IMG_INFO tOsdImgInfo[8];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELADOCAM1_NOR_ICON, 8, &tOsdImgInfo[0]);
	
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);	
	tOSD_Img2(&tOsdImgInfo[2*UI_GetAudioSrc() + UI_ICON_HIGHLIGHT], OSD_UPDATE);
	tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemIdx = UI_GetAudioSrc();		

}
//------------------------------------------------------------------------------

void UI_DrawCamSelSubMenuPage(void)
{
	OSD_IMG_INFO tOsdImgInfo[VDO_VIEW_MAX*2];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1NOR_ICON, VDO_VIEW_MAX*2, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < VDO_VIEW_MAX;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
					
	tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemIdx = tUI_ViewModeSel;			
	tOSD_Img2(&tOsdImgInfo[2*tUI_ViewModeSel + UI_ICON_HIGHLIGHT], OSD_UPDATE);
}
//------------------------------------------------------------------------------

void UI_DrawPairingSubMenuPage(void)
{
	OSD_IMG_INFO tOsdImgInfo[16],tCamRdyOsdImgInfo;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_PAIRING_TITLE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_PAIRING_TITLE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_PAIRING_TITLE_FR:OSD2IMG_PAIRING_TITLE_CHN, 3, &tOsdImgInfo[0]);
	//word
	for(uint8_t i = 0;i < 3;i++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);

	//cam1 ~cam4
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_PAIRCAM1NOR_ICON:OSD2IMG_PAIRCAM1NOR_ICON_GER, 8, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 8;i++)
	{
		tOsdImgInfo[8 + i] = tOsdImgInfo[i];
		tOsdImgInfo[8 + i].uwYStart = PAIRING_Y + PAIRING_Y_STEP;
	}
	for(uint8_t i = 0;i < 8;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
	//pairing success flag
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tCamRdyOsdImgInfo);
	for(uint16_t i = 0;i < 4;i++)
	{
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
		if(tUI_CamStatus[i].ulCAM_ID != INVALID_ID)
		{
			tCamRdyOsdImgInfo.uwXStart = tOsdImgInfo[2*i].uwXStart + (tOsdImgInfo[2*i].uwHSize - tCamRdyOsdImgInfo.uwHSize)/2;
			tOSD_Img2(&tCamRdyOsdImgInfo, OSD_QUEUE);
		}
	}	
	//highLight
	tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
}



//------------------------------------------------------------------------------
void UI_SearchDCIMFolder(void)
{
	uint16_t uwLdState = UI_SEARCH_DCIMFOLDER;

	UI_SetLdDispStatus(UI_OSDLDDISP_ON);
	osMessagePut(osUI_OsdLdDispQueue, &uwLdState, 0);
	osDelay(200);
	tUI_RecFoldersInfo.uwTotalRecFolderNum = ulKNL_GetSortingFolders(KNL_REAL_FLD,SORT_BY_NAME_DESCENDING,&tUI_RecFoldersInfo.tRecFolderInfo[0]);
	UI_SetLdDispStatus(UI_OSDLDDISP_OFF);
	osDelay(350);
}

//------------------------------------------------------------------------------
void UI_ListDCIMFolderInfo(uint16_t uwStartFileIdx, uint16_t uwEndFileIdx, OSD_UPDATE_TYP tUpdateMode)
{
	OSD_IMG_INFO tRecOsdImgInfo[5],tEraseOsdImgInfo;
	uint16_t uwRecXStart[2] = {0, 0}, uwRecYStart[2] = {0, 0};
	uint8_t ubRecFolderIdx, ubOpenOsdIdx = 0;
	uint16_t uwIdx;

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_REC_FOLDERCLOSE_ICON, 2, &tRecOsdImgInfo[0]);

	uwRecXStart[0] = tRecOsdImgInfo[0].uwXStart;
	uwRecYStart[0] = tRecOsdImgInfo[0].uwYStart;
	uwRecXStart[1] = tRecOsdImgInfo[1].uwXStart;
	uwRecYStart[1] = tRecOsdImgInfo[1].uwYStart;
	for(uwIdx = uwStartFileIdx; uwIdx < uwEndFileIdx; uwIdx++)
	{
		UI_SetRecImgColor(UI_RECIMG_COLOR1);
		ubOpenOsdIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx == uwIdx)?1:0;
		ubRecFolderIdx = uwIdx % REC_FOLDER_LIST_MAXNUM;
		
		tRecOsdImgInfo[ubOpenOsdIdx].uwXStart += ((ubRecFolderIdx % 5) * 180);
		tRecOsdImgInfo[ubOpenOsdIdx].uwYStart += ((ubRecFolderIdx / 5) * 220);
		tOSD_Img2(&tRecOsdImgInfo[ubOpenOsdIdx], OSD_QUEUE);
		
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, (72 + ((ubRecFolderIdx % 5) * 180)), (225 + ((ubRecFolderIdx / 5) * 220)), 
			tUI_RecOsdImgInfo, OSD_QUEUE, tUI_RecFoldersInfo.tRecFolderInfo[uwIdx].FldName.chName);
		
		tRecOsdImgInfo[ubOpenOsdIdx].uwXStart = uwRecXStart[ubOpenOsdIdx];
		tRecOsdImgInfo[ubOpenOsdIdx].uwYStart = uwRecYStart[ubOpenOsdIdx];
	}
	//页码和换页按钮
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAGE_SLASH, 5, &tRecOsdImgInfo[0]);
	
	tOSD_Img2(&tRecOsdImgInfo[0], OSD_QUEUE);
	
	uint8_t ubTotalPageNum,ubCurrentPageNum;
	ubTotalPageNum = tUI_RecFoldersInfo.uwTotalRecFolderNum / REC_FOLDER_LIST_MAXNUM + ((tUI_RecFoldersInfo.uwTotalRecFolderNum % REC_FOLDER_LIST_MAXNUM)?1:0);
	ubCurrentPageNum = (tUI_RecFoldersInfo.uwRecFolderSelIdx + 1) / REC_FOLDER_LIST_MAXNUM + (((tUI_RecFoldersInfo.uwRecFolderSelIdx + 1) % REC_FOLDER_LIST_MAXNUM)?1:0);
	
	tEraseOsdImgInfo.uwXStart = tRecOsdImgInfo[0].uwXStart - 42;//擦除数值再重新显示，避免有残留
	tEraseOsdImgInfo.uwYStart = tRecOsdImgInfo[0].uwYStart + 16;
	tEraseOsdImgInfo.uwHSize  = 14 * 3;
	tEraseOsdImgInfo.uwVSize  = 28;
	OSD_EraserImg2_NoUpdate(&tEraseOsdImgInfo);
	
	if(ubCurrentPageNum < 10)
		UI_ShowPlaybackPageNum(ubCurrentPageNum,tRecOsdImgInfo[0].uwXStart - 14,tRecOsdImgInfo[0].uwYStart + 16,OSD_QUEUE);
	else if(ubCurrentPageNum < 100)
		UI_ShowPlaybackPageNum(ubCurrentPageNum,tRecOsdImgInfo[0].uwXStart - 28,tRecOsdImgInfo[0].uwYStart + 16,OSD_QUEUE);
	else if(ubCurrentPageNum < 1000)
		UI_ShowPlaybackPageNum(ubCurrentPageNum,tRecOsdImgInfo[0].uwXStart - 42,tRecOsdImgInfo[0].uwYStart + 16,OSD_QUEUE);

	UI_ShowPlaybackPageNum(ubTotalPageNum,tRecOsdImgInfo[0].uwXStart + 24,tRecOsdImgInfo[0].uwYStart + 16,tUpdateMode);
	if(ubUI_TouchPanelSts)
	{
		tOSD_Img2(&tRecOsdImgInfo[1], OSD_QUEUE);
		tOSD_Img2(&tRecOsdImgInfo[3], tUpdateMode);
	}
}
//------------------------------------------------------------------------------

void UI_DrawDCIMFolderMenu(void)
{
	OSD_IMG_INFO tRecBgOsdImgInfo;

	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tRecBgOsdImgInfo);
	OSD_Weight(OSD_WEIGHT_8DIV8);
	tOSD_Img1(&tRecBgOsdImgInfo, OSD_QUEUE);
	
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tRecBgOsdImgInfo);
		tOSD_Img2(&tRecBgOsdImgInfo, OSD_QUEUE);
	}
	UI_SearchDCIMFolder();
	
	//显示文件夹信息
	if(tUI_RecFoldersInfo.uwTotalRecFolderNum)
	{
		uint8_t ubFldStartIdx = 0, ubFldEndIdx = 0;
		if(tUI_RecFoldersInfo.uwRecFolderSelIdx == 0)
		{
			ubFldStartIdx = 0;
			ubFldEndIdx = (tUI_RecFoldersInfo.uwTotalRecFolderNum < REC_FOLDER_LIST_MAXNUM)?tUI_RecFoldersInfo.uwTotalRecFolderNum:REC_FOLDER_LIST_MAXNUM;
		}
		else
		{
			ubFldStartIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;//该界面第一个文件夹的序号
			
			ubFldEndIdx = tUI_RecFoldersInfo.uwTotalRecFolderNum - ubFldStartIdx;//ubFldStartIdx之后总剩余文件夹数量
			ubFldEndIdx = (ubFldEndIdx < REC_FOLDER_LIST_MAXNUM)?tUI_RecFoldersInfo.uwTotalRecFolderNum:(ubFldStartIdx + REC_FOLDER_LIST_MAXNUM);

		}
		UI_ListDCIMFolderInfo(ubFldStartIdx, ubFldEndIdx, OSD_UPDATE);
	}
		
}

void UI_DrawPlaybackSubMenuPage(void)
{
	UI_DrawDCIMFolderMenu();
}
//------------------------------------------------------------------------------

void UI_DrawSettingSubMenuPage(void)
{
	OSD_IMG_INFO tOsdImgInfo[22];

	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_CAMERA_SETTING_WORD:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_CAMERA_SETTING_WORD_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_CAMERA_SETTING_WORD_FR:OSD2IMG_CAMERA_SETTING_WORD_CHN, SETTINGITEM_MAX, &tOsdImgInfo[0]);
	//WORD
	for(uint8_t i = 0;i < SETTINGITEM_MAX;i++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);

	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMERA_SETTING_NOR_ICON, SETTINGITEM_MAX*2, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < SETTINGITEM_MAX;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
	

	tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx = CAMERASET_ITEM;

}
//------------------------------------------------------------------------------
void UI_DrawHeatingSubMenuPage(void)
{	
	uint8_t i, j;
	OSD_IMG_INFO *p_osdImgInfo;
	OSD_IMG_INFO tOsdImgInfo[10];
	
	// cal pos if not init
	if(HeatingPageItemPos[0][0] == 0){
		uint16_t *array;
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 1, &tOsdImgInfo[0]);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_3MIN_NOR, 1, &tOsdImgInfo[1]);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_NO, 1, &tOsdImgInfo[2]);
		HeatingYesNoPos[0][1] = 160;
		HeatingYesNoPos[1][1] = 160 + (51 + 38);
		HeatingYesNoPos[2][1] = 160 + 2*(51 + 38);
		HeatingYesNoPos[3][1] = 160 + 3*(51 + 39);
		for(i=0; i<4; ++i){
			// yes or no
			HeatingYesNoPos[i][0] = 110;
			
			// on off
			array = &HeatingPageItemPos[4*i][0];
			array[0] = 280;
			array[1] = 165 + i*(tOsdImgInfo[0].uwVSize + 50);
			array[2] = tOsdImgInfo[0].uwHSize;
			array[3] = tOsdImgInfo[0].uwVSize;
			// min
			for(j=0; j<3; ++j){
				array = &HeatingPageItemPos[4*i+j+1][0];
				array[0] = 430+j*(40+tOsdImgInfo[1].uwHSize);
				array[1] = 160+i*(30+tOsdImgInfo[1].uwVSize);
				array[2] = tOsdImgInfo[1].uwHSize;
				array[3] = tOsdImgInfo[1].uwVSize;
			}
		}
	}

	// title
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_HEATING_TITLE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_HEATING_TITLE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_HEATING_TITLE_FR:OSD2IMG_HEATING_TITLE_CHN, 1, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, 
		(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN) ? OSD2IMG_AUTOSCAN_CAM1_WORD_GER : OSD2IMG_AUTOSCAN_CAM1_WORD, 
		4, &tOsdImgInfo[0]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tOsdImgInfo[4]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_NO, 1, &tOsdImgInfo[5]);
	
	for(i=0; i<4; ++i)
	{
		// yes or no
		if(isHeatingCamAndOnline[i])
		{
			tOsdImgInfo[4].uwXStart = HeatingYesNoPos[i][0];
			tOsdImgInfo[4].uwYStart = HeatingYesNoPos[i][1];
			tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);
		}else{
			tOsdImgInfo[5].uwXStart = HeatingYesNoPos[i][0];
			tOsdImgInfo[5].uwYStart = HeatingYesNoPos[i][1];
			tOSD_Img2(&tOsdImgInfo[5], OSD_QUEUE);
		}
		
		// CAM1-4 word
		tOsdImgInfo[i].uwXStart = 150;
		tOsdImgInfo[i].uwYStart = 150 + i*(tOsdImgInfo[0].uwVSize + 20);
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);
	}

	// on off
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 4, &tOsdImgInfo[0]);
	for(i=0; i<4; ++i)
	{
		if(is_heating[i])
			p_osdImgInfo = &tOsdImgInfo[2];
		else
			p_osdImgInfo = &tOsdImgInfo[0];

		p_osdImgInfo->uwXStart = HeatingPageItemPos[4*i][0];
		p_osdImgInfo->uwYStart = HeatingPageItemPos[4*i][1];
		tOSD_Img2(p_osdImgInfo, OSD_QUEUE);
	}
	
	// min
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_3MIN_NOR, 1, &tOsdImgInfo[0]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_5MIN_NOR, 1, &tOsdImgInfo[1]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_10MIN_NOR, 1, &tOsdImgInfo[2]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_SELECT_3MIN_NOR, 1, &tOsdImgInfo[3]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_SELECT_5MIN_NOR, 1, &tOsdImgInfo[4]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_HEATING_SELECT_10MIN_NOR, 1, &tOsdImgInfo[5]);
	for(i=0; i<4; ++i)
	{
		for(j=0; j<3; ++j)
		{
			if(isHeatingCamAndOnline[i] && tUI_CuSetting.heating_time[i] == j)
				p_osdImgInfo = &tOsdImgInfo[j+3];	
			else
				p_osdImgInfo = &tOsdImgInfo[j];
			
			p_osdImgInfo->uwXStart = HeatingPageItemPos[4*i+j+1][0];
			p_osdImgInfo->uwYStart = HeatingPageItemPos[4*i+j+1][1];
			tOSD_Img2(p_osdImgInfo, OSD_QUEUE);
		}
	}

	// highlight
	for(i=0; i<4; ++i){
		if(isHeatingCamAndOnline[i])
		{
			tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx = 4*i;
			if(is_heating[i])
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_ON_HL, 1, &tOsdImgInfo[0]);
			else
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_HL, 1, &tOsdImgInfo[0]);
			tOsdImgInfo[0].uwXStart = HeatingPageItemPos[4*i][0];
			tOsdImgInfo[0].uwYStart = HeatingPageItemPos[4*i][1];
			tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
			break;
		}
	}
	OSD_UpdateImg2();
}
//------------------------------------------------------------------------------

void UI_DrawZoomSubMenuPage(void)
{
	//选择变焦摄像头页面
	OSD_IMG_INFO tOsdImgInfo[8];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1NOR_ICON, 8, &tOsdImgInfo[0]);
	uint8_t HLIndex = 0xFF;

	printf("UI_DrawZoomSubMenuPage\n");
	for(uint8_t i = 0; i < ZOOM_CAM_MAX; i++)
	{
		tOSD_Img2(&tOsdImgInfo[i*2], OSD_QUEUE);
	}

	//未接入变焦摄像头的
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1_NORTOUCH_ICON, 4, &tOsdImgInfo[0]);
	for(uint8_t i = 0; i < ZOOM_CAM_MAX; i++)
	{
//		if(!(tUI_CamStatus[i].tCamConnSts == CAM_ONLINE) || !tUI_CuSetting.ubisZoomCameraFlag[i])
		if(!tUI_CuSetting.ubisZoomCameraFlag[i])
		{
//			printf("tUI_CamStatus[%d].tCamConnSts = %d, tUI_CuSetting.ubisZoomCameraFlag[%d] = %d\n", i, tUI_CamStatus[i].tCamConnSts, i, tUI_CuSetting.ubisZoomCameraFlag[i]);
			tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);
		}
		else
		{
			if(HLIndex == 0xFF)
			{
				HLIndex = i;
			}
		}
	}
	//HL
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1NOR_ICON, 8, &tOsdImgInfo[0]);
	if(isRollBackZoomSubMenu)
	{
		tOSD_Img2(&tOsdImgInfo[2*tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx + 1], OSD_UPDATE);
		isRollBackZoomSubMenu = FALSE;
	}
	else
	{
		if(HLIndex != 0xFF)
		{
			tOSD_Img2(&tOsdImgInfo[2*HLIndex + 1], OSD_UPDATE);
			tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = HLIndex;
			tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx = HLIndex;
		}
		else
		{
			OSD_UpdateImg2();
			tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = 0xFF;
			tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx = 0xFF;
		}
	}
//	UI_ClearOsdImageNoUpdate();
//	SendIrCodeFlag = 1;
//	UI_DrawZoomButton(1);
}
//------------------------------------------------------------------------------
void UI_DrawPowerSubMenuPage(void)
{
	OSD_IMG_INFO tOsdImgInfo[5];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_POWER_BG, 5, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[3], OSD_QUEUE);
	//WORD
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_POWER_SCREENOFF_WORD:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_POWER_SCREENOFF_WORD_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_POWER_SCREENOFF_WORD_FR:OSD2IMG_POWER_SCREENOFF_WORD_CHN, 2, &tOsdImgInfo[0]);	
									
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
}

//------------------------------------------------------------------------------

void UI_DrawSubMenuPage(UI_MenuItemList_t MenuItem)
{
	static UI_DrawSubMenuFuncPtr_t DrawSubMenuFunc[MENUITEM_MAX] =
	{
		UI_DrawAdoSelSubMenuPage,
		UI_DrawCamSelSubMenuPage,
		UI_DrawPairingSubMenuPage,
		NULL,
		UI_DrawPlaybackSubMenuPage,
		UI_DrawSettingSubMenuPage,
		UI_DrawHeatingSubMenuPage,
		UI_DrawPowerSubMenuPage,
		UI_DrawZoomSubMenuPage,
	};
	OSD_IMG_INFO tOsdImgInfo;

	if(NULL == DrawSubMenuFunc[MenuItem].pvFuncPtr)
		return;
	
	osSemaphoreWait(osUI_CuUiCtr, osWaitForever);
	if(MenuItem != ADO_MODE_ITEM && MenuItem != VDO_MODE_ITEM && MenuItem != POWER_ITEM && MenuItem != ZOOM_ITEM)	
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
		tOSD_Img1(&tOsdImgInfo, OSD_QUEUE);
		if(!ubUI_TouchPanelSts)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		}
	}
	DrawSubMenuFunc[MenuItem].pvFuncPtr();
	osSemaphoreRelease(osUI_CuUiCtr);
}
//------------------------------------------------------------------------------
//uint8_t curr_indx = 0;
uint8_t uinPairingSubMenuPageFlag = FALSE;
void UI_SubKeyMenu(UI_ArrowKey_t tArrowKey)
{
	static UI_MenuFuncPtr_t SubMenuFunc[MENUITEM_MAX] =
	{
		UI_AdoSelSubMenuPage,
		UI_CamSelSubMenuPage,  //画面选择
		UI_PairingSubMenuPage,
		NULL,
		UI_PlaybackSubMenuPage,		
		UI_SettingSubMenuPage,
		UI_HeatingSubMenuPage,
		UI_PowerSubMenuPage,
		UI_ZoomSubMenuPage,
	};
	if(tArrowKey == EXIT_ARROW)
	{
		if(uinPairingSubMenuPageFlag)
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
//			if(LinkCameraNum == 1 && tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)LinkCameraIndex] == 1 &&UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)LinkCameraIndex].cCamVersion,30,zoomchar,3))
			if(LinkCameraNum == 1 && UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)LinkCameraIndex].cCamVersion,30,zoomchar,3))
			{
				tUI_CuSetting.tPowerOnMode = tUI_CuSetting.tOldPowerOnMode = (UI_PowerOnMode_t)LinkCameraIndex;
				DeskTopShowView = tUI_ViewModeSel= (UI_CamNum_t)LinkCameraIndex;
				UI_UpdateDevStatusInfo();
			}
			UI_SwitchViewType(DeskTopShowView,TRUE);
			uinPairingSubMenuPageFlag = FALSE;
		}
		
		if(tUI_MenuItem.ubItemIdx == ZOOM_ITEM)
		{
			tUI_MenuItem.ubItemIdx = ADO_MODE_ITEM;
			tUI_State = UI_DISPLAY_STATE;
			MenuOnFlag = FALSE;
			UI_ClearOsdImageNoUpdate();
			UI_DrawDesktopIcon();

			for(int i = 0; i < MENUITEM_MAX; i++)
				memset(&tUI_SubMenuItem[i].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));

			if(ubUI_RestartRec && ubUI_FinishViewSwitch)//重新启动录像
			{
				UI_VideoRecordingExec(UI_REC_START);
				ubUI_RestartRec = FALSE;
			}
			
			return;
		}
//		if(isShowFoucsRestart){
//			isShowFoucsRestart = 0;
//			curr_indx = 0;
//			UI_ClearOsdImageNoUpdate();
//			UI_DrawZoomButton(1);
//			tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx = ZOOM_SLIDER;
//			tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = ZOOM_SLIDER;
//			return;
//		}
//		if(ubisShowZoomflag)
//		{
//			ubdigitalZoomScaleFlage = 0;
//			isShowFoucsRestart = 0;
//			if(tUI_ViewModeSel != SCANVIEW_ITEM)
//				DeskTopShowView = tUI_ViewModeSel;
//			else 
//				DeskTopShowView = tUI_AutoScanNumSel;
//			UI_SwitchViewType(DeskTopShowView,TRUE);
//			ubisShowZoomflag = 0;

//			tUI_State = UI_DISPLAY_STATE;
//			MenuOnFlag = FALSE;
//			UI_ClearOsdImageNoUpdate();
//			UI_DrawDesktopIcon();
//			
//			if(ubUI_RestartRec && ubUI_FinishViewSwitch)//重新启动录像
//			{
//				UI_VideoRecordingExec(UI_REC_START);
//				ubUI_RestartRec = FALSE;
//			}
//			if(ubZoomRestartFlag)
//			{
//				ubZoomRestartFlag = FALSE;
//				tUI_CuSetting.ubSD_Switch = 1;
//			}
//			
//			UI_UpdateDevStatusInfo();
//			
//			return;
//		}
		tUI_State = UI_MAINMENU_STATE;
		UI_ClearOsdImageNoUpdate();
		UI_DrawDesktopIcon();
		UI_DrawMenuPage();
		for(int i = 0; i < MENUITEM_MAX; i++)
			memset(&tUI_SubMenuItem[i].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		
		if(ubUI_RestartRec && ubUI_FinishViewSwitch)//重新启动录像
		{
			UI_VideoRecordingExec(UI_REC_START);
			ubUI_RestartRec = FALSE;
		}
		
		return;

	}
	if(SubMenuFunc[tUI_MenuItem.ubItemIdx].pvFuncPtr)
		SubMenuFunc[tUI_MenuItem.ubItemIdx].pvFuncPtr(tArrowKey);
}
//------------------------------------------------------------------------------adoSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------adoSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------adoSel-----------------------------------------------------------------------------------------
static void UI_AdoSelSubMenuExecute(void)
{
	UI_CamNum_t tAdoCamNumSel = (UI_CamNum_t)tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemIdx;
	UI_SetAudioSrc(tAdoCamNumSel);
	UI_SwitchAudioSource(tAdoCamNumSel);
	UI_AudioOn();
	UI_UpdateDevStatusInfo();
}
//------------------------------------------------------------------------------
static void UI_AdoSelDrawSubMenuItem(void)
{
	uint16_t uwSubMenuItemOsdImg[4] = {
										OSD2IMG_SELADOCAM1_NOR_ICON, 
										OSD2IMG_SELADOCAM2_NOR_ICON,
										OSD2IMG_SELADOCAM3_NOR_ICON,
										OSD2IMG_SELADOCAM4_NOR_ICON,
										};
	uint8_t ubSubMenuItemPreIdx = tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubSubMenuItemIdx = tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemIdx;
	UI_DrawHLandNormalIcon(uwSubMenuItemOsdImg[ubSubMenuItemPreIdx], 
		(uwSubMenuItemOsdImg[ubSubMenuItemIdx] + UI_ICON_HIGHLIGHT));

}
//------------------------------------------------------------------------------
void UI_AdoSelSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	if(tCamViewSel.tCamViewType == SINGLE_VIEW)//单画面不允许切换音频
	{
		UI_MenuKey();
        UI_LeftArrowKey();
		return;
	}
	
	UI_MenuAct_t tMenuAct;
		
	if(tUI_State == UI_MAINMENU_STATE)
	{
		tUI_State = UI_SUBMENU_STATE;
		memset(&tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		UI_DrawSubMenuPage(ADO_MODE_ITEM);
		return;
	}
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[ADO_MODE_ITEM]);
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_AdoSelDrawSubMenuItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_AdoSelSubMenuExecute();
			//回到desktop
			UI_ClearOsdImageNoUpdate();
			MenuOnFlag = FALSE;
			tUI_State = UI_DISPLAY_STATE;
			UI_DrawDesktopIcon();
			break;
		}
		case EXIT_MENUFUNC:
			break;
		default:
			break;
	}
	
	
}



//------------------------------------------------------------------------------camSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------camSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------camSel-----------------------------------------------------------------------------------------


static void UI_CamSelSubMenuExecute(void)
{
	tUI_ViewModeSel = (UI_CamNum_t)tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemIdx;

	if(tUI_ViewModeSel != SCANVIEW_ITEM)
	{
		ulAutoScanCount = 0;
		DeskTopShowView = tUI_ViewModeSel;
		UI_SwitchViewType(tUI_ViewModeSel,TRUE);
		
	}
	else 
	{
		UI_CamNum_t CamNum;
		//找出自动扫描第一个画面
		for(CamNum = CAM1;CamNum < AUTOSCAN_MAX/2;CamNum ++)
		{
			if(tUI_CuSetting.AutoScanEnable[CamNum])
				break;
		}
		DeskTopShowView = CamNum;
		UI_SwitchViewType(CamNum,TRUE);
		
		tUI_AutoScanNumSel = CamNum;
		ulAutoScanCount = tUI_CuSetting.AutoScanDuty[tUI_AutoScanNumSel]* 1000;
	}
	if(tUI_CuSetting.tPowerOnMode == POWERON_LAST)
		UI_SaveLastMode();


}
//------------------------------------------------------------------------------

static void UI_CamSelDrawSubMenuItem(void)
{
	uint16_t uwSubMenuItemOsdImg[8] = {
										OSD2IMG_SELCAM1NOR_ICON, 
										OSD2IMG_SELCAM2NOR_ICON,
										OSD2IMG_SELCAM3NOR_ICON,
										OSD2IMG_SELCAM4NOR_ICON,
										OSD2IMG_SEL4TDUALNOR_ICON,
										OSD2IMG_SELCAM4T_QUAD_NOR_ICON,
										OSD2IMG_SEL4TSCANNOR_ICON
										};
	uint8_t ubSubMenuItemPreIdx = tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubSubMenuItemIdx = tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemIdx;
	UI_DrawHLandNormalIcon(uwSubMenuItemOsdImg[ubSubMenuItemPreIdx], 
		(uwSubMenuItemOsdImg[ubSubMenuItemIdx] + UI_ICON_HIGHLIGHT));

}
//------------------------------------------------------------------------------

void UI_CamSelSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;
	if(tUI_State == UI_MAINMENU_STATE)
	{
		tUI_State = UI_SUBMENU_STATE;
		memset(&tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		UI_DrawSubMenuPage(VDO_MODE_ITEM);
		return;
	}
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[VDO_MODE_ITEM]);
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_CamSelDrawSubMenuItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_CamSelSubMenuExecute();
			//回到desktop
			UI_ClearOsdImageNoUpdate();
			MenuOnFlag = FALSE;
			tUI_State = UI_DISPLAY_STATE;
			
			
			UI_DrawDesktopIcon();
			break;
		}
		case EXIT_MENUFUNC:
			break;
		default:
			break;
	}


}

//------------------------------------------------------------------------------Pairing----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------Pairing----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------Pairing----------------------------------------------------------------------------------------

void UI_PairingSubMenuExecute(void)
{
	uint16_t tSubMenuItem = tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemIdx;
	OSD_IMG_INFO tOsdImgInfo;
	if(tSubMenuItem <= PAIRINGCAM4_ITEM)
	{
		APP_EventMsg_t tUI_PairMessage = {0};
		tPairInfo.tPairSelCam = (UI_CamNum_t)tSubMenuItem;
		tPairInfo.ubDrawFlag = TRUE;
		tUI_State = UI_PAIRING_STATE;
		tUI_PairMessage.ubAPP_Event 	 = APP_PAIRING_START_EVENT;
		tUI_PairMessage.ubAPP_Message[0] = 2;		//! Message Length
		tUI_PairMessage.ubAPP_Message[1] = tPairInfo.tPairSelCam;
		tUI_PairMessage.ubAPP_Message[2] = tUI_CamStatus[tPairInfo.tPairSelCam].tCamDispLocation;
		tUI_PairMessage.ubAPP_Message[3] = FALSE;
		UI_SendMessageToAPP(&tUI_PairMessage);
		if(tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)tSubMenuItem] == 1)
		{
			tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)tSubMenuItem] = 0;
		}
	}
	else
	{
		OSD_IMG_INFO tDelOsdInfo;

		UI_CamNum_t tUI_DelCam = (UI_CamNum_t)(tSubMenuItem - 4), tSwCamNum;

		if(tUI_CuSetting.ubiHeatingCamFlag[tUI_DelCam] == 1)
		{
			tUI_CuSetting.ubiHeatingCamFlag[tUI_DelCam] = 0;
			isHeatingCamAndOnline[tUI_DelCam] = 0;
			is_heating[tUI_DelCam] = 0;
		}

		if(INVALID_ID == tUI_CamStatus[tUI_DelCam].ulCAM_ID)
		{
			return;
		}
		
//		if(tUI_CuSetting.ubisZoomCameraFlag[tUI_DelCam] == 1)
//		{
//			tUI_CuSetting.ubisZoomCameraFlag[tUI_DelCam] = 0;
//		}
		UI_UnBindCam(tUI_DelCam);
		
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRRDYMASK_ICON, 1, &tDelOsdInfo);
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAIRCAM1NOR_ICON + 2*tUI_DelCam, 1, &tOsdImgInfo);
		
		tDelOsdInfo.uwXStart = tOsdImgInfo.uwXStart + (tOsdImgInfo.uwHSize - tDelOsdInfo.uwHSize)/2;
		OSD_EraserImg2(&tDelOsdInfo);
	}

}

//-----------------------------------------------------------------------------
static void UI_PairingDrawSubMenuItem(void)
{
	uint16_t uwFirstItemOsdImg = (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_PAIRCAM1NOR_ICON:OSD2IMG_PAIRCAM1NOR_ICON_GER;
	uint8_t ubItemPreIdx = tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemIdx = tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemIdx;
	OSD_IMG_INFO tOsdImgInfo;
	for(uint8_t i = 0;i < 2;i++)
	{
		uint8_t ubItem = (i == 0)?ubItemPreIdx:ubItemIdx;
		tOSD_GetOsdImgInfor(1, OSD_IMG2, uwFirstItemOsdImg + 2*(ubItem%4) + i, 1, &tOsdImgInfo);
		tOsdImgInfo.uwYStart = PAIRING_Y + PAIRING_Y_STEP*(ubItem/4);
		tOSD_Img2(&tOsdImgInfo, (i == 0)?OSD_QUEUE:OSD_UPDATE);
	}

}

//-----------------------------------------------------------------------------
void UI_PairingSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;

	if(tUI_State == UI_MAINMENU_STATE)
	{
		tUI_State = UI_SUBMENU_STATE;
		memset(&tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		UI_SwitchViewType(QUALVIEW_ITEM,FALSE);
		uinPairingSubMenuPageFlag = TRUE;
		UI_DrawSubMenuPage(PAIRING_ITEM);
		if(UI_REC_START == tUI_RecPlayAct.tRecAct)//进入对接界面需要停止录像
		{
			UI_VideoRecordingExec(UI_REC_STOP);
			ubUI_RestartRec = TRUE;
		}
		return;
	}
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[PAIRING_ITEM]);
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_PairingDrawSubMenuItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_PairingSubMenuExecute();
			break;
		}
		case EXIT_MENUFUNC:
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------playback---------------------------------------------------------------------------
//------------------------------------------------------------------------------playback---------------------------------------------------------------------------
//------------------------------------------------------------------------------playback---------------------------------------------------------------------------
void UI_ListRecFileInfo(uint16_t uwStartFileIdx, uint16_t uwEndFileIdx, OSD_UPDATE_TYP tUpdateMode)
{
	UI_CamNum_t tRecSelCamNum;
	KNL_ROLE tRecRoleNum;
	OSD_IMG_INFO tOsdImgInfo,tEraseOsdImgInfo;
	uint16_t uwIdx;
	int iGrpRIdx = 1;
	char cRecFileName[32], cRecFileCreated[20];
	char cListGrp[9];
	tEraseOsdImgInfo.uwXStart = 0;
	tEraseOsdImgInfo.uwYStart = 62;
	tEraseOsdImgInfo.uwHSize  = 1024;
	tEraseOsdImgInfo.uwVSize  = 450;
	OSD_EraserImg2_NoUpdate(&tEraseOsdImgInfo);

	for(uwIdx = uwStartFileIdx; uwIdx < uwEndFileIdx; uwIdx++)
	{
		
		tRecRoleNum = VDO_KNLSrcNumMap2KNLRoleNum((KNL_SRC)tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.SrcNum);
		if(KNL_NONE == tRecRoleNum)
		{
			printd(DBG_ErrorLvl, "Get kernel role info Err!\n");
			tRecRoleNum = KNL_STA1;
		}
		APP_KNLRoleMap2CamNum(tRecRoleNum, tRecSelCamNum);
		//显示录像文件对应的tx序号
		tOSD_GetOsdImgInfor(1, OSD_IMG2, ((tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_FILE_CAM1_WORD:OSD2IMG_FILE_CAM1_WORD_GER) + tRecSelCamNum, 1, &tOsdImgInfo);
		tOsdImgInfo.uwYStart = 85 + (uwIdx % REC_FILE_LIST_MAXNUM) * 50;
		tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		
		//显示文件名
		UI_SetRecImgColor(UI_RECIMG_COLOR1);
		memset(cRecFileName, 0, sizeof(cRecFileName));
		snprintf(cRecFileName, sizeof(cRecFileName), "%s.%s", tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.FileName.chName, tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.FileName.chExt);
		cRecFileName[(tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.FileName.ubLen + 4)] = 0;
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, 163, (85 + ((uwIdx % REC_FILE_LIST_MAXNUM) * 50)), tUI_RecOsdImgInfo, OSD_QUEUE, cRecFileName);

		//显示组号	
		snprintf(cListGrp, sizeof(cListGrp), "Grp%d", tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.uwGroupIdx);
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, 795, (85 + ((uwIdx % REC_FILE_LIST_MAXNUM) * 50)), tUI_RecOsdImgInfo, OSD_QUEUE, cListGrp);
		
		//显示日期时间
		memset(cRecFileCreated, 0, sizeof(cRecFileCreated));
		snprintf(cRecFileCreated, sizeof(cRecFileCreated), "%02d-%02d-%02d %02d:%02d", tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.CreTime.uwYear, tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.CreTime.ubMonth, tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.CreTime.ubDay,
																					   tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.CreTime.ubHour, tUI_RecFilesInfo.tRecFilesInfo[uwIdx].HidnFileInfo.CreTime.ubMin);
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, 425, (85 + ((uwIdx % REC_FILE_LIST_MAXNUM) * 50)), tUI_RecOsdImgInfo,OSD_QUEUE, cRecFileCreated);
	}

	//页码和换页按钮
	OSD_IMG_INFO tRecOsdImgInfo[5];
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAGE_SLASH, 5, &tRecOsdImgInfo[0]);
	
	tOSD_Img2(&tRecOsdImgInfo[0], OSD_QUEUE);
	
	uint8_t ubTotalPageNum,ubCurrentPageNum;
	ubTotalPageNum = tUI_RecFilesInfo.uwTotalRecFileNum / REC_FILE_LIST_MAXNUM + ((tUI_RecFilesInfo.uwTotalRecFileNum % REC_FILE_LIST_MAXNUM)?1:0);
	ubCurrentPageNum = (tUI_RecFilesInfo.uwRecFileSelIdx + 1) / REC_FILE_LIST_MAXNUM + (((tUI_RecFilesInfo.uwRecFileSelIdx + 1) % REC_FILE_LIST_MAXNUM)?1:0);

	tEraseOsdImgInfo.uwXStart = tRecOsdImgInfo[0].uwXStart - 42;//擦除数值再重新显示，避免有残留
	tEraseOsdImgInfo.uwYStart = tRecOsdImgInfo[0].uwYStart + 16;
	tEraseOsdImgInfo.uwHSize  = 14 * 3;
	tEraseOsdImgInfo.uwVSize  = 28;
	OSD_EraserImg2_NoUpdate(&tEraseOsdImgInfo);
	
	if(ubCurrentPageNum < 10)
		UI_ShowPlaybackPageNum(ubCurrentPageNum,tRecOsdImgInfo[0].uwXStart - 14,tRecOsdImgInfo[0].uwYStart + 16,OSD_QUEUE);
	else if(ubCurrentPageNum < 100)
		UI_ShowPlaybackPageNum(ubCurrentPageNum,tRecOsdImgInfo[0].uwXStart - 28,tRecOsdImgInfo[0].uwYStart + 16,OSD_QUEUE);
	else if(ubCurrentPageNum < 1000)
		UI_ShowPlaybackPageNum(ubCurrentPageNum,tRecOsdImgInfo[0].uwXStart - 42,tRecOsdImgInfo[0].uwYStart + 16,OSD_QUEUE);

	UI_ShowPlaybackPageNum(ubTotalPageNum,tRecOsdImgInfo[0].uwXStart + 24,tRecOsdImgInfo[0].uwYStart + 16,tUpdateMode);
	
	if(ubUI_TouchPanelSts)
	{
		tOSD_Img2(&tRecOsdImgInfo[1], OSD_QUEUE);
		tOSD_Img2(&tRecOsdImgInfo[3], tUpdateMode);
	}
}
//------------------------------------------------------------------------------
void UI_SearchRecordFile(uint8_t ubRecFolderIndex)
{
	uint16_t uwLdState = UI_SEARCH_RECFILES;

	ubRecFolderIndex = ubRecFolderIndex;
	UI_SetLdDispStatus(UI_OSDLDDISP_ON);
	osMessagePut(osUI_OsdLdDispQueue, &uwLdState, 0);
	osDelay(200);
	tUI_RecFilesInfo.uwTotalRecFileNum = uwKNL_GetSortingFiles(ubRecFolderIndex, SORT_BY_TIME_DESCENDING, tUI_RecFilesInfo.tRecFilesInfo);
	UI_SetLdDispStatus(UI_OSDLDDISP_OFF);
	osDelay(500);
}
//------------------------------------------------------------------------------

void UI_DrawRecordFileMenu(void)
{
	OSD_IMG_INFO tOsdImgInfo[5];
	uint16_t uwFileStartIdx = 0, uwFileEndIdx = 0;
	uint8_t ubIdx;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FILE_HEADLINE, 5, &tOsdImgInfo[0]);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_FILE_HEADLINE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_FILE_HEADLINE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_FILE_HEADLINE_FR:OSD2IMG_FILE_HEADLINE_CHN, 1, &tOsdImgInfo[0]);

	UI_SearchRecordFile(tUI_RecFoldersInfo.uwRecFolderSelIdx);
	tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);
	if(tUI_RecFilesInfo.uwTotalRecFileNum)
	{
		uwFileStartIdx = (tUI_RecFilesInfo.uwRecFileSelIdx / REC_FILE_LIST_MAXNUM) * REC_FILE_LIST_MAXNUM;
		uwFileEndIdx   = tUI_RecFilesInfo.uwTotalRecFileNum - uwFileStartIdx;
		uwFileEndIdx   = (uwFileEndIdx < REC_FILE_LIST_MAXNUM)?tUI_RecFilesInfo.uwTotalRecFileNum:(uwFileStartIdx + REC_FILE_LIST_MAXNUM);
	
		UI_ListRecFileInfo(uwFileStartIdx, uwFileEndIdx, OSD_QUEUE);
		//BOX
		for(ubIdx = 0; ubIdx < 4; ubIdx++)
		{
			tOsdImgInfo[1 + ubIdx].uwYStart += ((tUI_RecFilesInfo.uwRecFileSelIdx % REC_FILE_LIST_MAXNUM) * 50);
			tOSD_Img2(&tOsdImgInfo[1 + ubIdx], OSD_UPDATE);
		}
	}
	tUI_State = UI_SUBSUBMENU_STATE;
}
//------------------------------------------------------------------------------

void UI_PlaybackSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	OSD_IMG_INFO tFolderSelOsdImgInfo[2], tOsdImgInfo;
	uint8_t ubUI_RecFolderIdx = 0, ubUI_PrevRecFolderIdx = 0, ubRecSelIdx;
	uint8_t ubRefreshFldIconFlag = FALSE;
	if(UI_MAINMENU_STATE == tUI_State)
	{
		tUI_RecFoldersInfo.uwRecFolderSelIdx = 0;
		tUI_State = UI_SUBMENU_STATE;
		UI_DrawSubMenuPage(PLAYBACK_ITEM);
		if(UI_REC_START == tUI_RecPlayAct.tRecAct)//进入播放器就停止录像
		{
			UI_VideoRecordingExec(UI_REC_STOP);
			ubUI_RestartRec = TRUE;
		}
		return;
	}

	if((!tUI_RecFoldersInfo.uwTotalRecFolderNum) && (EXIT_ARROW != tArrowKey))
		return;

	if(tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_REC_FOLDERCLOSE_ICON, 2, &tFolderSelOsdImgInfo[0]) != OSD_OK)
	{
		printd(DBG_ErrorLvl, "Load OSD Image FAIL, pls check (%d) !\n", __LINE__);
		return;
	}
	ubUI_RecFolderIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx % REC_FOLDER_LIST_MAXNUM);
	ubUI_PrevRecFolderIdx = ubUI_RecFolderIdx;
	
	switch(tArrowKey)
	{
		case LEFT_ARROW:
			if(!(ubUI_RecFolderIdx % REC_FOLDER_LIST_MAXNUM))//当前在第一个文件夹
			{
				if(tUI_RecFoldersInfo.uwRecFolderSelIdx > 0)//前面还有文件夹，切换到前一页
				{
					ubRefreshFldIconFlag = TRUE;
				}
				else
					return;//已经是第一个文件夹
					
			}
		
			tUI_RecFoldersInfo.uwRecFolderSelIdx --;
			ubUI_RecFolderIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx % REC_FOLDER_LIST_MAXNUM);
			break;
		case RIGHT_ARROW:
			if(tUI_RecFoldersInfo.uwTotalRecFolderNum / REC_FOLDER_LIST_MAXNUM == tUI_RecFoldersInfo.uwRecFolderSelIdx /REC_FOLDER_LIST_MAXNUM)//当前在最后一页
			{
				if(tUI_RecFoldersInfo.uwTotalRecFolderNum == tUI_RecFoldersInfo.uwRecFolderSelIdx + 1)//当前在最后一个文件夹
					return;
			}
			else
			{
				if(tUI_RecFoldersInfo.uwRecFolderSelIdx % REC_FOLDER_LIST_MAXNUM == REC_FOLDER_LIST_MAXNUM - 1)//当前在最后一个文件夹
					ubRefreshFldIconFlag = TRUE;
			}
			tUI_RecFoldersInfo.uwRecFolderSelIdx ++;
			ubUI_RecFolderIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx % REC_FOLDER_LIST_MAXNUM);
			break;
		case ENTER_ARROW:
			tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
			tOSD_Img1(&tOsdImgInfo, OSD_QUEUE);
			if(!ubUI_TouchPanelSts)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tOsdImgInfo);
				printf("x = %d y = %d \n", tOsdImgInfo.uwXStart, tOsdImgInfo.uwYStart);
				tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			}

			tUI_RecFilesInfo.uwRecFileSelIdx = 0;
			UI_DrawRecordFileMenu();
			SendIrCodeFlag = 1;
			return;
		case EXIT_ARROW:
			
			return;
		default:
			return;
	}
	if(TRUE == ubRefreshFldIconFlag)//翻页
	{
		uint8_t ubFldStartIdx = 0, ubFldEndIdx = 0;

		tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
		tOSD_Img1(&tOsdImgInfo, OSD_QUEUE);
		if(!ubUI_TouchPanelSts)
		{
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tOsdImgInfo);
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
		}
		ubFldStartIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;//该界面第一个文件夹的序号
		if(tArrowKey == LEFT_ARROW)
		{
			ubFldEndIdx = ubFldStartIdx + REC_FOLDER_LIST_MAXNUM;
		}
		else
		{
			ubFldEndIdx = tUI_RecFoldersInfo.uwTotalRecFolderNum - tUI_RecFoldersInfo.uwRecFolderSelIdx;
			ubFldEndIdx = (ubFldEndIdx < REC_FOLDER_LIST_MAXNUM)?tUI_RecFoldersInfo.uwTotalRecFolderNum:(ubFldStartIdx + REC_FOLDER_LIST_MAXNUM);

		}
		
		UI_ListDCIMFolderInfo(ubFldStartIdx, ubFldEndIdx, OSD_UPDATE);
	}
	if(FALSE == ubRefreshFldIconFlag)//刷新文件夹选中状态
	{
		ubRecSelIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;//该界面第一个文件夹的序号
		UI_SetRecImgColor(UI_RECIMG_COLOR1);
		tFolderSelOsdImgInfo[0].uwXStart += ((ubUI_PrevRecFolderIdx % 5) * 180);
		tFolderSelOsdImgInfo[0].uwYStart += ((ubUI_PrevRecFolderIdx / 5) * 220);
		tOSD_Img2(&tFolderSelOsdImgInfo[0], OSD_QUEUE);
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, (72 + ((ubUI_PrevRecFolderIdx % 5) * 180)), (225 + ((ubUI_PrevRecFolderIdx / 5) * 220)),
						tUI_RecOsdImgInfo, OSD_QUEUE, tUI_RecFoldersInfo.tRecFolderInfo[ubUI_PrevRecFolderIdx + ubRecSelIdx].FldName.chName);
		UI_SetRecImgColor(UI_RECIMG_COLOR1);
		tFolderSelOsdImgInfo[1].uwXStart += ((ubUI_RecFolderIdx % 5) * 180);
		tFolderSelOsdImgInfo[1].uwYStart += ((ubUI_RecFolderIdx / 5) * 220);
		tOSD_Img2(&tFolderSelOsdImgInfo[1], OSD_QUEUE);
		OSD_ImagePrintf(OSD_IMG_ROTATION_0, (72 + ((ubUI_RecFolderIdx % 5) * 180)), (225 + ((ubUI_RecFolderIdx / 5) * 220)),
						tUI_RecOsdImgInfo, OSD_UPDATE, tUI_RecFoldersInfo.tRecFolderInfo[ubUI_RecFolderIdx + ubRecSelIdx].FldName.chName);
	}

}
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------------------

static void UI_CameraSubMenuExecute(void)
{
	OSD_IMG_INFO tCamsOsdImgInfo[8],tOsdImgInfo;

	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMSCAM1NOR_ICON, 8, &tCamsOsdImgInfo[0]);
	//background
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
	
	UI_PanelOff();
	OSD_EraserImg1(&tOsdImgInfo);
	//CAM1~CAM4
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tCamsOsdImgInfo[2*i], OSD_QUEUE);
	tOSD_Img2(&tCamsOsdImgInfo[1], OSD_QUEUE);

	ubisCameraConfigFlag = TRUE;
	tUI_State = UI_SUBSUBMENU_STATE;
	UI_EnterKey();	
}
//-----------------------------------------------------------------------------
static void UI_RecordSubMenuExecute(void)
{
	#define RECORE_WORD_NUMBER 1
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
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_RECORD_TITLE_FR:OSD2IMG_RECORD_TITLE_CHN, RECORE_WORD_NUMBER, &tOsdImgInfo[0]);
	//文字
	for(uint8_t i = 0;i < RECORE_WORD_NUMBER;i++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);
	
	//record time
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RECTIME_1MIN_NOR, 12, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 3;i++)
	{
		if(tUI_CuSetting.RecInfo.tREC_Time == i)
			tOSD_Img2(&tOsdImgInfo[2*i + 6], OSD_QUEUE);//高亮图标上的文字
		else
			tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
	}
	//highLight record time 1 min
	if(tUI_CuSetting.RecInfo.tREC_Time == RECTIME_1MIN)
		tOSD_Img2(&tOsdImgInfo[7], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);
	
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FORMATSD_NOR, 2, &tOsdImgInfo[0]);
	
	//format sd card
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	//date&time
	//tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	
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
	tUI_State = UI_SUBSUBMENU_STATE;

}
//-----------------------------------------------------------------------------
static void UI_SystemSubMenuExecute(void)
{
	OSD_IMG_INFO tMenuOsdImgInfo,tOsdImgInfo[10];
	
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	
	//文字
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SYSTEM_TITLE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SYSTEM_TITLE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SYSTEM_TITLE_FR:OSD2IMG_SYSTEM_TITLE_CHN, 1, &tOsdImgInfo[0]);

	for(uint8_t i = 0;i < 1;i ++)
		tOSD_Img2(&tOsdImgInfo[i], OSD_QUEUE);

	//���黬��
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DIMMER_SLIDER, 6, &tOsdImgInfo[0]);
	//绿色高亮
	tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);		
	//滑杆
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	//滑块
	tOsdImgInfo[2].uwXStart = UI_Map(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,0,99,
		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
	tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	//数值
	if(tUI_CuSetting.BriLvL.ulBL_UpdateLvL < 10)
	{
		UI_ShowButtonValueHighLight(0,tOsdImgInfo[2].uwXStart + 4,tOsdImgInfo[2].uwYStart + 9,OSD_QUEUE);
		UI_ShowButtonValueHighLight(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,tOsdImgInfo[2].uwXStart + 24,tOsdImgInfo[2].uwYStart + 9,OSD_QUEUE);
	}
	else
		UI_ShowButtonValueHighLight(tUI_CuSetting.BriLvL.ulBL_UpdateLvL,tOsdImgInfo[2].uwXStart + 4,tOsdImgInfo[2].uwYStart + 9,OSD_QUEUE);
			
	//auto dimmer
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 6, &tOsdImgInfo[0]);

	if(tUI_CuSetting.ubAutoDimmer)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	tOSD_Img2(&tOsdImgInfo[4], OSD_QUEUE);

	
	//menu lock
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_MENULOCK_OFF_NOR, 4, &tOsdImgInfo[0]);

	if(tUI_CuSetting.ubMenuLock)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	//upgrade
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_UPGRADE_NOR, 1, &tOsdImgInfo[0]);
    tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	//language
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

	//buzzer
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_BUZZER_OFF_NOR, 4, &tOsdImgInfo[0]);

	if(tUI_CuSetting.ubBuzzer)
		tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
	else
		tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);

	// date & time
    tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_DATE_TIME_NOR, 1, &tOsdImgInfo[0]);
	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	OSD_ImagePrintf(OSD_IMG_ROTATION_0, 450, 570, tUI_CharOsdImgInfo, OSD_QUEUE, SOFEWARE_VERSION);

	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	OSD_UpdateQueueBuf();
	memset(&tSystemSubSubMenuItem.tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	tUI_State = UI_SUBSUBMENU_STATE;

}
//-----------------------------------------------------------------------------
static void UI_DualSubMenuExecute(void)
{
	OSD_IMG_INFO tCamOsdImgInfo[16],tOsdImgInfo;
	APP_EventMsg_t tUI_SwitchBuMsg = {0};
	uint16_t uwLcd_HSize = uwLCD_GetLcdHoSize();

	UI_PanelOff();
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
	OSD_EraserImg1(&tOsdImgInfo);
	//切换到dual画面
	UI_SwitchViewType(DUALVIEW_ITEM,TRUE);
	
	//line
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_LINE_DUAL, 1, &tCamOsdImgInfo[0]);
	tOSD_Img2(&tCamOsdImgInfo[0], OSD_QUEUE);
	
	
	for(uint8_t i = 0;i < 4;i++)
	{
		//left
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
		if(tUI_CamStatus[i].tCamDispLocation_Dual == DISP_LEFT)
		{
			tUI_SwitchBuMsg.ubAPP_Message[2] = i;
			tOSD_Img2(&tCamOsdImgInfo[8 + 2*i], OSD_QUEUE);
		}
		else
		{
			tOSD_Img2(&tCamOsdImgInfo[2*i], OSD_QUEUE);
		}
		//right
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
		if(tUI_CamStatus[i].tCamDispLocation_Dual == DISP_RIGHT)
		{
			tUI_SwitchBuMsg.ubAPP_Message[3] = i;
			tCamOsdImgInfo[8 + 2*i].uwXStart += uwLcd_HSize/2;
			tOSD_Img2(&tCamOsdImgInfo[8 + 2*i], OSD_QUEUE);
		}
		else
		{
			tCamOsdImgInfo[2*i].uwXStart += uwLcd_HSize/2;
			tOSD_Img2(&tCamOsdImgInfo[2*i], OSD_QUEUE);
		}
	}
	//highLight
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
	if(tUI_CamStatus[0].tCamDispLocation_Dual == DISP_LEFT)
		tOSD_Img2(&tCamOsdImgInfo[8 + 1], OSD_UPDATE);
	else
		tOSD_Img2(&tCamOsdImgInfo[1], OSD_UPDATE);
	
	//return
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	}
	
	memset(&tDualSubSubMenuItem.tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	tUI_State = UI_SUBSUBMENU_STATE;

}
//-----------------------------------------------------------------------------
static void UI_QuadSubMenuExecute(void)
{
	OSD_IMG_INFO tCamOsdImgInfo[16],tOsdImgInfo;
	uint16_t uwLcd_HSize = uwLCD_GetLcdHoSize();
	uint16_t uwLcd_VSize = uwLCD_GetLcdVoSize();

	UI_PanelOff();
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
	OSD_EraserImg1(&tOsdImgInfo);
	
	//切换到quad画面
	UI_SwitchViewType(QUALVIEW_ITEM,TRUE);
	//line
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DESKTOP_LINE_QUAD_H, 2, &tCamOsdImgInfo[0]);
	tOSD_Img2(&tCamOsdImgInfo[0], OSD_QUEUE);
	tOSD_Img2(&tCamOsdImgInfo[1], OSD_QUEUE);
	
	for(uint8_t i = 0;i < 4;i++)
	{
		//upper left
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
		if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_UPPER_LEFT)
		{
			tCamOsdImgInfo[8 + 2*i].uwYStart -= uwLcd_VSize/4;
			tOSD_Img2(&tCamOsdImgInfo[8 + 2*i], OSD_QUEUE);
		}
		else
		{
			tCamOsdImgInfo[2*i].uwYStart -= uwLcd_VSize/4;
			tOSD_Img2(&tCamOsdImgInfo[2*i], OSD_QUEUE);
		}
		//upper right
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
		if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_UPPER_RIGHT)
		{
			tCamOsdImgInfo[8 + 2*i].uwYStart -= uwLcd_VSize/4;
			tCamOsdImgInfo[8 + 2*i].uwXStart += uwLcd_HSize/2;
			tOSD_Img2(&tCamOsdImgInfo[8 + 2*i], OSD_QUEUE);
		}
		else
		{
			tCamOsdImgInfo[2*i].uwYStart -= uwLcd_VSize/4;
			tCamOsdImgInfo[2*i].uwXStart += uwLcd_HSize/2;
			tOSD_Img2(&tCamOsdImgInfo[2*i], OSD_QUEUE);
		}
		//lower left
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
		if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_LOWER_LEFT)
		{
			tCamOsdImgInfo[8 + 2*i].uwYStart += uwLcd_VSize/4;
			tOSD_Img2(&tCamOsdImgInfo[8 + 2*i], OSD_QUEUE);
		}
		else
		{
			tCamOsdImgInfo[2*i].uwYStart += uwLcd_VSize/4;
			tOSD_Img2(&tCamOsdImgInfo[2*i], OSD_QUEUE);
		}
		//lower right
		tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
		if(tUI_CamStatus[i].tCamDispLocation_Quad == DISP_LOWER_RIGHT)
		{
			tCamOsdImgInfo[8 + 2*i].uwXStart += uwLcd_HSize/2;
			tCamOsdImgInfo[8 + 2*i].uwYStart += uwLcd_VSize/4;
			tOSD_Img2(&tCamOsdImgInfo[8 + 2*i], OSD_QUEUE);
		}
		else
		{
			tCamOsdImgInfo[2*i].uwXStart += uwLcd_HSize/2;
			tCamOsdImgInfo[2*i].uwYStart += uwLcd_VSize/4;
			tOSD_Img2(&tCamOsdImgInfo[2*i], OSD_QUEUE);
		}
	}
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_DUAL_LEFT_CAM1NOR:OSD2IMG_DUAL_LEFT_CAM1NOR_GER, 16, &tCamOsdImgInfo[0]);
	//highLight
	if(tUI_CamStatus[0].tCamDispLocation_Quad == DISP_UPPER_LEFT)
	{
		tCamOsdImgInfo[8 + 1].uwYStart -= uwLcd_VSize/4;
		tOSD_Img2(&tCamOsdImgInfo[8 + 1], OSD_UPDATE);
	}
	else
	{
		tCamOsdImgInfo[1].uwYStart -= uwLcd_VSize/4;
		tOSD_Img2(&tCamOsdImgInfo[1], OSD_UPDATE);
	}

	//return
	if(ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 1, &tOsdImgInfo);
		tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	}

	memset(&tQuadSubSubMenuItem.tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	tUI_State = UI_SUBSUBMENU_STATE;
}
//-----------------------------------------------------------------------------
static void UI_TriggerSubMenuExecute(void)
{
	
	OSD_IMG_INFO tOsdImgInfo_Button[22],tOsdImgInfo_Word[9],tOsdImgInfo_2,tMenuOsdImgInfo;
	
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
	tOSD_Img2(&tOsdImgInfo_Button[1], OSD_QUEUE);
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
	UI_ShowButtonValueHighLight(tUI_CuSetting.TriggerDelay[0],
			tOsdImgInfo_Button[0].uwXStart + ((tUI_CuSetting.TriggerDelay[0] < 10)?40:30),tOsdImgInfo_Button[0].uwYStart + 15,OSD_UPDATE);
	
	tUI_State = UI_SUBSUBMENU_STATE;
}
//-----------------------------------------------------------------------------
void UI_AutoDrawOnOffButton(bool isButtonHL, uint8_t OnOffButtonHLIndex)
{
	OSD_IMG_INFO tOsdImgInfo[4];
//	uint16_t uwAutoScanXStart = 0, uwAutoScanYStart = 0;
	uint16_t uwIndex = 0;
	//OnOff
	for(uint8_t i = 0;i < 6;i++)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AUTOSCAN_OFF_CAM1_NOR, 4, &tOsdImgInfo[0]);
		uwIndex = tUI_CuSetting.AutoScanEnable[i] == 0?0:2;//off
		//高亮
		if(i == OnOffButtonHLIndex && isButtonHL)
		{
			uwIndex += 1;
		}
		if(i % 2)
		{
			tOsdImgInfo[uwIndex].uwXStart += AUTO_SCAN_OFF_DIST_X;
		}
		if(i >= 2)
		{
			tOsdImgInfo[uwIndex].uwYStart += AUTO_SCAN_OFF_DIST_Y*(i/2);
		}
		tOSD_Img2(&tOsdImgInfo[uwIndex], ((i == 5 && isButtonHL)?OSD_UPDATE:OSD_QUEUE));
	}
}


//-----------------------------------------------------------------------------
static void UI_AutoScanSubMenuExecute(void)
{
	
	OSD_IMG_INFO tOsdImgInfo[24],tMenuOsdImgInfo;
	
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
	
	
	UI_AutoDrawOnOffButton(TRUE, 0);
		
//		if(!tUI_CuSetting.AutoScanEnable[i])//off
//		{
//			tOSD_Img2(&tOsdImgInfo[4*i + ((i == 0)?UI_ICON_HIGHLIGHT:UI_ICON_NORMAL)], ((i == 5)?OSD_UPDATE:OSD_QUEUE));
//		}
//		else
//		{
//			tOSD_Img2(&tOsdImgInfo[2 + 4*i + ((i == 0)?UI_ICON_HIGHLIGHT:UI_ICON_NORMAL)], ((i == 5)?OSD_UPDATE:OSD_QUEUE));
//		}
//	}
	
	tUI_State = UI_SUBSUBMENU_STATE;
}	

//-----------------------------------------------------------------------------
static void UI_PowerOnSubMenuExecute(void)
{
	OSD_IMG_INFO tOsdImgInfo[2*POWERON_MAX],tMenuOsdImgInfo;
	
	tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tMenuOsdImgInfo);
	tOSD_Img1(&tMenuOsdImgInfo, OSD_QUEUE);
	if(!ubUI_TouchPanelSts)
	{
		tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_BLACK, 1, &tMenuOsdImgInfo);
		tOSD_Img2(&tMenuOsdImgInfo, OSD_QUEUE);
	}
	//title
	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_POWERON_TITLE:
									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_POWERON_TITLE_GER:
									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_POWERON_TITLE_FR:OSD2IMG_POWERON_TITLE_CHN, 1, &tOsdImgInfo[0]);

	tOSD_Img2(&tOsdImgInfo[0], OSD_QUEUE);
	
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_POWERON_CAM1_NOR, 2*(POWERON_MAX - 1), &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < (POWERON_MAX - 1);i++)
		tOSD_Img2(&tOsdImgInfo[2*i], (tUI_CuSetting.tPowerOnMode == POWERON_LAST)?OSD_UPDATE:OSD_QUEUE);
	
	if(tUI_CuSetting.tPowerOnMode != POWERON_LAST)
		tOSD_Img2(&tOsdImgInfo[2*tUI_CuSetting.tPowerOnMode + UI_ICON_HIGHLIGHT], OSD_UPDATE);
	tUI_State = UI_SUBSUBMENU_STATE;

	
	memset(&tPowerOnSubSubMenuItem.tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	ubUI_PowerOnSubSubStsUpdateFlag = FALSE;
}

//-----------------------------------------------------------------------------
static void UI_ParkingLineSubMenuExecute(void)
{
	
	OSD_IMG_INFO tOsdImgInfo[24],tMenuOsdImgInfo;
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
	//CALIBRATE
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_CAM1_CAL_NOR, 8, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
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
	//high light
	tOSD_Img2(&tOsdImgInfo[2*tUI_CuSetting.tParkingLineEnable[0] + 1], OSD_UPDATE);
	tUI_State = UI_SUBSUBMENU_STATE;
	
}

//-----------------------------------------------------------------------------
static void UI_GuideLineSubMenuExecute(void)
{
	OSD_IMG_INFO tOsdImgInfo[24],tMenuOsdImgInfo;
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
			
	//CALIBRATE
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PARKINGLINE_CAM1_CAL_NOR, 8, &tOsdImgInfo[0]);
	for(uint8_t i = 0;i < 4;i++)
		tOSD_Img2(&tOsdImgInfo[2*i], OSD_QUEUE);
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
	//high light
	tOSD_Img2(&tOsdImgInfo[2*tUI_CuSetting.GuideLineEnable[0] + 1], OSD_UPDATE);
	tUI_State = UI_SUBSUBMENU_STATE;
	
}

//-----------------------------------------------------------------------------
static void UI_SettingSubMenuExecute(void)
{
	uint8_t ubSubMenuItemIdx = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;
	memset(&tSettingSubSubMenuItem.tSettingS[ubSubMenuItemIdx].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
	
	ubUI_SubSubStsUpdateFlag = FALSE;
	if(ubSubMenuItemIdx == CAMERASET_ITEM)
	{
		UI_CameraSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == RECORDSET_ITEM)
	{
		UI_RecordSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == SYSTEMSET_ITEM)
	{		
		UI_SystemSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == DUALSET_ITEM)
	{
		UI_DualSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == QUADSET_ITEM)
	{
		UI_QuadSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == TRIGGERSET_ITEM)
	{
		UI_TriggerSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == AUTOSCANSET_ITEM)
	{
		UI_AutoScanSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == POWERONSET_ITEM)
	{
		UI_PowerOnSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == PARKINGLINE_ITEM)
	{
		UI_ParkingLineSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == GUIDELINE_ITEM)
	{
		UI_GuideLineSubMenuExecute();
	}
	else if(ubSubMenuItemIdx == AI_ITEM)
	{
		UI_AISubMenuExecute();
	}
	else
		return;
}
//-----------------------------------------------------------------------------
static void UI_SettingingDrawSubMenuItem(void)
{
	uint16_t uwSubMenuItemOsdImg[SETTINGITEM_MAX] = {
													OSD2IMG_CAMERA_SETTING_NOR_ICON, 
													OSD2IMG_RECORD_SETTING_NOR_ICON,
													OSD2IMG_SYSTEM_SETTING_NOR_ICON,
													OSD2IMG_DUAL_SETTING_NOR_ICON,
													OSD2IMG_QUAD_SETTING_NOR_ICON,
													OSD2IMG_TRIGGER_NOR_ICON,
													OSD2IMG_AUTOSCAN_NOR_ICON,
													OSD2IMG_POWERON_NOR_ICON,
													OSD2IMG_PARKINGLINE_NOR_ICON,
													OSD2IMG_GUIDELINE_NOR_ICON,
													OSD2IMG_AI_NOR_ICON,
													};
	uint8_t ubSubMenuItemPreIdx = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubSubMenuItemIdx = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;
	UI_DrawHLandNormalIcon(uwSubMenuItemOsdImg[ubSubMenuItemPreIdx], (uwSubMenuItemOsdImg[ubSubMenuItemIdx] + UI_ICON_HIGHLIGHT));

}
//-----------------------------------------------------------------------------

void UI_SettingSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;
	
	if(tUI_State == UI_MAINMENU_STATE)
	{
		tUI_State = UI_SUBMENU_STATE;
		memset(&tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		UI_DrawSubMenuPage(SETTING_ITEM);
		return;
	}
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[SETTING_ITEM]);
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_SettingingDrawSubMenuItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_SettingSubMenuExecute();
			break;
		}
		case EXIT_MENUFUNC:
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------zoom-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------zoom-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------zoom-----------------------------------------------------------------------------------

static void UI_ZoomSubMenuExecute() //待修改
{
	UI_EnterZoomSettingCamera();
}

static void UI_ZoomDrawSubMenuItem()
{
	OSD_IMG_INFO tOsdImgInfo[8];
	uint8_t ubItemPreIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubItemCurIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;

	
//	printf("UI_ZoomDrawSubMenuItem ubItemPreIdx = %d, ubItemCurIdx = %d\n", ubItemPreIdx, ubItemCurIdx);
	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1NOR_ICON, 8, &tOsdImgInfo);
	
	if(ubItemPreIdx != ubItemCurIdx)
	{
		//----------------------------pre----------------------------------------------
		tOSD_Img2(&tOsdImgInfo[ubItemPreIdx * 2], OSD_QUEUE);
		//----------------------------cur----------------------------------------------
		tOSD_Img2(&tOsdImgInfo[ubItemCurIdx * 2 + 1], OSD_UPDATE);
	}
	
}


void UI_ZoomSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	//循环找到在线的摄像头
	uint8_t isFindZoomFlag = FALSE, LinkZoomCameraNum = 0, LinkZoomCameraIndex = 0;;

	//统计变焦摄像头数量
	for(uint8_t i = 0; i < 4; i++)
	{
		printf("UI_ZoomSubMenuPage tUI_CuSetting.ubisZoomCameraFlag[%d] = %d, tUI_CamStatus[i].cCamVersion = %s\n", i, tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)i], tUI_CamStatus[(UI_CamNum_t)i].cCamVersion);
//		if(tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)i] == 1 && UI_CheckTxVersion(tUI_CamStatus[(UI_CamNum_t)i].cCamVersion,30,zoomchar,3))
		if(tUI_CuSetting.ubisZoomCameraFlag[(UI_CamNum_t)i] == 1)
		{
			LinkZoomCameraNum++;
			LinkZoomCameraIndex = i;
		}
	}

	//单个变焦的话直接进入变焦设置界面
	if(LinkZoomCameraNum == 1)
	{
		isSingleZoomCamera = TRUE;
		MenuOnFlag = TRUE;
		tUI_MenuItem.ubItemIdx = ZOOM_ITEM;
		tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = LinkZoomCameraIndex;
		UI_ZoomSubMenuExecute();
		return;
	}

	if(tUI_State != UI_SUBMENU_STATE)
	{
		MenuOnFlag = TRUE;

		if(tUI_State == UI_SET_ZOOM_CAMERA_STATE)
		{
			isRollBackZoomSubMenu = TRUE;
		}
		else
		{
			memset(&tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		}

		UI_DrawSubMenuPage(ZOOM_ITEM);
		tUI_MenuItem.ubItemIdx = ZOOM_ITEM;
		tUI_State = UI_SUBMENU_STATE;
		return;
	}

	tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	switch(tArrowKey)
	{
		case LEFT_ARROW:
		{
			for(int i = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx - 1; i >= 0; i--)
			{
				if(tUI_CuSetting.ubisZoomCameraFlag[i] == 0)
				{
					continue;
				}
				tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = i;
				isFindZoomFlag = TRUE;
				break;
			}

			if(!isFindZoomFlag)
			{
				for(int i = ZOOM_CAM4; i > tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx; i--)
				{
					if(tUI_CuSetting.ubisZoomCameraFlag[i] == 0)
					{
						continue;
					}
					tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			UI_ZoomDrawSubMenuItem();
			break;
		}
		case RIGHT_ARROW:
		{
			for(int i = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx + 1; i < ZOOM_CAM_MAX; i++)
			{
				if(tUI_CuSetting.ubisZoomCameraFlag[i] == 0)
				{
					continue;
				}
				tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = i;
				isFindZoomFlag = TRUE;
				break;
			}

			if(!isFindZoomFlag)
			{
				for(int i = ZOOM_CAM1; i < tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx; i++)
				{
					if(tUI_CuSetting.ubisZoomCameraFlag[i] == 0)
					{
						continue;
					}
					tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			UI_ZoomDrawSubMenuItem();
			break;
		}
		case ENTER_ARROW:
			UI_ZoomSubMenuExecute();
			break;
		default:
			break;
	}
	
	
	
//	UI_MenuAct_t tMenuAct;
//	uint8_t ubPreVolumeLvl = 10;
//	OSD_IMG_INFO tOsdImgInfo;

//	if(tUI_State != UI_SUBMENU_STATE)
//	{
//		MenuOnFlag = TRUE;
//		isShowFoucsRestart = 0;
//		ubdigitalZoomScaleFlage = 0;
//		ubisShowZoomflag = 1;
//		
//		//不是UI_ADJUST_ZOOM_AREA_STATE、UI_ADJUST_ZOOM_BSD_AREA_STATE状态才会清空变焦页面前后点击的状态并Relink
//		if(tUI_State != UI_ADJUST_ZOOM_AREA_STATE && tUI_State != UI_ADJUST_ZOOM_BSD_AREA_STATE)
//		{
//			for (uint8_t i = CAM1; i <= CAM4; i++)
//			{
//				if(tUI_CuSetting.ubisZoomCameraFlag[i]  == 1)
//				{
//					DeskTopShowView = i;
//					UI_SwitchViewType(i,FALSE);
//					break;
//				}
//			}
//			memset(&tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
//		}
//		UI_DrawSubMenuPage(ZOOM_ITEM);
//		tUI_MenuItem.ubItemIdx = ZOOM_ITEM;
//		tUI_State = UI_SUBMENU_STATE;
//		return;
//	}
//	if(isShowFoucsRestart){
//		handle_zoom_key_restart_ops(tArrowKey);
//		return;
//	}
//	if(tArrowKey == ENTER_ARROW || !ubdigitalZoomScaleFlage)
//	{
//		tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[ZOOM_ITEM]);
//		switch(tMenuAct)
//		{
//			case DRAW_HIGHLIGHT_MENUICON:
//			{
//				UI_ZoomDrawSubMenuItem();
//				break;
//			}
//			case DRAW_MENUPAGE:
//			{
//				UI_ZoomSubMenuExecute();
//				break;
//			}
//			case EXIT_MENUFUNC:
//				break;
//			default:
//				break;
//		}
//	}
//	else
//	{
//		switch(tArrowKey)
//		{
//			case LEFT_ARROW:
//			{
//				ubPreVolumeLvl = tUI_CamStatus[DeskTopShowView].digitalZoomScale;
//				tUI_CamStatus[DeskTopShowView].digitalZoomScale--;
//				if(tUI_CamStatus[DeskTopShowView].digitalZoomScale < 10)
//					tUI_CamStatus[DeskTopShowView].digitalZoomScale = 10;
//				updateDigitalZoomSlide(DeskTopShowView, ubPreVolumeLvl);
//				break;
//			}
//			case RIGHT_ARROW:
//			{
//				ubPreVolumeLvl = tUI_CamStatus[DeskTopShowView].digitalZoomScale;
//				tUI_CamStatus[DeskTopShowView].digitalZoomScale++;
//				if(tUI_CamStatus[DeskTopShowView].digitalZoomScale > 50)
//					tUI_CamStatus[DeskTopShowView].digitalZoomScale = 50;
//				updateDigitalZoomSlide(DeskTopShowView, ubPreVolumeLvl);
//				break;
//			}
//		}
//	}
}

//------------------------------------------------------------------------------heating-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------heating-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------heating-----------------------------------------------------------------------------------

static void UI_HeatingDrawSubMenuItem(void)
{
	uint8_t cam_i, col_i;
	OSD_IMG_INFO tOsdImgInfo;
	
	uint16_t timeImg[] = {
		OSD2IMG_RECTIME_3MIN_NOR,	// 0
		OSD2IMG_RECTIME_5MIN_NOR,
		OSD2IMG_HEATING_10MIN_NOR,
		OSD2IMG_RECTIME_SELECT_3MIN_NOR,	// 3
		OSD2IMG_RECTIME_SELECT_5MIN_NOR,
		OSD2IMG_HEATING_SELECT_10MIN_NOR,
		OSD2IMG_RECTIME_3MIN_HL,	// 6
		OSD2IMG_RECTIME_5MIN_HL,
		OSD2IMG_HEATING_10MIN_HL,
		OSD2IMG_RECTIME_SELECT_3MIN_HL,	// 9
		OSD2IMG_RECTIME_SELECT_5MIN_HL,
		OSD2IMG_HEATING_SELECT_10MIN_HL,
	};
	
	uint8_t preIdx = tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t curIdx = tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx;
	
	// preIdx
	cam_i = preIdx / 4;
	col_i = preIdx - (cam_i * 4);
	if(isHeatingCamAndOnline[cam_i]) {
		if(col_i == 0){	// on off
			if(is_heating[cam_i])
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_ON_NOR, 1, &tOsdImgInfo);
			else
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_NOR, 1, &tOsdImgInfo);
		}else{	// time
			--col_i;
			if(tUI_CuSetting.heating_time[cam_i] == col_i)
				tOSD_GetOsdImgInfor(1, OSD_IMG2, timeImg[3+col_i], 1, &tOsdImgInfo);
			else
				tOSD_GetOsdImgInfor(1, OSD_IMG2, timeImg[col_i], 1, &tOsdImgInfo);
		}
		tOsdImgInfo.uwXStart = HeatingPageItemPos[preIdx][0];
		tOsdImgInfo.uwYStart = HeatingPageItemPos[preIdx][1];
		tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	}
	
	// curIdx
	cam_i = curIdx / 4;
	col_i = curIdx - (cam_i * 4);
	if(isHeatingCamAndOnline[cam_i]) {
		if(col_i == 0){	// on off
			if(is_heating[cam_i])
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_ON_HL, 1, &tOsdImgInfo);
			else
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_HL, 1, &tOsdImgInfo);
		}else{	// time
			--col_i;
			if(tUI_CuSetting.heating_time[cam_i] == col_i)
				tOSD_GetOsdImgInfor(1, OSD_IMG2, timeImg[9+col_i], 1, &tOsdImgInfo);
			else
				tOSD_GetOsdImgInfor(1, OSD_IMG2, timeImg[6+col_i], 1, &tOsdImgInfo);
		}
		tOsdImgInfo.uwXStart = HeatingPageItemPos[curIdx][0];
		tOsdImgInfo.uwYStart = HeatingPageItemPos[curIdx][1];
		tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
	}
}

static bool set_Heating_status(UI_CamNum_t cam, bool enable)
{
	static uint8_t time_map[3] = {3, 5, 10};

	if(!isHeatingCamAndOnline[cam]) {
		return false;
	}
	UI_CUReqCmd_t tCamSetCmd;
	tCamSetCmd.tDS_CamNum				= cam;
	// tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = enable;
	tCamSetCmd.ubCmd[UI_SETTING_DATA+1] = enable ? time_map[tUI_CuSetting.heating_time[cam]] : 0;
	return UI_SendLaserorLedToCAM(osThreadGetId(), &tCamSetCmd, TWC_HEAT_CTRL) == rUI_SUCCESS;
}

static void UI_HeatingSubMenuExecute(void)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t curIdx = tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx;
	uint8_t cam_i = curIdx / 4;
	uint8_t col_i = curIdx - (cam_i * 4);
	
	if(col_i == 0){	// on off
		if(is_heating[cam_i] && set_Heating_status(cam_i, 0)){
			// cancel heating
			is_heating[cam_i] = 0;
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_OFF_HL, 1, &tOsdImgInfo);
		}else if(isHeatingCamAndOnline[cam_i] && set_Heating_status(cam_i, 1)){
			// heating
			is_heating[cam_i] = 1;
			heating_level[cam_i] = 2;
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SYSTEM_AUTO_ON_HL, 1, &tOsdImgInfo);
		}else{
			return;
		}
	}else{	// time
		--col_i;
		if(tUI_CuSetting.heating_time[cam_i] != col_i)
		{
			uint16_t timeImg[] = {
				OSD2IMG_RECTIME_3MIN_NOR,	// 0
				OSD2IMG_RECTIME_5MIN_NOR,
				OSD2IMG_HEATING_10MIN_NOR,
				OSD2IMG_RECTIME_SELECT_3MIN_HL,	// 3
				OSD2IMG_RECTIME_SELECT_5MIN_HL,
				OSD2IMG_HEATING_SELECT_10MIN_HL,
			};
			
			tOSD_GetOsdImgInfor(1, OSD_IMG2, timeImg[tUI_CuSetting.heating_time[cam_i]], 1, &tOsdImgInfo);
			tOsdImgInfo.uwXStart = HeatingPageItemPos[4*cam_i+tUI_CuSetting.heating_time[cam_i]+1][0];
			tOsdImgInfo.uwYStart = HeatingPageItemPos[4*cam_i+tUI_CuSetting.heating_time[cam_i]+1][1];
			tOSD_Img2(&tOsdImgInfo, OSD_QUEUE);
			
			tOSD_GetOsdImgInfor(1, OSD_IMG2, timeImg[3+col_i], 1, &tOsdImgInfo);
			tUI_CuSetting.heating_time[cam_i] = col_i;
			
			UI_UpdateDevStatusInfo();
		}
	}
	
	tOsdImgInfo.uwXStart = HeatingPageItemPos[curIdx][0];
	tOsdImgInfo.uwYStart = HeatingPageItemPos[curIdx][1];
	tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
}
	
void UI_HeatingSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;
	
	if(tUI_State == UI_MAINMENU_STATE)
	{
		tUI_State = UI_SUBMENU_STATE;
		memset(&tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		UI_DrawSubMenuPage(HEATING_ITEM);
		return;
	}
	tMenuAct = UI_HeatingKeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[HEATING_ITEM]);
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_HeatingDrawSubMenuItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_HeatingSubMenuExecute();
			break;
		}
		case EXIT_MENUFUNC:
			break;
		default:
			break;
	}
}
	
//------------------------------------------------------------------------------power-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------power-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------power-----------------------------------------------------------------------------------
static void UI_PowerSubMenuExecute(void)
{
	if(tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemIdx == POWER_SCREENOFF)
	{
		UI_EnterStandby();
	}
	else
	{
		UI_EnterStandby();
		ubUI_CuPowerOffFlag = TRUE;
	}
}
//------------------------------------------------------------------------------
static void UI_PowerDrawSubMenuItem(void)
{
	uint16_t uwSubMenuItemOsdImg[2] = {
										OSD2IMG_POWER_SCREENOFF_NOR, 
										OSD2IMG_POWER_SHUTDOWN_NOR,
										};
	uint8_t ubSubMenuItemPreIdx = tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemPreIdx;
	uint8_t ubSubMenuItemIdx = tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemIdx;
	UI_DrawHLandNormalIcon(uwSubMenuItemOsdImg[ubSubMenuItemPreIdx], 
		(uwSubMenuItemOsdImg[ubSubMenuItemIdx] + UI_ICON_HIGHLIGHT));

}

//------------------------------------------------------------------------------
void UI_PowerSubMenuPage(UI_ArrowKey_t tArrowKey)
{
	UI_MenuAct_t tMenuAct;

	if(tUI_State == UI_MAINMENU_STATE)
	{
		tUI_State = UI_SUBMENU_STATE;
		memset(&tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo, 0, sizeof(UI_MenuItem_t));
		UI_DrawSubMenuPage(POWER_ITEM);
		return;
	}
	tMenuAct = UI_KeyEventMap2SubMenuInfo(&tArrowKey, &tUI_SubMenuItem[POWER_ITEM]);
	switch(tMenuAct)
	{
		case DRAW_HIGHLIGHT_MENUICON:
		{
			UI_PowerDrawSubMenuItem();
			break;
		}
		case DRAW_MENUPAGE:
		{
			UI_PowerSubMenuExecute();
			break;
		}
		case EXIT_MENUFUNC:
			break;
		default:
			break;
	}
	
	
}


/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/
/********************************************************************TOUCH*********************************************************************************************/
void UI_SubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	static UI_MenuTouchFuncPtr_t SubMenuFunc[MENUITEM_MAX] = 
	{
		UI_AdoSelSubTouchMenu,
		UI_CamSelSubTouchMenu,
		UI_PairingSubTouchMenu,
		NULL,
		UI_PlaybackSubTouchMenu,
		UI_SettingSubTouchMenu,
		UI_HeatingSubTouchMenu,
		UI_PowerSubTouchMenu,
		UI_ZoomSubTouchMenu,
	};
	if(SubMenuFunc[tUI_MenuItem.ubItemIdx].pvFuncPtr)
		SubMenuFunc[tUI_MenuItem.ubItemIdx].pvFuncPtr(Touch_Info);
}
//------------------------------------------------------------------------------adoSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------adoSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------adoSel-----------------------------------------------------------------------------------------
void UI_AdoSelSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i = 0;
	tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:	
				
			for(i = 0; i < tUI_SubMenuItem[ADO_MODE_ITEM].ubItemCount;i++)//判断具体点击在哪个图标上
			{
				//读取当前操作界面的全部图标信息
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELADOCAM1_NOR_ICON + 2*i, 1, &tOsdImgInfo);	

				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			if(i < tUI_SubMenuItem[ADO_MODE_ITEM].ubItemCount)
			{
				if(tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemPreIdx != 
					tUI_SubMenuItem[ADO_MODE_ITEM].tSubMenuInfo.ubItemIdx)
				{
					UI_AdoSelDrawSubMenuItem(); 			
				}
				UI_EnterKey();	
			}
			else
			{
				//触摸调出音量条
				UI_MenuKey();
				UI_MenuKey();
                UI_LeftArrowKey();
			}	

		break;
		case TOUCH_TURNDOWN:
			break;

		default:
			return;

	}
}


//------------------------------------------------------------------------------camSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------camSel-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------camSel-----------------------------------------------------------------------------------------
void UI_CamSelSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i = 0;
	tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:	
				
			for(i = 0; i < tUI_SubMenuItem[VDO_MODE_ITEM].ubItemCount;i++)//判断具体点击在哪个图标上
			{
				//读取当前操作界面的全部图标信息
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1NOR_ICON + 2*i, 1, &tOsdImgInfo);	

				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_SubMenuItem[VDO_MODE_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			if(i < tUI_SubMenuItem[VDO_MODE_ITEM].ubItemCount)
			{
				UI_CamSelDrawSubMenuItem(); 
				osDelay(100);
				UI_EnterKey();	
			}
			else
			{
				UI_MenuKey();
			}

		break;
		case TOUCH_TURNDOWN:
			break;

		default:
			return;

	}

}
//------------------------------------------------------------------------------pairing-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------pairing-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------pairing-----------------------------------------------------------------------------------------
void UI_PairingSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i = 0;
	tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:	
				
			for(i = 0; i < tUI_SubMenuItem[PAIRING_ITEM].ubItemCount;i++)//判断具体点击在哪个图标上
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, ((tUI_CuSetting.tLanguage != LANGUAGE_GERMAN)?OSD2IMG_PAIRCAM1NOR_ICON:OSD2IMG_PAIRCAM1NOR_ICON_GER) + 2*(i%4), 1, &tOsdImgInfo);
				tOsdImgInfo.uwYStart = PAIRING_Y + PAIRING_Y_STEP*(i/4);
				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			
			if(i < tUI_SubMenuItem[PAIRING_ITEM].ubItemCount)	
			{
				if(tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemPreIdx != 
					tUI_SubMenuItem[PAIRING_ITEM].tSubMenuInfo.ubItemIdx)
				{
					UI_PairingDrawSubMenuItem();
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
			
		default:

			return;
	}
}

//------------------------------------------------------------------------------playback-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------playback-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------playback-----------------------------------------------------------------------------------------
/* ICON
* 0 1 2 3 4
* 5 6 7 8 9
*/
#define PLAYBACK_SUB_MAX_ICON 10
void UI_PlaybackSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	uint16_t uwHSize[PLAYBACK_SUB_MAX_ICON],uwVSize[PLAYBACK_SUB_MAX_ICON],uwXStart[PLAYBACK_SUB_MAX_ICON],uwYStart[PLAYBACK_SUB_MAX_ICON];
	uint8_t ubUI_RecFolderIdx = 0, ubUI_PrevRecFolderIdx = 0, ubRecSelIdx;
	OSD_IMG_INFO tFolderSelOsdImgInfo[2],tOsdImgInfo;
	uint8_t ubFldStartIdx = 0, ubFldEndIdx = 0;
	uint16_t i = 0;	
	ubRecSelIdx = tUI_RecFoldersInfo.uwRecFolderSelIdx;
	ubUI_RecFolderIdx = (ubRecSelIdx % REC_FOLDER_LIST_MAXNUM);//当前选中的图标在界面中排第几位
	ubUI_PrevRecFolderIdx = ubUI_RecFolderIdx;	
	
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:	
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_REC_FOLDERCLOSE_ICON, 2, &tFolderSelOsdImgInfo[0]);
			
			for(i = 0;i < PLAYBACK_SUB_MAX_ICON;i ++)
			{
				//计算该界面中每个图标的位置
				uwHSize[i] = tFolderSelOsdImgInfo[0].uwHSize;
				uwVSize[i] = tFolderSelOsdImgInfo[0].uwVSize;
				uwXStart[i] = tFolderSelOsdImgInfo[0].uwXStart + (i % 5) * 180;
				uwYStart[i] = tFolderSelOsdImgInfo[0].uwYStart + (i / 5) * 220;

				//判断点击在哪个图标上
				if(Touch_Info->startX > uwXStart[i] && Touch_Info->startX < uwXStart[i] + uwHSize[i]
					&& Touch_Info->startY > uwYStart[i] && Touch_Info->startY < uwYStart[i] + uwVSize[i])
				{
					break;
				}
			}
			if(i < PLAYBACK_SUB_MAX_ICON)//点击在文件夹上
			{
				if((ubRecSelIdx + i - ubUI_RecFolderIdx) >= tUI_RecFoldersInfo.uwTotalRecFolderNum)//当前点击的位置没有文件夹
					return;
				ubUI_RecFolderIdx = i;
				tUI_RecFoldersInfo.uwRecFolderSelIdx += i - ubUI_PrevRecFolderIdx;

				ubRecSelIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;//该界面第一个文件夹的序号
				UI_SetRecImgColor(UI_RECIMG_COLOR1);
				tFolderSelOsdImgInfo[0].uwXStart += ((ubUI_PrevRecFolderIdx % 5) * 180);
				tFolderSelOsdImgInfo[0].uwYStart += ((ubUI_PrevRecFolderIdx / 5) * 220);
				tOSD_Img2(&tFolderSelOsdImgInfo[0], OSD_QUEUE);
				OSD_ImagePrintf(OSD_IMG_ROTATION_0, (72 + ((ubUI_PrevRecFolderIdx % 5) * 180)), (225 + ((ubUI_PrevRecFolderIdx / 5) * 220)),
								tUI_RecOsdImgInfo, OSD_QUEUE, tUI_RecFoldersInfo.tRecFolderInfo[ubUI_PrevRecFolderIdx + ubRecSelIdx].FldName.chName);
				UI_SetRecImgColor(UI_RECIMG_COLOR1);
				tFolderSelOsdImgInfo[1].uwXStart += ((ubUI_RecFolderIdx % 5) * 180);
				tFolderSelOsdImgInfo[1].uwYStart += ((ubUI_RecFolderIdx / 5) * 220);
				tOSD_Img2(&tFolderSelOsdImgInfo[1], OSD_QUEUE);
				OSD_ImagePrintf(OSD_IMG_ROTATION_0, (72 + ((ubUI_RecFolderIdx % 5) * 180)), (225 + ((ubUI_RecFolderIdx / 5) * 220)),
								tUI_RecOsdImgInfo, OSD_UPDATE, tUI_RecFoldersInfo.tRecFolderInfo[ubUI_RecFolderIdx + ubRecSelIdx].FldName.chName);
				TIMER_Delay_ms(30);
				UI_EnterKey();
			}
			else
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_HL, 1, &tOsdImgInfo);	//return
				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
					TIMER_Delay_ms(30);
					UI_MenuKey();
					return;
				}
				
				for(i = 0;i < 2;i ++)//换页按钮
				{
					tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PAGE_BACKWARD_HL + 2*i, 1, &tOsdImgInfo);
					if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
					{						
						ubFldStartIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;//该界面第一个文件夹的序号 
						if(i == 0)//backward
						{
							//如果该文件夹序号大于0就说明上一页还有内容
							if(ubFldStartIdx > 0)
							{
								tUI_RecFoldersInfo.uwRecFolderSelIdx = ubFldStartIdx - 1;
								ubFldStartIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;
								ubFldEndIdx = ubFldStartIdx + REC_FOLDER_LIST_MAXNUM;
							}
							else
								return;

						}
						else//forward
						{
							//满足该条件表示下一页还有文件夹
							if(ubFldStartIdx + REC_FOLDER_LIST_MAXNUM < tUI_RecFoldersInfo.uwTotalRecFolderNum)
							{
								tUI_RecFoldersInfo.uwRecFolderSelIdx = ubFldStartIdx + REC_FOLDER_LIST_MAXNUM;
								ubFldStartIdx = (tUI_RecFoldersInfo.uwRecFolderSelIdx / REC_FOLDER_LIST_MAXNUM) * REC_FOLDER_LIST_MAXNUM;
								ubFldEndIdx = tUI_RecFoldersInfo.uwTotalRecFolderNum - tUI_RecFoldersInfo.uwRecFolderSelIdx;
								ubFldEndIdx = (ubFldEndIdx < REC_FOLDER_LIST_MAXNUM)?tUI_RecFoldersInfo.uwTotalRecFolderNum:(ubFldStartIdx + REC_FOLDER_LIST_MAXNUM);
							}
							else 
								return;

						}
						tOSD_Img2(&tOsdImgInfo, OSD_UPDATE);
						tOSD_GetOsdImgInfor(1, OSD_IMG1, OSD1IMG_SUBMENU2, 1, &tOsdImgInfo);
						tOSD_Img1(&tOsdImgInfo, OSD_QUEUE);
						
						UI_ListDCIMFolderInfo(ubFldStartIdx, ubFldEndIdx, OSD_UPDATE);

						break;
					}	
				}
				
				
				
			}

			break;
		case TOUCH_TURNLEFT:
			break;
		case TOUCH_TURNRIGHT:
			
			break;
		case TOUCH_TURNDOWN:
			break;
		
		default:

			return;
	}

}
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------setting-----------------------------------------------------------------------------------------


void UI_SettingSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i = 0;
	tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			
			for(i = 0; i < tUI_SubMenuItem[SETTING_ITEM].ubItemCount;i++)//判断具体点击在哪个图标上
			{
				//读取当前操作界面的全部图标信息
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_CAMERA_SETTING_NOR_ICON + 2*i, 1, &tOsdImgInfo);	
				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			
			if(i < tUI_SubMenuItem[SETTING_ITEM].ubItemCount)
			{
				if(tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemPreIdx != 
					tUI_SubMenuItem[SETTING_ITEM].tSubMenuInfo.ubItemIdx)
				{
					UI_SettingingDrawSubMenuItem();				
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

		default:

			return;
	}
}
//------------------------------------------------------------------------------heating-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------heating-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------heating-----------------------------------------------------------------------------------------
void UI_HeatingSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t i;
	uint8_t old = tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx;
	tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			// in which line
			for(i=0; i<tUI_SubMenuItem[HEATING_ITEM].ubItemCount; ++i)
			{
				if(Touch_Info->startX > HeatingPageItemPos[i][0] 
					&& Touch_Info->startX < HeatingPageItemPos[i][0] + HeatingPageItemPos[i][2]
					&& Touch_Info->startY > HeatingPageItemPos[i][1] 
					&& Touch_Info->startY < HeatingPageItemPos[i][1] + HeatingPageItemPos[i][3])
				{
					tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			
			if(i < tUI_SubMenuItem[HEATING_ITEM].ubItemCount)
			{
				if(tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx != 
					tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx)
				{
					i = i/4;
					if(isHeatingCamAndOnline[i])
					{
						UI_HeatingDrawSubMenuItem();
					}else{
						tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemIdx = tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx;
						tUI_SubMenuItem[HEATING_ITEM].tSubMenuInfo.ubItemPreIdx = old;
						return;
					}
				}
				UI_EnterKey();	
			}else{
				// return
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
		case TOUCH_PRESSDOWN:
			break;
		default:
			return;
	}
}
//------------------------------------------------------------------------------power-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------power-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------power-----------------------------------------------------------------------------------------
void UI_PowerSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint16_t i = 0;
	tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemIdx;
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:	
				
			for(i = 0; i < tUI_SubMenuItem[POWER_ITEM].ubItemCount;i++)//判断具体点击在哪个图标上
			{
				//读取当前操作界面的全部图标信息
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_POWER_SCREENOFF_NOR + 2*i, 1, &tOsdImgInfo);	

				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
					&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
					&& Touch_Info->startY > tOsdImgInfo.uwYStart 
					&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemIdx = i;
					break;
				}
			}
			if(i < tUI_SubMenuItem[POWER_ITEM].ubItemCount)
			{
				if(tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemPreIdx != 
					tUI_SubMenuItem[POWER_ITEM].tSubMenuInfo.ubItemIdx)
				{
					UI_PowerDrawSubMenuItem(); 			
					osDelay(100);
				}
				UI_EnterKey();	
			}
			else
			{
				UI_MenuKey();
			}	
		break;
		case TOUCH_TURNDOWN:
			break;

		default:
			return;

	}
}
//-------------------------------------------------------------------------------------Zoom

//void draw_restart_cam_box(void)
//{
//	OSD_IMG_INFO tOsdImgInfo[5];
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, (tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_FOCUS_RESTART_EN:
//								(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_FOCUS_RESTART_GER:
//								(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_FOCUS_RESTART_FR:OSD2IMG_FOCUS_RESTART_CHN, 1, &tOsdImgInfo[0]);
//	tOSD_Img2(&tOsdImgInfo[0], OSD_UPDATE);

//	tOSD_GetOsdImgInfor(1, OSD_IMG2,(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_FORMAT_BG:
//									(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_FORMAT_BG_GER:
//									(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_FORMAT_BG_FR:OSD2IMG_SDCARD_FORMAT_BG_CHN, 5, &tOsdImgInfo[0]);
//	tOSD_Img2(&tOsdImgInfo[2], OSD_UPDATE);
//	tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
//}

//uint8_t handle_zoom_touch_ops(TOUCH_EVENT_t *Touch_Info)
//{
//	OSD_IMG_INFO tOsdImgInfo[4],tOsdImgInfo1[2],tOsdImgInfo2[4],tOsdImgInfo3[2];
//	uint8_t touch_idx = 100;
//	TWC_STATUS send_ret = TWC_FAIL;

//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_AUTO_NOR, 4, &tOsdImgInfo[0]);
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 2, &tOsdImgInfo1[0]);
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_OFF_NOR, 4, &tOsdImgInfo2[0]);
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_ZOOM_TRACE_CALIBRATE_NOR, 2, &tOsdImgInfo3[0]);

//	for (uint8_t i = 0; i < 2; i++)
//	{
//		if (Touch_Info->startX > tOsdImgInfo[2*i].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo[2*i].uwXStart + tOsdImgInfo[2*i].uwHSize && \
//			Touch_Info->startY > tOsdImgInfo[2*i].uwYStart && \
//			Touch_Info->startY < tOsdImgInfo[2*i].uwYStart + tOsdImgInfo[2*i].uwVSize)
//		{
//			touch_idx = i;
//			break;
//		}
//		if (Touch_Info->startX > tOsdImgInfo1[0].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize && \
//			Touch_Info->startY > tOsdImgInfo1[0].uwYStart && \
//			Touch_Info->startY < tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize)
//		{
//			touch_idx = 2;
//			break;
//		}
//		if (Touch_Info->startX > tOsdImgInfo2[0].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo2[0].uwXStart + tOsdImgInfo2[0].uwHSize && \
//			Touch_Info->startY > tOsdImgInfo2[0].uwYStart && \
//			Touch_Info->startY < tOsdImgInfo2[0].uwYStart + tOsdImgInfo2[0].uwVSize)
//		{
//			touch_idx = 3;
//			tUI_CamStatus[DeskTopShowView].ubTrace = 1 - tUI_CamStatus[DeskTopShowView].ubTrace;
//			tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubTrace*2 + 1], OSD_UPDATE);
//			break;
//		}
//		if (Touch_Info->startX > tOsdImgInfo3[0].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo3[0].uwXStart + tOsdImgInfo3[0].uwHSize && \
//			Touch_Info->startY > tOsdImgInfo3[0].uwYStart && \
//			Touch_Info->startY < tOsdImgInfo3[0].uwYStart + tOsdImgInfo3[0].uwVSize)
//		{
//			touch_idx = 4;
//			tOSD_Img2(&tOsdImgInfo3[1], OSD_UPDATE);
//			break;
//		}
//		if (Touch_Info->startX > tOsdImgInfo2[0].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo2[0].uwXStart + tOsdImgInfo2[0].uwHSize && \
//			Touch_Info->startY > BSD_OFF_NOR_DIST && \
//			Touch_Info->startY < BSD_OFF_NOR_DIST + tOsdImgInfo2[0].uwVSize)
//		{
//			touch_idx = 5;
//			tUI_CamStatus[DeskTopShowView].ubBSD = 1 - tUI_CamStatus[DeskTopShowView].ubBSD;
//			tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubBSD*2 + 1].uwYStart = BSD_OFF_NOR_DIST;
//			tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubBSD*2 + 1], OSD_UPDATE);
//			break;
//		}
//		if (Touch_Info->startX > tOsdImgInfo3[0].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo3[0].uwXStart + tOsdImgInfo3[0].uwHSize && \
//			Touch_Info->startY > BSD_OFF_NOR_DIST && \
//			Touch_Info->startY < BSD_OFF_NOR_DIST + tOsdImgInfo3[0].uwVSize)
//		{
//			touch_idx = 6;
//			tOsdImgInfo3[1].uwYStart = BSD_OFF_NOR_DIST;
//			tOSD_Img2(&tOsdImgInfo3[1], OSD_UPDATE);
//			break;
//		}
//	}


//	if (touch_idx != 100)
//	{
//		switch (touch_idx)
//		{
//			case 0: // auto focus
//			{
//				//printf("auto button\n");
//				send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_auto[0], 7, 8);
//				osDelay(100);
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_AUTO;
//				return 1;
//			}
//			case 1: // restart device
//			{
//				//printf("restart button\n");
//				draw_restart_cam_box();
//				isShowFoucsRestart = 1;
//				//osDelay(100);
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_RESTART;
//				return 1;
//			}
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
//				UI_SubKeyMenu(EXIT_ARROW);
//				return 1;
//			}
//			case 3:
//			{
//				tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_Trace[0 + 7*tUI_CamStatus[DeskTopShowView].ubTrace], 7, 10);
//				//tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubTrace*2], OSD_UPDATE);
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_TRCAE;
//				return 1;
//			}
//			case 4:
//			{
//				UI_EnterAdjustZoomArea();
//				return 1;
//			}
//			case 5:
//			{
//				tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_BSD[0 + 7*tUI_CamStatus[DeskTopShowView].ubBSD], 7, 10);
//				tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubBSD*2].uwYStart = BSD_OFF_NOR_DIST;
//				//tOSD_Img2(&tOsdImgInfo2[tUI_CamStatus[DeskTopShowView].ubBSD*2], OSD_UPDATE);
//				tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_BSD;
//				return 1;
//			}
//			case 6:
//			{
//				UI_EnterAdjustZoomBSDArea();
//				return 1;
//			}
//			default:
//			{
//				return 0;
//			}
//		}
//	}
//	return 0;
//}

//uint8_t handle_zoom_touch_restart_ops(TOUCH_EVENT_t *Touch_Info)
//{
//	OSD_IMG_INFO tOsdImgInfo[5];
//	uint8_t touch_idx = 100;
//	TWC_STATUS send_ret = TWC_FAIL;

//	tOSD_GetOsdImgInfor(1, OSD_IMG2,(tUI_CuSetting.tLanguage == LANGUAGE_ENGLISH)?OSD2IMG_SDCARD_FORMAT_BG:
//								(tUI_CuSetting.tLanguage == LANGUAGE_GERMAN)?OSD2IMG_SDCARD_FORMAT_BG_GER:
//								(tUI_CuSetting.tLanguage == LANGUAGE_FRENCH)?OSD2IMG_SDCARD_FORMAT_BG_FR:OSD2IMG_SDCARD_FORMAT_BG_CHN, 5, &tOsdImgInfo[0]);
//	for (uint8_t i = 0; i < 2; i++)
//	{
//		if (Touch_Info->startX > tOsdImgInfo[2*i+1].uwXStart && \
//			Touch_Info->startX < tOsdImgInfo[2*i+1].uwXStart + tOsdImgInfo[2*i+1].uwHSize && \
//			Touch_Info->startY > tOsdImgInfo[2*i+1].uwYStart && \
//			Touch_Info->startY < tOsdImgInfo[2*i+1].uwYStart + tOsdImgInfo[2*i+1].uwVSize)
//		{
//			touch_idx = i;
//			break;
//		}
//	}

//	if (touch_idx != 100)
//	{
//		switch (touch_idx)
//		{
//			case 0: // auto focus
//			{
//				//printf("restart button\n");
//				if(tUI_CuSetting.ubSD_Switch)
//				{
//					ubZoomRestartFlag = TRUE;
//					tUI_CuSetting.ubSD_Switch = 0;
//					UI_SwitchViewType(DeskTopShowView,FALSE);
//				}
//				send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_restart[0], 7, 8);
//				osDelay(100);
//				tUI_CamStatus[DeskTopShowView].digitalZoomScale = 10;
//				isShowFoucsRestart = 0;
//				UI_ClearOsdImageNoUpdate();
//				UI_DrawZoomButton(1);
//				return 1;
//			}
//			case 1: // restart device
//			{
//				tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
//				tOSD_Img2(&tOsdImgInfo[4], OSD_UPDATE);
//				isShowFoucsRestart = 0;
//				UI_ClearOsdImageNoUpdate();
//				UI_DrawZoomButton(1);
//				return 1;
//			}
//			default:
//			{
//				return 0;
//			}
//		}
//	}
//	return 0;
//}

//void sendDigitalZoomScale(uint8_t val)
//{
//	uint16_t sum;

//	sum = 0x92 + val;
//	pelco_digital_zoom[3] = val;
//	pelco_digital_zoom[6] = (uint8_t)(sum&0x00FF);
//	tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_digital_zoom[0], 7, 1);
//}

//void updateDigitalZoomSlide(UI_CamNum_t chn, uint8_t ubPreVolumeLvl)
//{
//	
//	OSD_IMG_INFO tOsdImgInfo[5];
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_SLIDE, 3, &tOsdImgInfo[0]);


//	tOsdImgInfo[2].uwXStart = UI_Map(ubPreVolumeLvl, 10, 50,
//		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[2].uwHSize);
//	tOSD_Img2(&tOsdImgInfo[2], OSD_QUEUE);
//	
//	tOsdImgInfo[1].uwXStart = UI_Map(tUI_CamStatus[chn].digitalZoomScale, 10, 50,
//		tOsdImgInfo[0].uwXStart,tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize - tOsdImgInfo[1].uwHSize);
//	tOSD_Img2(&tOsdImgInfo[1], OSD_QUEUE);

//	OSD_ImagePrintf(OSD_IMG_ROTATION_0, 500, 480, tUI_TransparentCharOsdImgInfo, OSD_UPDATE, "%d.%d", tUI_CamStatus[chn].digitalZoomScale/10, tUI_CamStatus[chn].digitalZoomScale%10);

//	if (ulDigitalZoomCount == 0)
//	{
//		ulDigitalZoomCount = 300;
//		sendDigitalZoomScale(tUI_CamStatus[chn].digitalZoomScale);
//	}
//}

void UI_ZoomSubTouchMenu(TOUCH_EVENT_t *Touch_Info)
{
	OSD_IMG_INFO tOsdImgInfo;
	uint8_t i;
	uint8_t ubMenuItemCount = tUI_SubMenuItem[ZOOM_ITEM].ubItemCount;
	uint8_t isTouchButton = FALSE;
	tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx;
	
	switch(Touch_Info->Gesture)
	{
		case TOUCH_PRESS:
			for(i = 0; i < ubMenuItemCount; i++)
			{
				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_SELCAM1NOR_ICON + 2 * i, 1, &tOsdImgInfo);

				if(Touch_Info->startX > tOsdImgInfo.uwXStart 
						&& Touch_Info->startX < tOsdImgInfo.uwXStart + tOsdImgInfo.uwHSize
						&& Touch_Info->startY > tOsdImgInfo.uwYStart 
						&& Touch_Info->startY < tOsdImgInfo.uwYStart + tOsdImgInfo.uwVSize)
				{
					tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = i;
					if(tUI_CuSetting.ubisZoomCameraFlag[tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx] == 0)
					{
						tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemIdx = tUI_SubMenuItem[ZOOM_ITEM].tSubMenuInfo.ubItemPreIdx;
						return;
					}
					UI_ZoomDrawSubMenuItem();
					UI_EnterKey();
					isTouchButton = TRUE;
				}
			}
			//没有触摸到按键就返回
			if(!isTouchButton)
			{
				UI_MenuKey();
			}
			
			break;
		default:
	
			return;
	}
//	TWC_STATUS send_ret = TWC_FAIL;
//	OSD_IMG_INFO tOsdImgInfo[12],tOsdImgInfo1[2],tOsdImgInfo2;
//	static uint8_t touch_repaet_cnt = 0; // 
//	static uint8_t isZoomExecute = 0, isZoomSlideChange = 0;
//	uint8_t ubPreVolumeLvl = 10;

//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_FOCUS_IN_NOR, 12, &tOsdImgInfo[0]);
//	tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_RETURN_NOBACKGROUND_NOR, 2, &tOsdImgInfo1[0]);
//	//ubdigitalZoomScaleFlage = 0;

//	tZoomSettingCameraItem.tSubMenuInfo.ubItemPreIdx = tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx;
//	
//	switch(Touch_Info->Gesture)
//	{
//        case TOUCH_PRESSDOWN: //
//			if (isShowFoucsRestart==1)
//			{
//				break;
//			}


//			//限制长按时，滑到放大缩小倍数按钮外，没有stop指令发送
//			if(isZoomExecute == 1 && \
//				!((Touch_Info->endX > tOsdImgInfo[0].uwXStart && \
//				Touch_Info->endX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
//				Touch_Info->endY > tOsdImgInfo[0].uwYStart && \
//				Touch_Info->endY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize) || 
//				(Touch_Info->endX > tOsdImgInfo[2].uwXStart && \
//				Touch_Info->endX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
//				Touch_Info->endY > tOsdImgInfo[2].uwYStart && \
//				Touch_Info->endY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize)))
//			{
//				send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//				printf("TOUCH_PRESSDOWN stop\n");
//				isZoomExecute = 0;
//				break;
//			}
//			
//			if (Touch_Info->endX > tOsdImgInfo[0].uwXStart && \
//				Touch_Info->endX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
//				Touch_Info->endY > tOsdImgInfo[0].uwYStart && \
//				Touch_Info->endY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize)
//			{
//				if (isZoomExecute == 0)
//				{
//					touch_repaet_cnt = 25;
//				}
//				else
//				{
//					touch_repaet_cnt++;
//				}

//            	if (touch_repaet_cnt >= 25)
//				{
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
//					isZoomExecute = 1;
//					//tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_IN;
//					UI_ZoomDrawSubMenuItem();
//					touch_repaet_cnt = 0;
//				}
//			}
//			else if (Touch_Info->endX > tOsdImgInfo[2].uwXStart && \
//					 Touch_Info->endX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
//					 Touch_Info->endY > tOsdImgInfo[2].uwYStart && \
//					 Touch_Info->endY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize)
//			{
//				if (isZoomExecute == 0)
//				{
//					touch_repaet_cnt = 25;
//				}
//				else
//				{
//					touch_repaet_cnt++;
//				}

//            	if (touch_repaet_cnt >= 25)
//				{
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
//					isZoomExecute = 1;
//					//tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_OUT;
//					UI_ZoomDrawSubMenuItem();
//					touch_repaet_cnt = 0;
//				}
//			}
//			else if (Touch_Info->endX > tOsdImgInfo[4].uwXStart && \
//					 Touch_Info->endX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
//					 Touch_Info->endY > tOsdImgInfo[4].uwYStart && \
//					 Touch_Info->endY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize)
//			{
//				if (isZoomExecute == 0)
//				{
//					touch_repaet_cnt = 25;
//				}
//				else
//				{
//					touch_repaet_cnt++;
//				}

//            	if (touch_repaet_cnt >= 25)
//				{
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_near[0], 7, 8);
//					isZoomExecute = 1;
//					//tOSD_Img2(&tOsdImgInfo[5], OSD_UPDATE);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_NEAR;
//					UI_ZoomDrawSubMenuItem();
//					touch_repaet_cnt = 0;
//				}
//			}
//			else if (Touch_Info->endX > tOsdImgInfo[6].uwXStart && \
//					 Touch_Info->endX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
//					 Touch_Info->endY > tOsdImgInfo[6].uwYStart && \
//					 Touch_Info->endY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize)
//			{
//				if (isZoomExecute == 0)
//				{
//					touch_repaet_cnt = 25;
//				}
//				else
//				{
//					touch_repaet_cnt++;
//				}

//            	if (touch_repaet_cnt >= 25)
//				{
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_far[0], 7, 8);
//					isZoomExecute = 1;
//					//tOSD_Img2(&tOsdImgInfo[7], OSD_UPDATE);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_FAR;
//					UI_ZoomDrawSubMenuItem();
//					touch_repaet_cnt = 0;
//				}
//			}
//			else
//			{
//				OSD_IMG_INFO tOsdImgInfo1[3];
//				tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_DIGITAL_ZOOM_SLIDE, 3, &tOsdImgInfo1[0]);
//				if(Touch_Info->endX >= tOsdImgInfo1[0].uwXStart - 10 
//					&& Touch_Info->endX <= tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize + 10
//					&& Touch_Info->endY >= tOsdImgInfo1[0].uwYStart - 40
//					&& Touch_Info->endY <= tOsdImgInfo1[0].uwYStart + tOsdImgInfo1[0].uwVSize + 40)
//				{
//					if(Touch_Info->endX < tOsdImgInfo1[0].uwXStart)
//						Touch_Info->endX = tOsdImgInfo1[0].uwXStart;

//					if(Touch_Info->endX > tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize)
//						Touch_Info->endX = tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize;

//					//
//					ubPreVolumeLvl = tUI_CamStatus[DeskTopShowView].digitalZoomScale;
//						uint8_t ubValue = (uint8_t)UI_Map(Touch_Info->endX, \
//														  tOsdImgInfo1[0].uwXStart, tOsdImgInfo1[0].uwXStart + tOsdImgInfo1[0].uwHSize, \
//														  10, 50);

//					tUI_CamStatus[DeskTopShowView].digitalZoomScale = ubValue;
//					//printf("new digital zoom scale value = %d\n", ubValue);


//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_SLIDER;
//					UI_ZoomDrawSubMenuItem();
//					
//					updateDigitalZoomSlide(DeskTopShowView, ubPreVolumeLvl);
//					isZoomSlideChange = 1;
//				}
//			}
//			break;
//		case TOUCH_PRESS:
//			// if(ubisnotZoomflag)
//			// {
//			// 	ubisnotZoomflag = 0;
//			// 	UI_MenuKey();
//			// }
//			touch_repaet_cnt = 0;
//			if(isShowFoucsRestart==1){
//				if(handle_zoom_touch_restart_ops(Touch_Info)==1)
//				{
//					break;
//				}
//				return;
//			}
//			if (isZoomExecute == 1 || (\
//										((Touch_Info->startX > tOsdImgInfo[0].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[0].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize) ||\
//										(Touch_Info->startX > tOsdImgInfo[2].uwXStart &&\
//										Touch_Info->startX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[2].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize) ||\
//										(Touch_Info->startX > tOsdImgInfo[4].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[4].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize) ||\
//										(Touch_Info->startX > tOsdImgInfo[6].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[6].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize))))
//			{
//                if (isZoomExecute == 1 && ((Touch_Info->startX > tOsdImgInfo[0].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[0].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize) ||\
//										(Touch_Info->startX > tOsdImgInfo[2].uwXStart &&\
//										Touch_Info->startX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[2].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize)))
//                {
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//					isZoomExecute = 0;
//				}
//				else if(isZoomExecute == 1 && ((Touch_Info->startX > tOsdImgInfo[4].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[4].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize) ||\
//										(Touch_Info->startX > tOsdImgInfo[6].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[6].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize)))
//				{
//					isZoomExecute = 0;
//				}
//				else if((Touch_Info->startX > tOsdImgInfo[0].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[0].uwXStart + tOsdImgInfo[0].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[0].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[0].uwYStart + tOsdImgInfo[0].uwVSize))
//				{
//					//tOSD_Img2(&tOsdImgInfo[1], OSD_UPDATE);
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_IN;
//				}
//				else if((Touch_Info->startX > tOsdImgInfo[2].uwXStart &&\
//										Touch_Info->startX < tOsdImgInfo[2].uwXStart + tOsdImgInfo[2].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[2].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[2].uwYStart + tOsdImgInfo[2].uwVSize))
//				{
//					//tOSD_Img2(&tOsdImgInfo[3], OSD_UPDATE);
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_OUT;
//				}
//				else if((Touch_Info->startX > tOsdImgInfo[4].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[4].uwXStart + tOsdImgInfo[4].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[4].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[4].uwYStart + tOsdImgInfo[4].uwVSize))
//				{
//					//tOSD_Img2(&tOsdImgInfo[5], OSD_UPDATE);
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_near[0], 7, 8);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_NEAR;
//				}
//				else if((Touch_Info->startX > tOsdImgInfo[6].uwXStart && \
//										Touch_Info->startX < tOsdImgInfo[6].uwXStart + tOsdImgInfo[6].uwHSize && \
//										Touch_Info->startY > tOsdImgInfo[6].uwYStart && \
//										Touch_Info->startY < tOsdImgInfo[6].uwYStart + tOsdImgInfo[6].uwVSize))
//				{
//					tOSD_Img2(&tOsdImgInfo[7], OSD_UPDATE);
//					send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_far[0], 7, 8);
//					tZoomSettingCameraItem.tSubMenuInfo.ubItemIdx = ZOOM_FAR;
//				}
//				UI_ZoomDrawSubMenuItem();
//				break;
//			}
//			else if (DeskTopShowView <= CAM4VIEW_ITEM)
//			{
//				if (handle_zoom_touch_ops(Touch_Info) == 1)
//				{
//					UI_ZoomDrawSubMenuItem();
//					break;
//				}
//				else if (isZoomSlideChange == 1)
//				{
//					isZoomSlideChange = 0;
//					sendDigitalZoomScale(tUI_CamStatus[DeskTopShowView].digitalZoomScale);
//					UI_UpdateDevStatusInfo();
//				}
//			}
//			break;
//		default :
//			touch_repaet_cnt = 0;
//			if (isZoomExecute == 1)
//			{
//				send_ret = tTWC_Send(TWC_STA1 + DeskTopShowView, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//				isZoomExecute = 0;
//				break;
//			}
//			else if (isZoomSlideChange == 1)
//			{
//				isZoomSlideChange = 0;
//				sendDigitalZoomScale(tUI_CamStatus[DeskTopShowView].digitalZoomScale);
//				UI_UpdateDevStatusInfo();
//				break;
//			}
//			break;
//	}
}

#endif
