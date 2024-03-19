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

#include <cameraserver/CameraServer.h>
#if 0
#    include <opencv2/core/core.hpp>
#    include <opencv2/core/types.hpp>
#    include <opencv2/imgproc/imgproc.hpp>
#endif

#include "snider/padmode.hpp"

#include "engine.hpp"
#include "normalisablerange.hpp"
#include "parameters.hpp"
#include "scripting.hpp"

#include "robot.hpp"
#include "sol/table.hpp"

#define SIM_CAMERA_DISABLED 1

namespace lua {
extern void bind_gamepad (frc::XboxController*);
}

namespace detail {

// make a pose2d from a lua table.
frc::Pose2d makePose2d (sol::table tbl) {
    return frc::Pose2d (
        units::meter_t (tbl[1].get<double>()),
        units::meter_t (tbl[2].get<double>()),
        frc::Rotation2d (units::radian_t (tbl[3].get<double>())));
}

// make a trajectory config from a lua table.
frc::TrajectoryConfig makeTrajectoryConfig (sol::table tbl) {
    return frc::TrajectoryConfig (
        units::meters_per_second_t (tbl[1].get<double>()),
        units::meters_per_second_squared_t (tbl[2].get<double>()));
}

// compose a trajectory from lua configuration.
frc::Trajectory makeTrajectory (std::string_view symbol) {
    sol::function trajectory = lua::state()["config"]["trajectory"];
    sol::table tbl           = trajectory (symbol);
    return frc::TrajectoryGenerator::GenerateTrajectory (
        makePose2d (tbl["start"]),
        {},
        makePose2d (tbl["stop"]),
        makeTrajectoryConfig (tbl["config"]));
}

static void displayBanner() {
    // display engine and bot info.
    std::clog << LUA_COPYRIGHT << std::endl;
    std::clog.flush();
    std::cout.flush();
    std::cerr.flush();
}

static std::string findLuaDir() {
    return lua::search_directory();
}

/** Instantiate the robot. This could return a bot in error state. Be sure to
    check with `Robot::have_error()` and `Robot::error()`.
*/
static EnginePtr instantiateRobot (std::string_view botfile = "engine.bot") {
    auto& L = lua::state();
    std::filesystem::path path (findLuaDir());
    path /= botfile;
    path.make_preferred();
    return Engine::instantiate (L.lua_state(), path.string());
}

} // namespace detail

//==============================================================================
class RobotMain : public frc::TimedRobot {
public:
    RobotMain()
        : frc::TimedRobot (units::millisecond_t (
            lua::config::get ("engine", "period").as<double>())) {
        detail::displayBanner();
        // bind instances to Lua
        Parameters::bind (&params);
        Shooter::bind (&shooter);
        Lifter::bind (&lifter);
        Drivetrain::bind (&drivetrain);
        lua::bind_gamepad (&gamepad);
    }

    ~RobotMain() {
        // release instances from lua
        Parameters::bind (nullptr);
        Shooter::bind (nullptr);
        Lifter::bind (nullptr);
        Drivetrain::bind (nullptr);
        lua::bind_gamepad (nullptr);

        engine.reset();
    }

    void RobotInit() override {
        auto& L = lua::state();
        engine  = detail::instantiateRobot();

        if (engine != nullptr) {
            engine->init();
            if (engine->have_error()) {
                throw std::runtime_error (engine->error().data());
            }
        } else {
            throw std::runtime_error ("Failed to instantiate Lua engine");
        }

        try {
            auto matchPos = lua::config::match_start_position();
            trajectory    = detail::makeTrajectory (matchPos);
        } catch (const std::exception& e) {
            std::cerr
                << "[bot] error: lua trajectory could not be parsed." << std::endl
                << "[bot] what: " << e.what() << std::endl
                << "[bot] loading fallback trajectory" << std::endl;

            trajectory = frc::TrajectoryGenerator::GenerateTrajectory (
                frc::Pose2d { 2_m, 2_m, 0_rad },
                {},
                frc::Pose2d { 2.5_m, 2_m, 0_rad },
                frc::TrajectoryConfig (0.75_mps, 2_mps_sq));
        }

        L.collect_garbage();
#ifndef RUNNING_FRC_TESTS
        std::thread vision (visionThread);
        vision.detach();
#endif
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
        auto speeds    = ramsete.Calculate (drivetrain.estimatedPosition(), reference);
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

        drivetrain.driveNormalized (params.getSpeed(), params.getAngularSpeed());

        if (params.getButtonValue (Parameters::ButtonY))
            lifter.moveUp();
        else if (params.getButtonValue (Parameters::ButtonA))
            lifter.moveDown();
        else
            lifter.stop();

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
        shooter.reset();
        engine->test_init();
    }

    void TestPeriodic() override {
        if (! checkControllerConnection()) {
            driveDisabled();
            return;
        }

        processParameters();
        engine->test();
        shooter.process();
    }

    void TestExit() override {
        engine->test_exit();
    }

    //==========================================================================
    void SimulationInit() override {
        drivetrain.initializeSimulation();
    }

    void SimulationPeriodic() override {
        drivetrain.updateSimulation();
    }

private:
    EnginePtr engine;
    Parameters params;

    frc::XboxController gamepad { lua::config::port ("gamepad") };
    Drivetrain drivetrain;
    Lifter lifter;
    Shooter shooter;

    frc::Trajectory trajectory;
    frc::RamseteController ramsete;
    frc::Timer timer;

    // keep track of controller connection state.
    bool gamepadConnected = false;

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

    static void visionThread() {
#if SIM_CAMERA_DISABLED
        if (RobotBase::IsSimulation())
            return;
#endif
        const auto cameraName = "Camera 1";
        const auto width      = 640;
        const auto height     = 360;
        const auto fps        = 20;

        // Get the USB camera from CameraServer
        cs::UsbCamera camera = frc::CameraServer::StartAutomaticCapture (cameraName, 0);
        // Set the resolution
        camera.SetResolution (width, height);
        camera.SetFPS (fps);

#if 0
        // Get a CvSink. This will capture Mats from the Camera
        cs::CvSink cvSink = frc::CameraServer::GetVideo();
        // Setup a CvSource. This will send images back to the Dashboard
        cs::CvSource outputStream =
            frc::CameraServer::PutVideo (cameraName, width, height);

        // Mats are very memory expensive. Lets reuse this Mat.
        cv::Mat mat;

        while (true) {
            // Tell the CvSink to grab a frame from the camera and
            // put it
            // in the source mat.  If there is an error notify the
            // output.
            if (cvSink.GrabFrame (mat) == 0) {
                // Send the output the error.
                outputStream.NotifyError (cvSink.GetError());
                // skip the rest of the current iteration
                continue;
            }
#    if 0
            // Put a rectangle on the image
            rectangle (mat, cv::Point (100, 100), 
                            cv::Point (400, 400), 
                            cv::Scalar (255, 255, 255), 5);
            // Give the output stream a new image to display
            outputStream.PutFrame (mat);
#    endif
        }
#endif
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
#else
frc::TimedRobot* instantiate_robot() {
    auto bot = new RobotMain();
    bot->RobotInit();
    return bot;
}
#endif
