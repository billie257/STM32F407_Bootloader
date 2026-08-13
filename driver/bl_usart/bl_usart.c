#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "bl_usart.h"

// USE USART3
// RX: PC11
// TX: PC10
// MODE: 8-N-1
// BAUD: 115200
// DMA: TX/RX

static bl_usart_rx_callback_t rx_callback;

static void uart_io_init(void)
{
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void uart_lowlevel_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    // USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
    // USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

// static void uart_dma_init(void)
// {    
//     DMA_InitTypeDef DMA_InitStructure;
//     DMA_StructInit(&DMA_InitStructure);

//     // TX
//     DMA_InitStructure.DMA_Channel = DMA_Channel_4;
//     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
//     DMA_InitStructure.DMA_Memory0BaseAddr = 0;  // set later
//     DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//     DMA_InitStructure.DMA_BufferSize = 0;  // set later
//     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC16;
//     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//     DMA_Init(DMA1_Stream3, &DMA_InitStructure);
//     DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
//     DMA_Cmd(DMA1_Stream3, DISABLE);

//     // RX
//     DMA_InitStructure.DMA_Channel = DMA_Channel_4;
//     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
//     DMA_InitStructure.DMA_Memory0BaseAddr = 0;  // set later
//     DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//     DMA_InitStructure.DMA_BufferSize = 0;  // set later
//     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC16;
//     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//     DMA_Init(DMA1_Stream1, &DMA_InitStructure);
//     DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
//     DMA_Cmd(DMA1_Stream1, DISABLE);
// }

static void uart_it_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitTypeDef));
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority(USART3_IRQn, 5);

    // // // DMA TX
    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
    // NVIC_SetPriority(DMA1_Stream3_IRQn, 5);

    // // // DMA RX
    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
    // NVIC_SetPriority(DMA1_Stream1_IRQn, 5);
}

void bl_usart_init(void)
{
    uart_it_init();
    uart_lowlevel_init();
    uart_io_init();
}

void bl_usart_write(const uint8_t *data, uint32_t size)
{
    while (size--)
    {
        USART_SendData(USART3, *data++);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }

    // // DMA Tranfer 65536 bytes at most
    // while (size > 0)
    // {
    //     uint32_t chunk_size = size < 65535 ? size : 65535;
	// 	DMA1_Stream3->M0AR = (uint32_t)data;
	// 	DMA1_Stream3->NDTR = chunk_size;
	// 	DMA_Cmd(DMA1_Stream3, ENABLE);
    //     while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE);
    //     data += chunk_size;
    //     size -= chunk_size;
    // }
}

void bl_usart_register_rx_callback(bl_usart_rx_callback_t cb)
{
    rx_callback = cb;
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {  
        if (rx_callback)
        {
            uint8_t data = (uint8_t)USART_ReceiveData(USART3);
            rx_callback(&data, 1);
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// // DMA1 Stream3 for USART3 TX
// void DMA1_Stream3_IRQHandler(void)
// {
//     if (DMA_GetFlagStatus(DMA1_Stream3, DMA_FLAG_TCIF3) != RESET)
//     {

//         DMA_ClearITPendingBit(DMA1_Stream3, DMA_FLAG_TCIF3);
//     }
// }

// // DMA1 Stream1 for USART3 RX
// void DMA1_Stream1_IRQHandler(void)
// {
//     if (DMA_GetFlagStatus(DMA1_Stream1, DMA_FLAG_TCIF1) != RESET)
//     {

//         DMA_ClearITPendingBit(DMA1_Stream1, DMA_FLAG_TCIF1);
//     }
// }
