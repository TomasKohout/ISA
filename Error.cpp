//
// Created by tom on 30.9.17.
//

#include "Error.h"
#include <utility>
#include <iostream>

using namespace std;
Error::Error(string what , string why) {
    msg = what + " : "+ why;
}

const char *Error::what() const throw(){
    return msg.c_str();
}

Error::~Error() throw(){

}


