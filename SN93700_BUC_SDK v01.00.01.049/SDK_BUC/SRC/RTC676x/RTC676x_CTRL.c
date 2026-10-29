/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		RTC676x_CTRL.c
	\brief		RTC676x Control
	\author		Justin
	\version	0.22
	\date		2020/07/15
	\copyright	Copyright(C) 2020 SONiX Technology Co.,Ltd. All rights reserved.
*/
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "APP_HS.h"
#include "BUF.h"
#include "RTC676x_CTRL.h"
#if OP_AP
#include "VDO.h"
#endif
#include "rwrf.h"
#include "PAIR.h"
#include "cmd.h"
#include "cmd_rtc676x.h"

//------------------------------------------------------------------------------

#define debug(...) printf(__VA_ARGS__)

osThreadId RTC676x_TestThreadId;

int RTC676x_MasterSocketFd = 0;
int RTC676x_SlaveSocketFd = 0;
int RTC676x_SocketFd = 0;

int master_id = 0xA0;
int slave_id[4] = { 0xC0, 0xC1, 0xC2, 0xC3 };
int baseband_rate = PREF_BBR_QPSK;
int tx_pwr_att = MAX_PWR_ATT;
int tx_pwr_mgc = MAX_PWR_MGC;
unsigned int vco_group[4];

void RTC676x_SetPower(RW_PWR_PKT tPkt,RW_PWR_SEL tPwrSel)
{
	int att; 		// 4 = max, 0 = min power
	int mgc; 		// 7 = min, 0 = max power

	if(tPkt == RW_PWR_PKT_ACK)
	{
		if(tPwrSel == RW_PWR_HIGH)
		{
			att = RW_PWR_ATT_HIGH_ACK;
			mgc = RW_PWR_MGC_HIGH_ACK;
		}
		else if(tPwrSel == RW_PWR_MEDIUM)
		{
			att = RW_PWR_ATT_MEDIUM_ACK;
			mgc = RW_PWR_MGC_MEDIUM_ACK;
		}
		else if(tPwrSel == RW_PWR_LOW)
		{
			att = RW_PWR_ATT_LOW_ACK;
			mgc = RW_PWR_MGC_LOW_ACK;
		}	

		printf("PKT_ACK_PWR:(%d,%d)\r\n",att,mgc);
		rf_setopt(RF_SO_ACKPWRATT, &att, sizeof(att));
		rf_setopt(RF_SO_ACKPWRMGC, &mgc, sizeof(mgc));
	}
	else if(tPkt == RW_PWR_PKT_BEACON)
	{
		if(tPwrSel == RW_PWR_HIGH)
		{
			att = RW_PWR_ATT_HIGH_BEACON;
			mgc = RW_PWR_MGC_HIGH_BEACON;
		}
		else if(tPwrSel == RW_PWR_MEDIUM)
		{
			att = RW_PWR_ATT_MEDIUM_BEACON;
			mgc = RW_PWR_MGC_MEDIUM_BEACON;
		}
		else if(tPwrSel == RW_PWR_LOW)
		{
			att = RW_PWR_ATT_LOW_BEACON;
			mgc = RW_PWR_MGC_LOW_BEACON;
		}
		printf("PKT_BCN_PWR:(%d,%d)\r\n",att,mgc);		
		rf_setopt(RF_SO_BCPWRATT, &att, sizeof(att));
		rf_setopt(RF_SO_BCPWRMGC, &mgc, sizeof(mgc));
	}
	else if(tPkt == RW_PWR_PKT_TX)
	{
		if(tPwrSel == RW_PWR_HIGH)
		{
			att = RW_PWR_ATT_HIGH_TX;
			mgc = RW_PWR_MGC_HIGH_TX;
		}
		else if(tPwrSel == RW_PWR_MEDIUM)
		{
			att = RW_PWR_ATT_MEDIUM_TX;
			mgc = RW_PWR_MGC_MEDIUM_TX;
		}
		else if(tPwrSel == RW_PWR_LOW)
		{
			att = RW_PWR_ATT_LOW_TX;
			mgc = RW_PWR_MGC_LOW_TX;
		}
		printf("PKT_TX_PWR:(%d,%d)\r\n",att,mgc);
		rf_setopt(RF_SO_TXPWRATT, &att, sizeof(att));
		rf_setopt(RF_SO_TXPWRMGC, &mgc, sizeof(mgc));
	}
}

static void load_id(void)
{
    uint32_t *ptr = NULL;
    ptr = (uint32_t *)PAIR_GetId(PAIR_AP_SLAVE);
    if (*ptr == PAIR_INVALID_ID) {
        master_id = 0;
        debug("master id = invalid id\n");
    } else {
        master_id = *ptr;
        debug("master id = %08X\n", master_id);
    }

    int i;
    for (i = 0; i < 4; i++) {
        ptr = (uint32_t *)PAIR_GetId((PAIR_TAG)i);
        if (*ptr == PAIR_INVALID_ID) {
            slave_id[i] = 0;
            debug("slave id[%d] = invalid id\n", i);
        } else {
            slave_id[i] = *ptr;
            debug("slave id[%d] = %08X\n", i, slave_id[i]);
        }
    }
}

void update_id(void)
{
	int ulMasterId;
	int ulSlaveId[4];
#ifdef BUC_CAM
	int index;
#endif
	
	load_id();	

//#ifdef VBM_PU	
#ifdef BUC_CU
	//Update Kernel Information
	//======================================
	KNL_SetMasterId(master_id);
	KNL_SetSlaveId(0,slave_id[0]);
	KNL_SetSlaveId(1,slave_id[1]);
	KNL_SetSlaveId(2,slave_id[2]);
	KNL_SetSlaveId(3,slave_id[3]);	
	
	ulMasterId		= ulKNL_GetMasterId();
	ulSlaveId[0]	= ulKNL_GetSlaveId(0);
	ulSlaveId[1]	= ulKNL_GetSlaveId(1);
	ulSlaveId[2]	= ulKNL_GetSlaveId(2);
	ulSlaveId[3]	= ulKNL_GetSlaveId(3);	
	
	printf("ID->M(%d):0x%x,0x%x,0x%x,0x%x,0x%x\r\n",ubKNL_GetTRXSlotNum(),ulMasterId,ulSlaveId[0],ulSlaveId[1],ulSlaveId[2],ulSlaveId[3]);	
		
	rf_set_local_id(ulMasterId);
	rf_set_remote_id(ulSlaveId,ubKNL_GetTRXSlotNum());
#else

    index = PAIR_GetStaNumber();
    KNL_SetRole(index);	
	
	//Update Kernel Information
	//======================================
	KNL_SetMasterId(master_id);
	KNL_SetSlaveId(index,slave_id[index]);	
	
	ulMasterId			= ulKNL_GetMasterId();
	ulSlaveId[index]	= ulKNL_GetSlaveId(index);
	
	printf("ID->M:0x%x,S(%d)0x%x\r\n",ulMasterId,index,ulSlaveId[index]);
	
    rf_set_local_id(ulSlaveId[index]);
    rf_set_remote_id(&ulMasterId, 1);

#endif
}

void RTC676x_DelId(uint8_t ubStaNum)
{
#ifdef OP_AP
	uint8_t i;
	uint8_t ubNoMapCaseFlg = 1;	
	uint8_t ubSlotNum = 0;
	int ulSlaveId[4];	
	
	ubSlotNum = ubKNL_GetTRXSlotNum();	
	
	ubNoMapCaseFlg = 1;
	for(i=0;i<ubSlotNum;i++)
	{
		if(rf_get_remote_id(i) != ulKNL_GetSlaveId(i))
			ubNoMapCaseFlg = 0;
	}
	
	if(DISPLAY_MODE != ubSlotNum)
	{
		int iRfId[4] = {0, 0, 0, 0};
		uint32_t ulKnlId;

		switch(ubSlotNum)
		{
			case 1:
				iRfId[0] = rf_get_remote_id(0);
				ulKnlId  = ulKNL_GetSlaveId(ubStaNum);
				if(iRfId[0] == ulKnlId)
				{
					iRfId[0] = 0;
					rf_set_remote_id(iRfId, ubKNL_GetTRXSlotNum());
				}
				break;
			case 2:
			{
				uint8_t ubIdx;

				ulKnlId  = ulKNL_GetSlaveId(ubStaNum);
				for(ubIdx = 0; ubIdx < ubSlotNum; ubIdx++)
					iRfId[ubIdx] = rf_get_remote_id(ubIdx);
				for(ubIdx = 0; ubIdx < ubSlotNum; ubIdx++)
				{
					if(iRfId[ubIdx] == ulKnlId)
					{
						iRfId[ubIdx] = 0;
						rf_set_remote_id(iRfId, ubKNL_GetTRXSlotNum());
						break;
					}
				}
				break;
			}
			default:
				break;
		}
	}
	slave_id[ubStaNum] = 0;
	KNL_SetSlaveId(ubStaNum, slave_id[ubStaNum]);
	
	if(ubNoMapCaseFlg)
	{			
		//printf("No Mapping Case\r\n");
		ulSlaveId[0]	= ulKNL_GetSlaveId(0);
		ulSlaveId[1]	= ulKNL_GetSlaveId(1);
		ulSlaveId[2]	= ulKNL_GetSlaveId(2);
		ulSlaveId[3]	= ulKNL_GetSlaveId(3);
		rf_set_remote_id(ulSlaveId,ubSlotNum);	
	}
	if(ubSlotNum == 1)
	{
		if((rf_get_remote_id(0) == ulKNL_GetSlaveId(0)) || (rf_get_remote_id(0) == ulKNL_GetSlaveId(1))|| (rf_get_remote_id(0) == ulKNL_GetSlaveId(2))|| (rf_get_remote_id(0) == ulKNL_GetSlaveId(3)))
		{
			//printf("1 Slot Case\r\n");
			
			ulSlaveId[0] = rf_get_remote_id(0);
			ulSlaveId[1] = 0;
			ulSlaveId[2] = 0;
			ulSlaveId[3] = 0;
			rf_set_remote_id(ulSlaveId,4);				
		}
	}
#endif
}

//justin 2020.07.13 for Richwave RF @Low Temperature
//=======================================================
static void RTC676x_Main_Init(int ulId)
{
	int ret = 0,i;
	
	for(i=0;i<2;i++)
	{
#if OP_AP
		ret = rf_master_init(ulId);
#else
		ret = rf_slave_init(ulId);
#endif
		if(ret == 0)
			break;
	}
	if(ret != 0)
	{
		printf("[RTC6763] warm up!\r\n");
		{
			int vco_setting = 1;
			
			for(i=0;i<4;i++)
			{
				unsigned int vco_group = APP_GetRFVCOGroup(i);
				rf_setopt(RF_OPT_VCO_GROUP_0+i,&vco_group,sizeof(vco_group));
			}
			rf_setopt(RF_OPT_VCO_USER_SETTING,&vco_setting,sizeof(vco_setting));
		}
	#if OP_AP
		ret = rf_master_init(ulId);
	#else
		ret = rf_slave_init(ulId);
	#endif
	}
	else
	{
		#if 0
		int i;
		for(i=0;i<4;i++)
		{
			unsigned int vco_group;
			size_t vco_group_len;
			rf_getopt(RF_OPT_VCO_GROUP_0+i,&vco_group,&vco_group_len);
			APP_SetRFVCOGroup(i,(uint32_t)vco_group);
		}
		#else
		for(uint8_t i = 0;i < 4;i ++)
		{
			size_t vco_group_len;
			rf_getopt(RF_OPT_VCO_GROUP_0+i,&vco_group[i],&vco_group_len);
		}
		APP_SetRFVCOGroup(vco_group);
		#endif
	}	
}

//=======================================================
void RTC676x_Init(void)
{	
#if OP_AP
	int ulMasterId;
	int ulSlaveId[4];		  
	int inquiry_beacon_time = 133;    
	//int inquiry_beacon_time = 266;
	
#if 0
	//Speed up linking
	//====================================================================================================
	//Reduce broadcast beacon interval (Default is 1000 ms)
    int broadcast_link_interval = 500;
    rf_setopt(RF_OPT_BROADCAST_LINK_INTERVAL, &broadcast_link_interval, sizeof(broadcast_link_interval));

    //Reduce number of beacon (Default is 160)
    int max_num_bc = 80;
    rf_setopt(RF_OPT_MAX_NUM_BC, &max_num_bc, sizeof(max_num_bc));

    //Reduce IF filter calibration delay (Default is 30 ms)
    int if_loop_delay = 20;
    rf_setopt(RF_OPT_IF_FLT_LOOP_DELAY, &if_loop_delay, sizeof(if_loop_delay));

    //Reduce DC filter calibration delay (Default is 30 ms)
    int dc_loop_delay = 20;
    rf_setopt(RF_OPT_DC_FLT_LOOP_DELAY, &dc_loop_delay, sizeof(dc_loop_delay));
	//====================================================================================================
#endif

	//Speed up linking
	//====================================================================================================
	//Reduce broadcast beacon interval (Default is 1000 ms)
    int broadcast_link_interval = 500;
    rf_setopt(RF_OPT_BROADCAST_LINK_INTERVAL, &broadcast_link_interval, sizeof(broadcast_link_interval));

    //Reduce number of beacon (Default is 160)
    int max_num_bc = 80;
    rf_setopt(RF_OPT_MAX_NUM_BC, &max_num_bc, sizeof(max_num_bc));

    //Reduce IF filter calibration delay (Default is 30 ms)
    int if_loop_delay = 30;
    rf_setopt(RF_OPT_IF_FLT_LOOP_DELAY, &if_loop_delay, sizeof(if_loop_delay));

    //Reduce DC filter calibration delay (Default is 30 ms)
    int dc_loop_delay = 30;
    rf_setopt(RF_OPT_DC_FLT_LOOP_DELAY, &dc_loop_delay, sizeof(dc_loop_delay));
	//====================================================================================================
	
	//EVEN Section
	KNL_SetEvenSlot(APP_EVEN_SLOT);	
	printf("EvenSlot : %d\r\n",KNL_GetEvenSlot());
	ulMasterId		= ulKNL_GetMasterId();
	ulSlaveId[0]	= ulKNL_GetSlaveId(0);
	ulSlaveId[1]	= ulKNL_GetSlaveId(1);
	ulSlaveId[2]	= ulKNL_GetSlaveId(2);
	ulSlaveId[3]	= ulKNL_GetSlaveId(3);	
	
	rf_setopt(RF_OPT_INQ_BC_TIME, &inquiry_beacon_time, sizeof(inquiry_beacon_time));
	
	//justin 2020.07.14	For Low Temperature
	//rf_master_init(ulMasterId);	
	RTC676x_Main_Init(ulMasterId);
	
	//rf_set_remote_id(ulSlaveId,ubKNL_GetTRXSlotNum());
	KNL_SetRwRemoteId(ulSlaveId,ubKNL_GetTRXSlotNum());		
		
	RTC676x_SetPower(RW_PWR_PKT_ACK,RW_PWR_HIGH);
	RTC676x_SetPower(RW_PWR_PKT_BEACON,RW_PWR_HIGH);
	RTC676x_SetPower(RW_PWR_PKT_TX,RW_PWR_HIGH);

    KNL_SetSocketFd(RTC676x_SocketFd);

	if(rf_set_packet_priority(PRI_TYPE_SMS,PRI_LVL_HIGH) == (-1))
	{
		printf("Set Cmd Packet priority Err !!!\r\n");
	}
	if(rf_set_packet_priority(PRI_TYPE_AUD,PRI_LVL_MEDIUM) == (-1))
	{
		printf("Set ADO Packet priority Err !!!\r\n");
	}
	if(rf_set_packet_priority(PRI_TYPE_VID,PRI_LVL_LOW) == (-1))
	{
		printf("Set VDO Packet priority Err !!!\r\n");
	}
	
#else
	int ulMasterId;
	int ulSlaveId;	
	
#if 0
	//Speed up linking
	//====================================================================================================
	//Reduce broadcast beacon interval (Default is 1000 ms)
    int broadcast_link_interval = 500;
    rf_setopt(RF_OPT_BROADCAST_LINK_INTERVAL, &broadcast_link_interval, sizeof(broadcast_link_interval));

    //Reduce number of beacon (Default is 160)
    int max_num_bc = 80;
    rf_setopt(RF_OPT_MAX_NUM_BC, &max_num_bc, sizeof(max_num_bc));

    //Reduce IF filter calibration delay (Default is 30 ms)
    int if_loop_delay = 20;
    rf_setopt(RF_OPT_IF_FLT_LOOP_DELAY, &if_loop_delay, sizeof(if_loop_delay));

    //Reduce DC filter calibration delay (Default is 30 ms)
    int dc_loop_delay = 20;
    rf_setopt(RF_OPT_DC_FLT_LOOP_DELAY, &dc_loop_delay, sizeof(dc_loop_delay));
	//====================================================================================================
#endif
	
	//EVEN Section	
	KNL_SetEvenSlot(APP_EVEN_SLOT);
	printf("EvenSlot : %d\r\n",KNL_GetEvenSlot());

	ulMasterId	= ulKNL_GetMasterId();
	ulSlaveId 	= ulKNL_GetSlaveId(ubKNL_GetRole());
	
	//justin 2020.07.14 For Low Temperature
	//rf_slave_init(ulSlaveId);
	RTC676x_Main_Init(ulSlaveId);	
	
    rf_set_remote_id(&ulMasterId, 1);	
	
	if(KNL_GetCurRwBbRateMode() == PREF_BBR_QPSK)
		baseband_rate = PREF_BBR_QPSK;
	else if(KNL_GetCurRwBbRateMode() == PREF_BBR_16QAM)
		baseband_rate = PREF_BBR_16QAM;
		
    rf_setopt(RF_SO_PREFBBR, &baseband_rate, sizeof(baseband_rate));	
   
	if(APP_MODULATION_MODE == MODULATION_FIXED_QPSK)
	{		
		RTC676x_SetPower(RW_PWR_PKT_ACK,RW_PWR_HIGH);
		RTC676x_SetPower(RW_PWR_PKT_BEACON,RW_PWR_HIGH);
		RTC676x_SetPower(RW_PWR_PKT_TX,RW_PWR_HIGH);
	}
	else if(APP_MODULATION_MODE == MODULATION_FIXED_16QAM)
	{		
		RTC676x_SetPower(RW_PWR_PKT_ACK,RW_PWR_HIGH);		//Keep High Power
		RTC676x_SetPower(RW_PWR_PKT_BEACON,RW_PWR_HIGH);	//Keep High Power
		RTC676x_SetPower(RW_PWR_PKT_TX,RW_PWR_MEDIUM);
	}
	else if(APP_MODULATION_MODE == MODULATION_AUTO)
	{		
		RTC676x_SetPower(RW_PWR_PKT_ACK,RW_PWR_HIGH);
		RTC676x_SetPower(RW_PWR_PKT_BEACON,RW_PWR_HIGH);
		RTC676x_SetPower(RW_PWR_PKT_TX,RW_PWR_HIGH);
	}	
	
	KNL_SetSocketFd(RTC676x_SocketFd);	
	
	if(rf_set_packet_priority(PRI_TYPE_SMS,PRI_LVL_HIGH) == (-1))
	{
		printf("Set Cmd Packet priority Err !!!\r\n");
	}
	if(rf_set_packet_priority(PRI_TYPE_AUD,PRI_LVL_MEDIUM) == (-1))
	{
		printf("Set ADO Packet priority Err !!!\r\n");
	}
	if(rf_set_packet_priority(PRI_TYPE_VID,PRI_LVL_LOW) == (-1))
	{
		printf("Set VDO Packet priority Err !!!\r\n");
	}
#endif	
	
	//Channel Setting
	//---------------------------------------------------------	
	unsigned char frequency_table[80] = 
	{		
		 38, 41, 45, 48, 52, 55, 58, 62, 72, 75,	// 0- 9
		 78, 82, 85, 88, 95, 98,102,105,108,105,	//10-19		
		102, 98, 95, 88, 85, 82, 78, 75, 72, 62,	//20-29
		 58, 55, 52, 48, 45, 41, 38, 41, 45, 48,	//30-39
		 52, 55, 58, 62, 72, 75, 78, 82, 85, 88,	//40-49
		 95, 98,102,105,108,105,102, 98, 95, 88,	//50-59
		 85, 82, 78, 75, 72, 62, 58, 55, 52, 48,	//60-69
		 45, 41, 38, 41, 45, 48, 52, 55, 58, 62		//70-79
	};			
	rf_setopt(RF_OPT_FREQTABLE, frequency_table,80);		
}	

void RTC676x_CmdThread(void const *arg)
{
	//uint8_t reset = 1;
	
	int flags = 0;
	int Id;   
	//uint8_t ubBuf[1024];
	uint8_t ubBuf[PACKET_BUF_LEN];
	int result;
	uint32_t ulTemp;
#if OP_AP
	uint32_t i;
#endif
	KNL_PROCESS tProc;	

	while(1)
	{			
		result = rf_recv_smsg_packet(ubBuf, sizeof(ubBuf), flags, &Id);
		
		if(result== MAX_CMD_PACKET_LEN)
		{
		#if OP_AP
			for(i=0; i<4; i++)
			{
				if(Id == ulKNL_GetSlaveId(i))
				{
					switch(i)
					{
						case 0:
							ulTemp = ulBUF_GetPacket0FreeBuf();
							break;
						case 1:
							ulTemp = ulBUF_GetPacket1FreeBuf();
							break;
						case 2:
							ulTemp = ulBUF_GetPacket2FreeBuf();
							break;
						case 3:
							ulTemp = ulBUF_GetPacket3FreeBuf();
							break;
					}
					if(ulTemp != BUF_FAIL)
					{
						memcpy((uint8_t *)ulTemp,ubBuf,result);
						
						//Next				
						tProc.ulDramAddr2 	= ulTemp;
						tProc.ulSize 		= result;
						if(osMessagePut(KNL_PacketQue[i].Id, &tProc, 0) == osErrorResource)
						{
							printd(DBG_ErrorLvl, "PacketQueue[%d]->Full !!!!\r\n",i);
						}
					}
					else
					{
						printf("Busy[%d] @%s\r\n",i,__func__);
					}			
				}
			}
		#endif
		#if OP_STA			
			if(Id == ulKNL_GetMasterId())
			{
				ulTemp = ulBUF_GetPacket0FreeBuf();
					
				if(ulTemp != BUF_FAIL)
				{
					memcpy((uint8_t *)ulTemp,ubBuf,result);
					
					//Next				
					tProc.ulDramAddr2 	= ulTemp;
					tProc.ulSize 		= result;
					if(osMessagePut(KNL_PacketQue[0].Id, &tProc, 0) == osErrorResource)
					{
						printd(DBG_ErrorLvl, "PacketQueue[0]->Full !!!!\r\n");
					}
				}
				else
				{
					printf("Busy1 @%s\r\n",__func__);
				}
			}			
		#endif
		}
		else if(result > 0)
		{
			printf("!!! RxCmd-Err:%d\r\n",result);
			
			//printf("RF Software Reset(1) @%s\r\n",__func__);				
			//rf_setopt(RF_OPT_SW_RST, &reset, 1);
			//printf("RF Software Reset(2) @%s\r\n",__func__);
			ubKNL_AccessErrCnt(KNL_OPERATION_PLUS,0);			
		}
		else
		{
			printf("Err_%d @%s\r\n",result,__func__);
			
			//printf("RF Software Reset(1) @%s\r\n",__func__);				
			//rf_setopt(RF_OPT_SW_RST, &reset, 1);
			//printf("RF Software Reset(2) @%s\r\n",__func__);
			ubKNL_AccessErrCnt(KNL_OPERATION_PLUS,0);
		}	
	}
}

#if OP_AP
#define HDW24G_MAX_OBJECTS 20
#define HDW24G_RESULT_SIZE (18 + HDW24G_MAX_OBJECTS * 10)
// 机型保存在配对 Flash；RAM 仅记录变化，用于刷新本通道和缓存算法结果。
typedef struct
{
	uint32_t ulId;
	uint8_t ubCamType;
	uint16_t uwResultLen;
	uint32_t ulResultTick, ulPrintSeq;
	uint8_t ubPrintValid, ubPrintCount;
	uint8_t ubResult[HDW24G_RESULT_SIZE];
}RTC676x_Hdw24gState_t;
static RTC676x_Hdw24gState_t tRTC676x_Hdw24g[4];
static osMutexId osRTC676x_Hdw24gMutex;

static uint32_t RTC676x_Hdw24gGet32(const uint8_t *pData)
{
	return (uint32_t)pData[0] | ((uint32_t)pData[1] << 8) |
		((uint32_t)pData[2] << 16) | ((uint32_t)pData[3] << 24);
}
void RTC676x_Hdw24gInit(void)
{
	osMutexDef(RTC676x_Hdw24gMutex);
	osRTC676x_Hdw24gMutex = osMutexCreate(osMutex(RTC676x_Hdw24gMutex));
}
uint16_t RTC676x_GetHdw24gResult(uint8_t ubStaNum, uint8_t *pData, uint16_t uwSize)
{
	uint16_t uwLen = 0;
	if(ubStaNum > 3 || !osRTC676x_Hdw24gMutex)
		return 0;
	osMutexWait(osRTC676x_Hdw24gMutex, osWaitForever);
	if(tRTC676x_Hdw24g[ubStaNum].ulId == ulKNL_GetSlaveId(ubStaNum) &&
		(uint32_t)(osKernelSysTick() - tRTC676x_Hdw24g[ubStaNum].ulResultTick) <= osKernelSysTickFrequency)
		uwLen = tRTC676x_Hdw24g[ubStaNum].uwResultLen;
	if(uwLen > uwSize)
		uwLen = 0;
	if(uwLen)
		memcpy(pData, tRTC676x_Hdw24g[ubStaNum].ubResult, uwLen);
	osMutexRelease(osRTC676x_Hdw24gMutex);
	return uwLen;
}
void RTC676x_Hdw24gPoll(void)
{
	RTC676x_Hdw24gState_t *pState;
	uint8_t ubSta, ubCamType, ubRefresh, ubPrint, ubResult[HDW24G_RESULT_SIZE];
	uint16_t uwLen, uwIdx;
	uint32_t ulId, ulTick = osKernelSysTick(), ulSeq;
	if(!osRTC676x_Hdw24gMutex || ubKNL_GetStopTrxFlg())
		return;
	for(ubSta = 0; ubSta < 4; ubSta++)
	{
		ulId = ulKNL_GetSlaveId(ubSta);
		ubCamType = PAIR_IsHdw24gCam((PAIR_TAG)ubSta);
		ubPrint = ubRefresh = FALSE;
		osMutexWait(osRTC676x_Hdw24gMutex, osWaitForever);
		pState = &tRTC676x_Hdw24g[ubSta];
		if(pState->ulId != ulId || pState->ubCamType != ubCamType)
		{
			// 配对、删除或升级补齐机型后，仅刷新一次对应通道。
			memset(pState, 0, sizeof(*pState));
			pState->ulId = ulId;
			pState->ubCamType = ubCamType;
			ubRefresh = TRUE;
		}
		if(!ulId || ulId == 0xFFFFFFFF || ulId == PAIR_INVALID_ID || !rf_get_link_status(ulId))
		{
			// 离线只清算法缓存，持久机型和通道能力继续保留。
			pState->uwResultLen = 0;
			pState->ubPrintValid = FALSE;
		}
		uwLen = pState->uwResultLen;
		if(uwLen && (uint32_t)(ulTick - pState->ulResultTick) > osKernelSysTickFrequency)
			pState->uwResultLen = uwLen = 0;
		if(uwLen)
		{
			ulSeq = RTC676x_Hdw24gGet32(pState->ubResult + 8);
			if(!pState->ubPrintValid || pState->ulPrintSeq != ulSeq || pState->ubPrintCount != pState->ubResult[5])
			{
				memcpy(ubResult, pState->ubResult, uwLen);
				pState->ulPrintSeq = ulSeq;
				pState->ubPrintCount = ubResult[5];
				pState->ubPrintValid = ubPrint = TRUE;
			}
		}
		osMutexRelease(osRTC676x_Hdw24gMutex);
		if(ubRefresh)
			VDO_RefreshCamResolution(ubSta);
		// 打印放在周期线程，RF 线程只校验和缓存，避免逐框日志阻塞视频收包。
		if(ubPrint)
		{
			printf("[HDW24G][ALG] cam=%u id=%08X seq=%u count=%u base=%ux%u\r\n",
				ubSta + 1, ulId, ulSeq, ubResult[5],
				(uint16_t)ubResult[12] | ((uint16_t)ubResult[13] << 8),
				(uint16_t)ubResult[14] | ((uint16_t)ubResult[15] << 8));
			for(uwIdx = 16; uwIdx < uwLen - 2; uwIdx += 10)
				printf("[HDW24G][BOX] type=%u x=%u y=%u w=%u h=%u\r\n", ubResult[uwIdx],
					(uint16_t)ubResult[uwIdx + 2] | ((uint16_t)ubResult[uwIdx + 3] << 8),
					(uint16_t)ubResult[uwIdx + 4] | ((uint16_t)ubResult[uwIdx + 5] << 8),
					(uint16_t)ubResult[uwIdx + 6] | ((uint16_t)ubResult[uwIdx + 7] << 8),
					(uint16_t)ubResult[uwIdx + 8] | ((uint16_t)ubResult[uwIdx + 9] << 8));
		}
	}
}
#endif

#if OP_AP && defined(BSP_D_SNCC71_GM8285C_RX_V2)
// 与 TX 的 B24G v1 小端布局一致：16 字节头 + 最多 20 个 10 字节目标 + 2 字节校验。
// 校验后仅更新 RTC676x 的 RAM 缓存，暂不接入 UI/OSD 绘框。
static void RTC676x_RecvHdw24gResult(uint8_t ubStaNum, const uint8_t *pPkt, uint16_t uwPktLen)
{
	const uint8_t *pData = pPkt + PKT_HEADER_SZ;
	uint16_t uwLen, uwSum = 0, uwIdx;
	uint16_t uwBaseW, uwBaseH, uwX, uwY, uwW, uwH;

	if(!osRTC676x_Hdw24gMutex || uwPktLen < PKT_HEADER_SZ + 18)
		return;
	for(uwIdx = 0; uwIdx < PKT_ACTIVE_HEADER_SZ; uwIdx++)
		uwSum += pPkt[uwIdx];
	if(uwSum != ((uint16_t)pPkt[12] | ((uint16_t)pPkt[13] << 8)))
		return;
	uwLen = (uint16_t)pData[6] | ((uint16_t)pData[7] << 8);
	if(memcmp(pData, "B24G", 4) || pData[4] != 1 || pData[5] > HDW24G_MAX_OBJECTS ||
		uwLen != 18 + pData[5] * 10 || uwLen > uwPktLen - PKT_HEADER_SZ)
		return;
	uwSum = 0;
	for(uwIdx = 0; uwIdx < uwLen - 2; uwIdx++)
		uwSum += pData[uwIdx];
	if(uwSum != ((uint16_t)pData[uwLen - 2] | ((uint16_t)pData[uwLen - 1] << 8)))
		return;
	uwBaseW = (uint16_t)pData[12] | ((uint16_t)pData[13] << 8);
	uwBaseH = (uint16_t)pData[14] | ((uint16_t)pData[15] << 8);
	if(pData[5] && (!uwBaseW || !uwBaseH))
		return;
	for(uwIdx = 16; uwIdx < uwLen - 2; uwIdx += 10)
	{
		uwX = (uint16_t)pData[uwIdx + 2] | ((uint16_t)pData[uwIdx + 3] << 8);
		uwY = (uint16_t)pData[uwIdx + 4] | ((uint16_t)pData[uwIdx + 5] << 8);
		uwW = (uint16_t)pData[uwIdx + 6] | ((uint16_t)pData[uwIdx + 7] << 8);
		uwH = (uint16_t)pData[uwIdx + 8] | ((uint16_t)pData[uwIdx + 9] << 8);
		if(uwX >= uwBaseW || uwY >= uwBaseH || !uwW || !uwH ||
			uwW > uwBaseW - uwX || uwH > uwBaseH - uwY)
			return;
	}
	// 接收线程零等待获取缓存锁；只接收已配对并记录机型的 TX 结果。
	if(osMutexWait(osRTC676x_Hdw24gMutex, 0) != osOK)
		return;
	if(PAIR_IsHdw24gCam((PAIR_TAG)ubStaNum) &&
		tRTC676x_Hdw24g[ubStaNum].ulId == ulKNL_GetSlaveId(ubStaNum))
	{
		memcpy(tRTC676x_Hdw24g[ubStaNum].ubResult, pData, uwLen);
		tRTC676x_Hdw24g[ubStaNum].uwResultLen = uwLen;
		tRTC676x_Hdw24g[ubStaNum].ulResultTick = osKernelSysTick();
	}
	osMutexRelease(osRTC676x_Hdw24gMutex);
}
#endif
//------------------------------------------------------------------------------
void RTC676x_VdoThread(void const *arg)
{
    //uint8_t reset = 1;
	
	int flags = 0;
	int Id;   
	//uint8_t ubBuf[1024];
	uint8_t ubBuf[PACKET_BUF_LEN];
	int result;
	uint32_t ulTemp;
	
	KNL_PROCESS tProc;
#if OP_AP
	uint32_t i;
#endif
	uint16_t uwRW_S2M_SZ_BPSK_1T;
	uint16_t uwRW_S2M_SZ_BPSK_NT;
	uint16_t uwRW_S2M_SZ_QPSK_1T;
	uint16_t uwRW_S2M_SZ_QPSK_NT;
	uint16_t uwRW_S2M_SZ_16QAM_1T;
	uint16_t uwRW_S2M_SZ_16QAM_NT;
	
	if(KNL_GetEvenSlot() == 1200)
	{
		uwRW_S2M_SZ_BPSK_1T		= RW_S2M_SZ_BPSK_1T_1200;
		uwRW_S2M_SZ_BPSK_NT		= RW_S2M_SZ_BPSK_NT_1200;
		uwRW_S2M_SZ_QPSK_1T		= RW_S2M_SZ_QPSK_1T_1200;
		uwRW_S2M_SZ_QPSK_NT		= RW_S2M_SZ_QPSK_NT_1200;
		uwRW_S2M_SZ_16QAM_1T	= RW_S2M_SZ_16QAM_1T_1200;
		uwRW_S2M_SZ_16QAM_NT	= RW_S2M_SZ_16QAM_NT_1200;
	}
	else if(KNL_GetEvenSlot() == 1667)
	{
		uwRW_S2M_SZ_BPSK_1T		= RW_S2M_SZ_BPSK_1T_1667;
		uwRW_S2M_SZ_BPSK_NT		= RW_S2M_SZ_BPSK_NT_1667;
		uwRW_S2M_SZ_QPSK_1T		= RW_S2M_SZ_QPSK_1T_1667;
		uwRW_S2M_SZ_QPSK_NT		= RW_S2M_SZ_QPSK_NT_1667;
		uwRW_S2M_SZ_16QAM_1T	= RW_S2M_SZ_16QAM_1T_1667;
		uwRW_S2M_SZ_16QAM_NT	= RW_S2M_SZ_16QAM_NT_1667;
	}
	while(1)
	{			
		result = rf_recv_video_packet(ubBuf, sizeof(ubBuf), flags, &Id);	

		//printf("%d\r\n",result);
		
		//if(result == MAX_VDO_PACKET_LEN)
		if(	(result == uwRW_S2M_SZ_BPSK_1T)||(result == uwRW_S2M_SZ_BPSK_NT)||
			(result == uwRW_S2M_SZ_QPSK_1T)||(result == uwRW_S2M_SZ_QPSK_NT)||
			(result == uwRW_S2M_SZ_16QAM_1T)||(result == uwRW_S2M_SZ_16QAM_NT) )		
		{
		#if OP_AP			
			for(i=0; i<4; i++)
			{
				if(Id == ulKNL_GetSlaveId(i))
				{
					// type 5 为按已保存 RX 能力发送的算法包，必须在旧 CMD 分发/视频重组前单独截取。
					if(ubBuf[PKT_INFO_OFS_TYPE] == 5)
					{
					#ifdef BSP_D_SNCC71_GM8285C_RX_V2
						RTC676x_RecvHdw24gResult(i, ubBuf, result);
					#endif
						break;
					}
					switch(i)
					{
						case 0:
							ulTemp = ulBUF_GetPacket0FreeBuf();
							break;
						case 1:
							ulTemp = ulBUF_GetPacket1FreeBuf();
							break;
						case 2:
							ulTemp = ulBUF_GetPacket2FreeBuf();
							break;
						case 3:
							ulTemp = ulBUF_GetPacket3FreeBuf();
							break;
					}
					if(ulTemp != BUF_FAIL)
					{
						memcpy((uint8_t *)ulTemp,ubBuf,result);
						
						//Next				
						tProc.ulDramAddr2 	= ulTemp;
						tProc.ulSize 		= result;
						if(osMessagePut(KNL_PacketQue[i].Id, &tProc, 0) == osErrorResource)
						{
							printd(DBG_ErrorLvl, "PacketQueue[%d]->Full !!!!\r\n",i);
						}
					}
					else
					{
						printf("Busy[%d] @%s\r\n",i,__func__);
					}			
				}
			}
		#endif
		#if OP_STA			
			if(Id == ulKNL_GetMasterId())
			{
				ulTemp = ulBUF_GetPacket0FreeBuf();
					
				if(ulTemp != BUF_FAIL)
				{
					memcpy((uint8_t *)ulTemp,ubBuf,result);
					
					//Next				
					tProc.ulDramAddr2 	= ulTemp;
					tProc.ulSize 		= result;
					if(osMessagePut(KNL_PacketQue[0].Id, &tProc, 0) == osErrorResource)
					{
						printd(DBG_ErrorLvl, "PacketQueue[0]->Full !!!!\r\n");
					}
				}
				else
				{
					printf("Busy1 @%s\r\n",__func__);
				}
			}			
		#endif
		}
		else if(result > 0)
		{
			printf("!!! RxVdo-Err:%d\r\n",result);
			
			//printf("RF Software Reset(1) @%s\r\n",__func__);				
			//rf_setopt(RF_OPT_SW_RST, &reset, 1);
			//printf("RF Software Reset(2) @%s\r\n",__func__);			
			ubKNL_AccessErrCnt(KNL_OPERATION_PLUS,0);
		}
		else
		{
			printf("Err_%d @%s\r\n",result,__func__);
			
			//printf("RF Software Reset(1) @%s\r\n",__func__);				
			//rf_setopt(RF_OPT_SW_RST, &reset, 1);
			//printf("RF Software Reset(2) @%s\r\n",__func__);
			ubKNL_AccessErrCnt(KNL_OPERATION_PLUS,0);
		}	
	}
}

void RTC676x_AdoThread(void const *arg)
{
    //uint8_t reset = 1;
	
	int flags = 0;
	int Id;    
	//uint8_t ubBuf[1024];
	uint8_t ubBuf[PACKET_BUF_LEN];
	int result;
	uint32_t ulTemp;
	
	KNL_PROCESS tProc;	
#if OP_AP
	uint32_t i;
#endif
	while(1)
	{		
		result = rf_recv_audio_packet(ubBuf, sizeof(ubBuf), flags, &Id);
		
		if(result == MAX_ADO_PACKET_LEN)
		{
		#if OP_AP			
			for(i=0; i<4; i++)
			{
				if(Id == ulKNL_GetSlaveId(i))
				{				
					switch(i)
					{
						case 0:
							ulTemp = ulBUF_GetPacket0FreeBuf();
							break;
						case 1:
							ulTemp = ulBUF_GetPacket1FreeBuf();
							break;
						case 2:
							ulTemp = ulBUF_GetPacket2FreeBuf();
							break;
						case 3:
							ulTemp = ulBUF_GetPacket3FreeBuf();
							break;
					}
					if(ulTemp != BUF_FAIL)
					{
						memcpy((uint8_t *)ulTemp,ubBuf,result);
						
						//Next				
						tProc.ulDramAddr2 	= ulTemp;
						tProc.ulSize 		= result;
						if(osMessagePut(KNL_PacketQue[i].Id, &tProc, 0) == osErrorResource)
						{
							printd(DBG_ErrorLvl, "PacketQueue[%d]->Full !!!!\r\n",i);
						}
					}
					else
					{
						printf("Busy[%d] @%s\r\n",i,__func__);
					}			
				}
			}
		#endif
		#if OP_STA			
			if(Id == ulKNL_GetMasterId())
			{
				ulTemp = ulBUF_GetPacket0FreeBuf();
					
				if(ulTemp != BUF_FAIL)
				{
					memcpy((uint8_t *)ulTemp,ubBuf,result);
					
					//Next				
					tProc.ulDramAddr2 	= ulTemp;
					tProc.ulSize 		= result;
					if(osMessagePut(KNL_PacketQue[0].Id, &tProc, 0) == osErrorResource)
					{
						printd(DBG_ErrorLvl, "PacketQueue[0]->Full !!!!\r\n");
					}
				}
				else
				{
					printf("Busy1 @%s\r\n",__func__);
				}
			}			
		#endif
		}
		else if(result > 0)
		{
			printf("!!! RxAdo-Err:%d\r\n",result);
			
			//printf("RF Software Reset(1) @%s\r\n",__func__);				
			//rf_setopt(RF_OPT_SW_RST, &reset, 1);
			//printf("RF Software Reset(2) @%s\r\n",__func__);
			ubKNL_AccessErrCnt(KNL_OPERATION_PLUS,0);
		}
		else
		{
			printf("Err_%d @%s\r\n",result,__func__);
			
			//printf("RF Software Reset(1) @%s\r\n",__func__);				
			//rf_setopt(RF_OPT_SW_RST, &reset, 1);
			//printf("RF Software Reset(2) @%s\r\n",__func__);
			ubKNL_AccessErrCnt(KNL_OPERATION_PLUS,0);
		}	
	}
}

void RTC676x_TestPutUartString(char *cmd)
{
	if (strcmp(cmd, "exit") != 0)
		rf_mp_put_uart_string(strlen(cmd), cmd);
	else
		cmd_rtc676x_exit_RFMP_test_mode();
}

void RTC676x_TestThread(void const *arg)
{
    rf_mp_test_main();
}

void RTC676x_TestInit(void)
{
    osThreadDef(RTC676x_TestThreadName, RTC676x_TestThread, THREAD_PRIO_RF_TEST_HANDLER, 1, THREAD_STACK_RF_TEST_HANDLER);
    RTC676x_TestThreadId = osThreadCreate(osThread(RTC676x_TestThreadName), NULL);
}
