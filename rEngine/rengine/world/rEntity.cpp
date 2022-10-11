#include "rEntity.h"


rEntity::rEntity(rCamera* camera, rPhysics* physics)
{
	this->camera = camera;
	this->physics = physics;
}

rEntity::~rEntity()
{
}

void rEntity::Load()
{
}

void rEntity::Unload()
{
}

void rEntity::UpdateFrame()
{
}

Vector3 rEntity::GetPosition()
{
	return Vector3();
}

Vector3 rEntity::GetVelocity()
{
	return Vector3();
}

void rEntity::SetPosition(Vector3 position, bool resetVelocity)
{
}
