#include "Material.h"


/// <summary>
/// Create a material using a color tint, a pointer to a C++ vertex shader object , and a pointer to a C++ pixel shader object
/// </summary>
/// <param name="t"> - 4 component tint</param>
/// <param name="metal"> - Is it a metal? </param>
/// <param name="vs"> - C++ vertex shader object pointer</param>
/// <param name="ps"> - C++ pixel shader object pointer</param>
Material::Material(DirectX::XMFLOAT4 t, int metal, Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps) 
{
	tint = t;
	IsMetal = metal;
	scale = DirectX::XMFLOAT2(1, 1);
	offset = DirectX::XMFLOAT2(0, 0);
	vertexShader = vs;
	pixelShader = ps;
	textureSRVCount = 0;
	samplerCount = 0;
}

void Material::AddTextureSRV(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource) 
{
	textureSRVs[index] = resource; 
	textureSRVCount++;
}

void Material::AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler) 
{
	samplers[index] = sampler; 
	samplerCount++;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVS() { return vertexShader; }
Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPS() { return pixelShader; }
int Material::GetTextureSRVCount() { return textureSRVCount; }
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV(int s) 
{
	return textureSRVs[s];
}
DirectX::XMFLOAT4 Material::GetTint() { return tint; }
DirectX::XMFLOAT2 Material::GetScale() { return scale; }
DirectX::XMFLOAT2 Material::GetOffset() { return offset; }
int Material::GetIsMetal() { return IsMetal; }

void Material::SetTint(DirectX::XMFLOAT4 t) { tint = t; }
void Material::SetVS(Microsoft::WRL::ComPtr<ID3D11VertexShader> vs) { vertexShader = vs; }
void Material::SetPS(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps) { pixelShader = ps; }
void Material::SetScale(DirectX::XMFLOAT2 s) { scale = s; }
void Material::SetOffset(DirectX::XMFLOAT2 o) { offset = o; }

void Material::BindTexturesSamplers() 
{
	for (int i = 0; i < textureSRVCount; i++) 
	{
		Graphics::Context->PSSetShaderResources(i,1,textureSRVs[i].GetAddressOf());
	}

	for (int i = 0; i < samplerCount; i++) 
	{
		Graphics::Context->PSSetSamplers(i, 1, samplers[i].GetAddressOf());
	}
}