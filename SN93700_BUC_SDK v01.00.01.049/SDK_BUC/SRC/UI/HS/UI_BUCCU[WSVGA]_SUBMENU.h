#ifndef _UI_BUCCU_SUBMENU_H_
#define _UI_BUCCU_SUBMENU_H_
#include "UI_BUCCU[WSVGA].h"
#include "KNL.h"
#include "UI_BUCCU[WSVGA]_MENU.h"


void UI_ListRecFileInfo(uint16_t uwStartFileIdx, uint16_t uwEndFileIdx, OSD_UPDATE_TYP tUpdateMode);


void UI_AutoDrawOnOffButton(bool isButtonHL, uint8_t OnOffButtonHLIndex);

#endif

