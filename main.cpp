#include "ParseParameters.h"
#include <string>
#include <iostream>

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



    //parseAuthFile(args->paramFileC);
    return 0;
}