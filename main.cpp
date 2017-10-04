#ifndef MAIN
#define MAIN

#include "ParseParameters.h"
#include <string>
#include <iostream>
#include "Connection.h"
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

    if (params.paramT)
    {

    }
    else
    {
        Connection connect = Connection(params);

        try {
            int a;

            connect.authenticate();
            if (connect.downloadMessages(a))
            {
                cout << "Bylo staženo " + to_string(a) + " zpráv!" << endl;
            }
            else
                return 1;

        }
        catch (const Error &error)
        {
            cerr << error.what() << endl;
        }


    }



    return 0;
}

#endif