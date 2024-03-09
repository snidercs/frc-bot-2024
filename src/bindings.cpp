
#include "lua.hpp"
#include "parameters.hpp"
#include "sol/sol.hpp"

namespace lua {
namespace detail {

sol::table cxx_table (sol::state& ls) {
    if (ls["cxx"].get_type() != sol::type::table)
        ls["cxx"] = ls.create_table();
    return (sol::table) ls["cxx"];
}

} // namespace detail

} // namespace lua

using namespace lua;

bool Parameters::bind (Parameters* self) {
    auto cxx = detail::cxx_table (state());

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
