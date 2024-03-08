// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <memory>
#include <numbers>

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/RobotController.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
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

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/velocity.h>

#include "lua.hpp"
#include "types.hpp"

/**
 * Represents a differential drive style drivetrain.
 */
class Drivetrain {
public:
    Drivetrain();
    ~Drivetrain() {
        simulation.reset();
    }

    /** 3 Meters per second */
    static constexpr MetersPerSecond MaxSpeed = 3.0_mps;

    /** 1/2 rotation per second */
    static constexpr RadiansPerSecond MaxAngularSpeed { std::numbers::pi };

    static constexpr units::meter_t TrackWidth = 0.559_m;
    static constexpr double WheelRadius        = 0.076; // meters
    static constexpr int EncoderResolution     = 4096;

    void drive (units::meters_per_second_t xSpeed, units::radians_per_second_t rot);
    void updateSimulation();
    void postProcess();
    void resetOdometry (const frc::Pose2d& pose);
    frc::Pose2d position2d() const { return odometry.GetPose(); }

    void initializeSimulation() {
        if (simulation != nullptr)
            return;
        simulation = std::make_unique<Simulation> (*this);
    }

private:
    using IdleMode  = rev::CANSparkMax::IdleMode;
    using MotorType = rev::CANSparkLowLevel::MotorType;

    rev::CANSparkMax leftLeader { lua::config::port ("drive_left_leader"), MotorType::kBrushed };
    rev::CANSparkMax leftFollower { lua::config::port ("drive_left_follower"), MotorType::kBrushed };
    rev::CANSparkMax rightLeader { lua::config::port ("drive_right_leader"), MotorType::kBrushed };
    rev::CANSparkMax rightFollower { lua::config::port ("drive_right_follower"), MotorType::kBrushed };
    std::array<rev::CANSparkMax*, 4> motors { &leftLeader, &leftFollower, &rightLeader, &rightFollower };

    //==========================================================================
    frc::Encoder leftEncoder { 0, 1 };
    frc::Encoder rightEncoder { 2, 3 };

    frc::PIDController leftPIDController { 8.5, 0.0, 0.0 };
    frc::PIDController rightPIDController { 8.5, 0.0, 0.0 };

    frc::AnalogGyro gyro { 0 };

    frc::DifferentialDriveKinematics kinematics { TrackWidth };
    frc::DifferentialDriveOdometry odometry {
        gyro.GetRotation2d(),
        units::meter_t { 0.0 },
        units::meter_t { 0.0 }
    };

    // Gains are for example purposes only: must be determined for your own bot!
    frc::SimpleMotorFeedforward<units::meters> feedforward { 1_V, 3_V / 1_mps };

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
            drivetrainSimulator.Update (20_ms);

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
        Drivetrain& owner;
        frc::sim::AnalogGyroSim gyroSim { owner.gyro };
        frc::sim::EncoderSim leftEncoderSim { owner.leftEncoder };
        frc::sim::EncoderSim rightEncoderSim { owner.rightEncoder };
        frc::Field2d fieldSim;
        frc::LinearSystem<2, 2, 2> drivetrainSystem =
            frc::LinearSystemId::IdentifyDrivetrainSystem (
                1.98_V / 1_mps, 0.2_V / 1_mps_sq, 1.5_V / 1_mps, 0.3_V / 1_mps_sq);
        frc::sim::DifferentialDrivetrainSim drivetrainSimulator {
            drivetrainSystem, TrackWidth, frc::DCMotor::CIM (2), 8, 2_in
        };
    };

    std::unique_ptr<Simulation> simulation;

    void setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds);
    void updateOdometry();
};
