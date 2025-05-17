// ParticleShader.hlsl
#include "ShaderRegisters.hlsl"

#ifndef PARTICLE_SHADER
#define PARTICLE_SHADER

// Define 구분: 스프라이트 vs 메시 파티클
// -D PARTICLE_SPRITE_SHADER 또는 -D PARTICLE_MESH_SHADER 정의 필요

// ==================== Vertex Structures ====================

#ifdef PARTICLE_SPRITE_SHADER
struct VS_INPUT
{
    float3 Position     : POSITION;
    float RelativeTime  : TEXCOORD0;
    float3 OldPosition  : TEXCOORD1;
    float2 Size         : TEXCOORD2;
    float Rotation      : TEXCOORD3;
    float SubImageIndex : TEXCOORD4;
    float4 Color        : COLOR0;
};
#endif

#ifdef PARTICLE_MESH_SHADER
struct VS_INPUT
{
    float4 Transform0   : POSITION0;  // row0 of world transform
    float4 Transform1   : POSITION1;
    float4 Transform2   : POSITION2;
    float4 Velocity     : VELOCITY;
    float4 Color        : COLOR0;
    float  SubUVLerp    : TEXCOORD0;
    float  RelativeTime : TEXCOORD1;
};
#endif

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

// ==================== Vertex Shader ====================
//#define PARTICLE_MESH_SHADER 1;
VS_OUTPUT mainVS(VS_INPUT input)
{
    VS_OUTPUT output;

#ifdef PARTICLE_SPRITE_SHADER
    // Billboard 회전 처리 (간단한 Z축 회전)
    float2 quadPos = float2(0.5f, 0.5f); // TODO: 실제 Draw 시 Quad Vertex ID에 따라 좌표 세팅
    float s = sin(input.Rotation);
    float c = cos(input.Rotation);
    float2 rotated = float2(
        quadPos.x * input.Size.x * c - quadPos.y * input.Size.y * s,
        quadPos.x * input.Size.x * s + quadPos.y * input.Size.y * c
    );

    float3 worldPos = input.Position + float3(rotated, 0);
    output.WorldPos = worldPos;
    float4 viewPos = mul(float4(worldPos, 1.0f), ViewMatrix);
    output.Position = mul(viewPos, ProjectionMatrix);

    output.Color = input.Color;
    output.UV = float2(0.0f, 0.0f); // TODO: SubImage UV 계산
#endif

#ifdef PARTICLE_MESH_SHADER
    // Mesh 파티클의 World Transform 적용
    float3 localPos = float3(0, 0, 0); // TODO: 실제 정점 전달 필요
    float3 worldPos = localPos.x * input.Transform0.xyz +
                      localPos.y * input.Transform1.xyz +
                      localPos.z * input.Transform2.xyz +
                      input.Transform0.w * input.Transform1.w * input.Transform2.w;

    output.WorldPos = worldPos;
    float4 viewPos = mul(float4(worldPos, 1.0f), ViewMatrix);
    output.Position = mul(viewPos, ProjectionMatrix);
    output.Color = input.Color;
    output.UV = float2(0.0f, 0.0f); // TODO: SubUV 계산
#endif

    return output;
}

// ==================== Pixel Shader ====================

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
    float4 finalColor = input.Color;
    finalColor.rgb = LinearToSRGB(finalColor.rgb);
    return finalColor;
}

#endif // PARTICLE_SHADER
