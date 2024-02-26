#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_rcc.h>	
#include <stm32f4xx_hal_gpio.h>	
#include <stm32f4xx_hal_exti.h>	

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pins;
} wgpio_t;

typedef struct {
	GPIO_TypeDef *port;
	int pin;
} gpio_t;

typedef enum { eGPIO_OUTPP=0, eGPIO_OUTOD, eGPIO_INPUT, eGPIO_ADC, eGPIO_EXTI_RISING, eGPIO_EXTI_FALLING, eGPIO_EXTI_RISING_FALLING } egpio_type;

#ifdef __cplusplus
extern "C" {
#endif

//#pragma anon_unions

int gpio_readpin(GPIO_TypeDef *g, int pin);
int exti_irqn(int exti);

#ifdef __cplusplus
}

class gwgpio : public wgpio_t {
	union {
		uint16_t        _flag; // b0:1 inited, b1-b3 : 0:out, 1:in, 2:in/out,3:adc,4:exti,5:AF(timer,uart,...)
		struct {
			uint16_t	_inited:1; // 0: not, 1: inited
			uint16_t	_mode:4;   // egpio_type;
			uint16_t	_pullup:2; // 0: no , 1:pull up, 2: pull down
			uint16_t	_speed:2;  // 0: slow, 1: medium, 2: fast, 3: very fast
			uint16_t	_reserved:7;
		};
	};

protected:

public:
	gwgpio(GPIO_TypeDef *g,uint16_t mask,uint32_t mode = eGPIO_OUTPP, int pullup = 1, int speed = 2);
	gwgpio();
	virtual ~gwgpio();

	void init(GPIO_TypeDef *g,uint16_t mask,uint32_t mode = eGPIO_OUTPP, int pullup = 1, int speed = 2);
	void init();
	inline bool isinit() { return (_inited) ? true : false;}
	int mode(int mode=-1);
	// 0:out, 1:in, 2:x 3:adc,4:exti,5:AF(timer,uart,...)
	int pins_nr();

// read function
	int read();
	int read(int pin); 

// output functin -- pin is referenced pins
	void high(int pin);
	void low(int pin);
	void toggle(int pin);
	int write(int pin, int val);

// write 
	int write(uint16_t data);
	int write(uint32_t mask, uint16_t data);

	void toggle();

	void attach( void (*extif)(uint16_t));
	void detach();

// operator
	gwgpio &operator=(wgpio_t&);
	gwgpio &operator=(gpio_t&);
};

#endif // __cplusplus

#endif // __GPIO_H
