//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_CONNECTION_H
#define ISA_MASTER_CONNECTION_H

#include <string>
#include "Error.h"
#include "ParseParameters.h"
#include "ConnectionInterface.h"


using namespace std;
class ParseParameters;

class Connection : public ConnectionInterface {

private:
    string address = "";
    string portNum = "";
    bool paramD = false;    //delete messages
    bool paramN = false;    //work with new messages
    string USER;            //user name
    string PASS;            //user password
    string paramO = "";     //out dir
    bool SSL = false;

    int sockfd;

public:
    Connection();
    Connection(const ParseParameters &parse);
    static int hostToIp(string host);
    void establishConnection() override;
    void setUpSSL();
    void sendCommand(string cmd) override;
    string recvLine();



    string recvMessage() override;
    class BadIpOrDomainError;
    class FailedToConnect;

private:
    bool writeMsg();

    void pop3Automat();

    bool validResponse();
    ssize_t readSock(char *buff, int size);
};

class Connection::FailedToConnect : Error{
public:
    FailedToConnect(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

class Connection::BadIpOrDomainError : Error{
public:
    BadIpOrDomainError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};


#endif //ISA_MASTER_CONNECTION_H
