
#include "parameters.hpp"

using snider::BotMode;

void Parameters::process (const Context& context) noexcept {
    procTicker.tick();
#if 0
    // without saying to much;  this loops through the axes and prints
    // if the value has changed.
    for (int i = 0; i < 6; ++i)
        if (_lastContext.axis[i] != context.axis[i])
            std::clog << "[bot] axis #" << i << " = " << context.axis[i] << std::endl;

    // same for dpad
    for (int i = 0; i < 1; ++i)
        if (_lastContext.povs[i] != context.povs[i])
            std::clog << "[bot] dpad #" << i << " = " << context.povs[i] << std::endl;
#endif
    // Save the context in the previous one for change detection and future
    // interpolations...
    _lastContext = context;
}

void Parameters::modeChanged() {
    std::clog << "[params] bot mode changed: " << std::to_string (mode()) << std::endl;
}
