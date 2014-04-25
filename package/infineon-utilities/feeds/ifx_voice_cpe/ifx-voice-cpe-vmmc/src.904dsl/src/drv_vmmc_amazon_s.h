#ifndef _DRV_VMMC_AMAZON_S_H
#define _DRV_VMMC_AMAZON_S_H
/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/
/*

*/


#if defined(SYSTEM_AR9) || defined(SYSTEM_VR9)
#include <asm/ifx/ifx_gpio.h>
#else
#error no system selected
#endif

#define VMMC_TAPI_GPIO_MODULE_ID                        IFX_GPIO_MODULE_TAPI_VMMC
/**

*/

#if 0 // ctc merged

#define VMMC_PCM_IF_CFG_HOOK(mode, GPIOreserved, ret) \
do { \
   ret = VMMC_statusOk; \
   /* Reserve P0.0 as TDM/FSC */ \
   if (!GPIOreserved) \
      ret |= ifx_gpio_pin_reserve(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel0_set(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel1_set(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_open_drain_set(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID);\
   \
   /* Reserve P1.9 as TDM/DO */ \
   if (!GPIOreserved) \
      ret |= ifx_gpio_pin_reserve(IFX_GPIO_PIN_ID(1, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel0_set(IFX_GPIO_PIN_ID(1, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel1_clear(IFX_GPIO_PIN_ID(1, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_dir_out_set(IFX_GPIO_PIN_ID(1, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_open_drain_set(IFX_GPIO_PIN_ID(1, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   \
   /* Reserve P2.9 as TDM/DI */ \
   if (!GPIOreserved) \
      ret |= ifx_gpio_pin_reserve(IFX_GPIO_PIN_ID(2, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel0_clear(IFX_GPIO_PIN_ID(2, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel1_set(IFX_GPIO_PIN_ID(2, 9), VMMC_TAPI_GPIO_MODULE_ID);\
   ret |= ifx_gpio_dir_in_set(IFX_GPIO_PIN_ID(2, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   \
   /* Reserve P2.8 as TDM/DCL */ \
   if (!GPIOreserved) \
      ret |= ifx_gpio_pin_reserve(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel0_clear(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_altsel1_set(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_open_drain_set(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
   \
   if (mode == 2) { \
      /* TDM/FSC+DCL Master */ \
      ret |= ifx_gpio_dir_out_set(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID); \
      ret |= ifx_gpio_dir_out_set(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
   } else { \
      /* TDM/FSC+DCL Slave */ \
      ret |= ifx_gpio_dir_in_set(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID); \
      ret |= ifx_gpio_dir_in_set(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
   } \
} while(0);

/**

*/
#define VMMC_DRIVER_UNLOAD_HOOK(ret) \
do { \
   ret = VMMC_statusOk; \
   ret |= ifx_gpio_pin_free(IFX_GPIO_PIN_ID(0, 0), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_pin_free(IFX_GPIO_PIN_ID(1, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_pin_free(IFX_GPIO_PIN_ID(2, 9), VMMC_TAPI_GPIO_MODULE_ID); \
   ret |= ifx_gpio_pin_free(IFX_GPIO_PIN_ID(2, 8), VMMC_TAPI_GPIO_MODULE_ID); \
} while (0)

#endif

#endif /* _DRV_VMMC_AMAZON_S_H */
