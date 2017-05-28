#include <iostream>
#include "interpreter.h"
#include <boost/regex.hpp>
#include <fstream>

using namespace std;
void loop();
void process_file(string arg);

int main(int argc, const char *argv[])
{
    if(argc == 1){
        loop();
    } else {
        vector<string> arguments(argv + 1, argv + argc);
        string req = "";
        for(size_t i = 0; i < arguments.size(); i++){
            req += arguments.at(i);
            req += (i < arguments.size() - 1) ? " " : "";
        }
        vector<string> args;
        cout << req << endl;
        boost::regex splitArgs("(\"[^\"]+\"|[^\\s\"]+)");

        boost::sregex_token_iterator iter(req.begin(), req.end(), splitArgs, 0);
        boost::sregex_token_iterator end;

        for(; iter != end; ++iter ) {
                args.push_back(*iter);
        }

        for(size_t i = 0; i < args.size(); i++){
            args.at(i) = boost::replace_all_copy(args.at(i), "\"", "");;
        }
        for(size_t i = 0; i < arguments.size(); i++){
            cout << arguments.at(i) << endl;
            process_file(arguments.at(i));
        }
    }
    return 0;
}

void loop(){
    string line;
    int retCode = 0;
    interpreter shell;
    while(retCode == 0){
        cout << shell.getCurrentPath() << "$ ";
        getline(cin, line);
        if(line != ""){
        retCode = shell.process(line);
        }
    }
}

void process_file(string arg){
    ifstream myfile (arg.c_str());
    string line;
    interpreter shell;
    if (myfile.is_open()) {
        while ( getline (myfile,line) )
        {
            shell.process(line);
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file\n";
    }
}
