//
//  robotronScreen.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 11/26/13.
//
//

#ifndef __churchOfRobotronVR__robotronScreen__
#define __churchOfRobotronVR__robotronScreen__

#include "syphonClient.h"

class RobotronScreen
{
public:
  void init();
  void draw();
private:
  syphonClient mClient;
};


#endif /* defined(__churchOfRobotronVR__robotronScreen__) */
