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
    Parameters()  = default;
    ~Parameters() = default;

    Parameters (const Parameters& o) { operator= (o); }
    Parameters& operator= (const Parameters& o) {
        values      = o.values;
        lastContext = o.lastContext;
        return *this;
    }

    Parameters (Parameters&& o) { operator= (std::forward<Parameters> (o)); }
    Parameters& operator= (Parameters&& o) {
        values      = std::move (o.values);
        lastContext = std::move (o.lastContext);
        return *this;
    }

    enum : int {
        MaxAxes    = 32, ///> Max number of axes supported.
        MaxPOVs    = 1,  ///> Max number of dpads supported.
        MaxButtons = 16  ///> Max number of buttons supported.
    };

    enum Indexes : int {
        LeftStickX   = 0, ///> Index of Left stick X
        LeftStickY   = 1, ///> Index of Left stick Y
        TriggerLeft  = 2, ///> Index of Trigger left
        TriggerRight = 3, ///> Index of Trigger right
        RightStickX  = 4, ///> Index of Right stick X
        RightStickY  = 5  ///> Index of Right stick Y
    };

    // I don't think this works gotta check later -MC
    enum : int {
        ButtonA           = 0,
        ButtonB           = 1,
        ButtonX           = 2,
        ButtonY           = 3,
        ButtonRightBumper = 4,
        ButtonLeftBumper  = 5,
        ButtonBack        = 6,
        ButtonStart       = 7,
        ButtonHome        = 8,
        ButtonL3          = 9,
        ButtonR3          = 10

    };
    /** A context in which controller values get processed runs. The context 
        holds a cache of raw values read from the Xbox, or other, controller.
     */
    class Context {
    public:
        Context() { reset(); }
        ~Context() = default;

        /** Copy ctor and opera
        if (params.get_value(arm_motor_controller_trigger_away) > 0){

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

    /** Returns the axis value of the given axis */
    double getAxisValue (int axisParam) const noexcept {
        return values.axis[axisParam];
    }

    /** Returns the processed left stick X value. */
    double getLeftStickX() const noexcept { return getAxisValue (LeftStickX); }

    /** Returns the processed left stick Y value. */
    double getLeftStickY() const noexcept { return getAxisValue (LeftStickY); }

    /** Returns the processed right stick X value. */
    double getRightStickX() const noexcept { return getAxisValue (RightStickX); }

    /** Returns the processed right stick Y value. */
    double getRightStickY() const noexcept { return getAxisValue (RightStickY); }

    /** Returns the processed left trigger value. */
    double getTriggerLeft() const noexcept { return getAxisValue (TriggerLeft); }

    /** Returns the processed right trigger value. */
    double getTriggerRight() const noexcept { return getAxisValue (TriggerRight); }

    /** Return processed passed value. */
    bool getButtonValue (int givenParam) const noexcept {
        return values.buttons[givenParam];
    }
    /** Returns the processed A button value . */
    bool getAButton() const noexcept {
        return getButtonValue (ButtonA);
    }
    /** Returns the processed B button value . */
    bool getBButton() const noexcept {
        return getButtonValue (ButtonB);
    }
    /** Returns the processed X button value . */
    bool getXButton() const noexcept {
        return getButtonValue (ButtonX);
    }
    /** Returns the processed Y button value . */
    bool getYButton() const noexcept {
        return getButtonValue (ButtonY);
    }
    /** Returns the processed right bumper button value . */
    bool getRightBumperButton() const noexcept {
        return getButtonValue (ButtonRightBumper);
    }
    /** Returns the processed left bumper button value . */
    bool getLeftBumperButton() const noexcept {
        return getButtonValue (ButtonLeftBumper);
    }
    /** Returns the processed back button value . */
    bool getBackButton() const noexcept {
        return getButtonValue (ButtonBack);
    }
    /** Returns the processed start button value . */
    bool getStartButton() const noexcept {
        return getButtonValue (ButtonStart);
    }
    /** Returns the processed home button value . */
    bool getHomeButton() const noexcept {
        return getButtonValue (ButtonHome);
    }
    /** Returns the processed l3 button value . */
    bool getL3Button() const noexcept {
        return getButtonValue (ButtonL3);
    }
    /** Returns the processed r3 button value . */
    bool getR3Button() const noexcept {
        return getButtonValue (ButtonR3);
    }
    /** Return processed passed value. */
    int getPOVValue (int givenParam) const noexcept {
        return values.povs[givenParam];
    }

protected:
    void modeChanged();

private:
    Context values;
    Context lastContext;
    snider::BotMode _mode { snider::BotMode::Disconnected };
};
