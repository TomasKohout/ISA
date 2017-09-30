//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_CONNECTION_H
#define ISA_MASTER_CONNECTION_H

#include <string>
#include "Error.h"

using namespace std;

class Connection {
public:
    Connection();
    int hostToIp(string host);
    class BadIpOrDomainError;
};

class Connection::BadIpOrDomainError : Error{
public:
    BadIpOrDomainError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};


#endif //ISA_MASTER_CONNECTION_H
