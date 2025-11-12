#pragma once
#include <DirectXMath.h>
#define LIGHT_TYPE_DIRECTIONAL_MATTE 0
#define LIGHT_TYPE_DIRECTIONAL_SPECULAR 1
#define LIGHT_TYPE_POINT_MATTE 2
#define LIGHT_TYPE_POINT_SPECULAR 3
#define LIGHT_TYPE_SPOT_MATTE 4
#define LIGHT_TYPE_SPOT_SPECULAR 5

struct Light
{
	int Type; // Which kind of light? 0, 1 or 2 (see above)
	DirectX::XMFLOAT3 Direction; // Directional and Spot lights need a direction

	float Range; // Point and Spot lights have a max range for attenuation
	DirectX::XMFLOAT3 Position; // Point and Spot lights have a position in space

	float Intensity; // All lights need an intensity
	DirectX::XMFLOAT3 Color; // All lights need a color

	float SpotInnerAngle; // Inner cone angle (in radians) – Inside this, full light!
	float SpotOuterAngle; // Outer cone angle (radians) – Outside this, no light!
	DirectX::XMFLOAT2 Padding;// Purposefully padding to hit the 16-byte boundary
};
