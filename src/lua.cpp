
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
static std::string path;

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

static bool has_custom_path() { return ! path.empty(); }

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

std::string append_search_qualifiers (std::string_view input) {
    std::stringstream out;
    out << input << detail::separator() << "?.lua;"
        << detail::separator() << "?" << detail::separator() << "init.lua";
    return out.str();
}

void set_path (std::string_view path) {
    if (path.empty())
        return;
    auto& ls     = lua::state();
    detail::path = path;
    detail::path.shrink_to_fit();
    sol::table package { ls["package"] };
    package.set ("path", detail::path);
}

bool bootstrap() {
    if (detail::boostraped)
        return true;

    if (! detail::has_custom_path()) {
        set_path ([]() -> std::string {
            fs::path path;

            path = frc::filesystem::GetDeployDirectory();

            if (! fs::exists (path / "config.lua")) {
                path = frc::filesystem::GetOperatingDirectory();
                path /= "robot";
            }

            if (! fs::exists (path / "config.lua")) {
                path = frc::filesystem::GetLaunchDirectory();
                path /= "robot";
            }

            path.make_preferred();

            if (! fs::exists (path / "config.lua"))
                return "";

            std::clog << "[bot] bootstrap: lua path: " << path.string() << std::endl;
            return append_search_qualifiers (path.string());
        }());
    }

    auto& ls = state();
    sol::safe_function_result result;

    try {
        result = ls.script (R"(
            config = require ('config')
        )");
    } catch (const std::exception& e) {
        std::cerr << "[bot] " << e.what() << std::endl;
        return false;
    }

    if (! result.valid()) {
        auto err = sol::error { result };
        std::cerr << "[bot] " << err.what() << std::endl;
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

int num_ports() {
    auto& ls = state();
    return (int) ls["config"]["num_ports"]();
}

int port (std::string_view symbol) {
    auto& ls = state();
    return symbol.empty() ? -1 : (int) ls["config"]["port"](symbol);
}

} // namespace config

} // namespace lua
