#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

class interpreter
{
    public:
        interpreter();
        virtual ~interpreter();
        std::string getCurrentPath();
        std::string cd(std::string relPath);
        std::string pwd();
        std::vector<std::string>& getAvailable_commands();
        void update_commands(const std::string& path);
        void process(std::string command, std::string args[]);
        void start_process(std::string command, std::string args[]);
    protected:

    private:
        std::string curPath;
        std::vector<std::string> available_commands;
        bool isCommand(std::string input);
};

#endif // INTERPRETER_H
