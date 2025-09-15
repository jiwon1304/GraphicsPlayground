#include "OpenGLResources.h"

FOpenGLUniformBuffer::FOpenGLUniformBuffer(const FRHIUniformBufferLayout *InLayout)
    : FRHIUniformBuffer(InLayout)
    , Size(InLayout ? InLayout->GetSize() : 0)
{
    if (Size > 0)
    {
        FOpenGL::GenBuffers(1, &Resource);
        FOpenGL::BindBuffer(GL_UNIFORM_BUFFER, Resource);
        FOpenGL::BufferData(GL_UNIFORM_BUFFER, Size, nullptr, GL_DYNAMIC_DRAW);
    }
}