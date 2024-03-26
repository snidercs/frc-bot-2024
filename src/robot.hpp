#pragma once

#include <algorithm>
#include <array>
#include <numbers>

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/RobotBase.h>
#include <frc/RobotController.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/filter/SlewRateLimiter.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc/simulation/AnalogGyroSim.h>
#include <frc/simulation/DifferentialDrivetrainSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/smartdashboard/Field2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/system/plant/LinearSystemId.h>

#include <rev/CANSparkMax.h>
#include <rev/CANSparkMaxLowLevel.h>

#include "normalisablerange.hpp"
#include "scripting.hpp"
#include "types.hpp"

/** Represents a differential drive style drivetrain. */
class Drivetrain {
public:
    Drivetrain();
    ~Drivetrain();

    /** Drive the bot by FRC units. */
    void drive (MetersPerSecond xSpeed, RadiansPerSecond rot);

    /** Drive the bot by normalized speed and rotation (-1.0 to 1.0) */
    void driveNormalized (double speed, double rotation) noexcept;

    /** Reset odometry. */
    void resetOdometry (const frc::Pose2d& pose);

    /** Get estimated field position. */
    frc::Pose2d estimatedPosition() const { return odometry.GetPose(); }

private:
    friend class RobotMain;
    static void bind (Drivetrain*);

    const MetersPerSecond maxSpeed {
        lua::config::get ("drivetrain", "max_speed").as<double>()
    };
    const RadiansPerSecond maxAngularSpeed {
        lua::config::get ("drivetrain", "max_angular_speed").as<double>()
    };
    const units::meter_t trackWidth {
        lua::config::get ("drivetrain", "track_width").as<double>()
    };
    const double wheelRadius {
        lua::config::get ("drivetrain", "wheel_radius").as<double>()
    };
    const int encoderResolution {
        static_cast<int> (lua::config::get ("drivetrain", "encoder_resolution").as<double>())
    };
    const double rotationThrottle {
        lua::config::get ("drivetrain", "rotation_throttle").as<double>()
    };

    rev::CANSparkMax leftLeader {
        lua::config::port ("drive_left_leader"),
        MotorType::kBrushed
    };
    rev::CANSparkMax leftFollower {
        lua::config::port ("drive_left_follower"),
        MotorType::kBrushed
    };

    rev::CANSparkMax rightLeader {
        lua::config::port ("drive_right_leader"),
        MotorType::kBrushed
    };
    rev::CANSparkMax rightFollower {
        lua::config::port ("drive_right_follower"),
        MotorType::kBrushed
    };
    std::array<rev::CANSparkMax*, 4> motors { &leftLeader, &leftFollower, &rightLeader, &rightFollower };

    //==========================================================================
    frc::Encoder leftEncoder { 0, 1 };
    frc::Encoder rightEncoder { 2, 3 };

    frc::PIDController leftPIDController { 8.5, 0.0, 0.0 };
    frc::PIDController rightPIDController { 8.5, 0.0, 0.0 };

    frc::AnalogGyro gyro { 0 };

    frc::DifferentialDriveKinematics kinematics { trackWidth };
    frc::DifferentialDriveOdometry odometry {
        gyro.GetRotation2d(),
        units::meter_t { 0.0 },
        units::meter_t { 0.0 }
    };

    // Gains are for example purposes only: must be determined for your own bot!
    frc::SimpleMotorFeedforward<units::meters> feedforward { 1_V, 3_V / 1_mps };

    struct SpeedRange : public juce::NormalisableRange<double> {
        using range_type = juce::NormalisableRange<double>;
        SpeedRange() : range_type (-1.0, 1.0, 0.0, lua::config::gamepad_skew_factor(), true) {}
        ~SpeedRange() = default;
    } speedRange;

    // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0  to 1.
    // This is also called parameter smoothing.
    frc::SlewRateLimiter<units::scalar> speedLimiter { 3 / 1_s };
    frc::SlewRateLimiter<units::scalar> rotLimiter { 3 / 1_s };

    void postProcess();
    const MetersPerSecond calculateSpeed (double value) noexcept;
    const RadiansPerSecond calculateRotation (double value) noexcept;
    void setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds);
    void updateOdometry();

    //==========================================================================
    class Simulation {
    public:
        Simulation() = delete;
        Simulation (Drivetrain& o) : owner (o) {
            frc::SmartDashboard::PutData ("Field", &fieldSim);
        }

        /** Update the overall state of the simulation. */
        void update() {
            // To update our simulation, we set motor voltage inputs, update the
            // simulation, and write the simulated positions and velocities to our
            // simulated encoder and gyro. We negate the right side so that positive
            // voltages make the right side move forward.
            drivetrainSimulator.SetInputs (
                units::volt_t { owner.leftLeader.Get() } * frc::RobotController::GetInputVoltage(),
                units::volt_t { owner.rightLeader.Get() } * frc::RobotController::GetInputVoltage());

            auto engineMillis = units::time::millisecond_t (enginePeriodMs);
            drivetrainSimulator.Update (engineMillis);

            leftEncoderSim.SetDistance (drivetrainSimulator.GetLeftPosition().value());
            leftEncoderSim.SetRate (drivetrainSimulator.GetLeftVelocity().value());
            rightEncoderSim.SetDistance (
                drivetrainSimulator.GetRightPosition().value());
            rightEncoderSim.SetRate (drivetrainSimulator.GetRightVelocity().value());
            gyroSim.SetAngle (-drivetrainSimulator.GetHeading().Degrees().value());
        }

        /** Called from the drive train during "RobotPeriodic" */
        void onPostProcess() {
            fieldSim.SetRobotPose (owner.odometry.GetPose());
        }

        /** Called when the drivetrain resets its Odometry. */
        void onOdometryReset (const frc::Pose2d& pose) {
            drivetrainSimulator.SetPose (pose);
        }

    private:
        const int enginePeriodMs { lua::config::get ("engine", "period").as<int>() };

        Drivetrain& owner;
        frc::sim::AnalogGyroSim gyroSim { owner.gyro };
        frc::sim::EncoderSim leftEncoderSim { owner.leftEncoder };
        frc::sim::EncoderSim rightEncoderSim { owner.rightEncoder };
        frc::Field2d fieldSim;
        frc::LinearSystem<2, 2, 2> drivetrainSystem =
            frc::LinearSystemId::IdentifyDrivetrainSystem (
                1.98_V / 1_mps, 0.2_V / 1_mps_sq, 1.5_V / 1_mps, 0.3_V / 1_mps_sq);
        frc::sim::DifferentialDrivetrainSim drivetrainSimulator {
            drivetrainSystem, owner.trackWidth, frc::DCMotor::CIM (2), 8, 2_in
        };
    };

    std::unique_ptr<Simulation> simulation;

    void initializeSimulation();
    void updateSimulation();
};

//==============================================================================
#define USE_LIFTER_ENCODERS 0

class Lifter {
public:
    Lifter();
    ~Lifter() = default;

    /** Bind to lua. See `bindings.cpp` */
    static void bind (Lifter*);

    void moveUp();
    void moveDown();
    void stop();

private:
#if USE_LIFTER_ENCODERS
    using EncoderType = rev::SparkRelativeEncoder;
    std::unique_ptr<EncoderType> encL, encR;
#endif
    rev::CANSparkMax leftArm { lua::config::port ("arm_left"), MotorType::kBrushless };
    rev::CANSparkMax rightArm { lua::config::port ("arm_right"), MotorType::kBrushless };
    std::array<rev::CANSparkMax*, 2> motors { &leftArm, &rightArm };

    void maybeInstantiateEncoders();
    void resetEncoders();
};

//==============================================================================
/** The shooter interface.

    Controls motors used in loading notes and shooting them.  It operates as a 
    state machine with three states: Idle, Shooting, Loading. Idle can 
    transition to Shooting or Loading then back to Idle.  Shooting and Loading 
    do not transition directly to each other which prevents shoot/load overlaps.
*/
class Shooter {
public:
    Shooter();

    ~Shooter() = default;

    /** State of this shooter. */
    enum State : int {
        Idle,     ///> Idle state. not shooting. not loading.
        Shooting, ///> Is shooting.
        Loading   ///> is Loading.
    };

    /** Reset to default values. */
    void reset();

    /** Run motors for intake if not loading or shooting. */
    void intake();

    /** Prepare motors to shoot if not already loading or shooting. */
    void shoot();

    /** Stop the motors. */
    void stop();

    /** Returns the current state. */
    constexpr State state() const noexcept { return _state; }

    /** Returns true if in Idle state. */
    constexpr bool isIdle() const noexcept { return _state == Idle; }

    /** Returns true if in Loading state. */
    constexpr bool isLoading() const noexcept { return _state == Loading; }

    /** Returns true if in Shooting state. */
    constexpr bool isShooting() const noexcept { return _state == Shooting; }

    /** Set appropriate motor speed and update state if needed. */
    void process() noexcept;

private:
    friend class RobotMain;
    /** Bind to lua. see bindings.cpp */
    static void bind (Shooter*);

    State _state { Idle };
    State lastState { Idle };

    // see `config.lua`
    const int intakeTimeMs;
    const int warmTimeMs;
    const int shootTimeMs;

    int totalTimeMs {};
    int periodMs { 20 };
    int tick       = 0;
    int delay      = 0;
    int delayTicks = 2; // delayTicks x 20ms = totalDelay

    double shootPower { -3.0 },
        intakePrimaryPower { -6.0 },
        intakeSecondaryPower { -3.0 };

    rev::CANSparkMax topMotor {
        lua::config::port ("shooter_primary"),
        MotorType::kBrushed
    };

    rev::CANSparkMax bottomMotor {
        lua::config::port ("shooter_secondary"),
        MotorType::kBrushed
    };

    rev::CANSparkMax supportMotor {
        lua::config::port ("shooter_support"),
        MotorType::kBrushless
    };

    std::array<rev::CANSparkMax*, 2> motors { &topMotor, &bottomMotor };

    std::string stateString() const noexcept;
};
