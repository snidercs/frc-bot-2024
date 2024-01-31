#include <iostream>
#include <memory>
#include <string>

#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <frc/smartdashboard/Field2d.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "drivetrain.hpp"
#include "robot.hpp"

using BotMode = snider::BotMode;

//==============================================================================
class RobotMain : public frc::TimedRobot {
public:
    void RobotInit() override {
        // autonomous options (not used yet)
        chooser.SetDefaultOption (autoNameDefault, autoNameDefault);
        chooser.AddOption (autoNameCustom, autoNameCustom);
        frc::SmartDashboard::PutData ("Auto Modes", &chooser);

        // Assign the 2d field widget
        frc::SmartDashboard::PutData ("2d Field", &field2d);

        // A safe assumption I guess....
        robot.setMode (BotMode::Disconnected);

        gamepad = std::make_unique<frc::XboxController> (Robot::DefaultControllerPort);
    }

    /**
     * This function is called every 20 ms, no matter the mode. Use
     * this for items like diagnostics that you want ran during disabled,
     * autonomous, teleoperated and test.
     *
     * This runs after the mode specific periodic functions, but before
     * LiveWindow and SmartDashboard integrated updating.
     */
    void RobotPeriodic() override {
        Robot::Context ctx;

        // check gamepad connection status.
        const bool wasConnected = gamepadConnected;
        gamepadConnected        = gamepad != nullptr ? gamepad->IsConnected() : false;
        if (gamepadConnected != wasConnected) {
            // connection state changed. noop.
        }

        if (gamepadConnected) {
            // the buttons
            if (gamepad->GetAButtonPressed()) {
                std::clog << "[frc] A pressed\n";
            } else if (gamepad->GetAButtonReleased()) {
                std::clog << "[frc] A released\n";
            }

            if (gamepad->GetBButtonPressed()) {
                std::clog << "[frc] B pressed\n";
            } else if (gamepad->GetBButtonReleased()) {
                std::clog << "[frc] B released\n";
            }

            if (gamepad->GetXButtonPressed()) {
                std::clog << "[frc] X pressed\n";
            } else if (gamepad->GetXButtonReleased()) {
                std::clog << "[frc] X released\n";
            }

            if (gamepad->GetYButtonPressed()) {
                std::clog << "[frc] Y pressed\n";
            } else if (gamepad->GetYButtonReleased()) {
                std::clog << "[frc] Y released\n";
            }

            // the bumpers
            if (gamepad->GetLeftBumperPressed()) {
                std::clog << "[frc] left bumper pressed\n";
            } else if (gamepad->GetLeftBumperReleased()) {
                std::clog << "[frc] left bumper released\n";
            }

            if (gamepad->GetRightBumperPressed()) {
                std::clog << "[frc] right bumper pressed\n";
            } else if (gamepad->GetRightBumperReleased()) {
                std::clog << "[frc] right bumper released\n";
            }

            // Using 'std::min' garauntees that the 'Robot::Context::axis' array doesn't
            // overflow...  if it did... the firmware would crash in a very bad way.
            for (int i = std::min (gamepad->GetAxisCount(), (int) Robot::MaxAxes); --i >= 0;) {
                ctx.axis[i] = gamepad->GetRawAxis (i);
                // needed? gamepad->GetAxisType()
            }

            ctx.povs[0] = gamepad->GetPOVCount() <= 0 ? 0 : gamepad->GetPOV (0);

            // button indexing in FRC starts at 1 for some reason.
            for (int i = std::min (gamepad->GetButtonCount(), (int) Robot::MaxButtons); --i >= 0;) {
                ctx.buttons[i] = gamepad->GetRawButton (i + 1);
            }
        }

        // Now that the context is ready, pass it on over to our pure C++ robot
        // processor.
        robot.process (ctx);

        // positioning.
        // field2d.SetRobotPose (odometry.GetPose());
        if (true) { // (robot.isTeleop() || robot.isAuto()) {
            auto speed    = 1.0 * robot.rightStickY();
            auto rotation = 0.0; // acquire from context
            // std::clog << "speed=" << speed << std::endl;
            drivetrain.drive (Drivetrain::MaxSpeed, Drivetrain::MaxAngularSpeed);
            drivetrain.updateOdometry();
            field2d.SetRobotPose (drivetrain.pose2d());
        } else {
            drivetrain.drive (0.0, 0.0);
        }
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
    void AutonomousInit() override {
        autoSelected = chooser.GetSelected();
        autoSelected = frc::SmartDashboard::GetString ("Auto Selector", autoNameDefault);
        fmt::print ("Auto selected: {}\n", autoSelected);

        if (autoSelected == autoNameCustom) {
            // Custom Auto goes here
        } else {
            // Default Auto goes here
        }

        robot.setMode (BotMode::Autonomous);
    }

    void AutonomousPeriodic() override {
        if (autoSelected == autoNameCustom) {
            // Custom Auto goes here
        } else {
            // Default Auto goes here
        }
    }

    void TeleopInit() override {
        robot.setMode (BotMode::Teleop);
    }

    void TeleopPeriodic() {}

    void DisabledInit() override {
        robot.setMode (BotMode::Disabled);
    }

    void DisabledPeriodic() override {}

    void TestInit() override {
        robot.setMode (BotMode::Test);
    }

    void TestPeriodic() override {}

    void SimulationInit() override {
        // robot.setMode (BotMode::Simulation);
    }

    void SimulationPeriodic() override {}

private:
    Robot robot;

    std::string autoSelected;
    frc::SendableChooser<std::string> chooser;
    const std::string autoNameDefault = "Default";
    const std::string autoNameCustom  = "My Auto";

    std::unique_ptr<frc::XboxController> gamepad;

    frc::Field2d field2d;
    Drivetrain drivetrain;

    bool gamepadConnected = false;
};

#ifndef RUNNING_FRC_TESTS
int main() {
    return frc::StartRobot<RobotMain>();
}
#endif