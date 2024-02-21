#pragma once

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

class MechanicalArm {
public:
    MechanicalArm()  = default;
    ~MechanicalArm() = default;

    enum Direction { Up,
                     Down };

    void move (Direction direction) {
        switch (direction) {
            case Up:
                snider::console::log ("Push arm away");
                break;
            case Down:
                snider::console::log ("Pull arm close");
                break;
        }
    }

    void moveUp() { move (Up); }
    void moveDown() { move (Down); }

private:
    rev::CANSparkMax rightArm { Port::RightArm,
                                 rev::CANSparkLowLevel::MotorType::kBrushed };
    rev::CANSparkMax leftArm { Port::LeftArm,
                                rev::CANSparkLowLevel::MotorType::kBrushed };
};
