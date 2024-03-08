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

local function print_settings (title, cat)
    local space_for_key = 24
    print (title..":")
    for key in pairs (cat) do
        local pad = space_for_key - #key
        print ("  "..key..string.rep(' ', pad).." = "..cat[key])
    end
end

local function lookup (cat, sym)
    if type(cat) ~= 'table' then return nil end
    sym = tostring (sym)
    for key, value in pairs (cat) do
        if key == sym then return value end
    end
    -- not found
    return nil
end

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

--- Get a port, index, or channel of some hardware by symbol lookup.
-- @treturn mixed The setting value or nil if the symbol wasn't found.
function M.get (symbol)
    return lookup (general, symbol)
end

function M.port (symbol)
    return lookup (ports, symbol)
end

return M
