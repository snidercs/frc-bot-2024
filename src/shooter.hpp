#pragma once

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

class Shooter {
public:
    Shooter()  = default;
    ~Shooter() = default;

    void load() { 
        std::clog << "[bot] load\n";
        topMotor.SetVoltage (units::volt_t { 12.0 * 0.20 });
        bottomMotor.SetVoltage (units::volt_t { 12.0 * 0.20 });
    }
    
    void shoot() {
        std::clog << "[bot] shoot\n";
        topMotor.SetVoltage (units::volt_t { 12.0 });
        bottomMotor.SetVoltage (units::volt_t { 12.0 * 0.90 });
    }

private:
    rev::CANSparkMax bottomMotor { Port::BottomShootingWheel,
                                   rev::CANSparkLowLevel::MotorType::kBrushless };
    rev::CANSparkMax topMotor { Port::TopShootingWheel,
                                  rev::CANSparkLowLevel::MotorType::kBrushless };
};
