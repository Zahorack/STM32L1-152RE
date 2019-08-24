/*
 * Adc.cpp
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */

#include "Periph/Adc.h"
#include "Util/Trace.h"
#include <stm32_hal_legacy.h>

namespace Periph {

static ADC_HandleTypeDef	adcHandles[AdcPins::Size];
static DMA_HandleTypeDef	dmaHandles[AdcPins::Size];
static ADC_ChannelConfTypeDef 	adcChannels[AdcPins::Size];


struct {
	GPIO_TypeDef	*gpio;
	ADC_TypeDef  	*adc;
	uint16_t 	pin;
	uint32_t 	channel;
} static const config[AdcPins::Size] = {
		{ /* AdcPin1 */
				.gpio = GPIOA,
				.adc = ADC1,
				.pin = GPIO_PIN_6,
				.channel = ADC_CHANNEL_6
		},
		{ /* AdcPin2 */
				.gpio = GPIOA,
				.adc = ADC1,
				.pin = GPIO_PIN_7,
				.channel = ADC_CHANNEL_7
		}
};

Adc::Adc(AdcPins::Enum a_id):
	id(a_id)
{
	initGpio();
	initRCC();
	initAdc();
}

void Adc::initGpio()
{
	GPIO_InitTypeDef init = {
		.Pin = config[id].pin,
		.Mode = GPIO_MODE_ANALOG,
		.Pull = GPIO_NOPULL,
	};

	HAL_GPIO_Init(config[id].gpio, &init);
}

void Adc::initRCC()
{
	if(config[id].gpio == GPIOA)
		__GPIOA_CLK_ENABLE();
	else if(config[id].gpio == GPIOB)
		__GPIOB_CLK_ENABLE();
	else if(config[id].gpio == GPIOC)
		__GPIOC_CLK_ENABLE();
	else if(config[id].gpio == GPIOD)
		__GPIOD_CLK_ENABLE();
	else if(config[id].gpio == GPIOE)
		__GPIOE_CLK_ENABLE();
	else if(config[id].gpio == GPIOF)
		__GPIOF_CLK_ENABLE();
	else if(config[id].gpio == GPIOG)
		__GPIOG_CLK_ENABLE();
	else if(config[id].gpio == GPIOH)
		__GPIOH_CLK_ENABLE();

	__ADC1_CLK_ENABLE();
}

void Adc::initAdc()
{
//	HAL_NVIC_SetPriority(ADC1_IRQn, 1, 0);
//	HAL_NVIC_EnableIRQ(ADC1_IRQn);

	adcHandles[id].Instance = ADC1;

	adcHandles[id].Init.ClockPrescaler = 0;
	adcHandles[id].Init.Resolution = ADC_RESOLUTION_12B;
	adcHandles[id].Init.ScanConvMode = DISABLE;
	adcHandles[id].Init.ContinuousConvMode = ENABLE;
	adcHandles[id].Init.DiscontinuousConvMode = DISABLE;
	adcHandles[id].Init.NbrOfDiscConversion = 0;
	adcHandles[id].Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adcHandles[id].Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_CC1;
	adcHandles[id].Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adcHandles[id].Init.NbrOfConversion = 1;
	adcHandles[id].Init.DMAContinuousRequests = ENABLE;
	adcHandles[id].Init.EOCSelection = DISABLE;

	HAL_ADC_Init(&adcHandles[id]);

	adcChannels[id].Channel = config[id].channel;
	adcChannels[id].Rank = 1;
	adcChannels[id].SamplingTime = ADC_SAMPLETIME_384CYCLES;

	if (HAL_ADC_ConfigChannel(&adcHandles[id], &adcChannels[id]) != HAL_OK)
	{
		TRACE("ADC init ERROR\n\r");
	}

	HAL_ADC_Start(&adcHandles[id]);
}

uint16_t Adc::read()
{
	if (HAL_ADC_PollForConversion(&adcHandles[id], 1000000) == HAL_OK)
	{
		return (uint16_t)HAL_ADC_GetValue(&adcHandles[id]);
	}
	else
		return 0;

}




}