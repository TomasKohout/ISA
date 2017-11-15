#ifndef MAIN
#define MAIN

#include "ParseParameters.h"
#include "Connection.h"
#include "TLSConnection.h"

using namespace std;
void printHelp(){
    cout << "Tato aplikace slouží ke stažení emailů pomocí POP3 protokolu." << endl;
    cout << "Použítí:" << endl;
    cout << "popcl [-h|--help] <server> [-p <port>] [-T|-S [-c <certfile>] [-C <certaddr>]] [-d] [-n] -a <auth_file> -o <out_dir>" << endl;
    cout << endl;
    cout << "-h, --help       : Vypíše nápovědu." << endl;
    cout << endl;
    cout << "<server>         : IPv4 nebo IPv6 adresa, nebo doménové jméno. Povinný argument!" << endl;
    cout << endl;
    cout << "[-p <port>]      : mění výchozí nastavení portu. port je číslo od 1 - 65535" << endl;
    cout << endl;
    cout << "[-T|-S] : Zapne šifrování komunikace." << endl;
    cout << endl;
    cout << "[-c <certfile>]  : Pouze s -S nebo -T. Cesta k souboru, který obsahuje certifikát s koncovkou .pem." << endl;
    cout << endl;
    cout << "[-C <certaddr>]  : Pouze s -S nebo -T. Cesta k adresáři obsahující adresář certikátů.  : " << endl;
    cout << endl;
    cout << "[-d] : smaže již stažené zprávy." << endl;
    cout << endl;
    cout << "[-n] : stáhne pouze nové zprávy, tedy ty které ještě nebyly staženy." << endl;
    cout << endl;
    cout << "[-a <auth_file>] : <auth_file> je soubor ve formátu:"  << endl;
    cout << "user = username" << endl;
    cout << "pass = password" << endl;
    cout << "jiný formát není podporovaný. Mezery jsou povinné. Povinný argument" << endl;
    cout << endl;
    cout << "[-o <out_dir>]   : adresář pro ukládání zpráv. Povinný argument." << endl;
    cout << endl;
    cout << "[-n -d]          : stáhne nové zprávy a následně smaže již stažené zprávy." << endl;
}

int main(int argc, char *argv[]) {
    int download, deleted = 0;
    ParseParameters params = ParseParameters();
    try {
        params.parse(argc, argv);
    }
    catch (Error& error)
    {
        cerr << error.what() << endl;
        cerr << "Pro nápovědu spusťte program s argumentem -h nebo --help" << endl;
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
                download = tlsConnection.downloadMessages();
            }
            else if (params.paramD && params.paramN)
            {
                download = tlsConnection.downloadMessages();
                deleted = tlsConnection.deleteMessages();
            }
            else if (params.paramD){
               deleted = tlsConnection.deleteMessages();
            }

            tlsConnection.cleanUp();
        }
        else
        {
            Connection connect = Connection(params);

            connect.authenticate();
            if (!params.paramD)
            {
                download = connect.downloadMessages();
            }
            else if (params.paramD && params.paramN)
            {
                download = connect.downloadMessages();
                deleted = connect.deleteMessages();
            }
            else if (params.paramD){
                deleted = connect.deleteMessages();
            }

            connect.cleanUp();
        }
    }
    catch (Error& e )
    {
        cerr << e.what()<<endl;
        exit(ERR);
    }

    if (!params.paramD)
        cout << to_string(download) + " zpráv bylo staženo!" << endl;
    else if (params.paramD && params.paramN)
        cout << to_string(download) + " zpráv bylo staženo a " + to_string(deleted) + " zpráv bylo smazáno!" << endl;
    else if (params.paramD)
        cout << to_string(deleted) + " zpráv bylo smazáno!"<< endl;
    return 0;
}

#endif