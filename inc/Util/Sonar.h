//
// Created by zahorack on 10/10/19.
//

#ifndef STM32L1_152RE_SONAR_H
#define STM32L1_152RE_SONAR_H

#include "Periph/Ultrasonic.h"

namespace Util {


class Sonar {

    Periph::Ultrasonic m_ultrasonic;
public:
    Sonar();


};

}
#endif //STM32L1_152RE_SONAR_H
