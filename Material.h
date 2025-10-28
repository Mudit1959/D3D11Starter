#pragma once
#include <d3d11.h>
#include<wrl/client.h>
#include "Vertex.h"
#include "Graphics.h"
#include <DirectXMath.h>

class Material
{

private:
	// Shaders, and characteristics
	DirectX::XMFLOAT4 tint;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 offset;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	
	// Texture resource and Sampler States
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRVs[128];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[16];
	unsigned int textureSRVCount, samplerCount;

public:
	Material(DirectX::XMFLOAT4 t, Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps); // pointer to first index of tint
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVS();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPS();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV(int s);
	int GetTextureSRVCount();
	DirectX::XMFLOAT4 GetTint();
	DirectX::XMFLOAT2 GetScale();
	DirectX::XMFLOAT2 GetOffset();
	void SetTint(DirectX::XMFLOAT4 t);
	void SetScale(DirectX::XMFLOAT2 s);
	void SetOffset(DirectX::XMFLOAT2 o);
	void SetVS(Microsoft::WRL::ComPtr<ID3D11VertexShader> vs);
	void SetPS(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps);

	void AddTextureSRV(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource);
	void AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void BindTexturesSamplers();

};

