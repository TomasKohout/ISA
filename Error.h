//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_ERROR_H
#define ISA_MASTER_ERROR_H

#include <string>
#include <stdexcept>

using namespace std;

class Error : public exception {
protected:
    string msg;

public:
    explicit Error(string what = "", string why = "");

    virtual ~Error() throw();

    const char * what() const throw() ;
};


#endif //ISA_MASTER_ERROR_H
