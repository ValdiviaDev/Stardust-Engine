#ifndef __ComponentTransform__H__
#define __ComponentTransform__H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ComponentTransform : public Component //TODO
{
public:
	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	void SetPosition(math::float3 pos);
	math::float3 GetPosition();
	void SetRotation(math::float3 rot);
	math::float3 GetRotation();
	void SetScale(math::float3 scale);
	math::float3 GetScale();
	math::float4x4 GetGlobalMatrix() const;
	

	void UpdateMatrix();

private:
	math::float4x4 local_matrix = math::float4x4::identity;
	math::float4x4 global_matrix = math::float4x4::identity;

	math::float3 position = math::float3::zero;
	math::float3 rotation = math::float3::zero;
	math::Quat quaternion_rot = math::Quat::identity;
	math::float3 scale = math::float3::one;
	


	
};

#endif