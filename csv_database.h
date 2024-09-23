#ifndef CSV_DATABASE_H
#define CSV_DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "database.h"

class CsvDatabase {
public:
    CsvDatabase(const std::string& filename);
    void load(Table& table);
    void save(const Table& table);

private:
    std::string filename;
    std::vector<std::string> split(const std::string& s, char delimiter);
};

#endif // CSV_DATABASE_H
