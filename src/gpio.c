#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "gpio.h"


void GPIOA_init(void);
void GPIOB_init(void);
void GPIOC_init(void);
void GPIOD_init(void);

bool GPIOA_initialized = false;
bool GPIOB_initialized = false;
bool GPIOC_initialized = false;
bool GPIOD_initialized = false;




void GPIOA_init(void){
	if(GPIOA_initialized == false){
		rcc_periph_clock_enable(RCC_GPIOA);	
		GPIOA_initialized = true;
	}
}

void GPIOB_init(void){
	if(GPIOB_initialized == false){
		rcc_periph_clock_enable(RCC_GPIOB);
		GPIOB_initialized = true;
	}
}

void GPIOC_init(void){
	if(GPIOC_initialized == false){
		rcc_periph_clock_enable(RCC_GPIOC);
		GPIOC_initialized = true;
	}
}

void GPIOD_init(void){
	if(GPIOD_initialized == false){
		rcc_periph_clock_enable(RCC_GPIOD);
		GPIOD_initialized = true;
	}
}


void status_leds_init(void){

	GPIOA_init();

	//LEDS
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
}

void ws2812_gpio_init(void){

	GPIOC_init();

	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
	gpio_set_af(GPIOC, GPIO_AF2, GPIO6);
}



void adc_gpio_init(void){
	
	GPIOA_init();
	
	//ADC
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

}


void dac_gpio_init(void){
	
	GPIOA_init();

	//DAC
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO5);
}