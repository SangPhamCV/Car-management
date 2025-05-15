#include "DriveMode.h"
#include "VehicleConfig.h"

DriveMode::DriveMode() {
    this->currentMode = Mode::ECO;
    const int MAX_POWER = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_ENGINE_POWER);
    DriveModeFactor driveModeFactor;
    powerOutputECO = driveModeFactor.ECO * MAX_POWER;
    powerOutputSport = MAX_POWER;
    std::cout << "DriveMode initialized" << std::endl;
}

DriveMode::~DriveMode() {
    std::cout << "DriveMode destroyed" << std::endl;
}

void DriveMode::setMode(Mode mode) {
    if (mode == Mode::ECO) {
        this->currentMode = Mode::ECO;
    } else if (mode == Mode::SPORT) {
        this->currentMode = Mode::SPORT;
    }
}

int DriveMode::getPowerOutput() const {
    if (currentMode == Mode::ECO) {
        return powerOutputECO;
    } else if (currentMode == Mode::SPORT) {
        return powerOutputSport;
    }
}

int DriveMode::limitSpeedECO(int currentSpeed) const {
    currentSpeed *= 0.9; 
    return currentSpeed;
}