#pragma once
#include "OpenGLDrv/OpenGLThirdParty.h"

#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "Core/HAL/PlatformType.h"
// 그냥 사용하기

namespace OpenGL3
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