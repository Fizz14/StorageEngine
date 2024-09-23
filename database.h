#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

enum class MyType { INT, DOUBLE, STRING };

using AnonType = std::variant<int, double, std::string>;

struct makeStringFunctor {
    std::string operator()(int x) const { return std::to_string(x); }
    std::string operator()(double x) const { return std::to_string(x); }
    std::string operator()(const std::string &x) const { return x; }
};

class Record {
public:
    std::unordered_map<std::string, AnonType> fields;

    void setField(const std::string& column, const AnonType value);
    AnonType getField(const std::string& column) const;
    bool checkFieldType(const std::string& column, MyType type) const;
};

class Table {
public:
    void defineField(const std::string& fieldName, MyType type);
    void createIndex(const std::string& column);
    void insert(int id, const Record& record);
    std::vector<int> query(const std::string& column, const AnonType& value);
    std::vector<std::unordered_map<std::string, std::string>> selectMultipleWhere(const std::vector<std::string>& columns, const std::string& conditionColumn, const AnonType& conditionValue);
    void printAll(std::ostringstream& os);

    const std::vector<std::tuple<std::string, MyType>>& getFields() const;
    const std::unordered_map<int, Record>& getData() const;

private:
    std::vector<std::tuple<std::string, MyType>> fields;
    std::unordered_map<int, Record> data;
    std::unordered_map<std::string, std::unordered_map<AnonType, std::set<int>>> indexes;
};

class Database {
public:
    void addTable(const std::string& tableName, const Table& table);
    Table& getTable(const std::string& tableName);
    void removeTable(const std::string& tableName);

private:
    std::unordered_map<std::string, Table> tables;
};

#endif // DATABASE_H
