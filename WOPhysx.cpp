#include "WOPhysx.h"

/************************************************************/
/*              WOPHysx function definitions                */
/************************************************************/
namespace Aftr {

    WOPhysx::~WOPhysx() { }

    WOPhysx::WOPhysx() : WO(), IFace(this) {  }


    WOPhysx* WOPhysx::New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst) {

        WOPhysx* WOpx = new WOPhysx();
        WOpx->init();
        WOpx->onCreate(path, scale, mst);

        return WOpx;
    }

    void WOPhysx::init() {

        /***     physx init stuff      ***/
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorcallback);
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), 0, NULL);
        physx::PxSceneDesc gSceneDesc(gPhysics->getTolerancesScale());

        gSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        gSceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

        gScene = gPhysics->createScene(gSceneDesc);
    }

    void WOPhysx::onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst) {

        WO::onCreate(path, scale, mst);
        physx::PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

        physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*gPhysics, physx::PxTransform(physx::PxVec3(0, 0, 10.0f)), physx::PxSphereGeometry(3.0f), *gMaterial, 10.0f);
        gScene->addActor(*dynamic);
        physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(1, 1, 1), *gMaterial, true);
        physx::PxTransform t({ 0,0,0 });

        actor = gPhysics->createRigidDynamic(t);
        actor->attachShape(*shape);

        actor->userData = this;
        gScene->addActor(*actor);
    }
}