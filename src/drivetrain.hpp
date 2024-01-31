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
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/velocity.h>

/** Represents a differential drive style drivetrain. 
 
    This class is where the controller gets sent to the bot or sim.
 */
class Drivetrain final {
public:
    using speed_type = units::meters_per_second_t;
    using rot_type   = units::radians_per_second_t;

    Drivetrain();

    void drive (double xSpeed, double rot);
    void drive (speed_type xSpeed, rot_type rot);
    void updateOdometry();

    /** Returns the current 2d position. */
    const frc::Pose2d pose2d() const { return m_odometry.GetPose(); }

// 3 meters per second
    static constexpr speed_type MaxSpeed = 3.0_mps;
    // 1/2 rotation per second
    static constexpr rot_type MaxAngularSpeed { std::numbers::pi };
private:
    
    static constexpr units::meter_t TrackWidth = 0.381_m * 2;
    static constexpr double WheelRadius        = 0.0508; // meters
    static constexpr int EncoderResolution     = 4096;

    frc::PWMSparkMax leftLeader { 1 };
    frc::PWMSparkMax leftFollower { 2 };
    frc::PWMSparkMax rightLeader { 3 };
    frc::PWMSparkMax rightFollower { 4 };

    frc::Encoder leftEncoder { 0, 1 };
    frc::Encoder rightEncoder { 2, 3 };

    frc::PIDController leftPIDController { 1.0, 0.0, 0.0 };
    frc::PIDController rightPIDController { 1.0, 0.0, 0.0 };

    frc::AnalogGyro gyro { 0 };

    frc::DifferentialDriveKinematics m_kinematics { TrackWidth };
    frc::DifferentialDriveOdometry m_odometry {
        gyro.GetRotation2d(),
        units::meter_t { leftEncoder.GetDistance() },
        units::meter_t { rightEncoder.GetDistance() }
    };

    // Gains are for example purposes only - must be determined for your own robot!
    frc::SimpleMotorFeedforward<units::meters> feedforward { 1_V, 3_V / 1_mps };

    void setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds);
};
