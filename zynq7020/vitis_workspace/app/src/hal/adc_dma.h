#ifndef SRC_ADC_DMA_H_
#define SRC_ADC_DMA_H_

/* 移除导致报错的驱动头文件，添加 IO 操作头文件 */
// #include "ad9280_sample.h"
#include "xil_io.h"

#include "xaxidma.h"
#include "dma_bd/dma_bd.h"
#include "xil_types.h"
#include "xscugic.h"

/*
 * 手动定义寄存器偏移和写操作宏
 * (替代原有的驱动函数)
 */
#define AD9280_SAMPLE_S00_AXI_SLV_REG0_OFFSET 0
#define AD9280_SAMPLE_S00_AXI_SLV_REG1_OFFSET 4

#define AD9280_SAMPLE_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/*
 * DMA redefines
 */
#define MAX_DMA_LEN		   0x800000      /* DMA max length in byte */
#define DMA_DEV_ID		   XPAR_AXIDMA_0_DEVICE_ID
#define S2MM_INTR_ID       XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR

/*
 * ADC defines
 */
/* >>> 关键修改：直接填入 Vivado Address Editor 中的地址 <<< */
#define AD9280_BASE        0x43C00000

#define AD9280_START       AD9280_SAMPLE_S00_AXI_SLV_REG0_OFFSET
#define AD9280_LENGTH      AD9280_SAMPLE_S00_AXI_SLV_REG1_OFFSET

/* >>> 修改点：16位 ADC <<< */
#define ADC_BYTE           2              /* 16-bit = 2 bytes */
#define ADC_BITS           16             /* Resolution */

/*
 * DMA BD defines
 */
#define BD_COUNT         4

/* 采样点数，可根据需要调整 */
#define ADC_SAMPLE_NUM  128

extern volatile int s2mm_flag;

int XAxiDma_Initial(u16 DeviceId, u16 IntrID, XAxiDma *XAxiDma, XScuGic *InstancePtr);
void Dma_Interrupt_Handler(void *CallBackRef);
void XAxiDma_Adc(u32 *BdChainBuffer, u32 adc_addr, u32 adc_len, u16 BdCount, XAxiDma *AxiDma) ;
void ad9280_sample(u32 adc_addr, u32 adc_len) ;

#endif /* SRC_ADC_DMA_H_ */
