#include "src/Game.h"

#ifdef _DEBUG
int main(int argc, const char** argv)
{
    std::string args;
    for (int i = 0; i < argc; i++)
    {
        args += argv[i];
    }
    Game* game = new Game();
    return game->Start((char*)args.c_str());
}
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int nCmdShow)
{
    Game* game = new Game();
    return game->Start(pCmdLine);
}
#endif // _DEBUG


