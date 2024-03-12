#pragma once

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/velocity.h>

#include <rev/CANSparkBase.h>
#include <rev/CANSparkLowLevel.h>

using IdleMode  = rev::CANSparkBase::IdleMode;
using MotorType = rev::CANSparkLowLevel::MotorType;

using MetersPerSecond  = units::meters_per_second_t;
using RadiansPerSecond = units::radians_per_second_t;