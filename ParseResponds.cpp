//
// Created by tom on 3.10.17.
//

#include <sstream>
#include "ParseResponds.h"

ParseResponds::ParseResponds() {

}

bool ParseResponds::respond(string msg) {
    return msg[0] == '+';
}

vector<string> ParseResponds::parseMultiline(string multLine) {
    vector<string> array;
    size_t count = multLine.find_first_of('\n');

    for (int i = 0; i <= count  ; i++) {
        multLine.erase(0, 1);
    }

    istringstream stream(multLine);
    string line;
    int i = 1;
    for (string word; stream >> word ; i++) {
        if (i % 2 == 0 && word.compare(".") != 0)
            array.push_back(word);
    }

    return array;
}



