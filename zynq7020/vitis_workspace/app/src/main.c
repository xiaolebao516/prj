// main.c
#include "app_main.h"
#include "app_monitor.h"
#include "xil_printf.h"

int main() {
    xil_printf("\r\n=== System Start: Professional Modular Architecture ===\r\n");

    /* 系统装配 */
    Hardware_Init();
    Network_Init();
    UI_Init();
    App_Monitor_Init();

    /* 核心调度循环 */
    while (1) {
        Network_Task();
        LVGL_Task();
        App_Monitor_Tick();
    }
    return 0;
}
