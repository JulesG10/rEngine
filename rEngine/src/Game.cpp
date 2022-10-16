#include "Game.h"

Game::Game() : world(&render)
{
}

Game::~Game()
{
}

int Game::Init(std::string args)
{
    this->InitDefault(1920, 1080);


#ifndef _DEBUG
    SetExitKey(KEY_NULL);
#endif // _DEBUG
    return 0;
}

void Game::UpdateFrame()
{
#if 0
    static int renderSizeIndex = 0;
    static Vector2 renderSizes[5] = {
        { 1920, 1080 },
        { 1280, 720 },
        { 854, 480 },
        { 640, 360 },
        { 426, 240 },
    };

    if (IsKeyPressed(71)) // G
    {
        renderSizeIndex++;
        this->render = LoadRenderTexture(renderSizes[renderSizeIndex].x, renderSizes[renderSizeIndex].y);
        this->renderRect = { 0, 0, renderSizes[renderSizeIndex].x, -renderSizes[renderSizeIndex].y };
    }
    else if (IsKeyPressed(72)) // H
    {
        renderSizeIndex--;
        this->render = LoadRenderTexture(renderSizes[renderSizeIndex].x, renderSizes[renderSizeIndex].y);
        this->renderRect = { 0, 0, renderSizes[renderSizeIndex].x, -renderSizes[renderSizeIndex].y };
    }
#endif // _DEBUG

    this->world.PreFrame();
    BeginTextureMode(this->render);
    ClearBackground(WHITE);
    this->world.UpdateFrame();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLANK);
    DrawTexturePro(this->render.texture, this->renderRect, this->screenRect, { 0, 0 }, 0, WHITE);
    EndDrawing();
}

void Game::LoadingFrame()
{
    Vector2 loadingTextPos = MeasureTextEx(GetFontDefault(), "Loading...", 30, 2);
    Vector2 centerText = { (GetRenderWidth() - loadingTextPos.x) / 2, (GetRenderHeight() - loadingTextPos.y) / 2 };

    BeginDrawing();
    ClearBackground(WHITE);
    DrawText("Loading ...", centerText.x, centerText.y, 30, BLACK);
    EndDrawing();
}

void Game::Load()
{
    this->render = LoadRenderTexture(1920, 1080); // 1280x960 4:3
    this->renderRect = { 0, 0, (float)this->render.texture.width, -(float)this->render.texture.height };
    this->screenRect = { 0, 0, (float)GetRenderWidth(), (float)GetRenderHeight() };

    this->world.Load();
}

void Game::UnLoad()
{
    this->world.Unload();
    UnloadRenderTexture(this->render);
}
