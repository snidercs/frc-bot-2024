#pragma once

#include <cstring> // for memcpy, memset

#include "snider/botmode.hpp"
#include "snider/messageticker.hpp"

/** The main robot class.  It is this object's job to process data coming from
    the FRC system.  Think of it as a realtime state of all the data points
    needed to control the bot.  e.g. it contains all the information needed to
    send messages to, change voltages of, etc etc.  And it gets updated in real-
    time in the frc callbacks.  And vise versa, the FRC classes call functions
    on this class to retrieve info when computing voltages etc etc etc.

    Welcome to the wonderful world of signal processing!

    By keeping FRC code out of this class, will be much much easier to port this
    base system we're building to another non-FRC system later if we want to.
*/
class Parameters final {
public:
    Parameters() {
        procTicker.enable (false);
    }

    ~Parameters() = default;

    /** The default controller port to use. */
    static constexpr int DefaultControllerPort = 0;
    /** Default joystick port, not currently used. */
    static constexpr int DefaultJoystickPort = 1;

    enum : int {
        MaxAxes    = 32,
        MaxPOVs    = 1,
        MaxButtons = 16
    };

    /** A context in which the bot runs. */
    class Context {
    public:
        Context() { reset(); }
        ~Context() = default;

        /** Copy ctor and operator.  I can explain what these are, ask me -MRF. */
        Context (const Context& o) { *this = o; }
        Context& operator= (const Context& o) noexcept {
            // This literally means memory copy, and that's exactly what it
            // does... copy raw memory from one address to another and how many
            // bytes.
            memcpy (axis, o.axis, MaxAxes * sizeof (double));
            memcpy (povs, o.povs, MaxPOVs * sizeof (int));
            return *this;
        }

        /** Storage for axis data (the thumb sticks)
            
            This is a plain C-style array with a fixed size.  It is a direct
            allocation in RAM on the stack.  Java cannot do this that I know of,
            which is to have raw, direct access to memory... lighthing fast to 
            get/set values from these types of arrays.
        */
        double axis[MaxAxes] = { 0 };

        /** POVs storage */
        int povs[MaxPOVs] = { 0 };

        /** Buttons storage */
        bool buttons[MaxButtons] = { 0 };

        /** Reset to default values */
        inline void reset() noexcept {
            memset (buttons, MaxButtons, sizeof (bool));
            memset (axis, MaxAxes, sizeof (double));
            memset (povs, MaxPOVs, sizeof (int));
        }
    };

    /** Returns the current mode in which the bot is running. */
    constexpr auto mode() const noexcept { return _mode; }

    /** Set the mode of the bot. */
    void setMode (snider::BotMode newMode) noexcept {
        if (newMode == _mode)
            return;
        _mode = newMode;
        modeChanged();
    }

    /** Returns true if in teleop mode. */
    bool isTeleop() const noexcept { return _mode == snider::BotMode::Teleop; }
    /** Returns true if in autonomous mode. */
    bool isAuto() const noexcept { return _mode == snider::BotMode::Autonomous; }

    /** Process the context, update values, prepare for replies to bot.
        
        @param context Prepared by main, called from the realtime function.
     */
    void process (const Context& context) noexcept;

    double leftStickX() const noexcept { return _lastContext.axis[0]; }
    double leftStickY() const noexcept { return _lastContext.axis[1]; }

    double rightStickX() const noexcept { return _lastContext.axis[4]; }
    double rightStickY() const noexcept { return _lastContext.axis[5]; }

protected:
    void modeChanged();

private:
    snider::MessageTicker procTicker { "[bot] process()" };
    Context _lastContext;
    snider::BotMode _mode { snider::BotMode::Disconnected };
};
