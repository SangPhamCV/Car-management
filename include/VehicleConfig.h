#ifndef VEHICLE_CONFIG_H
#define VEHICLE_CONFIG_H

#include <iostream>
#include <string>
#include <map>

bool strToBool(const std::string& str);

class VehicleCalculator {
private:
    static constexpr int GEAR_RATIO = 9; // Gear ratio
    static constexpr double PI = 3.14159;
    static constexpr double GR  = 9.1; // Final gear ratio
    static constexpr double GRAVITY = 9.81; // Acceleration due to gravity (m/s^2)
    static constexpr double CR = 0.006; // rolling friction coefficient 
    static constexpr double CD = 0.23; // drag coefficient
    static constexpr double US = 0.015; // Static friction coefficient
    static constexpr double UK = 0.7; // braking friction coefficient
    static constexpr double AIR_DENSITY = 1.225; // Air density (kg/m^3)
    static constexpr double EFFICIENCY_DRIVE = 0.95; // drive efficiency
    static constexpr double EFFICIENCY_ENGINE = 0.85; // engine efficiency
    static constexpr double AREA = 2.2; // Frontal area (m^2)

    static constexpr double T_ALPHA = 0.01; // Heat generation by engine coefficient
    static constexpr double T_BETA = 0.15; // Heat cooling affecting coefficient
    static constexpr int C_COOLINGBASE = 1000; // Base cooling capacity
    static constexpr int K_COOLING = 30; // Cooling rate
    static constexpr int DELTA_T = 20; // Maximum temperature difference

    static double getMinStaticFriction(const int& weight) {
        double fStaticFriction = US * weight * GRAVITY;
        return fStaticFriction;
    }

    static double getRollingFriction(const int& weight) {
        double fRollingFriction = CR * weight * GRAVITY;
        return fRollingFriction;
    }

    static double getAirDragForce(const double& speed) {
        double fAirDrag = 0.5 * CD * AIR_DENSITY * AREA * (speed * speed);
        return fAirDrag;
    }

    static double getBrakeForce(const double& speed, const int& weight, const int& brakeLevel) {
        double fMaxBrake = 0.0; 
        // when vehicle is moving, braking force is generated
        if (speed > 0) {
            fMaxBrake = (UK * 1.5) * weight * GRAVITY;
        } else {
            fMaxBrake = 0.0;
        }

        double brakeEffectMultiplier = 1.0 + (brakeLevel / 100.0); 
        return (brakeLevel * fMaxBrake * brakeEffectMultiplier) / 100.0;
    }

public:
    static double getTractiveForce(const int& wheelRadius, const double& currentTorque) {
        // Convert wheel radius from cm to meters
        double wheelRadiusMeters = wheelRadius / 100.0; 
        double simulationMultiplier = 100.0; // Increased multiplier to overcome static friction
        double fTractive = (currentTorque * GEAR_RATIO * EFFICIENCY_DRIVE * simulationMultiplier) / wheelRadiusMeters;
        return fTractive;
    }

    static double getAcceleration(const double& speed, const double& fTractive, const int& weight, const int& brakeLevel) {
        const double MIN_ACCELERATION = 0.2; 
        const double EPSILON = 0.01; 
        static double lastAcceleration = 0.0; 
        
        // Special case for starting from zero speed
        if (speed < EPSILON) { 
            double fStaticFriction = getMinStaticFriction(weight);
            
            if (fTractive < fStaticFriction) {
                lastAcceleration = 0.0;
                return 0.0;
            } else {
                double fRollingFriction = getRollingFriction(weight);
                double fAirDrag = getAirDragForce(0.0);
                double fBrakeForce = getBrakeForce(0.0, weight, brakeLevel);
                double fTotal = fTractive - (fRollingFriction + fAirDrag + fBrakeForce);
                double acceleration = fTotal / weight; 

                if (acceleration < MIN_ACCELERATION && fTractive > 0) {
                    acceleration = MIN_ACCELERATION;
                }
                
                lastAcceleration = acceleration;
                return acceleration;
            }
        } else if (speed < 0.0) {
            lastAcceleration = 0.0;
            return 0.0;
        }
        
        double fRollingFriction = getRollingFriction(weight);
        double fAirDrag = getAirDragForce(speed);
        double fBrakeForce = getBrakeForce(speed, weight, brakeLevel);
        double fTotal = fTractive - (fRollingFriction + fAirDrag + fBrakeForce);
        double acceleration = fTotal / weight; 
        
        if (acceleration < MIN_ACCELERATION && fTractive > 0) {
            acceleration = MIN_ACCELERATION;
        }
        
        if (fTractive > 0 && brakeLevel == 0 && acceleration < 0) {
            acceleration = lastAcceleration * 0.9; 
            if (acceleration < 0.01) acceleration = 0.0;
        }
        
        lastAcceleration = acceleration;
        return acceleration;
    }
    
    static double getTorque(const int& rpm, const int& maxRpm, const int& gasLevel, const int& maxTorque) {
        if (gasLevel <= 0) {
            return 0.0;
        }

        if (rpm == 0) {
            double startingTorque = maxTorque * (double)gasLevel / 100.0;
            return startingTorque;
        }
        
        const int RPM_THRESHOLD = 6000;
        double torque = 0.0;
        if (rpm < RPM_THRESHOLD) {
            torque = maxTorque;
        } else {
            torque = std::max(0.0, maxTorque * (1.0 - (rpm - RPM_THRESHOLD) / (double)(maxRpm - RPM_THRESHOLD)));
        }
        
        double result = torque * (double)gasLevel / 100.0;
        return result;
    }

    static int getRpm(const double& speed, const int& wheelRadius, const int& maxRpm) {
        double wheelRadiusMeters = wheelRadius / 100.0;
        double rpm = (speed * GR * 60.0) / (2.0 * PI * wheelRadiusMeters);

        if (rpm > maxRpm)       rpm = maxRpm;
        else if (rpm < 0.0)     rpm = 0.0;
        return static_cast<int>(rpm);
    }

    static double getAngularSpeed(const int& rpm) {
        return (rpm * 2.0 * PI) / 60.0;
    }

    static int getPowerAC(const int& envTemp, const int& desiredTemp, const int& maxAcPower) {
        int acPower = 0;
        if (desiredTemp == 0) {
            acPower = 0;
        } else {
            int kac = maxAcPower / DELTA_T;
            acPower = kac * std::abs(envTemp - desiredTemp);
        }
        return acPower;
    }

    static int getPowerWind(const int& windLevel) {
        if (windLevel == 0) {
            return 0;
        } else if (windLevel == 1) {
            return 300;
        } else if (windLevel == 2) {
            return 600;
        } else if (windLevel == 3) {
            return 900;
        } else if (windLevel == 4) {
            return 1200;
        } else if (windLevel == 5) {
            return 1500;
        } else {
            return 1500;
        }
    }

    static double getPowerEngine(const double& torque, const double& angularSpeed) {
        return (torque * angularSpeed)/ EFFICIENCY_DRIVE;
    }

    static double getBatteryTemp(const double previousBatteryTemp, const double& envTemp, const double& enginePower) {
        double enginePowerK = enginePower;
        double coolingEfficiency = C_COOLINGBASE + K_COOLING * (previousBatteryTemp - envTemp);
        coolingEfficiency /= 1000.0;
        enginePowerK /= 1000.0;
        double currentBatteryTemp = envTemp + T_ALPHA * enginePowerK - T_BETA * coolingEfficiency;
        return currentBatteryTemp;
    }
};

enum class VehicleOption {
    STANDAND,
    LONG_RANGE,
    PERFORMANCE,
    NOT_SET
};

enum class VehicleBrand {
    VINFAST,
    TESLA,
    HYUNDAI,
    NOT_SET
};

enum class VehicleAttribute {
    ENGINE_TOTAL,
    BATTERY_CAPACITY,
    BATTERY_VOLTAGE,
    MAX_RANGE,
    MAX_TORQUE,
    MAX_ENGINE_POWER,
    MAX_AC_POWER,
    MAX_SPEED_SPORT,
    MAX_SPEED_ECO,
    MAX_RPM,
    WEIGHT,
    WHEEL_RADIUS,
    AC_TEMP_MAX,
    AC_TEMP_MIN,
    WIND_LEVEL_MAX
};

using designValue = int16_t;
using vehicleBaseParam = std::map<VehicleAttribute, designValue>;

class ElectricVehicleInit {
private:
    static VehicleBrand brand;
    static VehicleOption option;
    static vehicleBaseParam baseParam;

public:
    ElectricVehicleInit(VehicleOption option, VehicleBrand brand);
    ~ElectricVehicleInit();
    
    static void displayVehicleInfo() {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << " + capacity: " << getDesignValue(VehicleAttribute::BATTERY_CAPACITY) << "kWh" << std::endl;
        std::cout << " + voltage: " << getDesignValue(VehicleAttribute::BATTERY_VOLTAGE) << "V" << std::endl;
        std::cout << " + torque: " << getDesignValue(VehicleAttribute::MAX_TORQUE) << "Nm" << std::endl;
        std::cout << " + max speed in sport mode: " << getDesignValue(VehicleAttribute::MAX_SPEED_SPORT) << "km/h" << std::endl;
        std::cout << " + max speed in eco mode: " << getDesignValue(VehicleAttribute::MAX_SPEED_ECO) << "km/h" << std::endl;
        std::cout << " + max power: " << getDesignValue(VehicleAttribute::MAX_ENGINE_POWER) << "kW" << std::endl;
        std::cout << " + max ac power: " << getDesignValue(VehicleAttribute::MAX_AC_POWER) << "kW" << std::endl;
        std::cout << " + max RPM: " << getDesignValue(VehicleAttribute::MAX_RPM) << std::endl;
        std::cout << " + max range: " << getDesignValue(VehicleAttribute::MAX_RANGE) << "km" << std::endl;
        std::cout << " + engine total: " << getDesignValue(VehicleAttribute::ENGINE_TOTAL) << std::endl;
        std::cout << " + weight: " << getDesignValue(VehicleAttribute::WEIGHT) << "kg" << std::endl;
        std::cout << " + wheel radius: " << getDesignValue(VehicleAttribute::WHEEL_RADIUS) << "m" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    static int getDesignValue(VehicleAttribute attribute) {
        if (baseParam.find(attribute) != baseParam.end()) {
            return baseParam[attribute];
        } else {
            return 0;
        }
    }

    static VehicleBrand getBrand() { return brand; }
    static VehicleOption getOption() { return option; }
};

#endif // VEHICLE_CONFIG_H
