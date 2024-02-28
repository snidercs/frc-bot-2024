#pragma once

#include <frc/Encoder.h>
#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

#define MECHANICAL_ARM_ENABLED 1

class MechanicalArm {
public:
    MechanicalArm() {
        follower.Follow (leader);
        resetEncoders();
    }

    ~MechanicalArm() = default;

    void moveUp() {
        leader.SetVoltage (units::volt_t { 0.3 * 12.0 });
    }

    void moveDown() {
        leader.SetVoltage (units::volt_t { 0.3 * -12.0 });
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

#if MECHANICAL_ARM_ENABLED
    rev::CANSparkMax follower { Port::LeftArm,
                                rev::CANSparkLowLevel::MotorType::kBrushless };
    rev::CANSparkMax leader { Port::RightArm,
                              rev::CANSparkLowLevel::MotorType::kBrushless };
#endif
};
