#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
class Entity
{
public:
	Entity(std::shared_ptr<Mesh> m);
	Transform* GetTransform();
	void Draw();

private:
	std::shared_ptr<Mesh> mesh;
	Transform transform;
};

