#pragma once

#include "../stdafx.h"
#include "../rCamera.h"
#include "../rPhysics.h"


class rWorld
{
public:
	rWorld();
	~rWorld();

	void virtual Load();
	void virtual Unload();
	void virtual UpdateFrame();

protected:
	rCamera camera;
	rPhysics physics;
};

