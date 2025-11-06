#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"


class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void Initialize();
	void OnResize();
	void SetExternalData(float totalTime, DirectX::XMFLOAT4 tint, float roughness, DirectX::XMFLOAT3 worldPos, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 offset, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj, Entity e);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	// void LoadShaders();
	void LoadVertexShader(std::wstring path);
	void LoadPixelShader(std::wstring path);
	void CreateGeometry();
	void UpdateImGui(float deltaTime);
	void RefreshUI();

	std::shared_ptr<Camera> camera, secondCamera;
	std::shared_ptr<Mesh> cubeMesh, sphereMesh, cylinderMesh, quadMesh, quadDoubleMesh, helixMesh, torusMesh;

	bool showDemo;
	Light lights[5];

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	//Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> psConstBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexInputLayout;
};

