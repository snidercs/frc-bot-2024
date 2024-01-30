#include <fmt/core.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "robot.hpp"

using snider::BasicRobot;
using snider::BotMode;

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
    }

    // then at the end process it here by adding a parameter, or something else
    // not exactly sure yet... but this is my plan -MRF
    _robot.process();
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
