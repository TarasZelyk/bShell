#include <iostream>
#include "interpreter.h"

using namespace std;

int main()
{
    interpreter shell;
    shell.cd("../examplesasf");
    cout << shell.getCurrentPath() << endl;
    return 0;
}
