//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_CONNECTION_H
#define ISA_MASTER_CONNECTION_H

#include <string>
#include <openssl/ossl_typ.h>
#include "Error.h"
#include "ParseParameters.h"
#include "ConnectionInterface.h"
#include "ParseResponds.h"
#include "structures.h"


using namespace std;
class ParseParameters;

class Connection : public ConnectionInterface {

private:
    string address = "";
    string portNum = "";
    bool paramD = false;    //delete messages
    bool paramN = false;    //work with new messages
    bool paramS = false;
    string USER;            //user name
    string PASS;            //user password
    string paramO = "";     //out dir
    bool stupidFlag = false;
    ParseResponds parse = ParseResponds();

    int sockfd;
    ssl_st *ssl;
    SSL_CTX *sslCtx;
    SSL *cssl;
    X509 * cert;
    X509_NAME *name;

public:
    Connection();
    Connection(const ParseParameters &parse);
    static int hostToIp(string host);
    void establishConnection() override;
    void setUpSSL();
    void sendCommand(string cmd) override;
    string recvLine(int &size);
    string recvMessage() override;
    string recvMultLine();
    bool authenticate();
    bool downloadMessages(int &count);


    class BadIpOrDomainError;
    class FailedToConnect;
    class ServerError;

private:
    vector<msgData> getOnlyNew();
    void initSSL();
    void destSSL();
    void shutSSL();


    bool validResponse();
    ssize_t readSock(char *buff, int size);
};

class Connection::FailedToConnect : public Error{
public:
    FailedToConnect(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

class Connection::BadIpOrDomainError : public Error{
public:
    BadIpOrDomainError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

class Connection::ServerError : public Error{
public:
    ServerError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

#endif //ISA_MASTER_CONNECTION_H
