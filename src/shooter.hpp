#pragma once

#include <array>

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "ports.hpp"
#include "snider/console.hpp"

class Shooter {
public:
    Shooter() {
        bottomMotor.Follow (topMotor, false);
        for (auto* const mt : motors) {
            mt->Set (0.0);
        }
    }

    ~Shooter() {}

    enum State {
        Idle,     ///> Idle state. not shooting. not loading.
        Shooting, ///> Is shooting.
        Loading   ///> is Loading.
    };

    /** Prepare motors to load if not loading or shooting. */
    void load() {
        if (_state != Idle)
            return;
        std::clog << "[bot] prepare to load\n";
        _state = Loading;
        tick   = std::max (5, loadDurationMs / periodMs);
    }

    /** Prepare motors to shoot if not already loading or shooting. */
    void shoot() {
        if (_state != Idle)
            return;
        std::clog << "[bot] prepare to shoot\n";
        _state = Shooting;
        tick   = std::max (5, shootDurationMs / periodMs);
    }

    /** Stop the motors. */
    void stop() {
        _state = Idle;
    }

    constexpr State state() const noexcept { return _state; }

    constexpr bool isIdle() const noexcept { return _state == Idle; }
    constexpr bool isLoading() const noexcept { return _state == Loading; }
    constexpr bool isShooting() const noexcept { return _state == Shooting; }

    /** Set appropriate motor speed and update state if needed. */
    void process() {
        // clang-format off
        const auto speed = _state == Shooting ? shootSpeed :
                           _state == Loading ? loadSpeed :
                           _state == Idle ? 0.0 : 0.0;
        
        if (_state != lastState && _state != Idle) {
            // if state changed and not idle....
            std::clog << "[bot] " << stateString() << std::endl;
        }

        topMotor.Set (speed);

        // clang-format on

        if (--tick <= 0) {
            if (! isIdle()) {
                std::clog << "[bot] shoot/load sequence finished.\n";
            }
            _state = Idle;
        }

        lastState = _state;
    }

private:
    State _state { Idle };
    State lastState { Idle };
    double loadSpeed { -0.20 };
    double shootSpeed { 1.0 };
    int shootDurationMs { 1000 };
    int loadDurationMs { 600 };
    int periodMs { 20 };
    int tick = 0;

    using MotorType = rev::CANSparkLowLevel::MotorType;
    rev::CANSparkMax bottomMotor { Port::BottomShootingWheel, MotorType::kBrushless };
    rev::CANSparkMax topMotor { Port::TopShootingWheel, MotorType::kBrushless };
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
