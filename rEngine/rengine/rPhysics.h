#pragma once
#include "stdafx.h"

typedef btRigidBody* rEntityBody;

class rPhysics 
{
public:
	rPhysics();
	~rPhysics();

	void Update();
	Matrix GetTransform(rEntityBody body, bool predicted = false, float predictStep = 1/240.f);

	void Init(Vector3 gravity = { 0.f, -9.8f, 0.f}, ContactAddedCallback collisionCallback = NULL);
	void Close();

	rEntityBody AddEntity(btConvexInternalShape* shape, float mass, Vector3 transform);
	void RemoveEntity(rEntityBody);

	btConvexInternalShape* GetShapeFromMesh(Model model, float scale = 1.f);
	Mesh GetMeshFromShape(btConvexInternalShape* shape, float scale = 1.f);

	bool CheckAABB(rEntityBody obj0, rEntityBody obj1);

	const float step = 1 / 300.f;
	const int maxSubStep = 100;
private:
	
	btBroadphaseInterface* broadphase = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btConstraintSolver* constrainSolver = nullptr;

	btDefaultCollisionConfiguration* collisionConf = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;
};

