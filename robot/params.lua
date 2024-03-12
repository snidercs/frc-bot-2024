---Access to bot Parameters.
---@name params
local M = {}

---Returns the speed ranged from  -1.0 to 1.0
function M.speed() return cxx.params.speed() end

---Returns rotation ranged from -1.0 to 1.0
function M.rotation() return cxx.params.rotation() end

return M
