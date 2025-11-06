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
    
    float3 diffuse, specular;
    float3 total = (0.0f, 0.0f, 0.0f);
    total += ambientColor;

    for (int i = 0; i < 5; i++)
    {
        diffuse = diffuseTerm(lights[i].Color, lights[i].Intensity, crosswalk.Sample(BasicSampler, input.uv).xyz, input.normal, lights[i].Direction);
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL_MATTE:
                total += diffuse;
                break;
        
            case LIGHT_TYPE_DIRECTIONAL_SPECULAR:
                specular = specularTerm(camWorldPos, input.worldPos, lights[i].Direction,  lights[i].Intensity, input.normal, roughness);
                total += (specular + diffuse);
                break;
        
            case LIGHT_TYPE_POINT:
                total += PointLight(input, crosswalk.Sample(BasicSampler, input.uv).xyz, lights[i]);
                break;
            
            case LIGHT_TYPE_SPOT:
                total += SpotLight(input, crosswalk.Sample(BasicSampler, input.uv).xyz, lights[i]);
                break;
        }
    }
    
    return float4(total, 1.0f);
}