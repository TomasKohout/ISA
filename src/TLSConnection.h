//
// Created by tom on 9.10.17.
//

#ifndef ISA_MASTER_TLSCONNECTION_H
#define ISA_MASTER_TLSCONNECTION_H

#include <string>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include "ConnectionInterface.h"
#include "ParseParameters.h"

using namespace std;

class TLSConnection : public ConnectionInterface{
public:
    explicit TLSConnection(const ParseParameters &parse);
    ~TLSConnection();

protected:
    void establishConnection() override;
    void sendCommand(string cmd) override;

    ssize_t readSock(char *buff, int size) override;

};


#endif //ISA_MASTER_TLSCONNECTION_H
