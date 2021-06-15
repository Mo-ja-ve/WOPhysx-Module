#pragma once

#include "GLView.h"
//#include "irrKlang.h"
#include "NetMsg.h"
#include "NetMessengerClient.h"
#include "NetMsgCreateWO_2.h"

#include "WOPhysx.h"
#include "WOPhysxGround.h"


namespace Aftr
{
   class Camera;

/**
   \class GLViewProjectOne
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewProjectOne : public GLView
{
public:
   static GLViewProjectOne* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewProjectOne();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createProjectOneWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

   WOPhysxGround* WOpxObj_ptr;
   WOPhysx* box;
   WOPhysx* box2;


   NetMessengerClient* client;
  
   
protected:
   GLViewProjectOne( const std::vector< std::string >& args );
   virtual void onCreate();
};

/** \} */

} //namespace Aftr
