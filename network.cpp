#include "network.h"

// Existing methods...

void handleHttpsRequest(SOCKET clientSocket, Database& database, const std::string& request) {
    std::istringstream requestStream(request);
    std::string method, path, version;
    requestStream >> method >> path >> version;

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/plain\r\n";
    responseStream << "Connection: close\r\n\r\n";

    // Parse the path to extract query parameters
    size_t pos = path.find("?");
    if (pos != std::string::npos) {
        std::string queryString = path.substr(pos + 1);
        size_t separatorPos = queryString.find("|");
        if (separatorPos != std::string::npos) {
            std::string tableName = queryString.substr(0, separatorPos);
            std::string columnsAndCondition = queryString.substr(separatorPos + 1);

            size_t conditionPos = columnsAndCondition.find("|");
            if (conditionPos != std::string::npos) {
                std::string columnsStr = columnsAndCondition.substr(0, conditionPos);
                std::string conditionStr = columnsAndCondition.substr(conditionPos + 1);

                std::istringstream conditionStream(conditionStr);
                std::string conditionColumn, conditionValue;
                std::getline(conditionStream, conditionColumn, '=');
                std::getline(conditionStream, conditionValue);

                // Split columnsStr into individual column names
                std::vector<std::string> columns;
                std::istringstream columnsStream(columnsStr);
                std::string column;
                while (std::getline(columnsStream, column, ',')) {
                    columns.push_back(column);
                }

                // Perform the query
                try {
                    Table& table = database.getTable(tableName);
                    std::vector<std::unordered_map<std::string, std::string>> results = table.selectMultipleWhere(columns, conditionColumn, conditionValue);

                    // Format the results
                    responseStream << "Query results for " << conditionColumn << " = " << conditionValue << ":\n";
                    for (const auto& row : results) {
                        for (const auto& col : columns) {
                            responseStream << col << ": " << row.at(col) << "\n";
                        }
                        responseStream << "\n";
                    }
                } catch (const std::exception& e) {
                    responseStream << "Error: " << e.what() << "\n";
                }
            } else {
                responseStream << "Invalid query format.\n";
            }
        } else {
            responseStream << "Invalid query format.\n";
        }
    } else {
        responseStream << "Invalid query format.\n";
    }

    std::string response = responseStream.str();
    send(clientSocket, response.c_str(), response.size(), 0);
    closesocket(clientSocket);
}

void handleClient(SOCKET clientSocket, Database& database) {
    try {
        char buffer[1024] = {0};
        recv(clientSocket, buffer, 1024, 0);

        std::string request(buffer);
        handleHttpsRequest(clientSocket, database, request);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        closesocket(clientSocket);
    }
}

void startServer(Database& database) {
    WSADATA wsaData;
    int iResult;

    try {
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cerr << "WSAStartup failed: " << iResult << std::endl;
            return;
        }

        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(8080);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        std::cout << "Server is running on port 8080" << std::endl;

        while (true) {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
                closesocket(serverSocket);
                WSACleanup();
                return;
            }
            std::thread(handleClient, clientSocket, std::ref(database)).detach();
        }

        closesocket(serverSocket);
        WSACleanup();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        WSACleanup();
    }
}
