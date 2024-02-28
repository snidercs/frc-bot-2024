#include <iostream>
#include <memory>
#include <string>

#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <frc/controller/RamseteController.h>
#include <frc/filter/SlewRateLimiter.h>
#include <frc/smartdashboard/Field2d.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/trajectory/TrajectoryGenerator.h>

#include "snider/padmode.hpp"

#include "drivetrain.hpp"
#include "mechanicalarm.hpp"
#include "parameters.hpp"
#include "ports.hpp"
#include "shooter.hpp"

using BotMode = snider::BotMode;

//==============================================================================
class RobotMain : public frc::TimedRobot {
public:
    void RobotInit() override {
        // autonomous options (not used yet)
        chooser.SetDefaultOption (autoNameDefault, autoNameDefault);
        chooser.AddOption (autoNameCustom, autoNameCustom);
        frc::SmartDashboard::PutData ("Auto Modes", &chooser);

        // A safe assumption I guess....
        params.setBotMode (BotMode::Disconnected);

        trajectory = frc::TrajectoryGenerator::GenerateTrajectory (
            frc::Pose2d { 2_m, 2_m, 0_rad },
            {},
            frc::Pose2d { 6_m, 4_m, 0_rad },
            frc::TrajectoryConfig (2_mps, 2_mps_sq));
    }

    /**
     * This function is called every 20 ms, no matter the mode. Use
     * this for items like diagnostics that you want ran during disabled,
     * autonomous, teleoperated and test.
     *
     * This RUNS AFTER the mode specific periodic functions, but before
     * LiveWindow and SmartDashboard integrated updating.
     */
    void RobotPeriodic() override {
        drivetrain.postProcess();
    }

    void AutonomousInit() override {
        autoSelected = chooser.GetSelected();
        autoSelected = frc::SmartDashboard::GetString ("Auto Selector", autoNameDefault);
        fmt::print ("Auto selected: {}\n", autoSelected);

        if (autoSelected == autoNameCustom) {
            // Custom Auto goes here
        } else {
            // Default Auto goes here
        }

        timer.Restart();

        drivetrain.resetOdometry (trajectory.InitialPose());

        params.setBotMode (BotMode::Autonomous);
    }

    void AutonomousPeriodic() override {
        if (autoSelected == autoNameCustom) {
            // Custom Auto goes here
        } else {
            // Default Auto goes here
        }

        auto elapsed   = timer.Get();
        auto reference = trajectory.Sample (elapsed);
        auto speeds    = ramsete.Calculate (drivetrain.position2d(), reference);
        if (elapsed < trajectory.TotalTime()) {
            drivetrain.drive (units::meters_per_second_t (1.0),
                              units::radians_per_second_t { 0.0 });
        } else {
            driveDisabled();
        }
    }

    //==========================================================================
    void TeleopInit() override {
        params.setBotMode (BotMode::Teleop);
    }

    void TeleopPeriodic() {
        if (! checkControllerConnection()) {
            driveDisabled();
            return;
        }

        processParameters();

        drivetrain.drive (calculateSpeed (params.getSpeed()),
                          calculateAngularSpeed (params.getAngularSpeed()));

        if (params.getButtonValue (Parameters::ButtonA))
            mechanicalArm.moveDown();
        else if (params.getButtonValue (Parameters::ButtonY))
            mechanicalArm.moveUp();

        if (gamepad.GetLeftBumperPressed()) {
            std::clog << "shooter.load()\n";
            shooter.load();
        } else if (gamepad.GetRightBumperPressed()) {
            std::clog << "shooter.shoot()\n";
            shooter.shoot();
        }

        shooter.process();
    }

    //==========================================================================
    void DisabledInit() override {
        params.setBotMode (BotMode::Disabled);
    }

    void DisabledPeriodic() override {
        driveDisabled();
    }

    //==========================================================================
    void TestInit() override {
        params.setBotMode (BotMode::Test);
    }

    void TestPeriodic() override {
        driveDisabled();
    }

    //==========================================================================
    void SimulationInit() override {}

    void SimulationPeriodic() override {
        drivetrain.updateSimulation();
    }

private:
    Parameters params;

    std::string autoSelected;
    frc::SendableChooser<std::string> chooser;
    const std::string autoNameDefault = "Default";
    const std::string autoNameCustom  = "My Auto";

    frc::XboxController gamepad { Port::DefaultGamepad };

    // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0  to 1.
    // This is also called parameter smoothing.
    frc::SlewRateLimiter<units::scalar> speedLimiter { 3 / 1_s };
    frc::SlewRateLimiter<units::scalar> rotLimiter { 3 / 1_s };

    Drivetrain drivetrain;
    MechanicalArm mechanicalArm;
    Shooter shooter;

    frc::Trajectory trajectory;
    frc::RamseteController ramsete;
    frc::Timer timer;

    // keep track of controller connection state.
    bool gamepadConnected = false;

    const units::meters_per_second_t calculateSpeed (double value) noexcept {
        // Get the x speed. We are inverting this because Xbox controllers return
        // negative values when we push forward.
        return -speedLimiter.Calculate (value) * Drivetrain::MaxSpeed;
    }

    const units::radians_per_second_t calculateAngularSpeed (double value) noexcept {
        // Get the rate of angular rotation. We are inverting this because we want a
        // positive value when we pull to the left (remember, CCW is positive in
        // mathematics). Xbox controllers return positive values when you pull to
        // the right by default.
        return -rotLimiter.Calculate (value) * Drivetrain::MaxAngularSpeed;
    }

    void driveDisabled() {
        drivetrain.drive (units::velocity::meters_per_second_t (0),
                          units::angular_velocity::radians_per_second_t (0));
    }

    // updates controller connection status, returns true if currently connected
    bool checkControllerConnection() {
        const bool wasConnected = gamepadConnected;
        gamepadConnected        = gamepad.IsConnected();
        if (gamepadConnected != wasConnected) {
            // noop. connection status changed.
        }
        return gamepadConnected;
    }

    // Filter parameter values before driving the bot.
    void processParameters() {
        Parameters::Context ctx;

        if (! gamepadConnected)
            return;

        // Copy axis values.
        for (int i = std::min (gamepad.GetAxisCount(), (int) Parameters::MaxAxes); --i >= 0;) {
            ctx.axis[i] = gamepad.GetRawAxis (i);
            // needed? gamepad.GetAxisType()
        }

        // Copy dpad info.
        ctx.povs[0] = gamepad.GetPOVCount() <= 0 ? 0 : gamepad.GetPOV (0);

        // copy button values. button indexing in FRC starts at 1 for some reason.
        for (int i = std::min (gamepad.GetButtonCount(), (int) Parameters::MaxButtons); --i >= 0;) {
            ctx.buttons[i] = gamepad.GetRawButton (i + 1);
        }

        params.process (ctx);
    }
};

#ifndef RUNNING_FRC_TESTS
int main() {
    return frc::StartRobot<RobotMain>();
}
#endif