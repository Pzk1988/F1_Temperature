#include <Can.hpp>
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "core_cm3.h"
#include <string.h>

namespace Driver
{

Can::Can(uint8_t ownId) : ownId(ownId)
{

}

uint8_t Can::Init()
{
	uint8_t ret = 0;
	// Clock configuration
	// Alternate function clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// Can1 clock
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_CAN1EN, ENABLE);
	// GPIOA clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//GPIO configuration
	GPIO_InitTypeDef GPIO_InitStruct;
	// Can Rx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	// Can Tx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Can configuration, 500 kbit/s
	CAN_InitTypeDef CAN_InitStruct;
	CAN_InitStruct.CAN_TTCM = DISABLE;
	CAN_InitStruct.CAN_ABOM = DISABLE;
	CAN_InitStruct.CAN_AWUM = DISABLE;
	CAN_InitStruct.CAN_NART = ENABLE;
	CAN_InitStruct.CAN_RFLM = DISABLE;
	CAN_InitStruct.CAN_TXFP = DISABLE;
	CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
	CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStruct.CAN_Prescaler = 4;
	ret = CAN_Init(CAN1, &CAN_InitStruct);

	// Can filter, for now pass all frames
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	// Interrupt
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable interrupt
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

	return ret;
}

uint8_t Can::Init(uint8_t filterId)
{
	uint8_t ret = 0;
	// Clock configuration
	// Alternate function clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// Can1 clock
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_CAN1EN, ENABLE);
	// GPIOA clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//GPIO configuration
	GPIO_InitTypeDef GPIO_InitStruct;
	// Can Rx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	// Can Tx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Can configuration, 500 kbit/s
	CAN_InitTypeDef CAN_InitStruct;
	CAN_InitStruct.CAN_TTCM = DISABLE;
	CAN_InitStruct.CAN_ABOM = DISABLE;
	CAN_InitStruct.CAN_AWUM = DISABLE;
	CAN_InitStruct.CAN_NART = ENABLE;
	CAN_InitStruct.CAN_RFLM = DISABLE;
	CAN_InitStruct.CAN_TXFP = DISABLE;
	CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
	CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStruct.CAN_Prescaler = 4;
	ret = CAN_Init(CAN1, &CAN_InitStruct);

	// Can filter, for now pass all frames
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	uint32_t filter_mask = 0x0000001f;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((filterId << 5)  | (filterId >> (32 - 5))) & 0xFFFF; // STID[10:0] & EXTID[17:13]
	CAN_FilterInitStructure.CAN_FilterIdLow = (filterId >> (11 - 3)) & 0xFFF8; // EXID[12:5] & 3 Reserved bits
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((filter_mask << 5)  | (filter_mask >> (32 - 5))) & 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (filter_mask >> (11 - 3)) & 0xFFF8;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	// Interrupt
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable interrupt
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

	return ret;
}

bool Can::SendDataFrame(uint16_t id, uint8_t* pData, uint8_t len)
{
	CanTxMsg canTxMsg;
	canTxMsg.StdId = (ownId << 5) | (id & 0x1f);
	canTxMsg.RTR = CAN_RTR_DATA;
	canTxMsg.IDE = CAN_ID_STD;
	canTxMsg.DLC = len;
	memcpy(canTxMsg.Data, pData, len);
	CAN_Transmit(CAN1, &canTxMsg);
	return true;
}

bool Can::SendDataFrame(uint16_t id, uint32_t extId, uint8_t* pData, uint8_t len)
{
	CanTxMsg canTxMsg;
	canTxMsg.RTR = CAN_RTR_DATA;
	canTxMsg.IDE = CAN_ID_EXT;
	canTxMsg.ExtId = (extId << 10) | ((ownId & 0x1f) << 5) | (id & 0x1f);
	canTxMsg.DLC = len;
	memcpy(canTxMsg.Data, pData, len);
	CAN_Transmit(CAN1, &canTxMsg);
	return true;
}

bool Can::SendRemoteFrame(uint16_t id)
{
	CanTxMsg canTxMsg;
	canTxMsg.StdId = (ownId << 5) | id;
	canTxMsg.RTR = CAN_RTR_REMOTE;
	canTxMsg.IDE = CAN_ID_STD;
	canTxMsg.DLC = 0;
	CAN_Transmit(CAN1, &canTxMsg);
	return true;
}

} // namespace Driver
