#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>

#include "ws2812.h"

//Sets the size of the buffer (each LED need 24Bits)
#define LEDS_BUFFER_SIZE (24 * 100)


//Dead time after all logical highs and lows are send to reset all led. (Make Leds ready for the next transmission)
#define LED_DEAD_TIME (2)


//This bit buffer contains the threshold values for the PWM (Timer)
uint16_t bit_buffer[LEDS_BUFFER_SIZE];

//LEDS_BUFFER_SIZE * clear_cycles ZEROS will be send out by the controller to turn off all leds at the beginning.
// LEDS put out = (LEDS_BUFFER_SIZE / 24 * clear_cyles) [Each led has 24 Bits  8 Red, 8 Blue, 8 Green]
uint16_t clear_cycles = 20;
uint16_t count_clear_cycles = 0;

enum led_stage {
	led_idle,
	led_sending,
	led_clear,
	led_done
};

enum dma_stage {
	dma_ready,
	dma_busy
};

volatile enum led_stage led_stage;
volatile enum dma_stage dma_stage;

struct ws2812 {
	ws2812_led_t *leds;
	int led_count;
	int leds_sent;
} ws2812;

//Electric
void pwm_setup(void);
void dma_setup(void);
void dma_start(void);
void dma_stop(void);

//Misc Funktions
void fill_buffer(bool b_high);
void fill_low_buffer(void);
void fill_high_buffer(void);
void fill_full_buffer(void);
int get_timing_value(int n);

void ws2812_clear_priv(void);

void pwm_setup(void) {

	rcc_periph_clock_enable(RCC_TIM4);
    	rcc_periph_reset_pulse(RST_TIM4);
    	timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    	timer_set_prescaler(TIM4, 0);
    	timer_continuous_mode(TIM4);
    	timer_set_period(TIM4, 104); /* 168000000 / 2 / 800000 (800khz pwm) */
    	timer_disable_oc_output(TIM4, TIM_OC1);
    	timer_disable_oc_clear(TIM4, TIM_OC1);
    	timer_enable_oc_preload(TIM4, TIM_OC1);
    	timer_set_oc_slow_mode(TIM4, TIM_OC1);
    	timer_set_oc_mode(TIM4, TIM_OC1, TIM_OCM_PWM1);
    	timer_set_oc_polarity_high(TIM4, TIM_OC1);
    	timer_set_oc_value(TIM4, TIM_OC1, 0);
    	timer_enable_oc_output(TIM4, TIM_OC1);
    	timer_enable_preload(TIM4);

    	timer_enable_irq(TIM4, TIM_DIER_UDE);

	timer_enable_counter(TIM4);
	
}


void dma_setup(void) {



	rcc_periph_clock_enable(RCC_DMA1);
    	nvic_enable_irq(NVIC_DMA1_STREAM6_IRQ);
	dma_stream_reset(DMA1, DMA_STREAM6);
    	dma_set_priority(DMA1, DMA_STREAM6, DMA_SxCR_PL_VERY_HIGH);
    	dma_set_memory_size(DMA1, DMA_STREAM6, DMA_SxCR_MSIZE_16BIT);
    	dma_set_peripheral_size(DMA1, DMA_STREAM6, DMA_SxCR_PSIZE_16BIT);
    	dma_enable_circular_mode(DMA1, DMA_STREAM6);
    	dma_enable_memory_increment_mode(DMA1, DMA_STREAM6);
    	dma_set_transfer_mode(DMA1, DMA_STREAM6, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    	dma_set_peripheral_address(DMA1, DMA_STREAM6, (uint32_t)&TIM4_CCR1);
    	dma_set_memory_address(DMA1, DMA_STREAM6, (uint32_t)(&bit_buffer[0]));
    	dma_set_number_of_data(DMA1, DMA_STREAM6, LEDS_BUFFER_SIZE);
    	dma_enable_half_transfer_interrupt(DMA1, DMA_STREAM6);
    	dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM6);
    	dma_channel_select(DMA1, DMA_STREAM6, DMA_SxCR_CHSEL_2);
    	nvic_clear_pending_irq(NVIC_DMA1_STREAM6_IRQ);
    	nvic_set_priority(NVIC_DMA1_STREAM6_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_STREAM6_IRQ);
	
	dma_stage = dma_ready;

}	


void dma_stop(void) {
	timer_set_oc_value(TIM4, TIM_OC1, 0);
	dma_disable_stream(DMA1, DMA_STREAM6);
	timer_set_oc_value(TIM4, TIM_OC1, 0);
	
	dma_stage = dma_ready;
	led_stage = led_idle;
}


void dma_start(void) {
	dma_stage = dma_busy;
	dma_enable_stream(DMA1, DMA_STREAM6);
	
}



void dma1_stream6_isr(void) {

    if (dma_get_interrupt_flag(DMA1, DMA_STREAM6, DMA_HTIF) != 0) {
        
	
	if(ws2812.leds_sent < (ws2812.led_count + LED_DEAD_TIME) && led_stage == led_sending){
		fill_low_buffer();
	}else if(ws2812.leds_sent >= (ws2812.led_count + LED_DEAD_TIME) && led_stage == led_sending){
		led_stage = led_done;
	}

	if(led_stage == led_done){
		dma_stop();
	}
	dma_clear_interrupt_flags(DMA1, DMA_STREAM6, DMA_HTIF);
    }

    if (dma_get_interrupt_flag(DMA1, DMA_STREAM6, DMA_TCIF) != 0) {
        

	if(ws2812.leds_sent < (ws2812.led_count + LED_DEAD_TIME) && led_stage == led_sending){
		fill_high_buffer();
	}else if(ws2812.leds_sent >= (ws2812.led_count + LED_DEAD_TIME) && led_stage == led_sending){
		led_stage = led_done;
	}

	if(led_stage == led_done){
		dma_stop();
	}

	if(count_clear_cycles < clear_cycles){
		count_clear_cycles++;
	}else if(led_stage == led_clear){
		dma_stop();
	}
	dma_clear_interrupt_flags(DMA1, DMA_STREAM6, DMA_TCIF);	
    }
}


//75 HIGH 
//29 LOW




//LED FUNKTIONS


void ws2812_clear(ws2812_led_t *leds, int led_count) {

	for(int i = 0; i < led_count; i++){
		leds[i].colors.r = 0;
		leds[i].colors.g = 0;
		leds[i].colors.b = 0;
	}

	ws2812_clear_priv();
}


void ws2812_clear_priv(){

	while(dma_stage != dma_ready || led_stage != led_idle){
		__asm__("nop");		
	}

	for(int i = 0; i < LEDS_BUFFER_SIZE; i++) {
		bit_buffer[i] = 29;
	}

	count_clear_cycles = 0;
	led_stage = led_clear;
	dma_start();

}


void ws2812_send(ws2812_led_t *leds, int led_count){

	

	while(dma_stage != dma_ready || led_stage != led_idle){
		__asm__("nop");		
	}

	ws2812.leds = leds;
	ws2812.led_count = led_count;
	ws2812.leds_sent = 0;

	fill_full_buffer();
	led_stage = led_sending;
	dma_start();
	
}




//misc functions

void fill_high_buffer(void){
	fill_buffer(true);
}

void fill_low_buffer(void){
	fill_buffer(false);
}

void fill_full_buffer(void){
	fill_buffer(false);
	fill_buffer(true);
}	

void fill_buffer(bool b_high){

	int offset = 0;

	if(b_high){
		offset = LEDS_BUFFER_SIZE / 2;
	}


	for(int i = 0; i < LEDS_BUFFER_SIZE / 48 ; i++){

		if(ws2812.leds_sent < ws2812.led_count){
			bit_buffer[i * 24 + offset +  0] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b10000000) >> 7);
			bit_buffer[i * 24 + offset +  1] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b01000000) >> 6);
			bit_buffer[i * 24 + offset +  2] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b00100000) >> 5);
			bit_buffer[i * 24 + offset +  3] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b00010000) >> 4);
			bit_buffer[i * 24 + offset +  4] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b00001000) >> 3);
			bit_buffer[i * 24 + offset +  5] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b00000100) >> 2);
			bit_buffer[i * 24 + offset +  6] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b00000010) >> 1);
			bit_buffer[i * 24 + offset +  7] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.r & 0b00000001) >> 0);

			bit_buffer[i * 24 + offset +  8] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b10000000) >> 7);
			bit_buffer[i * 24 + offset +  9] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b01000000) >> 6);
			bit_buffer[i * 24 + offset + 10] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b00100000) >> 5);
			bit_buffer[i * 24 + offset + 11] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b00010000) >> 4);
			bit_buffer[i * 24 + offset + 12] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b00001000) >> 3);
			bit_buffer[i * 24 + offset + 13] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b00000100) >> 2);
			bit_buffer[i * 24 + offset + 14] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b00000010) >> 1);
			bit_buffer[i * 24 + offset + 15] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.g & 0b00000001) >> 0);

			bit_buffer[i * 24 + offset + 16] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b10000000) >> 7);
 			bit_buffer[i * 24 + offset + 17] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b01000000) >> 6);
			bit_buffer[i * 24 + offset + 18] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b00100000) >> 5);
			bit_buffer[i * 24 + offset + 19] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b00010000) >> 4);
			bit_buffer[i * 24 + offset + 20] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b00001000) >> 3);
			bit_buffer[i * 24 + offset + 21] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b00000100) >> 2);
			bit_buffer[i * 24 + offset + 22] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b00000010) >> 1);
			bit_buffer[i * 24 + offset + 23] = get_timing_value((ws2812.leds[ws2812.leds_sent].colors.b & 0b00000001) >> 0);
				
			
			ws2812.leds_sent++;

		}else if(ws2812.leds_sent < ws2812.led_count + LED_DEAD_TIME){

			bit_buffer[i * 24 + offset] = 0;

			ws2812.leds_sent++;			

		}else{
			bit_buffer[i * 24 + offset] = 0;	

		}


	}

}


int get_timing_value(int n){
	if(n == 1){
		return 75;

	}else{

		return 29;
	}
}

bool ws2812_ready(void){
	if(led_stage == led_idle){
		return true;
	}else{
		return false;	
	}
}


void ws2812_init(void){
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOD, GPIO_AF2, GPIO12);

	pwm_setup();
	dma_setup();
	led_stage = led_idle;

	ws2812_clear_priv();
}


/*
int main(void){

	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOD, GPIO_AF2, GPIO12);


	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);

	gpio_set(GPIOA, GPIO6);
	gpio_set(GPIOA, GPIO7);	

	pwm_setup();
	dma_setup();
	led_stage = led_idle;

	clear();



	while (1) {
		__asm__("nop");
		
	}
}

*/