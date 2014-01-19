//
//  environment.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/19/14.
//
//

#ifndef __churchOfRobotronVR__environment__
#define __churchOfRobotronVR__environment__

#include "cinder/TriMesh.h"

class Environment
{
public:
  void init();
  void update();
  void draw();
private:
  cinder::TriMesh mGroundMesh;
  cinder::TriMesh mSkyMesh;
  
  void initGroundMesh();
  void initSkyMesh();
};

#endif /* defined(__churchOfRobotronVR__environment__) */
