#ifndef WS2812_H
#define WS2812_H


typedef union {
      struct __attribute__ ((__packed__)) {
	uint8_t _unused;
	uint8_t b;
	uint8_t r;
	uint8_t g;
      } colors;
} ws2812_led_t;


void ws2812_clear(ws2812_led_t *leds, int led_count);
void ws2812_send(ws2812_led_t *leds, int led_count);
bool ws2812_ready(void);
void ws2812_init(void);
#endif /* WS2812_H */