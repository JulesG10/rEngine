#include "rCore.h"

rCore::rCore()
{
}

int rCore::Start(char* cmdLine)
{

#ifndef _DEBUG
    SetTraceLogLevel(LOG_NONE);
#endif // _DEBUG

    if (this->Init(std::string(cmdLine)) != 0)
    {
        return 1;
    }

    /* TODO: load texture sync
    pthread_t threadId;
    int error = pthread_create(&threadId, NULL, &WindowLoadThread, this);
    if (error != 0)
    {
        return error;
    }
    */
     
    while (!WindowShouldClose())
    {
        if (this->loading)
        {
            this->LoadingFrame();
            this->Load();
            this->loading = false;
        }
        else
        {   this->UpdateFrame();
        }
    }
    
    this->UnLoad();
    CloseWindow();

    return 0;
}

void rCore::InitDefault(int w, int h, const char* title)
{
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status) && status.ACLineStatus == AC_LINE_ONLINE)
    {
        SetConfigFlags(FLAG_FULLSCREEN_MODE); // FLAG_MSAA_4X_HINT

        // shaders
        // l’Anti-Aliasing et anticrénelage
        // https://www.malekal.com/quest-ce-que-anti-aliasing-et-anticrenelage-fxaa-smaa-msaa-ssaa-txaa/
        // MSAA/SSAA	Les plus beaux rendus mais les plus gourmands en ressources
        // TXAA	Se place entre les deux en terme de coût de performances
        // FXAA/SMAA/TAA Bon rendu sans trop impacter les performances
    }
    else {
        SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    }

    InitWindow(w, h, title);
    DisableCursor();
    SetTargetFPS(0);
}

int rCore::Init(std::string args)
{
    

    return 0;
}

void rCore::UpdateFrame()
{
   
}

void rCore::LoadingFrame()
{
    
}


void rCore::Load()
{
    
}

void rCore::UnLoad()
{
    
}

static void* WindowLoadThread(void* data)
{
    
    if (!data)
    {
        return nullptr;
    }

    rCore* self = (rCore*)data;
    if (!self)
    {
        return nullptr;
    }

    self->Load();
    return nullptr;
}
