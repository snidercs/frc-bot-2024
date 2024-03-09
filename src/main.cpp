#include <iostream>
#include <memory>
#include <string>

#include <frc/Filesystem.h>
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
#include "lua.hpp"
#include "mechanicalarm.hpp"
#include "normalisablerange.hpp"
#include "parameters.hpp"
#include "shooter.hpp"
#include "sol/sol.hpp"

//==============================================================================
class RobotMain : public frc::TimedRobot {
public:
    RobotMain() {
        Parameters::bind (&params);
    }

    ~RobotMain() {
        Parameters::bind (nullptr);
    }

    void RobotInit() override {
        trajectory = frc::TrajectoryGenerator::GenerateTrajectory (
            frc::Pose2d { 2_m, 2_m, 0_rad },
            {},
            frc::Pose2d { 2.5_m, 2_m, 0_rad },
            frc::TrajectoryConfig (0.75_mps, 2_mps_sq));
    }

    /** Called every 20 ms, no matter the mode. This RUNS AFTER the mode 
        specific periodic functions, but before LiveWindow and SmartDashboard 
        integrated updating.
     */
    void RobotPeriodic() override {
        drivetrain.postProcess();
    }

    void AutonomousInit() override {
        timer.Restart();
        drivetrain.resetOdometry (trajectory.InitialPose());
    }

    void AutonomousPeriodic() override {
        auto elapsed   = timer.Get();
        auto reference = trajectory.Sample (elapsed);
        auto speeds    = ramsete.Calculate (drivetrain.position2d(), reference);
        if (elapsed <= trajectory.TotalTime()) {
            drivetrain.drive (speeds.vx, speeds.omega);
        } else {
            driveDisabled();
        }
    }

    //==========================================================================
    void TeleopInit() override {
        shooter.reset();
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
        else
            mechanicalArm.stop();

        if (gamepad.GetLeftBumperPressed()) {
            shooter.load();
        } else if (gamepad.GetRightBumperPressed()) {
            shooter.shoot();
        }

        shooter.process();
    }

    //==========================================================================
    void DisabledInit() override {
    }

    void DisabledPeriodic() override {
        driveDisabled();
    }

    //==========================================================================
    void TestInit() override {
        TeleopInit();
    }

    void TestPeriodic() override {
        TeleopPeriodic();
    }

    //==========================================================================
    void SimulationInit() override {
        drivetrain.initializeSimulation();
    }

    void SimulationPeriodic() override {
        drivetrain.updateSimulation();
    }

private:
    Parameters params;

    /** Used to apply a logarithmic scale to speed inputs. */
    struct SpeedRange : public juce::NormalisableRange<double> {
        using range_type = juce::NormalisableRange<double>;
        SpeedRange() : range_type (-1.0, 1.0, 0.0, 0.5, true) {}
        ~SpeedRange() = default;
    } speedRange;

    frc::XboxController gamepad { lua::config::port ("gamepad") };

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
        // clamp to valid -1 to 1 range.
        value = speedRange.snapToLegalValue (value);
        // convert to 0.0 - 1.0 range.
        value = (value - speedRange.start) / (speedRange.end - speedRange.start);
        // apply and re-scale using scale factor.
        value = speedRange.convertFrom0to1 (value);
        return -speedLimiter.Calculate (value) * Drivetrain::MaxSpeed;
    }

    const units::radians_per_second_t calculateAngularSpeed (double value) noexcept {
        value *= 0.5; // throttle down sensitivity.
        if (IsReal()) {
            // if real bot, invert direction of rotation.
            value *= -1.0;
        }
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
/** This is not ideal, but frc::StartRobot instantiates a singleton version
    of Robot main with no explicit shutdown.  Our lua engine must exist before
    and after the robot's ctor and dtor. Having lifecylce at the global scope
    helps avoid crashes when the app exits.
*/
static lua::Lifecycle luaEngine;

int main() {
    if (! lua::bootstrap())
        throw std::runtime_error ("lua engine could not be bootstrapped");
    return frc::StartRobot<RobotMain>();
}
#endif
