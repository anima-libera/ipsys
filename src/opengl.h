
#ifdef USE_GLEW
#include <GL/glew.h>
#else
#ifndef IPSYS_HEADER_OPENGL__
#define IPSYS_HEADER_OPENGL__

#include <GL/glx.h>
#include <GL/gl.h>

#define GL_VERSION_4_3 1
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

#endif /* IPSYS_HEADER_OPENGL__ */
#endif
