---Main robot.
---Facilities for controlling the bot.
---@name robot

local M          = {}

local drivetrain = cxx.drivetrain;
local lifter     = cxx.lifter;
local shooter    = cxx.shooter;

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

---Drive the robot.
---@param speed number Speed ranged -1 to 1
---@param rot number Rotation ranged -1 to 1
local function drive(speed, rot)
    drivetrain.drive(speed, rot)
end

-- Exports
M.drive          = drive
M.intake_running = intake_running
M.intake         = intake
M.shooting       = shooting
M.shoot          = shoot

M.stop_shooter = stop_shooter

return M
