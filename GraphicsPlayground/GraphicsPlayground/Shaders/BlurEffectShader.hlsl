#include "ShaderRegisters.hlsl"

Texture2D SceneDepthTexture : register(t99);
Texture2D SceneTexture : register(t100);
SamplerState SceneSampler : register(s0);

cbuffer DOFConstantBuffer : register(b0)
{
    float FocalDistance;
    float FocalRegion;
    float BlurAmount;
    float BlurRadius;
};

// 단순한 quad
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float3 ReconstructWorldPos(float2 UV, float Depth)
{
    float4 NDC;
    NDC.xy = UV * 2.0 - 1.0; // [0,1] → [-1,1]
    NDC.y *= -1;
    NDC.z = Depth;
    NDC.w = 1.0;

    float4 ViewPos = mul(NDC, InvProjectionMatrix);
    ViewPos /= ViewPos.w;

    return ViewPos.xyz;
}

// Depth 텍스처가 있고, 해당 픽셀의 depth 값을 구하는 함수가 있다고 가정
float GetSceneDepth(float2 uv)
{
    // 여기서 DSV는 R24G8
    float NDCDepth = SceneDepthTexture.Sample(SceneSampler, uv).r;
    return ReconstructWorldPos(uv, NDCDepth).z;
}

// focus 영역으로부터 멀면 1.0, focus 영역이면 0을 리턴하는 함수
float GetFocusFactor(float centerDepth, float FocalDistance, float FocalRegion, float BlurAmount)
{
    // 거리 기반으로 초점 영역을 확장
    float dynamicFocalRegion = FocalRegion * (1.0 + centerDepth * BlurAmount);

    if (centerDepth >= FocalDistance && centerDepth <= FocalDistance + dynamicFocalRegion)
    {
        return 0.0; // 완전한 포커스(블러 없음)
    }
    else if (centerDepth < FocalDistance)
    {
        return saturate((FocalDistance - centerDepth) / dynamicFocalRegion);
    }
    else
    {
        return saturate((centerDepth - (FocalDistance + dynamicFocalRegion)) / dynamicFocalRegion);
    }
}

float Gaussian(float x, float sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

float4 mainPS(VS_OUTPUT input) : SV_Target
{
    int k = int(BlurRadius);
    float3 FinalColor = float3(0, 0, 0);
    float TotalWeight = 0.0;
    float2 uv = input.UV;

    float centerDepth = GetSceneDepth(uv);
    float FocusFactor = GetFocusFactor(centerDepth, FocalDistance, FocalRegion, BlurAmount);


    for (int i = -k; i <= k; ++i)
    {
        for (int j = -k; j <= k; ++j)
        {
            float2 UVAdj = uv + float2(i, j) / 1080.0;
            
            // 만약 샘플링하는 픽셀이 in foucs 영역이면 블러 계산에 이용하지 않음.
            float DepthAdj = GetSceneDepth(UVAdj);
            float FocusFactorAdj = GetFocusFactor(DepthAdj, FocalDistance, FocalRegion, BlurAmount);
            
            float SamplingWeight = Gaussian(length(float2(i, j)), BlurRadius);

            float Weight = FocusFactorAdj * SamplingWeight;
            
            float3 ColorAdj = SceneTexture.Sample(SceneSampler, UVAdj).rgb;
            FinalColor += ColorAdj * Weight;
            TotalWeight += Weight;
        }
    }

    if (TotalWeight > 0.0)
        FinalColor /= TotalWeight;
    else
        FinalColor = SceneTexture.Sample(SceneSampler, uv).rgb;


    return float4(FinalColor, FocusFactor);
}

VS_OUTPUT
    mainVS(
    uint vertexId : SV_VertexID)
{
    VS_OUTPUT output;

    // 6개의 정점으로 2개의 삼각형(쿼드) 그리기
    // 정점 인덱스별 위치와 UV 좌표
    float2 positions[6] =
    {
        float2(-1.0, -1.0), // bottom left
        float2(-1.0, 1.0), // top left
        float2(1.0, -1.0), // bottom right
        float2(-1.0, 1.0), // top left
        float2(1.0, 1.0), // top right
        float2(1.0, -1.0) // bottom right
    };
    float2 uvs[6] =
    {
        float2(0.0, 1.0),
        float2(0.0, 0.0),
        float2(1.0, 1.0),
        float2(0.0, 0.0),
        float2(1.0, 0.0),
        float2(1.0, 1.0)
    };

    output.Position = float4(positions[vertexId], 0.0, 1.0);
    output.UV = uvs[vertexId];
    return output;
}
