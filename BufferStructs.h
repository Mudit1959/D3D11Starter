#pragma once
#include <DirectXMath.h>

struct ExtraVertexData 
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
};

struct ExtraPixelData 
{
	DirectX::XMFLOAT4 colourTint;
	float totalTime;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 offset;
};
