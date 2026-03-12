#ifndef APP_MAIN_H
#define APP_MAIN_H

void Hardware_Init(void);
void UI_Init(void);
void Network_Init(void);

void Network_Task(void);
void LVGL_Task(void);

#endif /* APP_MAIN_H */
