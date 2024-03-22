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
            
            {
                sol::object obj = res;
                if (obj.get_type() != sol::type::function) {
                    self->_error = "Did not get a factory function";
                    return self;
                }
            }
            
            Factory factory = res;
            sol::protected_function_result pr = factory();
            if (! pr.valid() || pr.get_type() != sol::type::table) {
                self->_error = "Did not get an engine descriptor table";
                return self;
            }

            self->M = pr;

#define ASSIGN_F(name)                                    \
    if (self->M[#name].get_type() == sol::type::function) \
        self->f_##name = self->M[#name];
            ASSIGN_F (init)
            ASSIGN_F (prepare)
            ASSIGN_F (run)
            ASSIGN_F (cleanup)
#undef ASSIGN_F

        } catch (const std::exception& e) {
            self->_error = e.what();
        }

        return self;
    }

    const std::string error() const noexcept { return _error; }
    bool have_error() const noexcept { return ! _error.empty(); }

    bool init() {
        if (f_init)
            f_init();
        return (bool) f_init;
    }

#define METHOD(name)    \
    void name() {       \
        if (f_##name)   \
            f_##name(); \
    }

    METHOD (prepare)
    METHOD (run)
    METHOD (cleanup)
#undef METHOD

private:
    Engine (lua_State* state)
        : L (state) {}
    using Factory    = sol::protected_function;
    using LoadResult = sol::load_result;
    using Path       = std::filesystem::path;
    sol::state_view L;
    sol::table M;
    std::string _error;
    sol::function f_init, f_prepare, f_run, f_cleanup;
};

using EnginePtr = std::unique_ptr<Engine>;
