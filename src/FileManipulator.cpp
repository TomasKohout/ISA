//
// Created by tom on 30.9.17.
//

#include "FileManipulator.h"
/**
 * Checks if param path is really a path.
 * @var FI 0
 * @var FO 1
 * @var NOTH 2
 * @param path
 * @return
 */
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

/**
 * Read from auth file and gets user name and password.
 * @param path Path to an auth file.
 * @return vector with USER and PASS data
 */
std::vector<string> FileManipulator::readAuthFile(string path) {
    ifstream authFile (path);
    string line, word;
    vector<string> arr(2);
    arr.clear();
    int count = 0;
    if (authFile.is_open()) {
        while (authFile >> word) {
            switch (count) {
                case 2: //second word is always user name
                    arr.push_back(word);
                    break;
                case 5: //fifth word is always password
                    arr.push_back(word);
                    break;
                default:
                    break;
            }
            count++;
        }
    } else
        throw FileIsNotOpenError("authfile", "Bad thing had just happend!");
    if (count !=  6) //count have to be equal to 6
    {
        authFile.close();
        throw BadFileFormatError("authfile", "AuthFile has a bad format");
    }
    authFile.close();
    return arr;
}
