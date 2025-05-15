#include "SpeedCalculator.h"

#define LOAD 200    // suppose max load of car is 200kg

SpeedCalculator::SpeedCalculator(DriveMode* driveMode, SafetyManager* safetyManager) {
    this->driveMode = driveMode;
    this->safetyManager = safetyManager;
    totalDistance = 0.0;
    currentSpeed = 0;
    powerConsumption = 0.0;
    maxSpeedEco = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_SPEED_ECO);
    maxSpeedSport = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_SPEED_SPORT);
    std::cout << "SpeedCalculator initialized" << std::endl;
}

SpeedCalculator::~SpeedCalculator() {}

void SpeedCalculator::adjustSpeed(bool isAcceleratorPressed, bool isBrakePressed) {
    if (isAcceleratorPressed && !isBrakePressed) {
        safetyManager->activateAccelerator();
        safetyManager->releaseBrake();
    } else if (!isAcceleratorPressed && isBrakePressed) {
        safetyManager->activateBrake();
        safetyManager->releaseAccelerator();
    } else if (!isAcceleratorPressed && !isBrakePressed) {
        safetyManager->releaseBrake();
        safetyManager->releaseAccelerator();
   } else {
        safetyManager->releaseBrake();
        safetyManager->releaseAccelerator();
   }
}

void SpeedCalculator::adjustSpeedForDriveMode(const std::string& driveMode) {
    static int lastSpeed = 0;
    static std::string lastDriveMode = "";
    DriveModeFactor driveModeFactor;
    
    // Store the current speed before adjustment
    int preAdjustSpeed = currentSpeed;
    
    // Check if drive mode has changed
    bool driveModeChanged = (lastDriveMode != driveMode && !lastDriveMode.empty());
    
    // If we've switched from SPORT to ECO mode, enforce ECO speed limit immediately
    if (driveModeChanged && driveMode == "ECO" && currentSpeed > maxSpeedEco) {
        currentSpeed = maxSpeedEco;
        lastSpeed = currentSpeed; // Reset lastSpeed to prevent exceeding limit on next frame
    }
    
    // Apply drive mode factor to current speed
    if (driveMode == "ECO") {
        // Apply acceleration factor only to the increment since last frame
        int speedIncrement = preAdjustSpeed - lastSpeed;
        if (speedIncrement > 0) {
            // Apply the ECO factor only to the increment
            currentSpeed = lastSpeed + (speedIncrement * driveModeFactor.ECO);
        }
        
        // Always enforce ECO mode speed limit
        if (currentSpeed > maxSpeedEco) {
            currentSpeed = maxSpeedEco;
        }
    } else if (driveMode == "SPORT") {
        // Apply acceleration factor only to the increment since last frame
        int speedIncrement = preAdjustSpeed - lastSpeed;
        if (speedIncrement > 0) {
            // Apply the SPORT factor only to the increment
            currentSpeed = lastSpeed + (speedIncrement * driveModeFactor.SPORT);
        }
        
        // Cap at max speed
        if (currentSpeed > maxSpeedSport) {
            currentSpeed = maxSpeedSport;
        }
    }
    
    // During acceleration, never allow speed to decrease (unless drive mode changed)
    if (!driveModeChanged && currentSpeed < lastSpeed) {
        currentSpeed = lastSpeed;
    }
    
    // Update last speed and drive mode for next frame
    lastSpeed = currentSpeed;
    lastDriveMode = driveMode;
}

int SpeedCalculator::calculateSpeed(bool isAcceleratorPressed, bool isBrakePressed) {
    static const int MAX_RPM = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_RPM);
    static const int MAX_TORQUE = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_TORQUE);
    static const int WHEEL_RADIUS = ElectricVehicleInit::getDesignValue(VehicleAttribute::WHEEL_RADIUS);
    static const int VEHICLE_WEIGHT = ElectricVehicleInit::getDesignValue(VehicleAttribute::WEIGHT);
    static const int TOTAL_WEIGHT = VEHICLE_WEIGHT + LOAD;
    static std::string previousMode = "";

    static auto previousTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count() / 1000.0; // Convert ms to seconds
    
    // Ensure we have a reasonable deltaTime 
    if (deltaTime <= 0.001) deltaTime = 0.001;
    if (deltaTime > 1.0) deltaTime = 1.0;

    // Update safety manager based on pedal states
    adjustSpeed(isAcceleratorPressed, isBrakePressed);

    // Get pedal intensities from safety manager
    int acceleratorIntensity = safetyManager->getAcceleratorIntensity();
    int brakeIntensity = safetyManager->getBrakeIntensity();
    double speedMetersPerSecond = currentSpeed / 3.6; // Convert km/h to m/s
    
    int rpm = VehicleCalculator::getRpm(speedMetersPerSecond, WHEEL_RADIUS, MAX_RPM);
    double torque = VehicleCalculator::getTorque(rpm, MAX_RPM, acceleratorIntensity, MAX_TORQUE);
    double angularSpeed = VehicleCalculator::getAngularSpeed(rpm);
    
    powerConsumption = VehicleCalculator::getPowerEngine(torque, angularSpeed);

    double traction = VehicleCalculator::getTractiveForce(WHEEL_RADIUS, torque);
    double acceleration = VehicleCalculator::getAcceleration(speedMetersPerSecond, traction, TOTAL_WEIGHT, brakeIntensity);
    
    speedMetersPerSecond += acceleration * deltaTime;
    
    if (speedMetersPerSecond < 0) {
        speedMetersPerSecond = 0;
    }
    
    int newSpeed = static_cast<int>(speedMetersPerSecond * 3.6);
    
    currentSpeed = newSpeed;
    
    // Get current drive mode
    std::string mode = driveMode->getMode() == DriveMode::Mode::ECO ? "ECO" : "SPORT";
    
    // Check if drive mode has changed
    bool modeChanged = (previousMode != mode && !previousMode.empty());
    
    // Apply drive mode limits
    if (isAcceleratorPressed) {
        // Only adjust speed for drive mode when accelerator is pressed
        adjustSpeedForDriveMode(mode);
    } else {
        // When not accelerating, enforce speed limits but don't automatically increase speed
        if (mode == "ECO" && currentSpeed > maxSpeedEco) {
            currentSpeed = maxSpeedEco;
        } else if (mode == "SPORT" && currentSpeed > maxSpeedSport) {
            currentSpeed = maxSpeedSport;
        }
        
        // If we just switched to SPORT mode and are not accelerating,
        // maintain current speed instead of jumping to max SPORT speed
        if (modeChanged && mode == "SPORT" && !isAcceleratorPressed) {
            // Don't increase speed automatically when switching to SPORT mode
            // Speed will only increase when user presses accelerator
        }
    }

    double distanceThisFrame = speedMetersPerSecond * deltaTime + 0.5 * acceleration * deltaTime * deltaTime;
    
    static double distanceInMeters = 0;
    distanceInMeters += distanceThisFrame;
    
    totalDistance = distanceInMeters / 1000.0;

    previousTime = currentTime;
    previousMode = mode;
    return currentSpeed;
}

int SpeedCalculator::getMaxSpeed(const std::string& driveMode) const {
    if (driveMode == "ECO") return maxSpeedEco;
    else return maxSpeedSport;
}