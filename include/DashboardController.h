#ifndef DASHBOARD_CONTROLLER_H
#define DASHBOARD_CONTROLLER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "VehicleConfig.h"

// Observer pattern interface
class Observer {
public:
    virtual void update(
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
    ) = 0;

    virtual ~Observer() {};
};

// Subject class (Observable)
class DashboardController {
public:
    DashboardController();
    ~DashboardController();

    void readData(const std::unordered_map<std::string, std::string>& newData);
    
    // Observer pattern
    void registerObserver(Observer* observer);
    void unregisterObserver(Observer* observer);
    void notifyObservers() const;

    // getters
    std::string getDriveMode() const { return driveMode; }
    bool getIsBrake() const { return isBrake; }
    bool getIsAccelerator() const { return isAccelerator; }
    bool getAcStatus() const { return acStatus; }
    uint16_t getSpeed() const { return speed; }
    uint16_t getRemainingRange() const { return remainingRange; }
    int getBatteryLevel() const { return batteryLevel; }
    int getClimateTemp() const { return climateTemp; }
    int getWindLevel() const { return windLevel; }
    int getTurnSignal() const { return turnSignal; }

private:
    std::string driveMode;
    bool isBrake, isAccelerator, acStatus;
    uint16_t speed, remainingRange;
    int batteryLevel, climateTemp, windLevel, turnSignal;

    std::vector<Observer*> observers;
};

#endif // DASHBOARD_CONTROLLER_H