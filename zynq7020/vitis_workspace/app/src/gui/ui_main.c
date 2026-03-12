#include "ui_main.h"
#include <stdio.h>
#include <string.h>

/* --- 全局界面组件句柄 --- */
static lv_obj_t* label_status = NULL;
static lv_obj_t* label_prf = NULL;
static lv_obj_t* label_acq = NULL;
static lv_obj_t* label_packets = NULL;
static lv_obj_t* label_bottom_status = NULL;

/* --- 内部状态缓存 (仅用于状态栏文字联动) --- */
static int is_connected = 0;
static int is_acquiring = 0;

/* --- 样式定义 --- */
static lv_style_t style_panel;
static lv_style_t style_title;
static lv_style_t style_value_normal;
static lv_style_t style_value_connected;
static lv_style_t style_value_disconnected;
static lv_style_t style_value_running;
static lv_style_t style_value_stopped;
static lv_style_t style_indicator;

/* --- 私有函数声明 --- */
static void create_compact_status_display(void);
static void apply_styles(void);
static const char* get_bottom_status_text(void);

/*===========================================================================
 * 公有函数实现
 *===========================================================================*/

void my_gui_init(void) {
    printf("[GUI] Initializing 320x172 interface...\n");

    /* 1. 初始化所有组件的样式 */
    apply_styles();

    /* 2. 创建主界面的容器和标签 */
    my_gui_create_main_interface();

    /* 3. 设置安全的初始默认值 */
    my_gui_update_connection_status(0);
    my_gui_update_prf_value(0);
    my_gui_update_acquisition_status(0);
    my_gui_update_packet_count(0);

    printf("[GUI] Initialization completed\n");
}

void my_gui_create_main_interface(void) {
    /* 设置深色背景 */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x1E2A3A), LV_PART_MAIN);

    /* 顶部标题栏 */
    lv_obj_t* title_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(title_bar, 310, 20);
    lv_obj_align(title_bar, LV_ALIGN_TOP_MID, 0, 2);
    lv_obj_set_style_bg_color(title_bar, lv_color_hex(0x2C3E50), 0);
    lv_obj_set_style_border_width(title_bar, 0, 0);
    lv_obj_set_style_radius(title_bar, 3, 0);

    lv_obj_t* title = lv_label_create(title_bar);
    lv_label_set_text(title, "ADC Monitor System");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_center(title);

    /* 中间的核心数据面板 */
    create_compact_status_display();

    /* 底部状态指示栏 */
    lv_obj_t* indicator_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(indicator_panel, 310, 20);
    lv_obj_align(indicator_panel, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_obj_add_style(indicator_panel, &style_panel, 0);

    label_bottom_status = lv_label_create(indicator_panel);
    lv_obj_add_style(label_bottom_status, &style_indicator, 0);
    lv_label_set_text(label_bottom_status, get_bottom_status_text());
    lv_obj_center(label_bottom_status);
}

void my_gui_deinit(void) {
    printf("[GUI] Cleaning up resources\n");
}

/*===========================================================================
 * 数据更新接口 (供 App 逻辑层调用)
 *===========================================================================*/

void my_gui_update_connection_status(int connected) {
    is_connected = connected;

    if (label_status) {
        if (connected) {
            lv_label_set_text(label_status, "Connected");
            lv_obj_add_style(label_status, &style_value_connected, 0);
        } else {
            lv_label_set_text(label_status, "Disconnected");
            lv_obj_add_style(label_status, &style_value_disconnected, 0);
        }
    }
    /* 同步刷新底部动态提示 */
    if (label_bottom_status) {
        lv_label_set_text(label_bottom_status, get_bottom_status_text());
    }
}

void my_gui_update_prf_value(uint32_t prf) {
    if (label_prf) {
        /* 使用 %lu 和强转来消除 Zynq SDK 里的警告 */
        lv_label_set_text_fmt(label_prf, "%lu Hz", (unsigned long)prf);
    }
}

void my_gui_update_acquisition_status(int acquiring) {
    is_acquiring = acquiring;

    if (label_acq) {
        if (acquiring) {
            lv_label_set_text(label_acq, "Running");
            lv_obj_add_style(label_acq, &style_value_running, 0);
        } else {
            lv_label_set_text(label_acq, "Stopped");
            lv_obj_add_style(label_acq, &style_value_stopped, 0);
        }
    }
    if (label_bottom_status) {
        lv_label_set_text(label_bottom_status, get_bottom_status_text());
    }
}

void my_gui_update_packet_count(uint32_t count) {
    if (label_packets) {
        /* 使用 %lu 和强转来消除警告 */
        lv_label_set_text_fmt(label_packets, "%lu", (unsigned long)count);
    }
}

/*===========================================================================
 * 私有函数实现
 *===========================================================================*/

static void create_compact_status_display(void) {
    lv_obj_t* status_panel = lv_obj_create(lv_scr_act());
    /* 优化：去掉模拟器文字后，面板高度从 100 缩小到 85，整体更紧凑 */
    lv_obj_set_size(status_panel, 310, 85);
    lv_obj_align(status_panel, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_add_style(status_panel, &style_panel, 0);

    /* 紧凑的两列布局坐标 */
    int x_left = 15;
    int x_right = 165;
    int y_start = 18;  /* 稍微往下挪一点，让内容居中 */
    int y_step = 30;   /* 增大行距，让数值看起来不那么拥挤 */

    /* ----- 第一行 ----- */
    /* 左列：连接状态 */
    lv_obj_t* label_conn = lv_label_create(status_panel);
    lv_label_set_text(label_conn, "Connection:");
    lv_obj_add_style(label_conn, &style_title, 0);
    lv_obj_set_pos(label_conn, x_left, y_start);

    label_status = lv_label_create(status_panel);
    lv_label_set_text(label_status, "---");
    lv_obj_add_style(label_status, &style_value_disconnected, 0);
    lv_obj_set_pos(label_status, x_left + 75, y_start); /* 微调 x 轴对齐 */

    /* 右列：PRF值 */
    lv_obj_t* label_prf_title = lv_label_create(status_panel);
    lv_label_set_text(label_prf_title, "PRF:");
    lv_obj_add_style(label_prf_title, &style_title, 0);
    lv_obj_set_pos(label_prf_title, x_right, y_start);

    label_prf = lv_label_create(status_panel);
    lv_label_set_text(label_prf, "--- Hz");
    lv_obj_add_style(label_prf, &style_value_normal, 0);
    lv_obj_set_pos(label_prf, x_right + 50, y_start);

    /* ----- 第二行 ----- */
    y_start += y_step;
    /* 左列：采集状态 */
    lv_obj_t* label_acq_title = lv_label_create(status_panel);
    lv_label_set_text(label_acq_title, "Acquisition:");
    lv_obj_add_style(label_acq_title, &style_title, 0);
    lv_obj_set_pos(label_acq_title, x_left, y_start);

    label_acq = lv_label_create(status_panel);
    lv_label_set_text(label_acq, "---");
    lv_obj_add_style(label_acq, &style_value_stopped, 0);
    lv_obj_set_pos(label_acq, x_left + 75, y_start);

    /* 右列：包计数 */
    lv_obj_t* label_pkt_title = lv_label_create(status_panel);
    lv_label_set_text(label_pkt_title, "Packets:");
    lv_obj_add_style(label_pkt_title, &style_title, 0);
    lv_obj_set_pos(label_pkt_title, x_right, y_start);

    label_packets = lv_label_create(status_panel);
    lv_label_set_text(label_packets, "0");
    lv_obj_add_style(label_packets, &style_value_normal, 0);
    lv_obj_set_pos(label_packets, x_right + 50, y_start);
}

static void apply_styles(void) {
    /* 基础面板样式 */
    lv_style_init(&style_panel);
    lv_style_set_bg_color(&style_panel, lv_color_hex(0x2C3E50));
    lv_style_set_border_color(&style_panel, lv_color_hex(0x34495E));
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_radius(&style_panel, 5);
    lv_style_set_pad_all(&style_panel, 0);

    /* 文本标题样式 */
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_hex(0xBDC3C7));
    lv_style_set_text_font(&style_title, &lv_font_montserrat_12);

    /* 动态数值常规样式 */
    lv_style_init(&style_value_normal);
    lv_style_set_text_color(&style_value_normal, lv_color_hex(0xECF0F1));
    lv_style_set_text_font(&style_value_normal, &lv_font_montserrat_12);

    /* 状态颜色：连接 (绿) */
    lv_style_init(&style_value_connected);
    lv_style_set_text_color(&style_value_connected, lv_color_hex(0x2ECC71));
    lv_style_set_text_font(&style_value_connected, &lv_font_montserrat_12);

    /* 状态颜色：断开 (红) */
    lv_style_init(&style_value_disconnected);
    lv_style_set_text_color(&style_value_disconnected, lv_color_hex(0xE74C3C));
    lv_style_set_text_font(&style_value_disconnected, &lv_font_montserrat_12);

    /* 状态颜色：运行中 (绿) */
    lv_style_init(&style_value_running);
    lv_style_set_text_color(&style_value_running, lv_color_hex(0x2ECC71));
    lv_style_set_text_font(&style_value_running, &lv_font_montserrat_12);

    /* 状态颜色：停止 (橙) */
    lv_style_init(&style_value_stopped);
    lv_style_set_text_color(&style_value_stopped, lv_color_hex(0xF39C12));
    lv_style_set_text_font(&style_value_stopped, &lv_font_montserrat_12);

    /* 底部信息栏字体样式 */
    lv_style_init(&style_indicator);
    lv_style_set_text_color(&style_indicator, lv_color_hex(0x3498DB));
    lv_style_set_text_font(&style_indicator, &lv_font_montserrat_10);
}

static const char* get_bottom_status_text(void) {
    if (!is_connected) {
        return "System Ready - Awaiting UDP Connection";
    } else if (!is_acquiring) {
        return "Link Established - Idle";
    } else {
        return "Acquiring ADC Data Stream...";
    }
}
