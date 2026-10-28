/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		BSP_Board.c
	\brief		BUC CU/CAM Demo Board
	\author		Hanyi Chiu
	\version	0.9
	\date		2020/04/16
	\copyright	Copyright(C) 2020 SONiX Technology Co., Ltd. All rights reserved.
*/
//------------------------------------------------------------------------------
#if defined BUC_CU || defined BUC_CAM

#include "BSP.h"
#include "_510PF.h"
#include "APP_CFG.h"
#ifdef BSP_SN93711_FHD_REC_RX_V4
void BSP_BoardInit(void)
{
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! LCD
	//! GPIO SPI
	GLB->PADIO47 = 1;
	GLB->PADIO48 = 1;
	GLB->PADIO49 = 1;
	GLB->PADIO50 = 1;
	//! LCD Pin
	GLB->PADIO26 = 5;
	GLB->PADIO27 = 5;
	GLB->PADIO28 = 5;
	GLB->PADIO29 = 5;
	GLB->PADIO30 = 5;
	GLB->PADIO31 = 5;
	GLB->PADIO32 = 5;
	GLB->PADIO33 = 5;
	GLB->PADIO34 = 5;
	GLB->PADIO35 = 5;
	GLB->PADIO36 = 5;

	//! RF SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;
	GLB->PADIO15 = 0;
	GLB->PADIO21 = 0;

	//! SD
	GLB->PADIO0  = 4;
	GLB->PADIO1  = 4;
	GLB->PADIO2  = 4;
	GLB->PADIO3  = 4;
	GLB->PADIO4  = 4;
	GLB->PADIO5  = 4;
	GLB->PADIO6  = 4;
	GLB->PADIO7  = 4;

	//! Speaker
	GLB->PADIO54 = 0;

	//! LED
	GLB->PADIO16 = 0;
	GLB->PADIO17 = 0;
	GLB->PADIO55 = 7;
	GLB->PADIO56 = 7;
	GLB->PADIO57 = 7;

	//! BL
	GLB->PADIO51 = 7;

	//! LCD Power
	GLB->PADIO52 = 0;
	//! LCD Reset
	GLB->PADIO11 = 7;
}
#endif
//------------------------------------------------------------------------------
#ifdef BSP_D_SN93701_SSD2828_RX_V5
void BSP_BoardInit(void)
{
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! LCD
	//! GPIO SPI
	GLB->PADIO47 = 1;
	GLB->PADIO48 = 1;
	GLB->PADIO49 = 1;
	GLB->PADIO50 = 1;
	//! LCD Pin
	GLB->PADIO26 = 5;
	GLB->PADIO27 = 5;
	GLB->PADIO28 = 5;
	GLB->PADIO29 = 5;
	GLB->PADIO30 = 5;
	GLB->PADIO31 = 5;
	GLB->PADIO32 = 5;
	GLB->PADIO33 = 5;
	GLB->PADIO34 = 5;
	GLB->PADIO35 = 5;
	GLB->PADIO36 = 5;

	//! RF SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;
	GLB->PADIO15 = 0;
	GLB->PADIO21 = 0;

	//! SD
	GLB->PADIO0  = 4;
	GLB->PADIO1  = 4;
	GLB->PADIO2  = 4;
	GLB->PADIO3  = 4;
	GLB->PADIO4  = 4;
	GLB->PADIO5  = 4;
	GLB->PADIO6  = 4;
	GLB->PADIO7  = 4;

	//! Speaker
	GLB->PADIO54 = 0;

	//! LED
	GLB->PADIO16 = 0;
	GLB->PADIO17 = 0;
	GLB->PADIO55 = 7;
	GLB->PADIO56 = 7;
	GLB->PADIO57 = 7;

	//! BL
	GLB->PADIO51 = 7;

	//! LCD Power
	GLB->PADIO52 = 0;
	//! LCD Reset
	GLB->PADIO11 = 7;
	
	//! UVC1/2 Power Control
#if defined(POWER_UVC1_IO)
	//GLB->PADIO14 = 0;	//GPIO0	//Old Board for Center 
	GLB->PADIO6 = 0;	//GPIO6	//New Board for Center
#endif
#if defined(POWER_UVC2_IO)
	//GLB->PADIO13 = 0;	//GPIO13
	GLB->PADIO6 = 0;	//GPIO6	//New Board for Center
#endif
}
#endif
//------------------------------------------------------------------------------
#ifdef BSP_D_SNCC71_GM8285C_RX_V2
void BSP_BoardInit(void)
{
#if 0
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! LCD
	//! GPIO SPI
	GLB->PADIO47 = 1;
	GLB->PADIO48 = 1;
	GLB->PADIO49 = 1;
	GLB->PADIO50 = 1;
	//! LCD Pin
	GLB->PADIO26 = 5;
	GLB->PADIO27 = 5;
	GLB->PADIO28 = 5;
	GLB->PADIO29 = 5;
	GLB->PADIO30 = 5;
	GLB->PADIO31 = 5;
	GLB->PADIO32 = 5;
	GLB->PADIO33 = 5;
	GLB->PADIO34 = 5;
	GLB->PADIO35 = 5;
	GLB->PADIO36 = 5;

	//! RF SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;
	GLB->PADIO15 = 0;
	GLB->PADIO21 = 0;

	//! SD
	GLB->PADIO0  = 4;
	GLB->PADIO1  = 4;
	GLB->PADIO2  = 4;
	GLB->PADIO3  = 4;
	GLB->PADIO4  = 4;
	GLB->PADIO5  = 4;
	GLB->PADIO6  = 4;
	GLB->PADIO7  = 4;

	//! Speaker
	GLB->PADIO54 = 0;

	//! LED
	GLB->PADIO16 = 0;
	GLB->PADIO17 = 0;
	GLB->PADIO55 = 7;
	GLB->PADIO56 = 7;
	GLB->PADIO57 = 7;

	//! BL
	GLB->PADIO51 = 7;
	GLB->PADIO53 = 7;

	//! RF Power Reset
	GLB->PADIO11 = 0;

	//! LCD Power
	GLB->PADIO52 = 7;
	//! LCD Reset
	GLB->PADIO25 = 7;

#if (BSP_RTC_TIMER_SEL == RTC_TIMER_EXTERNAL)
	//! I2C2 for RTC PCF85063A
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;
	//! RTC PCF85063A trigger
	GLB->PADIO10 = 7;	//! change to mode-PWM10
	GLB->PADIO24 = 0;	//! GPIO10
#endif
#endif
//! LCD
	//! GPIO SPI
	GLB->PADIO47 = 1;
	GLB->PADIO48 = 1;
	GLB->PADIO49 = 1;
	GLB->PADIO50 = 1;
	//! LCD Pin
	GLB->PADIO26 = 5;
	GLB->PADIO27 = 5;
	GLB->PADIO28 = 5;
	GLB->PADIO29 = 5;
	GLB->PADIO30 = 5;
	GLB->PADIO31 = 5;
	GLB->PADIO32 = 5;
	GLB->PADIO33 = 5;
	GLB->PADIO34 = 5;
	GLB->PADIO35 = 5;
	GLB->PADIO36 = 5;

	//! RF SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;
	
	GLB->PADIO17 = 0;//20201216 rf rst
	
	GLB->PADIO21 = 0;

	//! SD
	GLB->PADIO0  = 4;
	GLB->PADIO1  = 4;
	GLB->PADIO2  = 4;
	GLB->PADIO3  = 4;
	GLB->PADIO4  = 4;
	GLB->PADIO5  = 4;
	GLB->PADIO6  = 4;
	GLB->PADIO7  = 4;


#if (BSP_RTC_TIMER_SEL == RTC_TIMER_EXTERNAL)
	//! I2C2 for RTC PCF85063A
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;
	//! RTC PCF85063A trigger
	GLB->PADIO10 = 7;	//! change to mode-PWM10
	GLB->PADIO24 = 0;	//! GPIO10
#endif


//--------------------------------------my board--------------------------------------------------------------
	//20201216 uart1
	GLB->PADIO15 = 1;
	GLB->PADIO51 = 2;

	//20201216 配置加密芯片IIC，以下引脚需要避开GPIO0 GPIO13
	#if 1
	#if(BSP_RTC_TIMER_SEL != RTC_TIMER_EXTERNAL)
		GLB->PADIO13 = 2;
		GLB->PADIO14 = 2;
	#endif
		GLB->PADIO41 = 2;
		GLB->PADIO42 = 2;
	#endif
	
	//20201308 screen
		//! LCD Power
	GLB->PADIO48 = 7;
		//! LCD Reset
	GLB->PADIO22 = 7;    
		//! BL
	GLB->PADIO24 = 7; 

	//20201217 rf power
	GLB->PADIO23 = 7; 


	//20201218  io55 io56会用在触摸和加密芯片
	GLB->PADIO55 = 5;
	GLB->PADIO56 = 5;	

	//20210308 audio_pwm
	GLB->PADIO25 = 5;

	//掉电检测引脚
	GLB->PADIO47 = 0;

	//RF433
	GLB->PADIO16 = 0; //reset
	GLB->PADIO49 = 2; //uart
	GLB->PADIO50 = 2;

	//buzzer
    GLB->PADIO52 = 7; //PWM4
}
#endif
//------------------------------------------------------------------------------
#ifdef BSP_D_SNCC71_TP2915_AHD_RX_V3
void BSP_BoardInit(void)
{
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! LCD
	//! GPIO SPI
	//GLB->PADIO47 = 0;
	//GLB->PADIO48 = 1;
	//GLB->PADIO49 = 1;
	//GLB->PADIO50 = 1;
	//! LCD Pin
	GLB->PADIO26 = 5;
	GLB->PADIO27 = 5;
	GLB->PADIO28 = 5;
	GLB->PADIO29 = 5;
	GLB->PADIO30 = 5;
	GLB->PADIO31 = 5;
	GLB->PADIO32 = 5;
	GLB->PADIO33 = 5;
	GLB->PADIO34 = 5;
	GLB->PADIO35 = 5;
	GLB->PADIO36 = 5;

	//! RF SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;
	GLB->PADIO15 = 0;
	GLB->PADIO21 = 0;

	//! SD
	GLB->PADIO0  = 4;
	GLB->PADIO1  = 4;
	GLB->PADIO2  = 4;
	GLB->PADIO3  = 4;
	GLB->PADIO4  = 4;
	GLB->PADIO5  = 4;
	GLB->PADIO6  = 4;
	GLB->PADIO7  = 4;

	//! Speaker
	GLB->PADIO54 = 0;

	//! LED
	GLB->PADIO16 = 0;
	GLB->PADIO48 = 7;
	GLB->PADIO49 = 7;
	GLB->PADIO50 = 0;
	GLB->PADIO51 = 7;
	//! NTSC
	GLB->PADIO10 = 7;
	GLB->PADIO24 = 0;
	GLB->PADIO38 = 7;
	GLB->PADIO66 = 7;

	//! BL
	//GLB->PADIO51 = 7;
	GLB->PADIO53 = 7;

	//! RF Power Reset
	GLB->PADIO11 = 0;

	//! LCD Power
	GLB->PADIO52 = 7;
	//! LCD Reset
	GLB->PADIO17 = 0;
	
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;

#if (BSP_RTC_TIMER_SEL == RTC_TIMER_EXTERNAL)
	//! I2C2 for RTC PCF85063A
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;
	//! RTC PCF85063A trigger
	GLB->PADIO10 = 7;	//! change to mode-PWM10
	GLB->PADIO24 = 0;	//! GPIO10
#endif
}
#endif
//------------------------------------------------------------------------------
#if (defined(BSP_SNCC70_AHD_TP9950_TX_V1) || defined(BSP_SNCC70_AHD_RN6752M_TX_V1))
void BSP_BoardInit(void)
{
	//! SD
	GLB->PADIO0 = 4;
	GLB->PADIO1 = 4;
	GLB->PADIO2 = 4;
	GLB->PADIO3 = 4;
	GLB->PADIO4 = 4;
	GLB->PADIO5 = 4;
//	GLB->PADIO6 = 4;
	GLB->PADIO7 = 4;

//RN6752 REST    
	GLB->PADIO6 = 0;	
	
	//! I2C-2
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;
	
	//! RF_SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;
//	GLB->PADIO28 = 0;
    GLB->PADIO9 = 0;
	GLB->PADIO27 = 0;
#ifdef RTC676x
	//! Richwave RF PWR CTRL
	//GLB->PADIO12 = 1;
	GLB->PADIO26 = 0;
#endif	
	
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;
	
	//! LED
	GLB->PADIO29 = 0;
	GLB->PADIO30 = 0;
	
	//! Auto Pair Pin (GPIO16)
	GLB->JTAG_EN = 0;
}
#endif
//------------------------------------------------------------------------------
#if (defined(BSP_D_SNCC70_TX_V2) || defined(BSP_SN93710_FHD_REC_TX_V4))
void BSP_BoardInit(void)
{
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! RF_SPI
	GLB->PADIO18 = 1;
	GLB->PADIO19 = 1;
	GLB->PADIO20 = 1;

//	GLB->PADIO28 = 0;
#if (APP_DOORPHONE_ENABLE==0)
    GLB->PADIO9 = 0;
#endif
#if APP_MAC_FUNC_ENABLE
	GLB->PADIO27 = 4;	//MAX_TXD[0]
	GLB->PADIO28 = 4;	//MAX_TXD[1]
	
	GLB->PADIO38 = 0;	//GIO2(AMIC)	
	GLB->PADIO39 = 0;	//CKO(AMIC)
#else
	GLB->PADIO27 = 0;
#endif

	//! SD
	GLB->PADIO0 = 4;
	GLB->PADIO1 = 4;
	GLB->PADIO2 = 4;
	GLB->PADIO3 = 4;
	GLB->PADIO4 = 4;
	GLB->PADIO5 = 4;
	GLB->PADIO6 = 4;
	GLB->PADIO7 = 4;

	//! I2C-2
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;

    //RN6752 REST
    GLB->PADIO6 = 0;

	//! Speaker
#if APP_MAC_FUNC_ENABLE
	GLB->PADIO32 = 4;	//MAX_RXD[1]
	
	GLB->PADIO43 = 0;	//SPK_EN
#else
	GLB->PADIO32 = 0;
#endif

	//! LED
#if APP_MAC_FUNC_ENABLE
	GLB->PADIO29 = 4;	//MAX_RX_DV
	GLB->PADIO30 = 4;	//MAX_RX_ER
	GLB->PADIO31 = 4;	//MAX_RXD[0]
	
	GLB->PADIO40 = 0;	//LED
	GLB->PADIO41 = 0;	//LED
	GLB->PADIO42 = 0;	//LED
#else
	GLB->PADIO29 = 0;
	GLB->PADIO30 = 0;
	GLB->PADIO31 = 0;
#endif

#ifdef S2019A
	//! Power ctrl
	GLB->PADIO12 = 1;
	GLB->PADIO26 = 0;
#endif
#ifdef RTC676x
	//! Richwave RF PWR CTRL
	//GLB->PADIO12 = 1;
	GLB->PADIO26 = 0;
#endif

#if !APP_MAC_FUNC_ENABLE
#if ( (BSP_ADO_I2S_MODE_ENABLE == 1) | (APP_ADO_AEC_NR_TYPE == AEC_NR_HW) )
	//! Tx I2S
#if (APP_DOORPHONE_ENABLE==0)
	GLB->PADIO37 = 6;
	GLB->PADIO38 = 6;
#endif
	GLB->PADIO39 = 6;
	GLB->PADIO40 = 6;
	GLB->PADIO41 = 6;
#endif
#endif
#if (APP_DOORPHONE_ENABLE==1)
    GLB->PADIO9 = 7;
    GLB->PADIO10 = 7;
    GLB->PADIO24 = 7;

    GLB->PADIO8  = 0;   // Pairing to RX2
	GLB->PADIO37 = 0;   // Calling Key1
	GLB->PADIO38 = 0;   // Calling Key2
#endif

#if APP_ADO_AEC_NR_TYPE == AEC_NR_HW
	//! I2C-1
	GLB->PADIO6  = 5;	//i2c_0 clk
	GLB->PADIO46 = 1;	//i2c_0 sda
#endif

#if APP_MAC_FUNC_ENABLE
	GLB->PADIO26 = 4;	//MAX_TXEN
	GLB->PADIO37 = 0;	//WIFI_PWREN
	
	GLB->PADIO33 = 4;	//MAX_MDC
	GLB->PADIO34 = 4;	//MAX_MDIO
	GLB->PADIO35 = 4;	//MAX_PHY_LINKSTS
	GLB->PADIO36 = 4;	//MAX_PDN_PHY
#endif
}
#endif
//------------------------------------------------------------------------------
#if (defined(BSP_SNCC72_AHD_RN6752M_TX_V1)||defined(BSP_SNCC72_AHD_TP9950_TX_V2))
void BSP_BoardInit(void)
{
#ifdef RTC676x
	//! RF_SPI(Richwave RTC676x RF)	
	GLB->PADIO4 = 6;	//SPI_MO
	GLB->PADIO5 = 6;	//SPI_MI
	GLB->PADIO6 = 6;	//SPI_CLK
   
	GLB->PADIO42 = 0;	//RSTN, GPIO 0	
	GLB->PADIO41 = 0;	//IRQN, GPIO 13 (Input)	
	//==============================
	//Set other mode for "GPI 13"	
	GLB->PADIO27 = 7;	
	//==============================	
#endif

#ifdef A7130
	//! A7130	
	GLB->PADIO4 = 3;
	GLB->PADIO6 = 3;
	GLB->PADIO7 = 3;
	GLB->PADIO5 = 0;
	GLB->PADIO41 = 0;
	GLB->PADIO27 = 7;
#endif	

	//! I2C-2
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;

    //RN6752 REST
    GLB->PADIO15 = 0;
	
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! RF PWR CTRL
	//! TMS GPIO18
	GLB->JTAG_EN = 0;
	
	//! KEY TDI GPIO19
	//! LCD TD0 GPIO20
}
#endif
//------------------------------------------------------------------------------
#ifdef BSP_D_SNCC72_TX_V1
void BSP_BoardInit(void)
{
#ifdef RTC676x
	//! RF_SPI(Richwave RTC676x RF)	
	GLB->PADIO4 = 6;	//SPI_MO
	GLB->PADIO5 = 6;	//SPI_MI
	GLB->PADIO6 = 6;	//SPI_CLK
   
	GLB->PADIO42 = 0;	//RSTN, GPIO 0	
	GLB->PADIO41 = 0;	//IRQN, GPIO 13 (Input)	
	//==============================
	//Set other mode for "GPI 13"	
	GLB->PADIO27 = 7;	
	//==============================	
#endif	

#ifdef A7130
	//! A7130	
	GLB->PADIO4 = 3;
	GLB->PADIO6 = 3;
	GLB->PADIO7 = 3;
	GLB->PADIO5 = 0;
	GLB->PADIO41 = 0;
	GLB->PADIO27 = 7;
#endif	

	//! I2C-2
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;

    //RN6752 REST
    GLB->PADIO15 = 0;
	
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! RF PWR CTRL
	//! TMS GPIO18
	GLB->JTAG_EN = 0;
	
	//! KEY TDI GPIO19
	//! LCD TD0 GPIO20
}
#endif
//------------------------------------------------------------------------------
#ifdef BSP_D_SN93712_VBM_TX_V2
void BSP_BoardInit(void)
{
	//! UART
	GLB->PADIO22 = 2;
	GLB->PADIO23 = 2;

	//! RF Power
	GLB->PADIO4  = 0;
	//! RF SPI IO
	GLB->PADIO39 = 2;
	GLB->PADIO42 = 2;
	GLB->PADIO41 = 2;
	//! RF GKO IO
	GLB->PADIO12 = 4;
	GLB->PADIO26 = 2;
	GLB->PADIO40 = 0;
	//! RF TXSW IO
	GLB->PADIO5  = 0;

	//! Reset Sensor
	GLB->PADIO1  = 4;
	GLB->PADIO15 = 0;

	//! I2C-2
	GLB->PADIO13 = 4;
	GLB->PADIO14 = 4;

	//! Pairing Key
	GLB->PADIO6  = 0;
#if (APP_DOORPHONE_ENABLE==1)
    GLB->PADIO8  = 0;
#endif
	//! LED
	GLB->JTAG_EN = 0;
}
#endif
//------------------------------------------------------------------------------

#endif //! End #if defined BUC_CU || defined BUC_CAM
