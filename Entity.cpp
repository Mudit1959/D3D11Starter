#include "Entity.h"
Entity::Entity(std::shared_ptr<Mesh> m) 
{
	mesh = m;
}

Transform* Entity::GetTransform() 
{
	return &transform;
}

void Entity::Draw() 
{
	mesh->Draw();
}