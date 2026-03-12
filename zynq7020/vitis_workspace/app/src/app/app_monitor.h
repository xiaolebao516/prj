// app/app_monitor.h
#ifndef APP_MONITOR_H
#define APP_MONITOR_H

#include "xil_types.h"

void App_Monitor_Init(void);
void App_Monitor_Tick(void);

// 网络初始化需要调用它
void setup_udp_connection(void);

#endif /* APP_MONITOR_H */
