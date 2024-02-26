#ifdef __cplusplus
extern "C" {
#endif

#include <stm32yyxx_ll_iwdg.h>

void init_iwdg() {

  LL_IWDG_Enable(IWDG);
  LL_IWDG_EnableWriteAccess(IWDG);

  // 32kHz / 32 = 1kHz
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_32);

  // counter (999+1) is 1sec
  LL_IWDG_SetReloadCounter(IWDG, 300);

  while (LL_IWDG_IsReady(IWDG) != 1);
  LL_IWDG_ReloadCounter(IWDG);
}

void clear_iwdg() {
	LL_IWDG_ReloadCounter(IWDG);
}

#ifdef __cplusplus
}
#endif
