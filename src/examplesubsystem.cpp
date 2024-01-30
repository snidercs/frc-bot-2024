// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <iostream>

#include "proto.hpp"
#include "examplesubsystem.hpp"

ExampleSubsystem::ExampleSubsystem() {
  // Implementation of subsystem constructor goes here.
}

frc2::CommandPtr ExampleSubsystem::ExampleMethodCommand() {
  // Inline construction of command goes here.
  // Subsystem::RunOnce implicitly requires `this` subsystem.
  return RunOnce([/* this */] {
    std::clog << "example command called\n";  
  });
}

bool ExampleSubsystem::ExampleCondition() {
  // Query some boolean state, such as a digital sensor.
  return false;
}

void ExampleSubsystem::Periodic() {
  static snider::MessageTicker subsysPeriodic ("ExampleSubsystem::Periodic()");
  subsysPeriodic.tick();
}

void ExampleSubsystem::SimulationPeriodic() {
  // Implementation of subsystem simulation periodic method goes here.
}
