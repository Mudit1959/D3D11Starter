#include "ShaderInclude.hlsli"

Texture2D Albedo : register(t0); // "t" registers for textures
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
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
    
    
    // -- RENORMALIZING AND SCALING NORMALS AND UV --
    input.uv = input.uv * scale + offset;
    input.normal = normalize(input.normal);
    
    float3 total = (0.0f, 0.0f, 0.0f);
    
    // -- GAMMA CORRECT SURFACE(ALBEDO) COLOR --
    float3 surfaceColor = Albedo.Sample(BasicSampler, input.uv).xyz;
    surfaceColor = pow(surfaceColor, 2.2); // Remove gamma correction from texture
    
    // -- SAMPLE ROUGHNESS AND METALNESS -- 
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r * isMetal;
    float3 f0 = lerp(0.04, surfaceColor.rgb, metalness);
    
    
    // -- ADD SURFACE COLOR WITH TINT
    total += surfaceColor * colorTint.rgb;
    //total *= ambientColor;
    
    // -- SAMPLE NORMAL MAP, CHANGE NORMALS TO ACCOUNT FOR SURFACE UNEVENENESS -- 
    float3 finalNormal;
    finalNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).xyz * 2.0f -1.0f); // First unpack the normal map's normal
    
    float3 T, B, N;
    N = input.normal;
    T = normalize(input.tangent - dot(input.tangent, N) * N);
    B = cross(T, N);
    
    float3x3 TBN = float3x3(T, B, N);
    finalNormal = mul(finalNormal, TBN);
    input.normal = finalNormal; // Reassigned incoming data to hold new normal
    
    
    float3 balancedDiffuse, microfacetBRDF, toCamera, halfVector;
    toCamera = normalize(camWorldPos - input.worldPos);
    
    for (int i = 0; i < 5; i++)
    {
        halfVector = (normalize(lights[i].Direction) + toCamera) / 2;
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_PBR_DIRECTIONAL: // 6
            
            
            case LIGHT_TYPE_PBR_POINT: //7
            
            case LIGHT_TYPE_PBR_SPOT: //8
        }
    }
    
    total = pow(total, 1.0f / 2.2f); // Gamma correct final color
    return float4(total, 1.0f);
    
}


/*
case LIGHT_TYPE_DIRECTIONAL_MATTE: //0
    total += DiffuseDirTerm(input.normal, surfaceColor, lights[i]);
    break;
        
case LIGHT_TYPE_DIRECTIONAL_SPECULAR: //1
    total += DiffuseDirTerm(input.normal, surfaceColor, lights[i]);
    total += SpecularDirTerm(input.worldPos, input.normal, lights[i], roughness);
    break;
        
case LIGHT_TYPE_POINT_MATTE: //2
    total += DiffusePointLight(input.worldPos, input.normal, surfaceColor, lights[i]) * Attenuate(lights[i], input.worldPos);
    break;
            
case LIGHT_TYPE_POINT_SPECULAR: //3
    total += (DiffusePointLight(input.worldPos, input.normal, surfaceColor, lights[i]) + SpecularDirTerm(input.worldPos, input.normal, lights[i], roughness)) * Attenuate(lights[i], input.worldPos);
    break;
            
case LIGHT_TYPE_SPOT_MATTE: //4
    total += DiffuseSpotLight(input.worldPos, input.normal, surfaceColor, lights[i]) * Attenuate(lights[i], input.worldPos);
    break;
            
case LIGHT_TYPE_SPOT_SPECULAR: //5
    total += (DiffuseSpotLight(input.worldPos, input.normal, surfaceColor, lights[i]) + SpecularDirTerm(input.worldPos, input.normal, lights[i], roughness) * DirectionalTerm(input.worldPos, lights[i])) * Attenuate(lights[i], input.worldPos);
    break;
*/