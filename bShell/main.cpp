#include <iostream>
#include "interpreter.h"
#include <boost/regex.hpp>
#include <fstream>

using namespace std;

void loop();

void process_file(string arg);

int main(int argc, const char *argv[]) {
    if (argc == 1) {
        loop();
    } else {
        for (size_t i = 1; i < argc; i++) {
            process_file(argv[i]);
        }
    }
    return 0;
}

void loop() {
    string line;
    int retCode = 0;
    interpreter shell;
    while (retCode == 0) {
        cout << shell.getCurrentPath() << "$ ";
        getline(cin, line);
        if (line != "") {
            retCode = shell.process(line);
            if (retCode == 2) {
                std::cerr << "Syntax error!";
            } else if (retCode != 0) {
                std::cerr << "Unexpected error!";
            }
        }
    }
}

void process_file(string arg) {
    ifstream myfile(arg.c_str());
    string line;
    interpreter shell;
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            int retCode = shell.process(line);
            if (retCode == 2) {
                std::cerr << "Syntax error!";
            } else if (retCode != 0) {
                std::cerr << "Unexpected error!";
            }
        }
        myfile.close();
    } else {
        cout << "Unable to open file\n";
    }
}
