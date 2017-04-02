#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);
#define BAD_COMMAND "You entered not a command. Use help to get a list of all commands."

class interpreter
{
    public:
        interpreter();
        virtual ~interpreter();
        std::string getCurrentPath();
        std::vector<std::string>& getAvailable_commands();
        int process(std::string command);
    protected:

    private:
        std::string curPath;
        std::vector<std::string> available_commands;
        void update_commands(const std::string& path);
        bool isCommand(std::string input);
        int executeBuiltIn(std::vector<std::string> input);
        void stop(std::vector<std::string> input);
        void start_process(std::vector<std::string> command);
        void pwd(std::vector<std::string> argv);
        void cd(std::vector<std::string> argv);
        void help();
};

#endif // INTERPRETER_H
