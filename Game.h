#pragma once

#include "Server.h"

class Game
{
public:
    // Public methods
    Game(Server* server);

private:
    // Private members
    Server* server = nullptr;

};
