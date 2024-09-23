#include "database.h"

// Record methods
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

// Table methods
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
        AnonType fieldValue = record.getField(conditionColumn);

        // Debugging statements
        std::cout << "Checking record ID: " << pair.first << std::endl;
        std::cout << "Field value: " << std::visit(makeStringFunctor(), fieldValue) << std::endl;
        std::cout << "Condition value: " << std::visit(makeStringFunctor(), conditionValue) << std::endl;

        bool match = false;
        if (fieldValue.index() == conditionValue.index()) {
            match = fieldValue == conditionValue;
        } else {
            // Handle comparison between different types
            if (std::holds_alternative<int>(fieldValue) && std::holds_alternative<std::string>(conditionValue)) {
                match = std::to_string(std::get<int>(fieldValue)) == std::get<std::string>(conditionValue);
            } else if (std::holds_alternative<double>(fieldValue) && std::holds_alternative<std::string>(conditionValue)) {
                match = std::to_string(std::get<double>(fieldValue)) == std::get<std::string>(conditionValue);
            } else if (std::holds_alternative<std::string>(fieldValue) && std::holds_alternative<int>(conditionValue)) {
                match = std::get<std::string>(fieldValue) == std::to_string(std::get<int>(conditionValue));
            } else if (std::holds_alternative<std::string>(fieldValue) && std::holds_alternative<double>(conditionValue)) {
                match = std::get<std::string>(fieldValue) == std::to_string(std::get<double>(conditionValue));
            }
        }

        if (match) {
            std::unordered_map<std::string, std::string> row;
            for (const auto& column : columns) {
                AnonType value = record.getField(column);
                row[column] = std::visit(makeStringFunctor(), value);
            }
            result.push_back(row);

            // Debugging statement
            std::cout << "Record matched. Collected row: ";
            for (const auto& col : columns) {
                std::cout << col << ": " << row[col] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Debugging statements
    std::cout << "Final query results for " << conditionColumn << " = " << std::visit(makeStringFunctor(), conditionValue) << ":\n";
    for (const auto& row : result) {
        for (const auto& col : columns) {
            std::cout << col << ": " << row.at(col) << "\n";
        }
        std::cout << "\n";
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

const std::vector<std::tuple<std::string, MyType>>& Table::getFields() const {
    return fields;
}

const std::unordered_map<int, Record>& Table::getData() const {
    return data;
}

// Database methods
void Database::addTable(const std::string& tableName, const Table& table) {
    tables[tableName] = table;
}

Table& Database::getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
        throw std::runtime_error("Table not found: " + tableName);
    }
    return it->second;
}

void Database::removeTable(const std::string& tableName) {
    tables.erase(tableName);
}
