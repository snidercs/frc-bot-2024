
#include <memory>

#include <frc/Filesystem.h>

#include "lua.hpp"
#include "sol/state.hpp"

namespace lua {

namespace detail {

static sol::state* _state { nullptr };
static bool boostraped { false };

static void init() {
    if (_state != nullptr)
        return;
    _state = new sol::state();
    _state->open_libraries();
}

static void destroy() {
    if (_state == nullptr)
        return;
    delete _state;
    _state = nullptr;
}

} // namespace detail

//==============================================================================
Lifecycle::Lifecycle() {
    if (detail::_state != nullptr) {
        detail::destroy();
        throw std::runtime_error ("lua::Lifecycle already exists.");
    }
    detail::init();
}

Lifecycle::~Lifecycle() { detail::destroy(); }

//==============================================================================
sol::state& state() {
    if (detail::_state == nullptr)
        throw std::runtime_error ("Lua state was not initialized");
    return *detail::_state;
}

bool bootstrap() {
    if (detail::boostraped)
        return true;

    auto make_path = []() -> std::string {
        std::string pathstr (frc::filesystem::GetDeployDirectory());
        pathstr += "/?.lua;/?/init.lua";
        return pathstr;
    };

    auto& ls = lua::state();
    sol::table package { ls["package"] };
    package.set ("path", make_path());

    sol::safe_function_result result;
    
    try {
        result = ls.safe_script (R"(
            config = require ('config')
            print('')
            config.print()
            print('')
        )");
    } catch (const std::exception& e) {
        std::cerr << "[bot] " << e.what() << std::endl;
        return false;
    }

    if (result.valid()) {
        std::cout << "[bot] lua intiialized ok" << std::endl;
        std::cout << "[bot] team: " << lua::config::team_name() 
            << " (" << lua::config::team_number() << ")"
            << std::endl;
    } else {
        auto err = sol::error { result };
        std::cerr << "[bot] error: " << err.what() << std::endl;
        return false;
    }

    detail::boostraped = true;
    return detail::boostraped;
}

namespace config {

std::string team_name() {
    sol::table general = state()["config"]["general"];
    return general.get_or ("team_name", std::string());
}

int team_number() {
    sol::table general = state()["config"]["general"];
    return general.get_or ("team_number", 0);
}

} // namespace config

} // namespace lua
