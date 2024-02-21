#pragma once

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

class Shooter {
public:
    Shooter()  = default;
    ~Shooter() = default;

    void load() { snider::console::log ("[bot] load"); }
    void shoot() { snider::console::log ("[bot] shoot"); }

private:
    rev::CANSparkMax intakeMotor { Port::BottomShootingWheel,
                                   rev::CANSparkLowLevel::MotorType::kBrushed };
    rev::CANSparkMax shootMotor { Port::TopShootingWheel,
                                  rev::CANSparkLowLevel::MotorType::kBrushed };
};
