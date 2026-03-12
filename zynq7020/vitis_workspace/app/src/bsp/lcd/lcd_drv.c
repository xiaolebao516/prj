#include "lcd_drv.h"
#include "xparameters.h"
#include "sleep.h"

#define LCD_DC_PIN      54
#define LCD_RST_PIN     55
#define LCD_LED_PIN     56
#define SPI_DEVICE_ID   XPAR_XSPIPS_0_DEVICE_ID

XGpioPs Gpio;
XSpiPs SpiInstance;

#define LCD_DC_CMD  XGpioPs_WritePin(&Gpio, LCD_DC_PIN, 0)
#define LCD_DC_DAT  XGpioPs_WritePin(&Gpio, LCD_DC_PIN, 1)
#define LCD_RST_0   XGpioPs_WritePin(&Gpio, LCD_RST_PIN, 0)
#define LCD_RST_1   XGpioPs_WritePin(&Gpio, LCD_RST_PIN, 1)
#define LCD_LED_ON  XGpioPs_WritePin(&Gpio, LCD_LED_PIN, 1)

void LCD_WR_REG(u8 dat) {
    LCD_DC_CMD;
    XSpiPs_PolledTransfer(&SpiInstance, &dat, NULL, 1);
}

void LCD_WR_DATA8(u8 dat) {
    LCD_DC_DAT;
    XSpiPs_PolledTransfer(&SpiInstance, &dat, NULL, 1);
}

void LCD_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2) {
    u16 x_offset = 0;
    u16 y_offset = 34; // 如果全覆盖刷屏后依然不亮，尝试改为 0

    LCD_WR_REG(0x2A); // Column Address Set
    u8 data_x[4] = {(x1 + x_offset) >> 8, (x1 + x_offset) & 0xFF, (x2 + x_offset) >> 8, (x2 + x_offset) & 0xFF};
    LCD_DC_DAT; XSpiPs_PolledTransfer(&SpiInstance, data_x, NULL, 4);

    LCD_WR_REG(0x2B); // Row Address Set
    u8 data_y[4] = {(y1 + y_offset) >> 8, (y1 + y_offset) & 0xFF, (y2 + y_offset) >> 8, (y2 + y_offset) & 0xFF};
    LCD_DC_DAT; XSpiPs_PolledTransfer(&SpiInstance, data_y, NULL, 4);

    LCD_WR_REG(0x2C); // Memory Write
}

int LCD_Init_Hardware(void) {
    // 1. GPIO 初始化
    XGpioPs_Config *GpioConfig = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&Gpio, GpioConfig, GpioConfig->BaseAddr);
    XGpioPs_SetDirectionPin(&Gpio, LCD_DC_PIN, 1); XGpioPs_SetOutputEnablePin(&Gpio, LCD_DC_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, LCD_RST_PIN, 1); XGpioPs_SetOutputEnablePin(&Gpio, LCD_RST_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, LCD_LED_PIN, 1); XGpioPs_SetOutputEnablePin(&Gpio, LCD_LED_PIN, 1);

    // 2. SPI 初始化 (改为 Mode 3: CPOL=1, CPHA=1)
    XSpiPs_Config *SpiConfig = XSpiPs_LookupConfig(SPI_DEVICE_ID);
    XSpiPs_CfgInitialize(&SpiInstance, SpiConfig, SpiConfig->BaseAddress);

    // 【修改点】增加时钟极性和相位配置
    XSpiPs_SetOptions(&SpiInstance, XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION |
                                   XSPIPS_CLK_ACTIVE_LOW_OPTION | XSPIPS_CLK_PHASE_1_OPTION);

    XSpiPs_SetClkPrescaler(&SpiInstance, XSPIPS_CLK_PRESCALE_16); // 降低频率到 ~10MHz 确保稳定
    XSpiPs_SetSlaveSelect(&SpiInstance, 0x00); // 确保选中 CS0

    // 3. LCD 初始化序列
    LCD_RST_1; usleep(10000); LCD_RST_0; usleep(20000); LCD_RST_1; usleep(120000);
    LCD_WR_REG(0x11); // Sleep Out
    usleep(120000);

    LCD_WR_REG(0x36); LCD_WR_DATA8(0x70); // MADCTL
    LCD_WR_REG(0x3A); LCD_WR_DATA8(0x05); // 16-bit color

    // 针对这类国产屏常用的配置
    LCD_WR_REG(0x21); // Display Inversion ON (如果颜色反转了请注释掉这一行)

    LCD_WR_REG(0xB2); LCD_WR_DATA8(0x0C); LCD_WR_DATA8(0x0C); LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x33); LCD_WR_DATA8(0x33);
    LCD_WR_REG(0xB7); LCD_WR_DATA8(0x35);
    LCD_WR_REG(0xBB); LCD_WR_DATA8(0x19);
    LCD_WR_REG(0xC0); LCD_WR_DATA8(0x2C);
    LCD_WR_REG(0xC2); LCD_WR_DATA8(0x01);
    LCD_WR_REG(0xC3); LCD_WR_DATA8(0x12);
    LCD_WR_REG(0xC4); LCD_WR_DATA8(0x20);
    LCD_WR_REG(0xC6); LCD_WR_DATA8(0x0F);
    LCD_WR_REG(0xD0); LCD_WR_DATA8(0xA4); LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0x29); // Display ON
    LCD_LED_ON;       // 点亮背光

    return XST_SUCCESS;
}

void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p) {
    LCD_SetRegion(area->x1, area->y1, area->x2, area->y2);
    uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
    LCD_DC_DAT;
    XSpiPs_PolledTransfer(&SpiInstance, (u8*)color_p, NULL, size);
    lv_disp_flush_ready(disp);
}
