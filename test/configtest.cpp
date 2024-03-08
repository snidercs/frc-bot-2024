
#include <gtest/gtest.h>

#include "lua.hpp"
#include "sol/sol.hpp"

namespace detail {
} // namespace detail

#if 0
class ConfigTest : public testing::Test {
public:
    ConfigTest() { lua::bootstrap(); }

protected:
    lua::Lifecycle engine;
};

TEST_F (ConfigTest, Ports) {
    const std::vector<std::string> symbols {
        "gamepad",
        "joystick",

        "drive_left_leader",
        "drive_left_follower",
        "drive_right_leader",
        "drive_right_follower",

        "arm_left",
        "arm_right",

        "shooter_primary",
        "shooter_secondary",
    };

    for (const auto& sym : symbols)
        EXPECT_GE (lua::config::port (sym), 0);
}

TEST_F (ConfigTest, NoExceptions) {
    auto& ls = lua::state();
    try {
        lua::config::port ("");
        EXPECT_EQ (lua::config::team_name(), "The Gold Standard");
        EXPECT_EQ (lua::config::team_number(), 9431);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        EXPECT_TRUE (false);
    }
}
#endif
