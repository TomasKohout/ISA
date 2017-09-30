//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_CONNECTION_H
#define ISA_MASTER_CONNECTION_H

#include <string>
using namespace std;

class Connection {
public:
    Connection();
    int hostToIp(string host);
};


#endif //ISA_MASTER_CONNECTION_H
