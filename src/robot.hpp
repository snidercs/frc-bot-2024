#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <frc/smartdashboard/SendableChooser.h>

#include "snider/basicrobot.hpp"
#include "snider/messageticker.hpp"

// In case you are tempted... doing things like   "using namespace xyz;"   in
// a header can and will cause many compiler errors when other headers try to
// include it and names start clashing. Best to keep all the "using" stuff inside
// a class or in *.cpp files.... because cpp files don't get included elsewhere.

//==============================================================================
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
class Robot : public snider::BasicRobot {
public:
    /** The default controller port to use. */
    static constexpr int DefaultControllerPort = 0;
    /** Default joystick port, not currently used. */
    static constexpr int DefaultJoystickPort = 1;

    /** A context in which the bot runs. */
    struct Context {
    };

    void process() noexcept {
        _procTicker.tick();
    }

protected:
    snider::MessageTicker _procTicker { "[bot] process()" };
    void modeChanged() override {
        // clang-format off
        // Utilize that "Mode" alias as shown below.
        _procTicker.enable (mode() == Mode::Autonomous || 
                            mode() == Mode::Teleop);
        std::clog << "[bot] mode changed: " << std::to_string (mode()) << std::endl;
        // clang-format on
    }

private:
    // This kind of thing is common to do in the private: section in c++.  The
    // verbosity of the language is unavoidable.  It's a way of aliasing a long
    // typename with all the namespaces and what not but not affect all of the
    // other code that might use this class.
    using Mode = snider::BotMode;
};

//==============================================================================
class RobotMain : public frc::TimedRobot {
public:
    void RobotInit() override;
    void RobotPeriodic() override;
    void AutonomousInit() override;
    void AutonomousPeriodic() override;
    void TeleopInit() override;
    void TeleopPeriodic() override;
    void DisabledInit() override;
    void DisabledPeriodic() override;
    void TestInit() override;
    void TestPeriodic() override;
    void SimulationInit() override;
    void SimulationPeriodic() override;

private:
    Robot _robot;
    std::string m_autoSelected;
    frc::SendableChooser<std::string> m_chooser;
    const std::string kAutoNameDefault = "Default";
    const std::string kAutoNameCustom  = "My Auto";

    std::unique_ptr<frc::XboxController> _gamepad;
};
