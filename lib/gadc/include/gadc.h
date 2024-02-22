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
  int add_channel(uint32_t ch); // Vref, temp, 
  void conv();
  //inline ADC_HandleTypeDef *get_handle() { return &_ha; }
public:
  int _channel_nr;

public:
  stm32adc();
  stm32adc(ADC_TypeDef *adc, int chs, struct adc_channels *ac);
  virtual ~stm32adc(){};
  
  void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac);
  void setup();

  void start(int timeout=-1);
  void stop();

  bool isready() { return (_status != eADC_NOTSETUP);}
  int channel_nr() { return _channel_nr; };
  uint16_t& mode() { return _mode; }
  ADC_HandleTypeDef *get_handle() { return &_ha; }

  int read();
};

class stm32adcint : public stm32adc {
protected:
public:
public:
    stm32adcint();
    stm32adcint(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)());
    ~stm32adcint();
    void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)());
    void setup();

    void attach( void (*intrf)() );
    void detach();
    void start();

    int read();
};

class stm32adcdma : public stm32adcint {
protected:
    uint16_t *_dmabuf;
    uint16_t *get_buffer() { return _dmabuf;};
public:
public:
    stm32adcdma();
    stm32adcdma(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);
    ~stm32adcdma();
    
    void setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf);
    void setup();
    void start();

    int read(uint16_t *bp, uint16_t bsize);
    uint16_t *read();

};

#endif // __GADC_H__

