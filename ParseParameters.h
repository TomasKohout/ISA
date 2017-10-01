//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_PARSEPARAMETERS_H
#define ISA_MASTER_PARSEPARAMETERS_H

#include <string>
#include <cstring>
#include <unistd.h>
#include "FileManipulator.h"
#include "Connection.h"
#include "Error.h"

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
    bool paramT = false;    //pop3s
    bool paramS = false;    //STLS
    string paramFileC = ""; //certfile
    string paramDirC = "";  //dir with cerfiles
    //if not SSL_CTX_set_default_verify_paths()
    bool paramD = false;    //delete messages
    bool paramN = false;    //work with new messages
    string USER;            //user name
    string PASS;            //user password
    string paramA = "";     //auth file
    string paramO = "";     //out dir
    FileManipulator manipulate;

public:
    ParseParameters();
    void parse(int argc, char *argv[]);

    class BadArgumentError;
    class NotFileOrDirError;
    class MandatoryArgsError;
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

class ParseParameters::MandatoryArgsError : public Error{
public:
    MandatoryArgsError(string const &wha, string const &wh) {
        msg = wha +  wh;
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
