--- Robot configuration.
--
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
--- @name config

------------------------------ SETTING BEGIN -----------------------------------
---General settings.
local general = {
    -- Team information
    team_name            = "The Gold Standard",
    team_number          = 9431,

    -- Physical starting position (left, middle, right)
    -- This option is secondary to the Dashboard selection.
    match_start_position = "Left"
}

---The Gamepad
local gamepad = {
    -- controller mode to use (not in use)
    controller_mode = 'standard',

    -- skew factor applied to speed control
    skew_factor     = 0.5
}

---Engine specific settings.
local engine = {
    ---Periodic time out in milliseconds. Greater than 0
    period = math.max (1.0, math.floor ((1.0 / 60.0) * 1000.0)) -- 60fps
    -- period = 4
}

---Driving specific settings
local drivetrain = {
    ---Max speed. 3 Meters per second.
    max_speed = 3.0,

    ---Max angular speed. 1/2 rotation per second
    max_angular_speed = math.pi,

    ---Distance between wheels?
    track_width = 0.559,

    ---Wheel radius in meters.
    wheel_radius = 0.076,

    ---Encoder ticks per revolution.
    encoder_resolution = 4096,

    -- Ratio applied to angular velocity. (0.0 to 1.0)
    rotation_throttle = 0.54
}

-- lifter specific settings
local lifter = {}

---Shooter specific settings
local shooter = {
    ---Time to run intake motor (seconds)
    intake_time = 3.0,

    ---Time to warm up before shooting (seconds)
    warmup_time = 1.0,

    ---Time taken to shoot the note (seconds)
    shoot_time = 2.0,

    ---Power while shooting (volts)
    shoot_power = 14.0,

    ---Power while intaking. Primary motor. (volts)
    intake_primary_power = 5.0,

    ---Power while intaking. Secondary motor. (volts)
    intake_secondary_power = 5.0
}

---Ports, channels, indexes used in motor controllers, gamepads, etc...
local ports = {
    gamepad              = 0,
    joystick             = 1,

    drive_left_leader    = 7,
    drive_left_follower  = 3,
    drive_right_leader   = 6,
    drive_right_follower = 5,

    arm_left             = 8,
    arm_right            = 11,

    shooter_primary      = 9,  -- top green
    shooter_secondary    = 10, -- bottom green
    shooter_support      = 12, -- top roller
    shooter_support_2    = 13  -- bottom roller
}

---Trajectories to use for each match starting position.
local trajectories = {
    ["Left"] = {
        shoot     = false,
        reverse   = true,
        start     = { 2.0, 2.0, 0.0 }, -- x, y, rotation
        waypoints = {},                -- ??? research needed
        stop      = { 2.5, 2.0, 0.0 }, -- x, y, rotation
        config    = { 2.0, 2.0 },      -- max speed, max accel
    },

    ["Left (shoot)"] = {
        shoot     = true,
        reverse   = true,
        start     = { 2.0, 2.0, 0.0 }, -- x, y, rotation
        waypoints = {},                -- ??? research needed
        stop      = { 2.5, 2.0, 0.0 }, -- x, y, rotation
        config    = { 2.0, 2.0 },      -- max speed, max accel
    },

    ["Middle"] = {
        shoot     = false,
        reverse   = true,
        start     = { 2.0, 4.0, 0 }, -- x meters, y meters, rotation degrees
        waypoints = {},
        stop      = { 2.5, 4.0, 0 }, -- x meters, y meters, rotation degrees
        config    = { 1.0, 1.0 },    -- max speed, max accel
    },

    ["Middle (shoot)"] = {
        shoot     = true,
        reverse   = true,
        start     = { 2.0, 4.0, 0 }, -- x meters, y meters, rotation degrees
        waypoints = {},
        stop      = { 2.5, 4.0, 0 }, -- x meters, y meters, rotation degrees
        config    = { 1.0, 1.0 },    -- max speed, max accel
    },

    ["Right"] = {
        shoot     = false,
        reverse   = false,
        start     = { 2.0, 7.0, 0 }, -- x meters, y meters, rotation degrees
        waypoints = {},
        stop      = { 2.5, 7.0, 0 }, -- x meters, y meters, rotation degrees
        config    = { 1.0, 1.0 },    -- max speed, max accel
    },

    ["Right (shoot)"] = {
        shoot     = true,
        reverse   = true,
        start     = { 2.0, 7.0, 0 }, -- x meters, y meters, rotation degrees
        waypoints = {},
        stop      = { 2.5, 7.0, 0 }, -- x meters, y meters, rotation degrees
        config    = { 1.0, 1.0 },    -- max speed, max accel
    }
}
------------------------------- SETTINGS END -----------------------------------

-- cache the total number of ports
local total_ports = 0
for _ in pairs(ports) do
    total_ports = total_ports + 1
end

-- prints all settings in a category with Title lead-in
local function print_settings(title, cat)
    local space_for_key = 24
    print(title .. ":")
    for key in pairs(cat) do
        local pad = space_for_key - #key
        print("  " .. key .. string.rep(' ', pad) .. " = " .. cat[key])
    end
end

-- Returns nil when not found.
local function lookup(cat, sym)
    sym = tostring(sym)
    if #sym <= 0 or type(cat) ~= 'table' then return nil end
    return cat[sym] or nil
end

---- public interface -----

local M = { format_version = 0 }

---General settings
M.general = general

---Port indexes
M.ports = ports

---Gamepad specific settings
M.gamepad = gamepad

---Drivetrain settings
M.drivetrain = drivetrain

---Lifter settings
M.lifter = lifter

--- Shooter settings
M.shooter = shooter

---Trajectories used in auto mode
M.trajectories = trajectories

---Engine settings
M.engine = engine

---Print all settings to the console.
function M.print()
    print("Configuration")
    print(string.rep('-', 40))
    print_settings("General", general)
    print("")
    print_settings("Ports", ports)
    print("")
    print_settings("Engine", engine)
    print(string.rep('-', 40))
end

---Get the team name. Convenience function that returns the team name setting.
---@return string
function M.team_name()
    return general.team_name
end

---Get the team number. Convenience function that returns the team number
---setting.
---@return integer
function M.team_number()
    return general.team_number
end

---Get a general setting by symbol lookup.
---@param symbol string The symbol to lookup
---@param fallback any A fallback value (defaults to nil)
---@return any
function M.get(symbol, fallback)
    local res = lookup(general, symbol)
    if res == nil then return fallback end
    return res
end

---Get a port index by symbol lookup. Returns negative value on failure.
---@return integer
function M.port(symbol)
    return lookup(ports, symbol) or -1
end

---Get the total number of port indexes used in the Robot
---@return integer
function M.num_ports() return total_ports end

---Returns a trajectory table or nil when not found
---@return table
function M.trajectory(symbol)
    return trajectories[symbol] or nil
end

return M
