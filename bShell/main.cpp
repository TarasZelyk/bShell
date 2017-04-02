#include <iostream>
#include "interpreter.h"

using namespace std;
void loop();
void process_file(const char *arg);

int main(int argc, const char *argv[])
{
    if(argc == 1){
        loop();
    } else {
        process_file(argv[1]);
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

void process_file(const char *arg){
    ifstream myfile (arg);
    string line;
    interpreter shell;
    if (myfile.is_open()) {
        while ( getline (myfile,line) )
        {
            shell.process(line);
            //cout << line << '\n';
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file";
    }
}
