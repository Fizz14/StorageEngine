#include "csv_database.h"

CsvDatabase::CsvDatabase(const std::string& filename) : filename(filename) {}

std::vector<std::string> CsvDatabase::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void CsvDatabase::load(Table& table) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Read header line
    std::vector<std::string> headers = split(line, ',');

    while (std::getline(file, line)) {
        std::vector<std::string> values = split(line, ',');
        if (values.size() != headers.size()) {
            std::cerr << "Mismatch between number of headers and values in line: " << line << std::endl;
            continue; // Skip this line
        }

        Record record;
        for (size_t i = 0; i < headers.size(); ++i) {
            const auto& fieldName = headers[i];
            const auto& fieldType = table.getFields()[i];

            try {
                // Check type and set field
                switch (std::get<1>(fieldType)) {
                    case MyType::INT:
                        record.setField(fieldName, std::stoi(values[i]));
                        break;
                    case MyType::DOUBLE:
                        record.setField(fieldName, std::stod(values[i]));
                        break;
                    case MyType::STRING:
                        record.setField(fieldName, values[i]);
                        break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error setting field " << fieldName << " with value " << values[i] << ": " << e.what() << std::endl;
                throw; // Re-throw the exception to stop loading
            }
        }
        table.insert(std::stoi(values[0]), record); // Assuming the first column is the ID
    }

    file.close();
}

void CsvDatabase::save(const Table& table) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    // Write header
    std::ostringstream headerStream;
    for (const auto& field : table.getFields()) {
        headerStream << std::get<0>(field) << ",";
    }
    std::string header = headerStream.str();
    header.pop_back(); // Remove trailing comma
    file << header << "\n";

    // Write data
    for (const auto& pair : table.getData()) {
        std::ostringstream lineStream;
        lineStream << pair.first << ","; // Write ID
        for (const auto& field : pair.second.fields) {
            lineStream << std::visit(makeStringFunctor(), field.second) << ",";
        }
        std::string line = lineStream.str();
        line.pop_back(); // Remove trailing comma
        file << line << "\n";
    }

    file.close();
}
