//
// Created by tom on 30.9.17.
//
#include <cstring>
#include <netdb.h>
#include <locale>
#include <sys/stat.h>
#include "ParseParameters.h"
#include <string>
#include <iostream>

using namespace std;

ParseParameters::ParseParameters() {
    this->manipulate = FileManipulator();
    this->connection = Connection();
}

void ParseParameters::parse(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i] , "-p") == 0)
            setPortNum(argv[i]);
        else if (strcmp(argv[i] ,"-T") == 0)
            setParamT();
        else if (strcmp(argv[i] ,"-S") == 0)
            setParamS();
        else if (strcmp(argv[i] ,"-c" ) == 0 && (paramT || paramS))
            setParamFileC(argv[++i]);
        else if (strcmp(argv[i] ,"-C") == 0 && (paramT || paramS))
            setParamDirC(argv[++i]);
        else if (strcmp(argv[i] , "-d") == 0)
            setParamD();
        else if (strcmp(argv[i] ,"-n") == 0)
            setParamN();
        else if (strcmp(argv[i] ,"-a") == 0)
            setParamA(argv[++i]);
        else if (strcmp(argv[i], "-o") == 0)
            setParamO(argv[++i]);
        else{
            if (address.empty()) {
                setAddress(argv[i]);
                continue;
            }
            throw BadArgumentError("Bad arguments", "Not allowed argument or badly tipped one.");
        }
    }
    areMandatoryArgsSeted();
}


void ParseParameters::areMandatoryArgsSeted() {
    if (address.empty() || paramA.empty() || paramO.empty())
        throw MandatoryArgsError("One or many of the mandatory parameters are missing", "");
}

bool ParseParameters::isNumeric(string str) {
    for (string::size_type i = 0; i < portNum.length(); i++) {
        if (!isdigit(portNum[i]))
            return false;
    }
    return true;
}

void ParseParameters::setAddress(char *argv) {
        if (connection.hostToIp(string(argv)) == 0)
            address = string(argv);
        else
            throw BadArgumentError(string(argv), "This host can not be reached");


}

void ParseParameters::setPortNum(char *argv) {
    portNum = string(argv);
    if (!isNumeric(portNum))
        throw BadPortError(portNum,"Try again");
}

void ParseParameters::setParamT() {
    paramT = true;
    portNum = "995";
    if (paramS)
        throw TooManyArgsError("-T", "-S is already setted");
}

void ParseParameters::setParamS() {
    paramS = true;
    if (paramT)
        throw TooManyArgsError("-S", "-T is already setted");
}

void ParseParameters::setParamFileC(char *argv) {
    if(manipulate.fileOrFolder(string(argv)) == FI)
        paramFileC = string(argv);
    else
        throw NotFileOrDirError("-c", "not a file");
}

void ParseParameters::setParamDirC(char *argv) {
    if (manipulate.fileOrFolder(string(argv)) == FO)
        paramDirC = string(argv);
    else
        throw NotFileOrDirError("-C", "not a directory");
}

void ParseParameters::setParamD() {
    paramD = true;
}

void ParseParameters::setParamN() {
    paramN = true;
}

void ParseParameters::setParamA(char *argv) {
    if (manipulate.fileOrFolder(paramA) == FI)
        paramA = string(argv);
    else
        throw NotFileOrDirError("-a", "not a file");
}

void ParseParameters::setParamO(char *argv) {
    if (manipulate.fileOrFolder(paramO) == FO)
        paramO = string(argv);
    else
        throw NotFileOrDirError("-o", "not a directory");
}



