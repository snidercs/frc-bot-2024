// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/RobotController.h>

#include "drivetrain.hpp"

Drivetrain::Drivetrain() {
    gyro.Reset();

#if BOT_USE_4_PWM
    leftFollower.Follow (leftLeader);
    rightFollower.Follow (rightLeader);
#endif

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.SetInverted (true);

    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    leftEncoder.SetDistancePerPulse (
        2 * std::numbers::pi * WheelRadius / EncoderResolution);
    rightEncoder.SetDistancePerPulse (
        2 * std::numbers::pi * WheelRadius / EncoderResolution);

    leftEncoder.Reset();
    rightEncoder.Reset();

    frc::SmartDashboard::PutData ("Field", &fieldSim);
}

void Drivetrain::drive (units::meters_per_second_t xSpeed,
                        units::radians_per_second_t rot) {
    setSpeeds (kinematics.ToWheelSpeeds ({ xSpeed, 0_mps, rot }));
}

void Drivetrain::setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds) {
    auto leftFeedforward  = feedforward.Calculate (speeds.left);
    auto rightFeedforward = feedforward.Calculate (speeds.right);
    double leftOutput     = 0.0; // leftPIDController.Calculate (leftEncoder.GetRate(),
                                 //                  speeds.left.value());
    double rightOutput    = 0.0; // rightPIDController.Calculate (rightEncoder.GetRate(),
                                 //                      speeds.right.value());
    leftLeader.SetVoltage (units::volt_t { leftOutput } + leftFeedforward);
    rightLeader.SetVoltage (units::volt_t { rightOutput } + rightFeedforward);
}

void Drivetrain::postProcess() {
    updateOdometry();
    fieldSim.SetRobotPose (odometry.GetPose());
}

void Drivetrain::updateOdometry() {
    odometry.Update (gyro.GetRotation2d(),
                     units::meter_t { leftEncoder.GetDistance() },
                     units::meter_t { rightEncoder.GetDistance() });
}

void Drivetrain::resetOdometry (const frc::Pose2d& pose) {
    leftEncoder.Reset();
    rightEncoder.Reset();
    drivetrainSimulator.SetPose (pose);
    odometry.ResetPosition (gyro.GetRotation2d(),
                            units::meter_t { leftEncoder.GetDistance() },
                            units::meter_t { rightEncoder.GetDistance() },
                            pose);
}

void Drivetrain::updateSimulation() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated positions and velocities to our
    // simulated encoder and gyro. We negate the right side so that positive
    // voltages make the right side move forward.
    drivetrainSimulator.SetInputs (units::volt_t { leftLeader.Get() } * frc::RobotController::GetInputVoltage(),
                                   units::volt_t { rightLeader.Get() } * frc::RobotController::GetInputVoltage());
    drivetrainSimulator.Update (20_ms);

    leftEncoderSim.SetDistance (drivetrainSimulator.GetLeftPosition().value());
    leftEncoderSim.SetRate (drivetrainSimulator.GetLeftVelocity().value());
    rightEncoderSim.SetDistance (
        drivetrainSimulator.GetRightPosition().value());
    rightEncoderSim.SetRate (drivetrainSimulator.GetRightVelocity().value());
    gyroSim.SetAngle (-drivetrainSimulator.GetHeading().Degrees().value());
}
