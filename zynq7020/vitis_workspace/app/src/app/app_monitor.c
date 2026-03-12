// app/app_monitor.c
#include "app_monitor.h"
#include "ui_main.h"
#include "adc_dma.h"
#include "xil_cache.h"
#include "sleep.h"
#include <stdlib.h>
#include <string.h>

#include "lwip/udp.h"

#define DEST_IP_ADDR   "192.168.1.100"
#define DEST_PORT      8080

/* 引用底层已经初始化好的资源 */
extern volatile int s2mm_flag;
extern u16 DmaRxBuffer[];
extern u32 BdChainBuffer[];
extern XAxiDma AxiDma;

static struct udp_pcb *udp_pcb_send;
static ip_addr_t ip_dest;

/* 业务状态变量 */
static u32 g_prf_hz = 30;
static u32 g_prf_delay_us = 33333;
static int g_connected_once = 0;
static u32 total_packets = 0;

static void Set_System_PRF(u32 target_hz) {
    if(target_hz > 500) target_hz = 100;
    if(target_hz < 1) target_hz = 1;

    g_prf_hz = target_hz;
    g_prf_delay_us = 1000000 / target_hz;

    my_gui_update_prf_value(g_prf_hz);
}

static void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    if (p != NULL) {
        char clean_buf[16] = {0};
        u16 copy_len = (p->len > 15) ? 15 : p->len;
        memcpy(clean_buf, p->payload, copy_len);

        for(int i = 0; i < 16; i++) {
            if(clean_buf[i] == '\r' || clean_buf[i] == '\n' || clean_buf[i] == ' ' || clean_buf[i] == '\t') {
                clean_buf[i] = '\0';
                break;
            }
        }

        int new_prf = atoi(clean_buf);
        if (new_prf > 0) {
            Set_System_PRF((u32)new_prf);
        }

        if (!g_connected_once) {
            g_connected_once = 1;
            my_gui_update_connection_status(1);
        }
        pbuf_free(p);
    }
}

void setup_udp_connection(void) {
    udp_pcb_send = udp_new();
    udp_bind(udp_pcb_send, IP_ADDR_ANY, 8080);
    ipaddr_aton(DEST_IP_ADDR, &ip_dest);
    udp_connect(udp_pcb_send, &ip_dest, DEST_PORT);
    udp_recv(udp_pcb_send, udp_recv_callback, NULL);
}

static void send_adc_data_udp(u16 *data, u32 sample_num) {
    struct pbuf *p;
    u32 total_bytes = sample_num * sizeof(u16);
    p = pbuf_alloc(PBUF_TRANSPORT, total_bytes, PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, data, total_bytes);

    if(udp_send(udp_pcb_send, p) == ERR_OK) {
        if (!g_connected_once) {
            g_connected_once = 1;
            my_gui_update_connection_status(1);
        }
    }
    pbuf_free(p);
}

void App_Monitor_Init(void) {
    total_packets = 0;
    Set_System_PRF(30);

    // 启动第一次 ADC 采集
    XAxiDma_Adc(BdChainBuffer, AD9280_BASE, ADC_SAMPLE_NUM, BD_COUNT, &AxiDma);
}

void App_Monitor_Tick(void) {
    if (s2mm_flag) {
        s2mm_flag = 0; // 清标志位
        Xil_DCacheInvalidateRange((uintptr_t)DmaRxBuffer, ADC_SAMPLE_NUM * ADC_BYTE);

        // 发送数据到上位机
        send_adc_data_udp(DmaRxBuffer, ADC_SAMPLE_NUM);

        // 刷新 UI 计数
        total_packets++;
        my_gui_update_packet_count(total_packets);

        // 维持采样频率
        if (g_prf_delay_us > 0) {
            usleep(g_prf_delay_us);
        }

        // 重启下一次采集
        XAxiDma_Adc(BdChainBuffer, AD9280_BASE, ADC_SAMPLE_NUM, BD_COUNT, &AxiDma);
    }
}
