// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/AnalogGyroSim.h>
#include <frc/simulation/DifferentialDrivetrainSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/smartdashboard/Field2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/system/plant/LinearSystemId.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/velocity.h>

#define BOT_USE_4_PWM 0

/**
 * Represents a differential drive style drivetrain.
 */
class Drivetrain {
public:
    Drivetrain();
    ~Drivetrain() = default;

    static constexpr units::meters_per_second_t MaxSpeed =
        3.0_mps; // 3 meters per second
    static constexpr units::radians_per_second_t MaxAngularSpeed {
        std::numbers::pi
    }; // 1/2 rotation per second

    void drive (units::meters_per_second_t xSpeed, units::radians_per_second_t rot);
    void updateSimulation();
    void postProcess();
    void resetOdometry (const frc::Pose2d& pose);
    frc::Pose2d position2d() const { return odometry.GetPose(); }

private:
    static constexpr units::meter_t TrackWidth = 0.381_m * 2;
    static constexpr double WheelRadius        = 0.0508; // meters
    static constexpr int EncoderResolution     = 4096;

#if BOT_USE_4_PWM
    frc::PWMSparkMax leftLeader { 1 };
    frc::PWMSparkMax leftFollower { 2 };
    frc::PWMSparkMax rightLeader { 3 };
    frc::PWMSparkMax rightFollower { 4 };
#else
    frc::PWMSparkMax leftLeader { 1 };
    frc::PWMSparkMax rightLeader { 3 };
#endif

    frc::Encoder leftEncoder { 0, 1 };
    frc::Encoder rightEncoder { 2, 3 };

    frc::PIDController leftPIDController { 8.5, 0.0, 0.0 };
    frc::PIDController rightPIDController { 8.5, 0.0, 0.0 };

    frc::AnalogGyro gyro { 0 };

    frc::DifferentialDriveKinematics kinematics { TrackWidth };
    frc::DifferentialDriveOdometry odometry {
        gyro.GetRotation2d(),
        units::meter_t { leftEncoder.GetDistance() },
        units::meter_t { rightEncoder.GetDistance() }
    };

    // Gains are for example purposes only - must be determined for your own
    // robot!
    frc::SimpleMotorFeedforward<units::meters> feedforward { 1_V, 3_V / 1_mps };

    // Simulation classes help us simulate our robot
    frc::sim::AnalogGyroSim gyroSim { gyro };
    frc::sim::EncoderSim leftEncoderSim { leftEncoder };
    frc::sim::EncoderSim rightEncoderSim { rightEncoder };
    frc::Field2d fieldSim;
    frc::LinearSystem<2, 2, 2> drivetrainSystem =
        frc::LinearSystemId::IdentifyDrivetrainSystem (
            1.98_V / 1_mps, 0.2_V / 1_mps_sq, 1.5_V / 1_mps, 0.3_V / 1_mps_sq);
    frc::sim::DifferentialDrivetrainSim drivetrainSimulator {
        drivetrainSystem, TrackWidth, frc::DCMotor::CIM (2), 8, 2_in
    };

    void setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds);
    void updateOdometry();
};
