---A set of useful iterators and utilities.
---@class iterate

local M = {
    ---Iterate an object that supports # indexing with []
    ---@return function
    values = function (t)
        local i = 0
        return function ()
            i = i + 1
            return t[i]
        end
    end
}

setmetatable(M, {
    __call = function (_, t)
        return M.values (t)
    end
})

return M
