#include <filesystem>
#include <hal/HAL.h>

#include "gtest/gtest.h"
#include "lua.hpp"

int main (int argc, char** argv) {
    // clang-format off
    auto robot_dir = std::filesystem::path (__FILE__)
        .parent_path()
        .parent_path()
        .make_preferred() / "robot";
    // clang-format on

    lua::Lifecycle engine;
    lua::set_path (lua::append_search_qualifiers (robot_dir.string()));
    lua::bootstrap();
    
    HAL_Initialize (500, 1);
    ::testing::InitGoogleTest (&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
