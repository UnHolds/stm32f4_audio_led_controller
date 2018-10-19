#ifndef WS2812_VAR_H
#define WS2812_VAR_H

//Sets the size of the buffer (each LED need 24Bits)
#define LEDS_BUFFER_SIZE (24 * 100)

//This bit buffer contains the threshold values for the PWM (Timer)
extern uint16_t bit_buffer[LEDS_BUFFER_SIZE];

#endif /* WS2812_VAR_H */