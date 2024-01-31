// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "drivetrain.hpp"

Drivetrain::Drivetrain() {
    leftLeader.AddFollower (leftFollower);
    rightLeader.AddFollower (rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.SetInverted (true);

    gyro.Reset();
    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    leftEncoder.SetDistancePerPulse (2 * std::numbers::pi * WheelRadius / EncoderResolution);
    rightEncoder.SetDistancePerPulse (2 * std::numbers::pi * WheelRadius / EncoderResolution);

    leftEncoder.Reset();
    rightEncoder.Reset();
}

void Drivetrain::drive (double xSpeed, double rot) {
    drive (speed_type (xSpeed), rot_type (rot));
}

void Drivetrain::drive (speed_type xSpeed, rot_type rot) {
    setSpeeds (m_kinematics.ToWheelSpeeds ({ xSpeed, 0_mps, rot }));
}

void Drivetrain::updateOdometry() {
    m_odometry.Update (gyro.GetRotation2d(),
                       units::meter_t { leftEncoder.GetDistance() },
                       units::meter_t { rightEncoder.GetDistance() });
}

void Drivetrain::setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds) {
    const auto leftFeedforward  = feedforward.Calculate (speeds.left);
    const auto rightFeedforward = feedforward.Calculate (speeds.right);
    const double leftOutput     = leftPIDController.Calculate (
        leftEncoder.GetRate(), speeds.left.value());
    const double rightOutput = rightPIDController.Calculate (
        rightEncoder.GetRate(), speeds.right.value());

    leftLeader.SetVoltage (units::volt_t { leftOutput } + leftFeedforward);
    rightLeader.SetVoltage (units::volt_t { rightOutput } + rightFeedforward);
}
