<<<<<<< HEAD
#ifndef __GADC_H__
#define __GADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32yyxx_hal_conf.h>
//#include <stm32yyxx_ll_adc.h>
//#include <stm32yyxx_ll_gpio.h>
//#include <stm32yyxx_ll_rcc.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_adc.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_dma.h>

extern void (*_stm32adc_callback)();

#ifdef __cplusplus
}
#endif

struct adc_channels {
  int ch;
  int sampling; // default 

  GPIO_TypeDef *port;
  int pin;

  int rank;
};

enum { // adc mode
    eADC_POLLING = 0,
    eADC_INTERRUPT,
    eADC_DMA,
    eADC_DMAINTERRUPT
};

enum { // adc error
    eADC_NOTSETUP = -1,
    eADC_NOK = -1,
    eADC_OK = 0,

};

class stm32adc {
private:
protected:
  ADC_HandleTypeDef _ha; // ->Instance (ADC_TypeDef *adc);
  ADC_ChannelConfTypeDef _ac;
  struct adc_channels *_chs;  
  uint16_t _mode;
  int _status; 
  uint16_t _timeout;
  
  int add_channel(adc_channels *ac); // return channels;
  void conv();
  //inline ADC_HandleTypeDef *get_handle() { return &_ha; }
public:
  int _channel_nr;

public:
  stm32adc();
  stm32adc(ADC_TypeDef *adc, int chs, struct adc_channels *ac);
  virtual ~stm32adc(){};
  
  void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac);

  void start(int timeout=-1);
  void stop();

  bool isready() { return (_status != eADC_NOTSETUP);}
  int channel_nr() { return _channel_nr; };
  int read();
};

class stm32adcint : public stm32adc {
protected:
    void (*_intrf)();
public:
    stm32adcint();
    stm32adcint(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)());
    ~stm32adcint();
    void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)());

    void attach_intr( void (*intrf)() );
    void detach_intr();

    int read();
};

class stm32adcdma : public stm32adcint {
protected:
public:
    uint16_t *_dmabuf;
public:
    stm32adcdma();
    stm32adcdma(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);
    ~stm32adcdma();
    
    void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);

    int read(uint16_t **bp);
};

#endif // __GADC_H__

#if 0
stm32adc adc(ADC1,0);
//adc.start()
while(1) {
    val = adc.read();
}

=======
#ifndef __GADC_H__
#define __GADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32yyxx_hal_conf.h>
//#include <stm32yyxx_ll_adc.h>
//#include <stm32yyxx_ll_gpio.h>
//#include <stm32yyxx_ll_rcc.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_adc.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_dma.h>

extern void (*_stm32adc_callback)();

#ifdef __cplusplus
}
#endif

struct adc_channels {
  int ch;
  int sampling; // default 

  GPIO_TypeDef *port;
  int pin;

  int rank;
};

enum { // adc mode
    eADC_POLLING = 0,
    eADC_INTERRUPT,
    eADC_DMA,
    eADC_DMAINTERRUPT
};

enum { // adc error
    eADC_NOTSETUP = -1,
    eADC_NOK = -1,
    eADC_OK = 0,

};

class stm32adc {
private:
protected:
  ADC_HandleTypeDef _ha; // ->Instance (ADC_TypeDef *adc);
  ADC_ChannelConfTypeDef _ac;
  struct adc_channels *_chs;  
  uint16_t _mode;
  int _status; 
  uint16_t _timeout;
  
  int add_channel(adc_channels *ac); // return channels;
  void conv();
  //inline ADC_HandleTypeDef *get_handle() { return &_ha; }
public:
  int _channel_nr;

public:
  stm32adc();
  stm32adc(ADC_TypeDef *adc, int chs, struct adc_channels *ac);
  virtual ~stm32adc(){};
  
  void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac);

  void start(int timeout=-1);
  void stop();

  bool isready() { return (_status != eADC_NOTSETUP);}
  int channel_nr() { return _channel_nr; };
  int read();
};

class stm32adcint : public stm32adc {
protected:
    void (*_intrf)();
public:
    stm32adcint();
    stm32adcint(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)());
    ~stm32adcint();
    void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)());

    void attach_intr( void (*intrf)() );
    void detach_intr();

    int read();
};

class stm32adcdma : public stm32adcint {
protected:
public:
    uint16_t *_dmabuf;
public:
    stm32adcdma();
    stm32adcdma(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);
    ~stm32adcdma();
    
    void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);

    int read(uint16_t **bp);
};

#endif // __GADC_H__

#if 0
stm32adc adc(ADC1,0);
//adc.start()
while(1) {
    val = adc.read();
}

>>>>>>> 8cc7fef95d838e9e191f2e03980cbadb5dd857b4
#endif