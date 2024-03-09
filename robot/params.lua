local M = {}

--- reference the cxx functions to reduce table lookups in private code
local speed = cxx.params.speed
local rotation = cxx.params.rotation

-- "export" cxx fuctions to public api as well.
M.speed = speed
M.rotation = rotation

return M
