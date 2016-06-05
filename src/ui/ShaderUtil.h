#ifndef UI_SHADERUTIL_H
#define UI_SHADERUTIL_H

#include <string>
#include <vector>

#include <GL/glew.h>

typedef std::pair<std::string, GLenum> ShaderInfo;

class ShaderUtil
{
 public:
  
  static GLuint CreateShaderProgram(const std::vector<ShaderInfo> &shaders);

  static std::string CheckProgramInfo(GLuint program_ID, GLenum status_type = GL_LINK_STATUS);
  
  static std::string CheckShaderInfo(GLuint shader_ID, GLenum status_type = GL_COMPILE_STATUS);
  
 private:
  
  static GLuint PrepareShader(const std::string &source_file_path, GLenum shader_type);

  ShaderUtil();
};


#endif //UI_SHADER_H
