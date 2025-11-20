struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition	: POSITION; // XYZ position
    float2 uv				: TEXCOORD; // UV Coordinates
    float3 normal			: NORMAL; // NORMAL
    float3 tangent			: TANGENT; // TANGENT FOR NORMAL MAPPING
};

struct VertexToPixel
{
    float4 position		: SV_POSITION;
    float3 sampleDir    : DIRECTION;
};

//Buffer for external data
cbuffer ExternalVertexData : register(b0)
{
    float4x4 view : VIEWMATRIX;
    float4x4 proj : PROJECTIONMATRIX;
};


VertexToPixel main( VertexShaderInput input )
{
    VertexToPixel output;
    
    float4x4 viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    output.position = mul(mul(proj, viewNoTranslation), float4(input.localPosition, 1.0f));
    output.sampleDir = input.localPosition;
    output.position.z = output.position.w;
    
	return output;
}