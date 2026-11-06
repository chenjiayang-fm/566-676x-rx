#include "BSP.h"

#if defined(BSP_D_SNCC71_GM8285C_RX_V2) && defined(RTC676x)
#include "UI_BUCCU[WSVGA].h"
#include "UI_AlgCanvas.h"
#include "RTC676x_CTRL.h"

// Algorithm enable flags are indexed by camera; canvases are indexed by slot.
static uint8_t ubUI_Hdw24gAlgEnable[CAM_4T];
osMutexId osUI_AlgCanvasMutex;
static uint32_t ulUI_Hdw24gCanvasSeq;
// 画布固定属于显示 slot，更换 CAM 只换数据源，不改变画布位置和大小。
typedef struct
{
	// tArea 为屏幕范围；旧框和人车图标保存 slot 内的局部坐标，供下次局部擦除。
	OSD_IMG_INFO tArea;
	OSD_IMG_INFO tBox[HDW24G_MAX_OBJECTS], tFlag[2];
	uint8_t ubBoxCount, ubFlagCount, ubCam, ubOptions;
	uint16_t uwResultLen;
	uint8_t ubResult[HDW24G_RESULT_SIZE];
}UI_Hdw24gCanvas_t;
static UI_Hdw24gCanvas_t tUI_Hdw24gCanvas[CAM_4T];
#define UI_HDW24G_DRAW_WAIT_MS 50
static osThreadId osUI_Hdw24gDrawThdId;
static void UI_Hdw24gDrawThread(void const *argument);

void UI_AlgCanvasInit(void)
{
	if(NULL == osUI_Hdw24gDrawThdId)
	{
		osThreadDef(UiHdw24gDraw, UI_Hdw24gDrawThread, osPriorityBelowNormal, 1, 4096);
		osUI_Hdw24gDrawThdId = osThreadCreate(osThread(UiHdw24gDraw), NULL);
		RTC676x_SetHdw24gNotifyThread(osUI_Hdw24gDrawThdId);
	}
}
void UI_AlgCanvasReset(void)
{
	// 与整屏清除配套：只使旧记录失效，像素由调用方清除，后续不再按旧坐标擦屏。
	ulUI_Hdw24gCanvasSeq++;
}
void UI_AlgCanvasSetEnable(uint8_t ubCam, uint8_t ubEnable)
{
	ubUI_Hdw24gAlgEnable[ubCam] = ubEnable;
}
uint8_t UI_AlgCanvasGetEnable(uint8_t ubCam)
{
	return ubUI_Hdw24gAlgEnable[ubCam];
}

#define READ_RECT_LINE_LENGTH 12
#define READ_RECT_LINE_WIDTH 4
// 仅用 OSD1 背景恢复旧框的四条 4 像素边，保留框内区域。
static void UI_ClearHdw24gBox(OSD_IMG_INFO *pBox)
{
	OSD_IMG_INFO tLine = *pBox;
	tLine.uwVSize = READ_RECT_LINE_WIDTH;
	OSD_EraserImg2_NoUpdate(&tLine);
	tLine.uwYStart += pBox->uwVSize - READ_RECT_LINE_WIDTH;
	OSD_EraserImg2_NoUpdate(&tLine);
	tLine.uwYStart = pBox->uwYStart + READ_RECT_LINE_WIDTH;
	tLine.uwHSize = READ_RECT_LINE_WIDTH;
	tLine.uwVSize = pBox->uwVSize - READ_RECT_LINE_WIDTH*2;
	OSD_EraserImg2_NoUpdate(&tLine);
	tLine.uwXStart += pBox->uwHSize - READ_RECT_LINE_WIDTH;
	OSD_EraserImg2_NoUpdate(&tLine);
}

// 沿用 RS 的小目标扩框方式，再平移或裁剪到当前 slot 安全区，避免覆盖分割线。
static void UI_FitAIBoxToSafeArea(UI_LinePoint_t *ptCorner, OSD_IMG_INFO *tArea)
{
	int32_t xMin = tArea->uwXStart;
	int32_t yMin = tArea->uwYStart;
	int32_t xMax = xMin + tArea->uwHSize - 1;
	int32_t yMax = yMin + tArea->uwVSize - 1;
	int32_t x1 = ptCorner[0].xPoint, y1 = ptCorner[0].yPoint, x2 = ptCorner[1].xPoint, y2 = ptCorner[1].yPoint;
	int32_t offset;

	if(x2 - x1 < READ_RECT_LINE_LENGTH)
	{
		if(xMax - x2 < READ_RECT_LINE_LENGTH)
			x1 -= READ_RECT_LINE_LENGTH;
		else
			x2 += READ_RECT_LINE_LENGTH;
	}
	if(y2 - y1 < READ_RECT_LINE_LENGTH)
	{
		if(yMax - y2 < READ_RECT_LINE_LENGTH)
			y1 -= READ_RECT_LINE_LENGTH;
		else
			y2 += READ_RECT_LINE_LENGTH;
	}
	if(x1 < xMin)
	{
		offset = xMin - x1;
		x1 += offset;
		x2 += offset;
	}
	if(x2 > xMax)
	{
		offset = x2 - xMax;
		x1 -= offset;
		x2 -= offset;
	}
	if(y1 < yMin)
	{
		offset = yMin - y1;
		y1 += offset;
		y2 += offset;
	}
	if(y2 > yMax)
	{
		offset = y2 - yMax;
		y1 -= offset;
		y2 -= offset;
	}
	if(x1 < xMin) x1 = xMin;
	if(y1 < yMin) y1 = yMin;
	ptCorner[0].xPoint = (uint16_t)x1;
	ptCorner[0].yPoint = (uint16_t)y1;
	ptCorner[1].xPoint = (uint16_t)x2;
	ptCorner[1].yPoint = (uint16_t)y2;
}

// 只擦本 slot 上次绘制的框和图标；访问 OSD2 时才将局部坐标加上画布偏移。
static void UI_ClearHdw24gCanvas(UI_Hdw24gCanvas_t *pCanvas)
{
	OSD_IMG_INFO tRect;
	for(uint8_t i = 0; i < pCanvas->ubBoxCount; i++)
	{
		tRect = pCanvas->tBox[i];
		tRect.uwXStart += pCanvas->tArea.uwXStart;
		tRect.uwYStart += pCanvas->tArea.uwYStart;
		UI_ClearHdw24gBox(&tRect);
	}
	for(uint8_t i = 0; i < pCanvas->ubFlagCount; i++)
	{
		tRect = pCanvas->tFlag[i];
		tRect.uwXStart += pCanvas->tArea.uwXStart;
		tRect.uwYStart += pCanvas->tArea.uwYStart;
		OSD_EraserImg2_NoUpdate(&tRect);
	}
	pCanvas->ubBoxCount = pCanvas->ubFlagCount = 0;
}

static void UI_DrawHdw24gResult(void)
{
	uint8_t ubResult[HDW24G_RESULT_SIZE];
	uint8_t ubSlotCount, ubUpdated = FALSE;
	static uint8_t ubLoaded, ubOldSlotCount;
	static uint32_t ulCanvasSeq;
	static OSD_IMG_INFO tPerson[2], tCar[2];
	UI_CamViewType_t tViewType = tCamViewSel.tCamViewType;
	// 菜单或非稳定预览期间暂停绘制，避免覆盖菜单和切换画面。
	if(MenuOnFlag || tUI_State != UI_DISPLAY_STATE || !ubUI_FinishViewSwitch ||
		ubUI_CuPowerDiscFlag || (ubUI_CuStandbyFlag && !TriggerLock))
		return;
	switch(tViewType)
	{
		case SINGLE_VIEW: ubSlotCount = 1; break;
		case DUAL_VIEW: ubSlotCount = 2; break;
		case QUAD_VIEW: ubSlotCount = 4; break;
		default: return;
	}
	if(ulCanvasSeq != ulUI_Hdw24gCanvasSeq || ubOldSlotCount != ubSlotCount)
	{
		// 只换布局时先擦旧框；若已整屏清除，旧坐标作废，不能再擦新画面。
		if(ulCanvasSeq == ulUI_Hdw24gCanvasSeq && ubOldSlotCount)
		{
			for(uint8_t slot = 0; slot < ubOldSlotCount; slot++)
				UI_ClearHdw24gCanvas(&tUI_Hdw24gCanvas[slot]);
			ubUpdated = TRUE;
		}
		// 单画面 1024x600，双画面各 512x600，四分割各 512x300。
		memset(tUI_Hdw24gCanvas, 0, sizeof(tUI_Hdw24gCanvas));
		for(uint8_t slot = 0; slot < ubSlotCount; slot++)
		{
			UI_Hdw24gCanvas_t *pCanvas = &tUI_Hdw24gCanvas[slot];
			pCanvas->tArea.uwHSize = (tViewType == SINGLE_VIEW)?OSD_WIDTH:OSD_WIDTH/2;
			pCanvas->tArea.uwVSize = (tViewType == QUAD_VIEW)?OSD_HEIGHT/2:OSD_HEIGHT;
			pCanvas->tArea.uwXStart = (slot%2)*pCanvas->tArea.uwHSize;
			pCanvas->tArea.uwYStart = (slot/2)*pCanvas->tArea.uwVSize;
			pCanvas->ubCam = CAM_4T;
		}
		ubOldSlotCount = ubSlotCount;
		ulCanvasSeq = ulUI_Hdw24gCanvasSeq;
	}
	for(uint8_t slot = 0; slot < ubSlotCount; slot++)
	{
		UI_Hdw24gCanvas_t *pCanvas = &tUI_Hdw24gCanvas[slot];
		uint8_t ubCam = tCamViewSel.tCamViewPool[slot];
		uint8_t ubTypes = 0;
		// bit0: 人检测，bit1: 车检测，bit2: 显示框；开关变化也触发本 slot 更新。
		uint8_t ubOptions = !!tUI_CuSetting.ubDetectPeopleFlag[ubCam] |
			(!!tUI_CuSetting.ubDetectCarFlag[ubCam] << 1) | (!!ubDrawBSDRange << 2);
		uint16_t uwLen = 0, uwBaseW, uwBaseH;
		OSD_IMG_INFO tSafeArea = {0}, tBox;
		// 接口只返回当前有效缓存；失效/过期、零目标或关闭人车检测时仅清本 slot。
		if(ubUI_Hdw24gAlgEnable[ubCam] && tUI_CamStatus[ubCam].tCamConnSts == CAM_ONLINE)
			uwLen = RTC676x_GetHdw24gResult(APP_GetSTANumMappingTable(ubCam)->tPAIR_StaNum,
				ubResult, sizeof(ubResult));
		if((uwLen && !ubResult[5]) || !(ubOptions & 3))
			uwLen = 0;
		// Keep the slot when result, camera and options are unchanged.
		if(uwLen == pCanvas->uwResultLen && ubCam == pCanvas->ubCam && ubOptions == pCanvas->ubOptions &&
			(!uwLen || memcmp(pCanvas->ubResult, ubResult, uwLen) == 0))
			continue;
		ubUpdated |= (pCanvas->ubBoxCount || pCanvas->ubFlagCount);
		UI_ClearHdw24gCanvas(pCanvas);
		pCanvas->uwResultLen = uwLen;
		pCanvas->ubCam = ubCam;
		pCanvas->ubOptions = ubOptions;
		if(!uwLen)
			continue;
		memcpy(pCanvas->ubResult, ubResult, uwLen);
		if(!ubLoaded)
		{
			OSD_IMG_INFO tColors[3];
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_PERSON_DETECUED_FLAG, 2, tPerson);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_Car_DETECUED_FLAG, 2, tCar);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AI_RANGE_RED, 1, &tColors[0]);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AI_RANGE_YELLOW, 1, &tColors[1]);
			tOSD_GetOsdImgInfor(1, OSD_IMG2, OSD2IMG_AI_RANGE_GREEN, 1, &tColors[2]);
			get_bmp_idx(tColors);
			ubLoaded = TRUE;
		}
		// 缩放仍使用完整画布尺寸，仅在分割边缘各留 2 像素，保护中间 4 像素分割线。
		tSafeArea.uwHSize = pCanvas->tArea.uwHSize;
		tSafeArea.uwVSize = pCanvas->tArea.uwVSize;
		if(tViewType != SINGLE_VIEW)
		{
			tSafeArea.uwHSize -= 2;
			if(slot%2) tSafeArea.uwXStart = 2;
		}
		if(tViewType == QUAD_VIEW)
		{
			tSafeArea.uwVSize -= 2;
			if(slot/2) tSafeArea.uwYStart = 2;
		}
		ubUpdated = TRUE;
		// 已校验载荷：12~15 为坐标基准宽高；16 起每目标 10 字节，16 位字段为小端。
		uwBaseW = (uint16_t)ubResult[12] | ((uint16_t)ubResult[13] << 8);
		uwBaseH = (uint16_t)ubResult[14] | ((uint16_t)ubResult[15] << 8);
		for(uint8_t i = 0; i < ubResult[5]; i++)
		{
			// type(1人/2车)、alarm(0无/1绿/2黄/3红)、x、y、w、h。
			const uint8_t *pObj = ubResult + 16 + i*10;
			UI_LinePoint_t tCorner[2];
			uint16_t uwX, uwY, uwW, uwH;
			if((pObj[0] == 1 && !tUI_CuSetting.ubDetectPeopleFlag[ubCam]) ||
				(pObj[0] == 2 && !tUI_CuSetting.ubDetectCarFlag[ubCam]) || pObj[0] < 1 || pObj[0] > 2)
				continue;
			// 每 slot 最多绘制 20/10/5 个框；达到上限后仍遍历目标，汇总人车图标。
			ubTypes |= pObj[0];
			if(!ubDrawBSDRange || pObj[1] < 1 || pObj[1] > 3 || pCanvas->ubBoxCount >= HDW24G_MAX_OBJECTS/ubSlotCount)
				continue;
			uwX = (uint16_t)pObj[2] | ((uint16_t)pObj[3] << 8);
			uwY = (uint16_t)pObj[4] | ((uint16_t)pObj[5] << 8);
			uwW = (uint16_t)pObj[6] | ((uint16_t)pObj[7] << 8);
			uwH = (uint16_t)pObj[8] | ((uint16_t)pObj[9] << 8);
			// TX 坐标先缩放到 slot 局部坐标，边界修正后再加屏幕偏移绘制。
			tCorner[0].xPoint = (uint32_t)uwX*pCanvas->tArea.uwHSize/uwBaseW;
			tCorner[0].yPoint = (uint32_t)uwY*pCanvas->tArea.uwVSize/uwBaseH;
			tCorner[1].xPoint = ((uint32_t)(uwX + uwW)*pCanvas->tArea.uwHSize - 1)/uwBaseW;
			tCorner[1].yPoint = ((uint32_t)(uwY + uwH)*pCanvas->tArea.uwVSize - 1)/uwBaseH;
			UI_FitAIBoxToSafeArea(tCorner, &tSafeArea);
			tBox.uwXStart = tCorner[0].xPoint + pCanvas->tArea.uwXStart;
			tBox.uwYStart = tCorner[0].yPoint + pCanvas->tArea.uwYStart;
			tBox.uwHSize = tCorner[1].xPoint - tCorner[0].xPoint + 1;
			tBox.uwVSize = tCorner[1].yPoint - tCorner[0].yPoint + 1;
			if(tOSD_Img2_DrawBox(&tBox, pObj[1], OSD_QUEUE) == OSD_OK)
			{
				tBox.uwXStart = tCorner[0].xPoint;
				tBox.uwYStart = tCorner[0].yPoint;
				pCanvas->tBox[pCanvas->ubBoxCount++] = tBox;
			}
		}
		for(uint8_t i = 0; i < 2; i++)
		{
			if(!(ubTypes & (1 << i))) 
				continue;
			tBox = (i == 0)?tPerson[tViewType != SINGLE_VIEW]:tCar[tViewType != SINGLE_VIEW];
			tBox.uwXStart = (i == 0)?450:((tViewType == QUAD_VIEW)?500:550);
			if(tViewType != SINGLE_VIEW) tBox.uwXStart -= OSD_WIDTH/4;
			tBox.uwYStart = 10;
			pCanvas->tFlag[pCanvas->ubFlagCount++] = tBox;
			tBox.uwXStart += pCanvas->tArea.uwXStart;
			tBox.uwYStart += pCanvas->tArea.uwYStart;
			tOSD_Img2(&tBox, OSD_QUEUE);
		}
	}
	if(ubUpdated)
		OSD_UpdateImg2Buf();
}

// 一个线程读取各 CAM 最新缓存，各路到包独立刷新，不等待四路凑齐，也不排队绘制旧结果。
static void UI_Hdw24gDrawThread(void const *argument)
{
	while(1)
	{
		uint8_t ubStaMask = 0, ubSlotCount = 0;
		// Protect layout and canvas only; status icons keep their own UI lock.
		osMutexWait(osUI_AlgCanvasMutex, osWaitForever);
		if(ubUI_FinishViewSwitch)
		{
			switch(tCamViewSel.tCamViewType)
			{
				case SINGLE_VIEW: ubSlotCount = 1; break;
				case DUAL_VIEW: ubSlotCount = 2; break;
				case QUAD_VIEW: ubSlotCount = 4; break;
				default: break;
			}
			for(uint8_t slot = 0; slot < ubSlotCount; slot++)
				ubStaMask |= 1 << APP_GetSTANumMappingTable(tCamViewSel.tCamViewPool[slot])->tPAIR_StaNum;
			RTC676x_SetHdw24gVisibleMask(ubStaMask);
			UI_DrawHdw24gResult();
		}
		osMutexRelease(osUI_AlgCanvasMutex);
		// Let status updates run even when result signals are continuous.
		osDelay(1);
		osSignalWait(0, UI_HDW24G_DRAW_WAIT_MS);
	}
}

#endif
