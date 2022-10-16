#include "rPhysics.h"


rPhysics::rPhysics()
{
}

rPhysics::~rPhysics()
{
}

void rPhysics::Update()
{
    world->stepSimulation(GetFrameTime(), this->maxSubStep, this->step);
}

Matrix rPhysics::GetTransform(rEntityBody body, bool predicted, float predictStep)
{
    /*
    model->transform.m0 = m[0];
    model->transform.m1 = m[1];
    model->transform.m2 = m[2];
    model->transform.m3 = m[3];
    model->transform.m4 = m[4];
    model->transform.m5 = m[5];
    model->transform.m6 = m[6];
    model->transform.m7 = m[7];
    model->transform.m8 = m[8];
    model->transform.m9 = m[9];
    model->transform.m10 = m[10];
    model->transform.m11 = m[11];
    model->transform.m12 = m[12];
    model->transform.m13 = m[13];
    model->transform.m14 = m[14];
    model->transform.m15 = m[15];
    */

    btTransform transform;
    if (predicted && body)
    {
        body->predictIntegratedTransform(predictStep, transform);
    }
    else {
        if (body && body->getMotionState()) {
            body->getMotionState()->getWorldTransform(transform);
        }
        else {
            transform = body->getWorldTransform();
        }
    }
   
    btScalar m[16];
    transform.getOpenGLMatrix(m);

    Matrix out = MatrixIdentity();
    out.m0 = m[0];
    out.m1 = m[1];
    out.m2 = m[2];
    out.m3 = m[3];
    out.m4 = m[4];
    out.m5 = m[5];
    out.m6 = m[6];
    out.m7 = m[7];
    out.m8 = m[8];
    out.m9 = m[9];
    out.m10 = m[10];
    out.m11 = m[11];
    out.m12 = m[12];
    out.m13 = m[13];
    out.m14 = m[14];
    out.m15 = m[15];

    return out;   
}

void rPhysics::Init(Vector3 gravity, ContactAddedCallback collisionCallback)
{
    collisionConf = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConf);

    broadphase = new btDbvtBroadphase();
    constrainSolver = new btSequentialImpulseConstraintSolver();
    
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, constrainSolver, collisionConf);
    world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

    gContactAddedCallback = collisionCallback;
}

void rPhysics::Close()
{
    for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = world->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            while (body->getNumConstraintRefs())
            {
                btTypedConstraint* constraint = body->getConstraintRef(0);
                world->removeConstraint(constraint);
                delete constraint;
            }
            delete body->getMotionState();
            world->removeRigidBody(body);
        }
        else
        {
            world->removeCollisionObject(obj);
        }
        delete obj;
    }

    delete world;
    world = nullptr;

    delete constrainSolver;
    constrainSolver = nullptr;

    delete broadphase;
    broadphase = nullptr;

    delete dispatcher;
    dispatcher = nullptr;

    delete collisionConf;
    collisionConf = nullptr;
}

rEntityBody rPhysics::AddEntity(btConvexInternalShape* shape, float mass, Vector3 position)
{
    btVector3 intertia = btVector3(0, 0, 0);
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));

    /*btScalar m[16];
    m[0] = matrix.m0;
    m[1] = matrix.m1;
    m[2] = matrix.m2;
    m[3] = matrix.m3;
    m[4] = matrix.m4;
    m[5] = matrix.m5;
    m[6] = matrix.m6;
    m[7] = matrix.m7;
    m[8] = matrix.m8;
    m[9] = matrix.m9;
    m[10] = matrix.m10;
    m[11] = matrix.m11;
    m[12] = matrix.m12;
    m[13] = matrix.m13;
    m[14] = matrix.m14;
    m[15] = matrix.m15;

    btTransform transform;
    transform.setFromOpenGLMatrix(m);
    */
    
    btDefaultMotionState *defaultMotionSt = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, defaultMotionSt, shape, intertia);
    
    btRigidBody* body = new btRigidBody(cInfo);
    body->setUserIndex(-1);
    
    if (mass != 0 && !shape->isConcave())
    {
        body->getCollisionShape()->calculateLocalInertia(btScalar(mass), intertia);
    }
        
  
    world->addRigidBody(body);
    
    return body;
}

void rPhysics::RemoveEntity(rEntityBody body)
{
    world->removeRigidBody(body);
   
    btMotionState* ms = body->getMotionState();
    delete body;
    delete ms;
}

btConvexInternalShape* rPhysics::GetShapeFromMesh(Model model, float scale)
{
    if (!model.meshCount)
    {
        return nullptr;
    }

    btTriangleMesh* triangles = new btTriangleMesh();
    Mesh mesh = model.meshes[model.meshCount-1];
    for (size_t i = 0; i < mesh.triangleCount; i++)
    {
        Vector3 a, b, c;
        Vector3* vertdata = (Vector3*)mesh.vertices;

        if (mesh.indices)
        {
            a = vertdata[mesh.indices[i * 3 + 0]];
            b = vertdata[mesh.indices[i * 3 + 1]];
            c = vertdata[mesh.indices[i * 3 + 2]];
        }
        else
        {
            a = vertdata[i * 3 + 0];
            b = vertdata[i * 3 + 1];
            c = vertdata[i * 3 + 2];
        }

        a = Vector3Multiply(a, { scale, scale, scale });
        b = Vector3Multiply(b, { scale, scale, scale });
        c = Vector3Multiply(c, { scale, scale, scale });


        triangles->addTriangle(btVector3(a.x, a.y, a.z), btVector3(b.x, b.y, b.z), btVector3(c.x, c.y, c.z));
    }
    return (btConvexInternalShape*)new btBvhTriangleMeshShape(triangles, true);
}

btConvexInternalShape* rPhysics::GetShapeHullFromMesh(Model model, float scale)
{
    return (btConvexInternalShape*)new btShapeHull(this->GetShapeFromMesh(model, scale));
}

Mesh rPhysics::GetMeshFromShape(btConvexInternalShape* shape, float scale)
{
    Mesh mesh = Mesh();
    if (!shape->isConvex())
    {
        return mesh;
    }

    btShapeHull* hull = new btShapeHull((btConvexShape*)shape);
    hull->buildHull(shape->getMargin());
    if (!(hull->numTriangles() > 0))
    {
        return mesh;
    }

    mesh.vertexCount = hull->numTriangles() * 3;
    mesh.triangleCount = hull->numTriangles();

    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));


    int currentVertice = 0;
    int index = 0;

    const unsigned int* idx = hull->getIndexPointer();
    const btVector3* vtx = hull->getVertexPointer();

    for (int i = 0; i < hull->numTriangles(); i++)
    {
        int i1 = index++;
        int i2 = index++;
        int i3 = index++;
        btAssert(i1 < hull->numIndices() && i2 < hull->numIndices() && i3 < hull->numIndices());

        int index1 = idx[i1];
        int index2 = idx[i2];
        int index3 = idx[i3];
        btAssert(index1 < hull->numVertices() && index2 < hull->numVertices() && index3 < hull->numVertices());

        btVector3 v1 = vtx[index1];
        btVector3 v2 = vtx[index2];
        btVector3 v3 = vtx[index3];
        btVector3 normal = (v3 - v1).cross(v2 - v1);
        normal.normalize();

        mesh.vertices[currentVertice] = v1.x();
        mesh.vertices[currentVertice + 1] = v1.y();
        mesh.vertices[currentVertice + 2] = v1.z();
        mesh.normals[currentVertice] = normal.getX();
        mesh.normals[currentVertice + 1] = normal.getY();
        mesh.normals[currentVertice + 2] = normal.getZ();

        mesh.vertices[currentVertice + 3] = v2.x();
        mesh.vertices[currentVertice + 4] = v2.y();
        mesh.vertices[currentVertice + 5] = v2.z();
        mesh.normals[currentVertice + 3] = normal.getX();
        mesh.normals[currentVertice + 4] = normal.getY();
        mesh.normals[currentVertice + 5] = normal.getZ();

        mesh.vertices[currentVertice + 6] = v3.x();
        mesh.vertices[currentVertice + 7] = v3.y();
        mesh.vertices[currentVertice + 8] = v3.z();
        mesh.normals[currentVertice + 6] = normal.getX();
        mesh.normals[currentVertice + 7] = normal.getY();
        mesh.normals[currentVertice + 8] = normal.getZ();

        currentVertice += 9;
    }


    UploadMesh(&mesh, false);
    return mesh;
}

bool rPhysics::CheckAABB(rEntityBody obj0, rEntityBody obj1)
{
    btVector3 fmin, fmax;
    obj0->getAabb(fmin, fmax);

    btVector3 pmin, pmax;
    obj1->getAabb(pmin, pmax);

    BoundingBox fBox = { {fmin.getX(), fmin.getY(), fmin.getZ()}, {fmax.getX(), fmax.getY(), fmax.getZ()} };
    BoundingBox pBox = { {pmin.getX(), pmin.getY(), pmin.getZ()}, {pmax.getX(), pmax.getY(), pmax.getZ()} };

    return CheckCollisionBoxes(fBox, pBox);
}
