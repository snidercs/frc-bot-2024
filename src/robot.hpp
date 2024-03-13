
#include <algorithm>
#include <array>

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "lua.hpp"
#include "types.hpp"

/** The shooter interface.

    Controls motors used in loading notes and shooting them.  It operates as a 
    state machine with three states: Idle, Shooting, Loading. Idle can 
    transition to Shooting or Loading then back to Idle.  Shooting and Loading 
    do not transition directly to each other which prevents shoot/load overlaps.
*/
class Shooter {
public:
    Shooter();

    ~Shooter() = default;

    /** State of this shooter. */
    enum State : int {
        Idle,     ///> Idle state. not shooting. not loading.
        Shooting, ///> Is shooting.
        Loading   ///> is Loading.
    };

    /** Reset to default values. */
    void reset();

    /** Prepare motors to load if not loading or shooting. */
    void load();

    /** Prepare motors to shoot if not already loading or shooting. */
    void shoot();

    /** Stop the motors. */
    void stop();

    /** Returns the current state. */
    constexpr State state() const noexcept { return _state; }

    /** Returns true if in Idle state. */
    constexpr bool isIdle() const noexcept { return _state == Idle; }

    /** Returns true if in Loading state. */
    constexpr bool isLoading() const noexcept { return _state == Loading; }

    /** Returns true if in Shooting state. */
    constexpr bool isShooting() const noexcept { return _state == Shooting; }

    /** Set appropriate motor speed and update state if needed. */
    void process() noexcept;

private:
    friend class RobotMain;
    /** Bind to lua. see bindings.cpp */
    static void bind (Shooter*);

    State _state { Idle };
    State lastState { Idle };

    // see `config.lua`
    const int intakeTimeMs;
    const int warmTimeMs;
    const int shootTimeMs;

    int totalTimeMs {};
    int periodMs { 20 };
    int tick       = 0;
    int delay      = 0;
    int delayTicks = 2; // delayTicks x 20ms = totalDelay

    rev::CANSparkMax topMotor {
        lua::config::port ("shooter_primary"),
        MotorType::kBrushed
    };

    rev::CANSparkMax bottomMotor {
        lua::config::port ("shooter_secondary"),
        MotorType::kBrushed
    };

    std::array<rev::CANSparkMax*, 2> motors { &topMotor, &bottomMotor };

    std::string stateString() const noexcept;
};
