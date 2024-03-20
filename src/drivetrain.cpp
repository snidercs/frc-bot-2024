
#include "robot.hpp"
#include <iostream>

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
    leftEncoder.SetDistancePerPulse (2 * std::numbers::pi * wheelRadius / encoderResolution);
    rightEncoder.SetDistancePerPulse (leftEncoder.GetDistancePerPulse());
    leftEncoder.Reset();
    rightEncoder.Reset();
#endif
}

Drivetrain::~Drivetrain() {
    simulation.reset();
}

void Drivetrain::drive (MetersPerSecond xSpeed, RadiansPerSecond rot) {
    setSpeeds (kinematics.ToWheelSpeeds ({ xSpeed, 0_mps, rot }));
}

void Drivetrain::driveNormalized (double speed, double rotation) noexcept {
    drive (calculateSpeed (speed), calculateRotation (rotation));
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

const MetersPerSecond Drivetrain::calculateSpeed (double value) noexcept {
    // clamp to valid -1 to 1 range.
    value = speedRange.snapToLegalValue (value);
    // convert to 0.0 - 1.0 range.
    value = (value - speedRange.start) / (speedRange.end - speedRange.start);
    // apply and re-scale using scale factor.
    value = speedRange.convertFrom0to1 (value);
    return -speedLimiter.Calculate (value) * maxSpeed;
}

const RadiansPerSecond Drivetrain::calculateRotation (double value) noexcept {
    value *= rotationThrottle;
    if (frc::RobotBase::IsReal()) {
        // if real bot, invert direction of rotation.
        value *= -1.0;
    }
    return -rotLimiter.Calculate (value) * maxAngularSpeed;
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

void Drivetrain::initializeSimulation() {
    if (simulation != nullptr)
        return;
    simulation = std::make_unique<Simulation> (*this);
}

void Drivetrain::updateSimulation() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated positions and velocities to our
    // simulated encoder and gyro. We negate the right side so that positive
    // voltages make the right side move forward.
    if (simulation)
        simulation->update();
}
