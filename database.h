#include <iostream>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>

enum class MyType{INT, DOUBLE, STRING};

using AnonType = std::variant<int, double, std::string>;

struct makeStringFunctor {
  std::string operator()(int x) const { return std::to_string(x); }
  std::string operator()(double x) const { return std::to_string(x); }
  std::string operator()(const std::string &x) const { return x; }
};

class Record {
public:
  std::unordered_map<std::string, AnonType> fields;

  void setField(const std::string& column, const AnonType value) {
    fields[column] = value;
  }

  AnonType getField(const std::string& column) const {
    auto it = fields.find(column);
    if (it != fields.end()) {
      return it->second;
    }
    return "";
  }
};

class Table {
public:
  void defineField(const std::string& fieldName, MyType type) {
    auto newEntry = std::make_tuple(fieldName, type);
    fields.push_back(newEntry);
  }

  void createIndex(const std::string& column) {
    if (indexes.find(column) == indexes.end()) {
      indexes[column] = std::unordered_map<AnonType, std::set<int>>();
    }
  }

  void insert(int id, const Record& record) {
    //check types of fields in record against table's fields
    for(const auto& field : record.fields) {
      
    }
    
    data[id] = record;
    for (const auto& field : record.fields) {
      if (indexes.find(field.first) != indexes.end()) {
        indexes[field.first][field.second].insert(id);
      }
    }
  }

  std::vector<int> query(const std::string& column, const AnonType& value) {
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

  void printAll(std::ostringstream& os) {
    for (const auto& pair : data) {
      os << "ID: " << pair.first << std::endl;
      for (const auto& field : pair.second.fields) {
        os << "  " << field.first << ": ";
        os << std::visit( makeStringFunctor() , field.second) << std::endl;
      }
    }
  }

private:
  std::vector<std::tuple<std::string, MyType>> fields;
  std::unordered_map<int, Record> data;
  std::unordered_map<std::string, std::unordered_map<AnonType, std::set<int>>> indexes;
};

/*
int main() {
  Table userTable;

  // Define fields
  userTable.defineField("Name", MyType::STRING);
  userTable.defineField("Email", MyType::STRING);
  userTable.defineField("Age", MyType::INT);

  // Uncomment the line below to create an index on the "Name" field
  userTable.createIndex("Name");

  // Insert records
  Record record1;
  record1.setField("Name", "Alice");
  record1.setField("Email", "alice@example.com");
  record1.setField("Age", 24);
  userTable.insert(1, record1);

  Record record2;
  record2.setField("Name", "Bob");
  record2.setField("Email", "bob@example.com");
  record2.setField("Age", 22);
  userTable.insert(2, record2);

  Record record3;
  record3.setField("Name", 4.0);
  record3.setField("Email", "charlie@example.com");
  record3.setField("Age", 35);
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
*/
