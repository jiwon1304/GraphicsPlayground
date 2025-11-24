#pragma once

struct GLFWwindow;
namespace FOpenGL 
{
    using Window = GLFWwindow;

    typedef          float         khronos_float_t;
    typedef signed   char          khronos_int8_t;
    typedef unsigned char          khronos_uint8_t;
    typedef signed   short int     khronos_int16_t;
    typedef unsigned short int     khronos_uint16_t;
#ifdef _WIN64
    typedef signed   long long int khronos_intptr_t;
    typedef signed   long long int khronos_ssize_t;
#else
    typedef signed   long  int     khronos_intptr_t;
    typedef signed   long  int     khronos_ssize_t;
#endif
    // lightweight GL typedefs (avoid including <GL/gl.h> here)
    using GLuint = unsigned int;
    using GLenum = unsigned int;
    using GLsizei = int;
    using GLint = int;
    using GLboolean = unsigned char;
    using GLfloat = float;
    using GLsizeiptr = khronos_ssize_t;
    using GLintptr = khronos_intptr_t;

    // GLFW-like callback typedefs (match usage in OpenGL3.h)
    using GLFWerrorfun = void(*)(int, const char*);
    using WindowSizeCallback = void(*)(Window*, int, int);
    using GLFWkeyfun = void(*)(Window*, int, int, int, int);
    using GLFWcharfun = void(*)(Window*, unsigned int);
    using GLFWmousebuttonfun = void(*)(Window*, int, int, int);
    using GLFWcursorposfun = void(*)(Window*, double, double);
    using GLFWscrollfun = void(*)(Window*, double, double);

    // Core
    int Init();
    void SetErrorCallback(GLFWerrorfun callback);
    void WindowHint(int hint, int value);
    Window* CreateWindowGLFW(int width, int height, const char* title);
    void MakeContextCurrent(Window* window);
    int LoadGLLoader();
    void DestroyWindow(Window* window);
    void Terminate();
    void Flush();
    void Finish();

    // Buffers
    void GenBuffers(GLsizei n, GLuint* buffers);
    void DeleteBuffers(GLsizei n, const GLuint* buffers);
    void BindBuffer(GLenum target, GLuint buffer);
    void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);

    // Textures
    void GenTextures(GLsizei n, GLuint* textures);
    void DeleteTextures(GLsizei n, const GLuint* textures);
    void BindTexture(GLenum target, GLuint texture);

    // Samplers
    void GenSamplers(GLsizei n, GLuint* samplers);
    void DeleteSamplers(GLsizei n, const GLuint* samplers);
    void BindSampler(GLuint unit, GLuint sampler);
    void SetSamplerParameter(GLuint sampler, GLenum pname, GLint param);
    void SetSamplerParameter(GLuint sampler, GLenum pname, GLfloat param);

    // Framebuffers
    void GenFramebuffers(GLsizei n, GLuint* framebuffers);
    void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
    void BindFramebuffer(GLenum target, GLuint framebuffer);

    // Viewport
    void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);

    // Vertex arrays / attributes
    void GenVertexArrays(GLsizei n, GLuint* arrays);
    void DeleteVertexArrays(GLsizei n, const GLuint* arrays);
    void BindVertexArray(GLuint array);
    void EnableVertexAttribArray(GLuint index);
    void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

    // Shaders / Programs
    GLuint CreateShader(GLenum ShaderType, const char* Code);
    GLuint CreateProgram(GLuint VertexShader, GLuint FragmentShader, GLuint GeometryShader);
    void DeleteShader(GLuint Shader);
    void DeleteProgram(GLuint Program);
    void UseProgram(GLuint Program);

    // Input / callbacks
    void PollEvents();
    bool ShouldClose(Window* window);
    void SetWindowUserPointer(Window* Window, void* Pointer);
    void* GetWindowUserPointer(Window* Window);
    void SetWindowSizeCallback(Window* Window, WindowSizeCallback Callback);
    void SetKeyCallback(Window* Window, GLFWkeyfun Callback);
    void SetCharCallback(Window* Window, GLFWcharfun Callback);
    void SetMouseButtonCallback(Window* Window, GLFWmousebuttonfun Callback);
    void SetCursorPosCallback(Window* Window, GLFWcursorposfun Callback);
    void SetScrollCallback(Window* Window, GLFWscrollfun Callback);
    int RawMouseMotionSupported();
    void SetInputMode(Window* Window, int mode, int value);

    // Window helpers
    void SetWindowPos(Window* window, int x, int y);
    void GetWindowPos(Window* window, int* x, int* y);
    void SetWindowSize(Window* window, int width, int height);
    void GetWindowSize(Window* window, int* width, int* height);
    void ShowWindow(Window* window);
    void RestoreWindow(Window* window);
    void FocusWindow(Window* window);
    bool IsWindowFocused(Window* window);
}

// Shader / program
class FOpenGLLinkedProgram;
class FOpenGLShader;
class FOpenGLVertexShader;
class FOpenGLPixelShader;
class FOpenGLGeometryShader;
class FOpenGLComputeShader;

// Buffers
class FOpenGLBufferBase;
class FOpenGLBaseBuffer;
template <typename BaseType, typename BufferBindFunc> class TOpenGLBuffer;
class FOpenGLUniformBuffer;
class FOpenGLStagingBuffer;

// Vertex/decl
struct FOpenGLVertexElement;
class FOpenGLVertexDeclaration;

// Bound shader state
class FOpenGLBoundShaderState;

// Textures / views
class FOpenGLTexture;
class FOpenGLTextureDesc;
class FOpenGLTextureCreateDesc;
class FOpenGLUnorderedAccessView;
class FOpenGLShaderResourceView;

// Viewport / context
class FOpenGLViewport;

// State objects
class FOpenGLSamplerState;
class FOpenGLRasterizerState;
class FOpenGLDepthStencilState;
class FOpenGLBlendState;

// Misc
template <class T> struct TOpenGLResourceTraits;