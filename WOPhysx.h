//#pragma once
#ifndef WOPHYSX_H
#define WOPHYSX_H

#include "WO.h"

namespace Aftr{

    class WOPhysx : public WO {
    public:
        ~WOPhysx();
        WOPhysx();
        static WOPhysx* New();
        void init();
        void onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst);

        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorcallback;
        physx::PxRigidDynamic* actor = NULL;
        physx::PxFoundation* gFoundation;
        physx::PxPhysics* gPhysics;
        physx::PxScene* gScene;
    };
}

#endif