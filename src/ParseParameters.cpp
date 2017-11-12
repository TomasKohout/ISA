//
// Created by tom on 30.9.17.
//
#include "ParseParameters.h"

using namespace std;

bool port = false;
/**
 *
 * Constructor
 * It only creates FileManipulator object
 */
ParseParameters::ParseParameters() {
    this->manipulate = FileManipulator();
}
/**
 * This method is used for processing program arguments.
 * @param argc Count of arguments
 * @param argv String array with options
 */
void ParseParameters::parse(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i] , "-p") == 0)
            setPortNum(argv[++i]);
        else if (strcmp(argv[i] ,"-T") == 0)
            setParamT();
        else if (strcmp(argv[i] ,"-S") == 0)
            setParamS();
        else if (strcmp(argv[i] ,"-c" ) == 0)
            setParamFileC(argv[++i]);
        else if (strcmp(argv[i] ,"-C") == 0)
            setParamDirC(argv[++i]);
        else if (strcmp(argv[i] , "-d") == 0)
            setParamD();
        else if (strcmp(argv[i] ,"-n") == 0)
            setParamN();
        else if (strcmp(argv[i] ,"-a") == 0)
            getUserAndPass(argv[++i]);
        else if (strcmp(argv[i], "-o") == 0)
            setParamO(argv[++i]);
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            help = true;
            return;
        }
        else{
            if (address.empty()) {
                setAddress(argv[i]);
                continue;
            }
            throw BadArgumentError("Bad arguments", "Not allowed argument or badly tipped one.");
        }
    }
    areMandatoryArgsSeted();
    if (paramO.back() == '/')
        mkdir((paramO + ".cache").c_str(), 0777);
    else
        mkdir((paramO + "/.cache").c_str(), 0777);


}

/**
 * Method that checks if all necessary arguments are setted.
 */
void ParseParameters::areMandatoryArgsSeted() {
    if (address.empty() || paramA.empty() || paramO.empty() || (paramT && paramS))
        throw BadArgumentError("Bad arguments", "Not allowed argument or badly tipped one.");
}

/**
 * Method that checks if str contains only numebers.
 * @param str String
 * @return true or false
 */
bool ParseParameters::isNumeric(string str) {
    for (string::size_type i = 0; i < str.length(); i++) {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}
/**
 * This method is setter for this->address
 * @param argv string
 */
void ParseParameters::setAddress(char *argv) {
        if (string(argv).find('.') == string::npos && string(argv).find(':') == string::npos)
            throw BadArgumentError("Bad argumet", string(argv));

        address = string(argv);

}
/**
 * This method is setter for this->portNum
 * @param argv string
 */
void ParseParameters::setPortNum(char *argv) {
    portNum = string(argv);
    port = true;
    if (!isNumeric(portNum))
        throw BadPortError(portNum,"Try again. Port must be numeric.");
}

/**
 * This method sets paramT and validate that paramS is not set.
 * Also set this->portNum with value of 995.
 * If paramS has been set, exception will be thrown.
 */
void ParseParameters::setParamT() {
    paramT = true;
    if (!port)
        portNum = "995";

    if (paramS)
        throw TooManyArgsError("-T", "-S has been already set");
}
/**
 * This method sets paramS and validate that paramT is not set.
 * If paramT has been set, exception will be thrown.
 */
void ParseParameters::setParamS() {
    paramS = true;
    if (!port)
        portNum = "110";
    if (paramT)
        throw TooManyArgsError("-S", "-T has been already set");
}
/**
 * Checks if argv is path to a file and sets this->paramFileC.
 * If argv is not the file, exception will be thrown.
 * @param argv string
 */
void ParseParameters::setParamFileC(char *argv) {
    if(manipulate.fileOrFolder(string(argv)) == FI)
        paramFileC = string(argv);
    else
        throw NotFileOrDirError("-c", "not a file");
}

/**
 * Checks if argv is path to a directory and sets this->paramDirC.
 * If it is not a dir, exception will be thrown.
 * @param argv string
 */
void ParseParameters::setParamDirC(char *argv) {
    if (manipulate.fileOrFolder(string(argv)) == FO)
        paramDirC = string(argv);
    else
        throw NotFileOrDirError("-C", "not a directory");
}
/**
 * Sets this->paramN.
 * If this->paramD has been already set, exception will be thrown.
 */
void ParseParameters::setParamD() {
    paramD = true;
}
/**
 * Sets this->paramd.
 * If this->paramN has been already set, exception will be thrown.
 */
void ParseParameters::setParamN() {
    paramN = true;
}
/**
 * Checks if argv is path to a directory and sets this->paramDirC.
 * If argv is not the directory, exception will be thrown.
 * @param argv string
 */
void ParseParameters::setParamO(char *argv) {
    if (manipulate.fileOrFolder(string(argv)) == FO)
    {
        paramO = string(argv);
        if (paramO.back() != '/')
            paramO += "/";
    }
    else
        throw NotFileOrDirError("-o", "not a directory");
}
/**
 * Checks if argv is path to a file.
 * If argv is path to a file FileManipulator::readAuthFile(string path)
 * will be called. After that this->USER and this->PASS is set.
 * @param argv
 */
void ParseParameters::getUserAndPass(char *argv) {
    vector<string> vec;

    if (manipulate.fileOrFolder(string(argv)) == FI)
        paramA = string(argv);
    else
        throw NotFileOrDirError("-a", "not a file");

    vec = manipulate.readAuthFile(paramA);
    USER = vec[0];
    PASS = vec[1];
}




