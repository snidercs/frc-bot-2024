#pragma once

/** Ports defined in this file are indexes/IDs used by PWM controller, Gamepads,
    and any other device connected which has a Port ID.
*/
namespace Port {

// Gamepads and joysticks
/** The default gamepad port to use. */
static constexpr int DefaultGamepad = 0;

/** Default joystick port, not currently used. */
static constexpr int DefaultJoystick = 1;

// Motor Controllers
/** Controller for the left drive motors. */
static constexpr int DriveLeftBack  = 3;
static constexpr int DriveLeftFront = 7;

/** Controller for the right drive motors. */
static constexpr int DriveRightBack  = 5;
static constexpr int DriveRightFront = 6;

/** Controller for arm rotation. */
static constexpr int LeftArm = 8;

/** Controller for arm pickup action. */
static constexpr int RightArm = 11;

/** Controller for the shooter intake. */
static constexpr int BottomShootingWheel = 9;

/** Controller for the shooting. */
static constexpr int TopShootingWheel = 10;

} // namespace Port
