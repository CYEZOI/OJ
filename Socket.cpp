#include "Settings.hpp"
#include "Socket.hpp"
#include <thread>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std::literals::string_literals;

void SOCKET::SubThread(int Socket, sockaddr_in ClientAddress, SOCKET::CALL_BACK CallBack)
{
    std::string ClientName = std::to_string(Socket) + "(" + inet_ntoa(ClientAddress.sin_addr) + ":" + std::to_string(ntohs(ClientAddress.sin_port)) + ")";

    std::string RequestHTTPData = "";
    while (true)
    {
        char Buffer[1024];
        int Length = recv(Socket, Buffer, 1024, 0);
        if (Length == -1)
        {
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
    for (size_t i = 0; i < ResponseHTTPData.length(); i += 1024)
    {
        size_t Length = std::min(ResponseHTTPData.length() - i, (size_t)1024);
        int SendLength = send(Socket, ResponseHTTPData.substr(i, Length).c_str(), Length, 0);
        if (SendLength == -1)
        {
            Logger.Error("Can not send to socket " + ClientName);
            break;
        }
    }

    if (close(Socket) == -1)
        Logger.Error("Can not close socket " + ClientName);
}

SOCKET::SOCKET(CALL_BACK CallBack)
{
    signal(SIGPIPE, SIG_IGN);

    Port = Settings.GetSocketPort();

    memset(&ServerAddress, 0, sizeof(struct sockaddr_in));
    ServerAddress.sin_port = htons(Port);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_family = AF_INET;

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == -1)
        Logger.Fetal("Can not create socket");

    if (bind(ListenSocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1)
        Logger.Fetal("Can not bind port " + std::to_string(Port));

    if (listen(ListenSocket, 1024) == -1)
        Logger.Fetal("Can not listen");

    while (true)
    {
        struct sockaddr_in ClientAddress;
        socklen_t ClientAddressLength = sizeof(ClientAddress);
        int ClientSocket = accept(ListenSocket, (struct sockaddr *)&ClientAddress, &ClientAddressLength);
        if (ClientSocket == -1)
            Logger.Error("Can not accept");
        // new std::thread(SubThread, ClientSocket, ClientAddress, CallBack);
        new std::thread(
            [this, ClientSocket, ClientAddress, CallBack]
            {
                this->SubThread(ClientSocket, ClientAddress, CallBack);
            });
    }
}

SOCKET::~SOCKET()
{
    close(ListenSocket);
}
