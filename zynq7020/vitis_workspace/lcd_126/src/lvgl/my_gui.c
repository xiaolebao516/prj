// my_gui.c
#include "my_gui.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* 全局界面组件句柄 */
static lv_obj_t* label_status = NULL;
static lv_obj_t* label_prf = NULL;
static lv_obj_t* label_acq = NULL;
static lv_obj_t* label_packets = NULL;
static lv_obj_t* label_bottom_status = NULL;

/* 模拟数据状态 */
static uint32_t packet_count = 0;
static uint32_t prf_value = 1000;
static int is_connected = 0;
static int is_acquiring = 0;
static uint32_t update_counter = 0;

/* 样式定义 */
static lv_style_t style_panel;
static lv_style_t style_title;
static lv_style_t style_value_normal;
static lv_style_t style_value_connected;
static lv_style_t style_value_disconnected;
static lv_style_t style_value_running;
static lv_style_t style_value_stopped;
static lv_style_t style_indicator;

/* 私有函数声明 */
static void create_compact_status_display(void);
static void apply_styles(void);
static const char* get_bottom_status_text(void);

/*===========================================================================
 * 公有函数实现
 *===========================================================================*/

/* GUI初始化 */
void my_gui_init(void) {
    printf("[GUI] Initializing 320x172 interface...\n");

    /* 初始化样式 */
    apply_styles();

    /* 创建主界面 */
    my_gui_create_main_interface();

    /* 设置初始状态 */
    my_gui_update_connection_status(0);
    my_gui_update_prf_value(1000);
    my_gui_update_acquisition_status(0);
    my_gui_update_packet_count(0);

    printf("[GUI] Initialization completed\n");
}

/* 创建主界面 */
void my_gui_create_main_interface(void) {
    /* 1. 设置背景色 */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x1E2A3A), LV_PART_MAIN);

    /* 2. 顶部标题栏（极简） */
    lv_obj_t* title_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(title_bar, 310, 20);
    lv_obj_align(title_bar, LV_ALIGN_TOP_MID, 0, 2);
    lv_obj_set_style_bg_color(title_bar, lv_color_hex(0x2C3E50), 0);
    lv_obj_set_style_border_width(title_bar, 0, 0);
    lv_obj_set_style_radius(title_bar, 3, 0);

    lv_obj_t* title = lv_label_create(title_bar);
    lv_label_set_text(title, "ADC Monitor System");  // 改为英文
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_center(title);

    /* 3. 状态显示面板（紧凑布局） */
    create_compact_status_display();

    /* 4. 底部状态指示器 */
    lv_obj_t* indicator_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(indicator_panel, 310, 20);
    lv_obj_align(indicator_panel, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_obj_add_style(indicator_panel, &style_panel, 0);

    label_bottom_status = lv_label_create(indicator_panel);
    lv_obj_add_style(label_bottom_status, &style_indicator, 0);
    lv_label_set_text(label_bottom_status, get_bottom_status_text());
    lv_obj_center(label_bottom_status);
}

/* 更新连接状态 */
void my_gui_update_connection_status(int connected) {
    is_connected = connected;

    if (label_status) {
        if (connected) {
            lv_label_set_text(label_status, "Connected");  // 改为英文
            lv_obj_add_style(label_status, &style_value_connected, 0);
        } else {
            lv_label_set_text(label_status, "Disconnected");  // 改为英文
            lv_obj_add_style(label_status, &style_value_disconnected, 0);
        }
    }

    /* 更新底部状态 */
    if (label_bottom_status) {
        lv_label_set_text(label_bottom_status, get_bottom_status_text());
    }
}

/* 更新PRF值 */
void my_gui_update_prf_value(uint32_t prf) {
    prf_value = prf;

    if (label_prf) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%u Hz", prf);
        lv_label_set_text(label_prf, buffer);
    }
}

/* 更新采集状态 */
void my_gui_update_acquisition_status(int acquiring) {
    is_acquiring = acquiring;

    if (label_acq) {
        if (acquiring) {
            lv_label_set_text(label_acq, "Running");  // 改为英文
            lv_obj_add_style(label_acq, &style_value_running, 0);
        } else {
            lv_label_set_text(label_acq, "Stopped");  // 改为英文
            lv_obj_add_style(label_acq, &style_value_stopped, 0);
        }
    }

    /* 更新底部状态 */
    if (label_bottom_status) {
        lv_label_set_text(label_bottom_status, get_bottom_status_text());
    }
}

/* 更新包计数 */
void my_gui_update_packet_count(uint32_t count) {
    packet_count = count;

    if (label_packets) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%u", count);
        lv_label_set_text(label_packets, buffer);
    }
}

/* 模拟数据生成（测试用） */
void my_gui_generate_test_data(void) {
    update_counter++;

    /* 每100次更新模拟一次状态变化 */
    if (update_counter % 100 == 0) {
        /* 模拟连接状态切换 */
        if (update_counter % 200 == 0) {
            my_gui_update_connection_status(!is_connected);
        }

        /* 模拟采集状态切换（仅在连接时） */
        if (update_counter % 300 == 0 && is_connected) {
            my_gui_update_acquisition_status(!is_acquiring);
        }
    }

    /* 如果已连接且在采集，更新数据 */
    if (is_connected && is_acquiring) {
        /* 每5次更新增加包计数 */
        if (update_counter % 5 == 0) {
            packet_count += 3;
            my_gui_update_packet_count(packet_count);
        }

        /* 每10次更新模拟PRF变化 */
        if (update_counter % 10 == 0) {
            /* 模拟PRF在500-2000Hz之间变化 */
            static int prf_dir = 1;
            prf_value += prf_dir * 50;

            if (prf_value >= 2000) {
                prf_dir = -1;
                prf_value = 2000;
            } else if (prf_value <= 500) {
                prf_dir = 1;
                prf_value = 500;
            }

            my_gui_update_prf_value(prf_value);
        }
    }
}

/* 任务处理函数（为FreeRTOS准备） */
void my_gui_task_handler(void) {
    /* 调用模拟数据生成 */
    my_gui_generate_test_data();
}

/* 清理资源 */
void my_gui_deinit(void) {
    printf("[GUI] Cleaning up resources\n");
    /* 如果需要，可以添加资源清理代码 */
}

/*===========================================================================
 * 私有函数实现
 *===========================================================================*/

/* 创建紧凑状态显示 */
static void create_compact_status_display(void) {
    lv_obj_t* status_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(status_panel, 310, 100);
    lv_obj_align(status_panel, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_add_style(status_panel, &style_panel, 0);

    /* 紧凑的两列布局 */
    int x_left = 15;
    int x_right = 165;
    int y_start = 15;
    int y_step = 25;

    /* 左列：连接状态 */
    lv_obj_t* label_conn = lv_label_create(status_panel);
    lv_label_set_text(label_conn, "Connection:");  // 改为英文
    lv_obj_add_style(label_conn, &style_title, 0);
    lv_obj_set_pos(label_conn, x_left, y_start);

    label_status = lv_label_create(status_panel);
    lv_label_set_text(label_status, "Disconnected");  // 改为英文
    lv_obj_add_style(label_status, &style_value_disconnected, 0);
    lv_obj_set_pos(label_status, x_left + 70, y_start);

    /* 右列：PRF值 */
    lv_obj_t* label_prf_title = lv_label_create(status_panel);
    lv_label_set_text(label_prf_title, "PRF:");  // 改为英文
    lv_obj_add_style(label_prf_title, &style_title, 0);
    lv_obj_set_pos(label_prf_title, x_right, y_start);

    label_prf = lv_label_create(status_panel);
    lv_label_set_text(label_prf, "1000 Hz");
    lv_obj_add_style(label_prf, &style_value_normal, 0);
    lv_obj_set_pos(label_prf, x_right + 50, y_start);

    /* 第二行：左列 - 采集状态 */
    y_start += y_step;
    lv_obj_t* label_acq_title = lv_label_create(status_panel);
    lv_label_set_text(label_acq_title, "Acquisition:");  // 改为英文
    lv_obj_add_style(label_acq_title, &style_title, 0);
    lv_obj_set_pos(label_acq_title, x_left, y_start);

    label_acq = lv_label_create(status_panel);
    lv_label_set_text(label_acq, "Stopped");  // 改为英文
    lv_obj_add_style(label_acq, &style_value_stopped, 0);
    lv_obj_set_pos(label_acq, x_left + 70, y_start);

    /* 第二行：右列 - 包计数 */
    lv_obj_t* label_pkt_title = lv_label_create(status_panel);
    lv_label_set_text(label_pkt_title, "Packets:");  // 改为英文
    lv_obj_add_style(label_pkt_title, &style_title, 0);
    lv_obj_set_pos(label_pkt_title, x_right, y_start);

    label_packets = lv_label_create(status_panel);
    lv_label_set_text(label_packets, "0");
    lv_obj_add_style(label_packets, &style_value_normal, 0);
    lv_obj_set_pos(label_packets, x_right + 50, y_start);

    /* 添加分隔线 */
    lv_obj_t* line = lv_obj_create(status_panel);
    lv_obj_set_size(line, 280, 1);
    lv_obj_set_pos(line, 15, 75);
    lv_obj_set_style_bg_color(line, lv_color_hex(0x4A657F), 0);
    lv_obj_set_style_border_width(line, 0, 0);

    /* 状态说明 */
    lv_obj_t* label_info = lv_label_create(status_panel);
    lv_label_set_text(label_info, "Simulator Running - Click to toggle");  // 改为英文
    lv_obj_set_style_text_color(label_info, lv_color_hex(0x95A5A6), 0);
    lv_obj_set_style_text_font(label_info, &lv_font_montserrat_10, 0);
    lv_obj_set_pos(label_info, 15, 85);
}

/* 应用样式 */
static void apply_styles(void) {
    /* 面板样式 */
    lv_style_init(&style_panel);
    lv_style_set_bg_color(&style_panel, lv_color_hex(0x2C3E50));
    lv_style_set_border_color(&style_panel, lv_color_hex(0x34495E));
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_radius(&style_panel, 5);
    lv_style_set_pad_all(&style_panel, 0);

    /* 标题样式 */
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_hex(0xBDC3C7));
    lv_style_set_text_font(&style_title, &lv_font_montserrat_12);

    /* 普通数值样式 */
    lv_style_init(&style_value_normal);
    lv_style_set_text_color(&style_value_normal, lv_color_hex(0xECF0F1));
    lv_style_set_text_font(&style_value_normal, &lv_font_montserrat_12);

    /* 已连接样式（绿色） */
    lv_style_init(&style_value_connected);
    lv_style_set_text_color(&style_value_connected, lv_color_hex(0x2ECC71));
    lv_style_set_text_font(&style_value_connected, &lv_font_montserrat_12);

    /* 未连接样式（红色） */
    lv_style_init(&style_value_disconnected);
    lv_style_set_text_color(&style_value_disconnected, lv_color_hex(0xE74C3C));
    lv_style_set_text_font(&style_value_disconnected, &lv_font_montserrat_12);

    /* 运行中样式（绿色） */
    lv_style_init(&style_value_running);
    lv_style_set_text_color(&style_value_running, lv_color_hex(0x2ECC71));
    lv_style_set_text_font(&style_value_running, &lv_font_montserrat_12);

    /* 停止样式（橙色） */
    lv_style_init(&style_value_stopped);
    lv_style_set_text_color(&style_value_stopped, lv_color_hex(0xF39C12));
    lv_style_set_text_font(&style_value_stopped, &lv_font_montserrat_12);

    /* 底部指示器样式 */
    lv_style_init(&style_indicator);
    lv_style_set_text_color(&style_indicator, lv_color_hex(0x3498DB));
    lv_style_set_text_font(&style_indicator, &lv_font_montserrat_10);
}

/* 获取底部状态文本 */
static const char* get_bottom_status_text(void) {
    if (!is_connected) {
        return "Device Disconnected";  // 改为英文
    } else if (!is_acquiring) {
        return "Connected, Ready to Start";  // 改为英文
    } else {
        return "Acquisition Running...";  // 改为英文
    }
}
