#pragma once

#include "../vendordeps/lua-5.4.6/src/lauxlib.h"
#include "../vendordeps/lua-5.4.6/src/lua.h"
#include "../vendordeps/lua-5.4.6/src/lualib.h"

#include <string_view>

namespace sol {
class state;
}

namespace lua {

/** Initialize and destroy Lua with RAII pattern. Instantiating this class more 
    than once will throw a runtime exception. Using lua::state() before the 
    Lifecycle is present will crash badly.
 */
struct Lifecycle final {
    Lifecycle();
    ~Lifecycle();

    // no copy
    Lifecycle(const Lifecycle&) = delete;
    Lifecycle& operator= (const Lifecycle&) = delete;
    // no move
    Lifecycle(Lifecycle&&) = delete;
    Lifecycle& operator= (Lifecycle&&) = delete;
};

/** Returns the global Lua context. */
sol::state& state();

/** Bootstrap the interpreter. */
bool bootstrap();

namespace config {
/** Returns the team name. */
std::string team_name();
/** Returns the team number. */
int team_number();
}

}
