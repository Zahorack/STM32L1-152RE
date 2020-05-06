//
// Created by zahorack on 10/10/19.
//

#include "Util/Sonar.h"

namespace Util {


Sonar::Sonar()
{}

void Sonar::update() {
    for(int i = 0; i < Periph::Ultrasonics::Size; i++) {
        m_ultrasonics[i].update();
    }
}

void Sonar::trigger(Periph::Ultrasonics::Enum id) {
    m_ultrasonics[id].trigger();
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



}