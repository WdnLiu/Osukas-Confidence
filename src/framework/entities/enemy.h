#pragma once
#include "framework/entities/entityCollider.h"
#include "framework/entities/bullet/patterns.h"
#define HITBOX_RAD 0.2
#define PLAYER_HEIGHT 0.6

class Bullet;

class Enemy : public EntityCollider {

public:
	Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic, int HP);

	void render(Camera* camera);
	void update(float time_elapsed);

	bool touching_ground = false;
	bool colliding = false;
	bool grounded;
	float ground_below_y = 10000;
	float ground_y = 10000;
	Vector3 ground_normal = Vector3(0.0f);
	Vector3 direction = Vector3(0.0f);

	BulletNormal bullets_normal;

	int maxHP;
	int currHP;
	float m_spd;
	float v_spd;

	bool moving;
	float startMoving;
	float startFiring;
	float bulletCD = -1;

	bool looking_at_player = true;

	Mesh* hitbox_mesh = Mesh::Get("data/meshes/sphere.obj");
	Shader* flat_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");

	enum pattern {
		SWIRL,
		SHOTGUN,
		HORIZONTAL
	};

	std::vector<Bullet*> bullets;
	Shader* bullet_shaders[4]; Texture* bullet_textures[4]; Mesh* bullet_meshes[4];
	pattern current_pattern = SWIRL;
	bool canshoot = true;
	uint16 amount[4] = {6, 10, 20, 1 };

	Vector3 getPosition() const { return model.getTranslation(); };
	Vector3 getPositionGround() const { Vector3 res = getPosition(); res.y = 0; return res; }
	Vector3 getFront() { return model.frontVector(); }
	Vector3 getRight() { return model.rightVector(); }

	void randomPattern();

	void showHitbox(Camera* camera);
	void sphere_bullet_collision(Vector3 position, float radius);

	void loadTextures() {
		bullets_normal.isInstanced = true;
		bullets_normal.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_normal.material.diffuse = Texture::Get("data/meshes/bullet.mtl");
		bullets_normal.fromPlayer = false;
		bullets_normal.mesh = Mesh::Get("data/meshes/bullet.obj");

		Shader* s = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		bullet_shaders[0] = s;
		Texture* t1 = Texture::Get("data/meshes/bullet.mtl");
		bullet_textures[0] = t1;
		Mesh* m1 = Mesh::Get("data/meshes/bullet.obj");
		bullet_meshes[0] = m1;
	}
};

