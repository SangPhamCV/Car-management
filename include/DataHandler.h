#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>
#include <fstream>
#include <sstream>

using CSVMap = std::unordered_map<std::string, std::string>;

class DataHandler {
private:
    DataHandler(const std::string& filename);

    static DataHandler* instance;
    static std::mutex mtx; // variable to ensure thread safety
    std::string filename;
    
public:
    ~DataHandler() = default;

    static DataHandler* getInstance();
    CSVMap readData();
    void updateData(const CSVMap& updates);
    std::string getValue(const std::string& key);
};

#endif // DATA_HANDLER_H