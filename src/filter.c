#include <libopencm3/stm32/rcc.h>

#include "adc_var.h"
#include "dac_var.h"
#include "filter.h"


void ADC_HTIF_filter_isr(void){
	
	for(int i = 0; i < ADC_BUFFER_SIZE / 2; i++){
		dac_buffer[i] = adc_buffer1[i];
	}

}


void ADC_TCIF_filter_isr(void){
	
	for(int i = ADC_BUFFER_SIZE / 2; i < ADC_BUFFER_SIZE; i++){
		dac_buffer[i] = adc_buffer1[i];
	}

}