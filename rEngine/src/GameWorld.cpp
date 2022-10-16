#include "GameWorld.h"

GameWorld::GameWorld(RenderTexture* render)
{
    this->render = render;
}

GameWorld::~GameWorld()
{
}

void GameWorld::Load()
{
    this->camera.Init({ 0 }, { 0 }, 90.0f);
    this->physics.Init({ 0, -9.81, 0 });//, collisionCallback);

    this->playerCam = {
        {0.318f,0.183,0.134},
        {-0.681,0.167,0.140},
        {0,1,0},
        90.0f,
        CAMERA_PERSPECTIVE
    };

    this->playerGun = LoadRenderTexture(this->render->texture.width, this->render->texture.height);
    this->gun = LoadModel("./assets/models/gun/model.obj");

    this->player = LoadModel("./assets/models/man/Man.obj");
    
    btCapsuleShape* playerShape = new btCapsuleShape(1, 3);
    this->playerBox = LoadModelFromMesh(this->physics.GetMeshFromShape(playerShape));
    this->playerBody = this->physics.AddEntity(playerShape, 10.f, { 0, 100, 0 });
    this->playerBounding = GetModelBoundingBox(this->player);

    //GameWorldObject playerObj = { GameWorldObjectType::PLAYER,  0 };
    //this->playerBody->setUserPointer(&playerObj);



    
    //Texture2D texture = LoadTextureFromImage(GenImageWhiteNoise(100, 100, 0.2563f));
    //this->cube.materials[0].maps[MATERIAL_MAP_SPECULAR].texture = texture;

    Image heightmap = LoadImage("./assets/textures/heightmap.png");
    /*for (size_t i = 0; i < 100; i++) // heightmap chunk from random seed
    {
        ImageDrawPixel(&heightmap, GetRandomValue(0, 100), GetRandomValue(0, 100), WHITE);
    }*/

    Vector3 floorSize = { 200, 15, 200 };
    this->floor = LoadModelFromMesh(GenMeshHeightmap(heightmap, floorSize));
    this->floor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(heightmap);//LoadTexture("./assets/textures/grass.png");
    UnloadImage(heightmap);

    this->floorBody = this->physics.AddEntity(this->physics.GetShapeFromMesh(this->floor, 1.f), 0.f, { -floorSize.x/2, 0, -floorSize.z/2});

    //GameWorldObject floorObj = { GameWorldObjectType::FLOOR,  0 };
    //this->floorBody->setUserPointer(&floorObj);

    this->sky = LoadModelFromMesh(GenMeshCube(1.f, 1.f, 1.f));

    sky.materials[0].shader = LoadShader("./assets/shaders/sky.vs", "./assets/shaders/sky.fs");
    int envMap[1] = { MATERIAL_MAP_CUBEMAP };
    SetShaderValue(sky.materials[0].shader, GetShaderLocation(sky.materials[0].shader, "environmentMap"), (void*)envMap, SHADER_UNIFORM_INT);

    Image skyImage = LoadImage("./assets/textures/sky.png");
    sky.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(skyImage, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(skyImage);
}

void GameWorld::Unload()
{
    UnloadModel(this->sky);

    this->physics.Close();
}

void GameWorld::PreFrame()
{
    BeginTextureMode(this->playerGun);
    ClearBackground(BLANK);
    BeginMode3D(this->playerCam);
    DrawModel(this->gun, { 0 }, 1.f, WHITE);
    EndMode3D();
    EndTextureMode();
}

void GameWorld::UpdateFrame()
{
    this->physics.Update();
    this->Inputs();

    Vector3 playerHead = this->GetPlayerHead();
    if (mode == rCameraMode::ThirdPerson)
    {
        this->ModeTPS(playerHead);
    }
    else {
        this->ModeFPS(playerHead);
    }
    
    
    if ((abs(velocity.x) > 0.1f || abs(velocity.y) > 0.1f || abs(velocity.z) > 0.1f))
    {
        if (this->playerBody->wantsSleeping())
        {
            this->playerBody->setActivationState(ACTIVE_TAG);
        }

        Vector3 linearVel = {
            -((velocity.x * sinf(this->camera.rotation.x) - velocity.z * cosf(this->camera.rotation.x))),
            velocity.y,
            -((velocity.x * cosf(this->camera.rotation.x) + velocity.z * sinf(this->camera.rotation.x)))
        };
        this->playerBody->setLinearVelocity(btVector3(linearVel.x, this->playerBody->getLinearVelocity().getY(), linearVel.z));
    }
    
    if (!this->playerGravity)
    {
        if (this->physics.CheckAABB(this->floorBody, this->playerBody))
        {
            this->playerGravity = true;
            this->playerBody->setGravity({ 0,-100,0 });
        }
    }

    

    BeginMode3D(this->camera.camera);
    this->DrawSkyBox();

    Matrix playerTransform = this->physics.GetTransform(this->playerBody);
    if (mode == rCameraMode::ThirdPerson)
    {
     
        this->player.transform = MatrixRotateXYZ({ 0, this->camera.rotation.x, 0 });
        DrawModel(this->player, this->GetPlayerHead(false), this->playerScale, WHITE);


        this->playerBox.transform = this->physics.GetTransform(this->playerBody, true, this->physics.step * 10);
        DrawModelWires(this->playerBox, { 0 }, 1.f, GREEN);

        this->playerBox.transform = playerTransform;
        DrawModelWires(this->playerBox, { 0 }, 1.f, RED);
    }
    

    this->floor.transform = this->physics.GetTransform(this->floorBody);
    DrawModel(this->floor, { 0 }, 1.f, WHITE);


    
#ifdef _DEBUG
    static RayCollision rayCol;
    Vector2 crossPosition = { this->render->texture.width / 2.f, (this->render->texture.height / 2.f) - 200 };
    if (IsKeyPressed(72))//H
    {
        Ray ray = GetMouseRay(crossPosition, this->camera.camera);
        rayCol = GetRayCollisionMesh(ray, this->floor.meshes[0], this->floor.transform);
    }
    DrawLine3D(
        playerHead,
        rayCol.point,
        GREEN);
#endif // _DEBUG

   
    EndMode3D();
   
    if (mode == rCameraMode::FirstPerson)
    {
        DrawTexturePro(this->playerGun.texture, { 0, 0,  (float)this->render->texture.width, -(float)this->render->texture.height }, {0, 0, (float)this->render->texture.width, (float)this->render->texture.height}, {0, 0}, 0, WHITE);
    }

    DrawText(StrStats("Distance", Vector3Distance(playerHead, rayCol.point)).c_str(), crossPosition.x + 20, crossPosition.y, 10, BLACK);
    DrawCircle(crossPosition.x, crossPosition.y, 2, RED);

#ifdef _DEBUG
    this->DrawDebugStats();
#endif // _DEBUG
}


void GameWorld::DrawSkyBox()
{
    rlDisableBackfaceCulling();
    rlDisableDepthMask();

    DrawModel(sky, { 0, 0, 0 }, 1.f, WHITE);

    rlEnableBackfaceCulling();
    rlEnableDepthMask();
}

void GameWorld::DrawDebugStats()
{
    static int keyDown = 0;
    int currentKey = GetKeyPressed();
    if (currentKey != 0)
    {
        keyDown = currentKey;
    }

    btVector3 aabbMin;
    btVector3 aabbMax;
    this->playerBody->getAabb(aabbMin, aabbMax);

    std::string debug =
        StrStats("FPS", GetFPS()) + "\n" +
        StrVec3("Position", this->camera.camera.position) + "\n" +
        StrVec3("Target", this->camera.camera.target) + "\n" +
        StrVec3("Up", this->camera.camera.up) + "\n" +
        StrStats("FOV", this->camera.camera.fovy) + "\n" +
        StrStats("Aspect Ratio", (float)this->render->texture.width / (float)this->render->texture.height) + "\n" +
        StrSize("Render Texture Size", this->render->texture.width, this->render->texture.height) + "\n" +
        StrSize("Render Size", GetRenderWidth(), GetRenderHeight()) + "\n" +
        StrStats("Screen Hz", GetMonitorRefreshRate(GetCurrentMonitor())) + "\n" +
        StrStats("Last Key", keyDown) + "\n" +
        "Camera Matrix \n" +
        StrMatrix(GetCameraMatrix(this->camera.camera)) + "\n" +
        //StrVec3("Cube push velocity", this->cubeBody->getPushVelocity()) + "\n" +
        StrVec3("Player local inertia", this->playerBody->getLocalInertia()) + "\n" +
        StrVec3("Player linear velocity", this->playerBody->getLinearVelocity()) + "\n" + 
        //StrVec3("Cube angular velocity", this->cubeBody->getAngularVelocity()) + "\n" +
        //StrVec3("Cube total force", this->cubeBody->getTotalForce()) + "\n" +
        StrStats("Player friction",this->playerBody->getFriction()) + "\n" +
        StrStats("Player sleeping", this->playerBody->wantsSleeping()) + "\n" +
        StrStats("Player Forced Gravity", this->playerGravity) + "\n" +
        StrVec3("Input Velocity", this->velocity) + "\n" + 
        "Player Matrix\n" +
        StrMatrix(this->player.transform) + "\n" +
        StrVec3("AABB min", aabbMin) + "\n" + 
        StrVec3("AABB Max",aabbMax)
        ;
    DrawText(debug.c_str(),  0,0 , 10, BLACK);
}



void GameWorld::Inputs()
{
    velocity = Vector3Multiply(velocity, { 0.9f,0.9f ,0.9f });
    float speed = 1.f;
    if (IsKeyDown(32)) // space
    {
        speed *= 3;
    }

    if (IsKeyDown(87)) // front/back
    {
        velocity.x += speed;
    }
    else if (IsKeyDown(83))
    {
        velocity.x -= speed;
    }

    if (IsKeyDown(68)) // right/left
    {
        velocity.z += speed;

    }
    else if (IsKeyDown(65))
    {
        velocity.z -= speed;
    }

    if (IsKeyDown(82)) // up/down
    {
        velocity.y += speed;
    }
    else if (IsKeyDown(70))
    {
        velocity.y -= speed;
    }
}

void GameWorld::ModeFPS(Vector3 pos)
{
    this->camera.camera.position = pos;
    this->camera.UpdateFirst({ 0 });

    if (this->camera.targetDistance >= 2)
    {
        mode = rCameraMode::ThirdPerson;
        this->camera.SwitchUpdate(mode);
    }
}

void GameWorld::ModeTPS(Vector3 pos)
{
    this->camera.camera.target = pos;
    this->camera.UpdateThird({ 0 });

    if (this->camera.targetDistance < 2)
    {
        mode = rCameraMode::FirstPerson;
        this->camera.SwitchUpdate(mode);
    }
}

Vector3 GameWorld::GetPlayerHead(bool add)
{
    float diff = (playerBounding.max.y - playerBounding.min.y) * this->playerScale;
    diff = add ? diff : -diff;

    btVector3 origin = this->playerBody->getWorldTransform().getOrigin();
    return { origin.getX(), origin.getY() + diff / 2.f, origin.getZ() };
}


/*
if (!this->playerGravity)
    {
        GameWorldObject* plptr = static_cast<GameWorldObject*>(this->playerBody->getUserPointer());
        if (plptr != nullptr && plptr->hit >= 1)
        {
            this->playerGravity = true;
            this->playerBody->setGravity({ 0,-100,0 });
        }
}

bool collisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj0, int partId0, int index0, const btCollisionObjectWrapper* obj1, int partId1, int index1)
{
    GameWorldObject* obj0ptr = static_cast<GameWorldObject*>(obj0->getCollisionObject()->getUserPointer());
    GameWorldObject* obj1ptr = static_cast<GameWorldObject*>(obj1->getCollisionObject()->getUserPointer());

    if (obj0ptr != nullptr && obj1ptr != nullptr)
    {
        if (obj0ptr->type == GameWorldObjectType::FLOOR && obj1ptr->type == GameWorldObjectType::PLAYER)
        {
            obj1ptr->hit++;
        }
        else if (obj0ptr->type == GameWorldObjectType::PLAYER && obj1ptr->type == GameWorldObjectType::FLOOR)
        {
            obj0ptr->hit++;
        }
    }

    return false;
}
*/