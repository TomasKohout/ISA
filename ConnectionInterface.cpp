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
    string substr = "";
    int size = 0;
    while(true)
    {
        tmp.clear();
        tmp = recvLine(size);


        if (tmp == ".\r\n" || size == 0)
        {
            break;
        }
        else if(tmp.front() == '.'){
            tmp.erase(0,1);
            ret += tmp;
        }
        else
            ret += tmp;

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
    size = 0;
    while(readSock(buff, sizeof(buff))){
        ret += buff;

        size++;
        if(ret[ret.length() - 2] == '\r' && ret.back() == '\n')
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
    vector<int> arr;
    int name = getStartNameNumber();
    int count_of_messages = getCountOfMessages();
    string respond;
    fstream cachefile(cachePath, ios::app);

    arr = getOnlyNew(count_of_messages);

    ssize_t hash;
    int j;
    for (j = 0; j < arr.size(); j++) {
        sendCommand("RETR " + to_string(arr[j]));
        if (this->validResponse()) {
            hash = hashFunc((respond = recvMessage()));
            cachefile << hash << endl;

            if (paramO.back() == '/')
            {
                ofstream outfile(paramO + to_string(name+j+1));
                outfile << respond;
                outfile.close();
            } else{
                ofstream outfile(paramO + "/" + to_string(name+j+1));
                outfile << respond;
                outfile.close();
            }
        }

    }

    cachefile.close();

    if (paramN)
        count = arr.size();
    else
        count = count_of_messages;
    return true;
}
/**
 *
 * @return
 */
vector<int> ConnectionInterface::getOnlyNew(int count) {
    vector<long> hash;
    vector<int> ret;

    ssize_t hashToCompare;

    ifstream hashFile(this->cachePath);
    long temp;
    while (hashFile >> temp)
        hash.push_back(temp);
    bool notNew = true;

    for (int k = 0; k < count; k++) {
        sendCommand("RETR " + to_string(k+1));
        if (validResponse()){
            hashToCompare = hashFunc(recvMessage());
            for (int i = 0; i < hash.size(); i++) {
                if (hash[i] == hashToCompare)
                {
                    notNew = false;
                    break;
                }
            }

            if (notNew)
                ret.push_back(k+1);
            else
                notNew = true;
        }
    }


    return ret;
}

vector<int> ConnectionInterface::getDelMsg() {
    vector<long> hash;
    vector<int> ret;
    ssize_t hashToCompare;

    ifstream hashFile(this->cachePath);
    long temp;
    while (hashFile >> temp)
        hash.push_back(temp);
    bool del = false;

    for (int i = 0; i < hash.size(); i++) {
        sendCommand("RETR " + to_string(i+1));
        if (validResponse()){
            hashToCompare = hashFunc(recvMessage());
            for (int k = 0; k < hash.size(); k++) {
                if (hash[k] == hashToCompare)
                {
                    del = true;
                    break;
                }
            }

            if (del)
                ret.push_back(i+1);
            else
                del = false;
        }
    }

    return ret;
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
            continue;
        }
        if (connect(this->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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
    createSock();

    if (paramS)
    {
        if (!this->validResponse())
            throw ServerError("Error", "Server respond is not OK");

        string tmp = "STLS\r\n";
        send(this->sockfd, tmp.c_str(), tmp.length(), 0);

        if (!this->validResponse())
            throw ServerError("STLS", "Server is not supporting STLS commnad");

        this->stupidFlag = true;    //this flag is as stupid as writer of this code
                                    //necessary for validResponse()
    }

    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    this->sslCtx = SSL_CTX_new(SSLv23_client_method());
    SSL_CTX_set_options(this->sslCtx, SSL_OP_SINGLE_DH_USE);

    if (this->paramFileC.empty() && this->paramDirC.empty())
        retSSLCert = SSL_CTX_set_default_verify_paths(this->sslCtx);
    else if (!this->paramFileC.empty() && !this->paramDirC.empty())
        retSSLCert = SSL_CTX_load_verify_locations(this->sslCtx, this->paramFileC.c_str(), this->paramDirC.c_str());
    else if (this->paramFileC.empty())
        retSSLCert = SSL_CTX_load_verify_locations(this->sslCtx, NULL, this->paramDirC.c_str());
    else if (this->paramDirC.empty())
        retSSLCert = SSL_CTX_load_verify_locations(this->sslCtx, this->paramFileC.c_str(), NULL);

    if (retSSLCert != 1)
        throw ClientError("SSL certs", "No certificates");

    this->ssl = SSL_new(this->sslCtx);
    SSL_set_fd(this->ssl, this->sockfd);

    if (SSL_connect(this->ssl) != 1)
        throw ServerError("SSL_connect", "Can not initialize a connection");
    X509 * cert = SSL_get_peer_certificate(this->ssl);
    if (cert == NULL)
        throw ServerError("Cert", "Server does not send any certificate");
    retSSLCert = SSL_get_verify_result(this->ssl);
    if(X509_V_OK != retSSLCert)
        throw ServerError("Cert", "Certificate is not OK");

}

void ConnectionInterface::destSSL() {
    ERR_free_strings();
    EVP_cleanup();
}

void ConnectionInterface::shutSSL() {
    if (this->ssl != NULL)
        SSL_shutdown(this->ssl);
    close(this->sockfd);
    if (this->ssl != NULL)
        SSL_free(this->ssl);
    if (this->sslCtx != NULL)
        SSL_CTX_free(this->sslCtx);

}

bool ConnectionInterface::deleteMessages(int &count) {
    vector<int> msg = getDelMsg();

    for (int i = 0; i < msg.size(); i++) {
        sendCommand("DELE " + to_string(msg[i]));
        if (!validResponse())
            throw ServerError("DEL", "Can not delete this email with ID: " + msg[i]);
    }
    count = msg.size();
    return true;
}

/**
 * Sends QUIT command to pop3 server and receive last respond.
 */
void ConnectionInterface::cleanUp() {
    sendCommand("QUIT");
    validResponse();
}

int ConnectionInterface::getCountOfMessages() {
    int a;
    sendCommand("STAT");
    string respond = recvLine(a);
    respond.erase(0,4);

    int i = static_cast<int>(respond.find_first_of(" "));
    respond.erase(i,respond.size());

    return atoi(respond.c_str());
}

int ConnectionInterface::getStartNameNumber() {
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

    int number = 0;
    int temp;
    stringstream stream(fileNames);
    while (stream >> temp)
    {
        if (temp > number)
            number = temp;
    }

    return number;
}




