
#include <cmath>
#include <vector>

#include "parameters.hpp"

#ifndef BOT_LOG_AXES
#    define BOT_LOG_AXES 0
#endif

using snider::BotMode;

void Parameters::process (const Context& context) noexcept {
    values = context;

    // A vector is a list of things.  In this case, integer indexes.
    std::vector<int> indexes = { LeftStickX, LeftStickY, RightStickX, RightStickY };
    // This is another type of for loop: called a 'foreach' loop.
    // more on loops: https://www.w3schools.com/cpp/cpp_for_loop.asp
    for (auto i : indexes) {
        if (std::abs (values.axis[i]) <= .005) {
            values.axis[i] = 0;
        }
    }

#if BOT_LOG_AXES
    for (int i = 0; i < 6; ++i)
        if (lastContext.axis[i] != context.axis[i])
            std::clog << "[bot] axis #" << i << " = " << context.axis[i] << std::endl;

    for (int i = 0; i < 1; ++i)
        if (lastContext.povs[i] != context.povs[i])
            std::clog << "[bot] dpad #" << i << " = " << context.povs[i] << std::endl;
#endif

    // Save the context in the previous one for change detection and future
    // interpolations...
    lastContext = values;
}

void Parameters::modeChanged() {
    std::clog << "[params] bot mode changed: " << std::to_string (mode()) << std::endl;
}
