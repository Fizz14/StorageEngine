#include "database.h"

void Record::setField(const std::string& column, const AnonType value) {
    fields[column] = value;
}

AnonType Record::getField(const std::string& column) const {
    auto it = fields.find(column);
    if (it != fields.end()) {
        return it->second;
    }
    return "";
}

bool Record::checkFieldType(const std::string& column, MyType type) const {
    auto it = fields.find(column);
    if (it != fields.end()) {
        switch (type) {
            case MyType::INT:
                return std::holds_alternative<int>(it->second);
            case MyType::DOUBLE:
                return std::holds_alternative<double>(it->second);
            case MyType::STRING:
                return std::holds_alternative<std::string>(it->second);
        }
    }
    return false;
}


const std::vector<std::tuple<std::string, MyType>>& Table::getFields() const {
    return fields;
}

const std::unordered_map<int, Record>& Table::getData() const {
    return data;
}

void Table::defineField(const std::string& fieldName, MyType type) {
    auto newEntry = std::make_tuple(fieldName, type);
    fields.push_back(newEntry);
}

void Table::createIndex(const std::string& column) {
    if (indexes.find(column) == indexes.end()) {
        indexes[column] = std::unordered_map<AnonType, std::set<int>>();
    }
}

void Table::insert(int id, const Record& record) {
    // Check types of fields in record against table's fields
    for (const auto& field : record.fields) {
        // Type checking logic can be added here
    }

    data[id] = record;
    for (const auto& field : record.fields) {
        if (indexes.find(field.first) != indexes.end()) {
            indexes[field.first][field.second].insert(id);
        }
    }
}

std::vector<int> Table::query(const std::string& column, const AnonType& value) {
    std::vector<int> result;
    if (indexes.find(column) != indexes.end() && indexes[column].find(value) != indexes[column].end()) {
        result = std::vector<int>(indexes[column][value].begin(), indexes[column][value].end());
    } else {
        // Perform linear search if no index is available
        for (const auto& pair : data) {
            if (pair.second.getField(column) == value) {
                result.push_back(pair.first);
            }
        }
    }
    return result;
}

std::vector<std::unordered_map<std::string, std::string>> Table::selectMultipleWhere(const std::vector<std::string>& columns, const std::string& conditionColumn, const AnonType& conditionValue) {
    std::vector<std::unordered_map<std::string, std::string>> result;
    for (const auto& pair : data) {
        const Record& record = pair.second;
        if (record.getField(conditionColumn) == conditionValue) {
            std::unordered_map<std::string, std::string> row;
            for (const auto& column : columns) {
                AnonType value = record.getField(column);
                row[column] = std::visit(makeStringFunctor(), value);
            }
            result.push_back(row);
        }
    }
    return result;
}

void Table::printAll(std::ostringstream& os) {
    for (const auto& pair : data) {
        os << "ID: " << pair.first << std::endl;
        for (const auto& field : pair.second.fields) {
            os << "  " << field.first << ": ";
            os << std::visit(makeStringFunctor(), field.second) << std::endl;
        }
    }
}
