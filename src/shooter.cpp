#include "robot.hpp"
#include "scripting.hpp"

namespace cfg = lua::config;

#define DEBUG_SHOOTER 0 // change to 1 to enable debug logging.
#if DEBUG_SHOOTER
#    include <iostream>
#    define SHOOTER_LOG(msg) std::clog << msg << std::endl;
#else
#    define SHOOTER_DBG(msg)
#endif

// clang-format off
Shooter::Shooter()
    : intakeTimeMs { int(1000.0 * cfg::get ("shooter", "intake_time").as<lua_Number>()) },
      warmTimeMs { int(1000.0 * cfg::get ("shooter", "warmup_time").as<lua_Number>()) },
      shootTimeMs { int(1000.0 * cfg::get ("shooter", "shoot_time").as<lua_Number>()) }
{
    reset();
    for (auto* const mt : motors) {
        mt->SetInverted (true);
    }
}
// clang-format on

void Shooter::reset() {
    _state = lastState = Idle;
    periodMs           = static_cast<int> (
        cfg::get ("engine", "period").as<lua_Number>());
}

void Shooter::load() {
    if (_state != Idle)
        return;
    _state      = Loading;
    totalTimeMs = intakeTimeMs;
    tick        = std::max (periodMs, intakeTimeMs / periodMs);
}

void Shooter::shoot() {
    if (_state != Idle)
        return;

    _state      = Shooting;
    totalTimeMs = warmTimeMs + shootTimeMs;
    tick        = std::max (periodMs, totalTimeMs / periodMs);
    delayTicks  = std::max (periodMs, warmTimeMs / periodMs);
    delay       = 0;

    // clang-format off
    SHOOTER_DBG ("shoot period=" << periodMs
        << " warm=" << warmTimeMs
        << " shoot=" << shootTimeMs
        << " tick =" << tick
        << " delayTicks=" << delayTicks);
    // clang-format on
}

void Shooter::stop() {
    _state = Idle;
}

void Shooter::process() noexcept {
    switch (_state) {
        case Loading: {
            topMotor.SetVoltage (units::volt_t { -6.0 });
            bottomMotor.SetVoltage (units::volt_t { -3.0 });
            supportMotor.SetVoltage (units::volt_t { -3.0 });
            break;
        }
        case Shooting: {
            topMotor.SetVoltage (units::volt_t { 6.0 });
            supportMotor.SetVoltage (units::volt_t { 6.0 });
            if (delay >= delayTicks) {
                bottomMotor.SetVoltage (units::volt_t { 6.0 });
            }
            ++delay;
            break;
        }
        case Idle: {
            topMotor.SetVoltage (units::volt_t { 0.0 });
            bottomMotor.SetVoltage (units::volt_t { 0.0 });
            supportMotor.SetVoltage (units::volt_t { 0.0 });
            break;
        }
    }

    if (! isIdle() && --tick <= 0) {
        // shoot load seq. finished. transition back to Idle.
        _state = Idle;
    }

    lastState = _state;
}

std::string Shooter::stateString() const noexcept {
    switch (_state) {
        case Shooting:
            return "shoot!";
            break;
        case Loading:
            return "load!";
            break;
        case Idle:
            return "idle...";
            break;
    }

    return "unknown";
}
