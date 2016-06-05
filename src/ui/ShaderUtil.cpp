#include "ShaderUtil.h"

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <GL/glew.h>


#include "../util/FileUtil.h"


GLuint ShaderUtil::CreateShaderProgram(const std::vector< std::pair<std::string, GLenum> > &shaders)
{

  if(shaders.size() <= 0)
    {
      throw std::runtime_error("No shaders were provided to create the program");
    }
  
  GLuint program_ID = glCreateProgram();

  if(program_ID == 0)
    {
      throw std::runtime_error("glCreateProgram failed");
    }
  
  std::vector<GLuint> shader_IDs;

  for(size_t shader_index = 0; shader_index < shaders.size(); ++shader_index)
    {
      GLuint shader_ID = PrepareShader(shaders[shader_index].first,
				       shaders[shader_index].second);
      glAttachShader(program_ID, shader_ID);
      std::cout << shaders[shader_index].first << " Attached!" << std::endl;
      shader_IDs.push_back(shader_ID);
    }

  glLinkProgram(program_ID);

  std::string link_info = CheckProgramInfo(program_ID, GL_LINK_STATUS);
  if(!link_info.empty())
    {
      std::cerr << link_info << std::endl;
    }
  std::cout << "Progam Linked!" << std::endl;

  //Delete shaders as they are no longer necessary after being linked into program
  for(size_t i = 0; i < shader_IDs.size(); ++i)
    {
      glDeleteShader(shader_IDs[i]);
    }

  return program_ID;
}


std::string ShaderUtil::CheckProgramInfo(GLuint program_ID, GLenum status_type)
{
  GLint result = GL_TRUE;
  glGetProgramiv(program_ID, status_type, &result);
  if (GL_FALSE != result)
    {
      return std::string("");
    }
  else
    {
      std::stringstream ss;
      ss << "Program error ocurred\n";
      GLint info_log_len;
      glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_len);
      if (info_log_len > 0)
	{
	  std::unique_ptr<char[]> log(new char[info_log_len]);
	  GLsizei written;
	  glGetProgramInfoLog(program_ID, info_log_len, &written, log.get());
	  ss << log.get() << std::endl;
	}
      return ss.str();
    }
}

std::string ShaderUtil::CheckShaderInfo(GLuint shader_ID, GLenum status_type)
{
  GLint result = GL_TRUE;
  glGetShaderiv(shader_ID, status_type, &result);
  if (GL_FALSE != result)
    {
      return std::string("");
    }
  else
    {
      std::stringstream ss;
      ss << "Shader error occurred\n";
      GLint info_log_len;
      glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_log_len);
      if (info_log_len > 0)
	{
	  std::unique_ptr<char[]> log(new char[info_log_len]);
	  GLsizei written;
	  glGetShaderInfoLog(shader_ID, info_log_len, &written, log.get());
	  ss << log.get() << std::endl;
	}
      return ss.str();
    }
}



GLuint ShaderUtil::PrepareShader(const std::string &source_file_path, GLenum shader_type)
{
  std::string source = FileUtil::read(source_file_path);
  const GLchar* shader_source = source.c_str();

  GLuint shader_ID = glCreateShader(shader_type);
  if(shader_ID == 0)
    {
      throw std::runtime_error("glCreateShader failed");
    }

  glShaderSource(shader_ID, 1, &shader_source, NULL);
  glCompileShader(shader_ID);

  //Check for Errors
  std::string compile_info = CheckShaderInfo(shader_ID, GL_COMPILE_STATUS);
  if(!compile_info.empty())
    {
      std::cerr << compile_info << std::endl;
    }

  return shader_ID;

}

