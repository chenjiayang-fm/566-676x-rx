/*!
	The information contained herein is the exclusive property of SONiX and
	shall not be distributed, or disclosed in whole or in part without prior
	permission of SONiX.
	SONiX reserves the right to make changes without further notice to the
	product to improve reliability, function or design. SONiX does not assume
	any liability arising out of the application or use of any product or
	circuits described herein. All application information is advisor and does
	not from part of the specification.

	\file		OSD1Image_Table.h
	\brief		OSD1 Image Table
	\author		Hanyi Chiu
	\version	1
	\date		2016/09/20
	\copyright	Copyright (C) 2016 SONiX Technology Co., Ltd. All rights reserved.
*/
//------------------------------------------------------------------------------

//!           Index 	  	  		  SF Address     Pos X     Pos Y
OSD1IMGPOOL(OSD1IMAGE,        			0,        	   0,        0)
OSD1IMGPOOL(SUBMENU2,					0x2000, 	   0,		 0)
OSD1IMGPOOL(FWUSTARTBG, 				0x2000,        0,      620)
OSD1IMGPOOL(POWER_DISCONNECT, 				0x2000, 	   0,	   620)
OSD1IMGPOOL(POWER_CONNECT, 				0x2000, 	   0,	   620)
OSD1IMGPOOL(RESET_LOGO,				0x2000, 	   0,	   620)

#undef OSD1IMGPOOL
