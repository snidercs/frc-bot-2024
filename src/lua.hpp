#pragma once

#include "../vendordeps/lua-5.4.6/src/lauxlib.h"
#include "../vendordeps/lua-5.4.6/src/lua.h"
#include "../vendordeps/lua-5.4.6/src/lualib.h"

#include <string>
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
    Lifecycle (const Lifecycle&)            = delete;
    Lifecycle& operator= (const Lifecycle&) = delete;
    // no move
    Lifecycle (Lifecycle&&)            = delete;
    Lifecycle& operator= (Lifecycle&&) = delete;
};

/** Returns the global Lua context. */
sol::state& state();

/** Adds lua path qualifiers. e.g. ?.lua and /?/init.lua  to the input string.  
    Input does not get modified.

    @param input The input string to append qualifiers to
    @returns The input path + lua qualifiers.
*/
std::string with_search_qualifiers (std::string_view input);

/** Set the Lua search path.
    Does nothing if passed an empty string.
    @param path A non empty Lua path string.
*/
void set_path (std::string_view path);

/** Bootstrap the interpreter (call once before robot init)
    @returns true if Lua could be bootstrapped.
*/
bool bootstrap();

/** Lua to C++ config bindings.
    
    Functions in this namespace call in to Lua to retrieve values
    from the module.
 */
namespace config {

/** Returns the total number of port indexes. */
int num_ports();

/** Returns a port index by symbol. */
int port (std::string_view symbol);

/** Returns the team name. */
std::string team_name();

/** Returns the team number. */
int team_number();

} // namespace config

} // namespace lua
