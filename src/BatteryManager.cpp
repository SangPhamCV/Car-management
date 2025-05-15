#include "BatteryManager.h"
#include "VehicleConfig.h"

static const double ENVIRONMENT_TEMP = 35.0;    

BatteryManager::BatteryManager(SpeedCalculator* speedCalculator) {
    this->speedCalculator = speedCalculator;
    batteryMaxCapacity = (double)ElectricVehicleInit::getDesignValue(VehicleAttribute::BATTERY_CAPACITY);
    batteryTemp = ENVIRONMENT_TEMP;
    drainPerKm = 0.1; 
    currentKwH = batteryMaxCapacity;
    std::cout << "BatteryManager initialized" << std::endl;
}

BatteryManager::~BatteryManager() {
    delete speedCalculator;
}

double BatteryManager::calculateBatteryTemp() {
    double powerEngine = speedCalculator->getPowerConsumption();
    batteryTemp = VehicleCalculator::getBatteryTemp(batteryTemp, ENVIRONMENT_TEMP, powerEngine);
    return batteryTemp;
}

double BatteryManager::calculateDrainPerKm(int acTemp, int windLevel) const {
    static const int MAX_AC_POWER = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_AC_POWER);

    double enginePower = speedCalculator->getPowerConsumption();
    int acPower = VehicleCalculator::getPowerAC(ENVIRONMENT_TEMP, acTemp, MAX_AC_POWER);
    int windPower = VehicleCalculator::getPowerWind(windLevel);

    double drainKwH = (enginePower + acPower + windPower) / 1000.0;

    return drainKwH / 3600.0;   // return kWh/s to calculate battery with second
}

double BatteryManager::calculateRemainingRange() {
    // Get current battery capacity in kWh
    double currentBatteryCapacity = getBatteryKwH();
    
    double effectiveDrainRate = (drainPerKm < 0.001) ? 0.1 : drainPerKm;
    
    // Calculate remaining range in km
    double remainingRange = currentBatteryCapacity / effectiveDrainRate;
    
    static const int MAX_RANGE = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_RANGE);
    
    if (remainingRange > MAX_RANGE) {
        remainingRange = MAX_RANGE;
    }
    
    return remainingRange;
}

void BatteryManager::updateBatteryCapacity(int acTemp, int windLevel) {
    static auto previousTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime).count() / 1000.0; // Convert ms to seconds
    
    if (deltaTime <= 0.001) deltaTime = 0.001;
    if (deltaTime > 1.0) deltaTime = 1.0;
    
    double drainKwHPerSecond = calculateDrainPerKm(acTemp, windLevel);
    
    currentKwH -= drainKwHPerSecond * deltaTime;
    
    if (currentKwH < 0) {
        currentKwH = 0;
    }
    
    // Update battery percentage
    batteryCapacity = (currentKwH / batteryMaxCapacity) * 100.0; 
    // Get maximum range from vehicle configuration
    static const int MAX_RANGE = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_RANGE);
    // Get current distance traveled
    double currentRangeTraveled = speedCalculator->getTotalDistance();
    // Only update drain rate if we've traveled some distance
    if (currentRangeTraveled > 0.1) {
        double energyUsed = batteryMaxCapacity - currentKwH;
        drainPerKm = energyUsed / currentRangeTraveled;
        static double previousDrainPerKm = 0.1; // Default starting value
        drainPerKm = 0.9 * previousDrainPerKm + 0.1 * drainPerKm; // Exponential moving average
        previousDrainPerKm = drainPerKm;
    } else {
        // Use a default value when we haven't traveled far enough
        drainPerKm = batteryMaxCapacity / MAX_RANGE;
    }
    
    if (drainPerKm < 0.001) drainPerKm = 0.1; // Minimum drain rate
    
    previousTime = now;
}