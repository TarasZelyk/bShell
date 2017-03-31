#include "interpreter.h"

interpreter::interpreter()
{
    update_commands();
    boost::filesystem::path full_path( boost::filesystem::current_path());
    interpreter::curPath = full_path.generic_string();
}

interpreter::~interpreter()
{
    //dtor
}

std::string interpreter::getCurrentPath(){
    return interpreter::curPath;
}

std::string interpreter::cd(std::string newPath){
    std::string newFullPath = getCurrentPath() + newPath;
    DIR* dir = opendir(newFullPath.c_str());
    if(dir){
        boost::filesystem::path newAbsPath = boost::filesystem::canonical(newPath, getCurrentPath());
        std::string absPath = newAbsPath.generic_string();
        std::cout << newPath << std::endl << absPath << std::endl;
        interpreter::curPath = absPath;
    }
    return getCurrentPath();
}

void interpreter::update_commands(){

    interpreter::available_commands.push_back("aa");
}
