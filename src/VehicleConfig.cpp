#include "VehicleConfig.h"

VehicleOption ElectricVehicleInit::option = VehicleOption::NOT_SET;
VehicleBrand ElectricVehicleInit::brand = VehicleBrand::NOT_SET;
vehicleBaseParam ElectricVehicleInit::baseParam = {
    {VehicleAttribute::BATTERY_CAPACITY,0},
    {VehicleAttribute::BATTERY_VOLTAGE,0},
    {VehicleAttribute::MAX_TORQUE,0},
    {VehicleAttribute::MAX_AC_POWER,0},
    {VehicleAttribute::MAX_ENGINE_POWER,0},
    {VehicleAttribute::MAX_SPEED_SPORT,0},
    {VehicleAttribute::MAX_SPEED_ECO,0},
    {VehicleAttribute::MAX_RPM,0},
    {VehicleAttribute::WEIGHT,0},
    {VehicleAttribute::AC_TEMP_MAX,0},
    {VehicleAttribute::AC_TEMP_MIN,0},
    {VehicleAttribute::WIND_LEVEL_MAX,0},
    {VehicleAttribute::ENGINE_TOTAL,0}         
};

vehicleBaseParam TeslaModel3Standard = {
    {VehicleAttribute::BATTERY_CAPACITY,54},
    {VehicleAttribute::BATTERY_VOLTAGE,350},
    {VehicleAttribute::MAX_RANGE,409},
    {VehicleAttribute::MAX_TORQUE,300},
    {VehicleAttribute::MAX_AC_POWER,2500},
    {VehicleAttribute::MAX_ENGINE_POWER,211},
    {VehicleAttribute::MAX_SPEED_SPORT,225},
    {VehicleAttribute::MAX_SPEED_ECO,160},
    {VehicleAttribute::MAX_RPM,16000},
    {VehicleAttribute::WEIGHT,1612},
    {VehicleAttribute::WHEEL_RADIUS,34},
    {VehicleAttribute::AC_TEMP_MAX,28},
    {VehicleAttribute::AC_TEMP_MIN,15},
    {VehicleAttribute::WIND_LEVEL_MAX,5},
    {VehicleAttribute::ENGINE_TOTAL,1}
};

vehicleBaseParam TeslaModel3LongRange = {
    {VehicleAttribute::BATTERY_CAPACITY,75},
    {VehicleAttribute::BATTERY_VOLTAGE,350},
    {VehicleAttribute::MAX_RANGE,560},
    {VehicleAttribute::MAX_TORQUE,440},
    {VehicleAttribute::MAX_AC_POWER,3000},
    {VehicleAttribute::MAX_ENGINE_POWER,324},
    {VehicleAttribute::MAX_SPEED_SPORT,233},
    {VehicleAttribute::MAX_SPEED_ECO,190},
    {VehicleAttribute::MAX_RPM,17000},
    {VehicleAttribute::WEIGHT,1847},
    {VehicleAttribute::WHEEL_RADIUS,35},
    {VehicleAttribute::AC_TEMP_MAX,28},
    {VehicleAttribute::AC_TEMP_MIN,15},
    {VehicleAttribute::WIND_LEVEL_MAX,5},
    {VehicleAttribute::ENGINE_TOTAL,2},
};

vehicleBaseParam HyundaiIoniq5Performance = {
    {VehicleAttribute::BATTERY_CAPACITY,75},
    {VehicleAttribute::BATTERY_VOLTAGE,350},
    {VehicleAttribute::MAX_RANGE,530},
    {VehicleAttribute::MAX_TORQUE,650},
    {VehicleAttribute::MAX_AC_POWER,3500},
    {VehicleAttribute::MAX_ENGINE_POWER,393},
    {VehicleAttribute::MAX_SPEED_SPORT,261},
    {VehicleAttribute::MAX_SPEED_ECO,210},
    {VehicleAttribute::MAX_RPM,18000},
    {VehicleAttribute::WEIGHT,1847},
    {VehicleAttribute::WHEEL_RADIUS,36},
    {VehicleAttribute::AC_TEMP_MAX,28},
    {VehicleAttribute::AC_TEMP_MIN,15},
    {VehicleAttribute::WIND_LEVEL_MAX,5},
    {VehicleAttribute::ENGINE_TOTAL,2}    
};

vehicleBaseParam HyundaiIoniq5 = {
    {VehicleAttribute::BATTERY_CAPACITY,58},
    {VehicleAttribute::BATTERY_VOLTAGE,360},
    {VehicleAttribute::MAX_RANGE,400},
    {VehicleAttribute::MAX_TORQUE,300},
    {VehicleAttribute::MAX_AC_POWER,2500},
    {VehicleAttribute::MAX_ENGINE_POWER,211},
    {VehicleAttribute::MAX_SPEED_SPORT,225},
    {VehicleAttribute::MAX_SPEED_ECO,160},
    {VehicleAttribute::MAX_RPM,16000},
    {VehicleAttribute::WEIGHT,1612},
    {VehicleAttribute::WHEEL_RADIUS,34},
    {VehicleAttribute::AC_TEMP_MAX,28},
    {VehicleAttribute::AC_TEMP_MIN,15},
    {VehicleAttribute::WIND_LEVEL_MAX,5},
    {VehicleAttribute::ENGINE_TOTAL,1}
};

ElectricVehicleInit::ElectricVehicleInit(VehicleOption option, VehicleBrand brand) {
    if (brand == VehicleBrand::TESLA) {
        std::string optionName;
        this->brand = VehicleBrand::TESLA;
        if (option == VehicleOption::STANDAND) {
            baseParam = TeslaModel3Standard;
            this->option = VehicleOption::STANDAND;
            optionName = "STANDAND";
        } else if (option == VehicleOption::LONG_RANGE) {
            baseParam = TeslaModel3LongRange;
            this->option = VehicleOption::LONG_RANGE;
            optionName = "LONG_RANGE";
        }
        if (optionName.empty()) {
            std::cout << "Invalid option for Tesla model 3" << std::endl;
            return;
        }
        std::cout << "parameters of Tesla model 3: (" + optionName + ") is loaded" << std::endl;
    } else if (brand == VehicleBrand::HYUNDAI) {
        std::string optionName;
        this->brand = VehicleBrand::HYUNDAI;
        if (option == VehicleOption::STANDAND) {
            baseParam = HyundaiIoniq5;
            this->option = VehicleOption::STANDAND;
            optionName = "STANDAND";
        } else if (option == VehicleOption::PERFORMANCE) {
            baseParam = HyundaiIoniq5Performance;
            this->option = VehicleOption::PERFORMANCE;
            optionName = "PERFORMANCE";
        }
        if (optionName.empty()) {
            std::cout << "Invalid option for Hyundai ioniq 5" << std::endl;
            return;
        }
        std::cout << "parameters of Hyundai ioniq 5: (" + optionName + ") is loaded" << std::endl;
    } else {
        std::cout << "Invalid brand or option" << std::endl;
    }
}

ElectricVehicleInit::~ElectricVehicleInit() {}

bool strToBool(const std::string& str) {
    return (str == "1");
}
