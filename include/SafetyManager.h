#ifndef SAFETY_MANAGER_H
#define SAFETY_MANAGER_H

#include <iostream>
#include <string>

class SafetyManager {
public:
    SafetyManager();
    ~SafetyManager();

    bool isBrakeAndAcceleratorCoincidence(const bool& isBrake, const bool& isAccelerator);  // Check if brake and accelerator are pressed at the same time
    void activateBrake(); 
    void activateAccelerator(); 
    void releaseBrake(); 
    void releaseAccelerator(); 

    int getBrakeIntensity() const { return brakeIntensity; }
    int getAcceleratorIntensity() const { return acceleratorIntensity; }

private:
    int brakeIntensity;
    int acceleratorIntensity;
};

extern bool isSafetyAction;

#endif