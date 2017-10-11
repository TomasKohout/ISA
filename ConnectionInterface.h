//
// Created by tom on 1.10.17.
//
#ifndef ISA_MASTER_CONNECTIONINTERFACE_H
#define ISA_MASTER_CONNECTIONINTERFACE_H
#include <string>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include "Error.h"
#include <openssl/ossl_typ.h>
#include <vector>

#include "structures.h"
using namespace std;
class ConnectionInterface {
protected:
    string address = "";
    string portNum = "";
    string USER;            //user name
    string PASS;            //user password
    string paramO = "";     //out dir
    string paramFileC = "";
    string paramDirC = "";

    bool paramD = false;    //delete messages
    bool paramN = false;    //work with new messages
    bool paramS = false;
    bool paramT = false;
    bool stupidFlag = false;

    int sockfd;
    BIO *bio;
    ssl_st *ssl;
    SSL_CTX *sslCtx;
    SSL *cssl;
    X509 *cert;
    X509_NAME *name;

public:
    virtual void establishConnection()= 0;
    void cleanUp();

    bool authenticate();
    bool downloadMessages(int &count);
    bool deleteMessages(int &count);

    static int hostToIp(string host);

    class ClientError;
    class FailedToConnect;
    class ServerError;

protected:
    virtual ssize_t readSock(char *buff, int size) = 0;
    virtual void sendCommand(std::string cmd) = 0;

    vector<msgData> getDelMsg();
    vector<msgData> getUIDLs();
    vector<msgData> getOnlyNew();
    vector<msgData> parseMultiline(string multLine);

    void createSock();
    void initSSL();
    void destSSL();
    void shutSSL();

    bool validResponse();
    string recvLine(int &size);
    string recvMessage();
};


class ConnectionInterface::FailedToConnect : public Error{
public:
    FailedToConnect(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

class ConnectionInterface::ClientError : public Error{
public:
    ClientError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

class ConnectionInterface::ServerError : public Error{
public:
    ServerError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};



#endif //ISA_MASTER_CONNECTIONINTERFACE_H
