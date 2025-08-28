
#include "ShaderRegisters.hlsl"

StructuredBuffer<float4x4> BoneMatrices : register(t1);

cbuffer FCPUSkinningConstants : register(b1)
{
    bool bCPUSkinning;
    float3 pad0;
}

PS_INPUT_SkeletalMesh mainVS(VS_INPUT_SkeletalMesh Input)
{
    PS_INPUT_SkeletalMesh Output;

    float4 SkinnedPosition = float4(0, 0, 0, 0);
    float3 SkinnedNormal = float3(0, 0, 0);
    float3 SkinnedTangent = float3(0, 0, 0);
    
    if (bCPUSkinning)
    {
        SkinnedPosition = float4(Input.Position, 1.0f);
        SkinnedNormal = Input.Normal;
        SkinnedTangent = Input.Tangent.xyz;
    }
    else
    {    
        // 가중치 합산
        float TotalWeight = 0.0f;
    
        for (int i = 0; i < 4; ++i)
        {
            float Weight = Input.BoneWeights[i];
            TotalWeight += Weight;
        
            if (Weight > 0.0f)
            {
                uint BoneIdx = Input.BoneIndices[i];
            
                // 본 행렬 적용 (BoneMatrices는 이미 최종 스키닝 행렬)
                // FBX SDK에서 가져온 역바인드 포즈 행렬이 이미 포함됨
                float4 pos = mul(float4(Input.Position, 1.0f), BoneMatrices[BoneIdx]);
                float3 norm = mul(float4(Input.Normal, 0.0f), BoneMatrices[BoneIdx]).xyz;
                float3 tan = mul(float4(Input.Tangent.xyz, 0.0f), BoneMatrices[BoneIdx]).xyz;
            
                SkinnedPosition += Weight * pos;
                SkinnedNormal += Weight * norm;
                SkinnedTangent += Weight * tan;
            }
        }
    
        // 가중치 예외 처리
        if (TotalWeight < 0.001f)
        {
            SkinnedPosition = float4(Input.Position, 1.0f);
            SkinnedNormal = Input.Normal;
            SkinnedTangent = Input.Tangent.xyz;
        }
        else if (abs(TotalWeight - 1.0f) > 0.001f && TotalWeight > 0.001f)
        {
            // 가중치 합이 1이 아닌 경우 정규화
            SkinnedPosition /= TotalWeight;
            SkinnedNormal /= TotalWeight;
            SkinnedTangent /= TotalWeight;
        }
    }
    
    Output.Position = SkinnedPosition;
    Output.Position = mul(Output.Position, WorldMatrix);
    Output.WorldPosition = Output.Position.xyz;
    
    Output.Position = mul(Output.Position, ViewMatrix);
    Output.Position = mul(Output.Position, ProjectionMatrix);
    
    Output.WorldNormal = normalize(mul(normalize(SkinnedNormal), (float3x3)InverseTransposedWorld));

    // Begin Tangent
    float3 WorldTangent = mul(normalize(SkinnedTangent), (float3x3)WorldMatrix);
    WorldTangent = normalize(WorldTangent - Output.WorldNormal * dot(Output.WorldNormal, WorldTangent));

    Output.WorldTangent = float4(WorldTangent, Input.Tangent.w);
    // End Tangent

    Output.Color = Input.Color;
    Output.UV = Input.UV;

    Output.MaterialIndex = 0;

    return Output;
}
