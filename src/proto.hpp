#pragma once

#include <iostream>
#include <string>

#include <boost/algorithm/string/trim.hpp>

// Prototype functions and classes to be moved to botlib.

namespace snider {

/** Logging helpers. */
namespace console {

/** Log a message or messages to the console. This version can only take string
    type arguments at the moment.  So int and float types should be converted
    before passing in here with `std::to_string` or some other way.

    Not passing string types only will result in insane template errors.
 */
template <typename... Str>
inline static void log (Str&&... msgs) {
    std::string out;
    // loop through messages and var args and store in a string buffer.
    ([&, &out = out] {
        out += msgs + std::string ("   ");
    }(),
     ...);

    // trim the last tab
    boost::trim (out);
    // render to console
    std::clog << out << std::endl;
}

} // namespace console

class MessageTicker {
public:
    MessageTicker() = delete;
    /** A new ticker.  Instantiate one and call tick() to print a message,
        every period of @delaySeconds

        @param str The message to display
        @param delaySeconds How long between readouts.
        @param intervalMs The interval in milliseconds of the realtime calling 
                          function.
    */
    MessageTicker (std::string_view str, int delaySeconds = 2, int intervalMs = 20)
        : _message (str),
          _throttleTime (delaySeconds * 1000 / std::max (1, intervalMs)),
          _tick (_throttleTime) {
    }

    /** Call this inside the realtime function cycling at `intervalMs`
        per second.
    */
    void tick() noexcept {
        if (--_tick <= 0) {
            if (enabled())
                console::log (std::to_string (++_count), _message);
            _tick = _throttleTime;
        }
    }

    void enable (bool yn = true) noexcept {
        if (_enabled == yn)
            return;
        _enabled = yn;
    }

    constexpr bool enabled() const noexcept { return _enabled; }

private:
    std::string _message;
    int _throttleTime = 0;
    int _count        = 0;   
    int _tick { 0 };
    bool _enabled { false };
};

} // namespace snider
