#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>

#include "dma.h"
#include "isr.h"
#include "ws2812_var.h"
#include "adc_var.h"



void dma1_init(void);
void dma2_init(void);


void dma1_init(void){

	rcc_periph_clock_enable(RCC_DMA1);
    	nvic_enable_irq(NVIC_DMA1_STREAM6_IRQ);
	dma_stream_reset(DMA1, DMA_STREAM6);
    	dma_set_priority(DMA1, DMA_STREAM6, DMA_SxCR_PL_VERY_HIGH);
    	dma_set_memory_size(DMA1, DMA_STREAM6, DMA_SxCR_MSIZE_16BIT);
    	dma_set_peripheral_size(DMA1, DMA_STREAM6, DMA_SxCR_PSIZE_16BIT);
    	dma_enable_circular_mode(DMA1, DMA_STREAM6);
    	dma_enable_memory_increment_mode(DMA1, DMA_STREAM6);
    	dma_set_transfer_mode(DMA1, DMA_STREAM6, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    	dma_set_peripheral_address(DMA1, DMA_STREAM6, (uint32_t)&TIM4_CCR1);
    	dma_set_memory_address(DMA1, DMA_STREAM6, (uint32_t)(&bit_buffer[0]));
    	dma_set_number_of_data(DMA1, DMA_STREAM6, LEDS_BUFFER_SIZE);
    	dma_enable_half_transfer_interrupt(DMA1, DMA_STREAM6);
    	dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM6);
    	dma_channel_select(DMA1, DMA_STREAM6, DMA_SxCR_CHSEL_2);
    	nvic_clear_pending_irq(NVIC_DMA1_STREAM6_IRQ);
    	nvic_set_priority(NVIC_DMA1_STREAM6_IRQ, 0); 
	nvic_enable_irq(NVIC_DMA1_STREAM6_IRQ);

}


void dma2_init(void){

	rcc_periph_clock_enable(RCC_DMA2);
    	nvic_enable_irq(NVIC_DMA2_STREAM0_IRQ);
	
	dma_stream_reset(DMA2, DMA_STREAM0);
    	dma_set_priority(DMA2, DMA_STREAM0, DMA_SxCR_PL_VERY_HIGH);
    	dma_set_memory_size(DMA2, DMA_STREAM0, DMA_SxCR_MSIZE_16BIT);
    	dma_set_peripheral_size(DMA2, DMA_STREAM0, DMA_SxCR_PSIZE_16BIT);
    	dma_enable_circular_mode(DMA2, DMA_STREAM0);
    	dma_enable_memory_increment_mode(DMA2, DMA_STREAM0);
    	dma_set_transfer_mode(DMA2, DMA_STREAM0, DMA_SxCR_DIR_PERIPHERAL_TO_MEM);
    	dma_set_peripheral_address(DMA2, DMA_STREAM0, (uint32_t)&ADC1_DR);
    	dma_set_memory_address(DMA2, DMA_STREAM0, (uint32_t)(&adc_buffer[0]));
    	dma_set_number_of_data(DMA2, DMA_STREAM0, ADC_BUFFER_SIZE);
    	dma_enable_half_transfer_interrupt(DMA2, DMA_STREAM0);
    	dma_enable_transfer_complete_interrupt(DMA2, DMA_STREAM0);
    	dma_channel_select(DMA2, DMA_STREAM0, DMA_SxCR_CHSEL_0);
    	nvic_clear_pending_irq(NVIC_DMA2_STREAM0_IRQ);
    	nvic_set_priority(NVIC_DMA2_STREAM0_IRQ, 0);
	nvic_enable_irq(NVIC_DMA2_STREAM0_IRQ);
}




void ws2812_dma_init(void){

	dma1_init();
}



void adc_dma_init(void){
	dma2_init();

}




void dac_dma_init(void){



}



//ISRs


void dma1_stream6_isr(void) {
	dma1_str6_isr();
}



void dma2_stream0_isr(void) {
	dma2_str0_isr();
}