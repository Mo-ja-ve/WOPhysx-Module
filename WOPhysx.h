//#pragma once

#ifndef WOPHYSX_H
#define WOPHYSX_H

#include "WO.h"


namespace Aftr{

    class WOPhysx : public WO {
    public:
        ~WOPhysx();
        WOPhysx();

        static WOPhysx* New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst);
        void init();
        void onCreate( const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst);
        void updatePoseFromPhysx();
        void setPosition(float x, float y, float z);

        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorcallback;
        physx::PxRigidDynamic* actor = NULL;
        physx::PxFoundation* gFoundation = NULL;
        //removed gPhysics and gScene to declare them in loadmap()
        physx::PxPhysics* gPhysics;
        physx::PxScene* gScene;
    };
}

#endif