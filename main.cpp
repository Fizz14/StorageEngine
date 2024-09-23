#include "network.h"
#include "csv_database.h"

int main() {
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

    return 0;
}
