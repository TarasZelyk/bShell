#include "interpreter.h"

interpreter::interpreter()
{
    boost::filesystem::path full_path( boost::filesystem::current_path());
    interpreter::curPath = full_path.generic_string();
    update_commands("bin");
}

interpreter::~interpreter()
{
    //dtor
}

std::string interpreter::getCurrentPath(){
    return interpreter::curPath;
}

std::string interpreter::cd(std::string newPath){
    std::string newFullPath = getCurrentPath() + "/" + newPath;

    DIR* dir = opendir(newFullPath.c_str());
    if(dir){
        boost::filesystem::path newAbsPath = boost::filesystem::canonical(newPath, getCurrentPath());
        std::string absPath = newAbsPath.generic_string();
        //std::cout << newPath << std::endl << absPath << std::endl;
        interpreter::curPath = absPath;
    }
    //std::cout << newFullPath << std::endl;
    return getCurrentPath();
}

std::vector<std::string>& interpreter::getAvailable_commands(){
    return interpreter::available_commands;
}

void interpreter::update_commands(const std::string& path){
    interpreter::available_commands.clear();
    if (!path.empty())
    {
        namespace fs = boost::filesystem;

        fs::path apk_path(path);
        fs::recursive_directory_iterator end;

        for (fs::recursive_directory_iterator i(apk_path); i != end; ++i)
        {
            const fs::path cp = (*i);
            interpreter::available_commands.push_back(cp.string());
        }
    }
}

void interpreter::process(std::string command, std::string args[]){
    if(isCommand(command)){
        start_process(command, args);
    } else {
    }
}


bool interpreter::isCommand(std::string input){
    return true;
}

void interpreter::start_process(std::string command, std::string args[]){
    pid_t processID = fork();
    if(processID == -1)
    {
        // handle the error here
    }
    else if(processID == 0)
    {
        std::string path = "bin/" + command;
        execl("bin/test", "bin/test", (char *)NULL);
        char errstr[] = "Error calling execl, exiting the child.\n";
        write(STDERR_FILENO, errstr, sizeof(errstr));
        _exit(1);
    }
    else
    {
        // this code only runs in the parent process, and processID
        // contains the child's process identifier
    }
}
