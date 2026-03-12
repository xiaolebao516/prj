#ifndef LCD_DRV_H
#define LCD_DRV_H

#include "xil_types.h"
#include "xgpiops.h"
#include "xspips.h"
#include "lvgl/lvgl.h"

// 声明公开接口
int LCD_Init_Hardware(void);
void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p);
void LCD_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_WR_REG(u8 dat);
void LCD_WR_DATA8(u8 dat);

// 声明全局变量
extern XGpioPs Gpio;
extern XSpiPs SpiInstance;

#endif
