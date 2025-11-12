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
    
   
    
    input.uv = input.uv * scale + offset;
    float3 total = (0.0f, 0.0f, 0.0f);
    
    float3 diffuse, specular;
    
    float3 surfaceColor = material.Sample(BasicSampler, input.uv).xyz;
    total += ambientColor * float3(1.0f, 1.0f, 1.0f);
    
    float3 normalFromMap = normalize(normalMap.Sample(BasicSampler, input.uv) * 2.0f - 1.0f);
    float3 T = normalize(input.normal);
    float3 B = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);
    float3 N = cross(T, B);
    
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    
    float3x3 TBN = float3x3(T, B, N);
    float3 finalNormal = normalize(mul(TBN, normalFromMap));
    
    for (int i = 0; i < 5; i++)
    {
        diffuse = diffuseTerm(lights[i].Color, lights[i].Intensity, material.Sample(BasicSampler, input.uv).xyz, finalNormal, lights[i].Direction);
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL_MATTE:
                total += diffuse;
                break;
        
            case LIGHT_TYPE_DIRECTIONAL_SPECULAR:
                specular = specularTerm(camWorldPos, input.worldPos, surfaceColor, lights[i].Color, lights[i].Direction, lights[i].Intensity, finalNormal, roughness) * any(diffuse);
                total += (specular + diffuse);
                break;
        
            case LIGHT_TYPE_POINT:
                total += PointLight(input, surfaceColor, finalNormal, lights[i]); 
                break;
            
            case LIGHT_TYPE_SPOT:
                total += SpotLight(input, surfaceColor, finalNormal, lights[i]);
                break;
        }
    }
    
    return float4(diffuse+specular, 1.0f);
    
}