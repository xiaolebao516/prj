#include "adc_dma.h"

volatile int s2mm_flag = FALSE;

void XAxiDma_Adc(u32 *BdChainBuffer, u32 adc_addr, u32 adc_len, u16 BdCount, XAxiDma *AxiDma)
{
	/* Clear BD Status */
	Bd_StatusClr(BdChainBuffer, BdCount) ;
	/* Start sample */
	ad9280_sample(adc_addr, adc_len)  ;
	/* start DMA translation from ADC channel 0 to DDR3 */
	Bd_Start(BdChainBuffer, BdCount, AxiDma, RXPATH) ;
}

/*
 *Initial DMA and connect interrupt to handler
 */
int XAxiDma_Initial(u16 DeviceId, u16 IntrID, XAxiDma *XAxiDma, XScuGic *InstancePtr)
{
	XAxiDma_Config *CfgPtr;
	int Status;
	/* Initialize the XAxiDma device. */
	CfgPtr = XAxiDma_LookupConfig(DeviceId);
	if (!CfgPtr) {
		xil_printf("No config found for %d\r\n", DeviceId);
		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(XAxiDma, CfgPtr);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}

	/* Disable MM2S interrupt, Enable S2MM interrupt */
	XAxiDma_IntrEnable(XAxiDma, XAXIDMA_IRQ_IOC_MASK,
			XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(XAxiDma, XAXIDMA_IRQ_ALL_MASK,
			XAXIDMA_DMA_TO_DEVICE);

	return XST_SUCCESS ;
}

/*
 * Interrupt Handler
 */
void Dma_Interrupt_Handler(void *CallBackRef)
{
	XAxiDma *XAxiDmaPtr ;
	XAxiDmaPtr = (XAxiDma *) CallBackRef ;

	int  s2mm_sr = XAxiDma_IntrGetIrq(XAxiDmaPtr, XAXIDMA_DEVICE_TO_DMA) ;
	// xil_printf("Interrupt Value is 0x%x\r\n", s2mm_sr) ;

	if (s2mm_sr & XAXIDMA_IRQ_IOC_MASK)
	{
		/* Clear interrupt */
		XAxiDma_IntrAckIrq(XAxiDmaPtr, XAXIDMA_IRQ_IOC_MASK,
				XAXIDMA_DEVICE_TO_DMA) ;

		s2mm_flag = TRUE;
	}
}

/*
 * Configure Custom IP Core
 */
void ad9280_sample(u32 adc_addr, u32 adc_len)
{
	/* Write length (Samples) */
	AD9280_SAMPLE_mWriteReg(adc_addr, AD9280_LENGTH, adc_len)  ;
	/* Write Start bit */
	AD9280_SAMPLE_mWriteReg(adc_addr, AD9280_START, 1) ;
}
