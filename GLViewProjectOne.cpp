#include "GLViewProjectOne.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
//#include "PhysicsEngineODE.h"
//#include "physx/include/PxPhysicsAPI.h"
//#include "ctype.h"
//#include "physx/include/vehicle/PxVehicleSDK.h"

//Different WO used by this module
//#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include <cstring>

using namespace Aftr;
//using namespace irrklang;


GLViewProjectOne* GLViewProjectOne::New( const std::vector< std::string >& args )
{
   GLViewProjectOne* glv = new GLViewProjectOne( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}

GLViewProjectOne::GLViewProjectOne( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewProjectOne::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewProjectOne::onCreate() is invoked after this module's LoadMap() is completed.
}

void GLViewProjectOne::onCreate()
{
   //GLViewProjectOne::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewProjectOne::~GLViewProjectOne()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewProjectOne::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.

   Aftr::Vector v_camPos(0, 0, 0);
   Aftr::Vector v_lookDir(0, 0, 0);
   Aftr::Vector v_up(0, 0, 0);

   int numBoxes = physx_boxes.size();

   Vector camvec = this->cam->getPosition();
   float camF[3] = { 1, 2 , 3 };
   Vec2Float(camvec, &camF);
   camPxActor->setPosition(camF[0], camF[1], camF[2]);
   
   WOpxObj_groundfloor->gScene->simulate(0.1);
   WOpxObj_groundfloor->gScene->fetchResults(true);
   physx::PxU32 numActors = 0;
   physx::PxActor** actors = this->WOpxObj_groundfloor->gScene->getActiveActors(numActors);
   
    std::vector <Vector> originalPos;
    originalPos.resize(numBoxes);
    for (int i = 0; i < numBoxes; i++) {
         originalPos[i] = physx_boxes[i]->getPosition();
    }
   
   for (physx::PxU32 i = 0; i < numActors; ++i) {
       physx::PxActor* actor = actors[i];
       //WOpxObj_ptr->updatePoseFromPhysx();
       for (int j = 0; j < numBoxes; j++) {
         physx_boxes[j]->updatePoseFromPhysx();
       }

       /*physx_boxes[1]->updatePoseFromPhysx();
       physx_boxes[2]->updatePoseFromPhysx();
       physx_boxes[3]->updatePoseFromPhysx();*/
       camPxActor->updatePoseFromPhysx();
   }

   
   cam->setPosition(camPxActor->getPosition());

   //for (int i = 0; i < numBoxes; i++) {
   //    if (originalPos[i] != physx_boxes[i]->getPosition()) {
 
   //        msg.WOindex = physx_boxes[i]->pxWO_ID;
   //        msg.trans_str[0] = physx_boxes[i]->getPose().toString();

   //        float locF[3];
   //        Vec2Float(physx_boxes[i]->getPosition(), &locF);
   //        msg.xPos = locF[0];
   //        msg.yPos = locF[1];
   //        msg.zPos = locF[2];
   //        msg.box_index = i;
   //        client->sendNetMsgSynchronousTCP(msg);
   //        //msg.onMessageArrived();
   //    }
   //}

   //v_camPos = this->cam->getPosition();
   //irrklang::vec3df v_camPosinirr(v_camPos.x, v_camPos.y, v_camPos.z);

   //v_lookDir = this->cam->getLookDirection();
   //irrklang::vec3df v_lookDirinirr(v_lookDir.y, v_lookDir.x, v_lookDir.z);

   //v_up = this ->cam->getNormalDirection();
   //irrklang::vec3df v_upinirr(v_up.x, v_up.y, v_up.z);

   //irrklang::vec3df veloc(0, 0, 0);
   //this->engine->setListenerPosition(v_camPosinirr, v_lookDirinirr, veloc, v_upinirr);
   //this->engine->setRolloffFactor(1);

   //this->worldLst->at(3)->moveRelative(Vector(-0.1, 0, 0));
   //
   //Aftr::Vector Barrel_move_V(0, 0, 0);
   //Barrel_move_V = this->worldLst->at(3)->getPosition();
   //irrklang::vec3d Barrel_move_Vinirr(Barrel_move_V.x, Barrel_move_V.y, Barrel_move_V.z);
   //
   //this->s_3d->setPosition(Barrel_move_Vinirr);
}


void GLViewProjectOne::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
   
}


void GLViewProjectOne::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewProjectOne::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewProjectOne::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewProjectOne::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      //this->setNumPhysicsStepsPerRender( 1 );

    
   //  barrel rotation added upon keypress '1' and '2'
   //
   //if( key.keysym.sym == SDLK_1 )
   //{
   //    this->worldLst->at(5)->rotateAboutGlobalX(0.08);
   //    this->worldLst->at(5)->rotateAboutGlobalY(0.08);
   //}

   if (key.keysym.sym == SDLK_2) 
   { 
     /*  this->worldLst->at(5)->rotateAboutGlobalX(-0.08);
       this->worldLst->at(5)->rotateAboutGlobalY(-0.08);*/

      //WOpxObj_ptr->gScene->simulate(0.1);
      //WOpxObj_ptr->gScene->fetchResults(true);
      //physx::PxU32 numActors = 0;
      //physx::PxActor** actors = this->WOpxObj_ptr->gScene->getActiveActors(numActors);
      //std::cout << std::endl << std::endl << "NUMBER OF ACTIVE ACTORS: " << numActors << std::endl << std::endl;
      //for (physx::PxU32 i = 0; i < numActors; ++i) {
      //    physx::PxActor* actor = actors[i];
      //    //WOpxObj_ptr->updatePoseFromPhysx();
      //    box->updatePoseFromPhysx();
      //    box2->updatePoseFromPhysx();
      //    box3->updatePoseFromPhysx();
      //    box4->updatePoseFromPhysx();
      //    camPxActor->updatePoseFromPhysx();
      //}

   }

   if (key.keysym.sym == SDLK_3) {

       aftrColor4f new_specular(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getSpecular());
       aftrColor4f new_ambient(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getAmbient());
       new_specular[3] -= 0.1f;
       new_ambient[3]  -= 0.1f;

       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setSpecular(new_specular);
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setAmbient(new_ambient);
   
   }
   if (key.keysym.sym == SDLK_4) {

       aftrColor4f new_specular(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getSpecular());
       aftrColor4f new_ambient(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getAmbient());
       new_specular[3] += 0.1f;
       new_ambient[3]  += 0.1f;
       
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setSpecular(new_specular);
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setAmbient(new_ambient);

   }


   if (key.keysym.sym == SDLK_5) {

       aftrColor4f new_specular(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getSpecular());
       aftrColor4f new_ambient(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getAmbient());
       new_specular[3] -= 0.1f;
       new_ambient[3]  -= 0.1f;

       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setSpecular(new_specular);
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setAmbient(new_ambient);

   }

   if (key.keysym.sym == SDLK_6) {

       aftrColor4f new_specular(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getSpecular());
       aftrColor4f new_ambient(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getAmbient());
       new_specular[3] += 0.1f;
       new_ambient[3]  += 0.1f;

       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setSpecular(new_specular);
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setAmbient(new_ambient);

   }


   if (key.keysym.sym == SDLK_7) {

       aftrColor4f new_specular(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).getSpecular());
       aftrColor4f new_ambient(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).getAmbient());
       new_specular[3] -= 0.1f;
       new_ambient[3] -= 0.1f;

       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).setSpecular(new_specular);
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).setAmbient(new_ambient);

   }
   if (key.keysym.sym == SDLK_8) {

       aftrColor4f new_specular(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).getSpecular());
       aftrColor4f new_ambient(this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).getAmbient());
       new_specular[3] += 0.1f;
       new_ambient[3]  += 0.1f;

       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).setSpecular(new_specular);
       this->worldLst->at(6)->getModel()->getModelDataShared()->getModelMeshes().at(2)->getSkins().at(0).setAmbient(new_ambient);

   }

   if (key.keysym.sym == SDLK_1) {


       //this->spawn_box();

       std::cout << std::endl << "heloo!!!!!!!!!" << std::endl;
       std::cout << std::endl << "heloo!!!!!!!!!" << std::endl;
       std::cout << std::endl << "heloo!!!!!!!!!" << std::endl;
       std::cout << std::endl << "heloo!!!!!!!!!" << std::endl;
       std::cout << std::endl << "heloo!!!!!!!!!" << std::endl;
       std::cout << std::endl << "heloo!!!!!!!!!" << std::endl;

       //ImGui::End();
       //static int z;

       //NetMsgCreateWO_2 msg;
       //msg.xPos = 100;
       //msg.yPos = 200;
       //msg.zPos = z * 10;
       //z++;

       //client->sendNetMsgSynchronousTCP(msg);
       //msg.onMessageArrived();
   }
}


void GLViewProjectOne::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewProjectOne::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst   = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 149,149, 0.5 );
   
   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   std::string barrel (ManagerEnvironmentConfiguration::getSMM() + "/models/barrel1.wrl" );

   std::string boxbox(ManagerEnvironmentConfiguration::getLMM() + "/models/fallout-new-vegas-destroyed-diner/source/PC Computer - Fallout New Vegas - Diner Destroyed/Diner/dinernosign.obj");

   std::string second_diner_texture(ManagerEnvironmentConfiguration::getSMM() + "/models/fallout-new-vegas-destroyed-diner/source/PC Computer - Fallout New Vegas - Diner Destroyed/Diner/edgetrim01_alpha.png");

   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 0, 0, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 1 ), this->getCameraPtrPtr() );
   wo->setPosition( 100,100,100 );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   WOPhysxGround* WOpxGround = WOPhysxGround::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   WOpxGround->setPosition(  0, 0, 0);
   WOpxGround->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin& grassSkin = WOpxGround->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   WOpxGround->setLabel( "Grass" );
   worldLst->push_back(WOpxGround);

   WOpxObj_groundfloor = static_cast<WOPhysxGround*>(WOpxGround->actor->userData);
   
   //// wo 3
   //WOPhysx* WOpxobj = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
   //WOpxobj->setPosition(49, 100, 1110);
   //WOpxobj->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   //WOpxobj->setLabel("FIRST BOX");
   //worldLst->push_back(WOpxobj);

   //physx_boxes.resize(physx_boxes.size()+1);
   //physx_boxes[0] = static_cast<WOPhysx*>(WOpxobj->actor->userData);
   //physx_boxes[0]->pxWO_ID = 3;

   //////std::cout << std::endl << worldLst->at(2)->getLabel() << std::endl;
   ////
   //// wo 4
   //WOpxobj = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
   //WOpxobj->setPosition(50, 100, 1115);
   //WOpxobj->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   //worldLst->push_back(WOpxobj);
   //
   //physx_boxes.resize(physx_boxes.size() + 1);
   //physx_boxes[1] = static_cast<WOPhysx*>(WOpxobj->actor->userData);
   //physx_boxes[1]->pxWO_ID = 4;

   //// wo 5
   //WOpxobj = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
   //WOpxobj->setPosition(52, 100, 1119);
   //WOpxobj->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   //worldLst->push_back(WOpxobj);

   //physx_boxes.resize(physx_boxes.size() + 1);
   //physx_boxes[2] = static_cast<WOPhysx*>(WOpxobj->actor->userData);
   //physx_boxes[2]->pxWO_ID = 5;

   //// wo 6
   //WOpxobj = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
   //WOpxobj->setPosition(52, 100, 1120);
   //WOpxobj->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   //worldLst->push_back(WOpxobj);

   //physx_boxes.resize(physx_boxes.size() + 1);
   //physx_boxes[3] = static_cast<WOPhysx*>(WOpxobj->actor->userData);
   //physx_boxes[3]->pxWO_ID = 6;

   //WOpxobj = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
   //WOpxobj->setPosition(52, 100, 1020);
   //WOpxobj->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   //worldLst->push_back(WOpxobj);

   ////physx_boxes.resize(physx_boxes.size() + 1);
   //physx_boxes.emplace_back( static_cast<WOPhysx*>(WOpxobj->actor->userData));
   ////physx_boxes[4]->pxWO_ID = 6;


   float camF[3];
   Vec2Float(cam->getPosition(), &camF);

   //worldLst->at(5)->setPosition(159, 159, 0.5);
   //worldLst->at(6)->setPosition(169, 169, 0.5);
   //worldLst->at(7)->setPosition(179, 179, 0.5);
   //worldLst->at(8)->setPosition(189, 189, 0.5);

   WOPhysx_cameraCollider *WOpxobj_cam = WOPhysx_cameraCollider::New(shinyRedPlasticCube, Vector(0.001, 0.001, 0.001), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
   WOpxobj_cam->setPosition(camF[0], camF[1], camF[2]);
   WOpxobj_cam->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   worldLst->push_back(WOpxobj_cam);
   aftrColor4f new_specular(-10.0f);
   aftrColor4f new_ambient(-10.0f);
   WOpxobj_cam->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setSpecular(new_specular);
   WOpxobj_cam->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setAmbient(new_ambient);

   camPxActor = static_cast<WOPhysx_cameraCollider*>(WOpxobj_cam->actor->userData);


   //random barrel models i added in for first module project
   wo = WO::New(barrel, Vector(1, 1, 1), MESH_SHADING_TYPE::mstSMOOTH);
   wo->setPosition(149,149,0.5);
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   wo->setLabel("barrel");
   worldLst->push_back(wo);

   wo = WO::New(barrel, Vector(1.5, 1.5, 1.5), MESH_SHADING_TYPE::mstFLAT);
   wo->setPosition(145, 145, 0.7);
   wo->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   worldLst->push_back(wo);
   
   wo = WO::New(barrel, Vector(1.5, 1.5, 1.5), MESH_SHADING_TYPE::mstFLAT);
   wo->setPosition(140, 140, 0.7);
   wo->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
   worldLst->push_back(wo);
   

   wo = WO::New(boxbox, Vector(0.01, 0.01, 0.01), MESH_SHADING_TYPE::mstAUTO);
   wo->setPosition(138, 130, 2.5);
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin dinerheightmap(ManagerTexture::loadTexture(second_diner_texture));
   dinerheightmap.setMeshShadingType(MESH_SHADING_TYPE::mstSMOOTH);
   dinerheightmap.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 0.4f));
   dinerheightmap.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 0));
   dinerheightmap.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 0));
   
   wo->getModel()->getModelDataShared()->getModelMeshes().at(3)->getSkin().setMeshShadingType(MESH_SHADING_TYPE::mstSMOOTH);

   worldLst->push_back(wo);

   WO_imgui* WOgui = WO_imgui::New(nullptr, 10.0, 10.0);
   //WOgui->physxWO_ptr = physx_boxes[0];
   WOgui->v = this;
   worldLst->push_back(WOgui);


   //physx_boxes.resize(physx_boxes.size() + 1);
   //physx_boxes.resize(physx_boxes.size() + 1);
   //this->spawn_box(WOpxGround);
   //this->spawn_box(WOpxGround);
   //this->spawn_box(WOpxGround);
   //this->spawn_box(WOpxGround);

   ////Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
   //wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
   //wo->setPosition( Vector(0,0,0) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   ////Create the infinite grass plane that uses NVIDIAPhysX(the floor)
   //wo = WONVStaticPlane::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0,0) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   //wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0,50.0f) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0.5f,75.0f) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue("sharedmultimediapath") + "/models/sphereRp5.wrl", Vector(1.0f, 1.0f, 1.0f), mstSMOOTH );
   //wo->setPosition( 0,0,100.0f );
   //wo->setLabel( "Sphere" );
   //this->worldLst->push_back( wo );

   //wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
   //((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
   //((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
   //wo->setPosition( Vector(20,20,20) );
   //wo->setLabel( "Paladin" );
   //worldLst->push_back( wo );
   //actorLst->push_back( wo );
   //netLst->push_back( wo );
   //this->setActor( wo );
   //
   //wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
   //wo->setPosition( Vector(20,10,20) );
   //wo->isVisible = false; //hide the WOHuman's bounding box
   //((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
   //wo->setLabel( "Human Cyborg" );
   //worldLst->push_back( wo );
   //actorLst->push_back( wo ); //Push the WOHuman as an actor
   //netLst->push_back( wo );
   //this->setActor( wo ); //Start module where human is the actor

   ////Create and insert the WOWheeledVehicle
   //std::vector< std::string > wheels;
   //std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
   //wo->setPosition( Vector( 5, -15, 20 ) );
   //wo->setLabel( "Car 1970s Beater" );
   //((WOODE*)wo)->mass = 200;
   //worldLst->push_back( wo );
   //actorLst->push_back( wo );
   //this->setActor( wo );
   //netLst->push_back( wo );

   createProjectOneWayPoints();

   //audioContainer();

   client = NetMessengerClient::New("127.0.0.1", "12683");

}


void GLViewProjectOne::createProjectOneWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}


void GLViewProjectOne::spawn_box() {

    std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");

    float camPosF[3];

    Vector distance(20, 20, 40);

    Vector spawnPosition = this->cam->getPosition() + (this->cam->getLookDirection() * distance);

    Vec2Float(spawnPosition, &camPosF);
        
    WOPhysx* WOpxobj_spawn = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxObj_groundfloor->gScene);
    WOpxobj_spawn->setPosition(camPosF[0], camPosF[1], camPosF[2]+5);

    WOpxobj_spawn->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
    WOpxobj_spawn->setLabel("red box");
    this->worldLst->push_back(WOpxobj_spawn);
    WOID++;


    unsigned int numBoxes = this->physx_boxes.size();
    this->physx_boxes.resize(numBoxes+1);
    
    physx_boxes[physx_boxes.size()-1] = static_cast<WOPhysx*>(WOpxobj_spawn->actor->userData);
    physx_boxes[physx_boxes.size()-1]->pxWO_ID = WOID;
 }

//void GLViewProjectOne::audioContainer() {
//    
//    std::string sound4(ManagerEnvironmentConfiguration::getSMM() + "/sounds/sound4.wav");
//    char* cstr = new char[sound4.length() + 1];
//    std::strcpy(cstr, sound4.c_str());
//
//    std::string sound3D(ManagerEnvironmentConfiguration::getSMM() + "/sounds/sound5.wav");
//    
//      
//    //ISoundEngine* engine = createIrrKlangDevice();
//    //ISound* s = engine->play2D(cstr, true);
//
//    //ISoundEngine* engine3D = createIrrKlangDevice();
//    
//    irrklang::vec3df b_position(149, 149, 0.5);
//    
//    s_3d = engine->play3D(cstr, b_position, true, false, true);
//
//    irrklang::vec3df veloc(-10, 0, 0);
//
//    engine->setDopplerEffectParameters(1.0);
//
//    s_3d->setVelocity(veloc);
//}

void GLViewProjectOne::Vec2Float(Vector vec, float (*camF)[3]) {
    (*camF)[0] = vec.x;
    (*camF)[1] = vec.y;
    (*camF)[2] = vec.z;
}