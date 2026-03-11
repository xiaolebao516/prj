#include "lcd_drv.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "xspips.h"
#include "sleep.h"

// ================= 硬件定义 =================
#define LCD_DC_PIN  54
#define LCD_RST_PIN 55
#define LCD_LED_PIN 56
#define SPI_DEVICE_ID   XPAR_XSPIPS_0_DEVICE_ID

static XGpioPs Gpio;
static XSpiPs SpiInstance;

// ================= 内部宏与辅助函数 =================
#define LCD_DC_CMD  XGpioPs_WritePin(&Gpio, LCD_DC_PIN, 0)
#define LCD_DC_DAT  XGpioPs_WritePin(&Gpio, LCD_DC_PIN, 1)
#define LCD_RST_0   XGpioPs_WritePin(&Gpio, LCD_RST_PIN, 0)
#define LCD_RST_1   XGpioPs_WritePin(&Gpio, LCD_RST_PIN, 1)
#define LCD_LED_ON  XGpioPs_WritePin(&Gpio, LCD_LED_PIN, 1)

static void LCD_WR_REG(u8 dat) {
    LCD_DC_CMD;
    XSpiPs_PolledTransfer(&SpiInstance, &dat, NULL, 1);
}

static void LCD_WR_DATA8(u8 dat) {
    LCD_DC_DAT;
    XSpiPs_PolledTransfer(&SpiInstance, &dat, NULL, 1);
}

static void LCD_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2) {
    // 偏移量根据你的调试结果设定
    u16 x_offset = 0;
    u16 y_offset = 34;

    LCD_WR_REG(0x2A);
    u8 data_x[4] = {(x1 + x_offset) >> 8, (x1 + x_offset) & 0xFF, (x2 + x_offset) >> 8, (x2 + x_offset) & 0xFF};
    LCD_DC_DAT; XSpiPs_PolledTransfer(&SpiInstance, data_x, NULL, 4);

    LCD_WR_REG(0x2B);
    u8 data_y[4] = {(y1 + y_offset) >> 8, (y1 + y_offset) & 0xFF, (y2 + y_offset) >> 8, (y2 + y_offset) & 0xFF};
    LCD_DC_DAT; XSpiPs_PolledTransfer(&SpiInstance, data_y, NULL, 4);

    LCD_WR_REG(0x2C);
}

// ================= 公开接口实现 =================

int LCD_Init_Hardware(void) {
    // 1. GPIO Init
    XGpioPs_Config *GpioConfig = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&Gpio, GpioConfig, GpioConfig->BaseAddr);
    XGpioPs_SetDirectionPin(&Gpio, LCD_DC_PIN, 1); XGpioPs_SetOutputEnablePin(&Gpio, LCD_DC_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, LCD_RST_PIN, 1); XGpioPs_SetOutputEnablePin(&Gpio, LCD_RST_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, LCD_LED_PIN, 1); XGpioPs_SetOutputEnablePin(&Gpio, LCD_LED_PIN, 1);

    // 2. SPI Init
    XSpiPs_Config *SpiConfig = XSpiPs_LookupConfig(SPI_DEVICE_ID);
    XSpiPs_CfgInitialize(&SpiInstance, SpiConfig, SpiConfig->BaseAddress);
    XSpiPs_SetOptions(&SpiInstance, XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION);
    // 这里保留高性能配置 Prescaler 4
    XSpiPs_SetClkPrescaler(&SpiInstance, XSPIPS_CLK_PRESCALE_4);
    XSpiPs_SetSlaveSelect(&SpiInstance, 0x00);

    // 3. LCD Init Sequence
    LCD_RST_1; usleep(10000); LCD_RST_0; usleep(20000); LCD_RST_1; usleep(120000);
	LCD_WR_REG(0x11); usleep(120000);

	// 关键：模拟器是横屏 (320x172)，所以必须设置旋转
	// 0x70 = MV(交换XY)|MX|MY
	LCD_WR_REG(0x36); LCD_WR_DATA8(0x70);
	LCD_WR_REG(0x3A); LCD_WR_DATA8(0x05);
	LCD_WR_REG(0xB2); LCD_WR_DATA8(0x0C); LCD_WR_DATA8(0x0C); LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x33); LCD_WR_DATA8(0x33);
	LCD_WR_REG(0xB7); LCD_WR_DATA8(0x35);
	LCD_WR_REG(0xBB); LCD_WR_DATA8(0x19);
	LCD_WR_REG(0xC0); LCD_WR_DATA8(0x2C);
	LCD_WR_REG(0xC2); LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xC3); LCD_WR_DATA8(0x12);
	LCD_WR_REG(0xC4); LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xC6); LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0xD0); LCD_WR_DATA8(0xA4); LCD_WR_DATA8(0xA1);
	LCD_WR_REG(0x21); LCD_WR_REG(0x29); LCD_LED_ON;

    return XST_SUCCESS;
}

// LVGL 调用的刷新函数
void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p) {
    LCD_SetRegion(area->x1, area->y1, area->x2, area->y2);
    int32_t len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
    LCD_DC_DAT;
    XSpiPs_PolledTransfer(&SpiInstance, (u8*)color_p, NULL, len);
    lv_disp_flush_ready(disp);
}
