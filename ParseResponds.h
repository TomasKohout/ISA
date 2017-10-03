//
// Created by tom on 3.10.17.
//

#ifndef ISA_MASTER_PARSERESPONDS_H
#define ISA_MASTER_PARSERESPONDS_H

#include <string>
#include <vector>

using namespace std;
class ParseResponds {

    bool respond(string msg);


public:
    ParseResponds();

    vector<string> parseMultiline(string multLine);
};


#endif //ISA_MASTER_PARSERESPONDS_H
