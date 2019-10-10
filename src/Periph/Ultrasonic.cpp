/*
 * Ultrasonic.cpp
 *
 *  Created on: 27.9.2079
 *      Author: Zahorack
 */

#include "Periph/Ultrasonic.h"
#include "Util/Trace.h"
#include "Periph/FineTimer.h"

namespace Periph {

typedef struct {
        float x_est_last = 0;
        float P_last = 0;
} kalmanArgs_t;

namespace Edges {
    enum  Enum : uint8_t {
        Rising = 0,
        Falling
    };
}

typedef struct {
	bool data_ready;
	Edges::Enum last_edge;
    uint64_t triggerTime;
    wavesArgs_t waves[UltrasonicWaves::Size];
    uint8_t waveIndex;
	kalmanArgs_t kalman_args;
} ultrasonicArgs_t;


static ultrasonicArgs_t UltrasonicArgs[Ultrasonics::Size];

Periph::FineTimer s_micros;

static const uint32_t MaxEchoTimeMicros = 50000;
static const uint32_t TriggerPulseWidthMicros = 500;
static const uint32_t SilenceWidthMicros = TriggerPulseWidthMicros + 600;

static const struct {
	GPIO_TypeDef *port;
	uint16_t pin;
	IRQn_Type Irs;
} config[Ultrasonics::Size]= {
	{/* Ultrasonic1 */
			port: GPIOA,
			pin: GPIO_PIN_8,
			Irs: EXTI9_5_IRQn,
	},
	{/* Ultrasonic2 */
			port: GPIOB,
			.pin =  GPIO_PIN_10,
			.Irs =  EXTI15_10_IRQn,
	}
};

Ultrasonic::Ultrasonic(Ultrasonics::Enum id):
	m_id(id),
    m_state(UltrasonicStates::None),
	m_timer(Util::Time::FromMilliSeconds(100))
{
    initRcc();
	configure(UltrasonicStates::Trigger);
	m_timer.start();
}

void Ultrasonic::initRcc()
{
    if(config[m_id].port == GPIOA)
                __GPIOA_CLK_ENABLE();
    else if(config[m_id].port == GPIOB)
                __GPIOB_CLK_ENABLE();
    else if(config[m_id].port == GPIOC)
                __GPIOC_CLK_ENABLE();
    else if(config[m_id].port == GPIOD)
                __GPIOD_CLK_ENABLE();
    else if(config[m_id].port == GPIOE)
                __GPIOE_CLK_ENABLE();
    else if(config[m_id].port == GPIOF)
                __GPIOF_CLK_ENABLE();
    else if(config[m_id].port == GPIOG)
                __GPIOG_CLK_ENABLE();
    else if(config[m_id].port == GPIOH)
                __GPIOH_CLK_ENABLE();
}

void Ultrasonic::configure(UltrasonicStates::Enum fcn)
{
	GPIO_InitTypeDef init;
	init.Pin = static_cast<uint32_t>(config[m_id].pin);

	if(fcn == UltrasonicStates::Echo) {
		init.Mode = 	GPIO_MODE_IT_RISING_FALLING;
		init.Pull = 	GPIO_PULLUP;
		HAL_NVIC_SetPriority(config[m_id].Irs, 1, 0);
		HAL_NVIC_EnableIRQ(config[m_id].Irs);
	}
	else if(fcn == UltrasonicStates::Trigger) {
		init.Mode = 	GPIO_MODE_OUTPUT_OD;
		init.Pull = 	GPIO_PULLUP;
		HAL_NVIC_DisableIRQ(config[m_id].Irs);
	}

	init.Speed = 	GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(config[m_id].port, &init);
}

void Ultrasonic::trigger()
{
	configure(UltrasonicStates::Trigger);
    m_state = UltrasonicStates::Trigger;
	UltrasonicArgs[m_id].data_ready = false;
	UltrasonicArgs[m_id].waveIndex = UltrasonicWaves::Echo1;

	HAL_GPIO_WritePin(config[m_id].port,config[m_id].pin, GPIO_PIN_RESET);
    UltrasonicArgs[m_id].triggerTime = s_micros.read();
}

void Ultrasonic::update()
{
    s_micros.update();

	if(m_timer.run()) {

	}

    if(UltrasonicArgs[m_id].data_ready == true) {
        configure(UltrasonicStates::Trigger);
    }

    if(m_state == UltrasonicStates::Trigger) {
		if(s_micros.read() > (UltrasonicArgs[m_id].triggerTime + TriggerPulseWidthMicros)) {
            HAL_GPIO_WritePin(config[m_id].port, config[m_id].pin, GPIO_PIN_SET);
            m_state = UltrasonicStates::Silent;
        }
    }
    else if(m_state == UltrasonicStates::Silent) {
        if (s_micros.read() > (UltrasonicArgs[m_id].triggerTime + SilenceWidthMicros)) {
            m_state = UltrasonicStates::Echo;
            configure(UltrasonicStates::Echo);
        }
    }
    else if(m_state == UltrasonicStates::Echo) {
        if (!available() && s_micros.read() > (UltrasonicArgs[m_id].triggerTime + MaxEchoTimeMicros)) {
            if (UltrasonicArgs[m_id].waveIndex == UltrasonicWaves::Echo1)
                trigger();

            else if (UltrasonicArgs[m_id].waveIndex > UltrasonicWaves::Echo1) {
                UltrasonicArgs[m_id].data_ready = true;
                //m_state = UltrasonicStates::None;
                TRACE("wi %d\n\r", UltrasonicArgs[m_id].waveIndex);
            }
        }
    }
}

void Ultrasonic::evaluate(Container::Result<ultrasonicResult_t> *data, UltrasonicWaves::Enum wave)
{
    if(UltrasonicArgs[m_id].waveIndex > wave) {
        data->value.echoInterval[wave] =  (uint32_t)(UltrasonicArgs[m_id].waves[wave].echoTime - UltrasonicArgs[m_id].triggerTime);
        data->value.pulseInterval[wave] =  (uint32_t)(UltrasonicArgs[m_id].waves[wave].endTime - UltrasonicArgs[m_id].waves[wave].echoTime);
    }
    else {
        data->value.echoInterval[wave] =  0;
        data->value.pulseInterval[wave] =  0;
    }
}

Container::Result<ultrasonicResult_t> Ultrasonic::read()
{
	Container::Result<ultrasonicResult_t> result;

	if(available()){
        for(int i = UltrasonicWaves::Echo1; i < UltrasonicWaves::Size; i++) {
            evaluate(&result, static_cast<UltrasonicWaves::Enum>(i));
        }
        return result;
	}
	return Container::Result<ultrasonicResult_t>();
}

static void UltrasonicHandler(Ultrasonics::Enum id)
{
	Periph::Edges::Enum current_edge = HAL_GPIO_ReadPin(Periph::config[id].port, Periph::config[id].pin) ? Periph::Edges::Rising : Periph::Edges::Falling;

	if(UltrasonicArgs[id].data_ready != true) {
        if (UltrasonicArgs[id].last_edge == Periph::Edges::Rising && current_edge == Periph::Edges::Falling) {
            /* Falling edge is event for wave echo time */
            UltrasonicArgs[id].waves[UltrasonicArgs[id].waveIndex].echoTime = s_micros.read();
            UltrasonicArgs[id].last_edge = current_edge;
        }
        if (UltrasonicArgs[id].last_edge == Periph::Edges::Falling && current_edge == Periph::Edges::Rising) {
            /* Rising edge is event for wave end time */
            UltrasonicArgs[id].waves[UltrasonicArgs[id].waveIndex].endTime = s_micros.read();
            UltrasonicArgs[id].last_edge = current_edge;

            UltrasonicArgs[id].waveIndex++;

            if(UltrasonicArgs[id].waveIndex >= UltrasonicWaves::Size) {
                UltrasonicArgs[id].data_ready = true;
                UltrasonicArgs[id].waveIndex--;
            }
        }
   }

}

bool Ultrasonic::available()
{
    return UltrasonicArgs[m_id].data_ready == true;
}


extern "C" void EXTI0_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_0)){
		TRACE("EXTI0_IRQHandler\r\n");
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

extern "C" void EXTI1_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_1)){
		TRACE("EXTI0_IRQHandler\r\n");
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

extern "C" void EXTI2_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_2)){
		TRACE("EXTI0_IRQHandler\r\n");
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

extern "C" void EXTI3_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_3)){
		TRACE("EXTI0_IRQHandler\r\n");
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

extern "C" void EXTI4_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_4)){
		TRACE("EXTI0_IRQHandler\r\n");
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

extern "C" void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_8)){
		UltrasonicHandler(Ultrasonics::Ultrasonic1);
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

extern "C" void EXTI15_10_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_15)){

	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}


}