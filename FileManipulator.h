//
// Created by tom on 30.9.17.
//

#ifndef ISA_MASTER_FILEMANIPULATOR_H
#define ISA_MASTER_FILEMANIPULATOR_H

#include <string>
#include <fstream>
#include <vector>
#include "Error.h"

#define FI 0
#define FO 1
#define NOTH 2

using namespace std;

class FileManipulator {
private:
    ofstream readFile;
    ofstream writeFile;
    string pathToDir;

public:
    FileManipulator();
    int fileOrFolder(string path);
    void openFileRead(string path);
    void openFileWrite(string path);
    string readOneLine();
    void writeToFile();
    std::vector<string> readAuthFile(string path);

    class BadFileFormatError;
    class FileIsNotOpenError;
};

class FileManipulator::BadFileFormatError : public Error{
public:
    BadFileFormatError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};

class FileManipulator::FileIsNotOpenError : public Error {
public:
    FileIsNotOpenError(string const &wha, string const &wh)
    {
        msg = wha + " : " + wh;

    }
};


#endif //ISA_MASTER_FILEMANIPULATOR_H
