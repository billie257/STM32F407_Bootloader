#include "stm32f4xx.h"

void board_lowlevel_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
}
