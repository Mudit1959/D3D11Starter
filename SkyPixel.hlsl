struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

TextureCube cubeMap : register(t0);
SamplerState cubeMapSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    return cubeMap.Sample(cubeMapSampler, input.sampleDir);
}