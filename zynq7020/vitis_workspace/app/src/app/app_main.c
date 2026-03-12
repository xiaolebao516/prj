// app/app_main.c
#include "app_main.h"

/* --- 硬件与中间件头文件 --- */
#include "platform.h"
#include "platform_config.h"
#include "lcd_drv.h"
#include "sys_timer.h"
#include "zynq_interrupt.h"
#include "adc_dma.h"
#include "ui_main.h"
#include "lv_conf.h"
#include "lvgl.h"

/* --- LwIP 网络头文件 --- */
#include "netif/xadapter.h"
#include "lwip/init.h"
#include "lwip/udp.h"
#include "xil_exception.h"

/* --- 宏定义 (从原 main.c 搬运) --- */
#define SRC_IP_ADDR    "192.168.1.10"
#define SRC_GW         "192.168.1.1"
#define SRC_NETMASK    "255.255.255.0"
#define INT_DEVICE_ID  XPAR_SCUGIC_SINGLE_DEVICE_ID
#ifdef XPAR_XEMACPS_0_BASEADDR
    #define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#else
    #define PLATFORM_EMAC_BASEADDR 0xE000B000
#endif

/* --- 全局硬件与网络句柄 --- */
XScuGic XScuGicInstance;
XAxiDma AxiDma;
struct netif server_netif;
struct netif *echo_netif;

/* --- DMA 缓冲区 --- */
u16 DmaRxBuffer[MAX_DMA_LEN / 2] __attribute__ ((aligned(64)));
u32 BdChainBuffer[BD_ALIGNMENT*16] __attribute__ ((aligned(64)));

/* --- LVGL 显存缓冲区 --- */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[320 * 40];
static lv_disp_drv_t disp_drv;

/* 声明外部函数 (实现在 app_monitor.c 中) */
extern void setup_udp_connection(void);

void Hardware_Init(void) {
    init_platform();
    LCD_Init_Hardware();

    extern XGpioPs Gpio; // 引用 LCD 驱动里的 GPIO 句柄
    XGpioPs_WritePin(&Gpio, 56, 1);

    InterruptInit(INT_DEVICE_ID, &XScuGicInstance);
    System_Timer_Init();

    // DMA 与中断初始化
    XAxiDma_Initial(DMA_DEV_ID, S2MM_INTR_ID, &AxiDma, &XScuGicInstance);
    InterruptConnect(&XScuGicInstance, S2MM_INTR_ID, Dma_Interrupt_Handler, &AxiDma, 0, 3);
    CreateBdChain(BdChainBuffer, BD_COUNT, ADC_SAMPLE_NUM * ADC_BYTE, (unsigned char *)DmaRxBuffer, RXPATH);
}

void UI_Init(void) {
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 320 * 40);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 172;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    my_gui_init();
    // 原来直接锁定运行状态的逻辑搬到这里
    my_gui_update_acquisition_status(1);
}

void Network_Init(void) {
    ip_addr_t ipaddr, netmask, gw;
    unsigned char mac_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    lwip_init();
    ipaddr_aton(SRC_IP_ADDR, &ipaddr);
    ipaddr_aton(SRC_NETMASK, &netmask);
    ipaddr_aton(SRC_GW, &gw);
    echo_netif = &server_netif;

    if (!xemac_add(echo_netif, &ipaddr, &netmask, &gw, mac_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\r\n");
        return;
    }
    netif_set_default(echo_netif);
    Xil_ExceptionEnable();
    netif_set_up(echo_netif);

    setup_udp_connection(); // 启动 UDP 监听
}

void Network_Task(void) {
    xemacif_input(echo_netif);
}

void LVGL_Task(void) {
    lv_timer_handler();
}
