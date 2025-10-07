#pragma once
#include <d3d11.h>
#include<wrl/client.h>
#include "Vertex.h"
#include "Graphics.h"
#include <DirectXMath.h>

class Material
{

private:
	DirectX::XMFLOAT4 tint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;


public:
	Material(DirectX::XMFLOAT4 t, Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps); // pointer to first index of tint
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVS();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPS();
	DirectX::XMFLOAT4 GetTint();
	void SetTint(DirectX::XMFLOAT4 t);
	void SetVS(Microsoft::WRL::ComPtr<ID3D11VertexShader> vs);
	void SetPS(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps);
};

