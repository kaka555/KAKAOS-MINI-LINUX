CONFIG_NAME := stm32f103_config
export CONFIG_NAME
PROJECT := stm32f103
export PROJECT
FLAGS_AUXILIARY := -mcpu=cortex-m3 -mthumb
export FLAGS_AUXILIARY
DEF := -D STM32F10X_HD -D USE_STDPERIPH_DRIVER
export DEF
LIBPATH := -L /usr/lib/gcc/arm-none-eabi/4.9.3/armv7e-m/softfp
