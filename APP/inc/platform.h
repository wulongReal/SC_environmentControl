#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "stdint.h"

#if defined(PLATFORM_F103)
  #include "stm32f10x_conf.h"
  #include "stm32f10x_it.h"
  #include "stm32f10x.h"


#elif defined(PLATFORM_F405)  
  #include "stm32f4xx_conf.h"
  #include "stm32f4xx_it.h"
  #include "stm32f4xx.h"
#endif


#endif

