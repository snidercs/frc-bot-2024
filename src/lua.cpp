
#include <filesystem>
#include <memory>
#include <sstream>

#include <frc/Filesystem.h>

#include "lua.hpp"
#include "sol/state.hpp"

namespace fs = std::filesystem;

namespace lua {

namespace detail {

static sol::state* _state { nullptr };
static bool boostraped { false };
static std::string path;
static std::string search_dir;

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

/** Adds lua path qualifiers. e.g. ?.lua and /?/init.lua  to the input string.  
    Input does not get modified.*/
std::string with_search_qualifiers (std::string_view input) {
    std::stringstream out;
    out << input << detail::separator() << "?.lua;"
        << detail::separator() << "?" << detail::separator() << "init.lua";
    return out.str();
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
    if (path.empty())
        return;
    auto& L            = lua::state();
    detail::search_dir = path;
    detail::search_dir.shrink_to_fit();
    detail::path = detail::with_search_qualifiers (path);
    detail::path.shrink_to_fit();
    sol::table package = L["package"];
    package.set ("path", detail::path);
}

const std::string& search_directory() {
    return detail::search_dir;
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
            return path.string();
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
        sol::error err = result;
        std::cerr << "[bot] " << err.what() << std::endl;
        return false;
    }

    detail::boostraped = true;
    return detail::boostraped;
}

namespace config {

#if 0
// todo, implement this
template<typename T>
T get_or (std::string_view cat, std::string_view sym, T fallback) {
    if (cat.empty() || sym.empty())
        return fallback;
    sol::table tbl = state()["config"][cat];
    return (T) tbl.get_or (sym, fallback);
}
#endif

sol::object get (std::string_view category, std::string_view symbol) {
    if (category.empty() || symbol.empty())
        return sol::object {};

    auto& L { state() };

    sol::object obj = L["config"][category];
    if (obj.is<sol::table>()) {
        auto cat = obj.as<sol::table>();
        return cat[symbol];
    }

    return {};
}

sol::object get (std::string_view symbol) {
    return get ("general", symbol);
}

double get_double (std::string_view cat, std::string_view sym, double fallback) {
    if (cat.empty() || sym.empty())
        return fallback;

    auto obj = state()["config"][cat];
    if (obj.is<sol::table>()) {
        sol::table tbl = obj;
        return tbl.get_or (sym, fallback);
    }

    return fallback;
}

double gamepad_skew_factor() {
    sol::table tbl = state()["config"]["gamepad"];
    return tbl.get_or ("skew_factor", 1.0);
}

std::string team_name() {
    sol::table general = state()["config"]["general"];
    return general.get_or ("team_name", std::string());
}

int team_number() {
    sol::table general = state()["config"]["general"];
    return general.get_or ("team_number", 0);
}

/** Match start position. */
std::string match_start_position() {
    sol::table general = state()["config"]["general"];
    return general.get_or ("match_start_position", std::string { "left" });
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
