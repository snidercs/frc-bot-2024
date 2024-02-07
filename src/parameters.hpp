#pragma once

#include <cstring> // for memcpy, memset

#include "snider/botmode.hpp"
#include "snider/messageticker.hpp"

/** Parameter state. e.g. Raw controller value storage and filtering.
 
    It is this object's job to process data coming from the FRC system.  Think 
    of it  as a realtime state of all the data points needed to control the bot.  
    e.g. it contains all the information needed to send messages to, change 
    voltages of, etc etc. It gets updated in real-time in the frc callbacks. And 
    vise versa, the FRC classes call functions on this class to retrieve info 
    when calculating voltages etc etc etc.

    General Usage: Stick a Parameters object in your main class.  Then inside the
    periodic functions instantiate a Parameters::Context, copy raw joystick and
    button values, then pass to Parameters::process().
*/
class Parameters final {
public:
    Parameters() = default;
    ~Parameters() = default;

    /** The default controller port to use. */
    static constexpr int DefaultControllerPort = 0;
    /** Default joystick port, not currently used. */
    static constexpr int DefaultJoystickPort = 1;

    Parameters (const Parameters& o) { operator= (o); }
    Parameters& operator= (const Parameters& o) {
        values = o.values;
        lastContext = o.lastContext;
        return *this;
    }

    Parameters (Parameters&& o) { operator= (std::forward<Parameters>(o)); }
    Parameters& operator= (Parameters&& o) {
        values = std::move (o.values);
        lastContext = std::move(o.lastContext);
        return *this;
    }

    enum : int {
        MaxAxes    = 32,    ///> Max number of axes supported.
        MaxPOVs    = 1,     ///> Max number of dpads supported.
        MaxButtons = 16     ///> Max number of buttons supported.
    };

    enum Indexes : int {
        LeftStickX   = 0, ///> Index of Left stick X
        LeftStickY   = 1, ///> Index of Left stick Y
        TriggerLeft  = 2, ///> Index of Trigger left
        TriggerRight = 3, ///> Index of Trigger right
        RightStickX  = 4, ///> Index of Right stick X
        RightStickY  = 5  ///> Index of Right stick Y
    };

    /** A context in which controller values get processed runs. The context 
        holds a cache of raw values read from the Xbox, or other, controller.
     */
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

        /** Storage for axis data (the thumb sticks and triggers)
            
            This is a plain C-style array with a fixed size.  It is a direct
            allocation in RAM on the stack.  Java cannot do this that I know of,
            which is to have raw, direct access to memory... lighthing fast to 
            get/set values from these types of arrays.
        */
        double axis[MaxAxes] = { 0 };

        /** POVs storage. */
        int povs[MaxPOVs] = { 0 };

        /** Buttons storage. */
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

    /** Returns the processed left stick X value. */
    double leftStickX() const noexcept { return values.axis[LeftStickX]; }

    /** Returns the processed left stick Y value. */
    double leftStickY() const noexcept { return values.axis[LeftStickY]; }

    /** Returns the processed right stick X value. */
    double rightStickX() const noexcept { return values.axis[RightStickX]; }

    /** Returns the processed right stick Y value. */
    double rightStickY() const noexcept { return values.axis[RightStickY]; }

    /** Returns the processed left trigger value. */
    double triggerLeft() const noexcept { return values.axis[TriggerLeft]; }

    /** Returns the processed right trigger value. */
    double triggerRight() const noexcept { return values.axis[TriggerRight]; }

protected:
    void modeChanged();

private:
    Context values;
    Context lastContext;
    snider::BotMode _mode { snider::BotMode::Disconnected };
};
