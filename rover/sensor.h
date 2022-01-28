#ifndef SENSOR_H
#define SENSOR_H

#include "position.h"

// Klasa abstrakcyjna stanowiąca interfejs dla sensorów.
class Sensor {
public:
    virtual bool is_safe([[maybe_unused]] coordinate_t x,
                         [[maybe_unused]] coordinate_t y) = 0;
};

#endif //SENSOR_H
