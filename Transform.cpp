#include "Transform.h"

Transform::Transform()
{
	SetPosition(0.0f, 0.0f, 0.0f);
	SetScale(1.0f, 1.0f, 1.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX initialMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&world, initialMatrix);
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(initialMatrix)));
	edited = 0;
}

void Transform::SetPosition(float x, float y, float z) { position = DirectX::XMFLOAT3(x, y, z); edited++; };
void Transform::SetPosition(DirectX::XMFLOAT3 pos) { position = pos; edited ++;}
void Transform::SetRotation(float pitch, float yaw, float roll) { rotation = DirectX::XMFLOAT3(pitch, yaw, roll); edited++;}
void Transform::SetRotation(DirectX::XMFLOAT3 ro) { rotation = ro; edited ++;} // XMFLOAT4 for quaternion
void Transform::SetScale(float x, float y, float z) { scale = DirectX::XMFLOAT3(x, y, z); edited ++;}
void Transform::SetScale(DirectX::XMFLOAT3 s) { scale = s; edited ++;}

//Getters
DirectX::XMFLOAT3 Transform::GetPosition() { return position; }
DirectX::XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; } // XMFLOAT4 GetRotation() for quaternion
DirectX::XMFLOAT3 Transform::GetScale() { return scale; }


//Movements - Simplified by performing Math and Load within the Store function
void Transform::MoveAbsolute(float x, float y, float z) 
{
	DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), DirectX::XMVectorSet(x, y, z, 1.0f)));
	edited++;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), DirectX::XMVectorSet(offset.x, offset.y, offset.z, 1.0f)));
	edited++;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rotation), DirectX::XMVectorSet(pitch, yaw, roll, 1.0f)));
	edited++;
}

void Transform::Rotate(DirectX::XMFLOAT3 ro)
{
	DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rotation), DirectX::XMVectorSet(ro.x, ro.y, ro.z, 1.0f)));
	edited++;
}

// Scale needs to be multiplied!
void Transform::Scale(float x, float y, float z)
{
	DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&scale), DirectX::XMVectorSet(x, y, z, 1.0f)));
	edited++;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&scale), DirectX::XMVectorSet(scale.x, scale.y, scale.z, 1.0f)));
	edited++;
}

// Checks if any edits have been made using a counter. If there are edits it will recalculate, otherwise, it will return the float4x4 as is. 
DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() 
{ 
	if (edited == 0) { return world; }
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationRollPitchYaw(rotation.z, rotation.x, rotation.y) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&world, worldMatrix);
	DirectX::XMStoreFloat4x4(&worldInverseT,
		DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(worldMatrix)));
	edited = 0;
	return world;
}
DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (edited == 0) { return worldInverseT; }
	GetWorldMatrix();
	edited = 0;
	return worldInverseT;
}