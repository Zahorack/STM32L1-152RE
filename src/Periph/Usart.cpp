/*
 * Usart.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: xgallom
 */

#include "Periph/Usart.h"
#include "Container/Queue.h"
#include "Util/State.h"
#include <Util/Trace.h>

namespace Periph {
static Container::Queue<volatile uint8_t, 512> s_readQueues[Usarts::Size];
static Container::Queue<volatile uint8_t, 512> s_writeQueues[Usarts::Size];
static UART_HandleTypeDef huart[Usarts::Size];
static uint8_t rxByte[Usarts::Size];

namespace States {
enum Flags : uint8_t {
	Writing = 0x01
};
}
static Util::State<uint8_t> s_states;

struct {
	GPIO_TypeDef *gpio;
	uint32_t rx, tx;
	uint8_t gpioAf;
	USART_TypeDef *usart;
	IRQn_Type irqn;
} constexpr config[Usarts::Size] = {
	/* Usart1 */ {
		gpio: GPIOA,
		rx: GPIO_PIN_10,
		tx: GPIO_PIN_9,
		gpioAf: GPIO_AF7_USART1,
		usart: USART1,
		irqn: USART1_IRQn
	},
	/* Usart2 */ {
		gpio: GPIOA,
		rx: GPIO_PIN_3,
		tx: GPIO_PIN_2,
		gpioAf: GPIO_AF7_USART2,
		usart: USART2,
		irqn: USART2_IRQn
	},
	/* Usart3 */ {
		gpio: GPIOC,
		rx: GPIO_PIN_11,
		tx: GPIO_PIN_10,
		gpioAf: GPIO_AF7_USART3,
		usart: USART3,
		irqn: USART3_IRQn
	}
};

void Usart::initRcc()
{
	switch(id) {
		case Usarts::Usart1:
			__USART1_CLK_ENABLE();
			break;
		case Usarts::Usart2:
			__USART2_CLK_ENABLE();
			break;
		case Usarts::Usart3:
			__USART3_CLK_ENABLE();
			break;

		default: break;
	}

}

void Usart::initGpio()
{
	if(config[id].gpio == GPIOA)
		__GPIOA_CLK_ENABLE();
	else if(config[id].gpio == GPIOC)
		__GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpioInitStruct = {
		.Pin = config[id].rx | config[id].tx,
		.Mode = GPIO_MODE_AF_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_HIGH,
		.Alternate = config[id].gpioAf,
	};

	HAL_GPIO_Init(config[id].gpio, &gpioInitStruct);

}

void Usart::initUsart(uint32_t baudRate)
{
	// TODO: Make configurable

	huart[id].Instance = config[id].usart;
	huart[id].Init.BaudRate = baudRate;
	huart[id].Init.WordLength = UART_WORDLENGTH_8B;
	huart[id].Init.StopBits = UART_STOPBITS_1;
	huart[id].Init.Parity = UART_PARITY_NONE;
	huart[id].Init.Mode = UART_MODE_TX_RX;
	huart[id].Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart[id].Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&huart[id]);

	HAL_NVIC_SetPriority(config[id].irqn, 5, 0);
	HAL_NVIC_EnableIRQ(config[id].irqn);               // Enable Interrupt


	HAL_UART_Receive_IT(&huart[id], &rxByte[id], 1);
}


Usart::Usart(Usarts::Enum id, uint32_t baudRate) :
		id(id)
{
	initRcc();
	initGpio();
	initUsart(baudRate);
}

Usart::~Usart()
{
	HAL_UART_DeInit(&huart[id]);
	HAL_NVIC_DisableIRQ(config[id].irqn);
}

bool Usart::write(const uint8_t c)
{
	HAL_UART_Transmit(&huart[id], (uint8_t*)&c, 1, 1000);

	return true;
}

bool Usart::write(const char c[])
{
	while(*c) {
		if(!write(*c++))
			return false;
	}

	return true;
}

bool Usart::write(const uint8_t *c, uint32_t size)
{
	for(uint32_t n = 0; n < size; n++) {
		if(!write(c[n]))
			return false;
	}

	return true;
}


uint8_t Usart::read()
{
	Container::OperationResult<volatile uint8_t> readResult;

	while(!(readResult = s_readQueues[id].dequeue()).isValid) {}

	return readResult.value;
}

uint16_t Usart::readWord()
{
	uint8_t buff[2];

	for(uint8_t n = 0; n < 2; n++) {
		buff[n] = read();
	}

	return (uint16_t)(buff[1]<<8 | buff[0]);
}

uint32_t Usart::readLine(uint8_t *buffer, uint32_t maxSize)
{
	for(uint32_t n = 0; n < maxSize; n++) {
		buffer[n] = read();

		if(buffer[n] == '\n')
			return n +1;
	}

	return maxSize;
}

uint32_t Usart::readBytesUntil(uint8_t character, uint8_t *buffer, uint32_t maxSize)
{
	for(uint32_t n = 0; n < maxSize; n++) {
		buffer[n] = read();

		if(buffer[n] == character)
			return n;
	}
	return maxSize;
}

uint32_t Usart::readBytes(uint8_t *buffer, uint32_t maxSize)
{
	for(uint32_t n = 0; n < maxSize; n++) {
		buffer[n] = read();
	}
	return maxSize;
}

bool Usart::Available() const
{
	return !s_readQueues[id].isEmpty();
}

uint32_t Usart::bytesAvailable() const
{
	uint32_t volume = s_readQueues[id].size();
//	DTRACE("Uart bytes available : %d \r\n", volume);

	return volume;
}

} /* namespace Periph */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *ref)
{
	if (ref == &Periph::huart[Periph::Usarts::Usart1]) {
		// do something with rx_data
		//TRACE("UART1");
		Periph::s_readQueues[Periph::Usarts::Usart1].enqueue(static_cast<uint8_t>(Periph::rxByte[Periph::Usarts::Usart1]));
		HAL_UART_Receive_IT(&Periph::huart[Periph::Usarts::Usart1], &Periph::rxByte[Periph::Usarts::Usart1], 1);
	}


//	if (ref == &Periph::huart[Periph::Usarts::Usart2]) {
//		// do something with rx_data
//
//		Periph::s_readQueues[Periph::Usarts::Usart2].enqueue(static_cast<uint8_t>(Periph::rxByte[Periph::Usarts::Usart2]));
//		HAL_UART_Receive_IT(&Periph::huart[Periph::Usarts::Usart2], &Periph::rxByte[Periph::Usarts::Usart2], 1);
//	}

	if (ref == &Periph::huart[Periph::Usarts::Usart3]) {
		// do something with rx_data
		//TRACE("UART3");
//		Periph::s_readQueues[Periph::Usarts::Usart3].enqueue(static_cast<uint8_t>(Periph::rxByte[Periph::Usarts::Usart3]));
//		HAL_UART_Receive_IT(&Periph::huart[Periph::Usarts::Usart3], &Periph::rxByte[Periph::Usarts::Usart3], 1);
	}

}


void USART1_IRQHandler(void) {
	//TRACE("USART1_IRQHandler");
	HAL_UART_IRQHandler(&Periph::huart[Periph::Usarts::Usart1]);
}

//
//void USART2_IRQHandler()
//{
//	HAL_UART_IRQHandler(&Periph::huart[Periph::Usarts::Usart2]);
//}

void USART3_IRQHandler()
{
	//TRACE("USART3_IRQHandler");
	HAL_UART_IRQHandler(&Periph::huart[Periph::Usarts::Usart3]);
}
