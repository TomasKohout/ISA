//
// Created by tom on 30.9.17.
//
#include "Connection.h"

Connection::Connection(const ParseParameters &parse) {
    this->address =   parse.address;
    this->portNum =   parse.portNum;
    this->paramD  =   parse.paramD;
    this->paramN  =   parse.paramN;
    this->paramO  =   parse.paramO;
    this->paramS  =   parse.paramS;
    this->paramT  =   parse.paramT;
    this->USER    =   parse.USER;
    this->PASS    =   parse.PASS;
    this->paramFileC = parse.paramFileC;
    this->paramDirC  = parse.paramDirC;
}

void Connection::establishConnection() {
    this->createSock();

    if(this->paramS)
        this->initSSL();
    else if (!this->validResponse())
        throw ServerError("Error" , "Server is broken?");
}

void Connection::sendCommand(string cmd) {
    cmd += "\r\n";
    if(!this->paramS)
    {
        if(write(this->sockfd, cmd.c_str(), cmd.size()) < 0)
        {
            throw ServerError("Can not write into socket", "Server closed the connection");
        }
    }
    else
    {
        if (SSL_write(this->ssl, cmd.c_str(), static_cast<int>(cmd.size())) < 0)
            throw ServerError("Can not write into socket", "Server closed the connection");
    }
}

ssize_t Connection::readSock(char *buff, int size){

    int byte;
    if (this->paramS && this->stupidFlag)
    {
        byte = SSL_read(this->ssl, buff, size);
        if (byte < 0)
            throw ServerError("Reading from socket is inaccesible.", "Server might be down");
    }
    else{
        byte = static_cast<int>(recv(this->sockfd, buff, size, 0));
        if (byte < 0)
        {
            throw ServerError("Reading from socket is inaccesible.", "Server might be down");
        }

    }
    return byte;
}

Connection::~Connection() {
    if (this->paramS)
    {
        this->shutSSL();
        this->destSSL();
    }
    else
        close(this->sockfd);
}




