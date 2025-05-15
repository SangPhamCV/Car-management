#include "SafetyManager.h"

#define MAX_PEDAL 100
#define MIN_PEDAL 0

SafetyManager::SafetyManager() {
    brakeIntensity = 0;
    acceleratorIntensity = 0;
    std::cout << "SafetyManager initialized" << std::endl;
}

SafetyManager::~SafetyManager() {}

bool SafetyManager::isBrakeAndAcceleratorCoincidence(const bool& isBrake, const bool& isAccelerator) {
    if (isBrake && isAccelerator) {
        activateBrake();
        return true;
    }
    return false;
}

void SafetyManager::activateBrake() {
    brakeIntensity = std::min(brakeIntensity + 10, MAX_PEDAL);
    acceleratorIntensity = MIN_PEDAL;
}

void SafetyManager::activateAccelerator() {
    acceleratorIntensity = std::min(acceleratorIntensity + 1, MAX_PEDAL);
    brakeIntensity = MIN_PEDAL;
}

void SafetyManager::releaseBrake() {
    brakeIntensity = std::max(MIN_PEDAL, brakeIntensity - 10);
}

void SafetyManager::releaseAccelerator() {
    acceleratorIntensity = std::max(MIN_PEDAL, acceleratorIntensity - 1);
}