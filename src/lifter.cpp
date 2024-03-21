#include "robot.hpp"

Lifter::Lifter() {
    for (auto m : motors) {
        m->SetInverted (false);
        m->SetSmartCurrentLimit (40, 30);
    }
}

void Lifter::moveUp() {
    maybeInstantiateEncoders();
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.3 * 12.0 });
#if USE_LIFTER_ENCODERS
    std::clog << "pos: " << encL->GetPosition() << std::endl;
#endif
}

void Lifter::moveDown() {
    maybeInstantiateEncoders();
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.3 * -12.0 });
}

void Lifter::stop() {
    maybeInstantiateEncoders();
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.0 });
}

void Lifter::resetEncoders() {
#if USE_LIFTER_ENCODERS
    static bool hasInit = false;
    if (hasInit)
        return;
    std::array<rev::SparkAbsoluteEncoder*, 2> encs { encL.get(), encR.get() };
    constexpr auto wheelRadius = 0.1;

    for (auto e : encs) {
        e->SetPositionConversionFactor (2 * std::numbers::pi * wheelRadius / 4096);
    }

    hasInit = true;
#endif
}

void Lifter::maybeInstantiateEncoders() {
#if USE_LIFTER_ENCODERS

    if (encL == nullptr)
        encL = std::make_unique<rev::SparkAbsoluteEncoder> (leftArm.GetAbsoluteEncoder());
    if (encR == nullptr)
        encR = std::make_unique<rev::SparkAbsoluteEncoder> (rightArm.GetAbsoluteEncoder());
    resetEncoders();
#endif
}