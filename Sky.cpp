#include "Sky.h"


Sky::Sky(std::shared_ptr<Mesh> meshIn, 
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back, 
	const wchar_t* vertexPath, const wchar_t* pixelPath)
{
	// -- SETTING MESH --
	skyMesh = meshIn;

	// -- LOADING TEXTURE -- 
	cubeMapSRV = CreateCubemap(right, left, up, down, front, back);

	// -- CREATING SAMPLER STATE -- 
	D3D11_SAMPLER_DESC ssDesc = {};
	ssDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	ssDesc.MaxAnisotropy = 12;
	ssDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Using the address of the sampler state and the description, create the sampler state
	Graphics::Device->CreateSamplerState(&ssDesc, samplerState.GetAddressOf());

	// -- LOAD SHADERS --
	LoadVertexShader(vertexPath);
	LoadPixelShader(pixelPath);

	// -- RASTERIZER STATE --
	D3D11_RASTERIZER_DESC rDesc = {};
	rDesc.CullMode = D3D11_CULL_FRONT;
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.DepthClipEnable = true;

	Graphics::Device->CreateRasterizerState(&rDesc, rasterizerState.GetAddressOf());

	// -- DEPTH STENCIL STATE --
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	Graphics::Device->CreateDepthStencilState(&depthDesc, depthStencilState.GetAddressOf());



}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back) 
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here! +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};
	CreateWICTextureFromFile(Graphics::Device.Get(), right, (ID3D11Resource**)textures[0].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), left, (ID3D11Resource**)textures[1].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), up, (ID3D11Resource**)textures[2].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), down, (ID3D11Resource**)textures[3].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), front, (ID3D11Resource**)textures[4].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), back, (ID3D11Resource**)textures[5].GetAddressOf(), 0);
	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first texture
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);
	// Describe the resource for the cube map, which is simply
	// a "texture 2d array" with the TEXTURECUBE flag set.
	// This is a special GPU resource format, NOT just a
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width; // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	Graphics::Device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());
	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0, // Which mip (zero, since there's only one)
			i, // Which array element?
			1); // How many mip levels are in the texture?
		// Copy from one resource (texture) to another
		Graphics::Context->CopySubresourceRegion(
			cubeMapTexture.Get(), // Destination resource
			subresource, // Dest subresource index (one of the array elements)
			0, 0, 0, // XYZ location of copy
			textures[i].Get(), // Source resource
			0, // Source subresource index (we're assuming there's only one)
			0); // Source subresource "box" of data to copy (zero means the whole thing)
	}
	// At this point, all of the faces have been copied into the
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1; // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see
	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Graphics::Device->CreateShaderResourceView(
		cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());
	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}


void Sky::LoadPixelShader(const wchar_t* path)
{
	ID3DBlob* pixelShaderBlob;

	D3DReadFileToBlob(path, &pixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		0,
		skyPixelShader.GetAddressOf());

}

void Sky::LoadVertexShader(const wchar_t* path)
{
	ID3DBlob* skyVertexBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
	D3DReadFileToBlob(path, &skyVertexBlob);

	Graphics::Device->CreateVertexShader(
		skyVertexBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		skyVertexBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		skyVertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer

	// Create an input layout - INPUT LAYOUTS ARE ONLY REQUIRED FOR THE VERTEX SHADER!!!!
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[4] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a UV or Texture coordinate, which is 2 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;			// 2x 32-bit floats
		inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Set up the third element - a Normal, which is 3 more float values
		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;			// 3x 32-bit floats
		inputElements[2].SemanticName = "NORMAL";							// Match our vertex shader input!
		inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Set up the fourth element - the tangent for normal mapping - 3 more floats
		inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[3].SemanticName = "TANGENT";							// Match our vertex shader input!
		inputElements[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			4,										// How many elements in that array?
			skyVertexBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			skyVertexBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			skyVertexInputLayout.GetAddressOf());		// Address of the resulting ID3D11InputLayout pointer
	}
}

void Sky::Draw(std::shared_ptr<Camera> camera) 
{
	
	Graphics::Context->RSSetState(rasterizerState.Get());
	Graphics::Context->OMSetDepthStencilState(depthStencilState.Get(), 0);

	Graphics::Context->VSSetShader(skyVertexShader.Get(), 0, 0);
	Graphics::Context->PSSetShader(skyPixelShader.Get(), 0, 0);

	Graphics::Context->PSSetShaderResources(0, 1, cubeMapSRV.GetAddressOf());
	Graphics::Context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	

	ExtraSkyVertexData vsData;
	vsData.view = camera->GetView();
	vsData.proj = camera->GetProj();
	D3D11_MAPPED_SUBRESOURCE vertexMappedBuffer = {};

	Graphics::FillAndBindNextConstantBuffer(&vsData, sizeof(vsData), D3D11_VERTEX_SHADER, 0);

	skyMesh->Draw();

	Graphics::Context->RSSetState(0);
	Graphics::Context->OMSetDepthStencilState(0, 0);
}