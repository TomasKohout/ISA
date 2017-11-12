//
// Created by tom on 9.10.17.
//
#include "TLSConnection.h"
/**
 * Constructor. Set up atributs.
 * @param parse ParseParameters object
 */
TLSConnection::TLSConnection(const ParseParameters &parse) {
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
 * For more information go to ConnectionInterface::shutSSL() and
 * ConnectionInterface::destSSL().
 */
TLSConnection::~TLSConnection() {
    this->shutSSL();
    this->destSSL();
}

/**
 * This method is used for calling initSSL().
 * For more information go to ConnectionInterface::initSSL().
 */
void TLSConnection::establishConnection() {
    this->initSSL();
    validResponse();
}

/**
 * This is implemented virtual method from ConnectionInterface.
 * It is used for reading data from socket usualy by one byte.
 * @param buff array of chars
 * @param size size of buff
 * @return count of read bytes
 */
ssize_t TLSConnection::readSock(char *buff, int size) {
    int byte;

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
    return byte;
}

/**
 * This is implemented virtual method from ConnectionInterface.
 * It is used for sending data.
 *
 * @param cmd std::string without \r\n
 */
void TLSConnection::sendCommand(string cmd) {
    cmd += "\r\n";
    if (SSL_write(this->ssl, cmd.c_str(), static_cast<int>(cmd.size())) < 0)
        throw ServerError("Can not write into socket", "Server closed the connection");
}


