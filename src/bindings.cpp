
#include <frc/XboxController.h>

#include "lua.hpp"
#include "sol/sol.hpp"

#include "drivetrain.hpp"
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

void bind_xbox_controller (frc::XboxController* self) {
    auto& L  = state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.gamepad' global module.
    if (self != nullptr) {
        auto M = state().create_table();

        M["STICK_LEFT_X"]  = (int) Parameters::LeftStickX;
        M["STICK_LEFT_Y"]  = (int) Parameters::LeftStickY;
        M["STICK_RIGHT_X"] = (int) Parameters::RightStickX;
        M["STICK_RIGHT_Y"] = (int) Parameters::RightStickY;

        M["TRIGGER_LEFT"]  = (int) Parameters::TriggerLeft;
        M["TRIGGER_RIGHT"] = (int) Parameters::TriggerRight;

        // frc buttons are 1-indexed.  So is lua....
        M["BUTTON_A"] = (int) Parameters::ButtonA + 1;
        M["BUTTON_B"] = (int) Parameters::ButtonB + 1;
        M["BUTTON_X"] = (int) Parameters::ButtonX + 1;
        M["BUTTON_Y"] = (int) Parameters::ButtonY + 1;

        M["BUMPER_LEFT"]  = (int) Parameters::ButtonLeftBumper + 1;
        M["BUMPER_RIGHT"] = (int) Parameters::ButtonRightBumper + 1;

        M["raw_axis"] = [self] (int index) {
            return self->GetRawAxis (index);
        };

        M["raw_button"] = [self] (int button) {
            return self->GetRawButton (button);
        };

        M["raw_button_pressed"] = [self] (int button) {
            return self->GetRawButtonPressed (button);
        };

        M["raw_button_released"] = [self] (int button) {
            return self->GetRawButtonReleased (button);
        };

        cxx["gamepad"] = M;
    } else {
        // clang-format off
        detail::clear_function_bindings (L, "gamepad", { 
            "raw_axis", "raw_button", "raw_button_pressed", 
            "raw_button_released" 
        });
        // clang-format on
    }
}

} // namespace lua

using namespace lua;

bool Parameters::bind (Parameters* self) {
    auto& L  = state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.params' global module.
    if (self != nullptr) {
        auto M        = state().create_table();
        
        M["speed"]    = [self]() -> lua_Number { return self->getSpeed(); };
        M["rotation"] = [self]() -> lua_Number { return self->getAngularSpeed(); };
        M["brake"]    = [self]() -> lua_Number { return self->getBrake(); };
        
        cxx["params"] = M;
    } else {
        // clang-format off
        detail::clear_function_bindings (L, "params", { 
            "speed", "rotation", "brake" 
        });
        // clang-format on
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
        M["intake"]   = [self]() { self->load(); };
        M["shoot"]    = [self]() { self->shoot(); };       
        M["stop"]     = [self]() { self->stop(); };

        cxx["shooter"] = M;

    } else {
        // clang-format off
        detail::clear_function_bindings (L, "shooter", { 
            "shooting", "loading", "ready", "shoot", "load" 
        });
        // clang-format on
    }
}

void MechanicalArm::bind (MechanicalArm* self) {
    auto& L  = lua::state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.lifter' global module.
    if (self != nullptr) {
        auto M = L.create_table();

        M["move_up"]   = [self]() { self->moveUp(); };
        M["move_down"] = [self]() { self->moveDown(); };

        cxx["lifter"] = M;
    } else {
        detail::clear_function_bindings (L, "lifter", { "move_up", "move_down" });
    }
}

void Drivetrain::bind (Drivetrain* self) {
    auto& L  = lua::state();
    auto cxx = detail::cxx_table (L);

    // bind/unbind 'cxx.drivetrain' global module.
    if (self != nullptr) {
        auto M = L.create_table();

        M["drive"] = [self] (double speed, double rot) {
            self->driveNormalized (speed, rot);
        };

        cxx["drivetrain"] = M;
    } else {
        detail::clear_function_bindings (L, "drivetrain", { "drive" });
    }
}