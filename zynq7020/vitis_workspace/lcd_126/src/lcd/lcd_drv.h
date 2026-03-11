// 此模块封装了所有和屏幕硬件相关的脏活累活
#ifndef LCD_DRV_H
#define LCD_DRV_H

#include "xil_types.h"
#include "../lvgl/lvgl.h" // 需要用到 lv_disp_drv_t 等类型

// 初始化 LCD 及其底层的 SPI/GPIO
int LCD_Init_Hardware(void);

// 提供给 LVGL 的刷新回调函数
void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p);

#endif
