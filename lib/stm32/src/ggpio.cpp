#include <ggpio.h>

#if 0


((MODE) == GPIO_MODE_INPUT)              ||\
                            ((MODE) == GPIO_MODE_OUTPUT_PP)          ||\
                            ((MODE) == GPIO_MODE_OUTPUT_OD)          ||\
                            ((MODE) == GPIO_MODE_AF_PP)              ||\
                            ((MODE) == GPIO_MODE_AF_OD)              ||\
                            ((MODE) == GPIO_MODE_IT_RISING)          ||\
                            ((MODE) == GPIO_MODE_IT_FALLING)         ||\
                            ((MODE) == GPIO_MODE_IT_RISING_FALLING)  ||\
                            ((MODE) == GPIO_MODE_EVT_RISING)         ||\
                            ((MODE) == GPIO_MODE_EVT_FALLING)        ||\
                            ((MODE) == GPIO_MODE_EVT_RISING_FALLING) ||\
                            ((MODE) == GPIO_MODE_ANALOG)
#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_SPEED_FREQ_LOW)  || ((SPEED) == GPIO_SPEED_FREQ_MEDIUM) || \
                              ((SPEED) == GPIO_SPEED_FREQ_HIGH) || ((SPEED) == GPIO_SPEED_FREQ_VERY_HIGH))
#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || ((PULL) == GPIO_PULLUP) || \
                            ((PULL) == GPIO_PULLDOWN))
#define  GPIO_NOPULL        0x00000000U   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        0x00000001U   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      0x00000002U   /*!< Pull-down activation                */


typedef struct
{
  __IO uint32_t ACR;      /*!< FLASH access control register,   Address offset: 0x00 */
  __IO uint32_t KEYR;     /*!< FLASH key register,              Address offset: 0x04 */
  __IO uint32_t OPTKEYR;  /*!< FLASH option key register,       Address offset: 0x08 */
  __IO uint32_t SR;       /*!< FLASH status register,           Address offset: 0x0C */
  __IO uint32_t CR;       /*!< FLASH control register,          Address offset: 0x10 */
  __IO uint32_t OPTCR;    /*!< FLASH option control register ,  Address offset: 0x14 */
  __IO uint32_t OPTCR1;   /*!< FLASH option control register 1, Address offset: 0x18 */
} FLASH_TypeDef;

/**
  * @brief General Purpose I/O
  */

typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;
                            
#endif

#ifdef __cplusplus
extern "C" {
#endif

int gpio_readpin(GPIO_TypeDef *g, int pin) {
    if( pin < 0 || pin > 15) return -1;

    return (g->IDR & (1 << pin))? 1 : 0;
}

int exti_irqn(int exti) {
    switch(exti) {
    case 0: return EXTI0_IRQn;
    case 1: return EXTI1_IRQn;   
    case 2: return EXTI2_IRQn;
    case 3: return EXTI3_IRQn;    
    case 4: return EXTI4_IRQn;
    case 5:
    case 6:
    case 7:    
    case 8:
    case 9: return EXTI9_5_IRQn;   
    case 10: 
    case 11:    
    case 12:
    case 13:    
    case 14:
    case 15: return EXTI15_10_IRQn;
    default:;
    }
    return -1; 
} 

#if 0
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Callback(GPIO_Pin);
  }
}
#endif

int right_bitno( uint16_t val ) {
    for( int i=0; i < 16; i++, val >>= 1 ) {
        if( val & 1) return i;
    }
    return -1;
}

void (*_exti_callback)(uint16_t bits) = 0;

void exti_handler(uint16_t GPIO_Pin) {
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET) { // (EXTI->PR & (__EXTI_LINE__)
        uint16_t fl = EXTI->PR & (GPIO_Pin);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);         // (EXTI->PR = (__EXTI_LINE__))
        
        if( _exti_callback ) {
//            _exti_callback( GPIO_Pin );
            _exti_callback( fl );
        }
    }    
}

void HAL_GPIO_EXTI0_IRQHandler(void) {
    exti_handler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI1_IRQHandler(void) {
    exti_handler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI2_IRQHandler(void) {
    exti_handler(GPIO_PIN_2);
}

void HAL_GPIO_EXTI3_IRQHandler(void) {
    exti_handler(GPIO_PIN_3);
}

void HAL_GPIO_EXTI4_IRQHandler(void) {
    exti_handler(GPIO_PIN_4);
}

void HAL_GPIO_EXTI9_5_IRQHandler(void) {
  exti_handler( GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 );
}

void HAL_GPIO_EXTI15_10_IRQHandler(void) {
    exti_handler( GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 );
}

#ifdef __cplusplus
}

gwgpio::gwgpio(GPIO_TypeDef *g,uint16_t mask,uint32_t mode, int pullup, int speed) {
    this->port = g;
    this->pins = mask;

    _flag = 0;
    _mode = mode;
    _speed = (speed == 0 || speed == 1 || speed == 3)? speed : 2 ;
    _pullup = (pullup == 1 || pullup == 2)? pullup: 0;

    // init();
}

gwgpio::gwgpio() {
    this->port = 0;
    this->pins = 0;
    _flag = 0;
}

gwgpio::~gwgpio() {
    // 
    this->port = 0;
    this->pins = 0;

    if( _mode == eGPIO_EXTI_RISING_FALLING || _mode == eGPIO_EXTI_RISING  || _mode == eGPIO_EXTI_FALLING ) {
        detach();
    }

    _inited = 0;
}

void gwgpio::init(GPIO_TypeDef *g,uint16_t mask,uint32_t mode, int pullup, int speed) {
    this->port = g;
    this->pins = mask;

    _flag = 0;
    _mode = mode;
    _speed = (speed == 0 || speed == 1 || speed == 3)? speed : 2 ;
    _pullup = (pullup == 1 || pullup == 2)? pullup: 0;

    init();
}

void gwgpio::init() {
    GPIO_InitTypeDef gi;

    if( IS_GPIO_ALL_INSTANCE(this->port)) {
        if( IS_GPIO_ALL_INSTANCE(GPIOA) && (this->port == GPIOA) ) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOB) && (this->port == GPIOB )) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOC) && (this->port == GPIOC )) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOD) && (this->port == GPIOD )) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOE) && (this->port == GPIOE )) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
        else if( IS_GPIO_ALL_INSTANCE(GPIOH) && (this->port == GPIOH )) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    } else {
        return ;
    }

    uint32_t mode = 0;
    switch(_mode) {
        case eGPIO_OUTPP:
            mode = MODE_OUTPUT | OUTPUT_PP; // GPIO_MODE_OUTPUT_PP
            break;
        case eGPIO_OUTOD:
            mode = MODE_OUTPUT | OUTPUT_OD; // GPIO_MODE_OUTPUT_OD
            break;
        case eGPIO_INPUT:
            mode = MODE_INPUT; // GPIO_MODE_INPUT
            break;
        case eGPIO_ADC:
            mode = MODE_ANALOG; // GPIO_MODE_ANALOG
            break;
        case eGPIO_EXTI_RISING:
            mode = MODE_INPUT | EXTI_IT | TRIGGER_RISING; // GPIO_MODE_IT_RISING
            break;
        case eGPIO_EXTI_FALLING:
            mode = MODE_INPUT | EXTI_IT | TRIGGER_FALLING; // GPIO_MODE_IT_FALLING
            break;
        case eGPIO_EXTI_RISING_FALLING:
            mode = MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING; // GPIO_MODE_IT_RISING_FALLING
            break;
        default:;
    }

    gi.Pin = this->pins;
    gi.Mode = mode;
    gi.Speed = _speed;
    gi.Pull = _pullup;

    HAL_GPIO_Init( this->port, &gi );

    if( mode | EXTI_IT ) {
        uint16_t p = this->pins;
        for(int i=0; i < 16; i++, p >>=1 ) {
            if( !(p & 1) ) continue; // not pin
            int irqn = exti_irqn(i);
            if( irqn < 0 ) continue; // error
            
            HAL_NVIC_SetPriority( (IRQn_Type)irqn, 0, 0 );
            HAL_NVIC_EnableIRQ( (IRQn_Type)irqn );
        }
    }
    _inited = 1;
}

void gwgpio::attach( void (*extif)(uint16_t)) {
    _exti_callback = extif;
}

void gwgpio::detach() {
    _exti_callback = 0;
}

int gwgpio::read() {
    if( ! isinit() || _mode != eGPIO_INPUT ) return -1;
    
    return (this->port->IDR & this->pins);
}


int gwgpio::write(uint16_t val) {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return -1;
    
    val &= this->pins;
    this->port->ODR = (this->port->ODR & ~(this->pins) | val);
    
    return val;
}

void gwgpio::toggle() {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return;
    
    uint16_t cval = this->port->ODR & this->pins;
    cval = ~cval & this->pins;
    this->port->ODR = (this->port->ODR & ~(this->pins)) | cval;
}

// specific one pin

int gwgpio::read(int pin) {
    if( ! isinit() || (this->_mode != eGPIO_INPUT) ) return -1;
    if( pin < 0 || pin < 15 ) return -1;
    if( ( ((1 << pin) & this->pins) == 0 ) ) return -1;

    return (this->port->IDR & (1 << pin))? 1 : 0;
}

int gwgpio::write(int pin, int val) {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return -1;
    if( pin < 0 || pin < 15 ) return -1;
    if( ( ((1 << pin) & this->pins) == 0 ) ) return -1;

    uint16_t v = this->port->ODR & ~(1<<pin);
    if( val ) v |= (1 << pin);
    this->port->ODR = v;

    return (v)? 1 : 0;
}

void gwgpio::high( int pin ) {
    this->write(pin, 1);
}

void gwgpio::low( int pin ) {
    this->write(pin, 0);
}

void gwgpio::toggle(int pin) {
    if( ! isinit() || (this->_mode != eGPIO_OUTPP && this->_mode != eGPIO_OUTOD) ) return;
    if( pin < 0 || pin < 15 ) return;
    if( ( ((1 << pin) & this->pins) == 0 ) ) return;
#if 0
    uint16_t v = this->gpio->ODR & (1 << pin); 

    if( v ) {
        this->port->ODR &= (uint16_t)~(1 << pin);
    } else {
        this->port->ODR |= (uint16_t)(1 << pin);
    }
    return (v)? 1 : 0;
#else
    uint32_t v = 1 << pin;
    uint32_t odr = this->port->ODR;
    this->port->BSRR = ((odr & v) << pin) | ( ~odr & v);
#endif    
}

#endif // __cplusplus