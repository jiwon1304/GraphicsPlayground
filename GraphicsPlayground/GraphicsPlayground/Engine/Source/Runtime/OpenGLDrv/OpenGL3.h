#pragma once
#include "OpenGLDrv/OpenGLThirdParty.h"
#include "OpenGLDrv/OpenGLFwd.h"

#include "Core/HAL/PlatformType.h"

namespace FOpenGL
{
int Init()
{
    return glfwInit();
}

void SetErrorCallback(GLFWerrorfun callback)
{
    glfwSetErrorCallback(callback);
}

void WindowHint(int hint, int value)
{
    glfwWindowHint(hint, value);
}

Window* CreateWindowGLFW(int width, int height, const char* title)
{
    return glfwCreateWindow(width, height, title, NULL, NULL);
}

void MakeContextCurrent(Window* window)
{
    glfwMakeContextCurrent(window);
}

void SetInputMode(Window* Window, int mode, int value)
{
    glfwSetInputMode(Window, mode, value);
}

int LoadGLLoader()
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void DestroyWindow(Window* window)
{
    glfwDestroyWindow(window);
}

void Terminate()
{
    glfwTerminate();
}

void Flush()
{
    glFlush();
}

void Finish()
{
    glFinish();
}

// -------------------------------------------------------------
// Buffers
// --------------------------------------------------------------
void GenBuffers(GLsizei n, GLuint* buffers)
{
    glGenBuffers(n, buffers);
}

void DeleteBuffers(GLsizei n, const GLuint* buffers)
{
    glDeleteBuffers(n, buffers);
}

void BindBuffer(GLenum target, GLuint buffer)
{
    glBindBuffer(target, buffer);
}

void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
}

void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
    glBufferSubData(target, offset, size, data);
}

// -------------------------------------------------------------
// Textures
// --------------------------------------------------------------
void GenTextures(GLsizei n, GLuint* textures)
{
    glGenTextures(n, textures);
}

void DeleteTextures(GLsizei n, const GLuint* textures)
{
    glDeleteTextures(n, textures);
}

void BindTexture(GLenum target, GLuint texture)
{
    glBindTexture(target, texture);
}

// -------------------------------------------------------------
// Samplers
// --------------------------------------------------------------
void GenSamplers(GLsizei n, GLuint* samplers)
{
    glGenSamplers(n, samplers);
}

void DeleteSamplers(GLsizei n, const GLuint* samplers)
{
    glDeleteSamplers(n, samplers);
}

void BindSampler(GLuint unit, GLuint sampler)
{
    glBindSampler(unit, sampler);
}

void SetSamplerParameter(GLuint sampler, GLenum pname, GLint param)
{
    glSamplerParameteri(sampler, pname, param);
}

void SetSamplerParameter(GLuint sampler, GLenum pname, GLfloat param)
{
    glSamplerParameterf(sampler, pname, param);
}

// -------------------------------------------------------------
// Framebuffers
// --------------------------------------------------------------
void GenFramebuffers(GLsizei n, GLuint* framebuffers)
{
    glGenFramebuffers(n, framebuffers);
}

void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
    glDeleteFramebuffers(n, framebuffers);
}

void BindFramebuffer(GLenum target, GLuint framebuffer)
{
    glBindFramebuffer(target, framebuffer);
}

// -------------------------------------------------------------
// Viewports
// --------------------------------------------------------------
void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

// -------------------------------------------------------------
// Vertex Elements
// --------------------------------------------------------------
void GenVertexArrays(GLsizei n, GLuint* arrays)
{
    glGenVertexArrays(n, arrays);
}

void DeleteVertexArrays(GLsizei n, const GLuint* arrays)
{
    glDeleteVertexArrays(n, arrays);
}

void BindVertexArray(GLuint array)
{
    glBindVertexArray(array);
}

void EnableVertexAttribArray(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

// -------------------------------------------------------------
// Shaders
// --------------------------------------------------------------
GLuint CreateShader(GLenum ShaderType, const char* Code)
{
    GLuint Shader = glCreateShader(ShaderType);
    glShaderSource(Shader, 1, &Code, NULL);
    glCompileShader(Shader);
    
    // Check compilation status
    GLint Success;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if (!Success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
        std::fprintf(stderr, "Error compiling shader: %s\n", InfoLog);
        glDeleteShader(Shader);
        return 0;
    }

    return Shader;
}

GLuint CreateProgram(GLuint VertexShader, GLuint FragmentShader, GLuint GeometryShader = 0)
{
    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    if (GeometryShader != 0)
    {
        glAttachShader(Program, GeometryShader);
    }
    glLinkProgram(Program);

    // Check linking status
    GLint Success;
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    if (!Success)
    {
        GLchar InfoLog[1024];
        glGetProgramInfoLog(Program, 1024, NULL, InfoLog);
        std::fprintf(stderr, "Error linking program: %s\n", InfoLog);
        glDeleteProgram(Program);
        return 0;
    }

    // Detach shaders after successful link
    glDetachShader(Program, VertexShader);
    glDetachShader(Program, FragmentShader);
    if (GeometryShader != 0)
    {
        glDetachShader(Program, GeometryShader);
    }

    return Program;
};

void DeleteShader(GLuint Shader)
{
    glDeleteShader(Shader);
}

void DeleteProgram(GLuint Program)
{
    glDeleteProgram(Program);
}

void UseProgram(GLuint Program)
{
    glUseProgram(Program);
}

// -------------------------------------------------------------
// Input
// --------------------------------------------------------------
void PollEvents()
{
    glfwPollEvents();
}

void SetWindowUserPointer(Window* Window, void* Pointer)
{
    glfwSetWindowUserPointer(Window, Pointer);
}

void SetKeyCallback(Window* Window, GLFWkeyfun Callback)
{
    glfwSetKeyCallback(Window, Callback);
}

void SetCharCallback(Window* Window, GLFWcharfun Callback)
{
    glfwSetCharCallback(Window, Callback);
}

void SetMouseButtonCallback(Window* Window, GLFWmousebuttonfun Callback)
{
    glfwSetMouseButtonCallback(Window, Callback);
}

void SetCursorPosCallback(Window* Window, GLFWcursorposfun Callback)
{
    glfwSetCursorPosCallback(Window, Callback);
}

void SetScrollCallback(Window* Window, GLFWscrollfun Callback)
{
    glfwSetScrollCallback(Window, Callback);
}

int RawMouseMotionSupported()
{
    return glfwRawMouseMotionSupported();
}

void SetInputMode(Window* Window, int mode, int value)
{
    glfwSetInputMode(Window, mode, value);
}

}