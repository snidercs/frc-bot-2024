
#include <filesystem>
#include <memory>

#include <frc/Filesystem.h>

#include "lua.hpp"
#include "sol/state.hpp"

namespace fs = std::filesystem;

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

static char separator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
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

void set_path (std::string_view path) {
    auto& ls = lua::state();
    sol::table package { ls["package"] };
    package.set ("path", path);
}

bool bootstrap() {
    if (detail::boostraped)
        return true;

    set_path ([]() -> std::string {
        std::stringstream out;
        fs::path path;

#if __FRC_ROBORIO__
        path = frc::filesystem::GetDeployDirectory();
#else
        path = frc::filesystem::GetOperatingDirectory();
        path /= "robot";
#endif

        path.make_preferred();
        out << path.string() << detail::separator() << "?.lua;"
            << detail::separator() << "?" << detail::separator() << "init.lua";

        return out.str();
    }());

    auto& ls = state();
    sol::safe_function_result result;

    try {
        result = ls.script (R"(
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

int port (std::string_view symbol) {
    auto& ls = state();
    return (int) ls["config"]["port"](symbol);
}

} // namespace config

} // namespace lua
