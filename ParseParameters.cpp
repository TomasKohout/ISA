//
// Created by tom on 30.9.17.
//
#include <cstring>
#include <netdb.h>
#include <locale>
#include <sys/stat.h>
#include "ParseParameters.h"

ParseParameters::ParseParameters() {
    this->manipulate = FileManipulator();
    this->connection = Connection();
}

void ParseParameters::parse(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        if (argv[i] == "-p")
        {
            portNum = string(argv[++i]);
            if (!isNumeric(portNum))
                throw "Bad port!";
        }
        else if (argv[i] == "-T")
        {
            paramT = true;
            portNum = "995";
            if (paramS)
                throw "Param -S is already seted!";
        }
        else if (argv[i] == "-S")
        {
            paramS = true;
        }
        else if (argv[i] == "-c" &&
                 (paramT || paramS) &&
                 (this->manipulate.fileOrFolder(string(argv[i+1]))) == FI)
            paramFileC = string(argv[++i]);
        else if (argv[i] == "-C" &&
                 (paramT || paramS) &&
                 (this->manipulate.fileOrFolder(string(argv[i+1]))) == FO)
            paramDirC = string(argv[++i]);
        else if (argv[i] == "-d")
            paramD = true;
        else if (argv[i] == "-n")
            paramN = true;
        else if (strcmp(argv[i] ,"-a") == 0){
            paramA = string(argv[++i]);
            if (this->manipulate.fileOrFolder(paramA) != FI)
                throw "-a is not a file!";
        }
        else if (strcmp(argv[i], "-o") == 0){
            paramO = string(argv[++i]);
            if (this->manipulate.fileOrFolder(paramO) != FO){
                throw "-o is not a folder!";
        }
        else{
            if (address.empty())
            {
                if (connection.hostToIp(string(argv[i])) == 0)
                    address = string(argv[i]);

                continue;
            }
            throw "Bad arguments. Try again";
        }
    }

    ParseParameters::areMandatoryArgsSeted();
    }
}


void ParseParameters::areMandatoryArgsSeted() {
    if (address.empty() || paramA.empty() || paramO.empty())
        throw "One or many of the mandatory parameters are missing";
}

bool ParseParameters::isNumeric(string str) {
    for (string::size_type i = 0; i < portNum.length(); i++) {
        if (!isdigit(portNum[i]))
            return false;
    }
    return true;
}



