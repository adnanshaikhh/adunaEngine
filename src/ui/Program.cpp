#include "Program.h"
#include <GL/glew.h>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

Program::Program(const std::vector<ShaderInfo> &shaders):
  program_ID_(0)
{
  program_ID_ = ShaderUtil::CreateShaderProgram(shaders);

}

Program::~Program()
{
  if(program_ID_ != 0)
    {
      glDeleteProgram(program_ID_);
    }
}

GLuint Program::GetProgramID() const
{
    return program_ID_;
}

void Program::Use() const
{
    glUseProgram(program_ID_);
}

bool Program::IsInUse() const
{
    GLint current_program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    return (current_program == (GLint)program_ID_);
}

void Program::StopUsing() const
{
    assert(IsInUse());
    glUseProgram(0);
}

GLint Program::GetAttribLocation(const GLchar* attrib_name) const
{
  if(!attrib_name)
    throw std::runtime_error("attrib_name was NULL");
    
  GLint attrib = glGetAttribLocation(program_ID_, attrib_name);
  
  if(attrib == -1)
    throw std::runtime_error(std::string("Program attribute not found: ") + attrib_name);
    
  return attrib;
  
}

GLint Program::GetUniformLocation(const GLchar* uniform_name) const
{
    if(!uniform_name)
        throw std::runtime_error("uniform_name was NULL");
    
    GLint uniform = glGetUniformLocation(program_ID_, uniform_name);
    if(uniform == -1)
        throw std::runtime_error(std::string("Program uniform not found: ") + uniform_name);
    
    return uniform;
}

#define ATTRIB_N_UNIFORM_SETTERS(OGL_TYPE, TYPE_PREFIX, TYPE_SUFFIX) \
\
    void Program::SetAttrib(const GLchar* name, OGL_TYPE v0) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX (GetAttribLocation(name), v0); } \
    void Program::SetAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX (GetAttribLocation(name), v0, v1); } \
    void Program::SetAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX (GetAttribLocation(name), v0, v1, v2); } \
    void Program::SetAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX (GetAttribLocation(name), v0, v1, v2, v3); } \
\
    void Program::SetAttrib1v(const GLchar* name, const OGL_TYPE* v) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX ## v (GetAttribLocation(name), v); } \
    void Program::SetAttrib2v(const GLchar* name, const OGL_TYPE* v) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX ## v (GetAttribLocation(name), v); } \
    void Program::SetAttrib3v(const GLchar* name, const OGL_TYPE* v) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX ## v (GetAttribLocation(name), v); } \
    void Program::SetAttrib4v(const GLchar* name, const OGL_TYPE* v) \
        { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX ## v (GetAttribLocation(name), v); } \
\
    void Program::SetUniform(const GLchar* name, OGL_TYPE v0) \
        { assert(IsInUse()); glUniform1 ## TYPE_SUFFIX (GetUniformLocation(name), v0); } \
    void Program::SetUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
        { assert(IsInUse()); glUniform2 ## TYPE_SUFFIX (GetUniformLocation(name), v0, v1); } \
    void Program::SetUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
        { assert(IsInUse()); glUniform3 ## TYPE_SUFFIX (GetUniformLocation(name), v0, v1, v2); } \
    void Program::SetUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
        { assert(IsInUse()); glUniform4 ## TYPE_SUFFIX (GetUniformLocation(name), v0, v1, v2, v3); } \
\
    void Program::SetUniform1v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
        { assert(IsInUse()); glUniform1 ## TYPE_SUFFIX ## v (GetUniformLocation(name), count, v); } \
    void Program::SetUniform2v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
        { assert(IsInUse()); glUniform2 ## TYPE_SUFFIX ## v (GetUniformLocation(name), count, v); } \
    void Program::SetUniform3v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
        { assert(IsInUse()); glUniform3 ## TYPE_SUFFIX ## v (GetUniformLocation(name), count, v); } \
    void Program::SetUniform4v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
        { assert(IsInUse()); glUniform4 ## TYPE_SUFFIX ## v (GetUniformLocation(name), count, v); }

ATTRIB_N_UNIFORM_SETTERS(GLfloat, , f);
ATTRIB_N_UNIFORM_SETTERS(GLdouble, , d);
ATTRIB_N_UNIFORM_SETTERS(GLint, I, i);
ATTRIB_N_UNIFORM_SETTERS(GLuint, I, ui);



void Program::SetUniformMatrix2(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    assert(IsInUse());
    glUniformMatrix2fv(GetUniformLocation(name), count, transpose, v);
}

void Program::SetUniformMatrix3(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    assert(IsInUse());
    glUniformMatrix3fv(GetUniformLocation(name), count, transpose, v);
}

void Program::SetUniformMatrix4(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    assert(IsInUse());
    glUniformMatrix4fv(GetUniformLocation(name), count, transpose, v);
}

void Program::SetUniform(const GLchar* name, const glm::mat2& m, GLboolean transpose) {
    assert(IsInUse());
    glUniformMatrix2fv(GetUniformLocation(name), 1, transpose, glm::value_ptr(m));
}

void Program::SetUniform(const GLchar* name, const glm::mat3& m, GLboolean transpose) {
    assert(IsInUse());
    glUniformMatrix3fv(GetUniformLocation(name), 1, transpose, glm::value_ptr(m));
}

void Program::SetUniform(const GLchar* name, const glm::mat4& m, GLboolean transpose) {
    assert(IsInUse());
    glUniformMatrix4fv(GetUniformLocation(name), 1, transpose, glm::value_ptr(m));
}

void Program::SetUniform(const GLchar* uniformName, const glm::vec3& v) {
    SetUniform3v(uniformName, glm::value_ptr(v));
}

void Program::SetUniform(const GLchar* uniformName, const glm::vec4& v) {
    SetUniform4v(uniformName, glm::value_ptr(v));
}


