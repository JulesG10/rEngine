#pragma once

#include "../rengine/world/rWorld.h"

typedef enum GameWorldObjectType {
	PLAYER,
	FLOOR,
	UNDEFINED
}GameWorldObjectType;

typedef struct GameWorldObject {
	GameWorldObjectType type;
	int hit = 0;
}GameWorldObject;

class GameWorld : public rWorld
{
public:
	GameWorld(RenderTexture*);
	~GameWorld();

	void  Load() override;
	void  Unload() override;
	void  UpdateFrame() override;
	void  PreFrame();

private:
	void DrawSkyBox();
	void DrawDebugStats();


	void Inputs();
	void ModeFPS(Vector3 pos);
	void ModeTPS(Vector3 pos);
	Vector3 GetPlayerHead(bool add = true);

	RenderTexture* render;
	Model sky = {0};

	Model floor = { 0 };
	rEntityBody floorBody = nullptr;

	Model player = { 0 };
	Model playerBox = { 0 };
	rEntityBody playerBody = nullptr;
	bool playerGravity = false;
	BoundingBox playerBounding;
	float playerScale = 0.5f;
	RenderTexture playerGun;
	Camera3D playerCam;
	Model gun;

	Vector3 velocity = { 0 };
	rCameraMode mode = rCameraMode::ThirdPerson;
};

inline std::string StrVec3(const char* name, Vector3 vec)
{
	return std::string(name) + " (x: " + std::to_string(vec.x) + " y: " + std::to_string(vec.y) + " z:" + std::to_string(vec.z) + " )";
}

inline std::string StrVec3(const char* name, btVector3 vec)
{
	return std::string(name) + " (x: " + std::to_string(vec.getX()) + " y: " + std::to_string(vec.getY()) + " z:" + std::to_string(vec.getZ()) + " )";
}

inline std::string StrSize(const char* name, int w, int h)
{
	return std::string(name) + " " + std::to_string(w) + "x" + std::to_string(h);
}

inline std::string StrStats(const char* name, float val)
{
	if ((val - (int)val) == 0)
	{
		return std::string(name) + " " + std::to_string((int)val);
	}
	else {
		return std::string(name) + " " + std::to_string(val);
	}
}

inline std::string StrMatrix(Matrix matrix)
{
	return std::to_string(matrix.m0) + " " + std::to_string(matrix.m4) + " " + std::to_string(matrix.m8) + " " + std::to_string(matrix.m12) + "\n" +  
	std::to_string(matrix.m1) + " " + std::to_string(matrix.m5) + " " + std::to_string(matrix.m9) + " " + std::to_string(matrix.m13) + "\n" +
	std::to_string(matrix.m2) + " " + std::to_string(matrix.m6) + " " + std::to_string(matrix.m10) + " " + std::to_string(matrix.m14) + "\n" +
	std::to_string(matrix.m3) + " " + std::to_string(matrix.m7) + " " + std::to_string(matrix.m11) + " " + std::to_string(matrix.m15) + "\n";
}


//bool collisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);