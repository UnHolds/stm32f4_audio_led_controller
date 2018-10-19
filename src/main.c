#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/dma.h>
#include <libopencmsis/core_cm3.h>

#include "ws2812.h"
#include "adc.h"

#define LED_COUNT (300)


ws2812_led_t leds[LED_COUNT];



void clock_setup(void);



void clock_setup(void){
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

//DATA PIN = PD12


int main(void)
{
    	//clock_setup();
    	//ws2812_init();

	/*
	int curr_addr = 0;
	int past_addr = 300;


	while (1) {

		//for(int i = 0; i < 1000; i++){
		//	__asm__("nop");
		//}
        	
		past_addr = curr_addr;

		if(curr_addr == 300){
			curr_addr = 0;
		}else{
			curr_addr++;
		}

		leds[curr_addr].colors.b = 255;
		leds[past_addr].colors.b = 0;
	
	

		ws2812_send(leds, LED_COUNT);
		
    	}
	*/

	adc_init();

	while(1){
		__asm__("nop");
	}
   
    	return 0;
}