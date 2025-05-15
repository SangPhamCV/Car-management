#include <iostream>
#include <iomanip>  
#include <sstream>  
#include "Display.h"

#define ENVIRONMENT_TEMP 35
#define WARNING_BATTERY_LEVEL 10

int outputPower;
double updateOdometer;
double updateBatteryTemp;
bool isSafetyAction;
int gasIntensityDisplay;
int brakeIntensityDisplay;

Display::Display(DashboardController* dashboardController) {
    this->dashboardController = dashboardController;
    dashboardController->registerObserver(this);
    std::cout << "Display initialized" << std::endl;
}

Display::~Display() {
    dashboardController->unregisterObserver(this);
    std::cout << "Display destroyed" << std::endl;
}

void Display::update(
    const uint16_t& speed,
    const uint16_t& remainingRange,
    const int& batteryLevel,
    const int& climateTemp,
    const int& windLevel,
    const int& turnSignal,
    const std::string& driveMode,
    const bool& isBrake,
    const bool& isAccelerator,
    const bool& acStatus
    ) {
    if (dashboardController == nullptr) {
        showSpeed(speed);
        showBatteryLevel(batteryLevel);
        showClimateStatus(acStatus, climateTemp, windLevel);
        showDriveMode(driveMode);
        showRemainingRange(remainingRange);
        showTurnSignal(turnSignal);
        showBrakePressed(isBrake);
        showGasPressed(isAccelerator);
    }
    return;
}

void Display::updateDisplay() {
    std::cout << "----------------------------------------" << std::endl;
    showSpeed(dashboardController->getSpeed());
    showBatteryLevel(dashboardController->getBatteryLevel());
    showClimateStatus(dashboardController->getAcStatus(), dashboardController->getClimateTemp(), dashboardController->getWindLevel());
    showDriveMode(dashboardController->getDriveMode());
    showRemainingRange(dashboardController->getRemainingRange());
    showTurnSignal(dashboardController->getTurnSignal());
    showBrakePressed(dashboardController->getIsBrake());
    showGasPressed(dashboardController->getIsAccelerator());
    std::cout << "----------------------------------------" << std::endl;
}

void Display::showSpeed(const uint16_t& speed) {
    if (isSafetyAction) {
        std::cout << " -- Detected press gas and press brake at the same time --> refer to slow down" << std::endl;
    }
    std::cout << " -- Current speed of vehicle: " << speed << " km/h"
              << " - Max Power of vehicle: " << outputPower << " kW" << std::endl;
}

void Display::showBatteryLevel(const int& batteryLevel) {
    std::cout << " -- Current battery of vehicle: " << batteryLevel << "%"
              << " - Current battery temp of vehicle: " << std::setprecision(3) << updateBatteryTemp << "°C - " << "Enviroment Temp: " << ENVIRONMENT_TEMP << "°C" << std::endl;
    if (batteryLevel < WARNING_BATTERY_LEVEL) {
        std::cout << "Warning: Battery level is too low!" << std::endl;
    }
}

void Display::showClimateStatus(const bool& acStatus, const int& climateTemp, const int& windLevel) {
    auto acStatusStr = [acStatus]() -> std::string {
        return acStatus ? "AC is ON" : "AC is OFF";
    }();
    std::cout << " -- " << acStatusStr << " - Climate Temp of vehicle: " << static_cast<int>(climateTemp) << "°C - " << "Wind Level: " << static_cast<int>(windLevel) << std::endl;
}

void Display::showDriveMode(const std::string& driveMode) {
    auto driveModeStr = [driveMode]() -> std::string {
        return driveMode == "ECO" ? "Drive Mode: ECO" : "Drive Mode: SPORT";
    }();
    std::cout << " -- " << driveModeStr << std::endl;
}

void Display::showRemainingRange(const uint16_t& remainingRange) {
    std::ostringstream odometerStream;
    odometerStream << std::fixed << std::setprecision(2) << updateOdometer;
    
    std::cout << " -- Remaining Range of vehicle: " << remainingRange << " km"
              << " - Range Traveled: " << odometerStream.str() << " km" << std::endl;
}

void Display::showTurnSignal(const int& turnSignal) {
    if (turnSignal == 0) std::cout << " -- Turn Signal: OFF" << std::endl;
    else if (turnSignal == 1) std::cout << " -- Turn Signal: LEFT" << std::endl;
    else if (turnSignal == 2) std::cout << " -- Turn Signal: RIGHT" << std::endl;
    else std::cout << " -- Invalid Turn Signal" << std::endl;
}

void Display::showBrakePressed(const bool& isBrake) {
    isBrake ? std::cout << " -- Brake is pressed" << std::endl : std::cout << " -- Brake is released"
                        << " - Brake Intensity: " << brakeIntensityDisplay << " %" << std::endl;
}

void Display::showGasPressed(const bool& isAccelerator) {
    isAccelerator ? std::cout << " -- Gas is pressed" << std::endl : std::cout << " -- Gas is released"
                              << " - Gas Intensity: " << gasIntensityDisplay << " %" << std::endl;
}
    