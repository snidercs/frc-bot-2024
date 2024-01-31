#include <fmt/core.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "robot.hpp"

using snider::BasicRobot;
using snider::BotMode;

//==============================================================================
void Robot::process (const Context& context) noexcept {
    _procTicker.tick();

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
    // clang-format off
    _procTicker.enable (mode() == BotMode::Autonomous || 
                        mode() == BotMode::Teleop);
    std::clog << "[bot] mode changed: " << std::to_string (mode()) << std::endl;
    // clang-format on
}

//==============================================================================
void RobotMain::RobotInit() {
    m_chooser.SetDefaultOption (kAutoNameDefault, kAutoNameDefault);
    m_chooser.AddOption (kAutoNameCustom, kAutoNameCustom);
    frc::SmartDashboard::PutData ("Auto Modes", &m_chooser);

    // A safe assumption I guess....
    _robot.setMode (BotMode::Disconnected);

    _gamepad = std::make_unique<frc::XboxController> (Robot::DefaultControllerPort);
}

/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want ran during disabled,
 * autonomous, teleoperated and test.
 *
 * This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void RobotMain::RobotPeriodic() {
    Robot::Context ctx;

    // all these if elses and what not will eventually update properties on the
    // Context struct...

    if (_gamepad->IsConnected()) {
        // the buttons
        if (_gamepad->GetAButtonPressed()) {
            std::clog << "[frc] A pressed\n";
        } else if (_gamepad->GetAButtonReleased()) {
            std::clog << "[frc] A released\n";
        }

        if (_gamepad->GetBButtonPressed()) {
            std::clog << "[frc] B pressed\n";
        } else if (_gamepad->GetBButtonReleased()) {
            std::clog << "[frc] B released\n";
        }

        if (_gamepad->GetXButtonPressed()) {
            std::clog << "[frc] X pressed\n";
        } else if (_gamepad->GetXButtonReleased()) {
            std::clog << "[frc] X released\n";
        }

        if (_gamepad->GetYButtonPressed()) {
            std::clog << "[frc] Y pressed\n";
        } else if (_gamepad->GetYButtonReleased()) {
            std::clog << "[frc] Y released\n";
        }

        // the bumpers
        if (_gamepad->GetLeftBumperPressed()) {
            std::clog << "[frc] left bumper pressed\n";
        } else if (_gamepad->GetLeftBumperReleased()) {
            std::clog << "[frc] left bumper released\n";
        }

        if (_gamepad->GetRightBumperPressed()) {
            std::clog << "[frc] right bumper pressed\n";
        } else if (_gamepad->GetRightBumperReleased()) {
            std::clog << "[frc] right bumper released\n";
        }

        // Using 'std::min' garauntees that the 'Robot::Context::axis' array doesn't
        // overflow...  if it did... the firmware would crash in a very bad way.
        for (int i = std::min (_gamepad->GetAxisCount(), (int) Robot::MaxAxes); --i >= 0;) {
            ctx.axis[i] = _gamepad->GetRawAxis (i);
            // needed? _gamepad->GetAxisType()
        }

        ctx.povs[0] = _gamepad->GetPOVCount() <= 0 ? 0 : _gamepad->GetPOV (0);

        // button indexing in FRC starts at 1 for some reason.
        for (int i = std::min (_gamepad->GetButtonCount(), (int) Robot::MaxButtons); --i >= 0;) {
            ctx.buttons[i] = _gamepad->GetRawButton (i + 1);
        }
    }

    // Now that the context is ready, pass it on over to our pure C++ robot
    // processor.
    _robot.process (ctx);

    // now that the bot has processed and updated itself, send out whatever is
    // needed to the motor controllers etc etc...
}

/**
 * This autonomous (along with the chooser code above) shows how to select
 * between different autonomous modes using the dashboard. The sendable chooser
 * code works with the Java SmartDashboard. If you prefer the LabVIEW Dashboard,
 * remove all of the chooser code and uncomment the GetString line to get the
 * auto name from the text box below the Gyro.
 *
 * You can add additional auto modes by adding additional comparisons to the
 * if-else structure below with additional strings. If using the SendableChooser
 * make sure to add them to the chooser code above as well.
 */
void RobotMain::AutonomousInit() {
    m_autoSelected = m_chooser.GetSelected();
    m_autoSelected = frc::SmartDashboard::GetString ("Auto Selector", kAutoNameDefault);
    fmt::print ("Auto selected: {}\n", m_autoSelected);

    if (m_autoSelected == kAutoNameCustom) {
        // Custom Auto goes here
    } else {
        // Default Auto goes here
    }

    _robot.setMode (BotMode::Autonomous);
}

void RobotMain::AutonomousPeriodic() {
    if (m_autoSelected == kAutoNameCustom) {
        // Custom Auto goes here
    } else {
        // Default Auto goes here
    }
}

void RobotMain::TeleopInit() {
    _robot.setMode (BotMode::Teleop);
}

void RobotMain::TeleopPeriodic() {}

void RobotMain::DisabledInit() {
    _robot.setMode (BotMode::Disabled);
}

void RobotMain::DisabledPeriodic() {}

void RobotMain::TestInit() {
    _robot.setMode (BotMode::Test);
}

void RobotMain::TestPeriodic() {}

void RobotMain::SimulationInit() {
    _robot.setMode (BotMode::Simulation);
}

void RobotMain::SimulationPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() {
    return frc::StartRobot<RobotMain>();
}
#endif
