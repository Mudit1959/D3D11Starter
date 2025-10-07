#include "Material.h"


/// <summary>
/// Create a material using a color tint, a pointer to a C++ vertex shader object , and a pointer to a C++ pixel shader object
/// </summary>
/// <param name="t">4 component tint</param>
/// <param name="vs">C++ vertex shader object pointer</param>
/// <param name="ps">C++ pixel shader object pointer</param>
Material::Material(DirectX::XMFLOAT4 t, Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps) 
{
	tint = t;
	vertexShader = vs;
	pixelShader = ps;
}


Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVS() { return vertexShader; }
Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPS() { return pixelShader; }
DirectX::XMFLOAT4 Material::GetTint() { return tint; }

void Material::SetTint(DirectX::XMFLOAT4 t) { tint = t; }
void Material::SetVS(Microsoft::WRL::ComPtr<ID3D11VertexShader> vs) { vertexShader = vs; }
void Material::SetPS(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps) { pixelShader = ps; }