#pragma once

#include "../stdafx.h"
#include "../rCamera.h"
#include "../rPhysics.h"

class rEntity
{
public:
	rEntity(rCamera* camera, rPhysics* physics);
	~rEntity();


	virtual void Load();
	virtual void Unload();
	virtual void UpdateFrame();

	Vector3 GetPosition();
	Vector3 GetVelocity();
protected:
	void SetPosition(Vector3 position, bool resetVelocity);

	rCamera* camera = nullptr;
	rPhysics* physics = nullptr;
private:
	Vector3 position = {};
	Vector3 velocity = {};
};

