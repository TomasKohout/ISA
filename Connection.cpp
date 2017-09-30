//
// Created by tom on 30.9.17.
//

#include <netdb.h>
#include <cstring>
#include "Connection.h"

int Connection::hostToIp(string host) {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ( getaddrinfo( host.c_str() , "110" , &hints , &servinfo) != 0)
        return -1;

    freeaddrinfo(servinfo);
    return 0;
}

Connection::Connection() {


}
