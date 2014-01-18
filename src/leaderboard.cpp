//
//  leaderboard.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/14/14.
//
//

#include "leaderboard.h"
#include "cinder/text.h"
#include "cinder/gl/gl.h"
#include "cinder/font.h"

#include "MeshHelper.h"

#include "churchUtil.h"

using namespace cinder;

void Leaderboard::init()
{
  TriMesh cube = MeshHelper::createCube();
  
  TextLayout tl;
  tl.setFont(Font("Data70EF", 32));
  tl.setColor(Color::white());
  tl.addLine("BTR");
  Surface s = tl.render(true, true);
  mMesh.clear();
  float xSize = 0.125f;
  float ySize = 0.125f;
  for (int y = 0; y < s.getHeight(); y++)
  {
    for (int x = 0; x < s.getWidth(); x++)
    {
      uint8_t* red = s.getData(Vec2i(x, y));
      int value = *red > 0 ? 1 : 0;
      if (value)
      {
        size_t lastIndex = mMesh.getNumIndices();
        Vec3f offset(x * xSize, 0.0, (s.getHeight() - y) * ySize);
        for (auto v : cube.getVertices())
          mMesh.appendVertex((v * xSize * 0.75) + offset);
        mMesh.appendNormals(&cube.getNormals()[0], cube.getNormals().size());
        const auto& indices = cube.getIndices();
        for (int i = 0; i < indices.size(); i+=3)
        {
          mMesh.appendTriangle(indices[i+0] + lastIndex,
                               indices[i+1] + lastIndex,
                               indices[i+2] + lastIndex);
        }
      }
    }
  }
}

void Leaderboard::draw()
{
  gl::pushMatrices();
  gl::translate(Vec3f(-6.0f, 18.0f, -3.0));
  gl::draw(mMesh);
  gl::popMatrices();
}
