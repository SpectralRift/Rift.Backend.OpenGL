#pragma once

#ifdef GL_WITH_LOADER
#include <glad/gl.h>
#else
#if defined(GL_WITH_GLES) && !defined(GL_FORCE_API)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif