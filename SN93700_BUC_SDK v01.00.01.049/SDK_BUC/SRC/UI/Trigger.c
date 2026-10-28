#include "Trigger.h"

static UI_Event_t Trigger_Event;

TRIGGER_EVENT_t Trigger_View;

//------------------------------------------------------------------------------
static void TriggerCheck_Thread(void const *argument)
{
	uint8_t TriggerCheckQue[5];//存优先级0~4对应的tx序号,
	uint8_t TriggerSta[5];	//触发线状态
	uint8_t TriggerDisplay[5];
	uint16_t i, j;
	uint8_t ZoomTriggerFlag[5] = {0};
	static uint8_t FootSwitchlongCount = 0;
	static uint8_t FootSwitchWakeup = FALSE;
	uint16_t SADC_CH1_Value = 0;
	uint16_t SADC_CH2_Value = 0;
	uint8_t ZoomLinkCount = 0, ZoomLinkIndex = 0xFF;
	TOUCH_EVENT_t Touch_Info;
	
	while(1)
	{
		if(MenuOnFlag == FALSE && ubUI_CuPowerOffFlag == FALSE)//菜单开启时不响应触发，进入睡眠模式也不响应触发
		{
			#if 0
				printf("uwSADC_GetReport(SADC_CH1) = %d,uwSADC_GetReport(SADC_CH2) = %d\n",uwSADC_GetReport(SADC_CH1),uwSADC_GetReport(SADC_CH2));
			#endif
			//优先级0~4对应的tx序号存在TriggerCheckQue[i]中
			for(i = 0;i < 5;i++)
				TriggerCheckQue[tUI_CuSetting.TriggerPriority[i]] = i;

			SADC_CH1_Value = BubbleSort_GetAux(SADC_CH1);
			SADC_CH2_Value = BubbleSort_GetAux(SADC_CH2);

//			printf("SADC_CH1_Value = %d\n", SADC_CH1_Value);
			if(SADC_CH1_Value != ADC_VALID && SADC_CH2_Value != ADC_VALID)
			{
				TriggerSta[0] = TRIGGER_L(SADC_CH2_Value);
				TriggerSta[1] = TRIGGER_R(SADC_CH2_Value);
				TriggerSta[2] = TRIGGER_F(SADC_CH2_Value);
				TriggerSta[3] = TRIGGER_B(SADC_CH1_Value);
				TriggerSta[4] = TRIGGER_S(SADC_CH1_Value);

//				printf("TriggerSta[3] = %d, TriggerSta[4] = %d\n", TriggerSta[3], TriggerSta[4]);

				TriggerDisplay[0] = CAM1;
				TriggerDisplay[1] = CAM2;
				TriggerDisplay[2] = CAM3;
				TriggerDisplay[3] = CAM4;
				TriggerDisplay[4] = tUI_CuSetting.TriggerSplitView;

				if(tUI_CuSetting.ubZoomfootswitch)
				{
					if(ubUI_CuStandbyFlag || FootSwitchWakeup)
					{
						if(TriggerSta[CAM1] || TriggerSta[CAM2] || TriggerSta[CAM3] || TriggerSta[CAM4])
						{
							if(!FootSwitchWakeup)
							{
								UI_WakeupStandby();
								FootSwitchWakeup = TRUE;
								FootSwitchLock = TRUE;
							}
						}
						else
						{
							FootSwitchWakeup = FALSE;
							FootSwitchLock = FALSE;
						}
					}
					else
					{
						if(TriggerSta[CAM1] || TriggerSta[CAM4])
						{
							FootSwitchLock = TRUE;
							//跳转时不需要再统计
							if(TriggerSource == 0xFF)
							{
								ZoomLinkCount = 0;
								ZoomLinkIndex = 0xFF;
								//当前页面为单页面，如果待机则统计变焦
								if(DeskTopShowView <= CAM4 && tUI_CuSetting.ubisZoomCameraFlag[DeskTopShowView] == 1)
								{
									ZoomLinkIndex = DeskTopShowView;
									//当前息屏画面为变焦，则会跳转
									//if(ubUI_CuStandbyFlag)
									//{
									//	ZoomLinkCount = 1;
									//}
								}
								else//统计变焦,是否跳转
								{
									for(j = 0;j < 4; j++)
									{
										if(tUI_CuSetting.ubisZoomCameraFlag[j] == 1)
										{
											ZoomLinkCount++;
											ZoomLinkIndex = j;
										}
									}
									if(ZoomLinkCount > 1)
									{
										ZoomLinkIndex = 0xFF;
									}
								}
							}
							else
							{
								ZoomLinkIndex = TriggerSource;
								//停留十秒
								ulTriggerOverCount = 10 * 1000;
							}
							
							if((ZoomLinkCount == 1 && (UI_CamNum_t)ZoomLinkIndex != TriggerSource))
							{
								osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
								TriggerSource = ZoomLinkIndex;
								if(TriggerLock == FALSE)
								{
									TriggerBackUp_View = DeskTopShowView;//保存触发前的画面
									TriggerLock = TRUE;
									//if(ubUI_CuStandbyFlag)//待机触发
									//{
									//	POWER_LED_IO = 0;
									//}
								}
								Trigger_View.tUI_ViewSel = (UI_CamNum_t)ZoomLinkIndex;
								Trigger_Event.pvEvent = (void *)(&Trigger_View);
								Trigger_Event.tEventType = TRIGGER_EVENT;
								if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
										printd(DBG_ErrorLvl, "Trigger Q Full\n");
								
								ulTriggerOverCount = 10 * 1000;
								//切换状态时不发送TWC
								FootSwitchlongCount = 0;
								osSemaphoreRelease(osUI_CuTriggerCtr);
							}//为单画面变焦时，不需要跳转
							else if(ZoomLinkIndex <= CAM4 && tUI_CamStatus[ZoomLinkIndex].tCamConnSts == CAM_ONLINE && tUI_CuSetting.ubisZoomCameraFlag[ZoomLinkIndex])
							{
	//							TriggerDisplay[TriggerCheckQue[i]] == CAM1
								if(TriggerSta[CAM1])
								{
									if(FootSwitchlongCount == 0)
									{
	//									printf("pelco_in ZoomLinkIndex = %d\n", ZoomLinkIndex);
										tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
									}
									FootSwitchlongCount++;
									if(FootSwitchlongCount >= 25)
									{
										FootSwitchlongCount = 0;
									}
									ZoomTriggerFlag[CAM1] = TRUE;
								}
								else if(TriggerSta[CAM4])
								{
									if(FootSwitchlongCount == 0)
									{
	//									printf("pelco_out ZoomLinkIndex = %d\n", ZoomLinkIndex);
										tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
									}
									FootSwitchlongCount++;
									if(FootSwitchlongCount >= 25)
									{
										FootSwitchlongCount = 0;
									}
									ZoomTriggerFlag[CAM4] = TRUE;
								}
							}
						}
						else if(TriggerSta[CAM2] || TriggerSta[CAM3])
						{
							FootSwitchLock = TRUE;
							if(TriggerSta[CAM2] && !ZoomTriggerFlag[CAM2])
							{
								Touch_Info.Gesture = TOUCH_TURNLEFT;
								UI_DisplayTouchFunc(&Touch_Info);
								ZoomTriggerFlag[CAM2] = TRUE;
							}
							else if(TriggerSta[CAM3] && !ZoomTriggerFlag[CAM3])
							{
								Touch_Info.Gesture = TOUCH_TURNRIGHT;
								UI_DisplayTouchFunc(&Touch_Info);
								ZoomTriggerFlag[CAM3] = TRUE;
							}
						}
						else 
						{
							if(ZoomTriggerFlag[CAM1] || ZoomTriggerFlag[CAM4])
							{
								ZoomTriggerFlag[CAM1] = FALSE;
								ZoomTriggerFlag[CAM4] = FALSE;
	//							printf("pelco_stop ZoomLinkIndex = %d\n", ZoomLinkIndex);
								tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
								FootSwitchlongCount = 0;
							}
							if(ZoomTriggerFlag[CAM2] || ZoomTriggerFlag[CAM3])
							{
								ZoomTriggerFlag[CAM2] = FALSE;
								ZoomTriggerFlag[CAM3] = FALSE;
							}
							FootSwitchLock = FALSE;
						}
					}
				}
				else
				{
					for(i = 0;i < 5;i++)
					{	
						if(TriggerSta[TriggerCheckQue[i]])//有触发
						{
							if(TriggerSource != TriggerCheckQue[i])//触发源不一样,就执行画面切换
							{
								osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
								TriggerSource = TriggerCheckQue[i];
								if(TriggerLock == FALSE)
								{
									TriggerBackUp_View = DeskTopShowView;//保存触发前的画面
									TriggerLock = TRUE;
									if(ubUI_CuStandbyFlag)//待机触发
									{
										POWER_LED_IO = 0;
									}
								}
								
								Trigger_View.tUI_ViewSel = TriggerDisplay[TriggerCheckQue[i]];
								Trigger_Event.pvEvent = (void *)(&Trigger_View);
								Trigger_Event.tEventType = TRIGGER_EVENT;
								if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
										printd(DBG_ErrorLvl, "Trigger Q Full\n");
								osSemaphoreRelease(osUI_CuTriggerCtr); 
							}
							ulTriggerOverCount = tUI_CuSetting.TriggerDelay[TriggerCheckQue[i]] * 1000;
							break;
						}
					}
				}
			}
		}
		
		osDelay(50);
	}
}


//------------------------------------------------------------------------------
//static void TriggerCheck_Thread(void const *argument)
//{
//	uint8_t TriggerCheckQue[5];//存优先级0~4对应的tx序号,
//	uint8_t TriggerSta[5];	//触发线状态
//	uint8_t TriggerDisplay[5];
//	uint16_t i, j;
//	uint8_t ZoomTriggerFlag[5] = {0};
//	static uint8_t FootSwitchlongCount = 0;
//	uint16_t SADC_CH1_Value = 0;
//	uint16_t SADC_CH2_Value = 0;
//	uint8_t ZoomLinkCount = 0, ZoomLinkIndex = 0xFF;
//	TOUCH_EVENT_t Touch_Info;
//	
//	while(1)
//	{
//		osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
//		if(MenuOnFlag == FALSE && ubUI_CuPowerOffFlag == FALSE)//菜单开启时不响应触发，进入睡眠模式也不响应触发
//		{
//			#if 0
//				printf("uwSADC_GetReport(SADC_CH1) = %d,uwSADC_GetReport(SADC_CH2) = %d\n",uwSADC_GetReport(SADC_CH1),uwSADC_GetReport(SADC_CH2));
//			#endif
//			//优先级0~4对应的tx序号存在TriggerCheckQue[i]中
//			for(i = 0;i < 5;i++)
//				TriggerCheckQue[tUI_CuSetting.TriggerPriority[i]] = i;

//			SADC_CH1_Value = BubbleSort_GetAux(SADC_CH1);
//			SADC_CH2_Value = BubbleSort_GetAux(SADC_CH2);

//			//printf("SADC_CH1_Value = %d\n", SADC_CH1_Value);
//			if(SADC_CH1_Value != ADC_VALID && SADC_CH2_Value != ADC_VALID)
//			{
//				TriggerSta[0] = TRIGGER_L(SADC_CH2_Value);
//				TriggerSta[1] = TRIGGER_R(SADC_CH2_Value);
//				TriggerSta[2] = TRIGGER_F(SADC_CH2_Value);
//				TriggerSta[3] = TRIGGER_B(SADC_CH1_Value);
//				TriggerSta[4] = TRIGGER_S(SADC_CH1_Value);

//				//printf("TriggerSta[3] = %d, TriggerSta[4] = %d\n", TriggerSta[3], TriggerSta[4]);

//				TriggerDisplay[0] = CAM1;
//				TriggerDisplay[1] = CAM2;
//				TriggerDisplay[2] = CAM3;
//				TriggerDisplay[3] = CAM4;
//				TriggerDisplay[4] = tUI_CuSetting.TriggerSplitView;

//				if(tUI_CuSetting.ubZoomfootswitch)
//				{
//					if(TriggerSta[CAM1] || TriggerSta[CAM4])
//					{
//						FootSwitchLock = TRUE;
//						//跳转时不需要再统计
//						if(TriggerSource == 0xFF)
//						{
//							ZoomLinkCount = 0;
//							ZoomLinkIndex = 0xFF;
//							//当前页面为单页面，如果待机则统计变焦
//							if(DeskTopShowView <= CAM4 && tUI_CuSetting.ubisZoomCameraFlag[DeskTopShowView] == 1)
//							{
//								ZoomLinkIndex = DeskTopShowView;
//								//当前息屏画面为变焦，则会跳转
//								if(ubUI_CuStandbyFlag)
//								{
//									ZoomLinkCount = 1;
//								}
//							}
//							else//统计变焦,是否跳转
//							{
//								for(j = 0;j < 4; j++)
//								{
//									if(tUI_CuSetting.ubisZoomCameraFlag[j] == 1)
//									{
//										ZoomLinkCount++;
//										ZoomLinkIndex = j;
//									}
//								}
//								if(ZoomLinkCount > 1)
//								{
//									ZoomLinkIndex = 0xFF;
//								}
//							}
//						}
//						else
//						{
//							ZoomLinkIndex = TriggerSource;
//							//停留十秒
//							ulTriggerOverCount = 10 * 1000;
//						}
//						
//						if((ZoomLinkCount == 1 && (UI_CamNum_t)ZoomLinkIndex != TriggerSource))
//						{
//							TriggerSource = ZoomLinkIndex;
//							if(TriggerLock == FALSE)
//							{
//								TriggerBackUp_View = DeskTopShowView;//保存触发前的画面
//								TriggerLock = TRUE;
//								if(ubUI_CuStandbyFlag)//待机触发
//								{
//									POWER_LED_IO = 0;
//								}
//							}
//							Trigger_View.tUI_ViewSel = (UI_CamNum_t)ZoomLinkIndex;
//							Trigger_Event.pvEvent = (void *)(&Trigger_View);
//							Trigger_Event.tEventType = TRIGGER_EVENT;
//							if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
//									printd(DBG_ErrorLvl, "Trigger Q Full\n");
//							
//							ulTriggerOverCount = 10 * 1000;
//							//切换状态时不发送TWC
//							FootSwitchlongCount = 0;
//						}//为单画面变焦时，不需要跳转
//						else if(ZoomLinkIndex <= CAM4 && tUI_CamStatus[ZoomLinkIndex].tCamConnSts == CAM_ONLINE && tUI_CuSetting.ubisZoomCameraFlag[ZoomLinkIndex])
//						{
//							if(TriggerSta[CAM1])
//							{
//								if(FootSwitchlongCount == 0)
//								{
//									//printf("pelco_in ZoomLinkIndex = %d\n", ZoomLinkIndex);
//									tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
//								}
//								FootSwitchlongCount++;
//								if(FootSwitchlongCount >= 25)
//								{
//									FootSwitchlongCount = 0;
//								}
//								ZoomTriggerFlag[CAM1] = TRUE;
//							}
//							else if(TriggerSta[CAM4])
//							{
//								if(FootSwitchlongCount == 0)
//								{
//									//printf("pelco_out ZoomLinkIndex = %d\n", ZoomLinkIndex);
//									tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
//								}
//								FootSwitchlongCount++;
//								if(FootSwitchlongCount >= 25)
//								{
//									FootSwitchlongCount = 0;
//								}
//								ZoomTriggerFlag[CAM4] = TRUE;
//							}
//						}
//					}
//					else if(TriggerSta[CAM2] || TriggerSta[CAM3])
//					{
//						FootSwitchLock = TRUE;
//						if(TriggerSta[CAM2] && !ZoomTriggerFlag[CAM2])
//						{
//							osSemaphoreRelease(osUI_CuTriggerCtr);
//							Touch_Info.Gesture = TOUCH_TURNLEFT;
//							UI_DisplayTouchFunc(&Touch_Info);
//							osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
//							ZoomTriggerFlag[CAM2] = TRUE;
//						}
//						else if(TriggerSta[CAM3] && !ZoomTriggerFlag[CAM3])
//						{
//							osSemaphoreRelease(osUI_CuTriggerCtr);
//							Touch_Info.Gesture = TOUCH_TURNRIGHT;
//							UI_DisplayTouchFunc(&Touch_Info);
//							osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
//							ZoomTriggerFlag[CAM3] = TRUE;
//						}
//					}
//					else 
//					{
//						if(ZoomTriggerFlag[CAM1] || ZoomTriggerFlag[CAM4])
//						{
//							ZoomTriggerFlag[CAM1] = FALSE;
//							ZoomTriggerFlag[CAM4] = FALSE;
//							//printf("pelco_stop ZoomLinkIndex = %d\n", ZoomLinkIndex);
//							tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//							FootSwitchlongCount = 0;
//						}
//						if(ZoomTriggerFlag[CAM2] || ZoomTriggerFlag[CAM3])
//						{
//							ZoomTriggerFlag[CAM2] = FALSE;
//							ZoomTriggerFlag[CAM3] = FALSE;
//						}
//						FootSwitchLock = FALSE;
//					}
//				}
//				else
//				{
//					for(i = 0;i < 5;i++)
//					{	
//						if(TriggerSta[TriggerCheckQue[i]])//有触发
//						{
//							if(TriggerSource != TriggerCheckQue[i])//触发源不一样,就执行画面切换
//							{
//								TriggerSource = TriggerCheckQue[i];
//								if(TriggerLock == FALSE)
//								{
//									TriggerBackUp_View = DeskTopShowView;//保存触发前的画面
//									TriggerLock = TRUE;
//									if(ubUI_CuStandbyFlag)//待机触发
//									{
//										POWER_LED_IO = 0;
//									}
//								}
//								
//								Trigger_View.tUI_ViewSel = TriggerDisplay[TriggerCheckQue[i]];
//								Trigger_Event.pvEvent = (void *)(&Trigger_View);
//								Trigger_Event.tEventType = TRIGGER_EVENT;
//								if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
//										printd(DBG_ErrorLvl, "Trigger Q Full\n");
//							}
//							ulTriggerOverCount = tUI_CuSetting.TriggerDelay[TriggerCheckQue[i]] * 1000;
//							break;
//						}
//					}
//				}
//			}
//		}

//		osSemaphoreRelease(osUI_CuTriggerCtr);
//		
//		osDelay(50);
//	}
//}


//static void TriggerCheck_Thread(void const *argument)
//{
//	uint8_t TriggerCheckQue[5];//存优先级0~4对应的tx序号,
//	uint8_t TriggerSta[5];	//触发线状态
//	uint8_t TriggerDisplay[5];
//	uint16_t i, j;
//	uint8_t ZoomTriggerFlag[5] = {0};
//	static uint8_t FootSwitchlongCount = 0;
//	uint16_t SADC_CH1_Value = 0;
//	uint16_t SADC_CH2_Value = 0;
//	uint8_t ZoomLinkCount = 0, ZoomLinkIndex = 0;
//	
//	while(1)
//	{
//		osSemaphoreWait(osUI_CuTriggerCtr, osWaitForever);
//		if(MenuOnFlag == FALSE && ubUI_CuPowerOffFlag == FALSE)//菜单开启时不响应触发，进入睡眠模式也不响应触发
//		{
//			#if 0
//				printf("uwSADC_GetReport(SADC_CH1) = %d,uwSADC_GetReport(SADC_CH2) = %d\n",uwSADC_GetReport(SADC_CH1),uwSADC_GetReport(SADC_CH2));
//			#endif
//			//优先级0~4对应的tx序号存在TriggerCheckQue[i]中
//			for(i = 0;i < 5;i++)
//				TriggerCheckQue[tUI_CuSetting.TriggerPriority[i]] = i;

//			SADC_CH1_Value = BubbleSort_GetAux(SADC_CH1);
//			SADC_CH2_Value = BubbleSort_GetAux(SADC_CH2);

//			printf("SADC_CH1_Value = %d\n", SADC_CH1_Value);
//			if(SADC_CH1_Value != ADC_VALID && SADC_CH2_Value != ADC_VALID)
//			{
//				TriggerSta[0] = TRIGGER_L(SADC_CH2_Value);
//				TriggerSta[1] = TRIGGER_R(SADC_CH2_Value);
//				TriggerSta[2] = TRIGGER_F(SADC_CH2_Value);
//				TriggerSta[3] = TRIGGER_B(SADC_CH1_Value);
//				TriggerSta[4] = TRIGGER_S(SADC_CH1_Value);

//				printf("TriggerSta[3] = %d, TriggerSta[4] = %d\n", TriggerSta[3], TriggerSta[4]);


//				TriggerDisplay[0] = CAM1;
//				TriggerDisplay[1] = CAM2;
//				TriggerDisplay[2] = CAM3;
//				TriggerDisplay[3] = CAM4;
//				TriggerDisplay[4] = tUI_CuSetting.TriggerSplitView;
//				
//				for(i = 0;i < 5;i++)
//				{
//					
//					if(TriggerSta[TriggerCheckQue[i]])//有触发
//					{	
//						if(tUI_CuSetting.ubZoomfootswitch)
//						{
//							ZoomLinkCount = 0;
//							ZoomLinkIndex = 0;
//							for(j = 0;j < 4; j++)
//							{
//								if(tUI_CuSetting.ubisZoomCameraFlag[j] == 1)
//								{
//									ZoomLinkCount++;
//									ZoomLinkIndex = j;
//								}
//							}
//						
//							//变焦数量大于1时，不需要跳转，只需要当前画面放大缩小
//							if(ZoomLinkCount > 1)
//							{
//								ZoomLinkIndex = DeskTopShowView;
//							}

//							//不在该单变焦的画面
//							if((UI_CamNum_t)ZoomLinkIndex != DeskTopShowView || ubUI_CuStandbyFlag)
//							{
//								if (ZoomLinkCount == 1)
//								{
//									if(TriggerSource != ZoomLinkIndex)//变焦摄像头画面不一样,就执行画面切换
//									{
//										TriggerSource = ZoomLinkIndex;
//										if(TriggerLock == FALSE)
//										{
//											TriggerBackUp_View = DeskTopShowView;//保存触发前的画面
//											TriggerLock = TRUE;
//											if(ubUI_CuStandbyFlag)//待机触发
//											{
//												POWER_LED_IO = 0;
//											}
//										}
//										Trigger_View.tUI_ViewSel = (UI_CamNum_t)ZoomLinkIndex;
//										Trigger_Event.pvEvent = (void *)(&Trigger_View);
//										Trigger_Event.tEventType = TRIGGER_EVENT;
//										if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
//												printd(DBG_ErrorLvl, "Trigger Q Full\n");
//										
//										ulTriggerOverCount = 10 * 1000;
//										//切换状态时不发送TWC
//										FootSwitchlongCount = 0;
//										break;
//									}
//								}
//								//停留十秒
//								ulTriggerOverCount = 10 * 1000;
//							}

//							
//						
//							if (ZoomLinkIndex > CAM4VIEW_ITEM)
//							{
//								break;
//							}

//							
//							if(TriggerDisplay[TriggerCheckQue[i]] == CAM1 && tUI_CamStatus[ZoomLinkIndex].tCamConnSts == CAM_ONLINE)
//							{
//								//不是跳转触发，只是控制变焦放大缩小，则置位ZoomFootSwitchLock，用于控制变焦时禁用其他动作
//								printf("CAM1 TriggerCheck_Thread ulTriggerOverCount = %d, TriggerLock = %d\n", ulTriggerOverCount, TriggerLock);
//								if(ulTriggerOverCount == 0 && TriggerLock == FALSE)
//								{
//									ZoomFootSwitchLock = TRUE;
//									TriggerLock = TRUE;
//								}
//								if(FootSwitchlongCount == 0)
//								{
//									printf("pelco_in ZoomLinkIndex = %d\n", ZoomLinkIndex);
//									tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_in[0], 7, 8);
//								}
//								FootSwitchlongCount++;
//								if(FootSwitchlongCount >= 25)
//								{
//									FootSwitchlongCount = 0;
//								}
//								ZoomTriggerFlag[TriggerCheckQue[i]] = TRUE;
//							}
//							else if(TriggerDisplay[TriggerCheckQue[i]] == CAM4 && tUI_CamStatus[ZoomLinkIndex].tCamConnSts == CAM_ONLINE)
//							{
//								printf("CAM4 TriggerCheck_Thread ulTriggerOverCount = %d, TriggerLock = %d\n", ulTriggerOverCount, TriggerLock);
//								if(ulTriggerOverCount == 0 && TriggerLock == FALSE)
//								{
//									ZoomFootSwitchLock = TRUE;
//									TriggerLock = TRUE;
//								}
//								if(FootSwitchlongCount == 0)
//								{
//									printf("pelco_out ZoomLinkIndex = %d\n", ZoomLinkIndex);
//									tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_out[0], 7, 8);
//								}
//								FootSwitchlongCount++;
//								if(FootSwitchlongCount >= 25)
//								{
//									FootSwitchlongCount = 0;
//								}
//								ZoomTriggerFlag[TriggerCheckQue[i]] = TRUE;
//							}
//							break;
//						}
//						else{
//							if(TriggerSource != TriggerCheckQue[i])//触发源不一样,就执行画面切换
//							{
//								TriggerSource = TriggerCheckQue[i];
//								if(TriggerLock == FALSE)
//								{
//									TriggerBackUp_View = DeskTopShowView;//保存触发前的画面
//									TriggerLock = TRUE;
//									if(ubUI_CuStandbyFlag)//待机触发
//									{
//										POWER_LED_IO = 0;
//									}
//								}
//								
//								Trigger_View.tUI_ViewSel = TriggerDisplay[TriggerCheckQue[i]];
//								Trigger_Event.pvEvent = (void *)(&Trigger_View);
//								Trigger_Event.tEventType = TRIGGER_EVENT;
//								if(osMessagePut(UI_EventQueue, &Trigger_Event, 0) != osOK)
//										printd(DBG_ErrorLvl, "Trigger Q Full\n");
//							}
//							ulTriggerOverCount = tUI_CuSetting.TriggerDelay[TriggerCheckQue[i]] * 1000;
//							break;
//						}
//					}
//					else 
//					{
//						if(tUI_CuSetting.ubZoomfootswitch && ZoomTriggerFlag[TriggerCheckQue[i]])
//						{
//							if(ulTriggerOverCount == 0 && TriggerLock && ZoomFootSwitchLock)
//							{
//								ZoomFootSwitchLock = FALSE;
//								TriggerLock = FALSE;
//							}
//							ZoomTriggerFlag[TriggerCheckQue[i]] = FALSE;
//							printf("pelco_stop ZoomLinkIndex = %d\n", ZoomLinkIndex);
//							tTWC_Send(TWC_STA1 + ZoomLinkIndex, TWC_PELCO, (uint8_t *)&pelco_stop[0], 7, 8);
//							FootSwitchlongCount = 0;
//						}
//					}
//				}
//			}
//		}

//		osSemaphoreRelease(osUI_CuTriggerCtr);
//		
//		osDelay(50);
//	}
//}
//-----------------------------------------------------------------------
void Trigger_init(void)
{
	//初始化两个AD
	SADC_SetPinMode(SADC_CH2, SADC_PIN_ANALOG_INPUT_MODE);
	SADC_SetPinMode(SADC_CH1, SADC_PIN_ANALOG_INPUT_MODE);
	SADC_Enable();
	osThreadDef(TriggerCheck_Thread, TriggerCheck_Thread, osPriorityBelowNormal, 1, 1024);
	if(osThreadCreate(osThread(TriggerCheck_Thread), NULL) == NULL)
	{
			printd(DBG_ErrorLvl, "Create TouchCheck_Thread fail!\n");
			while(1);
	}	
	printf("Trigger_init\n");
}


