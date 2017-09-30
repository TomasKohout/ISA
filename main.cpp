#include "ParseParameters.h"
#include <string>
#include <iostream>

using namespace std;
int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;

    ParseParameters params = ParseParameters();
    try {
        params.parse(argc, argv);
    }
    catch (...){
        
    }



    //parseAuthFile(args->paramFileC);
    return 0;
}