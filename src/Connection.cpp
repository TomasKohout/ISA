//
// Created by tom on 30.9.17.
//
#include "Connection.h"
/**
 * Constructor. Set up atributs.
 * @param parse ParseParameters object
 */
Connection::Connection(const ParseParameters &parse) {
    this->address =   parse.address;
    this->portNum =   parse.portNum;
    this->paramD  =   parse.paramD;
    this->paramN  =   parse.paramN;
    this->paramO  =   parse.paramO;
    this->paramS  =   parse.paramS;
    this->paramT  =   parse.paramT;
    this->USER    =   parse.USER;
    this->PASS    =   parse.PASS;
    this->paramFileC = parse.paramFileC;
    this->paramDirC  = parse.paramDirC;

    if (paramO.back() == '/')
        this->cachePath += paramO + ".cache/file";
    else
        this->cachePath += paramO + "/.cache/file";
}
/**
 * This method is used for calling initSSL().
 * For more information go to ConnectionInterface::initSSL().
 */
void Connection::establishConnection() {
    this->createSock();

    if (!this->paramS)
        setSockNonBlock();

    if(this->paramS)
        this->initSSL();
    else if (!this->validResponse())
        throw ServerError("Error" , "Server is broken?");
}

/**
 * This is implemented virtual method from ConnectionInterface.
 * It is used for sending data.
 *
 * @param cmd std::string without \r\n
 */
void Connection::sendCommand(string cmd) {
    cmd += "\r\n";
    if(!this->paramS)
    {
        if(write(this->sockfd, cmd.c_str(), cmd.size()) < 0)
        {
            throw ServerError("Can not write into socket", "Server closed the connection");
        }
    }
    else
    {
        if (SSL_write(this->ssl, cmd.c_str(), static_cast<int>(cmd.size())) < 0)
            throw ServerError("Can not write into socket", "Server closed the connection");
    }
}

/**
 * This is implemented virtual method from ConnectionInterface.
 * It is used for reading data from socket usualy by one byte.
 * Using nonblocking sockets.
 * @param buff array of chars
 * @param size of buff
 * @return count of read bytes
 */
ssize_t Connection::readSock(char *buff, int size){

    int byte;

    if (this->paramS && this->stupidFlag)
    {
        while ((byte = SSL_read(this->ssl, buff, size)) < 0)
        {
            int err = SSL_get_error(this->ssl, byte);
            switch (err)
            {
                case SSL_ERROR_NONE: {
                    continue;
                }

                case SSL_ERROR_ZERO_RETURN:
                {
                    throw ServerError("You have been disconected from the server" , "Server error");
                }
                case SSL_ERROR_WANT_READ:
                {
                    err = timeout(5);
                    if (err > 0)
                        continue;

                    if (err == 0) {
                        throw ServerError("Cannot read from socket", "Timeout");
                    } else {
                        throw ServerError("Cannot read from socket." , "ServerError");
                    }
                }
                case SSL_ERROR_WANT_WRITE:
                {
                    err = timeout(5);
                    if (err > 0)
                        continue;
                    if (err == 0) {
                        throw ServerError("Cannot read from socket", "Timeout");
                    } else {
                        throw ServerError("Cannot read from socket." , "ServerError");
                    }

                }

                default:
                {
                    throw ClientError("Imposible happend", "");
                }

            }

        }
    }
    else
    {
        byte = timeout(5);

        if (byte == 0) {
            throw ServerError("Cannot read from socket", "Timeout");
        } else if (byte < 0){
            throw ServerError("Cannot read from socket." , "ServerError");
        }
        byte = static_cast<int>(read(this->sockfd, buff, size));

    }
    return byte;
}

/**
 * Destructor.
 */
Connection::~Connection() {
    if (this->paramS)
    {
        this->shutSSL();
        this->destSSL();
    }
    else
        close(this->sockfd);
}




