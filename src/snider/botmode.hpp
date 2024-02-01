#pragma once

#include <string>

namespace snider {

/** The running mode of a bot. */
enum class BotMode : int {
    Disconnected = 0,
    Autonomous,
    Teleop,
    Disabled,
    Test
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
    }
    // clang-format on

    return res;
}

} // namespace std
