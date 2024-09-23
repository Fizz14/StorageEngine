#include "network.h"

int main() {
    Table userTable;

    // Define fields
    userTable.defineField("Name", MyType::STRING);
    userTable.defineField("Email", MyType::STRING);
    userTable.defineField("Age", MyType::INT);

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

    startServer(userTable);

    return 0;
}
