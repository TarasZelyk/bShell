#include "interpreter.h"

interpreter::interpreter()
{
    boost::filesystem::path full_path( boost::filesystem::current_path());
    curPath = full_path.generic_string();
    update_commands("bin");
}

interpreter::~interpreter()
{
    //dtor
}

std::string interpreter::getCurrentPath(){
    return interpreter::curPath;
}

void interpreter::pwd(std::vector<std::string> argv){
    for(int i = 1; i < argv.size(); i++){
        if(argv.at(i) == "-h" || argv.at(i) == "--help"){
            std::cout << "Use this command to get current working directory" << std::endl;
            return;
        }
    }
    std::cout << getCurrentPath() << std::endl;
}

void interpreter::cd(std::vector<std::string> argv){
    for(int i = 1; i < argv.size(); i++){
        if(argv.at(i) == "-h" || argv.at(i) == "--help"){
            std::cout << "Use this command to get change the working directory" << std::endl;
            return;
        }
    }

    if (argv.size() < 2){
        std::cout << "Use this command to get change the working directory" << std::endl;
        return;
    }

    std::string newFullPath = getCurrentPath() + "/" + argv.at(1);
    DIR* dir = opendir(newFullPath.c_str());
    if(dir){
        boost::filesystem::path newAbsPath = boost::filesystem::canonical(argv.at(1), getCurrentPath());
        std::string absPath = newAbsPath.generic_string();
        curPath = absPath;
    }
}

std::vector<std::string>& interpreter::getAvailable_commands(){
    return available_commands;
}

void interpreter::update_commands(const std::string& path){
    available_commands.clear();
    if (!path.empty())
    {
        namespace fs = boost::filesystem;

        fs::path apk_path(path);
        fs::recursive_directory_iterator end;

        available_commands.clear();

        for (fs::recursive_directory_iterator i(apk_path); i != end; ++i)
        {
            const fs::path cp = (*i);
            std::string command = cp.stem().string();
            if (command != ""){
                available_commands.push_back(command);
            }
        }
    }
}

void interpreter::help(std::vector<std::string> input){
    std::cout << "some useful help goes here" << std::endl;
}

void interpreter::stop(std::vector<std::string> argv){
    for(int i = 1; i < argv.size(); i++){
        if(argv.at(i) == "-h" || argv.at(i) == "--help"){
            std::cout << "Use this command to get stop working with the beerShell. You can specify the exit code with the following: exit <exit code>" << std::endl;
            return;
        }
    }
    if(argv.size() == 1){
        exit(0);
    } else {
        exit(atoi(argv.at(1).c_str()));
    }
}

int interpreter::executeBuiltIn(std::vector<std::string> args){
    std::string command = args.at(0);
    if (command == "exit"){
        stop(args);
        return 0;
    }

    if (command == "help"){
        help(args);
        return 0;
    }

    if (command == "pwd"){
        pwd(args);
        return 0;
    }

    if (command == "cd"){
        cd(args);
        return 0;
    }

    if (command == "upd"){
        update_commands("bin");
        std::vector<std::string> coms = getAvailable_commands();
        for(int i = 0; i < coms.size(); i++){
            std::cout << coms.at(i) << std::endl;
        }
        return 0;
    }
    return -1;
}

int interpreter::process(std::string command){
    std::vector<std::string> args;

    boost::regex splitArgs("(\"[^\"]+\"|[^\\s\"]+)");

    boost::sregex_token_iterator iter(command.begin(), command.end(), splitArgs, 0);
    boost::sregex_token_iterator end;

    for(; iter != end; ++iter ) {
            args.push_back(*iter);
    }


    int retCode;
    if((retCode = executeBuiltIn(args)) == -1){
        if(isCommand(args.at(0))){
            start_process(args);
        } else {
            std::cout << BAD_COMMAND << std::endl;
        }
    } else {
    }
    return 0;
}


bool interpreter::isCommand(std::string input){
    std::vector<std::string> commands = getAvailable_commands();
    for(int i = 0; i < commands.size(); i++){
        if(commands.at(i) == input){
            return true;
        }
    }
    return false;
}

void interpreter::start_process(std::vector<std::string> command){
    pid_t processID = fork();
    pid_t wpid;
    int status;
    if(processID == -1)
    {
        // handle the error here
    }
    else if(processID == 0)
    {
        const char* execArgs[command.size()];
        for(int i = 1; i < command.size(); i++){
            execArgs[i-1] = command.at(i).c_str();
        }
        std::string path = "bin/" + command.at(0);
        execl(path.c_str(), path.c_str(), execArgs, (char*) NULL);
        char errstr[] = "Command not recognized.\nUse help to get.\n";
        write(STDERR_FILENO, errstr, sizeof(errstr));
        _exit(1);
    }
    else
    {
        // this code only runs in the parent process, and processID
        // contains the child's process identifier
    }
    do {
          wpid = waitpid(processID, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}


