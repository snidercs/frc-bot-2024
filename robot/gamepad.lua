--- The Gamepad.
--  Provides direct access to the gamepad button, trigger, and dpad values.

--[[
    ### Possible button/stick indexes
    - STICK_LEFT_X
    - STICK_LEFT_Y
    - STICK_RIGHT_X
    - STICK_RIGHT_Y
    - TRIGGER_LEFT
    - TRIGGER_RIGHT

    - BUTTON_A
    - BUTTON_B
    - BUTTON_X
    - BUTTON_Y
    - BUMPER_LEFT
    - BUMPER_RIGHT
--]]

local M = {}

local impl = cxx.gamepad

-- Copy constants from c++ bindings
for k, v in pairs(impl) do
    if type(v) == "number" then
        M[k] = v
    end
end

---Return an axis value by index.
---@param index integer Index of the axis control
---@return number
function M.axis(index)
    return impl.raw_axis(index)
end

---Get the left trigger value (0.0 to 1.0)
---@return number
function M.left_trigger()
    return impl.raw_axis(M.TRIGGER_LEFT)
end

---Get the right trigger value (0.0 to 1.0)
---@return number
function M.right_trigger()
    return impl.raw_axis (M.TRIGGER_RIGHT)
end

---Return true if the left bumper was pressed since the last check.
---@return boolean
function M.left_bumper_pressed()
    return impl.raw_button_pressed(M.BUMPER_LEFT)
end

---Return true if the left bumper is currently held down.
---@return boolean
function M.left_bumper()
    return impl.raw_button(M.BUMPER_LEFT)
end

---Return true if the right bumper was pressed since the last check.
---@return boolean
function M.right_bumper_pressed()
    return impl.raw_button_pressed(M.BUMPER_RIGHT)
end

---Return true if the right bumper is currently held down.
---@return boolean
function M.right_bumper()
    return impl.raw_button(M.BUMPER_RIGHT)
end

return M
