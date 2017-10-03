#include "ParseParameters.h"
#include <string>
#include <iostream>
#include "Connection.h"
#include "Error.h"
#include "ParseResponds.h"

using namespace std;
int main(int argc, char *argv[]) {
    ParseParameters params = ParseParameters();
    try {
        params.parse(argc, argv);
    }
    catch (Error& e ){
        cerr << e.what()<<endl;
        exit(ERR);
    }

    if (params.paramS)
    {
        Connection connect = Connection(params);


    }
    else if (params.paramT)
    {

    }
    else
    {
        Connection connect = Connection(params);

        try {
            int a;
            connect.establishConnection();
            cout << connect.recvLine(a) << endl;
            connect.sendCommand("USER "+ params.USER );
            cout << connect.recvLine(a) << endl;
            connect.sendCommand("PASS " + params.PASS );
            cout << connect.recvLine(a) << endl;
            connect.sendCommand("RETR 1");
            ParseResponds parse = ParseResponds();
            //parse.parseMultiline(connect.recvMultLine());
            cout << connect.recvMessage() << endl;
            connect.sendCommand("STAT");
            cout << connect.recvLine(a) << endl;


        }
        catch (Error &error)
        {
            cerr << error.what() << endl;
        }

    }



    return 0;
}