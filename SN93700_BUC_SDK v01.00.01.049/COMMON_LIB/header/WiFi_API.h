/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		WiFi_API.h
	\brief		Wi-Fi header
	\author		Chris
	\version	1.5
	\date		2020/12/04
	\copyright	Copyright(C) 2020 SONiX Technology Co.,Ltd. All rights reserved.
*/
//-----------------------------------------------------------------------------
#ifndef __WIFI_API_H__
#define __WIFI_API_H__

#include "_510PF.h"

#define PF_MAX_STA_NUM					4
#define E_FW_VER_LEN					30 
#define E_PF_SSID_LEN					30                          	
#define E_BUC_APPS_SYNC_PKT_INTERVAL    4  		// 4*60 = 240ms
#define E_BUC_APPS_SYNC_PKT_TIMEOUT	    (E_BUC_APPS_SYNC_PKT_INTERVAL*80 /*14*/)  // 20 sec

//------------------------------------------------------------------------------
#define AP_MAX_NUMBER					1
#define REPEATER_MAX_NUMBER				3
#define AP_WORKING_CHANNEL				11
#define WIFI_CH_MAX_NUM					0x0D
#define PF_ETH_BUF_CNT					10

#define MANUAL_PF_ENCRY_TYPE			0  //! 0:OPEN, 1:WEP
#define MANUAL_SET_PF_SSID_EN			0
#define MANUAL_SET_PF_PASSWORD_EN		1
#define MANUAL_SET_PF_WEP128_ENCRY		0

#define E_PF_PWD_WEP128_MAX_LEN			13
#define E_PF_PWD_WEP64_MAX_LEN			5
#define E_PF_PWD_MAX_LEN				E_PF_PWD_WEP128_MAX_LEN

#define MANUAL_PF_SSID					"SONiX_" 	
// WEP64 PASSWORD -  5 Bytes (10 digital Hex value).
#define MANUAL_PF_PWD					"password"

#define VDO_FRC_FPS						20  		
#define VDO_MIN_FPS						8
#define VDO_QC_DROP_TIME				3
#define E_RVCS_ALL_I_FRM				1

#define UI_CODE_VERSION					"21.01.22.01"       //! YY.MM.DD.XX   XX: build version in day.

#define FLIPW_word( x ) ( (((uint16_t) (x)[1])<<8) + (x)[0] ) 
#define FLIPW_long( x ) ( (((uint32_t) (x)[3])<<24) + (((uint32_t) (x)[2])<<16) + (((uint32_t) (x)[1])<<8) + (x)[0] ) 

enum {
	VDO_SEN_NOCHANGE = 0,
	VDO_SEN_MIRROR,
	VDO_SEN_FLIP,
	VDO_SEN_MIRROR_FLIP,
};

enum {
	VDO_SEN_POWER_FREQ_50HZ = 1,
	VDO_SEN_POWER_FREQ_60HZ = 2,		
};

enum {
	E_OPCODE_SET = 0,
	E_OPCODE_GET,
};


enum {
	E_PF_WEP64_ENCRY = 0,
	E_PF_WEP128_ENCRY,
	E_PF_OPEN, 
};

enum {		
	TARGET_STA0 = 0,
	TARGET_STA1,
	TARGET_STA2,
	TARGET_STA3,	
	TARGET_STA4,
	TARGET_STA5,
	TARGET_STA6,
	TARGET_STA7,
	TARGET_STA8,

	TARGET_AP = 0xF,

	TARGET_RPT0 = 0x10,
	TARGET_RPT1 = 0x20,
	TARGET_RPT2 = 0x40,
	TARGET_RPT3 = 0x80,
	TARGET_ESSID = 0x90,
	TARGET_ERROR = 0xEE,
	TARGET_BROADCAST = 0xFF,
};

//#define E_FW_VER_LEN		30  // TX(4)+BT(?)
#define E_FW_DATA_TX		0x01
#define E_FW_DATA_BT		0x04
#define E_FW_DATA_MASK		0x05

#define APP_VDO_OFFSET_PWD_ENABLE		8
#define APP_VDO_OFFSET_SECURITY_KEY		9
#define APP_VDO_OFFSET_PWD_LEN			10
#define APP_VDO_OFFSET_PWD_DATA			11

enum {  
	E_TWC_CMD_OK = 0,
	E_VDO_STOP_NO_RESOURCE_CONNECT,
	E_VDO_STOP_CERTIFICATE_FAIL,
};

enum {
	E_STATUS_FW_STATUS = 0,
	E_STATUS_FW_BEGIN_UPDATE,	
	E_STATUS_FW_TX_UPDATING,
	E_STATUS_FW_TX_UPDATE_OK,	
	E_STATUS_FW_TX_UPDATE_FAIL,	
	E_STATUS_FW_BT_UPDATE,	

	E_STATUS_APP_PREVIEW_TIMEOUT = 20,
	E_STATUS_WIFI_INITIAL_TIMEOUT
};

enum {
	E_BT_FOTA_FAIL = 0,
	E_BT_FOTA_OK,	
	E_BT_FOTA_PROCESSING,
};

//! Wireless Data(WD) OPCode --> (Internal: 0~127)
typedef enum 
{
	UI_APPTWC_VDO_START         = 0,
	UI_APPTWC_VDO_STOP          = 1,
	UI_APPTWC_VDO_RESTART       = 2,
	UI_APPTWC_VDO_I_FRAME       = 3,
	
	UI_APPTWC_ADO_START         = 4,
	UI_APPTWC_ADO_STOP          = 5,
	UI_APPTWC_ADO_RESTART       = 6,
	
	UI_APPTWC_FRM_OK            = 7,
	UI_APPTWC_FRM_LOST          = 8, 	
	
	UI_APPTWC_PV_MODE_SW        = 9,
	UI_APPTWC_SET_CWMAX         = 10,
	
	UI_APPTWC_STA_ALIVE         = 11,

	UI_APPTWC_SET_CWMIN         = 12,
	UI_APPTWC_SET_AIFSN         = 13,
	UI_APPTWC_SET_TXRTY         = 14,

	UI_APPTWC_REQ_FW_VERSION    = 15,
	UI_APPTWC_GET_FW_VERSION    = 16,
		
	UI_APPTWC_SEND_TE 		    = 17,
	UI_APPTWC_RECE_TE		    = 18,
	UI_APPTWC_CLEAR_TE	        = 19,
	
	UI_APPTWC_NOTIFY_FW_UPDATE	    = 20, 	// 0x14, 
	UI_APPTWC_SEND_FWUPDATE_OK	    = 21,
	UI_APPTWC_SEND_FWUPDATE_FAIL	= 22,
	
	UI_APPTWC_STA_ALIVE_AGAIN	    = 23,

	UI_APPTWC_WEM_SEND_TEST 		= 24,
	UI_APPTWC_WEM_RECEIVE_TEST		= 25,
	UI_APPTWC_WEM_CLEAR_TEST_REPORT	= 26,
	UI_APPTWC_VDO_PARAMETER     	= 27,
	UI_APPTWC_SET_SYNC_TIME_TEST  	= 28,

	UI_APPTWC_UPDATA_FW_INFO        = 0x25,  // 37

	UI_APPTWC_VDO_PARM_MIRROR_FLIP  = 0x30,

	UI_APPTWC_VDO_SENSOR_POWER_FREQ = 0x40,	

	UI_APPTWC_PROP_BEACON			= 0x7F,
	
	/* UI OP CODE start from 128th. The front 128 opcode reserve in EXT*/
	UI_APPTWC_VDO_IQ_BRIGHTNESS 	= 0x80,	
	UI_TWC_VDO_IQ_CHROMA,       			
	UI_TWC_VDO_IQ_CONTRACT,					
	UI_TWC_VDO_IQ_SATURATION,				

	UI_TWC_ADO_VOL_CTL	 			= 0x8C,
	UI_TWC_PUSHTALK_FUNC 			= 0x8D,
	UI_TWC_CHANGE_RF_MODE 			= 0x8E,
	UI_TWC_SET_PF_PASSWORD 			= 0x90,
	UI_TWC_PWIFI_PAIRING_MODE 		= 0x91,
	
	UI_APPTWC_MAX_NUM,
} CMD_Type;

typedef enum {
	APP_OTA_UPDT_SUCCESS = 0,
	APP_OTA_UPDT_SF_TRAN_OK,
	APP_OTA_UPDT_HDR_TAG_OK,
	APP_OTA_UPDT_CRC_OK,
	APP_OTA_UPDT_BURN_SF,
	
} E_APP_OTA_UPDT_RSLT;

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1) /* set alignment to 1 byte boundary */
//==========================
typedef struct
{
	uint8_t			PktSrcAddr;			// packet source address
	uint8_t			PktDestAddr;		// packet destination address
	uint8_t			PktIndex[2];		// packet index
	uint8_t			PktSize[2];			// packet size
	uint8_t			PktType;			// packet type
} PACKET_IDEX;

typedef struct
{
// 	uint8_t			Dummy1;
	uint8_t			TwcOpc;				// two way cmd op-code
	uint8_t			TwcData[4];			// two way cmd data field
} PKT_DP_Hdr, *pPKT_DP_Hdr;

typedef struct
{
	PACKET_IDEX     PacketIdex;
	PKT_DP_Hdr		DP_Hdr;
} DP;

typedef struct
{
	uint8_t			AdoSmpRate[2];		// audio sampling rate
	uint8_t			AdoTS[4];			// audio timestamp
	uint8_t			TwcOpc;				// two way cmd op-code
	uint8_t			TwcData[2];			// two way cmd data field
	uint8_t			DataPayload[4];		// Audio payload	
} PKT_ADOP_Hdr, *pPKT_ADOP_Hdr;

typedef struct
{
	PACKET_IDEX		PacketIdex;
	PKT_ADOP_Hdr	ADOP_Hdr;
} ADOP;
#pragma pack(pop)

//! Callback function definition.
typedef void(*pvUIApp_TwcProcess)(DP*, uint8_t *, uint8_t *);
typedef uint8_t(*pvUIApp_BTProcess)(uint8_t *, uint32_t);
typedef uint8_t(*pvUIApp_FWStatusProc)(uint8_t ubStatus, uint8_t *ubData);
typedef void(*pvUIApp_AdoProcess)(ADOP*);
typedef void(*pvDT_RstRc)(uint8_t);
typedef struct
{
	pvUIApp_TwcProcess		pvTwcProc;
	pvUIApp_AdoProcess		pvAdoProc;
	pvUIApp_BTProcess		pvBTProc;
	pvUIApp_FWStatusProc	pvFwStsProc;
	pvDT_RstRc				pvRcRstFunc;
}WiFiDt_CbFunc_t;
void WiFiDt_RegCbFunc(WiFiDt_CbFunc_t tCbFunc);
uint32_t ulWiFiDt_BufSetup(uint32_t ulBUF_StartAddr);
void WiFiDt_Open(void);
void WiFiDt_Close(void);
void WiFiDt_Setup(uint8_t ubFps, uint16_t uwVdo_H, uint16_t uwVdo_V);
uint16_t uwWiFiDt_GetBufUsedLvl(void);
uint16_t uwWiFiDt_GetTxVdoQues(uint8_t ubFreeCnt);
uint16_t uwWiFiDt_GetTxCmdQues(void);
void WiFiDt_SetDispFps(uint8_t ubFps);
uint8_t ubWiFiDt_GetDispFps(void);
uint8_t ubWiFiDt_GetRateLvl(void);
uint8_t ubWiFiDt_GetLinkStatus(void);
void WiFiDt_SetSsid(uint8_t ubMode, char *SSIDH, uint16_t uwSSIDHLen);
uint8_t ubWiFiDt_GetSsid(uint8_t *pSsid);
void WiFiDt_SetWepPassword(uint8_t ubWEPMode, uint8_t ubMode, uint8_t ubDPEN, char *chDF_Passwaord);
void WiFiDt_GetMacAddr(uint8_t *ubMACAddr);
void WiFiDt_SetDataRate(uint8_t ubRate);
void WiFiDt_SetPushTalkFunc(uint8_t ubEnable);
uint8_t ubWiFiDt_GetTransFrm(void);
uint8_t ubWiFiDt_GetRssi(uint8_t ubSta);
void WiFiDt_ResetVdoAdoQue(uint8_t ubTimeoutStop);
uint8_t ubWiFiDt_TwcProc(DP* pPtr, uint8_t *SrcIP, uint8_t *SrcMac, uint8_t ubFlag);
void WiFiDt_SendTwc(uint8_t ubOpCode, uint8_t ubRx_Id, uint8_t *DstIP, uint8_t *pbPtr, uint32_t ulDataLen, uint8_t ubCount);
uint8_t ubWiFiDt_GetVdoTransSts(void);
uint8_t ubWiFiDt_TxMQReq(uint32_t ulBufInAddr, uint32_t ulReqSize, uint32_t ubFrmType, uint8_t ubVdoGop, uint32_t ulTime);
void WiFiDt_SendAdoData(uint8_t ubRxId, uint8_t *pbPtr, uint32_t ulDataLen, uint32_t ulTime);
void WiFiDt_SetupStaEth(void);
void WiFiDt_EnStaEth(void);
uint16_t uwWiFiDt_GetVersion(void);
uint8_t ubWiFiDt_QueryAppConnectStatus(void);
void WiFiDt_UpdateAppConnectStatus(uint8_t ubStatus);

#define SUCCESS 		0
#define UNSUCCESS   	1
#define UNASSIGNED		2

#ifdef WIFI_TYPE_SDIO
#define WIFI_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX 	( 8 + 12 + 4 + 2 )
#else
#define WIFI_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX 	( 8 + 12 + 4 + 2 + 2 )
#endif
#define WIFI_LINK_TAIL_AFTER_ETHERNET_FRAME     		( 0 )
#define WIFI_ETHERNET_SIZE         						(14)
#define WIFI_PHYSICAL_HEADER       						(WIFI_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX + WIFI_ETHERNET_SIZE)
#define WIFI_PHYSICAL_TRAILER      						(WIFI_LINK_TAIL_AFTER_ETHERNET_FRAME)
#define WIFI_PAYLOAD_MTU           						(1500)
#define WIFI_LINK_MTU              						(WIFI_PAYLOAD_MTU + WIFI_PHYSICAL_HEADER + WIFI_PHYSICAL_TRAILER)

typedef enum
{
	WIFI_POWER_OFF,
	WIFI_POWER_ON
}WiFi_PowerCtrl_t;

typedef enum
{
	WIFI_MT7601,
	WIFI_AP6181
}WiFi_SupportType_t;

typedef enum
{
	WIFI_STA_INTERFACE     = 0, /**< STA Interface  */
    WIFI_AP_INTERFACE      = 1, /**< SoftAP Interface         */
}WIFI_Interface_t;

typedef enum
{
	RECV_DIRECT,
	RECV_MEMREALLOC,
}WIFI_RecvMemType_t;

typedef enum
{
    WIFI_SECURITY_OPEN           = 0,                 /**< Open security                           */
    WIFI_SECURITY_WPA_TKIP_PSK   = 3,                 /**< WPA Security with TKIP                  */
    WIFI_SECURITY_WPA_AES_PSK    = 4,                 /**< WPA Security with AES                   */
    WIFI_SECURITY_WPA2_AES_PSK   = 5,                 /**< WPA2 Security with AES                  */
    WIFI_SECURITY_WPA2_TKIP_PSK  = 6,                 /**< WPA2 Security with TKIP                 */
    WIFI_SECURITY_WPA2_MIXED_PSK = 7,  				  /**< WPA2 Security with AES & TKIP           */
}WIFI_Security_t;

typedef struct
{
	struct
	{
		uint8_t len;
		uint8_t val[32];
	}eSSID;
	WIFI_Security_t security;
	uint8_t security_key_length;
    char    security_key[64];
	uint8_t channel;
}WIFI_ConfigDct_t;

typedef void(*pvWiFi_PwrCtrl)(WiFi_PowerCtrl_t);
//------------------------------------------------------------------------
/*!
\brief Wi-Fi Function initialize
\param tWiFi_IF 	Wi-Fi interface
\param PwrCtrl_cb 	Wi-Fi dongle power control callback function
\par [Example]
\code    
	 WiFi_Init(WIFI_STA_INTERFACE, APP_WiFiPowerControl);
\return(no)
*/
void WiFi_Init(WIFI_Interface_t tWiFi_IF, pvWiFi_PwrCtrl PwrCtrl_cb);
//------------------------------------------------------------------------
/*!
\brief Wi-Fi dongle initialize
\return(no)
*/
void WiFi_Start(void);
//------------------------------------------------------------------------
/*!
\brief Wi-Fi Buffer Setup
\param ulBUF_StartAddr 	Avaliable DDR address
\param PwrCtrl_cb 	Wi-Fi dongle power control callback function
\par [Example]
\code
	ulWiFi_BufSetup(0xA0000);
\return Wi-Fi Buffer Size
*/
uint32_t ulWiFi_BufSetup(uint32_t ulBUF_StartAddr);
//------------------------------------------------------------------------
/*!
\brief Wi-Fi Config
\param tDct_Info 	Router information
\return(no)
*/
void WiFi_ConfigDct(WIFI_ConfigDct_t tDct_Info);
//------------------------------------------------------------------------
/*!
\brief Get interface of Wi-Fi dongle
\return Interface
*/
WIFI_Interface_t WiFi_GetInterface(void);
//------------------------------------------------------------------------
/*!
\brief Wi-Fi TX Function
\param data 		data
\param data_len		data length
\return result
*/
int WiFi_SendFunc(void *data, uint32_t data_len);
//------------------------------------------------------------------------
/*!
\brief Set callback function of Wi-Fi RX
\param recv_cb 		Callback function of RX
\return Function pointer
*/
typedef void(*pvWiFi_RX)(void *data, uint32_t data_len, void *type);
void WiFi_SetRecv_cbFunc(pvWiFi_RX recv_cb);
//------------------------------------------------------------------------
/*!
\brief Set callback function of Wi-Fi connected.(Link to Router)
\param connected_cb 		Callback function of connected
\return Function pointer
*/
typedef void(*pvWiFi_Connected)(void);
void WiFi_SetConnected_cbFunc(pvWiFi_Connected connected_cb);
//------------------------------------------------------------------------
/*!
\brief Set callback function of Wi-Fi disconnect
\param disconnect_cb 		Callback function of disconnect
\return Function pointer
*/
typedef void(*pvWiFi_DisConnect)(void);
void WiFi_SetDisConnect_cbFunc(pvWiFi_DisConnect disconnect_cb);
//------------------------------------------------------------------------
/*!
\brief Obtain the status of Wi-Fi dongle initialize
\return status
*/
uint32_t WiFi_GetInitStatus(void);
//------------------------------------------------------------------------
/*!
\brief Obtain the MAC address of Wi-Fi dongle
\param mac 		MAC address
\return result
*/
int WiFi_Get_MACAddr(void *mac);
//------------------------------------------------------------------------
/*!
\brief Obtain the Wi-Fi dongle type
\param mac 		MAC address
\return result
*/
uint8_t WiFi_GetWiFiDongleType(void);
//------------------------------------------------------------------------------
/*!
\brief 	Obtain Wi-Fi Version	
\return	Version
*/
uint16_t uwWiFi_GetVersion(void);
#endif
