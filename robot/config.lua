--- Robot configuration.
-- Manages categorized key/value pairs used in robot initialzation. It is loaded
-- on the root Lua state during bootstrap and is always available.
-- 
-- The settings are directly accessible through the module table 
-- e.g. `local val = config.gamepad.skew_factor`
--
-- It does not rely on any other module or frc runtime specific code and 
-- therefore settings can be passed to c++ contructors without concern of static
-- initialization
-- problems.
--
-- @module config
local M = { format_version = 0 }

----- Settings Begin -----

local general = {
    -- Team name
    team_name               = "The Gold Standard",
    -- Team number
    team_number             = 9431,
}

local gamepad = {
    -- controller mode to use.
    controller_mode         = 'standard',
    -- skew factor applied to speed control
    skew_factor             = 0.5 
}

local drivetrain = {

}

local lifter = {

}

local shooter = {
    duration                = 6000
}

local ports = {
    gamepad                 = 0,
    joystick                = 1,

    drive_left_leader       = 7,
    drive_left_follower     = 3,
    drive_right_leader      = 6,
    drive_right_follower    = 5,
    
    arm_left                = 8,
    arm_right               = 11,

    shooter_primary         = 10,
    shooter_secondary       = 9
}

----- End of Settings -----

----- private init and helpers -----

-- cache the total number of ports
local total_ports = 0
for _ in pairs (ports) do total_ports = total_ports + 1 end

local function print_settings (title, cat)
    local space_for_key = 24
    print (title..":")
    for key in pairs (cat) do
        local pad = space_for_key - #key
        print ("  "..key..string.rep(' ', pad).." = "..cat[key])
    end
end

-- returns -1 when not found.
local function lookup (cat, sym)
    sym = tostring (sym)
    if #sym <= 0 or type(cat) ~= 'table' then return -1 end
    return cat[sym] or -1
end

---- public interface -----

M.general = general
M.ports = ports
M.gamepad = gamepad
M.drivetrain = drivetrain
M.lifter = lifter
M.shooter = shooter

--- Port settings access.
-- Do not modify these in other lua scripts.
-- @field ports
M.ports = ports

--- Print all settings to the console.
-- @function print
function M.print()
    print ("Robot Configuration")
    print (string.rep ('-', 40))
    print_settings ("General", general)
    print("")
    print_settings ("Ports", ports)
    print (string.rep ('-', 40))
end

--- Get the team name.
-- Convenience function that returns the team name setting.
-- @function team_name
-- @treturn string The team name
function M.team_name() 
    return general.team_name
end

--- Get the team number.
-- Convenience function that returns the team number setting.
-- @function team_name
-- @treturn int The team name
function M.team_number() 
    return general.team_number 
end

--- Get a general setting by symbol lookup.
-- @function get
-- @tparam string symbol The symbol to lookup
-- @tparam fallback a fallback value (defaults to nil)
-- @treturn mixed The setting value, fallback, or nil.
function M.get (symbol, fallback)
    local res = lookup (general, symbol)
    if res >= 0 then return res end
    return fallback
end

--- Get a port index by symbol lookup.
-- @function port
-- @treturn int The port index
function M.port (symbol)
    return lookup (ports, symbol)
end

--- Get the total number of port configs
-- @function num_ports
-- @treturn int the number of ports
function M.num_ports() return total_ports end

return M
