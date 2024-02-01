
#include "parameters.hpp"

#ifndef BOT_LOG_AXES
#    define BOT_LOG_AXES 0
#endif

using snider::BotMode;

void Parameters::process (const Context& context) noexcept {
    values = context;

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
