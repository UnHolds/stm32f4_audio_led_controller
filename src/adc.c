#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/adc.h>

#include <libopencm3/stm32/dac.h>

#include "adc.h"
#include "dma.h"
#include "isr.h"
#include "gpio.h"
#include "adc_var.h"
#include "timer.h"
#include "filter.h"
#include "dac_var.h"



uint16_t adc_buffer1[ADC_BUFFER_SIZE];
uint16_t adc_buffer2[ADC_BUFFER_SIZE];
uint8_t channel_seq_1[1];
uint8_t channel_seq_2[1];
bool buffer_ready = false;
bool buffer_high = false;
bool buffer_low = false;

uint16_t dac_buffer_position = 0;

void adc_gpio_setup(void);
void adc_setup(void);
void adc_dma_setup(void);




void dma2_str0_isr(void) {

    if (dma_get_interrupt_flag(DMA2, DMA_STREAM0, DMA_HTIF) != 0) {
	
	buffer_high = false;
	
	ADC_HTIF_filter_isr();

	
	dac_buffer_position = 0;
	buffer_ready = true;
	buffer_low = true;

	dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_HTIF);
	
    }

    if (dma_get_interrupt_flag(DMA2, DMA_STREAM0, DMA_TCIF) != 0) {
	
	buffer_low = false;		

	ADC_TCIF_filter_isr();

	
	dac_buffer_position = DAC_BUFFER_SIZE / 2;
	buffer_high = true;

	dma_clear_interrupt_flags(DMA2, DMA_STREAM0, DMA_TCIF);
    }
}




void adc_isr(void){

	if(adc_get_flag(ADC1, ADC_SR_EOC) == 0){ // <-- doesn't work ? 
		if(buffer_ready){

			if((buffer_low == true && dac_buffer_position < DAC_BUFFER_SIZE / 2) || (buffer_high == true && dac_buffer_position < DAC_BUFFER_SIZE)){

				//(uint16_t) adc_read_regular(ADC1)
				//dac_load_data_buffer_single((uint16_t) adc_read_regular(ADC1), RIGHT12, CHANNEL_1);
				dac_load_data_buffer_single(dac_buffer[dac_buffer_position], RIGHT12, CHANNEL_1);
				dac_load_data_buffer_single(adc_buffer2[dac_buffer_position], RIGHT12, CHANNEL_2);//only for L R support
				dac_buffer_position++;
				dac_software_trigger(CHANNEL_1);
				dac_software_trigger(CHANNEL_2);

			}
		}
		adc_clear_flag(ADC1, ADC_SR_EOC);
	}
	
}



void adc_setup(void){

	channel_seq_1[0] = 0;
	channel_seq_2[0] = 1;

	nvic_set_priority(NVIC_ADC_IRQ, 0);
    	nvic_enable_irq(NVIC_ADC_IRQ);

	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_clock_enable(RCC_ADC2);
	
	rcc_periph_reset_pulse(RST_ADC);
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_144CYC);
	adc_set_resolution(ADC1, ADC_CR1_RES_12BIT);
	//adc_disable_external_trigger_regular(ADC1); //
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_TIM2_CC2, ADC_CR2_EXTEN_RISING_EDGE);
	//adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_TIM2_TRGO, ADC_CR2_EXTEN_RISING_EDGE);
	adc_set_regular_sequence(ADC1, 1, channel_seq_1);
	//adc_set_continuous_conversion_mode(ADC1);//
	adc_eoc_after_each(ADC1);
	//adc_eoc_after_group(ADC1);
	//adc_disable_eoc_interrupt(ADC1);
	adc_enable_eoc_interrupt(ADC1);
	adc_enable_dma(ADC1);
	adc_set_dma_continue(ADC1);
	adc_power_on(ADC1);	


	adc_power_off(ADC2);
	adc_disable_scan_mode(ADC2);
	adc_set_right_aligned(ADC2);
	adc_set_sample_time(ADC2, ADC_CHANNEL1, ADC_SMPR_SMP_144CYC);
	adc_set_resolution(ADC2, ADC_CR1_RES_12BIT);
	adc_enable_external_trigger_regular(ADC2, ADC_CR2_EXTSEL_TIM2_CC2, ADC_CR2_EXTEN_RISING_EDGE);
	adc_set_regular_sequence(ADC2, 1, channel_seq_2);
	//adc_eoc_after_each(ADC2);
	adc_eoc_after_group(ADC2);
	adc_disable_eoc_interrupt(ADC2);
	//adc_enable_eoc_interrupt(ADC2);
	adc_enable_dma(ADC2);
	adc_set_dma_continue(ADC2);
	adc_power_on(ADC2);	


	
}


void adc_init(void){
	adc_gpio_init();
	adc_timer_init();
	adc_dma_init();
	adc_setup();

	adc_start_conversion_regular(ADC1);
	adc_start_conversion_regular(ADC2);

	dma_enable_stream(DMA2, DMA_STREAM0);
	dma_enable_stream(DMA2, DMA_STREAM3);

}