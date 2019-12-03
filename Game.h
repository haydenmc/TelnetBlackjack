#pragma once

#include "TelnetServer.h"

class Game
{
public:
    // Public methods
    Game(TelnetServer* server);

private:
    // Private members
    TelnetServer* server = nullptr;

};
