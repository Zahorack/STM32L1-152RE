/*
 * Pwm.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: Zahorack
 */

#include "Periph/Pwm.h"
#include "Periph/Servo.h"

namespace Periph
{
	static const uint32_t
		PwmFrequency = 10000;
	static const uint16_t
		PwmSteps = 100,
		ServoPwmSteps = 20000,
		ServoPwmFrequency = 50;

	namespace Timers
	{
		enum Enum : uint8_t {
			Timer1 = 0,
			Timer2,
			Timer3,

			Size
		};
	}

	Util::RefCounter Pwm::s_refCounter = Util::RefCounter();
	TIM_HandleTypeDef htim[Timers::Size];


	struct {
			GPIO_TypeDef 	*gpio;
			TIM_TypeDef     *tim;
			uint32_t        p1, p2, p3, p4;
			uint8_t gpioAf;
	} static const Config[Timers::Size] = {
			{
				.gpio = GPIOA,
				.tim = TIM2,
				.p1 = GPIO_PIN_0,
				.p2 = GPIO_PIN_1,
				.p3 = GPIO_PIN_2,
				.p4 = GPIO_PIN_3,
				.gpioAf = GPIO_AF1_TIM2
			},
			{
				.gpio = GPIOC,
				.tim = TIM3,
				.p1 = GPIO_PIN_6,
				.p2 = GPIO_PIN_7,
				.p3 = GPIO_PIN_8,
				.p4 = GPIO_PIN_9,
				.gpioAf = GPIO_AF2_TIM3
			}
	};

	static volatile uint32_t *s_outChnl[Pwms::Size] = {
			&(Config[Timers::Timer1].tim->CCR1), // PA0
			&(Config[Timers::Timer1].tim->CCR2), // PA1
			&(Config[Timers::Timer1].tim->CCR3), // PA2
			&(Config[Timers::Timer1].tim->CCR4), // PA3

			&(Config[Timers::Timer2].tim->CCR1), // PC6
			&(Config[Timers::Timer2].tim->CCR2), // PC7
			&(Config[Timers::Timer2].tim->CCR3), // PC8
			&(Config[Timers::Timer2].tim->CCR4), // PC9

	};

	void initRCC()
	{
		 __HAL_RCC_TIM2_CLK_ENABLE();
		 __HAL_RCC_GPIOA_CLK_ENABLE();

		 __HAL_RCC_TIM3_CLK_ENABLE();
		 __HAL_RCC_GPIOC_CLK_ENABLE();
	}

	static void initSingleGpio(Timers::Enum timer)
	{
		GPIO_InitTypeDef init = {
			.Pin = Config[timer].p1 | Config[timer].p2 | Config[timer].p3 | Config[timer].p4,
			.Mode = GPIO_MODE_AF_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_HIGH,
			.Alternate = Config[timer].gpioAf
		};

		HAL_GPIO_Init(Config[timer].gpio, &init);
	}

	static void initGpio()
	{
		initSingleGpio(Timers::Timer1);
		initSingleGpio(Timers::Timer2);
	}

	void initSingleTimHandle(Timers::Enum timer, uint32_t period, uint32_t prescaler)
	{
		htim[timer].Instance = Config[timer].tim;
		htim[timer].Init.Prescaler = prescaler;
		htim[timer].Init.Period = period;
		htim[timer].Init.ClockDivision = 0;
		htim[timer].Init.CounterMode = TIM_COUNTERMODE_UP;

		if (HAL_TIM_PWM_Init(&htim[timer]) != HAL_OK) {
			/* Initialization Error */
		}
	}

	void initTimHandle()
	{

		const uint32_t
			period         = PwmSteps      - 1,
			servoPeriod    = ServoPwmSteps - 1,
			prescaler      = (SystemCoreClock / (PwmSteps      * PwmFrequency    )) - 1,
			servoPrescaler = (SystemCoreClock / (ServoPwmSteps * ServoPwmFrequency)) - 1;

		initSingleTimHandle(Timers::Timer1, period, prescaler);
		initSingleTimHandle(Timers::Timer2, servoPeriod, servoPrescaler);

		/* Servo Motor time base configuration
		 * Recommended frequency : 50 Hz
		 * neutral position 1500 us
		 * -90deg  position	1000 us
		 * +90deg  position	2000 us
		 */
	}




	static void initSingleTimerOC(Timers::Enum timer, uint32_t frequency)
	{
		static TIM_OC_InitTypeDef init = {
				.OCMode = TIM_OCMODE_PWM1,
				.Pulse = 0,
				.OCPolarity = TIM_OCPOLARITY_HIGH,
				.OCFastMode = TIM_OCFAST_DISABLE,
		};

		HAL_TIM_PWM_ConfigChannel(&htim[timer], &init, TIM_CHANNEL_1);
		HAL_TIM_PWM_ConfigChannel(&htim[timer], &init, TIM_CHANNEL_2);
		HAL_TIM_PWM_ConfigChannel(&htim[timer], &init, TIM_CHANNEL_3);
		HAL_TIM_PWM_ConfigChannel(&htim[timer], &init, TIM_CHANNEL_4);

		HAL_TIM_PWM_Start(&htim[timer], TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim[timer], TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim[timer], TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&htim[timer], TIM_CHANNEL_4);
	}

	static void initTimerOC()
	{
		initSingleTimerOC(Timers::Timer1, PwmFrequency);
		initSingleTimerOC(Timers::Timer2, ServoPwmFrequency);
	}

	static void deinitPwm()
	{
		 HAL_TIM_PWM_DeInit(&htim[Timers::Timer1]);
		 HAL_TIM_PWM_DeInit(&htim[Timers::Timer2]);
	}

	Pwm::Pwm()
	{
		if(!s_refCounter.referenceExists()) {
			initRCC();
			initGpio();
			initTimHandle();
			initTimerOC();
		}

		++s_refCounter;

//		write(Pwms::Pwm5, ServoHomePosition);
//		write(Pwms::Pwm6, ServoHomePosition);
//		write(Pwms::Pwm7, ServoHomePosition);
//		write(Pwms::Pwm8, ServoHomePosition);
		write(Pwms::Pwm5, ServoHomePosition);
	}

	Pwm::~Pwm()
	{
		--s_refCounter;

		if(s_refCounter.destroyReference())
			deinitPwm();
	}

	void Pwm::write(Pwms::Enum id, uint32_t value)
	{
		 *s_outChnl[id] = value;
	}

	uint16_t Pwm::read(Pwms::Enum id) const
	{
		return *s_outChnl[id];
	}
}