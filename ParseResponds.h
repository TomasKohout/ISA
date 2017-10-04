//
// Created by tom on 3.10.17.
//

#ifndef ISA_MASTER_PARSERESPONDS_H
#define ISA_MASTER_PARSERESPONDS_H

#include <string>
#include <vector>
#include "structures.h"

using namespace std;
class ParseResponds {




public:
    ParseResponds();
    bool respond(string msg);
    vector<msgData> parseMultiline(string multLine);
};


#endif //ISA_MASTER_PARSERESPONDS_H
