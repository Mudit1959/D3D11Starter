#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include <WICTextureLoader.h>
#include "PathHelpers.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include "BufferStructs.h"
#include "Camera.h"

class Sky
{
public:

	Sky(std::shared_ptr<Mesh> meshIn, const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back, const wchar_t* vertexPath, const wchar_t* pixelPath);

	// Chris Cascioli - Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	void LoadPixelShader(const wchar_t* path);
	void LoadVertexShader(const wchar_t* path);

	void Draw(std::shared_ptr<Camera> camera);



private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> skyVertexInputLayout;
	std::shared_ptr<Mesh> skyMesh;
};

