
#include "config.hpp"
#include "robot.hpp"

#define DEBUG_SHOOTER 0 // change to 1 to enable debug logging.
#if DEBUG_SHOOTER
#    include <iostream>
#    define SHOOTER_DBG(msg) std::clog << "[shooter] " << msg << std::endl;
#else
#    define SHOOTER_DBG(msg)
#endif

// clang-format off
Shooter::Shooter()
    : intakeTimeMs { int(1000.0 * config::number ("shooter", "intake_time")) },
      warmTimeMs { int(1000.0 * config::number ("shooter", "warmup_time")) },
      shootTimeMs { int(1000.0 * config::number ("shooter", "shoot_time")) }
{
    reset();

    primaryTop.SetInverted (false);
    primaryBottom.SetInverted (! primaryTop.GetInverted());
    for (auto* const mt : secondaryMotors) {
        mt->SetInverted (true);
    }
}
// clang-format on

void Shooter::reset() {
    _state = lastState = Idle;
    periodMs           = config::integer ("engine", "period");

    shootPower           = std::max (1.0, config::number ("shooter", "shoot_power"));
    intakePrimaryPower   = -1.0 * std::max (1.0, config::number ("shooter", "intake_primary_power"));
    intakeSecondaryPower = -1.0 * std::max (1.0, config::number ("shooter", "intake_secondary_power"));

    // clang-format off
    SHOOTER_DBG("shootPower=" << shootPower 
        << " intakePrimaryPower=" << intakePrimaryPower
        << " intakeSecondaryPower=" << intakeSecondaryPower);
    // clang-format on
}

void Shooter::intake() {
    if (_state != Idle)
        return;
    _state = Loading;
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
    SHOOTER_DBG ("shoot(): periodMs=" << periodMs
        << " warmTime=" << warmTimeMs
        << " shootTime=" << shootTimeMs
        << " ticks=" << tick
        << " delayTicks=" << delayTicks);
    // clang-format on
}

void Shooter::stop() {
    _state = Idle;
}

void Shooter::process() noexcept {
    switch (_state) {
        case Loading: {
            primaryTop.SetVoltage (units::volt_t { intakePrimaryPower });
            primaryBottom.SetVoltage (units::volt_t { intakePrimaryPower });
            secondaryTop.SetVoltage (units::volt_t { intakeSecondaryPower });
            secondaryBottom.SetVoltage (units::volt_t { intakeSecondaryPower });
            break;
        }
        case Shooting: {
            units::volt_t volts { shootPower };
            for (auto* m : primaryMotors)
                m->SetVoltage (volts);

            if (delay >= delayTicks) {
                for (auto* m : secondaryMotors)
                    m->SetVoltage (volts);
            }

            ++delay;
            break;
        }
        case Idle: {
            for (auto* m : motors)
                m->SetVoltage (units::volt_t { 0.0 });
            break;
        }
    }

    if (isShooting() && --tick <= 0) {
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
