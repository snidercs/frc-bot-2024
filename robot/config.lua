--- Robot configuration.
-- @module config
local M = { format_version = 0 }

----- Settings Begin -----

local general = {
    team_name               = "The Gold Standard",
    team_number             = 9431,
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
    if type(cat) ~= 'table' then return nil end
    sym = tostring (sym)
    for key, value in pairs (cat) do
        if key == sym then return value end
    end
    -- not found
    return -1
end

---- public interface -----

--- General settings access.
-- Do not modify these in other lua scripts.
-- @field general
M.general = general

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
    return nil
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
