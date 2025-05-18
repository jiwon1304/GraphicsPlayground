#ifndef PARTICLE_SHADER
#define PARTICLE_SHADER

#include "ShaderRegisters.hlsl"

// ==================== Config ====================

#ifndef PARTICLE_MESH
#define PARTICLE_SPRITE
#endif

cbuffer ParticleSettings : register(b4)
{
    int SubUVCols;
    int SubUVRows;
};

// ==================== Vertex Structures ====================

#if defined(PARTICLE_SPRITE)
struct VS_INPUT
{
    float3 Position : POSITION;
    float RelativeTime : TEXCOORD0;
    float3 OldPosition : TEXCOORD1;
    float2 Size : TEXCOORD2;
    float Rotation : TEXCOORD3;
    float SubImageIndex : TEXCOORD4;
    float2 UV : TEXCOORD5;
    float4 Color : COLOR0;
};
#endif

#if defined(PARTICLE_MESH)
struct VS_INPUT
{
    float3 LocalPos       : POSITION;
    float4 Color          : COLOR0;
    float4 Transform0     : TEXCOORD0;
    float4 Transform1     : TEXCOORD1;
    float4 Transform2     : TEXCOORD2;
    float4 Velocity       : TEXCOORD3;
    int2   SubUVParams01  : TEXCOORD4;
    float2 SubUVTime      : TEXCOORD5; // .x = SubUVLerp, .y = RelativeTime
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

VS_OUTPUT mainVS(VS_INPUT input)
{
    VS_OUTPUT output;

#if defined(PARTICLE_SPRITE)
    // (1) 회전값을 기준으로 쿼드 오프셋 계산
    float s = sin(input.Rotation);
    float c = cos(input.Rotation);

    float2 offset = float2(
        input.UV.x * input.Size.x * c - input.UV.y * input.Size.y * s,
        input.UV.x * input.Size.x * s + input.UV.y * input.Size.y * c
    );

    // (2) 파티클 중심 위치를 월드 좌표로 변환
    float3 worldCenter = mul(float4(input.Position, 1.0f), WorldMatrix).xyz;
    float3 worldPos = worldCenter + float3(offset, 0);

    // (3) 뷰/투영 변환
    float4 viewPos = mul(float4(worldPos, 1.0f), ViewMatrix);
    output.Position = mul(viewPos, ProjectionMatrix);
    output.WorldPos = worldPos;

    // (4) SubUV 계산
    float2 frameSize = float2(1.0f / SubUVCols, 1.0f / SubUVRows);
    float2 UVOffset = float2(fmod(input.SubImageIndex, SubUVCols), floor(input.SubImageIndex / SubUVCols)) * frameSize;
    output.UV = UVOffset + (input.UV + 0.5f) * frameSize;

    output.Color = input.Color;
#endif

#if defined(PARTICLE_MESH)
    // LocalPos를 Transform 행렬로 변환
    float3 worldPos =
        input.LocalPos.x * input.Transform0.xyz +
        input.LocalPos.y * input.Transform1.xyz +
        input.LocalPos.z * input.Transform2.xyz +
        float3(input.Transform0.w, input.Transform1.w, input.Transform2.w);

    float4 viewPos = mul(float4(worldPos, 1.0f), ViewMatrix);
    output.Position = mul(viewPos, ProjectionMatrix);
    output.WorldPos = worldPos;

    output.Color = input.Color;
    output.UV = float2(0.0f, 0.0f); // TODO: SubUV 계산 추가 가능
#endif

    return output;
}

// ==================== Pixel Shader ====================

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
    float4 finalColor = input.Color;
    finalColor.rgb = LinearToSRGB(finalColor.rgb);

    // 디버그용 컬러 제거
    // finalColor.rgb = float3(1, 0, 0);

    return finalColor;
}

#endif // PARTICLE_SHADER
