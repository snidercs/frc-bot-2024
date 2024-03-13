---Main robot.
---Facilities for controlling the bot.
---@class robot

local drivetrain = cxx.drivetrain;
local lifter     = cxx.lifter;
local shooter    = cxx.shooter;

---Drive the robot.
---@param speed number Speed ranged -1 to 1
---@param rot number Rotation ranged -1 to 1
local function drive(speed, rot)
    drivetrain.drive(speed, rot)
end

---Move arms up.
local function raise_arms()
    lifter.move_up()
end

---Move the arms down.
local function lower_arms()
    lifter.move_down()
end

---Returns true if the robot is shooting.
---@return boolean
local function shooting()
    return shooter.shooting()
end

---Returns true if the robot is trying to load a note.
---@return boolean
local function intake_running()
    return shooter.loading()
end

---Loads a note if not shooting or already loading.
local function intake()
    return shooter.intake()
end

--- Shoots a note if not loading and not already shooting.
local function shoot()
    return shooter.shoot()
end

---Trys to stop the shooter motors regardless of what they're doing.
local function stop_shooter()
    return shooter.stop()
end

local M = {
    drive          = drive,
    raise_arms     = raise_arms,
    lower_arms     = lower_arms,
    intake_running = intake_running,
    intake         = intake,
    shooting       = shooting,
    shoot          = shoot,
    stop_shooter   = stop_shooter
}

return M
