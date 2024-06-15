#pragma once

#include "framework/entities/EntityMesh.h"
#include "framework/animation.h"
#include "graphics/material.h"


struct sParticle {
	int id;
	Vector3 position;
	Vector3 velocity;
	float ttl;
	float rotation;

	bool active = false;
};

class ParticleEmitter : EntityMesh {

	int max_particles = 300;
	int active_particles = 0;
	// Particle container
	std::vector<sParticle> particles;

	// Properties of the emitter
	int last_id = 0;
	float emit_rate = 1.0f;
	float emit_timer = 0.f;
	
	Vector3 emit_position;
	Vector3 emit_velocity;
	float random_factor = 0.0f;

	// Properties of the particles
	float max_ttl = 1;
	std::vector<Vector4> colors = { Vector4(1) };
	std::vector<float> sizes;
	std::vector<float> alphas;
	Texture* texture = nullptr;
	int texture_grid_size = 1;
	bool additive_blending = false;
	bool animated_texture = false;
	bool sort_distance = true; // Si queremos ordenar por distancia (z buffer moment)

	bool emission_enabled = true;

	template<typename T>
	T sample(float time, int n, T* data) {
		float time_in_table = time * (n - 1);
		float entry;
		float amount = modf(time_in_table, &entry);
		return data[(int)entry] * (1 - amount) + data[(int)entry + 1] * amount;
	}

	void emit();

public:
	ParticleEmitter() {
		this->emit_position = Vector3(0,10,0);
		this->emit_velocity = Vector3(0,1,0);

		//this->sizes = {
		//	0, 1, 0.7, 0.5, 0.35, 0.25, 0.17, 0.12, 0.08, 0.05, 0.035, 0.02, 0.01, 0
		//};
		//this->alphas = {
		//	0, 1, 0.7, 0.5, 0.35, 0.25, 0.17, 0.12, 0.08, 0.05, 0.035, 0.02, 0.01, 0
		//};

		this->sizes = {
			1, 1
		};
		this->alphas = {
			1, 1
		};

		this->texture = Texture::Get("data/textures/particle.png");
		material.diffuse = Texture::Get("data/textures/particle.png");

		particles.resize(max_particles);
		
	}
	ParticleEmitter(Vector3 emit_position, Vector3 emit_velocity, Texture* texture, float maxttl = 1) {
		this->emit_position = emit_position;
		this->emit_velocity = emit_velocity;

		this->sizes = {
			0, 1, 0.7, 0.5, 0.35, 0.25, 0.17, 0.12, 0.08, 0.05, 0.035, 0.02, 0.01, 0
		};
		this->alphas = {
			0, 1, 0.7, 0.5, 0.35, 0.25, 0.17, 0.12, 0.08, 0.05, 0.035, 0.02, 0.01, 0
		};

		this->texture = texture;

		particles.resize(max_particles);
	}


	void clearParticles();

	void render(Camera* camera);
	void update(float seconds_elapsed);
};


