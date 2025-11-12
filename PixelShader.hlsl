#include "ShaderInclude.hlsli"

Texture2D material : register(t0); // "t" registers for textures
Texture2D normalMap : register(t1);
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
    
    // -- RENORMALIZING AND SCALING NORMALS AND UV --
    input.uv = input.uv * scale + offset;
    input.normal = normalize(input.normal);
    
    
    float3 total = (0.0f, 0.0f, 0.0f);
    
    float3 diffuse, specular;
    
    float3 surfaceColor = material.Sample(BasicSampler, input.uv).xyz;
    total += ambientColor * surfaceColor * colorTint.rgb;
    
    float3 finalNormal;
    
    finalNormal = normalize(normalMap.Sample(BasicSampler, input.uv).xyz * 2.0f -1.0f); // First unpack the normal map's normal
    
    float3 T, B, N;
    N = input.normal;
    T = normalize(input.tangent - dot(input.tangent, N) * N);
    B = cross(T, N);
    
    float3x3 TBN = float3x3(T, B, N);
    finalNormal = mul(finalNormal, TBN);
    input.normal = finalNormal;
    
    
    for (int i = 0; i < 5; i++)
    {
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL_MATTE: //0
                total += DiffuseDirTerm(input.normal, surfaceColor, lights[i]);
                break;
        
            case LIGHT_TYPE_DIRECTIONAL_SPECULAR: //1
                total += DiffuseDirTerm(input.normal, surfaceColor, lights[i]);
                total += SpecularDirTerm(input.worldPos, input.normal, lights[i]);
                break;
        
            case LIGHT_TYPE_POINT_MATTE: //2
                total += DiffusePointLight(input.worldPos, input.normal, surfaceColor, lights[i]) * Attenuate(lights[i], input.worldPos);
                break;
            
            case LIGHT_TYPE_POINT_SPECULAR: //3
                total += (DiffusePointLight(input.worldPos, input.normal, surfaceColor, lights[i]) + SpecularDirTerm(input.worldPos, input.normal, lights[i])) * Attenuate(lights[i], input.worldPos);
                break;
            
            case LIGHT_TYPE_SPOT_MATTE: //4
                total += DiffuseSpotLight(input.worldPos, input.normal, surfaceColor, lights[i]) * Attenuate(lights[i], input.worldPos);
                break;
            
            case LIGHT_TYPE_SPOT_SPECULAR: //5
                total += (DiffuseSpotLight(input.worldPos, input.normal, surfaceColor, lights[i]) + SpecularDirTerm(input.worldPos, input.normal, lights[i]) * DirectionalTerm(input.worldPos, lights[i])) * Attenuate(lights[i], input.worldPos);
                break;
        }
    }
    
    return float4(total, 1.0f);
    
}