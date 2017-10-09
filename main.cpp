#ifndef MAIN
#define MAIN

#include "ParseParameters.h"
#include "Connection.h"
#include "TLSConnection.h"

using namespace std;
int main(int argc, char *argv[]) {
    int a;
    ParseParameters params = ParseParameters();
    try {
        params.parse(argc, argv);

        if (params.paramT)
        {
            TLSConnection tlsConnection = TLSConnection(params);
            tlsConnection.authenticate();
            if (!params.paramD)
            {
                tlsConnection.downloadMessages(a);

                if(params.paramN)
                    cout << "Bylo staženo " + to_string(a) + " nových zpráv!" << endl;
                else
                    cout << "Bylo staženo " + to_string(a) + " zpráv!" << endl;
            }
            else {
                tlsConnection.deleteMessages(a);
                cout << to_string(a) + " starých zpráv bylo smazáno!" << endl;
            }

            tlsConnection.cleanUp();
        }
        else
        {
            Connection connect = Connection(params);

            connect.authenticate();
            if (!params.paramD)
            {
                connect.downloadMessages(a);
                if (params.paramN)
                    cout << "Bylo staženo " + to_string(a) + " nových zpráv!" << endl;
                else
                    cout << "Bylo staženo " + to_string(a) + " zpráv!" << endl;
            }
            else if (connect.deleteMessages(a))
                cout << to_string(a) + " starých zpráv bylo smazáno!" << endl;

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