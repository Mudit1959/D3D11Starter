#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Camera.h"
#include "Material.h"

#include <DirectXMath.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <vector>

// For the DirectX Math library
using namespace DirectX;

float guiTint[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float entity_1_prs[3][9] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
							0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
							0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

std::vector<Entity> entityList;
std::vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders;
std::vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixelShaders;
std::vector<std::shared_ptr<Material>> materials;


int cameraChoice = 0;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();
	Initialize(); //Initialize ImGui
	camera = std::make_shared<Camera>(10.0f, 0.0f, -30.0f, Window::AspectRatio());
	secondCamera = std::make_shared<Camera>(0.0f, 0.0f, -10.0f, Window::AspectRatio());
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
		Graphics::Context->IASetInputLayout(vertexInputLayout.Get());
		
		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		//Graphics::Context->VSSetShader(vertexShaders[0].Get(), 0, 0);
		//Graphics::Context->PSSetShader(pixelShaders[0].Get(), 0, 0);
		
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
		/*
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
		*/
		
	}

	if (ImGui::TreeNode("Entitities")) 
	{
		if (ImGui::TreeNode("Entity 1")) 
		{
			ImGui::DragFloat3("Position", &entity_1_prs[0][0], 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Rotation", &entity_1_prs[0][3], 0.01f, 0, 2.0f*3.14);
			ImGui::DragFloat3("Scale", &entity_1_prs[0][6], 0.01f, 0.01f, 10.0f);
			ImGui::Text("Indices: %i", entityList[2].GetMeshIndexCount());
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Entity 2"))
		{
			ImGui::DragFloat3("Position", &entity_1_prs[1][0], 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Rotation", &entity_1_prs[1][3], 0.01f, 0, 2.0f * 3.14);
			ImGui::DragFloat3("Scale", &entity_1_prs[1][6], 0.01f, 0.01f, 10.0f);
			ImGui::Text("Indices: %i", entityList[3].GetMeshIndexCount());
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Entity 3"))
		{
			ImGui::DragFloat3("Position", &entity_1_prs[2][0], 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Rotation", &entity_1_prs[2][3], 0.01f, 0, 2.0f * 3.14);
			ImGui::DragFloat3("Scale", &entity_1_prs[2][6], 0.01f, 0.01f, 10.0f);
			ImGui::Text("Indices: %i", entityList[4].GetMeshIndexCount());
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Tint & Offset"))
	{
		float max = 1.0f; float min = -1.0f;
		ImGui::ColorEdit4("Tint RGBA editor", guiTint);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Cameras"))
	{
		if (ImGui::Button("Camera 1"))
			cameraChoice = 0;
		if (cameraChoice==0)
		{
			ImGui::SameLine();
			ImGui::Text("Active");
		}

		if (ImGui::Button("Camera 2"))
			cameraChoice = 1;
		if (cameraChoice==1)
		{
			ImGui::SameLine();
			ImGui::Text("Active");
		}

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

void Game::LoadVertexShader(std::wstring path) 
{
	vertexShaders.push_back(Microsoft::WRL::ComPtr<ID3D11VertexShader>());
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(path.c_str(), &vertexShaderBlob);

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShaders[vertexShaders.size() - 1].GetAddressOf());			// The address of the ID3D11VertexShader pointer

	// Create an input layout - INPUT LAYOUTS ARE ONLY REQUIRED FOR THE VERTEX SHADER!!!!
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
		{
			D3D11_INPUT_ELEMENT_DESC inputElements[3] = {};

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

			// Create the input layout, verifying our description against actual shader code
			Graphics::Device->CreateInputLayout(
				inputElements,							// An array of descriptions
				3,										// How many elements in that array?
				vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
				vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
				vertexInputLayout.GetAddressOf());		// Address of the resulting ID3D11InputLayout pointer
		}
}

void Game::LoadPixelShader(std::wstring path) 
{
	pixelShaders.push_back(Microsoft::WRL::ComPtr<ID3D11PixelShader>());
	ID3DBlob* pixelShaderBlob;

	D3DReadFileToBlob(path.c_str(), &pixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		0,
		pixelShaders[pixelShaders.size() - 1].GetAddressOf());

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
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Load shaders, create materials
	{
		LoadVertexShader(FixPath(L"VertexShader.cso"));

		LoadPixelShader(FixPath(L"PixelShader.cso"));
		LoadPixelShader(FixPath(L"DebugUVs.cso"));
		LoadPixelShader(FixPath(L"DebugNormals.cso"));
		LoadPixelShader(FixPath(L"CustomPS.cso"));

		// Shaders are loaded

		materials.push_back(std::make_shared<Material>(white, vertexShaders[0], pixelShaders[0])); // 0 - white
		materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(0.676f, 0.43f, 0.43f, 1.0f), vertexShaders[0], pixelShaders[0])); // 1 - dull orange
		materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(0.013f, 0.392f, 0.125f, 1.0f), vertexShaders[0], pixelShaders[0])); // 2 - green
		materials.push_back(std::make_shared<Material>(red, vertexShaders[0], pixelShaders[0])); // 3 - red
		materials.push_back(std::make_shared<Material>(blue, vertexShaders[0], pixelShaders[0])); // 4 - blue
		materials.push_back(std::make_shared<Material>(black, vertexShaders[0], pixelShaders[0])); // 5 - black

		materials.push_back(std::make_shared<Material>(white, vertexShaders[0], pixelShaders[1])); // 6 - UV material
		materials.push_back(std::make_shared<Material>(white, vertexShaders[0], pixelShaders[2])); // 7 - Normals material
		materials.push_back(std::make_shared<Material>(white, vertexShaders[0], pixelShaders[3])); // 8 - Custom material

	}


	// Create the buffers to feed external data to the GPU
	{
		// 1. Define the constant buffer description - eData(external Data), eb(external data buffer)
		unsigned int eDataSize = ((sizeof(ExtraVertexData) + 15) / 16) * 16;
		D3D11_BUFFER_DESC eb = {};
		eb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		eb.ByteWidth = eDataSize;
		eb.Usage = D3D11_USAGE_DYNAMIC;
		eb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		eb.MiscFlags = 0;
		eb.StructureByteStride = 0;

		unsigned int pDataSize = ((sizeof(ExtraPixelData) + 15) / 16) * 16;
		D3D11_BUFFER_DESC p = {};
		p.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		p.ByteWidth = pDataSize;
		p.Usage = D3D11_USAGE_DYNAMIC;
		p.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		p.MiscFlags = 0;
		p.StructureByteStride = 0;

		//2. Create the constant buffer
		Graphics::Device->CreateBuffer(&eb, 0, vsConstBuffer.GetAddressOf());
		Graphics::Device->CreateBuffer(&p, 0, psConstBuffer.GetAddressOf());
	}

	

	cubeMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.ggp_obj").c_str());
	cylinderMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.ggp_obj").c_str());
	helixMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.ggp_obj").c_str());
	quadMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad.ggp_obj").c_str());
	quadDoubleMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad_double_sided.ggp_obj").c_str());
	sphereMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.ggp_obj").c_str());
	torusMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/torus.ggp_obj").c_str());


	// Now to create entities using the meshes
	

	entityList.push_back(Entity(cubeMesh, materials[7]));
	entityList.push_back(Entity(cylinderMesh, materials[7]));
	entityList.push_back(Entity(helixMesh, materials[7]));
	entityList.push_back(Entity(quadMesh, materials[7]));
	entityList.push_back(Entity(quadDoubleMesh, materials[7]));
	entityList.push_back(Entity(sphereMesh, materials[7]));
	entityList.push_back(Entity(torusMesh, materials[7]));

	entityList.push_back(Entity(cubeMesh,		materials[6]));
	entityList.push_back(Entity(cylinderMesh,	materials[6]));
	entityList.push_back(Entity(helixMesh,		materials[6]));
	entityList.push_back(Entity(quadMesh,		materials[6]));
	entityList.push_back(Entity(quadDoubleMesh, materials[6]));
	entityList.push_back(Entity(sphereMesh,		materials[6]));
	entityList.push_back(Entity(torusMesh,		materials[6]));

	

	entityList.push_back(Entity(cubeMesh, materials[0]));
	entityList.push_back(Entity(cylinderMesh, materials[1]));
	entityList.push_back(Entity(helixMesh, materials[2]));
	entityList.push_back(Entity(quadMesh, materials[3]));
	entityList.push_back(Entity(quadDoubleMesh, materials[4]));
	entityList.push_back(Entity(sphereMesh, materials[8]));
	entityList.push_back(Entity(torusMesh, materials[3]));

	for (unsigned int i = 0; i < 3; i++) 
	{
		for (unsigned int j = 0; j < 7; j++) 
		{
			entityList[(i * 7) + j].GetTransform()->MoveAbsolute((4.0f*j), (-4.0f * i), 0.0f);
		}
	}

}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	if (camera != NULL) 
	{
		camera->UpdateProjMatrix(Window::AspectRatio());
		secondCamera->UpdateProjMatrix(Window::AspectRatio());
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	Game::UpdateImGui(deltaTime);
	if (cameraChoice == 0) { camera->Update(deltaTime); }
	else { secondCamera->Update(deltaTime); }

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
		Graphics::Context->PSSetConstantBuffers(0, 1, psConstBuffer.GetAddressOf());
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{

		
		float offset = sin(totalTime)/2;
		for (int i = 0; i < 21; i++) 
		{
			entityList[i].GetTransform()->Rotate(0.0f, deltaTime, 0.0f);
		}
		
		for (int i = 0; i < 7; i++)
		{
			if (cameraChoice == 0) { SetExternalData(totalTime, entityList[i].GetTint(), camera->GetView(), camera->GetProj(), entityList[i]); }
			else { SetExternalData(totalTime, entityList[ i].GetTint(), secondCamera->GetView(), secondCamera->GetProj(), entityList[i]); }
			entityList[i].Draw();
		}

		
		for (int i = 0; i < 7; i++)
		{
			if (cameraChoice == 0) { SetExternalData(totalTime, entityList[7 + i].GetTint(), camera->GetView(), camera->GetProj(), entityList[7 + i]); }
			else { SetExternalData(totalTime, entityList[7 + i].GetTint(), secondCamera->GetView(), secondCamera->GetProj(), entityList[7 + i]); }
			entityList[7 + i].Draw();
		}


		
		for (int i = 0; i < 7; i++) 
		{
			if (cameraChoice == 0) { SetExternalData(totalTime, entityList[14 + i].GetTint(), camera->GetView(), camera->GetProj(), entityList[14 + i]); }
			else { SetExternalData(totalTime, entityList[14 + i].GetTint(), secondCamera->GetView(), secondCamera->GetProj(), entityList[14 + i]); }
			entityList[14 + i].Draw();
		}
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

void Game::SetExternalData(float totalTime, DirectX::XMFLOAT4 tint , DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj, Entity e)
{
	ExtraVertexData vsData;
	vsData.world = e.GetTransform()->GetWorldMatrix();
	vsData.view = view;
	vsData.proj = proj;
	D3D11_MAPPED_SUBRESOURCE vertexMappedBuffer = {};

	ExtraPixelData psData;
	psData.colourTint = tint;
	psData.totalTime = totalTime;
	D3D11_MAPPED_SUBRESOURCE pixelMappedBuffer = {};

	Graphics::Context->Map(vsConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexMappedBuffer);
	memcpy(vertexMappedBuffer.pData, &vsData, sizeof(vsData));
	Graphics::Context->Unmap(vsConstBuffer.Get(), 0);

	Graphics::Context->Map(psConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pixelMappedBuffer);
	memcpy(pixelMappedBuffer.pData, &psData, sizeof(psData));
	Graphics::Context->Unmap(psConstBuffer.Get(), 0);

}


