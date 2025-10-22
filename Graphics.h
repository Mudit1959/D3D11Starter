#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11shadertracing.h>
#include <string>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace Graphics
{
	// --- GLOBAL VARS ---

	// Primary D3D11 API objects
	inline Microsoft::WRL::ComPtr<ID3D11Device> Device;
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
	inline Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;

	// -- Ring Buffer Implementation -- 
	// The D3D11.1 version of the Context object
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context1;
	// One very large buffer to replace all smaller constant buffers
	inline Microsoft::WRL::ComPtr<ID3D11Buffer> ConstBufferHeap;
	// Size of the large ring buffer
	inline unsigned int cbSizeBytes;
	// Offset in Bytes
	inline unsigned int cbOffsetBytes;

	// Rendering buffers
	inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRTV;
	inline Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthBufferDSV;

	// Debug Layer
	inline Microsoft::WRL::ComPtr<ID3D11InfoQueue> InfoQueue;

	// --- FUNCTIONS ---

	// Getters
	bool VsyncState();
	std::wstring APIName();

	// General functions
	HRESULT Initialize(unsigned int windowWidth, unsigned int windowHeight, HWND windowHandle, bool vsyncIfPossible);
	void ShutDown();
	void ResizeBuffers(unsigned int width, unsigned int height);
	void FillAndBindNextConstantBuffer(void* data, unsigned int dataSizeInBytes, D3D11_SHADER_TYPE shaderType, unsigned int registerSlot);

	// Debug Layer
	void PrintDebugMessages();
}