// -- VERTEX SHADER --
// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD; // UV Coordinates
    float3 normal : NORMAL; // NORMAL
    float3 tangent: TANGENT; // TANGENT FOR NORMAL MAPPING
};

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
    
    float3 worldPos : POSITION;
    
    float3 tangent : TANGENT;
};

//Random value
float random(float2 s)
{
    return frac(sin(dot(s, float2(12.9898, 78.233))) * 43758.5453123);
}



// -- LIGHTING -- //
#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL_MATTE 0
#define LIGHT_TYPE_DIRECTIONAL_SPECULAR 1
#define LIGHT_TYPE_POINT_MATTE 2
#define LIGHT_TYPE_POINT_SPECULAR 3
#define LIGHT_TYPE_SPOT_MATTE 4
#define LIGHT_TYPE_SPOT_SPECULAR 5

struct Light
{
    int Type    : LIGHT_TYPE; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction    : DIRECTION; // Directional and Spot lights need a direction

    float Range         : RANGE; // Point and Spot lights have a max range for attenuation
    float3 Position     : WORLD_POS; // Point and Spot lights have a position in space

    float Intensity     : INTENSITY; // All lights need an intensity
    float3 Color        : LIGHT_COLOUR; // All lights need a color

    float SpotInnerAngle: SPOT_INNER; // Inner cone angle (in radians) – Inside this, full light!
    float SpotOuterAngle: SPOT_OUTER; // Outer cone angle (radians) – Outside this, no light!
    float2 Padding      : PADDING; // Purposefully padding to hit the 16-byte boundary
};

// -- PIXEL SHADER -- 
// External Data passed at Pixel Shader level
cbuffer ExternalPixelData : register(b0)
{
    float4 colorTint : TINT;
    
    float2 scale : SCALE;
    float2 offset : OFFSET;
    
    float3 camWorldPos : CAMERA_WORLD_POS;
    float totalTime : TIME;
    
    float roughness : ROUGHNESS;
    float3 ambientColor : AMBIENT_COLOR;
    
    Light lights[5];
};

// -- LIGHTING EQUATIONS --

float3 DiffuseDirTerm(float3 surfaceNormal, float3 surfaceColor, Light light)
{
    return saturate(dot(surfaceNormal, -light.Direction)) * surfaceColor * light.Color * light.Intensity;
}

// Specular requires V, L, N, and R
// V - View Vector - Direction from surface to camera. Needs the world position of the camera(camWorldPos) and the surface world position (worldPos)
// L - Light Direction Vector - The normalized direction of the light to the surface
// N - Surface normal vector - must be normalized
// R - The reflection of L along N
float SpecularDirTerm(float3 surfaceWorldPos, float3 surfaceNormal, Light light)
{
    float specExp = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    float3 V = normalize(camWorldPos - surfaceWorldPos);
    float3 L = light.Direction;
    float3 R = reflect(L, surfaceNormal);
    return pow(saturate(dot(R, V)), specExp) * light.Intensity;

}

float DirectionalTerm(float3 surfaceWorldPos, Light light)
{
    return saturate(dot(surfaceWorldPos - light.Position, light.Direction));
}

float3 DiffusePointLight(float3 surfaceWorldPos, float3 surfaceNormal, float3 surfaceColor, Light light)
{
    return saturate(dot(surfaceNormal, normalize(light.Position - surfaceWorldPos))) * surfaceColor * light.Color * light.Intensity;
}

float3 DiffuseSpotLight(float3 surfaceWorldPos, float3 surfaceNormal, float3 surfaceColor, Light light)
{
    float dotIL = saturate(dot(surfaceWorldPos - light.Position, light.Direction));
    float cosOuterAngle = cos(light.SpotOuterAngle);
    float cosInnerAngle = cos(light.SpotInnerAngle);
    float fallOff = cosOuterAngle - cosInnerAngle;
    
    float spotTerm = saturate((cosOuterAngle - dotIL) / fallOff);
    return spotTerm * DiffusePointLight(surfaceWorldPos, surfaceNormal, surfaceColor, light);
}

float Attenuate(Light light, float3 surfaceWorldPos)
{
    float dist = distance(light.Position, surfaceWorldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}





