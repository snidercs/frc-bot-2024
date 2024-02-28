#pragma once

#include <array>

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

/** The shooter interface.

    Controls motors used in loading notes and shooting
    them.  It operates as a state machine with three states: Idle, Shooting, 
    Loading. Idle can transition to Shooting or Loading then back.  Shooting and 
    Loading do not transition directly to each other which prevents shoot/load 
    overlaps.
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
        loadSpeed          = -0.20;
        loadDurationMs     = 600;
        shootSpeed         = 1.0;
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
        _state = Shooting;
        tick   = std::max (5, shootDurationMs / periodMs);
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
        // clang-format off
        const auto speed = _state == Shooting ? shootSpeed :
                           _state == Loading ? loadSpeed :
                           _state == Idle ? 0.0 : 0.0;
        // clang-format on

        if (_state != lastState && _state != Idle) {
            // if state changed and not idle....
            std::clog << "[bot] " << stateString() << std::endl;
        }

        // topMotor.Set (speed);
        switch (_state) {
            case Loading:
                topMotor.SetVoltage (units::volt_t { -6.0 });
                bottomMotor.SetVoltage (units::volt_t { -3.0 });
                break;
            case Shooting:
                topMotor.SetVoltage (units::volt_t { 12.0 });
                bottomMotor.SetVoltage (units::volt_t { 12.0 });
                break;
        }

        if (! isIdle() && --tick <= 0) {
            std::clog << "[bot] shoot/load sequence finished.\n";
            _state = Idle;
        }

        lastState = _state;
    }

private:
    using IdleMode  = rev::CANSparkBase::IdleMode;
    using MotorType = rev::CANSparkLowLevel::MotorType;

    State _state { Idle };
    State lastState { Idle };
    double loadSpeed { -0.20 };
    int loadDurationMs { 600 };
    double shootSpeed { 1.0 };
    int shootDurationMs { 1000 };
    int periodMs { 20 };
    int tick = 0;

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
