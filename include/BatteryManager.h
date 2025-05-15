#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include "SpeedCalculator.h"

/**
 * @brief BatteryManager class
 * 
 * This class manages the battery of the car.
 * It calculates the remaining range, battery temperature, and updates the battery capacity.
 */
class BatteryManager {
public:
    BatteryManager(SpeedCalculator* speedCalculator);
    ~BatteryManager();

    double calculateRemainingRange();
    double calculateBatteryTemp();
    void updateBatteryCapacity(int acTemp, int windLevel);
    
    double getBatteryCapacity() const {return batteryCapacity;}

private:
    SpeedCalculator* speedCalculator;

    double batteryCapacity;
    double batteryMaxCapacity;  // kWh
    double drainPerKm; // consumption capacity per km
    double currentKwH; // current battery capacity in kWh
    double batteryTemp;

    double calculateDrainPerKm(int acTemp, int windLevel) const;
    double getBatteryKwH() const {return currentKwH;}
};

#endif // BATTERY_MANAGER_H