#include "Texture.h"

#include <stdexcept>

#include <GL/glew.h>


static GLenum TextureFormatForBitmapFormat(Bitmap::Format format, bool srgb)
{
    switch (format) {
        case Bitmap::Format_Grayscale: return GL_LUMINANCE;
        case Bitmap::Format_GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
        case Bitmap::Format_RGB: return (srgb ? GL_SRGB : GL_RGB);
        case Bitmap::Format_RGBA: return (srgb ? GL_SRGB_ALPHA : GL_RGBA);
        default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

Texture::Texture(const Bitmap& bitmap, GLint min_mag_filter, GLint wrap_mode) :
    original_width_((GLfloat)bitmap.width()),
    original_height_((GLfloat)bitmap.height())
{
    glGenTextures(1, &texture_object_);
    glBindTexture(GL_TEXTURE_2D, texture_object_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 
                 TextureFormatForBitmapFormat(bitmap.format(), true),
                 (GLsizei)bitmap.width(), 
                 (GLsizei)bitmap.height(),
                 0, 
                 TextureFormatForBitmapFormat(bitmap.format(), false), 
                 GL_UNSIGNED_BYTE, 
                 bitmap.pixelBuffer());
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture_object_);
}

GLuint Texture::GetTextureID() const
{
    return texture_object_;
}

GLfloat Texture::OriginalWidth() const
{
    return original_width_;
}

GLfloat Texture::OrginialHeight() const
{
    return original_height_;
}
