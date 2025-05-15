#ifndef SPEED_CALCULATOR_H
#define SPEED_CALCULATOR_H

#include "VehicleConfig.h"
#include "DriveMode.h"
#include "SafetyManager.h"
#include <chrono>

class SpeedCalculator {
public:
    SpeedCalculator(DriveMode* driveMode, SafetyManager* safetyManager);
    ~SpeedCalculator();

    int calculateSpeed(bool isAcceleratorPressed, bool isBrakePressed);
    
    double getTotalDistance() const {return totalDistance;}
    double getPowerConsumption() const {return powerConsumption;}
    int getCurrentSpeed() const {return currentSpeed;}
    int getMaxSpeed(const std::string& driveMode) const;

private:
    DriveMode* driveMode;
    SafetyManager* safetyManager;
    double totalDistance;
    double powerConsumption;
    int currentSpeed;
    int maxSpeedEco;
    int maxSpeedSport;

    void adjustSpeed(bool isAcceleratorPressed, bool isBrakePressed);
    void adjustSpeedForDriveMode(const std::string& driveMode);
};

#endif // SPEED_CALCULATOR_H