/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2025  langningchen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**********************************************************************/

#include "Socket.hpp"
#include "Settings.hpp"
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std::literals::string_literals;

void SOCKET::SubThread(int Socket, sockaddr_in ClientAddress, SOCKET::CALL_BACK CallBack) {
    std::string ClientName = std::to_string(Socket) + "(" + inet_ntoa(ClientAddress.sin_addr) + ":" + std::to_string(ntohs(ClientAddress.sin_port)) + ")";

    std::string RequestHTTPData = "";
    fd_set ReadSet;
    FD_ZERO(&ReadSet);
    FD_SET(Socket, &ReadSet);
    timeval TimeOut;
    TimeOut.tv_sec = 1;
    TimeOut.tv_usec = 0;
    while (true) {
        int Result = select(Socket + 1, &ReadSet, NULL, NULL, &TimeOut);
        if (Result == -1) {
            Logger.Error("Can not select from socket " + ClientName);
            break;
        }
        if (Result == 0) {
            FD_ZERO(&ReadSet);
            FD_SET(Socket, &ReadSet);
            continue;
        }
        char Buffer[1024];
        int Length = recv(Socket, Buffer, 1024, 0);
        if (Length == -1) {
            Logger.Error("Can not receive from socket " + ClientName);
            break;
        }
        if (Length == 0)
            break;
        RequestHTTPData += std::string(Buffer, Length);
        if (Length < 1024)
            break;
    }

    std::string ResponseHTTPData = CallBack(RequestHTTPData);
    for (size_t i = 0; i < ResponseHTTPData.length(); i += 1024) {
        size_t Length = std::min(ResponseHTTPData.length() - i, (size_t)1024);
        int SendLength = send(Socket, ResponseHTTPData.substr(i, Length).c_str(), Length, 0);
        if (SendLength == -1) {
            Logger.Error("Can not send to socket " + ClientName);
            break;
        }
    }

    if (close(Socket) == -1)
        Logger.Error("Can not close socket " + ClientName);
}

SOCKET::SOCKET(CALL_BACK CallBack) {
    signal(SIGPIPE, SIG_IGN);

    SETTINGS::GetSettings("SocketPort", Port);

    memset(&ServerAddress, 0, sizeof(struct sockaddr_in));
    ServerAddress.sin_port = htons(Port);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_family = AF_INET;

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == -1)
        Logger.Fatal("Can not create socket");

    int OptionValue = 1;
    setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &OptionValue, sizeof(OptionValue));

    if (bind(ListenSocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1)
        Logger.Fatal("Can not bind port " + std::to_string(Port));

    if (listen(ListenSocket, 1024) == -1)
        Logger.Fatal("Can not listen");
    Logger.Warning("Listening on port " + std::to_string(Port));

    while (true) {
        struct sockaddr_in ClientAddress;
        socklen_t ClientAddressLength = sizeof(ClientAddress);
        int ClientSocket = accept(ListenSocket, (struct sockaddr *)&ClientAddress, &ClientAddressLength);
        if (ClientSocket == -1)
            Logger.Error("Can not accept");
        std::thread(
            [this, ClientSocket, ClientAddress, CallBack] {
                this->SubThread(ClientSocket, ClientAddress, CallBack);
                close(ClientSocket);
            }).detach();
    }
}

SOCKET::~SOCKET() {
    close(ListenSocket);
}
