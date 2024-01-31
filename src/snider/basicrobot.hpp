#pragma once

/** a namespace is just a grouping mechanism.... this is ours for Snider. */
namespace snider {

/** The running mode of a bot. */
enum class BotMode : int {
    Disconnected = 0,
    Autonomous,
    Teleop,
    Disabled,
    Test,
    Simulation
};

/** Base class for all robots. */
class BasicRobot {
public:
    BasicRobot()  = default;
    ~BasicRobot() = default;

    void setMode (BotMode newMode) noexcept {
        if (newMode == _mode)
            return;
        _mode = newMode;
        modeChanged();
    }

    auto mode() const noexcept { return _mode; }

protected:
    /** Override this to handle when the bot mode changes. */
    virtual void modeChanged() {}

private:
    BotMode _mode { BotMode::Disconnected };
};

} // namespace snider

namespace std {
/** Add string conversion via `std::to_string()` for our bod mode enum.  This
    isn't something that necessarily always needs done... but it will come in
    handy in print debugging, and this would be the "official" way to do it
    according to c++ (I think) -MRF
*/
inline static std::string to_string (snider::BotMode mode) noexcept {
    using snider::BotMode;

    std::string res;

    // clang-format off
    switch (mode) {
        case BotMode::Disconnected: res = "Disconnected"; break;
        case BotMode::Autonomous:   res = "Autonomous";  break;
        case BotMode::Teleop:       res = "Teleop";  break;
        case BotMode::Disabled:     res = "Disabled"; break;
        case BotMode::Test:         res = "Test";  break;
        case BotMode::Simulation:   res = "Simulation"; break; 
    }
    // clang-format on

    return res;
}

} // namespace std
