#pragma once

#include "framework/entities/entityMesh.h"
#include "framework/animation.h"
#include "graphics/material.h"
#include "framework/audio.h"

enum eButtonID {
	UndefinedButton,
	PlayButton,
	OptionsButton,
	ExitButton,
	SensBar,
	KeyWalk,
	KeyLeft,
	KeyBack,
	KeyRight,
	KeyJump, 
	KeyDash,
	KeyShoot,
	KeyAuto,
	KeyS1,
	KeyS2,
	KeyS3
	// ...
};

class EntityUI : public EntityMesh {

public:
	// Attributes of the derived class  
	bool visible = true;

	bool posteffects = true;

	eButtonID button_id;

	Vector2 size;
	Vector2 position;

	Vector4 default_color = Vector4::WHITE;
	Vector4 selected_color = Vector4(0.7, 0.7, 0.7, 1);

	bool is3d = false;
	Vector3 pos3d;

	float mask = 1.0f;

	EntityUI() { };
	EntityUI(Vector2 pos, Vector2 size, const Material& material, float height, eButtonID button_id = UndefinedButton) {
		this->position = pos;
		this->size = size;
		this->material = material;
		this->button_id = button_id;
		Mesh* quad = new Mesh();
		quad->createQuad(pos.x, height - pos.y, size.x, size.y, false);
		this->mesh = quad;
		this->button_id = button_id;
		if (button_id != UndefinedButton) {
			Audio::Get("data/audio/menu/select.wav");
		}
	};

	EntityUI(Vector2 pos, Vector2 size, const Material& material, float height, Vector4 default_col, Vector4 selected_col, eButtonID button_id = UndefinedButton) {
		this->default_color = default_col;
		this->selected_color = selected_col;
		this->position = pos;
		this->size = size;
		this->material = material;
		this->button_id = button_id;
		Mesh* quad = new Mesh();
		quad->createQuad(pos.x, height - pos.y, size.x, size.y, false);
		this->mesh = quad;
		this->button_id = button_id;
		if (button_id != UndefinedButton) {
			Audio::Get("data/audio/menu/select.wav");
		}
	};
	EntityUI(Vector2 size, const Material& material ) {};
	~EntityUI() {}
	
	Vector3 getPosition() const { return model.getTranslation(); };
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void update3D(Vector3 position3d);
	void addLOD(sMeshLOD mesh_lod);
};

