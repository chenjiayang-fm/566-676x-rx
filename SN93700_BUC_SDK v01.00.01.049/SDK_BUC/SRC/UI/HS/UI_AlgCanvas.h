#ifndef UI_ALG_CANVAS_H
#define UI_ALG_CANVAS_H

#include <stdint.h>
#include "cmsis_os.h"

extern osMutexId osUI_AlgCanvasMutex;

// 首次预览初始化完成后调用，仅创建一个绘制线程，并注册 RF 结果通知。
void UI_AlgCanvasInit(void);
// 与整屏清除配套调用，仅使旧画布记录失效，本接口不擦屏。
// Caller holds osUI_AlgCanvasMutex through the following full-screen clear.
void UI_AlgCanvasReset(void);
// 本次连接版本校验后的算法使能状态，ubCam 为 CAM 编号，不是显示 slot。
void UI_AlgCanvasSetEnable(uint8_t ubCam, uint8_t ubEnable);
uint8_t UI_AlgCanvasGetEnable(uint8_t ubCam);

#endif
