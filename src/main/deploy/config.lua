--- Robot configuration.
-- @module config

local M = {}

local general = {
    team_name       = "The Gold Standard",
    team_number     = 9431
}

local ports = {
    gamepad         = 0,
    joystick        = 1
}

local function print_settings (title, conf)
    print (title..":")
    for key in pairs (conf) do
        print ("  "..key, "= "..conf[key])
    end
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
function M.team_name() return general.team_name end

--- Get the team number.
-- Convenience function that returns the team number setting.
-- @function team_name
-- @treturn int The team name
function M.team_number() return general.team_number end

return M
