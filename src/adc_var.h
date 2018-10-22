#ifndef ADC_VAR_H
#define ADC_VAR_H

#define ADC_BUFFER_SIZE 16 //must be a muliple by 2 and the same as DAC_BUFFEr_SIZE
extern uint16_t adc_buffer1[ADC_BUFFER_SIZE];
extern uint16_t adc_buffer2[ADC_BUFFER_SIZE];
#endif /* ADC_VAR_H */