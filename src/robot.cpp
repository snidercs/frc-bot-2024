#include <fmt/core.h>

#include "robot.hpp"

using snider::BasicRobot;
using snider::BotMode;

void Robot::process (const Context& context) noexcept {
    procTicker.tick();

    // without saying to much;  this loops through the axes and prints
    // if the value has changed.
    for (int i = 0; i < 6; ++i)
        if (_lastContext.axis[i] != context.axis[i])
            std::clog << "[bot] axis #" << i << " = " << context.axis[i] << std::endl;

    // same for dpad
    for (int i = 0; i < 1; ++i)
        if (_lastContext.povs[i] != context.povs[i])
            std::clog << "[bot] dpad #" << i << " = " << context.povs[i] << std::endl;

    // Save the context in the previous one for change detection and future
    // interpolations...
    _lastContext = context;
}

void Robot::modeChanged() {
    std::clog << "[bot] mode changed: " << std::to_string (mode()) << std::endl;
}
