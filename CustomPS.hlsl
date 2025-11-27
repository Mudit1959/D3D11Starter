#include "ShaderInclude.hlsli"



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
	
	// u - r | v - g
	// UV values range from 0-1
	// To scale the x-axis to 25 - multiply u by 25 - new limits are 0-25
	// To map the v values from -1 to 1 - multiply v by 2 then subtract 1 - new limits are -1-1
	
	
    float sinVal = sin(input.uv.r*totalTime); // the multiplying factor with r controls the frequency/wavelength in an inverse manner
    float cosVal = cos((input.uv.r*totalTime - PI)  * 10);
    float yVal = -1 * ((input.uv.g * 6) - 3); 
	// the multiplying factor with g controls the amplitude in an inverse manner | the subtracted value controls whether the wave is centered around the sphere
	
    float sinResultVal = 1 - (abs(yVal - sinVal)*8); 
    float cosResultVal = 1 - (abs(yVal - cosVal) * 6);
	// abs() ensures both the positive y and negative y axes are taken into account | 
	// the multiplying factor controls the sharpness of the wave 
	
    return float4(sinResultVal, cosResultVal, 0.0f, 1.0f);

}

