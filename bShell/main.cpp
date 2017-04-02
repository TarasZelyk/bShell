#include <iostream>
#include "interpreter.h"

using namespace std;
void loop();

int main(int argc, const char *argv[])
{
    cout << argc << endl;
    if(argc == 1){
        loop();
    }
    interpreter shell;
    vector<string> coms = shell.getAvailable_commands();
    for(int i = 0; i < coms.size(); i++){
        cout << coms.at(i) << endl;
    }
    //shell.process(argv[1], args);
    //shell.start_process("as", args);
    /*cout << shell.getCurrentPath() << endl;
    shell.cd("../");
    cout << shell.getCurrentPath() << endl;
    shell.cd("..");
    cout << shell.getCurrentPath() << endl;
    shell.cd("..");
    cout << shell.getCurrentPath() << endl;*/
    return 0;
}

void loop(){
    string line;
    int retCode = 0;
    interpreter shell;
    vector<string> coms = shell.getAvailable_commands();
    for(int i = 0; i < coms.size(); i++){
        cout << coms.at(i) << endl;
    }
    while(retCode == 0){
        cout << shell.getCurrentPath() << "$ ";
        getline(cin, line);
        if(line != ""){
        retCode = shell.process(line);
        }
    }
}
