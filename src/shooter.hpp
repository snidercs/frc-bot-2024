#pragma once

#include <frc/motorcontrol/PWMSparkMax.h>

#include "ports.hpp"
#include "snider/console.hpp"

class Shooter {
public:
    Shooter()  = default;
    ~Shooter() = default;

    void load() { snider::console::log ("[bot] load"); }
    void shoot() { snider::console::log ("[bot] shoot"); }

private:
    frc::PWMSparkMax intakeMotor { Port::ShooterIntake };
    frc::PWMSparkMax shootMotor { Port::ShooterIntake };
};
