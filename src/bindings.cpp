
#include "lua.hpp"
#include "sol/sol.hpp"

#include "mechanicalarm.hpp"
#include "parameters.hpp"
#include "shooter.hpp"

namespace lua {

using function_list = std::initializer_list<const char*>;

namespace detail {

/** Returns the 'cxx' module in the root Lua state. */
sol::table cxx_table (sol::state& ls) {
    auto& G = ls;

    if (G["cxx"].get_type() != sol::type::table)
        G["cxx"] = ls.create_table();
    return (sol::table) G["cxx"];
}

template <typename Ls>
void clear_function_bindings (Ls& L, std::string_view mod,
                              function_list functions) {
    auto cxx   = cxx_table (L);
    auto proxy = cxx[mod];
    if (! proxy.valid())
        return;
    auto M = (sol::table) proxy;
    for (const auto f : functions)
        M.set (f, []() {});
}

} // namespace detail
} // namespace lua

using namespace lua;

bool Parameters::bind (Parameters* self) {
    auto& L  = state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.params' global module.
    if (self != nullptr) {
        auto M = state().create_table();
        M.set ("speed", [self]() -> lua_Number { return self->getSpeed(); });
        M.set ("rotation", [self]() -> lua_Number { return self->getAngularSpeed(); });
        cxx["params"] = M;

    } else {
        detail::clear_function_bindings (L, "params", { "speed", "rotation" });
    }

    return true;
}

void Shooter::bind (Shooter* self) {
    auto& L  = lua::state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.shooter' global module.
    if (self != nullptr) {
        auto M = L.create_table();

        M["shooting"] = [self]() { return self->isShooting(); };
        M["loading"]  = [self]() { return self->isLoading(); };
        M["ready"]    = [self]() { return self->isIdle(); };
        M["shoot"]    = [self]() { return self->shoot(); };
        M["load"]     = [self]() { self->load(); };

        cxx["shooter"] = M;

    } else {
        detail::clear_function_bindings (L, "shooter", { "shooting", "loading", "ready", "shoot", "load" });
    }
}

void MechanicalArm::bind (MechanicalArm* self) {
    auto& L  = lua::state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.lifter' global module.
    if (self == nullptr) {
        auto M = L.create_table();

        M["move_up"]   = [self]() { self->moveUp(); };
        M["move_down"] = [self]() { self->moveDown(); };

        cxx["lifter"] = M;
    } else {
        detail::clear_function_bindings (L, "lifter", { "move_up", "move_down" });
    }
}
