#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/dac.h>

#include "dac_var.h"
#include "isr.h"
#include "dac.h"
#include "dma.h"
#include "timer.h"
#include "gpio.h"

uint16_t dac_buffer[DAC_BUFFER_SIZE];


void dac_setup(void);

void dma1_str5_isr(void){

	if (dma_get_interrupt_flag(DMA1, DMA_STREAM5, DMA_HTIF) != 0) {
		
		dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_HTIF);
	
    	}

    	if (dma_get_interrupt_flag(DMA1, DMA_STREAM5, DMA_TCIF) != 0) {
		
		dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_TCIF);
    	}

}


void dac_setup(void){
	rcc_periph_clock_enable(RCC_DAC);
	/* Setup the DAC channel 1, with timer 2 as trigger source.
	 * Assume the DAC has woken up by the time the first transfer occurs */
	dac_trigger_enable(CHANNEL_1);
	dac_set_trigger_source(DAC_CR_TSEL1_T2);
	dac_dma_enable(CHANNEL_1);
	dac_enable(CHANNEL_1);
}

void dac_init(void){

	for(int i = 0; i < DAC_BUFFER_SIZE; i++){
		dac_buffer[i] = 0;
	}

	dac_gpio_init();
	dac_timer_init();
	dac_dma_init();
	dac_setup();

	

	dma_enable_stream(DMA1, DMA_STREAM5);

}