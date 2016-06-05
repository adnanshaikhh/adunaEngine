#ifndef UTIL_OBJLOADER_H
#define UTIL_OBJLOADER_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ObjLoader
{
 public:
  static bool LoadObj (const std::string file_name,
		       std::vector <glm::vec3> &out_vertices,
		       std::vector <glm::vec2> &out_tex_coord,
		       std::vector <glm::vec3> &out_normals);

  static GLfloat * LoadObjData(const std::string file_name);
};

#endif
