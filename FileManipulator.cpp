//
// Created by tom on 30.9.17.
//

#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "FileManipulator.h"

int FileManipulator::fileOrFolder(string path) {
    struct stat s;

    if( stat(path.c_str(),&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            return FO;
        }
        else if( s.st_mode & S_IFREG )
        {
            return FI;
        } else
            return NOTH;

    } else
        return NOTH;
}

FileManipulator::FileManipulator() {

}

void FileManipulator::openFileRead(string path) {
    readFile.open(path, ios::out);
}

std::vector<string> FileManipulator::readAuthFile(string path) {
    ifstream authFile (path);
    string line, word;
    vector<string> arr(2);
    arr.clear();
    int count = 0;
    if (authFile.is_open()) {
        while (authFile >> word) {
            switch (count) {
                case 2:
                    arr.push_back(word);
                    break;
                case 5:
                    arr.push_back(word);
                    break;
                default:
                    break;
            }
            count++;
        }
    } else
        throw FileIsNotOpenError("authfile", "Bad thing had just happend!");
    if (count > 6 || count < 6)
    {
        authFile.close();
        throw BadFileFormatError("authfile", "AuthFile has a bad format");
    }
    authFile.close();
    return arr;
}
