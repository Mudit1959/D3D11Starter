Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

cbuffer externalData : register(b0)
{
    int blurRadius;
    float pixelWidth; // How much of the UV does a pixel take up
    float pixelHeight;
}

struct PPVertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PPVertexToPixel input) : SV_TARGET
{
    // Track the total color and number of samples
    float4 total = 0;
    int count = 0;
    // Loop through the "box"
    for (int x = -blurRadius; x <= blurRadius; x++)
    {
        for (int y = -blurRadius; y <= blurRadius; y++)
        {
    // Calculate the uv for this sample
            float2 uv = input.uv;
            uv += float2(x * pixelWidth, y * pixelHeight);
    // Add this color to the running total
            total += Pixels.Sample(ClampSampler, uv);
            count++;
        }
    }
    // Return the average
    return total / count;
}