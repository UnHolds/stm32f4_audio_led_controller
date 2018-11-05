#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/dma.h>
#include <libopencmsis/core_cm3.h>

#include "ws2812.h"
#include "adc.h"
#include "dac.h"
#include "gpio.h"

#define LED_COUNT (300)


ws2812_led_t leds[LED_COUNT];



void clock_setup(void);



void clock_setup(void){
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

//DATA PIN = PD12


int main(void)
{
    	clock_setup();
	status_leds_init();
    	
	dac_init();
	adc_init();


	ws2812_init();

	
	int curr_addr = 0;
	int past_addr = 300;

	int r = 255;
	int g = 0;
	int b = 0;
	
	while (1) {


		if(r == 255 && g < 255 && b == 0){
			g++;
		}else if(r > 0 && g == 255 && b == 0){
			r--;
		}else if(r == 0 && g == 255 && b < 255){
			b++;
		}else if(r == 0 && g > 0 && b == 255){
			g--;
		}else if(r < 255 && g == 0 && b == 255){
			r++;
		}else if(r == 255 && g == 0 && b > 0){
			b--;
		}
        	
		
		past_addr = curr_addr;

		if(curr_addr == 300){
			curr_addr = 0;
		}else{
			curr_addr++;
		}


		leds[curr_addr].colors.r = r;
		leds[curr_addr].colors.g = g;
		leds[curr_addr].colors.b = b;
		
		leds[past_addr].colors.r = 0;
		leds[past_addr].colors.g = 0;
		leds[past_addr].colors.b = 0;
		

		
		//for(int i = 0; i < 70; i++){
		//	leds[i].colors.r = r;
		//	leds[i].colors.g = g;
		//	leds[i].colors.b = b;
		//}

		
		

		ws2812_send(leds, LED_COUNT);

		
    	}
	
	
	
	while(1){
		__asm__("nop");
	}
   
    	return 0;
}