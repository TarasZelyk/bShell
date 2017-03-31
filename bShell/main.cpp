#include <iostream>
#include "interpreter.h"

using namespace std;

int main(int argc, const char *argv[])
{
    interpreter shell;
    cout << argv[0] << endl << argv[1] << endl;
    std::string args[argc - 2];
    for(int i = 2; i < argc; i++){
        args[i-1] = argv[i];
    }
    shell.getAvailable_commands();
    shell.process(argv[1], args);
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
