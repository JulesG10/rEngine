#pragma once

#include "../rengine/rCore.h"
#include "GameWorld.h"

class Game : public rCore
{
public:
	Game();
	~Game();
private:
	int Init(std::string args) override;
	void UpdateFrame() override;
	void LoadingFrame() override;
	void Load() override;
	void UnLoad() override;

	RenderTexture render = { 0 };
	Rectangle screenRect = { 0 };
	Rectangle renderRect = { 0 };

	GameWorld world;
};

