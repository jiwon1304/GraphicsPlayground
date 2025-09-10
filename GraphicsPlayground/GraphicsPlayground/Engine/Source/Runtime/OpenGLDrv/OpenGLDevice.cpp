#include "OpenGLDrv.h"

#include "Core/Misc/CoreGlobals.h"

#include "OpenGLDrv/OpenGL3.h"
#include "OpenGLDrv/OpenGLHelper.h"

// @todo : d3d11이랑 맞추도록 공통된 헤더 만들기
using namespace OpenGL3;

GLFWwindow* InitGLFW()
{
    static bool bInitialized = false;
    assert(!bInitialized);
    assert(glfwInit());
    bInitialized = true;

    WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    WindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
#endif
    GLFWwindow* Window = CreateWindow(GWindowWidth, GWindowHeight, GWindowName);
    assert(Window);

    MakeContextCurrent(Window);
    SetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    assert(LoadGLLoader());
}

void ShutdownGLFW(GLFWwindow* Window)
{
    DestroyWindow(Window);
    Terminate();
}

FOpenGLDynamicRHI::FOpenGLDynamicRHI()
{
    assert(Singleton == nullptr);
    Singleton = this;
}

FOpenGLDynamicRHI::~FOpenGLDynamicRHI()
{
    assert(Singleton == this);
    Singleton = nullptr;
}

void FOpenGLDynamicRHI::Init()
{
    MainWindow = InitGLFW();
}

void FOpenGLDynamicRHI::Shutdown()
{
    ShutdownGLFW(MainWindow);
}

void FOpenGLDynamicRHI::RHITick(float DeltaTime)
{
}

void FOpenGLDynamicRHI::RHIExecuteCommandList(FRHICommandList *CmdList)
{
}

// -----------------------------------------------------------------------------
// Render Passes
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName)
{
    // 순수 가상(pure virtual)로 선언되어 있어서 실제 구현 파일 또는 다른 클래스로 분리할 수도 있음.
    // 여기선 예시. (헤더에 =0 으로 되어있으니, 실제 사용 시 헤더를 수정하거나 파생 클래스로 옮겨야 함)
    // InInfo에 ClearColor / ClearDepth 플래그 있으면 여기서 glClearBuffer* 호출
    // InName 은 디버그 마커 (glPushDebugGroup) 사용 가능 (KHR_debug)
    (void)InInfo;
    (void)InName;
    // 예:
    // if (InInfo.bClearColor) { glClearColor(...); glClear(GL_COLOR_BUFFER_BIT); }
    // if (InInfo.bClearDepth) { glClearDepth(...); glClear(GL_DEPTH_BUFFER_BIT); }
}

void FOpenGLDynamicRHI::RHIEndRenderPass()
{
    // glPopDebugGroup() 등
}

// -----------------------------------------------------------------------------
// Input Assembly
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHISetPrimitiveTopology(EPrimitiveType PrimitiveType)
{
    CurrentPrimitiveType = AsGL(PrimitiveType);
}

void FOpenGLDynamicRHI::RHISetInputLayout(FRHIVertexDeclaration* InputLayout)
{
    for (const FVertexElement& Attr : InputLayout->Attributes)
    {
        GLuint Index = AsGL(Attr.Usage, Attr.Index);
        GLenum Type = AsGL(Attr.Type);
        GLboolean bNormalized = Attr.bNormalized ? GL_TRUE : GL_FALSE;
        EnableVertexAttribArray(Index);
        VertexAttribPointer(Index, GetNumComponents(Attr.Type), Type, bNormalized, InputLayout->Stride, (void*)(uintptr_t)Attr.Offset);
    }
}

void FOpenGLDynamicRHI::RHISetVertexBuffer(uint32 Slot, FRHIBuffer* VertexBuffer, uint32 Stride, uint32 Offset)
{
    (void)Slot; // 여러 Slot/Binding 분리가 필요한 경우 VAO 안에서 glVertexAttribPointer 로 재구성
    if (auto* Buf = AsGL(VertexBuffer))
    {
        glBindBuffer(GL_ARRAY_BUFFER, Buf->BufferId);
        // Slot에 대한 Attribute 포인터 설정은 InputLayout 생성 시점에서 이미 끝낸다고 가정.
        // 혹은 여기서 Slot별 glEnableVertexAttribArray / glVertexAttribPointer 호출 가능.
        Buf->Stride = Stride;
        // Offset은 glVertexAttribPointer 의 pointer 파라미터로 사용 (void*)(uintptr_t)Offset
    }
}

void FOpenGLDynamicRHI::RHISetIndexBuffer(uint32 /*Slot*/, FRHIBuffer* IndexBuffer, uint32 Offset)
{
    if (auto* Buf = AsGL(IndexBuffer))
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buf->BufferId);
        // Offset은 DrawIndexed 시 glDrawElementsBaseVertex 등과 조합하거나 
        // 인덱스 시작 오프셋을 (void*)(uintptr_t)Offset 형태로 사용
        (void)Offset;
    }
}

// -----------------------------------------------------------------------------
// Shader Binding (단순: Program 단위)
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHISetVertexShader(FRHIVertexShader* VertexShader)
{
    // 단일 program 사용이라면 PixelShader 와 같은 ProgramId일 수도 있음
    if (auto* Prog = AsGL(VertexShader))
    {
        glUseProgram(Prog->ProgramId);
    }
}
void FOpenGLDynamicRHI::RHISetPixelShader(FRHIPixelShader* PixelShader)
{
    if (auto* Prog = AsGL(PixelShader))
    {
        glUseProgram(Prog->ProgramId);
    }
}
void FOpenGLDynamicRHI::RHISetComputeShader(FRHIComputeShader* ComputeShader)
{
    if (auto* Prog = AsGL(ComputeShader))
    {
        glUseProgram(Prog->ProgramId);
    }
}
void FOpenGLDynamicRHI::RHISetGeometryShader(FRHIGeometryShader* GeometryShader)
{
    if (auto* Prog = AsGL(GeometryShader))
    {
        glUseProgram(Prog->ProgramId);
    }
}

// Uniform Buffer
void FOpenGLDynamicRHI::RHISetStaticUniformBuffer(EShaderType /*TargetShader*/, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer)
{
    if (auto* UB = AsGL(UniformBuffer))
    {
        // Slot = glBindBufferBase binding point
        glBindBufferBase(GL_UNIFORM_BUFFER, Slot.Value, UB->BufferId);
    }
}

void FOpenGLDynamicRHI::RHISetDynamicUniformBuffer(EShaderType /*TargetShader*/, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer)
{
    // Dynamic / Static 구분 없이 동일 처리 (실제 엔진에서는 업로드 전략이 다를 수 있음)
    if (auto* UB = AsGL(UniformBuffer))
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, Slot.Value, UB->BufferId);
    }
}

void FOpenGLDynamicRHI::RHISetShaderResourceView(EShaderType /*TargetShader*/, FShaderResourceStaticSlot Slot, FRHIView* SRV)
{
    // SRV = TextureView 라고 가정 (단순 Texture)
    // 여기서는 FRHIView -> FGLTextureResource 로 캐스팅 필요 (프로젝트 구조에 따라 다를 수 있음)
    auto* Tex = reinterpret_cast<FGLTextureResource*>(SRV);
    if (Tex)
    {
        glActiveTexture(GL_TEXTURE0 + Slot.Value);
        glBindTexture(Tex->Target, Tex->TextureId);
        // GLSL sampler uniform location 세팅은 Shader Program Link 후 glUniform1i(location, Slot)
        // 그 부분은 셰이더 설정 단계(Program 준비)에서 해야 함
    }
}

void FOpenGLDynamicRHI::RHISetSampler(EShaderType /*TargetShader*/, FSamplerStaticSlot Slot, FRHISamplerState* SamplerState)
{
    if (auto* Sampler = AsGL(SamplerState))
    {
        glBindSampler(Slot.Value, Sampler->SamplerId);
    }
}

// -----------------------------------------------------------------------------
// Rasterizer State / Blend / DepthStencil
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHISetRasterizerState(FRHIRasterizerState* RasterizerState)
{
    if (auto* RS = AsGL(RasterizerState))
    {
        if (RS->CullEnable)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(RS->CullMode);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        glFrontFace(RS->FrontFace);
        if (RS->DepthClamp)
            glEnable(GL_DEPTH_CLAMP);
        else
            glDisable(GL_DEPTH_CLAMP);
        if (RS->Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void FOpenGLDynamicRHI::RHISetBlendState(FRHIBlendState* BlendState, const FLinearColor& BlendFactor, uint32 /*SampleMask*/)
{
    if (auto* BS = AsGL(BlendState))
    {
        if (BS->Enabled)
        {
            glEnable(GL_BLEND);
            glBlendFuncSeparate(BS->SrcRGB, BS->DstRGB, BS->SrcA, BS->DstA);
            glBlendEquationSeparate(BS->EqRGB, BS->EqA);
            // glBlendColor(BlendFactor.R, BlendFactor.G, BlendFactor.B, BlendFactor.A); 필요시
            glBlendColor(BlendFactor.R, BlendFactor.G, BlendFactor.B, BlendFactor.A);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
}

void FOpenGLDynamicRHI::RHISetDepthStencilState(FRHIDepthStencilState* DepthStencilState, uint32 StencilRef)
{
    if (auto* DS = AsGL(DepthStencilState))
    {
        if (DS->DepthTest)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(DS->DepthFunc);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        glDepthMask(DS->DepthWrite ? GL_TRUE : GL_FALSE);

        if (DS->StencilEnable)
        {
            glEnable(GL_STENCIL_TEST);
            // 단순 예: 양면 동일, 참고: glStencilFuncSeparate, glStencilOpSeparate 가능
            glStencilFunc(GL_ALWAYS, static_cast<GLint>(StencilRef), 0xFFFFFFFF);
            glStencilMask(0xFFFFFFFF);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }
    }
}

void FOpenGLDynamicRHI::RHISetViewport(FRHIViewport* Viewport)
{
    if (auto* VP = AsGL(Viewport))
    {
        glViewport(VP->X, VP->Y, VP->Width, VP->Height);
        // glDepthRange(VP->MinDepth, VP->MaxDepth);  // core profile에서는 glDepthRangef 가능
        glDepthRange(VP->MinDepth, VP->MaxDepth);
    }
}

// -----------------------------------------------------------------------------
// Render Targets (FBO)
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHISetRenderTargets(uint32 NumRTVs, FRHIView* const* RTVs, FRHIView* DSV)
{
    // 단순 예:
    // 1) FBO 하나를 바인딩
    // 2) Color attachment / Depth attachment 붙임
    // 실제 구현에서는 FBO 캐시, MRT, SRGB, Textarget 타입에 따른 glFramebufferTexture2D 분기 등 필요
    GLuint FBO = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    for (uint32 i = 0; i < NumRTVs; ++i)
    {
        auto* Tex = reinterpret_cast<FGLTextureResource*>(RTVs[i]);
        if (Tex)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, Tex->Target, Tex->TextureId, 0);
        }
    }

    if (DSV)
    {
        auto* DepthTex = reinterpret_cast<FGLTextureResource*>(DSV);
        if (DepthTex)
        {
            // Depth-only 라 가정
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthTex->Target, DepthTex->TextureId, 0);
        }
    }

    // DrawBuffers 설정
    if (NumRTVs > 0)
    {
        GLenum Attachments[8];
        for (uint32 i = 0; i < NumRTVs; ++i)
            Attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(static_cast<GLsizei>(NumRTVs), Attachments);
    }
    else
    {
        // glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    }

    // FBO 상태 체크 (개발단계)
#if !defined(NDEBUG)
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(Status == GL_FRAMEBUFFER_COMPLETE && "Incomplete FBO in RHISetRenderTargets");
#endif
}

// -----------------------------------------------------------------------------
// Updates (Buffer / UniformBuffer / Texture / Viewport)
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHIUpdateBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Size)
{
    if (auto* B = AsGL(Buffer))
    {
        glBindBuffer(B->Target, B->BufferId);
        // 전체 업데이트 (동기): glBufferSubData
        glBufferSubData(B->Target, 0, Size, Data);
    }
}

void FOpenGLDynamicRHI::RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBuffer, const void* Data, uint32 Size)
{
    if (auto* UB = AsGL(UniformBuffer))
    {
        glBindBuffer(GL_UNIFORM_BUFFER, UB->BufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, Size, Data);
    }
}

void FOpenGLDynamicRHI::RHIUpdateTexture(FRHITexture* Texture, const void* Data, uint32 Size)
{
    (void)Size; // Raw Size는 GL이 stride/format 필요 -> 실제 TextureDesc 필요
    if (auto* Tex = AsGL(Texture))
    {
        // 예시: RGBA8 2D 텍스처 전체 업데이트라고 가정
        // 실제로는 폭/높이/포맷 정보를 Texture에 저장해야 함
        // glBindTexture(Tex->Target, Tex->TextureId);
        // glTexSubImage2D(Tex->Target, 0, 0,0, Width,Height, GL_RGBA, GL_UNSIGNED_BYTE, Data);
        // 여기서는 정보가 없으니 TODO
    }
}

void FOpenGLDynamicRHI::RHIUpdateViewport(FRHIViewport* Viewport, const FRHIViewportDesc& Desc)
{
    if (auto* VP = AsGL(Viewport))
    {
        VP->X = Desc.X;
        VP->Y = Desc.Y;
        VP->Width  = Desc.Width;
        VP->Height = Desc.Height;
        VP->MinDepth = Desc.MinDepth;
        VP->MaxDepth = Desc.MaxDepth;
        glViewport(VP->X, VP->Y, VP->Width, VP->Height);
        glDepthRange(VP->MinDepth, VP->MaxDepth);
    }
}

// -----------------------------------------------------------------------------
// Clear
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHIClearColorTexture(FRHITexture* Texture, const FLinearColor& ClearColor)
{
    // 특정 텍스처만 지우려면 FBO + glClearBufferfv 사용
    // 여기서는 현재 바인딩된 RenderTarget 전체 Clear로 간단화
    (void)Texture;
    glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
    glClear(GL_COLOR_BUFFER_BIT);
}

void FOpenGLDynamicRHI::RHIClearDepthTexture(FRHITexture* Texture, float Depth, uint8 Stencil)
{
    (void)Texture;
    glClearDepth(Depth);
    glClearStencil(Stencil);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------
void FOpenGLDynamicRHI::RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances)
{
    // 기본 모드: GL_TRIANGLES 등 (PrimitiveTopology 저장 가정)
    // 베이스 버텍스 적용하려면 glDrawArraysInstancedBaseInstance (GL 4.2+) 또는 버텍스 셰이더에서 추가
    GLenum Mode = GL_TRIANGLES; // ToGLPrimitive(CurrentTopology) 식으로 교체
    if (NumInstances > 1)
    {
#if defined(GL_VERSION_4_2) || defined(GL_ES_VERSION_3_1)
        glDrawArraysInstancedBaseInstance(Mode,
                                          static_cast<GLint>(BaseVertexIndex),
                                          static_cast<GLsizei>(NumVertices),
                                          static_cast<GLsizei>(NumInstances),
                                          0);
#else
        glDrawArraysInstanced(Mode,
                              static_cast<GLint>(BaseVertexIndex),
                              static_cast<GLsizei>(NumVertices),
                              static_cast<GLsizei>(NumInstances));
#endif
    }
    else
    {
        glDrawArrays(Mode,
                     static_cast<GLint>(BaseVertexIndex),
                     static_cast<GLsizei>(NumVertices));
    }
}

void FOpenGLDynamicRHI::RHIDrawIndexedPrimitive(uint32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances)
{
    // Index Buffer가 이미 GL_ELEMENT_ARRAY_BUFFER 로 바인딩되어 있다고 가정
    GLenum Mode = GL_TRIANGLES; // ToGLPrimitive(CurrentTopology) 식으로 교체
    const GLvoid* IndicesOffset = reinterpret_cast<const GLvoid*>((uintptr_t)StartIndex * sizeof(uint32));

    if (NumInstances > 1)
    {
#if defined(GL_VERSION_4_2) || defined(GL_ES_VERSION_3_1)
        // glDrawElementsInstancedBaseVertexBaseInstance 사용 가능 (PC GL 4.2+)
        glDrawElementsInstancedBaseVertex(Mode,
                                          static_cast<GLsizei>(NumIndices),
                                          GL_UNSIGNED_INT,
                                          IndicesOffset,
                                          static_cast<GLsizei>(NumInstances),
                                          static_cast<GLint>(BaseVertexIndex));
#else
        glDrawElementsInstanced(Mode,
                                static_cast<GLsizei>(NumIndices),
                                GL_UNSIGNED_INT,
                                IndicesOffset,
                                static_cast<GLsizei>(NumInstances));
#endif
    }
    else
    {
#if defined(GL_VERSION_3_2)
        glDrawElementsBaseVertex(Mode,
                                 static_cast<GLsizei>(NumIndices),
                                 GL_UNSIGNED_INT,
                                 IndicesOffset,
                                 static_cast<GLint>(BaseVertexIndex));
#else
        // BaseVertexIndex 적용 불가 → 버텍스 셰이더에서 상수로 더하거나 인덱스 재생성 필요
        glDrawElements(Mode,
                       static_cast<GLsizei>(NumIndices),
                       GL_UNSIGNED_INT,
                       IndicesOffset);
#endif
    }
}