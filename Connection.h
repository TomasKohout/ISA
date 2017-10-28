//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_CONNECTION_H
#define ISA_MASTER_CONNECTION_H

#include <string>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <openssl/ossl_typ.h>
#include "Error.h"
#include "ParseParameters.h"
#include "ConnectionInterface.h"



using namespace std;
class ParseParameters;

class Connection : public ConnectionInterface {
public:
    explicit Connection(const ParseParameters &parse);
    ~Connection();

protected:
    void establishConnection() override ;
    void sendCommand(string cmd) override;

    ssize_t readSock(char *buff, int size) override;
};

#endif //ISA_MASTER_CONNECTION_H
