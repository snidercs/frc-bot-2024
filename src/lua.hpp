#pragma once
#include "../vendordeps/lua-5.4.6/src/lauxlib.h"
#include "../vendordeps/lua-5.4.6/src/lua.h"
#include "../vendordeps/lua-5.4.6/src/lualib.h"

namespace sol {
class state;
}

namespace lua {
/** Initialize lua */
void init();
/** Shutdown Lua. DO NOT call until RobotMain has exited. */
void destroy();

/** Init and destroy Lua with RAII pattern. */
struct Lifecycle {
    Lifecycle() { lua::init(); }
    ~Lifecycle() { lua::destroy(); }
};

/** Returns the global Lua context. */
sol::state& state();

}
