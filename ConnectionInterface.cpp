//
// Created by tom on 9.10.17.
//
#include "ConnectionInterface.h"
/**
 * Receive all multiline responds.
 * Keep reading from socket until ".\r\n". This is not included in @var ret
 * @return string with multiline message
 */
string ConnectionInterface::recvMessage() {
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
/**
 * Reads from socket until \r\n.
 * @param size Return size of chars in string
 * @return string
 */
string ConnectionInterface::recvLine(int &size) {
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
/**
 * Read line and checks first char.
 * If first char is +, true will be returned. False otherwise.
 * @return
 */
bool ConnectionInterface::validResponse() {
    int a;
    return (recvLine(a).front() == '+');
}

/**
 * Authentication state.
 * Sends USER and PASS commands and gets responds.
 * @return
 */
bool ConnectionInterface::authenticate() {
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

/**
 * Downloads new emails.
 * If paramN (-n) is set, only new emails will be downloaded.
 * @param count of messages
 * @return true
 */
bool ConnectionInterface::downloadMessages(int &count) {
    int a;
    vector<msgData> arr;
    if (paramN)
        arr = getOnlyNew();
    else{
        sendCommand("UIDL");
        arr = parseMultiline(recvMessage());
    }

    for (int i = 0; i < arr.size(); i++) {
        ofstream outfile(paramO + "/" + arr[i].uidl); //every file containing message is named by uidl
        sendCommand("RETR " + arr[i].id); //send command for downloading specific message
        if(this->validResponse()) //read one line of respond, server info
        {
            outfile << recvMessage(); //reads message and save it to a file
            outfile.close();
        } else
            throw ServerError("Download Message", "I do not know");

    }
    count = static_cast<int>(arr.size());
    return true;
}
/**
 * Gets names of already downladed messages using popen and ls command.
 * @return vector of msgData, msgData.id is empty string
 */
vector<msgData> ConnectionInterface::getUIDLs(){
    vector<msgData> ret;
    string searchStr = "ls " + paramO;
    char fileName[200];
    string fileNames;
    FILE *lsResult;
    lsResult = popen(searchStr.c_str(), "r");

    while(fgets(fileName, 200, lsResult)) //reads from pipe returned by popen
    {
        fileNames += fileName;
    }
    pclose(lsResult); //close pipe

    stringstream stream(fileNames);
    string name;
    int i = 0;
    while (stream >> name)
    {
        ret.push_back(msgData()); //add structure to vector
        ret[i].id = "";
        ret[i].uidl = name; //set uidl
        i++;
    }
    return ret;
}

/**
 *
 * @return
 */
vector<msgData> ConnectionInterface::getOnlyNew() {
    vector<string> result;
    sendCommand("UIDL");
    vector<msgData> uidlResult = parseMultiline(recvMessage());
    vector<msgData> localMsgs = getUIDLs();
    for (int j = 0; j < localMsgs.size(); j++) {
        for (int i = 0; i < uidlResult.size(); i++) {
            if (uidlResult[i].uidl == localMsgs[j].uidl)
                uidlResult.erase(uidlResult.begin() + i);
        }
    }

    return uidlResult;
}

vector<msgData> ConnectionInterface::getDelMsg() {
    vector<string> result;
    sendCommand("UIDL");
    vector<msgData> uidlResult = parseMultiline(recvMessage());
    vector<msgData> localMsgs = getUIDLs();
    for (int j = 0; j < localMsgs.size(); j++) {
        for (int i = 0; i < uidlResult.size(); i++) {
            if (uidlResult[i].uidl != localMsgs[j].uidl)
                uidlResult.erase(uidlResult.begin() + i);
        }
    }

    return uidlResult;
}

int ConnectionInterface::hostToIp(string host) {
    addrinfo hints;
    addrinfo *servinfo;

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

void ConnectionInterface::createSock() {
    addrinfo hints, *serverinfo, *p;
    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(this->address.c_str(), this->portNum.c_str(), &hints, &serverinfo) != 0)
    {
        throw ClientError("getaddrinfo", "address has not been found");
    }

    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((this->sockfd = socket(p->ai_family, p->ai_socktype,
                                   p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (connect(this->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(this->sockfd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        freeaddrinfo(serverinfo);
        throw FailedToConnect("Failed to connect", "Try again");
    }
    freeaddrinfo(serverinfo);
}

void ConnectionInterface::initSSL() {
    long retSSLCert;

    if (paramS)
    {
        if (!this->validResponse())
        {
            throw ServerError("Error", "Server respond is not OK");
        }
        string tmp = "STLS\r\n";
        send(this->sockfd, tmp.c_str(), tmp.length(), 0);

        if (!this->validResponse())
        {
            throw ServerError("STLS", "Server is not supporting STLS commnad");
        }
        this->stupidFlag = true;    //this flag is as stupid as writer of this code
                                    //necessary for validResponse()
    }

    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    this->sslCtx = SSL_CTX_new(SSLv23_client_method());
    SSL_CTX_set_options(this->sslCtx, SSL_OP_SINGLE_DH_USE);

    this->ssl = SSL_new(this->sslCtx);

    if (this->paramFileC.empty() && this->paramDirC.empty())
        retSSLCert = SSL_CTX_set_default_verify_paths(this->sslCtx);
    else
        retSSLCert = SSL_CTX_load_verify_locations(this->sslCtx, this->paramFileC.c_str(), this->paramDirC.c_str());

    if (retSSLCert == 0)
        throw ClientError("SSL certs", "No certificates");

    SSL_set_mode(this->ssl, SSL_MODE_AUTO_RETRY);
    SSL_set_fd(this->ssl, this->sockfd);

    if (SSL_connect(this->ssl) != 1)
    {
        throw ServerError("SSL_connect", "Can not initialize a connection");
    }

    this->cert = SSL_get_peer_certificate(this->ssl);


    if (this->cert)
        X509_free(this->cert);
    else
        throw ServerError("Cert", "Certificate is not OK");

    retSSLCert = SSL_get_verify_result(this->ssl);
    if(X509_V_OK != retSSLCert)
        throw ServerError("Cert", "Certificate is not OK");

}

void ConnectionInterface::destSSL() {
    ERR_free_strings();
    EVP_cleanup();
}

void ConnectionInterface::shutSSL() {
    close(this->sockfd);
    SSL_CTX_free(this->sslCtx);
    SSL_shutdown(this->ssl);
    SSL_free(this->ssl);
}


vector<msgData> ConnectionInterface::parseMultiline(string multLine) {
    vector<msgData> array;
    size_t count = multLine.find_first_of('\n');

    for (int i = 0; i <= count  ; i++) {
        multLine.erase(0, 1);
    }

    istringstream stream(multLine);
    string uidl;
    string id;
    int i = 0;
    for (string word; stream >> id, stream >> uidl ; i++) {
        if (id.compare(".") != 0 || uidl.compare(".") != 0)
        {
            array.push_back(msgData());
            array[i].id = id;
            array[i].uidl = uidl;
        }
    }

    return array;
}

bool ConnectionInterface::deleteMessages(int &count) {
    vector<msgData> msg = getDelMsg();

    for (int i = 0; i < msg.size(); i++) {
        sendCommand("DEL " + msg[i].id);
        if (!validResponse())
            throw ServerError("DEL", "Can not delete this email with ID: " + msg[i].id + " and UIDL: " + msg[i].uidl);
    }

    return true;
}

/**
 * Sends QUIT command to pop3 server and receive last respond.
 */
void ConnectionInterface::cleanUp() {
    sendCommand("QUIT");
    validResponse();
}


