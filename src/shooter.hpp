#pragma once

#include <frc/motorcontrol/PWMSparkMax.h>

#include "ports.hpp"

class Shooter {
public:
    Shooter()  = default;
    ~Shooter() = default;

    void load() {}
    void shoot() {}

private:
    frc::PWMSparkMax intakeMotor { Port::ShooterIntake };
    frc::PWMSparkMax shootMotor { Port::ShooterIntake };
};
