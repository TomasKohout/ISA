//
// Created by tom on 30.9.17.
//

#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include "Connection.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

int Connection::hostToIp(string host) {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ( getaddrinfo( host.c_str() , "110" , &hints , &servinfo) != 0)
    {
        return -1;
    }


    freeaddrinfo(servinfo);
    return 0;
}
Connection::Connection(){

}

Connection::Connection(const ParseParameters &parse) {
    address =   parse.address;
    portNum =   parse.portNum;
    paramD  =   parse.paramD;
    paramN  =   parse.paramN;
    paramO  =   parse.paramO;
    paramS  =   parse.paramS;
    USER    =   parse.USER;
    PASS    =   parse.PASS;
}

void Connection::establishConnection() {
    addrinfo hints, *serverinfo, *p;
    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(address.c_str(), portNum.c_str(), &hints, &serverinfo) != 0)
    {
        throw BadIpOrDomainError("getaddrinfo", "address has not been found");
    }

    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        freeaddrinfo(serverinfo);
        throw FailedToConnect("Failed to connect", "Try again");
    }
    if(paramS)
        initSSL();
    else if (!validResponse())
        throw ServerError("Error" , "Server is broken?");

    freeaddrinfo(serverinfo);
}

void Connection::sendCommand(string cmd) {
    cmd += "\r\n";
    if(!paramS)
    {
        if(write(sockfd, cmd.c_str(), cmd.size()) < 0)
        {
            throw ServerError("Can not write into socket", "Server closed the connection");
        }
    }
    else
    {
        if (SSL_write(ssl, cmd.c_str(), static_cast<int>(cmd.size())) < 0)
            throw ServerError("Can not write into socket", "Server closed the connection");
    }

}

string Connection::recvMessage() {
    string ret = "";
    string tmp = "";
    int size = 0;
    while(true)
    {
        tmp.clear();
        tmp = recvLine(size);

        if (tmp == ".\r\n" || size == 0)
        {
            break;
        }
        else
        {
            ret += tmp;
        }
        size = 0;
    }
    return ret;
}

string Connection::recvLine(int &size){
    char buff[1];
    string ret = "";
    while(readSock(buff, sizeof(buff))){
        ret += buff;
        size++;
        if(ret.back() == '\n' && ret[ret.length() - 2] == '\r')
        {
            break;
        }
    }
    return ret;
}

ssize_t Connection::readSock(char *buff, int size){

    int byte;
    if (paramS && stupidFlag)
    {
        byte = SSL_read(ssl, buff, size);
        if (byte < 0)
            throw ServerError("Reading from socket is inaccesible.", "Server might be down");
    }
    else{
        byte = static_cast<int>(recv(sockfd, buff, size, 0));
        if (byte < 0)
        {
            throw ServerError("Reading from socket is inaccesible.", "Server might be down");
        }

    }
    return byte;
}

bool Connection::validResponse() {
    int a;
    return (recvLine(a).front() == '+');
}

bool Connection::authenticate() {
    establishConnection();
    int a;
    sendCommand("USER "+ USER );

    if(validResponse())
        sendCommand("PASS " + PASS );
    else{
        throw ServerError("USER", "Bad username");
    }
    if (validResponse())
        return true;
    else
        throw ServerError("PASS", "Wrong password");
}

bool Connection::downloadMessages(int &count) {
    int a;
    vector<msgData> arr;
    if (paramN)
        arr = getOnlyNew();
    else {
        sendCommand("UIDL");
        arr = parse.parseMultiline(recvMessage());
    }

    for (int i = 0; i < arr.size(); i++) {
        ofstream outfile(paramO + "/" + arr[i].uidl);
        sendCommand("RETR " + arr[i].id);
        if( parse.respond(recvLine(a)))
        {
            outfile << recvMessage() ;
            outfile.close();
        } else
            throw ServerError("Download Message", "I do not know");

    }
    count = static_cast<int>(arr.size());
}

vector<msgData> Connection::getOnlyNew() {
    vector<string> result;
    sendCommand("UIDL");
    vector<msgData> uidlResult = parse.parseMultiline(recvMessage());
    string searchStr = "ls " + paramO;
    char fileName[200];
    string fileNames;
    FILE *lsResult;
    lsResult = popen(searchStr.c_str(), "r");

    while(fgets(fileName, 200, lsResult))
    {
        fileNames += fileName;
    }

    stringstream stream(fileNames);
    string name;
    while (stream >> name)
    {
        for (int i = 0; i < uidlResult.size(); i++) {
            if (uidlResult[i].uidl == name)
                uidlResult.erase(uidlResult.begin() + i);
        }
    }

    return uidlResult;
}

void Connection::initSSL() {

    if (!validResponse())
    {
        throw ServerError("Error", "");
    }
    string tmp = "STLS\r\n";
    send(sockfd, tmp.c_str(), tmp.length(), 0);

    if (!validResponse())
    {
        //throw ex
    }
    stupidFlag = true;
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    sslCtx = SSL_CTX_new(SSLv23_client_method());
    SSL_CTX_set_options(sslCtx, SSL_OP_SINGLE_DH_USE);

    ssl = SSL_new(sslCtx);
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) != 1)
    {

    }
    //SSL_CTX_load_verify_locations(sslCtx, )
    //cert = SSL_CTX_set_default_verify_paths("/etc/ssl/certs", "/etc/ssl/private");

    if (cert == NULL)
    {

    }

    name = X509_NAME_new();
    name = X509_get_subject_name(cert);

}

void Connection::destSSL() {
    ERR_free_strings();
    EVP_cleanup();
}

void Connection::shutSSL() {
    SSL_shutdown(ssl);
    SSL_free(ssl);
}


