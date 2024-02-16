#pragma once

/** Ports defined in this file are indexes/IDs used by PWM controller, Gamepads,
    and any other device connected which has a Port ID.
*/
namespace Port {

/** The default gamepad port to use. */
static constexpr int DefaultGamepad = 0;

/** Default joystick port, not currently used. */
static constexpr int DefaultJoystick = 1;

/** Controller for the left drive motors. */
static constexpr int DriveLeftLeader = 0;

/** Controller for the right drive motors. */
static constexpr int DriveRightLeader = 1;

/** Controller for the shooter intake. */
static constexpr int ShooterIntake = 2;

/** Controller for the shooting. */
static constexpr int ShooterShoot = 3;

/** Controller for arm rotation. */
static constexpr int ArmRotate = 4;

/** Controller for arm pickup action. */
static constexpr int ArmPickup = 5;
} // namespace Port
