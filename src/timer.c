#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>

#include "timer.h"

void adc_dac_timer_init(void);

void ws2812_timer_init(void){

	rcc_periph_clock_enable(RCC_TIM3);
    	rcc_periph_reset_pulse(RST_TIM3);
    	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    	timer_set_prescaler(TIM3, 0);
    	timer_continuous_mode(TIM3);
    	timer_set_period(TIM3, 104); /* 168000000 / 2 / 800000 (800khz pwm) */
    	timer_disable_oc_output(TIM3, TIM_OC1);
    	timer_disable_oc_clear(TIM3, TIM_OC1);
    	timer_enable_oc_preload(TIM3, TIM_OC1);
    	timer_set_oc_slow_mode(TIM3, TIM_OC1);
    	timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);
    	timer_set_oc_polarity_high(TIM3, TIM_OC1);
    	timer_set_oc_value(TIM3, TIM_OC1, 0);
    	timer_enable_oc_output(TIM3, TIM_OC1);
    	timer_enable_preload(TIM3);

    	timer_enable_irq(TIM3, TIM_DIER_UDE);

	timer_enable_counter(TIM3);

}



void adc_dac_timer_init(void){

	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_reset_pulse(RST_TIM2);
	/* Timer global mode: - No divider, Alignment edge, Direction up */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_continuous_mode(TIM2);
	timer_set_prescaler(TIM2, 0);
	timer_set_period(TIM2, 100);
	timer_enable_oc_output(TIM2, TIM_OC1);
	timer_disable_oc_clear(TIM2, TIM_OC1);
	timer_disable_oc_preload(TIM2, TIM_OC1);
	timer_set_oc_slow_mode(TIM2, TIM_OC1);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_TOGGLE);
	timer_set_oc_value(TIM2, TIM_OC1, 50);
	timer_disable_preload(TIM2);
	/* Set the timer trigger output (for the DAC) to the channel 1 output
	   compare */
	timer_set_master_mode(TIM2, TIM_CR2_MMS_COMPARE_OC1REF);
	timer_enable_counter(TIM2);


}

void adc_timer_init(void){
	adc_dac_timer_init();
}