//
// Created by tom on 30.9.17.
//

#include <sys/stat.h>
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
