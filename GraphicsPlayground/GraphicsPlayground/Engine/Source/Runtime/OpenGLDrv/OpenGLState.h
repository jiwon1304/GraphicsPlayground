#pragma once
#include "ThirdParty/OpenGL/glad/include/glad/glad.h"

struct FOpenGLRHIState
{
    GLuint VertexArrayId;
    GLuint FramebufferId;
    GLuint RenderbufferId;
    GLuint ProgramId;
    GLuint TextureId;
    GLuint SamplerId;

    GLenum CurrentPrimitiveType = 0;
};
