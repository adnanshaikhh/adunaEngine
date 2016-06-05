#ifndef UI_TEXTURE_H
#define UI_TEXTURE_H

#include <GL/glew.h>

#include "Bitmap.h"



class Texture
{
 public:
  Texture(const Bitmap& bitmap, GLint min_mag_filter = GL_LINEAR,
	  GLint wrap_mode = GL_CLAMP_TO_EDGE);

  ~Texture();

  GLuint GetTextureID() const;

  GLfloat OriginalWidth() const;

  GLfloat OrginialHeight() const;

 private:

  GLuint texture_object_;
  GLfloat original_width_;
  GLfloat original_height_;
  
  //copying disabled
  //Texture(const Texture&);
  //const Texture& operator=(const Texture&);
};


#endif //UI_TEXTURE_H
