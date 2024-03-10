
#include "lua.hpp"
#include "sol/sol.hpp"

#include "mechanicalarm.hpp"
#include "parameters.hpp"
#include "shooter.hpp"

namespace lua {
namespace detail {

/** Returns the 'cxx' module in the root Lua state. */
sol::table cxx_table (sol::state& ls) {
    auto& G = ls;

    if (G["cxx"].get_type() != sol::type::table)
        G["cxx"] = ls.create_table();
    return (sol::table) G["cxx"];
}

} // namespace detail

} // namespace lua

using namespace lua;

bool Parameters::bind (Parameters* self) {
    auto cxx = detail::cxx_table (state());

    // bind/unbind 'params' module.  See also `robot/params.lua`
    if (self != nullptr) {
        auto M = state().create_table();
        M.set ("speed", [self]() -> lua_Number { return self->getSpeed(); });
        M.set ("rotation", [self]() -> lua_Number { return self->getAngularSpeed(); });
        cxx["params"] = M;

    } else {
        auto proxy = cxx["params"];
        if (proxy.valid()) {
            auto M = (sol::table) proxy;
            for (const auto f : { "speed", "rotation" })
                M.set (f, []() -> lua_Number { return 0.0; });
        }
    }

    return true;
}

void Shooter::bind (Shooter* self) {
    auto cxx = detail::cxx_table (lua::state());

    // bind/unbind 'robot' methods module.  See also `robot/params.lua`
    if (self != nullptr) {
        auto M = lua::state().create_table();

        M["shooting"] = [self]() { return self->isShooting(); };
        M["loading"]  = [self]() { return self->isLoading(); };
        M["ready"]    = [self]() { return self->isIdle(); };

        M["shoot"] = [self]() { return self->shoot(); };
        M["load"]  = [self]() { self->load(); };

        cxx["shooter"] = M;

    } else {
        auto proxy = cxx["shooter"];
        if (proxy.valid()) {
            auto M = (sol::table) proxy;
            for (const auto f : { "shooting", "loading", "ready", "shoot", "load" })
                M.set (f, []() {});
        }
    }
}

void MechanicalArm::bind (MechanicalArm*) {
    // noop
}
