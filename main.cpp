#include <iostream>
#include "Server.h"

int main()
{
    Server server{33333};
    server.run();

    return 0;
}