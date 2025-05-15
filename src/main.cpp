#include "VehicleConfig.h"
#include "DataHandler.h"
#include "DashboardController.h"
#include "Display.h"
#include "SafetyManager.h"
#include "DriveMode.h"
#include "BatteryManager.h"
#include "SpeedCalculator.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>

void setTerminalRawMode(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
            std::cerr << "Error: Failed to get terminal attributes" << std::endl;
            return;
        }
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        newt.c_cc[VMIN] = 0;
        newt.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
            std::cerr << "Error: Failed to set terminal raw mode" << std::endl;
        }
    } else {
        if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1) {
            std::cerr << "Error: Failed to restore terminal mode" << std::endl;
        }
    }
}

void setNonBlocking(bool enable) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error: Failed to get file descriptor flags" << std::endl;
        return;
    }
    if (enable) {
        if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Error: Failed to set non-blocking mode" << std::endl;
        }
    } else {
        if (fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK) == -1) {
            std::cerr << "Error: Failed to clear non-blocking mode" << std::endl;
        }
    }
}

std::mutex shareMutex;

// Atomic variables for thread-safe access without locks
std::atomic<int> acTemp(0), odometer(0), windLevel(0), remainingRange(0);
std::atomic<int> currentSpeed(0), batteryLevel(0), batteryTemp(0), turnSignal(0);
std::atomic<bool> acStatus(false), brakeStatus(false), acceleratorStatus(false), ecoModeChanged(false);

std::string driveMode = "ECO";

void vehicleInit(DataHandler* handler);
void readData(DataHandler* handler, DashboardController* dashboardController);
void inputHandler(DataHandler* handler, SafetyManager* safetyManager, DriveMode* driveModeHandler);
void mainLoop(DataHandler* dataHandler, Display* display, 
              SpeedCalculator* speedCalculator, BatteryManager* batteryManager, DriveMode* driveModeHandler);

int main() {
    setTerminalRawMode(true);
    setNonBlocking(true);

    DataHandler* dataHandler = DataHandler::getInstance();

    ElectricVehicleInit TeslaModel3(VehicleOption::LONG_RANGE, VehicleBrand::TESLA);
    TeslaModel3.displayVehicleInfo();

    DashboardController* dashboardController = new DashboardController();
    Display* display = new Display(dashboardController);
    SafetyManager* safetyManager = new SafetyManager();
    DriveMode* driveModeHandler = new DriveMode();
    SpeedCalculator* speedCalculator = new SpeedCalculator(driveModeHandler, safetyManager);
    BatteryManager* batteryManager = new BatteryManager(speedCalculator);

    vehicleInit(dataHandler);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::thread dataThread(readData, dataHandler, dashboardController);
    std::thread inputThread(inputHandler, dataHandler, safetyManager, driveModeHandler);

    mainLoop(dataHandler, display, speedCalculator, batteryManager, driveModeHandler);

    dataThread.join();
    inputThread.join();

    setTerminalRawMode(false);
    setNonBlocking(false);

    delete speedCalculator;
    delete batteryManager;
    delete dashboardController;
    delete dataHandler;
    delete safetyManager;
    delete driveModeHandler;
    delete display;

    return 0;
}

void vehicleInit(DataHandler* dataHandler) {
    static const int MAX_RANGE = ElectricVehicleInit::getDesignValue(VehicleAttribute::MAX_RANGE);
    
    {
        std::lock_guard<std::mutex> lock(shareMutex);
        dataHandler->updateData({
            {"VEHICLE_SPEED", "0"},
            {"DRIVE_MODE", "ECO"},
            {"WIND_LEVEL", "2"},
            {"BATTERY_LEVEL", "100"},
            {"AC_STATUS", "1"},
            {"AC_CONTROL", "22"},
            {"BATTERY_TEMP", "35"},
            {"BRAKE", "0"},
            {"ACCELERATOR", "0"},
            {"ODOMETER", "0"},
            {"ROUTE_PLANNER", std::to_string(MAX_RANGE)},
            {"TURN_SIGNAL", "0"}
        });
    }
}

void readData(DataHandler* handler, DashboardController* dashboardController) {
    while (true) {
        std::unordered_map<std::string, std::string> allData;
        {
            std::lock_guard<std::mutex> lock(shareMutex);
            allData = handler->readData();
            if (allData.count("DRIVE_MODE")) {
                driveMode = allData["DRIVE_MODE"];
                if (driveMode == "ECO") {
                    ecoModeChanged = true;
                }
            }
        }
        dashboardController->readData(allData);
        
        try {
            if (allData.count("AC_CONTROL"))    acTemp = std::stoi(allData.at("AC_CONTROL"));
            if (allData.count("WIND_LEVEL"))    windLevel = std::stoi(allData.at("WIND_LEVEL"));
            if (allData.count("TURN_SIGNAL"))   turnSignal = std::stoi(allData.at("TURN_SIGNAL"));
            if (allData.count("ODOMETER"))      odometer = std::stoi(allData.at("ODOMETER"));
            if (allData.count("BATTERY_TEMP"))  batteryTemp = std::stoi(allData.at("BATTERY_TEMP"));
            if (allData.count("VEHICLE_SPEED")) currentSpeed = std::stoi(allData.at("VEHICLE_SPEED"));
            if (allData.count("BATTERY_LEVEL")) batteryLevel = std::stoi(allData.at("BATTERY_LEVEL"));
            if (allData.count("ROUTE_PLANNER")) remainingRange = std::stoi(allData.at("ROUTE_PLANNER"));
            if (allData.count("BRAKE"))         brakeStatus = (allData.at("BRAKE") == "1");
            if (allData.count("ACCELERATOR"))   acceleratorStatus = (allData.at("ACCELERATOR") == "1");
            if (allData.count("AC_STATUS"))     acStatus = (allData.at("AC_STATUS") == "1");
        } catch (const std::exception& e) {
            std::cerr << "Error in readData: " << e.what() << std::endl;
        }
         
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }
}

enum class KeyState { RELEASED, PRESSED };

void inputHandler(DataHandler* handler, SafetyManager* safetyManager, DriveMode* driveModeHandler) {
    const int AC_MIN = ElectricVehicleInit::getDesignValue(VehicleAttribute::AC_TEMP_MIN);
    const int AC_MAX = ElectricVehicleInit::getDesignValue(VehicleAttribute::AC_TEMP_MAX);
    const int MAX_WIND_LEVEL = ElectricVehicleInit::getDesignValue(VehicleAttribute::WIND_LEVEL_MAX);

    std::unordered_map<char, bool> keyStates = {{'w', false}, {'s', false}};
    char ch;
    
    while (true) {
        keyStates['w'] = false;
        keyStates['s'] = false;

        {
            std::lock_guard<std::mutex> lock(shareMutex);
            while (read(STDIN_FILENO, &ch, 1) > 0) {
                switch (ch) {
                    case 'c': // Increase AC temperature
                        if (acStatus) {
                            if (acTemp < AC_MAX) acTemp++;
                            handler->updateData({{"AC_CONTROL", std::to_string(acTemp)}});
                        }
                        break;
                        
                    case 'z': // Decrease AC temperature
                        if (acStatus) {
                            if (acTemp > AC_MIN) acTemp--;
                            handler->updateData({{"AC_CONTROL", std::to_string(acTemp)}});
                        }
                        break;
                        
                    case 'x': // Toggle AC status
                        acStatus = !acStatus;
                        handler->updateData({
                            {"AC_STATUS", acStatus ? "1" : "0"},
                            {"AC_CONTROL", acStatus ? "22" : "0"}
                        });
                        break;
                        
                    case 'a': // Adjust wind level
                        if (acStatus) {
                            windLevel = (windLevel < MAX_WIND_LEVEL) ? windLevel + 1 : 0;
                            handler->updateData({{"WIND_LEVEL", std::to_string(windLevel)}});
                        }
                        break;
                        
                    case 'd': // Toggle drive mode
                        if (driveMode == "ECO") {
                            handler->updateData({{"DRIVE_MODE", "SPORT"}});
                            driveModeHandler->setMode(DriveMode::Mode::SPORT);
                            driveMode = "SPORT";
                        } else {
                            ecoModeChanged = true;
                            handler->updateData({{"DRIVE_MODE", "ECO"}});
                            driveModeHandler->setMode(DriveMode::Mode::ECO);
                            driveMode = "ECO";
                        }
                        outputPower = driveModeHandler->getPowerOutput();
                        break;
                        
                    case 'q': // Left turn signal
                        turnSignal = (turnSignal == 1) ? 0 : 1;
                        handler->updateData({{"TURN_SIGNAL", std::to_string(turnSignal)}});
                        break;
                        
                    case 'e': // Right turn signal
                        turnSignal = (turnSignal == 2) ? 0 : 2;
                        handler->updateData({{"TURN_SIGNAL", std::to_string(turnSignal)}});
                        break;
                        
                    case 'w': // Accelerator
                        keyStates['w'] = true;
                        break;
                        
                    case 's': // Brake
                        keyStates['s'] = true;
                        break;
                }
            }

            // Update accelerator and brake status
            bool newAcceleratorStatus = keyStates['w'];
            if (acceleratorStatus != newAcceleratorStatus) {
                acceleratorStatus = newAcceleratorStatus;
                handler->updateData({{"ACCELERATOR", acceleratorStatus ? "1" : "0"}});
            }
            bool newBrakeStatus = keyStates['s'];
            if (brakeStatus != newBrakeStatus) {
                brakeStatus = newBrakeStatus;
                handler->updateData({{"BRAKE", brakeStatus ? "1" : "0"}});
            }
        }

        isSafetyAction = safetyManager->isBrakeAndAcceleratorCoincidence(brakeStatus, acceleratorStatus);
        brakeIntensityDisplay = safetyManager->getBrakeIntensity();
        gasIntensityDisplay = safetyManager->getAcceleratorIntensity();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void mainLoop(DataHandler* dataHandler, Display* display, 
              SpeedCalculator* speedCalculator, BatteryManager* batteryManager, DriveMode* driveModeHandler) {
    int updateSpeed = 0;
    outputPower = driveModeHandler->getPowerOutput();
    
    while (true) {
        display->updateDisplay();
        
        batteryManager->updateBatteryCapacity(acTemp, windLevel);
        double updateBatteryCapacity = batteryManager->getBatteryCapacity();
        double updateRemainingRange = batteryManager->calculateRemainingRange();
        updateBatteryTemp = batteryManager->calculateBatteryTemp();
        updateOdometer = speedCalculator->getTotalDistance();

        if (ecoModeChanged) {
            if (updateSpeed > speedCalculator->getMaxSpeed("ECO")) {
                updateSpeed = driveModeHandler->limitSpeedECO(updateSpeed);
            } else {
                ecoModeChanged = false;
            }
        } else {
            updateSpeed = speedCalculator->calculateSpeed(acceleratorStatus, brakeStatus);
        }
        
        std::unordered_map<std::string, std::string> updates;
        if (std::abs(updateSpeed - currentSpeed) >= 1) {
            updates["VEHICLE_SPEED"] = std::to_string(updateSpeed);
            currentSpeed = updateSpeed;
        }
        if (std::abs(updateOdometer - odometer) >= 0.1) {
            updates["ODOMETER"] = std::to_string(updateOdometer);
            odometer = updateOdometer;
        }
        if (std::abs(updateBatteryCapacity - batteryLevel) >= 1) {
            updates["BATTERY_LEVEL"] = std::to_string(static_cast<int>(updateBatteryCapacity));
            batteryLevel = static_cast<int>(updateBatteryCapacity);
        }
        if (std::abs(updateRemainingRange - remainingRange) >= 0.1) {
            updates["ROUTE_PLANNER"] = std::to_string(static_cast<int>(updateRemainingRange));
            remainingRange = static_cast<int>(updateRemainingRange);
        }
        if (std::abs(updateBatteryTemp - batteryTemp) >= 0.1) {
            updates["BATTERY_TEMP"] = std::to_string(static_cast<int>(updateBatteryTemp));
            batteryTemp = static_cast<int>(updateBatteryTemp);
        }
        
        if (!updates.empty()) {
            std::lock_guard<std::mutex> lock(shareMutex);
            dataHandler->updateData(updates);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
}