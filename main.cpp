#include "network.h"
#include "csv_database.h"

int main() {
    try {
        Table userTable;

        // Define fields
        userTable.defineField("ID", MyType::INT);
        userTable.defineField("Name", MyType::STRING);
        userTable.defineField("Email", MyType::STRING);
        userTable.defineField("Age", MyType::INT);

        // Load data from CSV
        CsvDatabase csvDb("users.csv");
        csvDb.load(userTable);

        // Start the server
        startServer(userTable);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
