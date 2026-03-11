#include "xil_printf.h"
#include "sleep.h"
#include "lv_conf.h"
#include "lvgl/lvgl.h"
#include "lvgl/my_gui.h"
#include "lcd/lcd_drv.h"
#include "lcd/sys_timer.h"

int main() {
    xil_printf("=== System Booting ===\r\n");

    // 1. 初始化底层硬件 (LCD, SPI, GPIO)
    if (LCD_Init_Hardware() != XST_SUCCESS) {
        xil_printf("LCD Init Failed!\r\n");
        return -1;
    }
    xil_printf("LCD Initialized.\r\n");

    // 2. 初始化定时器 (开启中断心跳)
    System_Timer_Init();

    // 3. 初始化 LVGL库
    lv_init();

    // 4. 注册显示驱动
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[320 * 20]; // 缓冲区 1/10 屏幕大小
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 320 * 20);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 172;
    disp_drv.flush_cb = my_disp_flush; // 关联 lcd_drv.c 里的函数
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 5. 创建用户界面 (进入业务逻辑)
    my_gui_init();
    my_gui_create_main_interface();
    xil_printf("GUI Started. Loop running...\r\n");

    // 6. 主循环
    while(1) {
        my_gui_task_handler(); // 更新业务数据
        lv_timer_handler();    // LVGL 渲染处理
        usleep(5000);          // 释放 CPU
    }

    return 0;
}
