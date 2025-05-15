#include "DashboardController.h"

#define EXIST 1
#define NOT_EXIST 0

DashboardController::DashboardController() {
    std::cout << "DashboardController initialized" << std::endl;
}

DashboardController::~DashboardController() {}

void DashboardController::readData(const std::unordered_map<std::string, std::string>& newData) {
    for (const auto& [key, value] : newData) {
        if (key == "VEHICLE_SPEED")    speed = std::stoi(value);
        else if (key == "DRIVE_MODE")    driveMode = value;
        else if (key == "BATTERY_LEVEL")    batteryLevel = std::stoi(value);
        else if (key == "ROUTE_PLANNER")    remainingRange = std::stoi(value);
        else if (key == "WIND_LEVEL")    windLevel = std::stoi(value);
        else if (key == "AC_CONTROL")    climateTemp = std::stoi(value);
        else if (key == "TURN_SIGNAL")    turnSignal = std::stoi(value);
        else if (key == "BRAKE")    isBrake = strToBool(value);
        else if (key == "ACCELERATOR")    isAccelerator = strToBool(value);
        else if (key == "AC_STATUS")    acStatus = strToBool(value);
    }

    notifyObservers();
}

void DashboardController::registerObserver(Observer* observer) {
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    } else {
        std::cout << "Observer already registered" << std::endl;
    }
}

void DashboardController::unregisterObserver(Observer* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    std::cout << "Observer unregistered" << std::endl;
}

void DashboardController::notifyObservers() const {
    for (const auto& observer : observers) {
        observer->update(speed, remainingRange, batteryLevel, climateTemp, windLevel, turnSignal, driveMode, isBrake, isAccelerator, acStatus);
    }
}