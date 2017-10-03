//
// Created by tom on 30.9.17.
//

#include <netdb.h>
#include <cstring>
#include <iostream>
#include "Connection.h"

int Connection::hostToIp(string host) {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ( getaddrinfo( host.c_str() , "110" , &hints , &servinfo) != 0)
    {
        freeaddrinfo(servinfo);
        return -1;
    }

    freeaddrinfo(servinfo);
    return 0;
}
Connection::Connection(){

}

Connection::Connection(const ParseParameters &parse) {
    address =   parse.address;
    portNum =   parse.portNum;
    paramD  =   parse.paramD;
    paramN  =   parse.paramN;
    paramO  =   parse.paramO;
    SSL     =   parse.paramS;
    USER    =   parse.USER;
    PASS    =   parse.PASS;
}

void Connection::establishConnection() {
    addrinfo hints, *serverinfo, *p;
    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(address.c_str(), portNum.c_str(), &hints, &serverinfo) != 0)
    {
        throw BadIpOrDomainError("getaddrinfo", "address has not been found");
    }

    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        freeaddrinfo(serverinfo);
        throw FailedToConnect("Failed to connect", "Try again?");
    }

    freeaddrinfo(serverinfo);
}

void Connection::sendCommand(string cmd) {
    cmd += "\r\n";
    if(!SSL)
    {
        if(write(sockfd, cmd.c_str(), cmd.size()) < 0)
        {} //ERROR thrwo exception
    }
    else
    {

    }

}

string Connection::recvMessage() {
    char buff[1];
    string ret = "";
    string tmp = "";
    int size = 0;
    if (!SSL)
    {
        while(true)
        {
            tmp.clear();
            tmp = recvLine(size);

            if (tmp == ".\r\n" || size == 0)
                break;
            else
            {
                ret += tmp;
            }
            size = 0;
        }
        return ret;
    }

    return ret;
}

string Connection::recvLine(int &size){
    char buff[1];
    string ret = "";
    while(readSock(buff, sizeof(buff))){
        ret += buff;
        size++;
        if(ret.back() == '\n' && ret[ret.length() - 2] == '\r')
        {
            break;
        }
    }
    return ret;
}

ssize_t Connection::readSock(char *buff, int size){
    int  byte = recv(sockfd, buff, size, 0);
    if (byte < 0)
    {
        //throw error
    }
    return byte;
}

bool Connection::validResponse() {
    int a;
    return (recvLine(a).front() == '+');
}

string Connection::recvMultLine() {
    char buff[1];
    string ret = "";

    bool dot = false;
    while(readSock(buff, sizeof(buff))){
        ret += buff;
        if(ret.back() == '\n' && dot)
            break;
        else if (ret.back() == '.')
            dot = true;
    }
    return ret;
}


