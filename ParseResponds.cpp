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

vector<msgData> ParseResponds::parseMultiline(string multLine) {
    vector<msgData> array;
    size_t count = multLine.find_first_of('\n');

    for (int i = 0; i <= count  ; i++) {
        multLine.erase(0, 1);
    }

    istringstream stream(multLine);
    string uidl;
    string id;
    int i = 0;
    for (string word; stream >> id, stream >> uidl ; i++) {
        if (id.compare(".") != 0 || uidl.compare(".") != 0)
        {
            array.push_back(msgData());
            array[i].id = id;
            array[i].uidl = uidl;
        }
    }

    return array;
}



