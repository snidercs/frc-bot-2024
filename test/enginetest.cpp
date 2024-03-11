
#include <filesystem>

#include <gtest/gtest.h>

#include "test.hpp"
#include "lua.hpp"
#include "sol/sol.hpp"

namespace fs = std::filesystem;
namespace config = lua::config;

class EngineTest : public testing::Test {
public:
    EngineTest() {}
};

TEST_F (EngineTest, Period) {
    EXPECT_NE(gTimedRobot, nullptr);
    double actual = units::time::millisecond_t (gTimedRobot->GetPeriod()).value();
    EXPECT_EQ (config::get ("engine", "period").as<double>(), actual);
}

TEST_F (EngineTest, RequireAll) {
    auto& L { lua::state() };
    EXPECT_NO_THROW (
        L.script (R"(
            require ('config')
            require ('params')
            require ('gamepad')
            require ('robot')
        )")
    );
}
