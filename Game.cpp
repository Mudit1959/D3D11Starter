#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Mesh.h"
#include "BufferStructs.h"

#include <DirectXMath.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

float guiTint[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float guiXOffset, guiYOffset, guiZOffset;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();
	Initialize(); //Initialize ImGui

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::Initialize() 
{
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();
	Game::showDemo = false;
}

/// <summary
/// Updates ImGui and within it, refreshes the information displayed. 
/// </summary>
/// <param name="deltaTime"></param>
void Game::UpdateImGui(float deltaTime) 
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	Game::RefreshUI();
}
	

void Game::RefreshUI() 
{
	
	//Create the window
	ImGui::Begin("Hello there!");

	// Feature 1 - Tree Node
	if (ImGui::TreeNode("Details")) {
		ImGui::Text("Frame Rate: %f fps", ImGui::GetIO().Framerate);

		ImGui::Text("Window Client Size: %ix%i", Window::Width(), Window::Height());

		ImGui::ColorEdit4("RGBA color editor", color);

		// Feature 2 - Drag and change
		const float drag_speed = 0.4f;
		static bool drag_clamp = false;
		static char v = 0; static char zero = 0; static char fifty = 50;
		ImGui::DragScalar("Drag to change!", ImGuiDataType_S8, &v, drag_speed, drag_clamp ? &zero : NULL, drag_clamp ? &fifty : NULL);

		// Feature 3 - Progress Bar
		static float progress = 0.0f, progress_dir = 1.0f;
		progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
		if (progress >= 1.1f) { progress = 1.1f; progress_dir = -1.0f; }
		if (progress <= -0.1f) { progress = -0.1f; progress_dir = 1.0f; }
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Sisyphus");

		ImGui::TreePop(); // Important for tree node! Collapsing Header does not require this!!!
	}

	if (ImGui::TreeNode("Meshes")) 
	{
		if (ImGui::TreeNode("Triangle"))
		{
			ImGui::Text("Triangles: %i", triangleMesh->GetIndexCount()/3);
			ImGui::Text("Vertices: %i", triangleMesh->GetVertexCount());
			ImGui::Text("Indices: %i", triangleMesh->GetIndexCount());

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Octagon"))
		{
			ImGui::Text("Triangles: %i", octMesh->GetIndexCount() / 3);
			ImGui::Text("Vertices: %i", octMesh->GetVertexCount());
			ImGui::Text("Indices: %i", octMesh->GetIndexCount());

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Rectangle"))
		{
			ImGui::Text("Triangles: %i", rectMesh->GetIndexCount() / 3);
			ImGui::Text("Vertices: %i", rectMesh->GetVertexCount());
			ImGui::Text("Indices: %i",  rectMesh->GetIndexCount());

			ImGui::TreePop();
		}

		ImGui::TreePop();

		
	}

	if (ImGui::TreeNode("Tint & Offset"))
	{
		float max = 1.0f; float min = -1.0f;
		ImGui::ColorEdit4("Tint RGBA editor", guiTint);
		ImGui::SliderScalar("X Offset", ImGuiDataType_Float, &guiXOffset, &min, &max, "%f");
		ImGui::SliderScalar("Y Offset", ImGuiDataType_Float, &guiYOffset, &min, &max, "%f");
		ImGui::SliderScalar("Z Offset", ImGuiDataType_Float, &guiZOffset, &min, &max, "%f");
		ImGui::TreePop();
	}
	

	const char* visibility = "Hide ImGui Demo Window";

	if (!Game::showDemo)
	{
		visibility = "Show ImGui Demo Window";
	}

	if (ImGui::Button(visibility))
	{
		Game::showDemo = !Game::showDemo;
	}

	if (Game::showDemo) { ImGui::ShowDemoWindow(); }

	//Destroy the window
	ImGui::End();
	
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer

		// Create the buffer to feed external data to the GPU

		// 1. Define the buffer description
		unsigned int eDataSize = ((sizeof(ExtraVertexData) + 15) / 16) * 16;
		D3D11_BUFFER_DESC eb = {};
		eb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		eb.ByteWidth = eDataSize;
		eb.Usage = D3D11_USAGE_DYNAMIC;
		eb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		eb.MiscFlags = 0;
		eb.StructureByteStride = 0;

		//2. Create the buffer
		Graphics::Device->CreateBuffer(&eb, 0, vsConstBuffer.GetAddressOf());


	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex triangleMeshVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int triangleMeshIndices[] = { 0, 1, 2 };

	Vertex rectMeshVertices[] =
	{
		{XMFLOAT3(-0.7f, +0.7f, +0.0f), red},
		{XMFLOAT3(-0.3f, +0.7f, +0.0f), blue},
		{XMFLOAT3(-0.3f, +0.5f, +0.0f), blue},
		{XMFLOAT3(-0.7f, +0.5f, +0.0f), red},
	};
	unsigned int rectMeshIndices[] = { 0, 1, 2, 3, 0, 2 };

	Vertex octMeshVertices[] =
	{
		{XMFLOAT3(+0.7f-0.05f, -0.7f, +0.0f), black},
		{XMFLOAT3(+0.6f-0.05f, -0.5f, +0.0f), black},
		{XMFLOAT3(+0.6f-0.05f, -0.3f, +0.0f), black},
		{XMFLOAT3(+0.7f-0.05f, -0.1f, +0.0f), black},
		{XMFLOAT3(+0.9f-0.05f, -0.1f, +0.0f), black},
		{XMFLOAT3(+1.0f-0.05f, -0.3f, +0.0f), black},
		{XMFLOAT3(+1.0f-0.05f, -0.5f, +0.0f), black},
		{XMFLOAT3(+0.9f-0.05f, -0.7f, +0.0f), black}
	};

	unsigned int octMeshIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6, 
		0, 6, 7,
	};

	triangleMesh = std::make_shared<Mesh>(&triangleMeshVertices[0], 3, &triangleMeshIndices[0], 3);
	rectMesh = std::make_shared<Mesh>(&rectMeshVertices[0], 4, &rectMeshIndices[0], 6);
	octMesh = std::make_shared<Mesh>(&octMeshVertices[0], 8, &octMeshIndices[0], 18);
	
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	Game::UpdateImGui(deltaTime);
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	Game::color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		// Bind the constant buffer
		Graphics::Context->VSSetConstantBuffers(0, 1, vsConstBuffer.GetAddressOf());
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		ExtraVertexData vsData;
		vsData.colourTint = XMFLOAT4(guiTint);
		vsData.offset = XMFLOAT3(guiXOffset, guiYOffset, guiZOffset);
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

		Graphics::Context->Map(vsConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));

		Graphics::Context->Unmap(vsConstBuffer.Get(), 0);

		triangleMesh->Draw();


		Graphics::Context->Map(vsConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));

		Graphics::Context->Unmap(vsConstBuffer.Get(), 0);
		rectMesh->Draw();


		Graphics::Context->Map(vsConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));

		Graphics::Context->Unmap(vsConstBuffer.Get(), 0);
		octMesh->Draw();
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



