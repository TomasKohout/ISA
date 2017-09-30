//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_FILEMANIPULATOR_H
#define ISA_MASTER_FILEMANIPULATOR_H

#include <string>

#define FI 0
#define FO 1
#define NOTH 2

using namespace std;

class FileManipulator {
public:
    FileManipulator();
    int fileOrFolder(string path);
};


#endif //ISA_MASTER_FILEMANIPULATOR_H
