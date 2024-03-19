#pragma once

#include <filesystem>
#include <string>

#include "scripting.hpp"
#include "sol/sol.hpp"

class Engine final {
public:
    ~Engine() {}

    static std::unique_ptr<Engine> instantiate (lua_State* state, std::string_view bot_file) {
        auto self = std::unique_ptr<Engine> (new Engine (state));

        if (! std::filesystem::exists (bot_file)) {
            self->_error = "file does not exist: ";
            self->_error += bot_file;
            return self;
        }

        sol::state_view L (state);
        Path path (bot_file);
        path.make_preferred();

        try {
            LoadResult res { L.load_file (path.string()) };
            if (! res.valid()) {
                sol::error err = res;
                self->_error   = err.what();
                return self;
            }

            Factory factory = res;
            self->M         = factory();

            self->f_init = self->M["init"];

#define ASSIGN_F(name)                                    \
    if (self->M[#name].get_type() == sol::type::function) \
        self->f_##name = self->M[#name];

            ASSIGN_F (teleop_init)
            ASSIGN_F (teleop)
            ASSIGN_F (teleop_exit)

            ASSIGN_F (autonomous_init)
            ASSIGN_F (autonomous)
            ASSIGN_F (autonomous_exit)

            ASSIGN_F (test_init)
            ASSIGN_F (test)
            ASSIGN_F (test_exit)

#undef ASSIGN_F

        } catch (const std::exception& e) {
            self->_error = e.what();
        }

        return self;
    }

    const std::string_view error() const noexcept { return _error; }
    bool have_error() const noexcept { return ! _error.empty(); }

    bool init() {
        if (f_init)
            f_init();
        return (bool) f_init;
    }

#define MEMBER_F(name)             \
    bool name() {                  \
        auto ok = (bool) f_##name; \
        if (f_##name)              \
            f_##name();            \
        return ok;                 \
    }

    MEMBER_F (teleop_init)
    MEMBER_F (teleop)
    MEMBER_F (teleop_exit)

    MEMBER_F (autonomous_init)
    MEMBER_F (autonomous)
    MEMBER_F (autonomous_exit)

    MEMBER_F (test_init)
    MEMBER_F (test)
    MEMBER_F (test_exit)
#undef MEMBER_F

private:
    Engine (lua_State* state)
        : L (state) {}
    using Factory    = sol::protected_function;
    using LoadResult = sol::load_result;
    using Path       = std::filesystem::path;
    sol::state_view L;
    sol::table M;
    std::string _error;
    sol::function f_init,
        f_test_init, f_test, f_test_exit,
        f_teleop_init, f_teleop, f_teleop_exit,
        f_autonomous_init, f_autonomous,
        f_autonomous_exit;
};

using EnginePtr = std::unique_ptr<Engine>;
