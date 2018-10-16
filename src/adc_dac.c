#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/adc.h>

#include "adc_dac.h"

#define ADC_BUFFER_SIZE 16
#define DAC_BUFFER_SIZE 16 //must be a muliple by 2


uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint16_t dac_buffer[DAC_BUFFER_SIZE];
uint8_t channel_seq[1];

void adc_gpio_setup(void);
void adc_setup(void);
void adc_dma_setup(void);






void adc_gpio_setup(void){
	
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
}


void adc_dma_setup(void){

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



void dma2_stream0_isr(void) {

    if (dma_get_interrupt_flag(DMA2, DMA_STREAM0, DMA_HTIF) != 0) {
	
	
	for(int i = 0; i < ADC_BUFFER_SIZE / 2; i++){
		dac_buffer[i] = adc_buffer[i];
	}
	dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_HTIF);
	
    }

    if (dma_get_interrupt_flag(DMA2, DMA_STREAM0, DMA_TCIF) != 0) {
		

	for(int i =  ADC_BUFFER_SIZE / 2; i < ADC_BUFFER_SIZE; i++){
		dac_buffer[i] = adc_buffer[i];
	}
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
	adc_disable_external_trigger_regular(ADC1);
	adc_set_regular_sequence(ADC1, 1, channel_seq);
	adc_set_continuous_conversion_mode(ADC1);
	adc_eoc_after_group(ADC1);
	adc_disable_eoc_interrupt(ADC1);
	adc_enable_dma(ADC1);
	adc_set_dma_continue(ADC1);
	adc_power_on(ADC1);	
	adc_start_conversion_regular(ADC1);
}


void adc_init(void){
	adc_gpio_setup();
	adc_dma_setup();
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