#include "engine/Nugget.h"

// Game Modules.
#include "Games/Epoch/Epoch.h"

int main(void)
{
    EpochGame epoch;

    Nugget::Instance().Run(&epoch);

    return 0;
}