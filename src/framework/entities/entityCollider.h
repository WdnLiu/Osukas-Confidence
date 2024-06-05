#pragma once

#include "framework/entities/entityMesh.h"

#define HITBOX_RAD 0.2
#define PLAYER_HEIGHT 0.6

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};

class EntityCollider : public EntityMesh {

public:
	bool is_dynamic = false;
	EntityCollider() {
		type = NONE;
	};
	EntityCollider(Mesh* mesh, const Material& material, COL_TYPE type = SCENARIO, const std::string& name = "", bool is_dynamic = false) {
		this->type = type;
		this->mesh = mesh;
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->is_dynamic = is_dynamic;
	};
	~EntityCollider() {};
};