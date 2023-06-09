#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_SAMPLES 0x14		// equals 20
#define DELAY_TIME 	0xFB0		// equals 4000
#define MULTIPLER 	0.795		// the needed m in y = mx + b
#define DIFF	  	23.5		// the needed b in y = mx + b
#define DELAY		0xF4240		// equals 1000000
#define TENS		0xA			// equals 10
#define UPPER_VOLT  0x3E8		// equal 1000
#define LOWER_VOLT	0x64		// equal 100

void ADC12_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */