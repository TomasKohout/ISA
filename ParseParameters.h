//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_PARSEPARAMETERS_H
#define ISA_MASTER_PARSEPARAMETERS_H

#include <string>
#include <unistd.h>
#include "FileManipulator.h"
#include "Connection.h"

#define ERR 42
#define OK 0

#define FI 0
#define FO 1
#define NOTH 2

using namespace std;

class ParseParameters{
private:
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
    Connection connection;

public:
    ParseParameters();
    void parse(int argc, char *argv[]);


private:
    void areMandatoryArgsSeted();
    bool isNumeric(string str);
};

#endif //ISA_MASTER_PARSEPARAMETERS_H
