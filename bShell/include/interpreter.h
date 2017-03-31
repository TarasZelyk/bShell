#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <boost/filesystem.hpp>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <vector>

class interpreter
{
    public:
        interpreter();
        virtual ~interpreter();
        std::string getCurrentPath();
        std::string cd(std::string relPath);
        std::string exit(int exitCode);
        std::string pwd();
    protected:

    private:
        std::string curPath;
        std::vector<std::string> available_commands;
        void update_commands();
};

#endif // INTERPRETER_H
