//
// Created by tom on 9.10.17.
//
#include <fcntl.h>
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
        else if(tmp.front() == '.'){
            tmp.erase(0,1);
        }
        ret += tmp;

        size = 0;
    }

    ret.pop_back();
    ret.pop_back();
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
 * This function download and rewrite downloaded messages that have same hash as is in cachePath file.
 * @param count_of_messages count of actual messages to download
 * @return count of downloaded messages
 */
int ConnectionInterface::regularDownload(int count_of_messages)
{
    string respond;
    fstream cachefile(cachePath, ios::app);
    vector<int> arr;
    int name;
    for (int i = 0; i < count_of_messages; i++) {
        arr.push_back(i+1);
    }
    name = checkMessages();


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
    return j;

}

/**
 * Downloads new emails.
 * If paramN (-n) is set, only new emails will be downloaded.
 * @return count of messages downloaded
 */
int ConnectionInterface::downloadMessages() {
    if (paramN)
        return getOnlyNew(getCountOfMessages());
    else
        return regularDownload(getCountOfMessages());
}
/**
 * Find out what messages are already downloaded and which are not. This function download messages from mailbox
 * and check if hash of that specific message is in hashfile. If it is not than this message will be downloaded.
 * @return vector of nubers that are specific to messages
 */
int ConnectionInterface::getOnlyNew(int count) {
    vector<long> hash;
    int name = getStartNameNumberForNews();
    ssize_t hashToCompare;
    string msg;
    fstream hashFile(this->cachePath);
    long temp;
    while (hashFile >> temp)
        hash.push_back(temp);
    bool newMsg = true;
    int counter = 0;
    for (int k = 0; k < count; k++) {
        sendCommand("RETR " + to_string(k+1));
        if (validResponse()){
            hashToCompare = hashFunc((msg=recvMessage()));
            for (int i = 0; i < hash.size(); i++) {
                if (hash[i] == hashToCompare)
                {
                    newMsg = false;
                    break;
                }
            }

            if (newMsg)
            {
                counter++;
                if (paramO.back() == '/')
                {
                    ofstream outfile(paramO + to_string(name+counter));
                    outfile << msg;
                    outfile.close();
                } else{
                    ofstream outfile(paramO + "/" + to_string(name+counter));
                    outfile << msg;
                    outfile.close();
                }
                hashFile.clear();
                hashFile << hashToCompare << endl;
            }
            else
                newMsg = true;
        }
    }

    hashFile.close();

    return counter;


}

/**
 * Set an timeout for @var time sec.
 * @param time seconds to wait for
 * @return return value of select() function
 */
int ConnectionInterface::timeout(int time){
    timeval timeout;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(this->sockfd, &fds);

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    return select(this->sockfd+1, &fds, NULL, NULL, &timeout);
}

/**
 * This method is from http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html.
 */
void ConnectionInterface::createSock() {
    addrinfo hints, *serverinfo, *p;
    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int flags, n, error;
    fd_set waitFd, tmpFd;
    timeval tval;
    socklen_t len;


    if (getaddrinfo(this->address.c_str(), this->portNum.c_str(), &hints, &serverinfo) != 0)
    {
        throw ClientError("getaddrinfo", "address has not been found");
    }

    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((this->sockfd = socket(p->ai_family, p->ai_socktype,
                                   p->ai_protocol)) == -1) {
            continue;
        }
        flags = fcntl(this->sockfd, F_GETFL, 0);
        fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);

        if ((n = connect(this->sockfd, p->ai_addr, p->ai_addrlen)) < 0) {
            if (errno != EINPROGRESS)
            {
                close(this->sockfd);
                continue;
            }
        }

        if (n == 0)
            break;

        FD_ZERO(&waitFd);
        FD_SET(this->sockfd, &waitFd);
        tmpFd = waitFd;
        tval.tv_sec = 10;
        tval.tv_usec = 0;

        if ((n = select(this->sockfd + 1, &waitFd, &tmpFd, nullptr, &tval )) == 0)
        {
            close(this->sockfd);
            continue;
        }

        if (FD_ISSET(this->sockfd, &waitFd) || FD_ISSET(this->sockfd , &tmpFd))
        {
            len = sizeof(error);
            if (getsockopt(this->sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
                continue;
        }
        else
        {
            continue;
        }

        break;
    }
    if (p == NULL) {
        freeaddrinfo(serverinfo);
        throw FailedToConnect("Failed to connect", "");
    }

    freeaddrinfo(serverinfo);

    fcntl(this->sockfd, F_SETFL, flags);

    if (error)
    {
        throw FailedToConnect("Failed to connect", "");
    }


}

void ConnectionInterface::setSockNonBlock(){
    int flags = fcntl(this->sockfd, F_GETFL, 0);
    if (fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK) == -1){

    }
}
/**
 * This method initialize SSL and create socket. It also send STLS command if paramS is present.
 */
void ConnectionInterface::initSSL() {
    long retSSLCert;

    if (paramS)
    {
        if (!this->validResponse())
            throw ServerError("Error", "Server respond is not OK");

        string tmp = "STLS\r\n";
        send(this->sockfd, tmp.c_str(), tmp.length(), 0);

        if (!this->validResponse())
            throw ServerError("STLS", "Server is not supporting STLS commnad");

        this->stupidFlag = true;    //this flag is as stupid as a writer of this code
                                    //necessary for validResponse()
    }
    else{
        createSock();
    }

    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    this->sslCtx = SSL_CTX_new(SSLv23_client_method());
    SSL_CTX_set_options(this->sslCtx, SSL_OP_SINGLE_DH_USE);

    //sets certificate
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
    //get certificate from server
    X509 * cert = SSL_get_peer_certificate(this->ssl);
    if (cert == NULL)
        throw ServerError("Cert", "Server does not send any certificate");
    //check if certificate is ok
    retSSLCert = SSL_get_verify_result(this->ssl);
    if(X509_V_OK != retSSLCert)
        throw ServerError("Cert", "Certificate is not OK");

    setSockNonBlock();
}

/**
 * Destroy ERR_strings.
 */
void ConnectionInterface::destSSL() {
    ERR_free_strings();
    EVP_cleanup();
}

/**
 * Closes SSL connection and socket.
 */
void ConnectionInterface::shutSSL() {
    if (this->ssl != NULL)
        SSL_shutdown(this->ssl);
    close(this->sockfd);
    if (this->ssl != NULL)
        SSL_free(this->ssl);
    if (this->sslCtx != NULL)
        SSL_CTX_free(this->sslCtx);

}
/**
 * Delete all messages in mailbox that are downloaded localy.
 * @return
 */
int ConnectionInterface::deleteMessages() {
    vector<long> hash;
    ssize_t hashToCompare;

    ifstream hashFile(this->cachePath);
    long temp;
    while (hashFile >> temp)
        hash.push_back(temp);

    bool del = false;
    hashFile.close();
    int counter = 0;
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
            {
                counter++;
                sendCommand("DELE " + to_string(i+1));
                if (!validResponse()) {
                    cerr << "Could not delete this msg: " + to_string(i + 1) << endl;

                    counter--;
                }
            }
            else
                del = false;
        }
    }
    hashFile.close();

    //delete cache file
    fstream cache;
    cache.open(this->cachePath, ofstream::out | ofstream::trunc);
    cache.close();
    return counter;
}

/**
 * Sends QUIT command to pop3 server and receive last respond.
 */
void ConnectionInterface::cleanUp() {
    sendCommand("QUIT");
    if (!validResponse())
        throw ServerError("Messages will not be deleted." , "");
}

/**
 * Gets count of messages in mailbox.
 * @return count of messages in mailbox
 */
int ConnectionInterface::getCountOfMessages() {
    int a;
    sendCommand("STAT");
    string respond = recvLine(a);
    respond.erase(0,4);

    int i = static_cast<int>(respond.find_first_of(" "));
    respond.erase(i,respond.size());

    return atoi(respond.c_str());
}

/**
 * Retrieve name for new messages if -n is setted.
 * @return name of message
 */
int ConnectionInterface::getStartNameNumberForNews() {
    string fileNames = lsOutDir();

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
/**
 * This function is like ls in linux.
 * @return string of filenames.
 */
string ConnectionInterface::lsOutDir(){
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

    return fileNames;
}

/**
 * This method is used for getting the right number for naming messages. Checks message hash against hashes in cachefile.
 * @return name of new message
 */
int ConnectionInterface::checkMessages() {
    vector<long> hash;
    vector<int> files;
    stringstream fileNames(lsOutDir());
    ifstream cacheFile(cachePath);

    stringstream cacheFileStr;
    cacheFileStr << cacheFile.rdbuf();
    long line;
    int name;
    while (fileNames >> name)
    {
        files.push_back(name);
    }
    while (cacheFileStr >> line)
    {
        hash.push_back(line);
    }

    int startName = 0;
    bool flag = true;
    for (int i = 0; i < files.size(); i++) {
        ifstream message(paramO + to_string(files[i]));

        stringstream tmpStream;
        tmpStream << message.rdbuf();
        long msgHash = hashFunc(tmpStream.str());
        tmpStream.clear();
        message.close();
        for (int j = 0; j < hash.size(); j++) {
            if (msgHash == hash[j])
                flag = false;
        }
        if (flag)
        {
            if (files[i] > startName)
                startName = files[i];
        }
    }


    return startName;

}




