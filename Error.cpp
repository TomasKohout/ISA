//
// Created by tom on 30.9.17.
//

#include "Error.h"

using namespace std;

/**
 * Creates error message.
 * @param what happend
 * @param why did it happen.
 */
Error::Error(string what , string why) {
    msg = what + " : "+ why;
}
/**
 * Overriden exception::what() method.
 * @return
 */
const char *Error::what() const throw(){
    return msg.c_str();
}

Error::~Error() throw(){

}


