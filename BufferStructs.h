#pragma once
#include <DirectXMath.h>

struct ExtraVertexData 
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInv;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	
};

struct ExtraPixelData 
{
	DirectX::XMFLOAT4 colourTint;
	
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 offset;
	
	DirectX::XMFLOAT3 worldPos;
	float totalTime;

	float roughness;
	DirectX::XMFLOAT3 ambientColor; 

	Light lights[5]; // Array of exactly 5 lights
};
