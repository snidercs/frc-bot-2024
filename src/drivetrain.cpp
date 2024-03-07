// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <iostream>

#include "drivetrain.hpp"

Drivetrain::Drivetrain() {
    gyro.Reset();
    for (auto* const motor : motors) {
        motor->SetIdleMode (IdleMode::kBrake);
    }
    leftFollower.Follow (leftLeader);
    rightFollower.Follow (rightLeader);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.SetInverted (true);

#if 1
    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    leftEncoder.SetDistancePerPulse (2 * std::numbers::pi * WheelRadius / EncoderResolution);
    rightEncoder.SetDistancePerPulse (leftEncoder.GetDistancePerPulse());
    leftEncoder.Reset();
    rightEncoder.Reset();
#endif
}

void Drivetrain::drive (units::meters_per_second_t xSpeed,
                        units::radians_per_second_t rot) {
    setSpeeds (kinematics.ToWheelSpeeds ({ xSpeed, 0_mps, rot }));
}

void Drivetrain::setSpeeds (const frc::DifferentialDriveWheelSpeeds& speeds) {
    auto leftFeedforward  = feedforward.Calculate (speeds.left);
    auto rightFeedforward = feedforward.Calculate (speeds.right);

    double leftOutput = leftPIDController.Calculate (
        leftEncoder.GetRate(), speeds.left.value());
    double rightOutput = rightPIDController.Calculate (
        rightEncoder.GetRate(), speeds.right.value());

    leftLeader.SetVoltage (units::volt_t { leftOutput } + leftFeedforward);
    rightLeader.SetVoltage (units::volt_t { rightOutput } + rightFeedforward);
}

void Drivetrain::postProcess() {
    updateOdometry();
    if (simulation)
        simulation->onPostProcess();
}

void Drivetrain::updateOdometry() {
    odometry.Update (gyro.GetRotation2d(),
                     units::meter_t { leftEncoder.GetDistance() },
                     units::meter_t { rightEncoder.GetDistance() });
}

void Drivetrain::resetOdometry (const frc::Pose2d& pose) {
    leftEncoder.Reset();
    rightEncoder.Reset();
    odometry.ResetPosition (gyro.GetRotation2d(),
                            units::meter_t { leftEncoder.GetDistance() },
                            units::meter_t { rightEncoder.GetDistance() },
                            pose);
    if (simulation)
        simulation->onOdometryReset (pose);
}

void Drivetrain::updateSimulation() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated positions and velocities to our
    // simulated encoder and gyro. We negate the right side so that positive
    // voltages make the right side move forward.
    if (simulation)
        simulation->update();
}
