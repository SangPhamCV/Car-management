#include "DataHandler.h"

DataHandler* DataHandler::instance = nullptr;
std::mutex DataHandler::mtx;

DataHandler::DataHandler(const std::string& filename) : filename(filename) {}

DataHandler* DataHandler::getInstance() {
    std::lock_guard<std::mutex> lock(mtx); // lock the mutex
    if (instance == nullptr) {
        instance = new DataHandler("../data/Database.csv"); // Create instance with default filename
    }
    return instance;
}

CSVMap DataHandler::readData() {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }

    CSVMap data;
    std::string line, key, value;
    std::getline(infile, line); // Skip header

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        if (std::getline(ss, key, ',') && std::getline(ss, value)) {
            data[key] = value;
        }
    }
    
    infile.close();
    return data;
}

void DataHandler::updateData(const CSVMap& updates) {
    CSVMap data = readData();
    // Update data
    for (const auto& [k, v] : updates) {
        data[k] = v;
    }
    // Write updated data
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    outfile << "key,value" << std::endl;
    // Write data
    for (const auto& [k, v] : data) {
        outfile << k << "," << v << std::endl;
    }
    outfile.close();
}

std::string DataHandler::getValue(const std::string& key) {
    CSVMap data = readData();
    std::lock_guard<std::mutex> lock(mtx);
    if (data.find(key) != data.end()) {
        return data.at(key);
    }
}