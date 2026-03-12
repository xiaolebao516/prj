#include "sys_timer.h"
#include "xparameters.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "../lvgl/lvgl.h"

#define TIMER_DEVICE_ID XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID  XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR

static XScuTimer TimerInstance;
static XScuGic IntcInstance;

// 中断服务函数
static void Timer_Handler(void *CallBackRef) {
    XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
    XScuTimer_ClearInterruptStatus(TimerInstancePtr);
    lv_tick_inc(1); // 告诉 LVGL 过了 1ms
}

int System_Timer_Init(void) {
    int Status;
    XScuTimer_Config *ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
    XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr, ConfigPtr->BaseAddr);
    XScuTimer_EnableAutoReload(&TimerInstance);
    // 666MHz CPU -> 333MHz Timer -> 333333 cycles = 1ms
    XScuTimer_LoadTimer(&TimerInstance, 333333);

    XScuGic_Config *IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(&IntcInstance, IntcConfig, IntcConfig->CpuBaseAddress);

    XScuGic_Connect(&IntcInstance, TIMER_IRPT_INTR, (Xil_ExceptionHandler)Timer_Handler, (void *)&TimerInstance);
    XScuGic_Enable(&IntcInstance, TIMER_IRPT_INTR);
    XScuTimer_EnableInterrupt(&TimerInstance);

    XScuTimer_Start(&TimerInstance);

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &IntcInstance);
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}
