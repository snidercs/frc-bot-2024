// file not in use yet, maybe never will be....

#pragma once

#include <frc2/command/CommandPtr.h>

#include "examplesubsystem.hpp"

namespace autos {
/**
 * Example static factory for an autonomous command.
 */
frc2::CommandPtr ExampleAuto (ExampleSubsystem* subsystem);
} // namespace autos
