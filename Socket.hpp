#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Logger.hpp"
#include <string>
#include <arpa/inet.h>

class SOCKET
{
public:
    typedef std::string (*CALL_BACK)(std::string RequestHTTPData);

private:
    int Port;
    int ListenSocket;
    struct sockaddr_in ServerAddress;

    void SubThread(int Socket, sockaddr_in ClientAddress, CALL_BACK CallBack);

public:
    SOCKET(CALL_BACK CallBack);
    ~SOCKET();
};

#endif
