#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Graphics.h"
class Entity
{
public:
	Entity(std::shared_ptr<Mesh> meshIn, std::shared_ptr<Material> materialIn);
	Transform* GetTransform();
	void Draw();
	int GetMeshIndexCount();
	DirectX::XMFLOAT4 GetTint();
	DirectX::XMFLOAT2 GetScale();
	DirectX::XMFLOAT2 GetOffset();
	float GetRoughness();

private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	Transform transform;
};

