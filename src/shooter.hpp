#pragma once

#include <algorithm>
#include <array>

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"

/** The shooter interface.

    Controls motors used in loading notes and shooting them.  It operates as a 
    state machine with three states: Idle, Shooting, Loading. Idle can 
    transition to Shooting or Loading then back to Idle.  Shooting and Loading 
    do not transition directly to each other which prevents shoot/load overlaps.
*/
class Shooter {
public:
    Shooter() {
        reset();
        for (auto* const mt : motors) {
            mt->SetInverted (true);
        }
    }

    ~Shooter() {}

    /** State of this shooter. */
    enum State {
        Idle,     ///> Idle state. not shooting. not loading.
        Shooting, ///> Is shooting.
        Loading   ///> is Loading.
    };

    /** Reset to default values. */
    void reset() {
        _state = lastState = Idle;
        loadDurationMs     = 600;
        shootDurationMs    = 1000;
        periodMs           = 20;
    }

    /** Prepare motors to load if not loading or shooting. */
    void load() {
        if (_state != Idle)
            return;
        _state = Loading;
        tick   = std::max (5, loadDurationMs / periodMs);
    }

    /** Prepare motors to shoot if not already loading or shooting. */
    void shoot() {
        if (_state != Idle)
            return;
        _state     = Shooting;
        tick       = std::max (5, shootDurationMs / periodMs);
        delayTicks = std::max (1, tick / 2);
        delay      = 0;
    }

    /** Stop the motors. */
    void stop() {
        _state = Idle;
    }

    /** Returns the current state. */
    constexpr State state() const noexcept { return _state; }
    /** Returns true if in Idle state. */
    constexpr bool isIdle() const noexcept { return _state == Idle; }
    /** Returns true if in Loading state. */
    constexpr bool isLoading() const noexcept { return _state == Loading; }
    /** Returns true if in Shooting state. */
    constexpr bool isShooting() const noexcept { return _state == Shooting; }

    /** Set appropriate motor speed and update state if needed. */
    void process() noexcept {
        switch (_state) {
            case Loading: {
                topMotor.SetVoltage (units::volt_t { -6.0 });
                bottomMotor.SetVoltage (units::volt_t { -3.0 });
                break;
            }
            case Shooting: {
                topMotor.SetVoltage (units::volt_t { 16.0 });
                if (delay >= delayTicks)
                    bottomMotor.SetVoltage (units::volt_t { 16.0 });
                ++delay;
                break;
            }
            case Idle: {
                topMotor.SetVoltage (units::volt_t { 0.0 });
                bottomMotor.SetVoltage (units::volt_t { 0.0 });
                break;
            }
        }

        if (! isIdle() && --tick <= 0) {
            // shoot load seq. finished. transition back to Idle.
            _state = Idle;
        }

        lastState = _state;
    }

private:
    using IdleMode  = rev::CANSparkBase::IdleMode;
    using MotorType = rev::CANSparkLowLevel::MotorType;

    State _state { Idle };
    State lastState { Idle };
    int loadDurationMs { 600 };
    int shootDurationMs { 2000 };

    int periodMs { 20 };
    int tick       = 0;
    int delay      = 0;
    int delayTicks = 2; // delayTicks x 20ms = totalDelay

    rev::CANSparkMax bottomMotor { Port::BottomShootingWheel, MotorType::kBrushed };
    rev::CANSparkMax topMotor { Port::TopShootingWheel, MotorType::kBrushed };
    std::array<rev::CANSparkMax*, 2> motors { &topMotor, &bottomMotor };

    std::string stateString() const noexcept {
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
};
