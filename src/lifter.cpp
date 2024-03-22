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
    std::clog << "[lifter] pos: " << encL->GetPosition()
        << " - " << encR->GetPosition() << std::endl;
#endif
}

void Lifter::moveDown() {
    maybeInstantiateEncoders();
    for (auto m : motors)
        m->SetVoltage (units::volt_t { 0.3 * -12.0 });
#if USE_LIFTER_ENCODERS
    std::clog << "[lifter] pos: " << encL->GetPosition()
        << " - " << encR->GetPosition() << std::endl;
#endif
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
    std::array<EncoderType*, 2> encs { encL.get(), encR.get() };
    [[maybe_unused]] constexpr auto wheelRadius = 0.1;

    for (auto e : encs) {
        e->SetPositionConversionFactor (2 * std::numbers::pi * wheelRadius / 4096);
        e->SetAverageDepth (8);
        e->SetMeasurementPeriod (15);
        e->SetPosition(0);
        // e->SetInverted (false);
    }

    hasInit = true;
#endif
}

void Lifter::maybeInstantiateEncoders() {
#if USE_LIFTER_ENCODERS
    if (encL == nullptr || encR == nullptr) {
        encL = std::make_unique<EncoderType> (
            leftArm.GetEncoder (EncoderType::Type::kHallSensor, 42));
        encR = std::make_unique<EncoderType> (
            rightArm.GetEncoder (EncoderType::Type::kHallSensor, 42));
        resetEncoders();
    }
#endif
}
