
#include <filesystem>

#include <gtest/gtest.h>

#include "lua.hpp"
#include "sol/sol.hpp"

namespace fs = std::filesystem;
namespace config = lua::config;

class EngineTest : public testing::Test {
public:
    EngineTest() {}
};

TEST_F (EngineTest, RequireAll) {
    auto& L { lua::state() };
    L.script (R"(
        require ('params')
        require ('robot')
    )");
}
