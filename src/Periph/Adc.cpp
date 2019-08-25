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

static ADC_HandleTypeDef	adcHandle;
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
	HAL_ADC_Stop(&adcHandle);
	HAL_ADC_DeInit(&adcHandle);

	adcHandle.Instance = ADC1;

	adcHandle.Init.ClockPrescaler = 0;
	adcHandle.Init.Resolution = ADC_RESOLUTION_12B;
	adcHandle.Init.ScanConvMode = DISABLE;
	adcHandle.Init.ContinuousConvMode = ENABLE;
	adcHandle.Init.DiscontinuousConvMode = DISABLE;
	adcHandle.Init.NbrOfDiscConversion = 0;
	adcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_CC1;
	adcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adcHandle.Init.NbrOfConversion = 1;
	adcHandle.Init.DMAContinuousRequests = ENABLE;
	adcHandle.Init.EOCSelection = DISABLE;

	HAL_ADC_Init(&adcHandle);

	adcChannels[id].Channel = config[id].channel;
	adcChannels[id].Rank = 1;
	adcChannels[id].SamplingTime = ADC_SAMPLETIME_384CYCLES;

//	if (HAL_ADC_ConfigChannel(&adcHandle, &adcChannels[id]) != HAL_OK)
//	{
//		TRACE("ADC init ERROR\n\r");
//	}
//	HAL_ADC_Start(&adcHandle);
}

uint32_t Adc::read()
{
	uint32_t result = 0;

	if (HAL_ADC_ConfigChannel(&adcHandle, &adcChannels[id]) != HAL_OK)
	{
		TRACE("ADC channel init ERROR\n\r");
	}
	HAL_ADC_Start(&adcHandle);

	if (HAL_ADC_PollForConversion(&adcHandle, 1000000) == HAL_OK)
	{
		result = HAL_ADC_GetValue(&adcHandle);
	}
	HAL_ADC_Stop(&adcHandle);

	return (uint32_t)result;
}




}