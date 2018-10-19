#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "gpio.h"

void ws2812_gpio_init(void){
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
	gpio_set_af(GPIOC, GPIO_AF2, GPIO6);
}



void adc_gpio_init(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	
	//ADC
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);

	//LEDS
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);

	//DAC
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);
}



void dac_gpio_init(void){

}