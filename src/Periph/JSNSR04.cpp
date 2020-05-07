//
// Created by zahorack on 5/6/20.
//

#include "Periph/JSNSR04.h"
#include "Periph/FineTimer.h"
#include "Util/Trace.h"

/*
 *  JSNSR04 is low cost ultrasonic module
 */


namespace Periph {

static ultrasonicArgs_t UltrasonicArgs[JSNSR04_Sensors::Size];
static Periph::FineTimer s_micros;

static const uint32_t MaxEchoTimeMicros = 50000;
static const uint32_t TriggerPulseWidthMicros = 15;
static const uint32_t SilenceWidthMicros = TriggerPulseWidthMicros + 0;

static const struct {
    GPIO_TypeDef *trigger_port;
    GPIO_TypeDef *echo_port;
    uint16_t trigger_pin;
    uint16_t echo_pin;
    IRQn_Type Irs;
} config[JSNSR04_Sensors::Size] = {
        {/* Ultrasonic1 */
                .trigger_port = GPIOB,
                .echo_port = GPIOC,
                .trigger_pin =   GPIO_PIN_4,
                .echo_pin = GPIO_PIN_12,
                .Irs =  EXTI15_10_IRQn,
        }
};


JSNRS04::JSNRS04(JSNSR04_Sensors::Enum id):
    m_id(id),
    m_state(UltrasonicStates::None)
{
    TRACE("init sensor 1\n");
    initRcc();
    initGpio();
    UltrasonicArgs[id].data_ready = false;
}


void JSNRS04::initRcc()
{
    if(config[m_id].trigger_port == GPIOA || config[m_id].echo_port == GPIOA)
        __GPIOA_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOB || config[m_id].echo_port == GPIOB)
        __GPIOB_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOC || config[m_id].echo_port == GPIOC)
        __GPIOC_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOD || config[m_id].echo_port == GPIOD)
        __GPIOD_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOE || config[m_id].echo_port == GPIOE)
        __GPIOE_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOF || config[m_id].echo_port == GPIOF)
        __GPIOF_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOG || config[m_id].echo_port == GPIOG)
        __GPIOG_CLK_ENABLE();
    if(config[m_id].trigger_port == GPIOH || config[m_id].echo_port == GPIOH)
        __GPIOH_CLK_ENABLE();
}

void JSNRS04::initGpio() {

    GPIO_InitTypeDef trigger;

    trigger.Pin = static_cast<uint32_t>(config[m_id].trigger_pin);
    trigger.Mode = 	GPIO_MODE_OUTPUT_OD;
    trigger.Pull = 	GPIO_PULLUP;
    trigger.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(config[m_id].trigger_port, &trigger);


    GPIO_InitTypeDef echo;

    echo.Pin = static_cast<uint32_t>(config[m_id].echo_pin);
    echo.Mode =     GPIO_MODE_IT_RISING_FALLING;
    echo.Pull =     GPIO_PULLUP;
    echo.Speed =    GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(config[m_id].echo_port, &echo);


    HAL_NVIC_SetPriority(config[m_id].Irs, 2, 0);
    HAL_NVIC_EnableIRQ(config[m_id].Irs);
}

void JSNRS04::triggerUp() {
//    TRACE("UP\n\r");
    HAL_GPIO_WritePin(config[m_id].trigger_port,config[m_id].trigger_pin, GPIO_PIN_SET);
}

void JSNRS04::triggerDown() {
//    TRACE("DOWN\n\r");
    HAL_GPIO_WritePin(config[m_id].trigger_port,config[m_id].trigger_pin, GPIO_PIN_RESET);
}


void JSNRS04::trigger()
{
    m_state = UltrasonicStates::Trigger;
    UltrasonicArgs[m_id].data_ready = false;
    UltrasonicArgs[m_id].waveIndex = UltrasonicWaves::Echo1;

    triggerUp();
    UltrasonicArgs[m_id].triggerTime = s_micros.read();
}

void JSNRS04::update()
{
    s_micros.update();

    if(m_state == UltrasonicStates::Trigger) {
        if(s_micros.read() > (UltrasonicArgs[m_id].triggerTime + TriggerPulseWidthMicros)) {
            triggerDown();
            m_state = UltrasonicStates::Echo;
        }
    }
//    else if(m_state == UltrasonicStates::Silent) {
//        if (s_micros.read() > (UltrasonicArgs[m_id].triggerTime + SilenceWidthMicros)) {
//            m_state = UltrasonicStates::Echo;
//        }
//    }
    else if(m_state == UltrasonicStates::Echo) {
        if (!available() && s_micros.read() > (UltrasonicArgs[m_id].triggerTime + MaxEchoTimeMicros)) {
                trigger();
        }
    }

}


Container::Result<jsnsr04Result_t> JSNRS04::read()
{
    Container::Result<jsnsr04Result_t> result;

    if(available()){
        //result.value.echoInterval =  (uint32_t)(UltrasonicArgs[m_id].waves[0].echoTime - UltrasonicArgs[m_id].triggerTime);
        result.value.echoInterval =  (uint32_t)(UltrasonicArgs[m_id].waves[0].endTime - UltrasonicArgs[m_id].waves[0].echoTime);
        return result;
    }
    return Container::Result<jsnsr04Result_t>();
}

bool JSNRS04::available()
{
    return UltrasonicArgs[m_id].data_ready == true;
}


static void UltrasonicHandler(JSNSR04_Sensors::Enum id)
{
    Periph::Edges::Enum current_edge = HAL_GPIO_ReadPin(Periph::config[id].echo_port, Periph::config[id].echo_pin) ? Periph::Edges::Rising : Periph::Edges::Falling;

    if(UltrasonicArgs[id].data_ready != true) {
        if (current_edge == Periph::Edges::Rising) {
            UltrasonicArgs[id].waves[0].echoTime = s_micros.read();
        } else {
            UltrasonicArgs[id].waves[0].endTime = s_micros.read();
            UltrasonicArgs[id].data_ready = true;
        }
    }

}


extern "C" void EXTI15_10_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_12)){
        UltrasonicHandler(JSNSR04_Sensors::Sensor1);
    }
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}



}