#include <stdbool.h>
#include <stddef.h>
#include "key_desc.h"
#include "key.h"

// key1 PE4
// key2 PE3

void key_init(key_desc_t key)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(& GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = key->pupd;
	GPIO_InitStruct.GPIO_Speed = GPIO_Medium_Speed;		  
	GPIO_InitStruct.GPIO_Pin = key->pin;
	GPIO_Init(key->port, &GPIO_InitStruct);	
}

bool key_read(key_desc_t key)
{
	return GPIO_ReadInputDataBit(key->port, key->pin) == key->press_level;
}
