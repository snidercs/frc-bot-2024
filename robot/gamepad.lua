--- The Gamepad.
-- Possible button/stick controls:
-- - BUTTON_A
-- - BUTTON_B
-- - BUTTON_X
-- - BUTTON_Y
-- - BUMPER_LEFT
-- - BUMPER_RIGHT
local M = {}

local impl = cxx.gamepad

-- Copy constants from c++ bindings
for k, v in pairs(impl) do
    if type(v) == "number" then
        M[k] = v
    end
end

--- Return true if the left bumper was pressed since the last check.
--- @return boolean
function M.left_bumper_pressed()
    return impl.raw_button_pressed(M.BUMPER_LEFT)
end

--- Return true if the left bumper is currently held down.
--- @return boolean
function M.left_bumper()
    return impl.raw_button(M.BUMPER_LEFT)
end

--- Return true if the right bumper was pressed since the last check.
--- @return boolean
function M.right_bumper_pressed()
    return impl.raw_button_pressed(M.BUMPER_RIGHT)
end

--- Return true if the right bumper is currently held down.
--- @return boolean
function M.right_bumper()
    return impl.raw_button(M.BUMPER_RIGHT)
end

return M
