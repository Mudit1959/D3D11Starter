#include "Entity.h"
Entity::Entity(std::shared_ptr<Mesh> meshIn, std::shared_ptr<Material> materialIn) 
{
	mesh = meshIn;
	material = materialIn;
}

Transform* Entity::GetTransform() 
{
	return &transform;
}

void Entity::Draw() 
{
	Graphics::Context->VSSetShader(material->GetVS().Get(), 0, 0);
	Graphics::Context->PSSetShader(material->GetPS().Get(), 0, 0);
	mesh->Draw();
}

int Entity::GetMeshIndexCount() 
{
	return mesh->GetIndexCount();
}

DirectX::XMFLOAT4 Entity::GetTint() { return material->GetTint(); }
DirectX::XMFLOAT2 Entity::GetScale() { return material->GetScale(); }
DirectX::XMFLOAT2 Entity::GetOffset() { return material->GetOffset(); }
float Entity::GetRoughness() { return material->GetRoughness(); }