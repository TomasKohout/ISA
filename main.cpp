#include "ParseParameters.h"
#include <string>
#include <iostream>
#include "Connection.h"
#include "Error.h"

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
            connect.establishConnection();
            cout << connect.recvLine() << endl;
        }
        catch (Error &error)
        {
            cerr << error.what() << endl;
        }

    }



    return 0;
}