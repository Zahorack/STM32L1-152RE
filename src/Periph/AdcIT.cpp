/*
 * Adc.cpp
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */

#include "Periph/AdcIT.h"
#include "Util/Trace.h"
#include <stm32_hal_legacy.h>

namespace Periph {

static ADC_HandleTypeDef	adcHandle;
static ADC_ChannelConfTypeDef 	adcChannels[AdcITPins::Size];
static ADC_ChannelConfTypeDef	actualChannel;

static volatile uint8_t adcFlags[AdcITPins::Size] = {AdcFlags::dataReady, AdcFlags::dataReady};
static volatile uint32_t adcData[AdcITPins::Size];

extern "C" bool isAdcReady();
extern "C" void ADC_StartRead_IT(ADC_ChannelConfTypeDef *channel);


struct {
	GPIO_TypeDef	*gpio;
	ADC_TypeDef  	*adc;
	uint16_t 	pin;
	uint32_t 	channel;
} static const config[AdcITPins::Size] = {
		{ /* AdcPin1 */
				.gpio = GPIOC,
				.adc = ADC1,
				.pin = GPIO_PIN_4,
				.channel = ADC_CHANNEL_14
		},
		{ /* AdcPin2 */
				.gpio = GPIOC,
				.adc = ADC1,
				.pin = GPIO_PIN_5,
				.channel = ADC_CHANNEL_15
		}
};

AdcIT::AdcIT(AdcITPins::Enum a_id):
	id(a_id)
{
	initGpio();
	initRCC();
	initAdc();

	adcFlags[id] = AdcFlags::dataReady;
}

void AdcIT::initGpio()
{
	GPIO_InitTypeDef init = {
		.Pin = config[id].pin,
		.Mode = GPIO_MODE_ANALOG,
		.Pull = GPIO_NOPULL,
	};

	HAL_GPIO_Init(config[id].gpio, &init);
}

void AdcIT::initRCC()
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

void AdcIT::initAdc()
{
	HAL_NVIC_SetPriority(ADC1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(ADC1_IRQn);

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

	if (HAL_ADC_ConfigChannel(&adcHandle, &adcChannels[id]) != HAL_OK)
	{
		TRACE("ADC init ERROR\n\r");
	}

	//HAL_ADC_Start_IT(&adcHandle);
}

void AdcIT::startRead()
{
	if(isAdcReady()) {
		TRACE("ADC IT start\n\r");
		adcFlags[id] = AdcFlags::dataBussy;
		ADC_StartRead_IT(&adcChannels[id]);
	}
}

bool AdcIT::isReady() {
	return adcFlags[id];
}

uint32_t AdcIT::getData()
{
	return adcData[id];
}

extern "C"
{
	void ADC_StartRead_IT(ADC_ChannelConfTypeDef *channel)
	{
		HAL_ADC_Stop(&adcHandle);
		if (HAL_ADC_ConfigChannel(&adcHandle, channel) != HAL_OK)
		{
			TRACE("ADC channel init ERROR\n\r");
		}
		else {
			TRACE("channel OK\n\r");
		}
		actualChannel = *channel;

		if(HAL_ADC_Start_IT(&adcHandle) != HAL_OK) {
			TRACE("HAL_ADC_Start_IT ERROR\n\r");
		}
		else {
			TRACE("HAL_ADC_Start_IT OK");
		}
	}

	void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* adcRef)
	{
		TRACE("HAL_ADC_ConvCpltCallback\n\r");
		for(int i = 0; i < AdcITPins::Size; i++) {
			if(actualChannel.Channel == config[i].channel) {
				adcFlags[i] = AdcFlags::dataReady;
				adcData[i] = HAL_ADC_GetValue(adcRef);
				break;
			}
		}

		HAL_ADC_Stop(adcRef);
	}

	void ADC_IRQHandler()
	{
		HAL_ADC_IRQHandler(&adcHandle);
	}

	bool isAdcReady()
	{
		for(int i = 0; i < AdcITPins::Size; i++) {
			if(adcFlags[i] == AdcFlags::dataBussy) {
				return false;
			}
		}
		return true;
	}
}



}