#pragma once

extern "C" {
#include "../vendordeps/lua-5.4.6/src/lauxlib.h"
#include "../vendordeps/lua-5.4.6/src/lua.h"
#include "../vendordeps/lua-5.4.6/src/lualib.h"
}

#include <string>
#include <string_view>

#include "sol/function.hpp"
#include "sol/state.hpp"

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

/** Set the Lua search path.
    Does nothing if passed an empty string.
    @param path A non empty Lua path string.
*/
void set_path (std::string_view path);

/** Returns the search directory for Lua. */
const std::string& search_directory();

/** Bootstrap the interpreter (call once before robot init)
    @returns true if Lua could be bootstrapped.
*/
bool bootstrap();

/** Lua to C++ config bindings.
    
    Functions in this namespace call in to Lua to retrieve values
    from the module.
 */
namespace config {

#if 0
// TODO: finish this: works for numeric values.
/** Get a value by symbol from the 'general' category in config.lua 
    @param symbol The key to lookup
    @param fallback the fallback value in case of lookup failure
    @tparam T the value type to return
    @tparam S string type
    @returns T the config value
*/
template<typename T, typename S>
T get_or (S&& symbol, T&& fallback) {
    sol::function f = state()["config"]["get"];
    sol::unsafe_function_result res = f(std::forward<S> (symbol), 
                                        std::forward<T> (fallback));
    std::clog << sol::type_name (state(), res.get_type()) << std::endl;
    std::clog << res.get<T>(0) << std::endl;

    return fallback; //res.valid() ? res.get<T>(0) : fallback;
}
#endif

/** Get a value from the 'general' settings. See `robot/config.lua` 
    @param symbol The symbol key to lookup.
    @returns the value or an invalid object.
*/
sol::object get (std::string_view symbol);

/** Get a value from any category
    @param category The category to check
    @param symbol The value key to get.
    @returns The value or an invalid object.
 */
sol::object get (std::string_view category, std::string_view symbol);

/** Return a double by category and symbol. */
double get_double (std::string_view cat, std::string_view sym, double fallback = 0.0);

/** Returns the default gamepad skew factor. */
double gamepad_skew_factor();

/** Returns the total number of port indexes. */
int num_ports();

/** Returns a port index by symbol. */
int port (std::string_view symbol);

/** Returns the team name. */
std::string team_name();

/** Returns the team number. */
int team_number();

/** Match start position. */
std::string match_start_position();

} // namespace config

} // namespace lua
