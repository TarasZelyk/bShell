/*
Developed by Sergey Dubovyk for the beerShell.
Ask dubovyk@ucu.edu.ua for all the questions and bugs.

Version 1.1.3
April 2, 2017
*/

#include <fcntl.h>
#include <regex>
#include <boost/algorithm/string/trim.hpp>
#include "interpreter.h"

interpreter::interpreter() {
    boost::filesystem::path full_path(boost::filesystem::current_path());
    curPath = full_path.generic_string();
    update_commands(BIN_PATH);
}

std::string interpreter::getCurrentPath() {
    return boost::filesystem::current_path().string();
}

void interpreter::pwd(std::vector<std::string> argv) {
    for (size_t i = 1; i < argv.size(); i++) {
        if (argv.at(i) == "-h" || argv.at(i) == "--help") {
            std::cout << "Use this command to get current working directory" << std::endl;
            return;
        }
    }
    std::cout << getCurrentPath() << std::endl;
}

void interpreter::echo(std::vector<std::string> argv) {
    for (size_t i = 1; i < argv.size(); i++) {
        std::cout << argv[i] << ((i != argv.size() - 1) ? " " : "");
    }
    std::cout << std::endl;
}

void interpreter::export_(std::vector<std::string> argv) {
    std::string name, value;
    size_t found = argv[1].find("=");
    if (found != std::string::npos) {
        name = argv[1].substr(0, found);
        value = argv[1].substr(found + 1, std::string::npos);
    } else {
        name = argv[1];
        if (variables.find(name) != variables.end()) {
            value = variables[name];
        } else {
            return;
        }
    }
    variables[name] = value;
    setenv(name.c_str(), value.c_str(), 1);
}

void interpreter::cd(std::vector<std::string> argv) {
    /*
     * Bug about storing new cd path in the bShell initial dir fixed by Taras Zelyk,
     * discovered by Nina Bondar.
     */
    for (size_t i = 1; i < argv.size(); i++) {
        if (argv.at(i) == "-h" || argv.at(i) == "--help") {
            std::cout << "Use this command to get change the working directory" << std::endl;
            return;
        }
    }

    if (argv.size() < 2) {
        std::cout << "Use this command to get change the working directory" << std::endl;
        return;
    }

    std::string newFullPath = getCurrentPath() + "/" + argv.at(1);
    DIR *dir = opendir(newFullPath.c_str());
    if (dir) {
        boost::filesystem::path newAbsPath = boost::filesystem::canonical(argv.at(1), getCurrentPath());
        std::string absPath = newAbsPath.generic_string();
        boost::filesystem::current_path(absPath);
    } else {
        std::cout << "Error. No such directory" << std::endl;
    }
}

std::vector<std::string> interpreter::getAvailable_commands() {
    return available_commands;
}

void interpreter::update_commands(const std::string &path) {
    /*
     * Bug not clearing the commands list before update fixed by Yasya Shpot,
     * discovered by Nina Bondar.
     */
    available_commands.clear();
    if (!path.empty()) {
        namespace fs = boost::filesystem;

        fs::path apk_path(path);
        fs::recursive_directory_iterator end;

        available_commands.clear();

        for (fs::recursive_directory_iterator i(apk_path); i != end; ++i) {
            const fs::path cp = (*i);
            std::string command = cp.stem().string();
            if (command != "") {
                available_commands.push_back(command);
            }
        }
    }
}

void interpreter::help() {
    std::cout << "Welcome to the beerShell!" << std::endl;
    std::cout << "Available commands are help, cd, pwd, exit and:\n============\n";
    std::vector<std::string> coms = getAvailable_commands();
    for (size_t i = 0; i < coms.size(); i++) {
        std::cout << coms.at(i) << std::endl;
    }
    std::cout << "============\nHave a great day" << std::endl;
}

void interpreter::stop(std::vector<std::string> argv) {
    for (size_t i = 1; i < argv.size(); i++) {
        if (argv.at(i) == "-h" || argv.at(i) == "--help") {
            std::cout
                    << "Use this command to get stop working with the beerShell. You can specify the exit code with the following: exit <exit code>"
                    << std::endl;
            return;
        }
    }
    if (argv.size() == 1) {
        exit(0);
    } else {
        exit(atoi(argv.at(1).c_str()));
    }
}

int interpreter::executeBuiltIn(std::vector<std::string> args) {
    std::string command = args.at(0);
    if (command == "exit") {
        stop(args);
        return 0;
    }

    if (command == "help") {
        help();
        return 0;
    }

    if (command == "pwd") {
        pwd(args);
        return 0;
    }
    if (command == "echo") {
        echo(args);
        return 0;
    }

    if (command == "export") {
        export_(args);
        return 0;
    }
    if (command == "cd") {
        cd(args);
        return 0;
    }

    if (command == "upd") {
        update_commands(BIN_PATH);
        return 0;
    }
    return -1;
}

int interpreter::process(std::string command) {
    std::vector<std::string> commands;

    std::regex comments_regex("#+.*");
    std::regex variables_regex("\\w+=\\w+");
    std::regex substitution_regex("\\w+=\\w+");
    boost::regex splitCommands("((?:[^|\"']|\"[^\"]*\"|'[^']*')+)"); // split by pipes '|'
    boost::regex splitArgs("(\"[^\"]*\"|'[^']*'|[^\\s]+)");

    command = std::regex_replace(command, comments_regex, ""); // remove comments
    boost::sregex_token_iterator commIter(command.begin(), command.end(), splitCommands, 0);
    boost::sregex_token_iterator endIter;

    for (; commIter != endIter; ++commIter) {
        commands.push_back(commIter->str());
    }
    int fd[2];
    io_desc iodesc;
    iodesc.pipe_in = 0;
    iodesc.pipe_out = 1;

    for (size_t i = 0; i < commands.size(); i++) {
        std::vector<std::string> args;
        iodesc.in = "";
        iodesc.out = "";
        iodesc.err = "";
        iodesc.errtoout = false;
        bool run_in_bckg = false;


        boost::sregex_token_iterator iter(commands.at(i).begin(), commands.at(i).end(), splitArgs, 0);
        boost::sregex_token_iterator end;
        if (DEBUG)
            std::cout << "Command " << commands.at(i) << " -> [";

        for (; iter != end; ++iter) {
            std::string token(iter->str());
            boost::trim(token);
            if (DEBUG)
                std::cout << token << ", ";
            if (run_in_bckg) //if there are arguments after '&'
                return 2;

            if (token == "&") {
                run_in_bckg = true;
            } else if (token == ">") {
                std::string next_token((++iter)->str());
                boost::trim(next_token);
                if (DEBUG)
                    std::cout << next_token << ", ";
                iodesc.out = next_token;
            } else if (token == "2>") {
                std::string next_token((++iter)->str());
                boost::trim(next_token);
                if (DEBUG)
                    std::cout << next_token << ", ";
                iodesc.err = next_token;
            } else if (token == "<") {
                std::string next_token((++iter)->str());
                boost::trim(next_token);
                if (DEBUG)
                    std::cout << next_token << ", ";
                iodesc.in = next_token;
            } else if (token == "2>&1") {
                iodesc.errtoout = true;
            } else if (std::regex_match(token, variables_regex) &&
                       std::find(args.begin(), args.end(), "export") == args.end()) {
                size_t found = token.find("=");
                std::string name = token.substr(0, found);
                std::string value = token.substr(found + 1, std::string::npos);
                variables[name] = value;
            } else {
                if (token[0] == '$') {
                    std::string name = token.substr(1, token.size() - 1);
                    if (variables.find(name) != variables.end()) {
                        token = variables[name];
                    }
                }
                args.push_back(token);
            }
        }

        if (DEBUG)
            std::cout << "]" << std::endl;

        if (DEBUG) {
            for (const auto &p : variables) {
                std::cout << "variables[" << p.first << "] = " << p.second << '\n';
            }
        }
        if (args.size() > 0) {
            if (i != commands.size() - 1) {
                if (pipe(fd)) {
                    std::cerr << "Pipe error"<< std::endl;
                    _exit(1);
                }
                iodesc.pipe_out = fd[1];

                if (executeBuiltIn(args) == -1) {
                    start_process(args, iodesc, run_in_bckg);
                }
                close(fd[1]);
                iodesc.pipe_in = fd[0];
            } else {
                if (executeBuiltIn(args) == -1) {
                    start_process(args, iodesc, run_in_bckg);
                }
            }
        }
    }
    return 0;
}


void interpreter::start_process(std::vector<std::string> command, io_desc iodesc, bool run_in_bckg) {
    pid_t processID = fork();
    pid_t wpid;
    int status;
    if (processID == -1) {
        std::cerr << "fork() failed!" << std::endl;
    } else if (processID == 0) {

        if (run_in_bckg && iodesc.out.empty()) {
            close(STDOUT_FILENO);
        }

        if (run_in_bckg && iodesc.err.empty()) {
            close(STDERR_FILENO);
        }

        if (run_in_bckg && iodesc.in.empty()) {
            close(STDIN_FILENO);
        }

        if (!iodesc.out.empty()) {
            int fd = open(iodesc.out.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        if (!iodesc.err.empty()) {
            int fd = open(iodesc.err.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        if (!iodesc.in.empty()) {
            int fd = open(iodesc.in.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (iodesc.errtoout) {
            dup2(1, 2);
        }
        if (iodesc.pipe_in != 0) {
            dup2(iodesc.pipe_in, 0);
            close(iodesc.pipe_in);
        }
        if (iodesc.pipe_out != 1) {
            dup2(iodesc.pipe_out, 1);
            close(iodesc.pipe_out);
        }

        char *args[command.size() + 1];// = malloc((command.size() + 1) * sizeof(char *));
        for (size_t i = 0; i < command.size(); i++) {
            args[i] = (char *) command.at(i).c_str();
        }
        args[command.size()] = NULL;

        std::string path = curPath + "/" + BIN_PATH + "/" + command.at(0);

        if (boost::filesystem::exists(path)) {
            execvp(path.c_str(), args);
        } else {
            execvp(command.at(0).c_str(), args);
        }

        std::cerr << "Command not recognized." << std::endl << "Use help to get." << std::endl;
        _exit(1);
    } else {
        do {
            if (!run_in_bckg) {
                wpid = waitpid(processID, &status, WUNTRACED);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        if (WIFSIGNALED(status)) {
            std::cerr << "Child killed (signal " << WTERMSIG(status) << ")" << std::endl;

        } else if (WIFSTOPPED(status)) {
            std::cerr << "Child stopped (signal " << WSTOPSIG(status) << ")" << std::endl;
        }
    }
}