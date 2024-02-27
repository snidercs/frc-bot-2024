#pragma once

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

#define MECHANICAL_ARM_ENABLED 0

class MechanicalArm {
public:
    MechanicalArm()  = default;
    ~MechanicalArm() = default;

    void moveUp() { snider::console::log ("Push arm away"); }
    void moveDown() { snider::console::log ("Pull arm close"); }

private:
#if MECHANICAL_ARM_ENABLED
    rev::CANSparkMax leftArm { Port::LeftArm,
                               rev::CANSparkLowLevel::MotorType::kBrushless };
    rev::CANSparkMax rightArm { Port::RightArm,
                                rev::CANSparkLowLevel::MotorType::kBrushless };

#endif
};
