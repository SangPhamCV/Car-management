#ifndef DISPLAY_H
#define DISPLAY_H

#include "DashboardController.h"
#include <iomanip>  // For std::setprecision

class Display : public Observer {
public:
    Display(DashboardController* dashboardController);
    ~Display();

    void updateDisplay(); // Update display from DashboardController

    void showSpeed(const uint16_t& speed);
    void showBatteryLevel(const int& batteryLevel);
    void showClimateStatus(const bool& acStatus, const int& climateTemp, const int& windLevel);
    void showDriveMode(const std::string& driveMode);
    void showRemainingRange(const uint16_t& remainingRange);
    void showTurnSignal(const int& turnSignal);
    void showBrakePressed(const bool& isBrake);
    void showGasPressed(const bool& isAccelerator);
    void showWarningAction();

    void update(
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
    ) override;

private:
    DashboardController* dashboardController;
};

extern int outputPower;            
extern double updateOdometer;      
extern double updateBatteryTemp;   
extern bool isSafetyAction;        
extern int gasIntensityDisplay;
extern int brakeIntensityDisplay;

#endif // DISPLAY_H