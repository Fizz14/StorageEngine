#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <set>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "database.h" // Include your database header

#pragma comment(lib, "Ws2_32.lib")

void handleClient(SOCKET clientSocket, Table& table) {
    char buffer[1024] = {0};
    recv(clientSocket, buffer, 1024, 0);

    std::string request(buffer);
    std::istringstream requestStream(request);
    std::string method, path, version;
    requestStream >> method >> path >> version;

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/plain\r\n";
    responseStream << "Connection: close\r\n\r\n";

    table.printAll(responseStream);

    std::string response = responseStream.str();
    send(clientSocket, response.c_str(), response.size(), 0);
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

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

    std::cout << "Server is running on port 8080" << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        std::thread(handleClient, clientSocket, std::ref(userTable)).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
