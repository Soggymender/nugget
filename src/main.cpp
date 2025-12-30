#include "engine/Nugget.h"

// Game Modules.
#include "Games/Epoch/Epoch.h"

// Create the engine singleton.
Nugget* Nugget::m_pInstance = nullptr;

int main(void)
{
    Nugget& nugget = *Nugget::GetInstance();

    EpochGame epoch;

    nugget.Run(&epoch);

    return 0;
}