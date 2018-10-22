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
#include "dac_var.h"


bool dma1_initialized = false;
bool dma2_initialized = false;

void dma1_init(void);
void dma2_init(void);


void dma1_init(void){

	if(dma1_initialized == false){
		rcc_periph_clock_enable(RCC_DMA1);
		dma1_initialized = true;
	}
}


void dma2_init(void){

	if(dma2_initialized == false){
		rcc_periph_clock_enable(RCC_DMA2);
		dma2_initialized = true;
	}
}



//DMA 1 Stream 2 Channel 5
void ws2812_dma_init(void){

	dma1_init();

	nvic_enable_irq(NVIC_DMA1_STREAM2_IRQ);
	dma_stream_reset(DMA1, DMA_STREAM2);
    	dma_set_priority(DMA1, DMA_STREAM2, DMA_SxCR_PL_VERY_HIGH);
    	dma_set_memory_size(DMA1, DMA_STREAM2, DMA_SxCR_MSIZE_16BIT);
    	dma_set_peripheral_size(DMA1, DMA_STREAM2, DMA_SxCR_PSIZE_16BIT);
    	dma_enable_circular_mode(DMA1, DMA_STREAM2);
    	dma_enable_memory_increment_mode(DMA1, DMA_STREAM2);
    	dma_set_transfer_mode(DMA1, DMA_STREAM2, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    	dma_set_peripheral_address(DMA1, DMA_STREAM2, (uint32_t)&TIM3_CCR1);
    	dma_set_memory_address(DMA1, DMA_STREAM2, (uint32_t)(&bit_buffer[0]));
    	dma_set_number_of_data(DMA1, DMA_STREAM2, LEDS_BUFFER_SIZE);
    	dma_enable_half_transfer_interrupt(DMA1, DMA_STREAM2);
    	dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM2);
    	dma_channel_select(DMA1, DMA_STREAM2, DMA_SxCR_CHSEL_5);
    	nvic_clear_pending_irq(NVIC_DMA1_STREAM2_IRQ);
    	nvic_set_priority(NVIC_DMA1_STREAM2_IRQ, 0); 
	nvic_enable_irq(NVIC_DMA1_STREAM2_IRQ);
}


//DMA 2 Stream 0 Channel 0
void adc_dma_init(void){

	dma2_init();

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




void dac_dma_init(void){
	
	dma1_init();

	nvic_enable_irq(NVIC_DMA1_STREAM5_IRQ);
	dma_stream_reset(DMA1, DMA_STREAM5);
    	dma_set_priority(DMA1, DMA_STREAM5, DMA_SxCR_PL_VERY_HIGH);
    	dma_set_memory_size(DMA1, DMA_STREAM5, DMA_SxCR_MSIZE_16BIT);
    	dma_set_peripheral_size(DMA1, DMA_STREAM5, DMA_SxCR_PSIZE_16BIT);
    	dma_enable_circular_mode(DMA1, DMA_STREAM5);
    	dma_enable_memory_increment_mode(DMA1, DMA_STREAM5);
    	dma_set_transfer_mode(DMA1, DMA_STREAM5, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    	dma_set_peripheral_address(DMA1, DMA_STREAM5, (uint32_t)&DAC_DHR12R1);
    	dma_set_memory_address(DMA1, DMA_STREAM5, (uint32_t)(&dac_buffer[0]));
    	dma_set_number_of_data(DMA1, DMA_STREAM5, DAC_BUFFER_SIZE);
    	dma_enable_half_transfer_interrupt(DMA1, DMA_STREAM5);
    	dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM5);
    	dma_channel_select(DMA1, DMA_STREAM5, DMA_SxCR_CHSEL_7);
    	nvic_clear_pending_irq(NVIC_DMA1_STREAM5_IRQ);
    	nvic_set_priority(NVIC_DMA1_STREAM5_IRQ, 0); 
	nvic_enable_irq(NVIC_DMA1_STREAM5_IRQ);

	
}



//ISRs


//WS2812
void dma1_stream2_isr(void) {
	dma1_str2_isr();
}


//DAC1
void dma1_stream5_isr(void) {
	dma1_str5_isr();
}


//ADC1
void dma2_stream0_isr(void) {
	dma2_str0_isr();
}