//
// Created by tom on 1.10.17.
//
#include <string>

#ifndef ISA_MASTER_CONNECTIONINTERFACE_H
#define ISA_MASTER_CONNECTIONINTERFACE_H
class ConnectionInterface{
public:
    virtual void sendCommand(std::string cmd)= 0;
    virtual std::string recvMessage()= 0;
    virtual void establishConnection()= 0;

};
#endif //ISA_MASTER_CONNECTIONINTERFACE_H
