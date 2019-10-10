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

typedef struct {
	bool data_ready;
	uint8_t last_edge;
	uint64_t echoTime;
	uint64_t triggerTime;
	uint64_t endTime;
	kalmanArgs_t kalman_args;
} ultrasonicArgs_t;



namespace Edges {
enum  : uint8_t {
	Rising = 0,
	Falling
};
}

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
	configure(UltrasonicStates::Trigger);
	m_timer.start();
}


void Ultrasonic::configure(UltrasonicStates::Enum fcn)
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

	GPIO_InitTypeDef init;
	init.Pin = static_cast<uint32_t>(config[m_id].pin);

	if(fcn == UltrasonicStates::Echo) {
		//TRACE("Echo");
		init.Mode = 	GPIO_MODE_IT_RISING_FALLING;
		init.Pull = 	GPIO_PULLUP;
		HAL_NVIC_SetPriority(config[m_id].Irs, 3, 0);
		HAL_NVIC_EnableIRQ(config[m_id].Irs);
	}
	else if(fcn == UltrasonicStates::Trigger) {
		//TRACE("Trigger");
		init.Mode = 	GPIO_MODE_OUTPUT_OD;
		init.Pull = 	GPIO_PULLUP;
		HAL_NVIC_DisableIRQ(config[m_id].Irs);
	}

	init.Speed = 	GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(config[m_id].port, &init);

	//TRACE("Ultrasonic inti ok \n\r");
}

void Ultrasonic::trigger()
{
	configure(UltrasonicStates::Trigger);
    m_state = UltrasonicStates::Trigger;
	UltrasonicArgs[m_id].data_ready = false;

	HAL_GPIO_WritePin(config[m_id].port,config[m_id].pin, GPIO_PIN_RESET);
    UltrasonicArgs[m_id].triggerTime = s_micros.read();
}

void Ultrasonic::update()
{
    s_micros.update();

	if(m_timer.run()) {

	}

    if(s_micros.read() > (UltrasonicArgs[m_id].triggerTime + MaxEchoTimeMicros) && m_state == UltrasonicStates::Echo) {
        if(!UltrasonicArgs[m_id].data_ready)
            trigger();

    }

	if(m_state == UltrasonicStates::Trigger) {
		if(s_micros.read() > (UltrasonicArgs[m_id].triggerTime + TriggerPulseWidthMicros)) {
            HAL_GPIO_WritePin(config[m_id].port, config[m_id].pin, GPIO_PIN_SET);
            m_state = UltrasonicStates::Silent;
        }
    }

    if(m_state == UltrasonicStates::Silent) {
        if (s_micros.read() > (UltrasonicArgs[m_id].triggerTime + SilenceWidthMicros)) {
            m_state = UltrasonicStates::Echo;
            configure(UltrasonicStates::Echo);
        }
    }

	if(m_state == UltrasonicStates::Echo) {

	}
}

Container::Result<ultrasonicResult_t> Ultrasonic::read()
{
	Container::Result<ultrasonicResult_t> result;

	if(UltrasonicArgs[m_id].data_ready){
		result.value.echoTime =  (uint32_t)(UltrasonicArgs[m_id].echoTime - UltrasonicArgs[m_id].triggerTime);
        result.value.pulseTime =  (uint32_t)(UltrasonicArgs[m_id].endTime - UltrasonicArgs[m_id].echoTime);
		return result;
	}

	return Container::Result<ultrasonicResult_t>();
}

static void UltrasonicHandler(Ultrasonics::Enum id)
{
	uint8_t current_edge = HAL_GPIO_ReadPin(Periph::config[id].port, Periph::config[id].pin) ? Periph::Edges::Rising : Periph::Edges::Falling;

	if(UltrasonicArgs[id].data_ready != true) {
        if (UltrasonicArgs[id].last_edge == Periph::Edges::Rising && current_edge == Periph::Edges::Falling) {
            UltrasonicArgs[id].echoTime = s_micros.read();
            //UltrasonicArgs[id].data_ready = true;
            UltrasonicArgs[id].last_edge = current_edge;
        }
        if (UltrasonicArgs[id].last_edge == Periph::Edges::Falling && current_edge == Periph::Edges::Rising) {
            UltrasonicArgs[id].endTime = s_micros.read();
            UltrasonicArgs[id].data_ready = true;
            UltrasonicArgs[id].last_edge = current_edge;
        }
   }
}

bool Ultrasonic::availbale()
{
    bool ret = UltrasonicArgs[m_id].data_ready;
    if(ret == true) {
//        TRACE("Data ready");
        return true;
    }
	return false;
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
//        TRACE("E\n\r");
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