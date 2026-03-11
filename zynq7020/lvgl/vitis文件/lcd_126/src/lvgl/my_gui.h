// my_gui.h
#ifndef MY_GUI_H
#define MY_GUI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GUI初始化函数 */
void my_gui_init(void);
void my_gui_deinit(void);

/* 界面创建函数 */
void my_gui_create_main_interface(void);

/* 数据更新接口 */
void my_gui_update_connection_status(int connected);
void my_gui_update_prf_value(uint32_t prf);
void my_gui_update_acquisition_status(int acquiring);
void my_gui_update_packet_count(uint32_t count);

/* 模拟数据生成（测试用） */
void my_gui_generate_test_data(void);

/* 任务处理函数（为FreeRTOS移植准备） */
void my_gui_task_handler(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* MY_GUI_H */
