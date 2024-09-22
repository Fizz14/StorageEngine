#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <set>

class Record {
public:
    std::unordered_map<std::string, std::string> fields;

    void setField(const std::string& column, const std::string& value) {
        fields[column] = value;
    }

    std::string getField(const std::string& column) const {
        auto it = fields.find(column);
        if (it != fields.end()) {
            return it->second;
        }
        return "";
    }
};

class Table {
public:
    void defineField(const std::string& fieldName) {
        fields.push_back(fieldName);
    }

    void createIndex(const std::string& column) {
        if (indexes.find(column) == indexes.end()) {
            indexes[column] = std::unordered_map<std::string, std::set<int>>();
        }
    }

    void insert(int id, const Record& record) {
        data[id] = record;
        for (const auto& field : record.fields) {
            if (indexes.find(field.first) != indexes.end()) {
                indexes[field.first][field.second].insert(id);
            }
        }
    }

    std::vector<int> query(const std::string& column, const std::string& value) {
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

    void printAll() {
        for (const auto& pair : data) {
            std::cout << "ID: " << pair.first << std::endl;
            for (const auto& field : pair.second.fields) {
                std::cout << "  " << field.first << ": " << field.second << std::endl;
            }
        }
    }

private:
    std::vector<std::string> fields;
    std::unordered_map<int, Record> data;
    std::unordered_map<std::string, std::unordered_map<std::string, std::set<int>>> indexes;
};

int main() {
    Table userTable;

    // Define fields
    userTable.defineField("Name");
    userTable.defineField("Email");

    // Uncomment the line below to create an index on the "Name" field
    userTable.createIndex("Name");

    // Insert records
    Record record1;
    record1.setField("Name", "Alice");
    record1.setField("Email", "alice@example.com");
    userTable.insert(1, record1);

    Record record2;
    record2.setField("Name", "Bob");
    record2.setField("Email", "bob@example.com");
    userTable.insert(2, record2);

    Record record3;
    record3.setField("Name", "Charlie");
    record3.setField("Email", "charlie@example.com");
    userTable.insert(3, record3);

    std::cout << "All Records:" << std::endl;
    userTable.printAll();

    std::string searchColumn = "Name";
    std::string searchValue = "Alice";
    std::vector<int> results = userTable.query(searchColumn, searchValue);

    std::cout << "\nRecords with " << searchColumn << " = " << searchValue << ":" << std::endl;
    for (int id : results) {
        std::cout << "ID: " << id << std::endl;
    }

    return 0;
}
