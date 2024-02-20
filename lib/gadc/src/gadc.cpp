#include "gadc.h"

#ifdef __cplusplus
extern "C" {
#endif

void ADC_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void);

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void (*_stm32adc_callback)() = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if( _stm32adc_callback != 0 ) {
    _stm32adc_callback();
  }
}

#ifdef __cplusplus
}
#endif

stm32adc::stm32adc() {
  _ha.Instance = 0;
  _channel_nr = 0;
  _timeout = 10; // ms
  _status = eADC_NOTSETUP; // -1
  _mode = eADC_POLLING; // 0       (_mode & 0x7)
}

stm32adc::stm32adc(ADC_TypeDef *adc, int chs, struct adc_channels *ac) {
  setup(adc, chs, ac);
}

void stm32adc::setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac) {
  __HAL_RCC_ADC1_CLK_ENABLE();

  _ha.Instance = adc;
  _channel_nr = 0;
  _timeout = 1; // ms
  _status = 0;
  _mode = 0;

  _ha.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  _ha.Init.Resolution = ADC_RESOLUTION_12B;
  _ha.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  _ha.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  _ha.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  _ha.Init.NbrOfConversion = chs;                   // <> _channel_nr
  _ha.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  _ha.Init.DiscontinuousConvMode = DISABLE;
  _ha.Init.ScanConvMode = ENABLE;
  _ha.Init.ContinuousConvMode = ENABLE;

// multi, continues, use dma
  if( _mode != eADC_DMA && _mode != eADC_DMAINTERRUPT ) {
    _ha.Init.DMAContinuousRequests = DISABLE;
  } else {
    __HAL_RCC_DMA2_CLK_ENABLE();
    _ha.Init.DMAContinuousRequests = ENABLE;
  }

  if (HAL_ADC_Init(&_ha) != HAL_OK)
  {
    Error_Handler();
  }

  _chs = ac;

  for(int i=0; i < _ha.Init.NbrOfConversion; i++) {
    add_channel(ac+i);
  }

#if 0
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;

  __HAL_RCC_DMA2_CLK_ENABLE();
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
  
  HAL_ADC_Start_IT(&hadc1); // _ha.Init.ContinuousConvMode = ENABLE; disable,. ... once or every time
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1Result, ADC1_NUM); // perform only once when _ha.Init.DMAContinuousRequests = ENABLE ;  when disable perform every time
  HAL_ADC_ConvCpltCallBack(&hadc1);


HAL_ADC_Start(&hadc1); // start the adc 

HAL_ADC_PollForConversion(&hadc1, 100); // poll for conversion 
adc_val = HAL_ADC_GetValue(&hadc1); // get the adc value 

HAL_ADC_Stop(&hadc1); // stop adc 

#endif

  _status = 0;
  start(); // //HAL_ADC_Start(&_ha);
}

int stm32adc::add_channel(struct adc_channels *ac) {
  ADC_ChannelConfTypeDef sConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  sConfig.Channel = ac->ch;
  sConfig.Rank = ac->rank = _channel_nr; // 0~ order by
  sConfig.SamplingTime = ac->sampling; //ADC_SAMPLETIME_3CYCLES;
  
  if (HAL_ADC_ConfigChannel(&_ha, &sConfig) != HAL_OK) {}

  if( ac->port != 0 ) { // Vrefint, temperture, ..
    if( IS_GPIO_ALL_INSTANCE(ac->port)) {
      if( IS_GPIO_ALL_INSTANCE(GPIOA) && (ac->port == GPIOA) ) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOB) && (ac->port == GPIOB )) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOC) && (ac->port == GPIOC )) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOD) && (ac->port == GPIOD )) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOE) && (ac->port == GPIOE )) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
      else if( IS_GPIO_ALL_INSTANCE(GPIOH) && (ac->port == GPIOH )) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    } else {
      _status |= 4;
      return -1;
    }

    GPIO_InitStruct.Pin = (1 << (ac->pin)); //GPIO_PIN_n;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ac->port, &GPIO_InitStruct);
  }

  return ++_channel_nr;
}

void stm32adc::start(int timeout) {
  if( ! isready() ) return;

  HAL_ADC_Start(&_ha);
  HAL_ADC_PollForConversion(&_ha, timeout);
}

void stm32adc::stop() {
  if( ! isready() ) return;

  HAL_ADC_Stop(&_ha);
}

void stm32adc::conv() {
  if( isready() ) {
    HAL_ADC_PollForConversion(&_ha, _timeout);
  }
}

int stm32adc::read() {
  if( ! isready() ) { 
    return (-1);
  }

  if( _mode == eADC_POLLING ) {
    start();
  }
  
  uint32_t val = HAL_ADC_GetValue(&_ha);  // get adc value

  if(val & 0xfffff000) {
    _status &= 1; // set error
  }

  if( _mode == eADC_POLLING ) {
    //stop();
  }
  
  return (int)(val & 0xfffl);
}

stm32adcint::stm32adcint() 
: stm32adc() {
}

stm32adcint::stm32adcint(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)()) 
: stm32adc(adc, chs, ac) {

}

stm32adcint::~stm32adcint() {
  detach_intr();
}

void stm32adcint::setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)()) {
  stm32adc::setup(adc, chs, ac);
  attach_intr(intrf);
}

void stm32adcint::attach_intr( void (*inf)()) {
  _intrf = inf;
  _stm32adc_callback = inf;
}


void stm32adcint::detach_intr() {
  _intrf = (void(*)())0;
  _stm32adc_callback = (void(*)())0;
}

int stm32adcint::read() {
  int val = stm32adc::read();
  return val;
}

stm32adcdma::stm32adcdma() 
: stm32adcint() {
}

stm32adcdma::stm32adcdma(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf) 
: stm32adcint(adc, chs, ac, intrf) {
  _dmabuf = dmabuf;
}

void stm32adcdma::setup(ADC_TypeDef *adc, int chs, struct adc_channels *ac, void (*intrf)(), uint16_t *dmabuf) {
  stm32adcint::setup(adc, chs, ac, intrf);

}

stm32adcdma::~stm32adcdma() {
}

int stm32adcdma::read( uint16_t **bp) {
  if(bp) {
    (*bp) = _dmabuf;
    return channel_nr();
  } 
  return 0;
}

#if 0
#define ADC_CHANNEL_0           0x00000000U
#define ADC_CHANNEL_1           ((uint32_t)ADC_CR1_AWDCH_0)
#define ADC_CHANNEL_2           ((uint32_t)ADC_CR1_AWDCH_1)
#define ADC_CHANNEL_3           ((uint32_t)(ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_4           ((uint32_t)ADC_CR1_AWDCH_2)
#define ADC_CHANNEL_5           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_6           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_7           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_8           ((uint32_t)ADC_CR1_AWDCH_3)
#define ADC_CHANNEL_9           ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_10          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_11          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_12          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2))
#define ADC_CHANNEL_13          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_14          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_15          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_16          ((uint32_t)ADC_CR1_AWDCH_4)
#define ADC_CHANNEL_17          ((uint32_t)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_18          ((uint32_t)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_1))

#define ADC_CHANNEL_VREFINT     ((uint32_t)ADC_CHANNEL_17)
#define ADC_CHANNEL_VBAT        ((uint32_t)ADC_CHANNEL_18)
/**
  * @}
  */

/** @defgroup ADC_sampling_times  ADC Sampling Times
  * @{
  */
#define ADC_SAMPLETIME_3CYCLES    0x00000000U
#define ADC_SAMPLETIME_15CYCLES   ((uint32_t)ADC_SMPR1_SMP10_0)
#define ADC_SAMPLETIME_28CYCLES   ((uint32_t)ADC_SMPR1_SMP10_1)
#define ADC_SAMPLETIME_56CYCLES   ((uint32_t)(ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_84CYCLES   ((uint32_t)ADC_SMPR1_SMP10_2)
#define ADC_SAMPLETIME_112CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_144CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1))
#define ADC_SAMPLETIME_480CYCLES  ((uint32_t)ADC_SMPR1_SMP10)

void ADC_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */
 
  /* USER CODE END ADC_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC_IRQn 1 */
 
  /* USER CODE END ADC_IRQn 1 */
}

void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
 
  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
 
  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if(hadc->Instance == hadc1.Instance)
    BSP_LED_On(LED1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  adcVal[adcIndex] = HAL_ADC_GetValue(&hadc1);

  if(adcIndex == 1)
  {
    vol1 = 3000 * adcVal[0] / 4096;
    vol2 = 3000 * adcVal[1] / 4096;

    /* LM35DZ 온도 = 전압(mV) / 10 */
    temp1 = vol1 / 10.0;

    /* 내장온도센서 = (Vsens - V25) / 2.5 + 25도 */
    temp2 = ((float)vol2 - 760.0) / 2.5 + 25.0;
  }

  adcIndex++;
  if(adcIndex > 1) adcIndex = 0;
}

DMA_HandleTypeDef hdma_adc1;
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(_ha,DMA_Handle,hdma_adc1);


int main() {
  stm32adc adc(ADC1,0);
  while(1) {
    int val = adc.read();
  }
}
#endif
