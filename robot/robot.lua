--- Main robot.
--- Facilities for controlling the bot.
--- @name robot

local M = {}

--- Returns true if the robot is shooting.
--- @return boolean
function M.shooting() return cxx.shooter.shooting() end

--- Returns true if the robot is loading.
--- @return boolean
function M.loading() return cxx.shooter.loading() end

--- Loads a note if not shooting or already loading.
function M.load() return cxx.shooter.load() end

--- Shoots a note if not loading and not already shooting.
function M.shoot() return cxx.shooter.shoot() end

--- Trys to stop the shooter motors regardless of what they're doing.
function M.stop_shooter() return cxx.shooter.stop() end

--- Drive the robot.
--- @param speed number Speed ranged -1 to 1
--- @param rot number Rotation ranged -1 to 1
function M.drive (speed, rot)

end

return M
