
Texture2D brick : register(t0); // "t" registers for textures
Texture2D concrete : register(t1); // "t" registers for textures
Texture2D crosswalk : register(t2); // "t" registers for textures
Texture2D rock : register(t3); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

cbuffer ExternalPixelData : register(b0)
{
    float4 colourTint : TINT;
    float totalTime : TIME;
    float2 scale : SCALE;
    float2 offset : OFFSET;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    //input.uv = input.uv * scale + offset;
    return crosswalk.Sample(BasicSampler, input.uv) * rock.Sample(BasicSampler, input.uv);
}