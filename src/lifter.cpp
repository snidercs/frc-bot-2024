#include "robot.hpp"

Lifter::Lifter() {
    for (auto m : motors)
        m->SetSmartCurrentLimit (40, 30);
    resetEncoders();
}

/** Bind to lua. See `bindings.cpp` */
static void bind (Lifter*);

void Lifter::moveUp() {
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.3 * 12.0 });
}

void Lifter::moveDown() {
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.3 * -12.0 });
}

void Lifter::stop() {
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.0 });
}

void Lifter::resetEncoders() {
    for (auto* enc : encoders) {
        const auto wheelRadius = 0.1;
        enc->SetDistancePerPulse (2 * std::numbers::pi * wheelRadius / 4096);
        enc->Reset();
    }
}