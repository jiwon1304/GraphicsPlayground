#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/Matrix.h"

struct FCone
{
    FVector ConeApex; // 원뿔의 꼭짓점
    float ConeRadius; // 원뿔 밑면 반지름

    FVector ConeBaseCenter; // 원뿔 밑면 중심
    float ConeHeight; // 원뿔 높이 (Apex와 BaseCenter 간 차이)
    
    FVector4 Color;

    int ConeSegmentCount; // 원뿔 밑면 분할 수
    float pad[3];
};

struct FPrimitiveCounts
{
    int BoundingBoxCount;
    int pad;
    int ConeCount;
    int pad1;
};

#define MAX_LIGHTS 16
#define NUM_FACES 6
#define MAX_CASCADE_NUM 5

enum ELightType {
    POINT_LIGHT = 1,
    SPOT_LIGHT = 2,
    DIRECTIONAL_LIGHT = 3,
    AMBIENT_LIGHT = 4,
    NUM_LIGHT_TYPES = 5
};

struct FMaterialConstants
{
    uint32 TextureFlag;
    FVector DiffuseColor;

    FVector SpecularColor;
    float Shininess;

    FVector EmissiveColor;
    float Transparency;

    float Metallic;
    float Roughness;
    FVector2D MaterialPadding;
};

struct FPointLightGSBuffer
{
    FMatrix World;
    FMatrix ViewProj[NUM_FACES]; // 6 : NUM_FACES
};

struct FCascadeConstantBuffer
{
    FMatrix World;
    FMatrix ViewProj[MAX_CASCADE_NUM];
    FMatrix InvViewProj[MAX_CASCADE_NUM];
    FMatrix InvProj[MAX_CASCADE_NUM];
    FVector4 CascadeSplit;

    float pad1;
    float pad2;
};

struct FShadowConstantBuffer
{
    FMatrix ShadowViewProj; // Light 광원 입장에서의 ViewProj
};

struct FObjectConstantBuffer
{
    FMatrix WorldMatrix;
    FMatrix InverseTransposedWorld;
    
    FVector4 UUIDColor;
    
    int bIsSelected;
    FVector pad;
};

struct FCameraConstantBuffer
{
    FMatrix ViewMatrix;
    FMatrix InvViewMatrix;
    
    FMatrix ProjectionMatrix;
    FMatrix InvProjectionMatrix;
    
    FVector ViewLocation;
    float Padding1;

    float NearClip;
    float FarClip;
    FVector2D Padding2;
};

struct FSubUVConstant
{
    FVector2D uvOffset;
    FVector2D uvScale;
};

struct FLitUnlitConstants
{
    int bIsLit; // 1 = Lit, 0 = Unlit 
    FVector pad;
};

struct FIsShadowConstants
{
    int bIsShadow;
    FVector pad;
};

struct FViewModeConstants
{
    uint32 ViewMode;
    FVector pad;
};

struct FSubMeshConstants
{
    float bIsSelectedSubMesh;
    FVector pad;
};

struct FTextureUVConstants
{
    float UOffset;
    float VOffset;
    float pad0;
    float pad1;
};

enum class EShaderSRVSlot : int8
{
    SRV_SpotLight = 50,
    SRV_DirectionalLight = 51,
    SRV_PointLight = 52,
    SRV_SceneDepth = 99,
    SRV_Scene = 100,
    SRV_PostProcess = 101,
    SRV_EditorOverlay = 102,
    SRV_Fog = 103,
    SRV_Debug = 104,
    SRV_CameraEffect = 105,
    SRV_Blur = 106,
    SRV_ShapeOverlay = 107,
    SRV_Viewport = 120,

    SRV_MAX = 127,
};

struct FVertexInfo
{
    uint32_t NumVertices;
    uint32_t Stride;
    struct ID3D11Buffer* VertexBuffer;
};

struct FIndexInfo
{
    uint32_t NumIndices;
    ID3D11Buffer* IndexBuffer;
};

struct FBufferInfo
{
    FVertexInfo VertexInfo;
    FIndexInfo IndexInfo;
};