#include "server.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " port users_dat_file\n";
        exit(1);
    }

    Server server(atoi(argv[1]), argv[2]);
    server.run();

    return 0;
}

