#ifndef MAIN
#define MAIN

#include "ParseParameters.h"
#include "Connection.h"
#include "TLSConnection.h"

using namespace std;
void printHelp(){
    cout << "Use this application for downloading emails. Popcl uses pop3 and pop3s protocol. Please be aware of it." << endl;
    cout << "Use:" << endl;
    cout << "popcl [-h|--help] <server> [-p <port>] [-T|-S [-c <certfile>] [-C <certaddr>]] [-d] [-n] -a <auth_file> -o <out_dir>" << endl;
    cout << endl;
    cout << "-h, --help       : prints this help" << endl;
    cout << endl;
    cout << "<server>         : server to connect to, hostname, IPv4 or IPv6. This is mandatory parameter!" << endl;
    cout << endl;
    cout << "[-p <port>]      : port is number from 1 to 65535. Check your email server for more info. Default value for -S and without -S or -T is port 110. -T default port is 995. This is not mandatory parameter!" << endl;
    cout << endl;
    cout << "[-T|-S] : start tls connection. This is not mandatory parameter!" << endl;
    cout << endl;
    cout << "[-c <certfile>]  : use this if you want load specific certificate. certfile is certificate in a pem format. This is not mandatory parameter!" << endl;
    cout << endl;
    cout << "[-C <certdir>]   : use this if you want load certificates from specific folder. certdir is directory with certificates in a pem format. Hash this folder firs! More info in openssl manual. This is not mandatory parameter!" << endl;
    cout << endl;
    cout << "[-d] : use this parameter if you want delete all messages that are you have downloaded before. This is not mandatory parameter!" << endl;
    cout << endl;
    cout << "[-n] : use this parameter for download only new messages. This is not mandatory parameter!" << endl;
    cout << endl;
    cout << "[-a <auth_file>] : <auth_file> is file in this format:"  << endl;
    cout << "username = user" << endl;
    cout << "password = pass" << endl;
    cout << "any other format is not supported! This is mandatory parameter!" << endl;
    cout << endl;
    cout << "[-o <out_dir>]   : out_dir specifies directory where messages will be stored. This is mandatory parameter!" << endl;
    cout << endl;
    cout << "if you use -d and -n together, popcl will firstly download new messages and after that it will delete downloaded messages." << endl;
}

int main(int argc, char *argv[]) {
    ParseParameters params = ParseParameters();
    try {
        params.parse(argc, argv);
    }
    catch (Error& error)
    {
        printHelp();
        return 0;
    }

    if (params.help)
    {
        printHelp();
        return 0;
    }

    try {


        if (params.paramT)
        {
            TLSConnection tlsConnection = TLSConnection(params);
            tlsConnection.authenticate();
            if (!params.paramD)
            {

                if(params.paramN)
                    cout << to_string(tlsConnection.downloadMessages()) + " new messages was downloaded!" << endl;
                else
                    cout << to_string(tlsConnection.downloadMessages()) + " messages was downloaded!" << endl;
            }
            else if (params.paramD && params.paramN)
            {
                cout << to_string(tlsConnection.downloadMessages()) + " new messages was downloaded and " + to_string(
                        tlsConnection.deleteMessages()) + " was deleted" << endl;
            }
            else if (params.paramD){
                cout << to_string(tlsConnection.deleteMessages()) + " old messages deleted!" << endl;
            }

            tlsConnection.cleanUp();
        }
        else
        {
            Connection connect = Connection(params);

            connect.authenticate();
            if (!params.paramD)
            {
                if(params.paramN)
                    cout << to_string(connect.downloadMessages()) + " new messages was downloaded!" << endl;
                else
                    cout << to_string(connect.downloadMessages()) + " messages was downloaded!" << endl;
            }
            else if (params.paramD && params.paramN)
            {
                cout << to_string(connect.downloadMessages()) + " new messages was downloaded and " + to_string(
                        connect.deleteMessages()) + " was deleted" << endl;
            }
            else if (params.paramD){
                cout << to_string(connect.deleteMessages()) + " old messages deleted!" << endl;
            }

            connect.cleanUp();
        }
    }
    catch (Error& e )
    {
        cerr << e.what()<<endl;
        exit(ERR);
    }
    return 0;
}

#endif