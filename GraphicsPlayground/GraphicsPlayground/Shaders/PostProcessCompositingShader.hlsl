Texture2D FogTexture : register(t103);
Texture2D BlurTexture : register(t106);
Texture2D ShapeOverlayTexture : register(t107);
// PostProcessing 추가 시 Texture 추가 (EShaderSRVSlot)

SamplerState CompositingSampler : register(s0);

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_Input mainVS(uint VertexID : SV_VertexID)
{
    PS_Input Output;

    float2 QuadPositions[6] = {
        float2(-1,  1),  // Top Left
        float2(1,  1),  // Top Right
        float2(-1, -1),  // Bottom Left
        float2(1,  1),  // Top Right
        float2(1, -1),  // Bottom Right
        float2(-1, -1)   // Bottom Left
    };

    float2 UVs[6] = {
        float2(0, 0), float2(1, 0), float2(0, 1),
        float2(1, 0), float2(1, 1), float2(0, 1)
    };

    Output.Position = float4(QuadPositions[VertexID], 0, 1);
    Output.UV = UVs[VertexID];

    return Output;
}

// Util: over 연산 (SRC over DEST)
float4 AlphaOver(float4 src, float4 dest)
{
    float outA = src.a + dest.a * (1 - src.a);
    float3 outRGB = (src.rgb * src.a + dest.rgb * dest.a * (1 - src.a)) / max(outA, 1e-5);
    return float4(outRGB, outA);
}

float4 mainPS(PS_Input input) : SV_Target
{
    float2 UV = input.UV;
    float4 FogColor = FogTexture.Sample(CompositingSampler, UV);
    float4 BlurColor = BlurTexture.Sample(CompositingSampler, UV);
    float4 ShapeOverlayColor = ShapeOverlayTexture.Sample(CompositingSampler, UV);

    // Fog가 가장 아래, 그 위에 Blur, 그 위에 ShapeOverlay가 덮인다고 가정
    float4 result = AlphaOver(BlurColor, FogColor);
    result = AlphaOver(ShapeOverlayColor, result);

    return result;
}
