#include <filesystem>

#include <hal/HAL.h>

#include <frc/TimedRobot.h>

#include "gtest/gtest.h"
#include "lua.hpp"

/** Same as the firmware: lua needs to be in global scope so it stays alive 
    when FRC is using a global static robot base with no shutdown system.
*/
static lua::Lifecycle engine;

extern frc::TimedRobot* instantiate_robot();

int main (int argc, char** argv) {
    // clang-format off
    auto robot_dir = std::filesystem::path (__FILE__)
        .parent_path()
        .parent_path()
        .make_preferred() / "robot";
    // clang-format on

    lua::set_path (lua::with_search_qualifiers (robot_dir.string()));
    if (! lua::bootstrap())
        throw std::runtime_error ("lua could not be bootstrapped for unit testing");
     
    HAL_Initialize (500, 1);
    
    if (auto bot = std::unique_ptr<frc::TimedRobot> ( instantiate_robot() )) {
        ::testing::InitGoogleTest (&argc, argv);
        int ret = RUN_ALL_TESTS();
        bot.reset();
        return ret;
    }

    return -1000;
}
