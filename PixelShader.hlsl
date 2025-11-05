#include "ShaderInclude.hlsli"

Texture2D brick : register(t0); // "t" registers for textures
Texture2D concrete : register(t1); // "t" registers for textures
Texture2D crosswalk : register(t2); // "t" registers for textures
Texture2D rock : register(t3); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers


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
    input.normal = normalize(input.normal); // Renormalize incoming normals that have been translated into world space, and INTERPOLATED
    input.uv = input.uv * scale + offset;

    if (light.Type == LIGHT_TYPE_DIRECTIONAL_MATTE)
    {
        return float4(ambientColor + diffuseTerm(light.Color, light.Intensity, crosswalk.Sample(BasicSampler, input.uv).xyz, input.normal, light.Direction), 1.0f);
    }
        
    
    else if (light.Type == LIGHT_TYPE_DIRECTIONAL_SPECULAR)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}