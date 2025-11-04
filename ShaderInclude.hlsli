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
};

// External Data passed at Pixel Shader level
cbuffer ExternalPixelData : register(b0)
{
    float4 colourTint : TINT;
    
    float2 scale : SCALE;
    float2 offset : OFFSET;
    
    
    float3 camWorldPos : CAMERA_WORLD_POS;
    float totalTime : TIME;
    
    float roughness : ROUGHNESS;
    float3 ambient : AMBIENT;
};

//Random value
float random(float2 s)
{
    return frac(sin(dot(s, float2(12.9898, 78.233))) * 43758.5453123);
}

