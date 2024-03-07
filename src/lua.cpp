
#include <memory>

#include "lua.hpp"
#include "sol/state.hpp"

namespace lua {

sol::state* _state { nullptr };

void init() {
    if (_state != nullptr)
        return;
    _state = new sol::state();
}

void destroy() {
    if (_state == nullptr)
        return;
    delete _state;
    _state = nullptr;
}

sol::state& state() { 
    if (_state == nullptr)
        throw std::runtime_error("Lua state was not initialized");
    return *_state;
}

}
