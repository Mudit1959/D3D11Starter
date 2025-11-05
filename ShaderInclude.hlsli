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
#define LIGHT_TYPE_POINT 2
#define LIGHT_TYPE_SPOT 3

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
    float4 colourTint : TINT;
    
    float2 scale : SCALE;
    float2 offset : OFFSET;
    
    
    float3 camWorldPos : CAMERA_WORLD_POS;
    float totalTime : TIME;
    
    float roughness : ROUGHNESS;
    float3 ambientColor : AMBIENT_COLOR;
    
    Light light;
};

float3 diffuseTerm(float3 lightColor, float lightIntensity, float3 surfaceColor, float3 surfaceNormal, float3 lightDir)
{
    return saturate(dot(surfaceNormal, -lightDir)) * lightColor * surfaceColor * lightIntensity;
}


