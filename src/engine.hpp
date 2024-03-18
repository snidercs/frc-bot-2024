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

            if (self->M["teleop"].get_type() == sol::type::function)
                self->f_teleop = self->M["teleop"];
            if (self->M["autonomous"].get_type() == sol::type::function)
                self->f_autonomous = self->M["autonomous"];
            if (self->M["test"].get_type() == sol::type::function)
                self->f_test = self->M["test"];
            if (self->M["test_init"].get_type() == sol::type::function)
                self->f_test_init = self->M["test_init"];
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

    bool teleop() {
        if (f_teleop)
            f_teleop();
        return (bool) f_teleop;
    }

    bool autonomous() {
        if (f_autonomous)
            f_autonomous();
        return (bool) f_autonomous;
    }

    bool test_init() {
        auto ok = (bool) f_test_init;
        if (ok)
            f_test_init();
        return ok;
    }

    bool test() {
        auto ok = (bool) f_test;
        if (ok)
            f_test();
        return ok;
    }

private:
    Engine (lua_State* state)
        : L (state) {}
    using Factory    = sol::protected_function;
    using LoadResult = sol::load_result;
    using Path       = std::filesystem::path;
    sol::state_view L;
    sol::table M;
    std::string _error;
    sol::function f_init, f_test_init, f_test, f_teleop, f_autonomous;
};

using EnginePtr = std::unique_ptr<Engine>;
