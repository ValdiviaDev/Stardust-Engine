#ifndef __ComponentTransform__H__
#define __ComponentTransform__H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class ComponentTransform : public Component //TODO
{
public:
	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	void Save(JSON_Array* comp_array);
	void Load(JSON_Object* comp_obj);

	void DrawInspector();
	void HandleGizmos();

	void SetPosition(math::float3 pos);
	math::float3 GetPosition() const;
	void SetRotation(math::float3 rot);
	void SetRotationFromQuat();
	math::float3 GetRotation() const;
	void SetScale(math::float3 scale);
	math::float3 GetScale() const;
	math::float4x4 GetGlobalMatrix() const;
	
	void SumPosition(math::float3 pos);
	void SumRotation(math::float3 rot);
	void SumScale(math::float3 scale);
	void Reset();

	math::float3 GetGlobalPos() const;
	math::float3 GetGlobalScale() const;

	void UpdateMatrix();

private:
	math::float4x4 local_matrix = math::float4x4::identity;
	math::float4x4 global_matrix = math::float4x4::identity;

	math::float4x4 last_global_matrix = math::float4x4::identity;

	math::float3 position = math::float3::zero;
	math::float3 rotation = math::float3::zero;
	math::Quat quaternion_rot = math::Quat::identity;
	math::float3 scale = math::float3::one;
	
};

#endif