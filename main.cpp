#include "network.h"
#include "csv_database.h"

int main() {
    try {
        Database database;

        // Define and load the first table
        Table userTable;
        userTable.defineField("ID", MyType::INT);
        userTable.defineField("Name", MyType::STRING);
        userTable.defineField("Email", MyType::STRING);
        userTable.defineField("Age", MyType::INT);

        CsvDatabase csvDbUsers("users.csv");
        csvDbUsers.load(userTable);
        database.addTable("users", userTable);

        // Define and load the second table (example)
        Table productTable;
        productTable.defineField("ProductID", MyType::INT);
        productTable.defineField("ProductName", MyType::STRING);
        productTable.defineField("Price", MyType::DOUBLE);

        CsvDatabase csvDbProducts("products.csv");
        csvDbProducts.load(productTable);
        database.addTable("products", productTable);

        // Start the server
        startServer(database);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
