//
// Created by zahorack on 10/10/19.
//

#include "Util/Sonar.h"
#include "Util/Trace.h"

namespace Util {

static uint32_t SpeedOfSoundINWater = 1480;
static uint32_t SpeedOfSoundInAir = 343;

static uint32_t sequenceIndex = Periph::Ultrasonics::Ultrasonic1;

Sonar::Sonar():
    m_state(SonarStates::None),
    m_timer(Util::Time::FromMilliSeconds(200)),
    singleBeamUltrasonic(Periph::JSNSR04_Sensors::Sensor1)
{
    m_timer.start();
}

Sonar::~Sonar()
{}

void Sonar::update() {

    /* Triggering */
    if(m_state == SonarStates::SequenceTrigger) {
        if(m_ultrasonics[sequenceIndex].available()) {
            sequenceIndex++;
            if(sequenceIndex >= Periph::Ultrasonics::Size)
                m_state = SonarStates::None;
            else
                m_ultrasonics[sequenceIndex].trigger();
        }
    }

    if(m_timer.run()) {
        if (available()) {
            if (singleBeamUltrasonic.available()) {
                singleBeamUltrasonic.trigger();
            }
        }
    }

    /* Updates */
    for(int i = 0; i < Periph::Ultrasonics::Size; i++) {
        m_ultrasonics[i].update();
    }
    singleBeamUltrasonic.update();
}

void Sonar::trigger(Periph::Ultrasonics::Enum id) {

    for(int i = 0; i < Periph::Ultrasonics::Size; i++) {
        if(i == static_cast<int>(id)) {
            m_ultrasonics[id].trigger();
        }
        else
            m_ultrasonics[i].listen();
    }

}

void Sonar::trigger() {
    for(int i = 0; i < Periph::Ultrasonics::Size; i++) {
        m_ultrasonics[i].trigger();
    }
}


bool Sonar::available() {
    bool result = true;

    for(int i = 0; i < Periph::Ultrasonics::Size; i++) {
        result = result && m_ultrasonics[i].available();
    }
    return result;
}

Container::Result<sonarResult_t> Sonar::read() {

    Container::Result<sonarResult_t> result;

    if(available()) {
        for (int i = 0; i < Periph::Ultrasonics::Size; i++) {
            result.value.ultrasonicsResults[i] = m_ultrasonics[i].read();
        }
        return result;
    }
    return Container::Result<sonarResult_t>();
}

void Sonar::sequencedTrigger() {
    sequenceIndex = Periph::Ultrasonics::Ultrasonic1;
    m_state = SonarStates::SequenceTrigger;

    m_ultrasonics[sequenceIndex].trigger();
}



}