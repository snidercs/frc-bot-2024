#pragma once

#include <array>
#include <numbers>

#include <frc/Encoder.h>
#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "types.hpp"

class MechanicalArm {
public:
    MechanicalArm() {
        for (auto m : motors)
            m->SetSmartCurrentLimit (40, 30);
        resetEncoders();
    }

    ~MechanicalArm() = default;

    void moveUp() {
        for (auto m : motors)
            m->SetVoltage (units::volt_t { 0.3 * 12.0 });
    }

    void moveDown() {
        for (auto m : motors)
            m->SetVoltage (units::volt_t { 0.3 * -12.0 });
    }

    void stop() {
        for (auto m : motors)
            m->SetVoltage (units::volt_t { 0.0 });
    }

    void resetEncoders() {
        for (auto* enc : encoders) {
            const auto wheelRadius = 0.1;
            enc->SetDistancePerPulse (2 * std::numbers::pi * wheelRadius / 4096);
            enc->Reset();
        }
    }

private:
    frc::Encoder encoderLeft { 10, 11 };
    frc::Encoder encoderRight { 12, 13 };
    std::array<frc::Encoder*, 2> encoders { &encoderLeft, &encoderRight };

    rev::CANSparkMax leftArm { Port::LeftArm, MotorType::kBrushless };
    rev::CANSparkMax rightArm { Port::RightArm, MotorType::kBrushless };
    std::array<rev::CANSparkMax*, 2> motors { &leftArm, &rightArm };
};
