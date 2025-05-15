#ifndef DRIVE_MODE_H
#define DRIVE_MODE_H

struct DriveModeFactor {
    double ECO = 0.60;   
    double SPORT = 0.80; 
};

class DriveMode {
public:
    enum class Mode {ECO, SPORT};

    DriveMode();
    ~DriveMode();

    void setMode(Mode mode);
    Mode getMode() const {return currentMode;}

    int getPowerOutput() const; // Get power output
    int limitSpeedECO(int curentSpeed) const; // Limit speed in ECO mode
    

private:
    Mode currentMode;
    int powerOutputSport;
    int powerOutputECO;
    int maxSpeedECO;
};

#endif // DRIVE_MODE_H