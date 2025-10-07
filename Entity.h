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

private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	Transform transform;
};

