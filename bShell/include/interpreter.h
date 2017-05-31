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
#define BIN_PATH "bin"
#define DEBUG 0

class interpreter {
public:
    interpreter();

    std::string getCurrentPath();

    std::vector<std::string> getAvailable_commands();

    int process(std::string command);

protected:

private:
    struct io_desc {
        std::string out;
        std::string err;
        std::string in;
        int pipe_in;
        int pipe_out;
        bool errtoout;
    };

    std::string curPath;
    std::vector<std::string> available_commands;
    std::map<std::string, std::string> variables;

    void update_commands(const std::string &path);

    int executeBuiltIn(std::vector<std::string> input);

    void stop(std::vector<std::string> input);

    void start_process(std::vector<std::string> command, io_desc iodesc, bool run_in_bckg);

    void pwd(std::vector<std::string> argv);

    void echo(std::vector<std::string> argv);

    void export_(std::vector<std::string> argv);

    void cd(std::vector<std::string> argv);

    void help();
};

#endif // INTERPRETER_H
