#include <iostream>
#include <thread>
#include <memory>
#include <climits>

#include "Card.h"
#include "Deck.h"
#include "Server.h"

const short DEFAULT_PORT = 23;

int main(int argc, char* argv[])
{
    short port = DEFAULT_PORT;
    if (argc > 1)
    {
        int portArg = std::stoi(argv[1]);
        if (portArg < 1 || portArg > SHRT_MAX)
        {
            std::cerr << "Must provide a valid port number between 1 and " << SHRT_MAX << std::endl;
            throw std::invalid_argument("Invalid port number");
        }
        port = static_cast<short>(portArg);
    }

    std::cout << "Starting server on port " << port << "..." << std::endl;
    auto server = std::make_shared<Server>(port);
    std::thread(&Server::Start, server).join();

    return 0;
}
