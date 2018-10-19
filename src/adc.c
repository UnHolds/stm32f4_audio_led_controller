#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/adc.h>

#include "adc.h"
#include "dma.h"
#include "isr.h"
#include "gpio.h"
#include "adc_var.h"
#include "timer.h"



uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint8_t channel_seq[1];

void adc_gpio_setup(void);
void adc_setup(void);
void adc_dma_setup(void);






//void adc_gpio_setup(void){ }





void dma2_str0_isr(void) {

    if (dma_get_interrupt_flag(DMA2, DMA_STREAM0, DMA_HTIF) != 0) {
	
	
	
	dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_HTIF);
	
    }

    if (dma_get_interrupt_flag(DMA2, DMA_STREAM0, DMA_TCIF) != 0) {
		

	
	dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_TCIF);
    }
}






void adc_setup(void){

	channel_seq[0] = 0;

	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_reset_pulse(RST_ADC);
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_3CYC);
	adc_set_resolution(ADC1, ADC_CR1_RES_12BIT);
	//adc_disable_external_trigger_regular(ADC1);
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_TIM2_TRGO, ADC_CR2_EXTEN_RISING_EDGE);
	adc_set_regular_sequence(ADC1, 1, channel_seq);
	//adc_set_continuous_conversion_mode(ADC1);
	adc_eoc_after_group(ADC1);
	adc_disable_eoc_interrupt(ADC1);
	adc_enable_dma(ADC1);
	adc_set_dma_continue(ADC1);
	adc_power_on(ADC1);	
	adc_start_conversion_regular(ADC1);
}


void adc_init(void){
	adc_gpio_init();
	adc_timer_init();
	adc_dma_init();
	adc_setup();
	gpio_set(GPIOA, GPIO6);
	gpio_set(GPIOA, GPIO7);	
	
	dma_enable_stream(DMA2, DMA_STREAM0);
	while(1){
		__asm__("nop");	

		if(adc_buffer[0] > 1000){
			gpio_clear(GPIOA, GPIO6);
		}else{
			gpio_set(GPIOA, GPIO6);
		}	
	}

}