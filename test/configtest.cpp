
#include <filesystem>

#include <gtest/gtest.h>

#include "lua.hpp"
#include "sol/sol.hpp"

namespace fs = std::filesystem;
namespace config = lua::config;

class ConfigTest : public testing::Test {
public:
    ConfigTest() {}
};

TEST_F (ConfigTest, GetDouble) {
    EXPECT_EQ (config::gamepad_skew_factor(), 
               config::get_double ("gamepad", "skew_factor"));
    EXPECT_EQ (config::get_double ("gamepad", "fake", 1001.0), 1001.0);
    EXPECT_EQ (config::get_double ("fake", "alsofake", -1002.0), -1002.0);
}

TEST_F (ConfigTest, Gamepad) {
    const auto skew_factor = lua::config::gamepad_skew_factor();
    EXPECT_GE (skew_factor, 0.0);
    EXPECT_LE (skew_factor, 1.0);
    EXPECT_EQ (skew_factor, lua::config::get_double ("gamepad", "skew_factor"));
}

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

    EXPECT_EQ (lua::config::num_ports(), (int) symbols.size());
    for (const auto& sym : symbols)
        EXPECT_GE (lua::config::port (sym), 0);
}

TEST_F (ConfigTest, NoExceptions) {
    try {
        EXPECT_LT (lua::config::port (""), 0);
        EXPECT_LT (lua::config::port ("invalid_____index____not_good"), 0);
        EXPECT_EQ (lua::config::team_name(), "The Gold Standard");
        EXPECT_EQ (lua::config::team_number(), 9431);
    } catch (...) {
        std::cerr << "exception in: " << __FILE__ << std::endl;
        EXPECT_TRUE (false);
    }
}
