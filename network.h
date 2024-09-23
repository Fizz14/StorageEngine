#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "database.h"

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

void handleClient(SOCKET clientSocket, Database& database);
void startServer(Database& database);
void handleHttpsRequest(SOCKET clientSocket, Database& database, const std::string& request);

#endif // NETWORK_H
