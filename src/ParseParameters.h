//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_PARSEPARAMETERS_H
#define ISA_MASTER_PARSEPARAMETERS_H

#include <string>
#include <cstring>
#include <unistd.h>
#include "FileManipulator.h"
#include "ConnectionInterface.h"

#define ERR 42
#define OK 0

#define FI 0
#define FO 1
#define NOTH 2

using namespace std;

class ParseParameters{
public:
    string address = "";
    string portNum = "110";
    string paramFileC = ""; //certfile
    string paramDirC = "";  //dir with cerfiles
    string USER;            //user name
    string PASS;            //user password
    string paramA = "";     //auth file
    string paramO = "";     //out dir

    bool help = false;
    bool paramT = false;    //pop3s
    bool paramS = false;    //STLS
    bool paramD = false;    //delete messages
    bool paramN = false;    //work with new messages

    FileManipulator manipulate;

public:
    ParseParameters();

    void parse(int argc, char *argv[]);

    class BadArgumentError;
    class NotFileOrDirError;
    class BadPortError;
    class TooManyArgsError;

private:
    void setAddress(char *argv);
    void setPortNum(char *argv);
    void setParamT();
    void setParamS();
    void setParamFileC(char *argv);
    void setParamDirC(char *argv);
    void setParamD();
    void setParamN();
    void setParamO(char *argv);
    void getUserAndPass(char *argv);
    void areMandatoryArgsSeted();
    bool isNumeric(string str);
};

class ParseParameters::BadArgumentError : public Error{
public:
    BadArgumentError(string const &wha, string const &wh)  {
        msg = wha + " : " + wh;
    }
};

class ParseParameters::NotFileOrDirError : public Error{
public:
    NotFileOrDirError(string const &wha, string const &wh) {
        msg = wha + " : " + wh;
    }
};

class ParseParameters::BadPortError : public Error{
public:
    BadPortError(string const &wha, string const &wh) {
        msg = wha + " : " + wh;
    }
};

class ParseParameters::TooManyArgsError : public Error{
public:
    TooManyArgsError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;
    }
};


#endif //ISA_MASTER_PARSEPARAMETERS_H
