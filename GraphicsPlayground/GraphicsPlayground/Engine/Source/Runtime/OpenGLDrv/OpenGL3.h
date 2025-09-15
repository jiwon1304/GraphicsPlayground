#pragma once
#include "OpenGLDrv/OpenGLThirdParty.h"

#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "Core/HAL/PlatformType.h"
// 그냥 사용하기

namespace FOpenGL
{
int Init()
{
    return glfwInit();
}

void WindowHint(int hint, int value)
{
    glfwWindowHint(hint, value);
}

GLFWwindow* CreateWindow(int width, int height, const char* title)
{
    return glfwCreateWindow(width, height, title, NULL, NULL);
}

void MakeContextCurrent(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
}

void SetInputMode(GLFWwindow* window, int mode, int value)
{
    glfwSetInputMode(window, mode, value);
}

int LoadGLLoader()
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void DestroyWindow(GLFWwindow* window)
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

void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
}

void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
    glBufferSubData(target, offset, size, data);
}

void EnableVertexAttribArray(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void BindVertexArray(GLuint array)
{
    glBindVertexArray(array);
}

};