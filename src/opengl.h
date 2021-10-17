
#ifndef IPSYS_HEADER_OPENGL__
#define IPSYS_HEADER_OPENGL__

#ifdef USE_GLEW

	#include <GL/glew.h>

#else

	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glx.h>

#endif

#endif /* IPSYS_HEADER_OPENGL__ */
