#include "stm32f4xx.h"
#include "key.h"
#include "key_desc.h"
#include "led.h"
#include "led_desc.h"

void board_lowlevel_init(void)
{  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

#define KEY_DEFINE(n, PORT, PIN, IRQn) \
static struct key_desc _key##n = \
{ \
    .port = GPIO##PORT, \
    .pin = GPIO_Pin_##PIN, \
    .pupd = GPIO_PuPd_DOWN, \
    .exti_port_src = EXTI_PortSourceGPIO##PORT, \
    .exti_pin_src = EXTI_PinSource##PIN, \
    .exti_line = EXTI_Line##PIN, \
    .irqn = IRQn, \
    .press_level = Bit_SET, \
}; \
key_desc_t key##n = &_key##n
KEY_DEFINE(1, E, 4, EXTI4_IRQn);
KEY_DEFINE(2, E, 3, EXTI3_IRQn);

static struct led_desc _led1 =
{
    GPIOF,
    GPIO_Pin_9,
    Bit_RESET,
    Bit_SET
};

static struct led_desc _led2 =
{
    GPIOF,
    GPIO_Pin_10,
    Bit_RESET,
    Bit_SET
};

led_desc_t led1 = &_led1;
led_desc_t led2 = &_led2;
